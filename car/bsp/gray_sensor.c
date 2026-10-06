#include "gray_sensor.h"

#include "ti_msp_dl_config.h"

#include <stdint.h>

/*
 * NCHD12 使用 PCA9555 兼容寄存器返回 12 路输入。样例工程的 PA0/PA1
 * 已分配给电机 PWM，所以这里用 PA28/PA31 软件模拟一条独立 I2C 总线。
 */
/* The NCHD12 sample uses a PCA9555-compatible 12-bit input image.  PA0/PA1
 * are occupied by motor PWM in this project, so the reserved PA28/PA31 pair
 * is used for a software I2C bus. */
#define GRAY_SCL_PIN             (GRAY_SENSOR_BUS_GRAY_SCL_PIN)
#define GRAY_SDA_PIN             (GRAY_SENSOR_BUS_GRAY_SDA_PIN)
#define GRAY_SCL_IOMUX           (GRAY_SENSOR_BUS_GRAY_SCL_IOMUX)
#define GRAY_SDA_IOMUX           (GRAY_SENSOR_BUS_GRAY_SDA_IOMUX)
#define GRAY_PORT                (GRAY_SENSOR_BUS_PORT)
#define GRAY_WRITE_ADDRESS       (0x40U)
#define GRAY_READ_ADDRESS        (0x41U)
#define GRAY_INPUT_REGISTER      (0x00U)
#define GRAY_DELAY_CYCLES        (CPUCLK_FREQ / 200000U)

static float s_last_position;
static uint16_t s_last_bits;

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

static float positionFromBits(uint16_t bits)
{
    /* 通道 0 到 11 映射为 -11 到 +11，多个连续亮点取平均位置。 */
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
    sdaRelease();
    sclOutput(true);
    i2cStop();
}

bool BspGraySensor_Read(GraySensorSample *sample)
{
    uint8_t low;
    uint8_t high;
    uint16_t bits;
    bool valid;
    if (sample == 0) return false;

    /* PCA9555 输入寄存器 0x00、0x01 连读，低 12 位对应有效通道。 */
    i2cStart();
    if (!writeByte(GRAY_WRITE_ADDRESS) || !writeByte(GRAY_INPUT_REGISTER)) {
        i2cStop();
        sample->bits = s_last_bits;
        sample->position = s_last_position;
        sample->valid = false;
        return false;
    }
    i2cStart();
    if (!writeByte(GRAY_READ_ADDRESS)) {
        i2cStop();
        sample->bits = s_last_bits;
        sample->position = s_last_position;
        sample->valid = false;
        return false;
    }
    low = readByte(true);
    high = readByte(false);
    i2cStop();

    bits = (uint16_t)(((uint16_t)high << 8) | low) & 0x0FFFU;
    valid = bits != 0U;
    if (valid) {
        s_last_bits = bits;
        s_last_position = positionFromBits(bits);
    }
    sample->bits = bits;
    sample->position = s_last_position;
    sample->valid = valid;
    return true;
}
