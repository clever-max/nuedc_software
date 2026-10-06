#include "gray_sensor.h"

#include "ti_msp_dl_config.h"

#include <stdint.h>

/*
 * NCHD12 使用 PCA9555 兼容寄存器返回 12 路输入。样例工程的 PA0/PA1
 * 已分配给电机 PWM，所以这里用 PA29/PA30 软件模拟一条独立 I2C 总线。
 */
/* The NCHD12 sample uses a PCA9555-compatible 12-bit input image.  PA0/PA1
 * are occupied by motor PWM in this project, so the reserved PA29/PA30 pair
 * is used for a software I2C bus. */
#define GRAY_SCL_PIN             (GRAY_SENSOR_BUS_GRAY_SCL_PIN)
#define GRAY_SDA_PIN             (GRAY_SENSOR_BUS_GRAY_SDA_PIN)
#define GRAY_SCL_IOMUX           (GRAY_SENSOR_BUS_GRAY_SCL_IOMUX)
#define GRAY_SDA_IOMUX           (GRAY_SENSOR_BUS_GRAY_SDA_IOMUX)
#define GRAY_PORT                (GRAY_SENSOR_BUS_PORT)
#define GRAY_DEFAULT_WRITE_ADDRESS (0x40U)
#define GRAY_INPUT_REGISTER      (0x00U)
#define GRAY_DELAY_CYCLES        (CPUCLK_FREQ / 200000U)
#define GRAY_ADDRESS_COUNT       (8U)
#define GRAY_ADDRESS_STEP        (2U)

static float s_last_position;
static uint16_t s_last_bits;
static uint8_t s_write_address;
static uint8_t s_bus_stage;
static bool s_bus_ready;

static void delayI2c(void)
{
    delay_cycles(GRAY_DELAY_CYCLES == 0U ? 1U : GRAY_DELAY_CYCLES);
}

static void sdaOutputLow(void)
{
    /* I2C 采用开漏等效方式：低电平主动拉低，高电平释放为输入。 */
    DL_GPIO_initDigitalOutput(GRAY_SDA_IOMUX);
    DL_GPIO_clearPins(GRAY_PORT, GRAY_SDA_PIN);
    DL_GPIO_enableOutput(GRAY_PORT, GRAY_SDA_PIN);
}

static void sdaRelease(void)
{
    DL_GPIO_disableOutput(GRAY_PORT, GRAY_SDA_PIN);
    DL_GPIO_initDigitalInputFeatures(GRAY_SDA_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

static void sclOutput(bool high)
{
    DL_GPIO_initDigitalOutput(GRAY_SCL_IOMUX);
    if (high) DL_GPIO_setPins(GRAY_PORT, GRAY_SCL_PIN);
    else DL_GPIO_clearPins(GRAY_PORT, GRAY_SCL_PIN);
    DL_GPIO_enableOutput(GRAY_PORT, GRAY_SCL_PIN);
}

static bool sdaIsHigh(void)
{
    return DL_GPIO_readPins(GRAY_PORT, GRAY_SDA_PIN) != 0U;
}

static void i2cStart(void)
{
    /* 起始条件：SCL 为高时 SDA 从高变低。 */
    sdaRelease();
    sclOutput(true);
    delayI2c();
    sdaOutputLow();
    delayI2c();
    sclOutput(false);
}

static void i2cStop(void)
{
    /* 停止条件：SCL 为高时 SDA 从低变高。 */
    sdaOutputLow();
    delayI2c();
    sclOutput(true);
    delayI2c();
    sdaRelease();
    delayI2c();
}

static bool waitAck(void)
{
    bool ack;
    sdaRelease();
    delayI2c();
    sclOutput(true);
    delayI2c();
    ack = !sdaIsHigh();
    sclOutput(false);
    delayI2c();
    return ack;
}

static bool writeByte(uint8_t value)
{
    uint8_t bit;
    for (bit = 0U; bit < 8U; ++bit) {
        if ((value & 0x80U) != 0U) sdaRelease();
        else sdaOutputLow();
        delayI2c();
        sclOutput(true);
        delayI2c();
        sclOutput(false);
        value <<= 1;
    }
    return waitAck();
}

static uint8_t readByte(bool acknowledge)
{
    uint8_t bit;
    uint8_t value = 0U;
    sdaRelease();
    for (bit = 0U; bit < 8U; ++bit) {
        value <<= 1;
        sclOutput(true);
        delayI2c();
        if (sdaIsHigh()) value |= 1U;
        sclOutput(false);
        delayI2c();
    }
    if (acknowledge) sdaOutputLow();
    else sdaRelease();
    delayI2c();
    sclOutput(true);
    delayI2c();
    sclOutput(false);
    sdaRelease();
    return value;
}

static void fillSample(GraySensorSample *sample, bool valid,
    uint16_t bits, float position)
{
    sample->bits = bits;
    sample->position = position;
    sample->valid = valid;
    /* 只有完整读事务走到数据阶段，才把总线报告为 OK；地址探测成功
     * 不能代替后续寄存器读事务的 ACK。 */
    sample->bus_ok = s_bus_stage == 4U;
    sample->bus_stage = s_bus_stage;
    sample->write_address = s_write_address;
}

static bool probeAddress(uint8_t write_address)
{
    bool acknowledged;
    i2cStart();
    acknowledged = writeByte(write_address);
    i2cStop();
    return acknowledged;
}

static bool findDeviceAddress(void)
{
    uint8_t index;
    /* PCA9555 的 A0/A1/A2 形成 0x20~0x27，按 8 位写地址扫描 0x40~0x4E。 */
    for (index = 0U; index < GRAY_ADDRESS_COUNT; ++index) {
        uint8_t candidate = (uint8_t)(GRAY_DEFAULT_WRITE_ADDRESS +
            index * GRAY_ADDRESS_STEP);
        if (probeAddress(candidate)) {
            s_write_address = candidate;
            return true;
        }
    }
    s_write_address = GRAY_DEFAULT_WRITE_ADDRESS;
    return false;
}

static float positionFromBits(uint16_t bits)
{
    /*
     * NCHD1 手册规定 bit0 从阵列最右侧开始，bit11 逐步向最左侧排列。
     * 这里让右侧为负、左侧为正，保持参考循迹算法的转向约定。
     */
    int32_t sum = 0;
    uint32_t count = 0U;
    uint8_t index;
    for (index = 0U; index < 12U; ++index) {
        if ((bits & (uint16_t)(1U << index)) != 0U) {
            sum += (int32_t)index * 2 - 11;
            ++count;
        }
    }
    if (count == 0U) return s_last_position;
    return (float)sum / (float)count;
}

void BspGraySensor_Init(void)
{
    s_last_bits = 0U;
    s_last_position = 0.0f;
    s_write_address = GRAY_DEFAULT_WRITE_ADDRESS;
    s_bus_stage = 0U;
    s_bus_ready = false;
    sdaRelease();
    sclOutput(true);
    i2cStop();
    /* 与供应商例程的 i2c_CheckDevice(0x40) 对齐，同时兼容 A0~A2 改址。 */
    s_bus_ready = findDeviceAddress();
    s_bus_stage = s_bus_ready ? 0U : 1U;
}

bool BspGraySensor_Read(GraySensorSample *sample)
{
    uint8_t low;
    uint8_t high;
    uint16_t bits;
    bool valid;
    if (sample == 0) return false;

    /* 复位后若初始化时没有找到器件，不在每个 5 ms 周期重复扫描。 */
    if (!s_bus_ready) {
        s_bus_stage = 1U;
        fillSample(sample, false, s_last_bits, s_last_position);
        return false;
    }

    /* PCA9555 输入寄存器 0x00、0x01 连读，低 12 位对应有效通道。 */
    i2cStart();
    if (!writeByte(s_write_address)) {
        s_bus_stage = 1U;
        i2cStop();
        fillSample(sample, false, s_last_bits, s_last_position);
        return false;
    }
    if (!writeByte(GRAY_INPUT_REGISTER)) {
        s_bus_stage = 2U;
        i2cStop();
        fillSample(sample, false, s_last_bits, s_last_position);
        return false;
    }
    i2cStart();
    if (!writeByte((uint8_t)(s_write_address | 0x01U))) {
        s_bus_stage = 3U;
        i2cStop();
        fillSample(sample, false, s_last_bits, s_last_position);
        return false;
    }
    s_bus_stage = 4U;
    low = readByte(true);
    high = readByte(false);
    i2cStop();

    bits = (uint16_t)(((uint16_t)high << 8) | low) & 0x0FFFU;
    valid = bits != 0U;
    if (valid) {
        s_last_bits = bits;
        s_last_position = positionFromBits(bits);
    }
    fillSample(sample, valid, bits, s_last_position);
    return true;
}
