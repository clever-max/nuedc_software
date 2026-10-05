/*
 * 原始 MPU6050 陀螺仪驱动。
 *
 * I2C 事务在每次读写前等待总线空闲并检查 STOP/NACK，使用寄存器指针
 * 写入后再启动读传输；启动时完成静止零偏和一阶滤波。
 * 本工程固定使用 MPU6050 原始 I2C 协议。
 * 接线：PB2=SCL、PB3=SDA、PB1=INT；AD0 悬空按 0x68 访问。
 */
#include "mpu6050.h"

#include "ti_msp_dl_config.h"

#include <stdint.h>

#define MPU6050_ADDRESS              (0x68U)
#define MPU6050_REG_SMPLRT_DIV       (0x19U)
#define MPU6050_REG_CONFIG           (0x1AU)
#define MPU6050_REG_GYRO_CONFIG      (0x1BU)
#define MPU6050_REG_PWR_MGMT_1       (0x6BU)
#define MPU6050_REG_PWR_MGMT_2       (0x6CU)
#define MPU6050_REG_WHO_AM_I         (0x75U)
#define MPU6050_REG_GYRO_ZOUT_H      (0x47U)
#define MPU6050_GYRO_LSB_PER_DPS     (131.0f) /* ±250 dps */
#define MPU6050_I2C_TIMEOUT           (1000000U)
#define MPU6050_CALIBRATION_SAMPLES   (100U)
#define MPU6050_FILTER_ALPHA          (0.35f)
#define MPU6050_GYRO_DEADZONE_DPS     (0.20f)

static bool s_ready;
static bool s_probe_failed;
static bool s_filter_ready;
static float s_gyro_z_bias;
static float s_filtered_rate_deg_s;
static float s_yaw_deg;
static float s_yaw_rate_deg_s;

static bool i2cWaitIdle(void)
{
    uint32_t timeout = MPU6050_I2C_TIMEOUT;
    while ((DL_I2C_getControllerStatus(MPU6050_I2C_INST) &
            DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) {
        if (timeout-- == 0U) return false;
    }
    return true;
}

static bool i2cWaitStopOrNack(void)
{
    uint32_t timeout = MPU6050_I2C_TIMEOUT;
    uint32_t status;

    do {
        status = DL_I2C_getRawInterruptStatus(MPU6050_I2C_INST,
            DL_I2C_INTERRUPT_CONTROLLER_STOP |
            DL_I2C_INTERRUPT_CONTROLLER_NACK);
        if (timeout-- == 0U) return false;
    } while (status == 0U);

    DL_I2C_clearInterruptStatus(MPU6050_I2C_INST,
        DL_I2C_INTERRUPT_CONTROLLER_STOP |
        DL_I2C_INTERRUPT_CONTROLLER_NACK);
    return (status & DL_I2C_INTERRUPT_CONTROLLER_NACK) == 0U;
}

static void i2cClearTransferStatus(void)
{
    DL_I2C_clearInterruptStatus(MPU6050_I2C_INST,
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_STOP |
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);
}

static bool writeRegisters(uint8_t reg, const uint8_t *data, uint32_t length)
{
    uint8_t packet[16];
    uint32_t total = length + 1U;
    uint32_t i;

    if (total > sizeof(packet)) return false;
    packet[0] = reg;
    for (i = 0U; i < length; ++i) packet[i + 1U] = data[i];
    if (!i2cWaitIdle()) return false;

    i2cClearTransferStatus();
    DL_I2C_flushControllerTXFIFO(MPU6050_I2C_INST);
    if (DL_I2C_fillControllerTXFIFO(MPU6050_I2C_INST, packet, total) != total)
        return false;
    DL_I2C_startControllerTransfer(MPU6050_I2C_INST, MPU6050_ADDRESS,
        DL_I2C_CONTROLLER_DIRECTION_TX, total);
    return i2cWaitStopOrNack() && i2cWaitIdle();
}

static bool readRegisters(uint8_t reg, uint8_t *data, uint8_t length)
{
    uint8_t i;

    if (data == 0 || length == 0U) return false;
    if (!i2cWaitIdle()) return false;

    /* 先发送寄存器地址，STOP 后再启动读，行为与实测仓库一致。 */
    i2cClearTransferStatus();
    DL_I2C_flushControllerTXFIFO(MPU6050_I2C_INST);
    if (DL_I2C_fillControllerTXFIFO(MPU6050_I2C_INST, &reg, 1U) != 1U)
        return false;
    DL_I2C_startControllerTransfer(MPU6050_I2C_INST, MPU6050_ADDRESS,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1U);
    if (!i2cWaitStopOrNack() || !i2cWaitIdle()) return false;

    i2cClearTransferStatus();
    DL_I2C_flushControllerRXFIFO(MPU6050_I2C_INST);
    DL_I2C_startControllerTransfer(MPU6050_I2C_INST, MPU6050_ADDRESS,
        DL_I2C_CONTROLLER_DIRECTION_RX, length);
    for (i = 0U; i < length; ++i) {
        uint32_t timeout = MPU6050_I2C_TIMEOUT;
        while (DL_I2C_isControllerRXFIFOEmpty(MPU6050_I2C_INST)) {
            if (timeout-- == 0U) return false;
        }
        data[i] = DL_I2C_receiveControllerData(MPU6050_I2C_INST);
    }
    return i2cWaitStopOrNack();
}

static bool writeByte(uint8_t reg, uint8_t value)
{
    return writeRegisters(reg, &value, 1U);
}

static bool readGyroRate(float *rate_deg_s)
{
    uint8_t data[2];
    int16_t raw;

    if (!readRegisters(MPU6050_REG_GYRO_ZOUT_H, data, sizeof(data)))
        return false;
    raw = (int16_t)(((uint16_t)data[0] << 8) | data[1]);
    *rate_deg_s = (float)raw / MPU6050_GYRO_LSB_PER_DPS;
    return true;
}

bool BspMpu6050_Init(void)
{
    uint8_t who = 0U;
    float sum = 0.0f;
    float rate_deg_s;
    uint32_t i;

    s_ready = false;
    s_probe_failed = false;
    s_filter_ready = false;
    s_gyro_z_bias = 0.0f;
    s_filtered_rate_deg_s = 0.0f;
    s_yaw_deg = 0.0f;
    s_yaw_rate_deg_s = 0.0f;

    DL_I2C_resetControllerTransfer(MPU6050_I2C_INST);
    if (!readRegisters(MPU6050_REG_WHO_AM_I, &who, 1U) ||
        (who != 0x68U && who != 0x69U && who != 0x70U)) {
        /* 0x68/0x69/0x70 都是同一 MPU6050 兼容器件的 ID 变体。 */
        s_probe_failed = true;
        return false;
    }

    /* 复位后使用 X 轴陀螺仪作为时钟源，配置 200 Hz 采样和 ±250 dps。 */
    if (!writeByte(MPU6050_REG_PWR_MGMT_1, 0x80U)) {
        s_probe_failed = true;
        return false;
    }
    delay_cycles(CPUCLK_FREQ / 100U);
    if (!writeByte(MPU6050_REG_PWR_MGMT_1, 0x01U) ||
        !writeByte(MPU6050_REG_PWR_MGMT_2, 0x00U) ||
        !writeByte(MPU6050_REG_CONFIG, 0x03U) ||
        !writeByte(MPU6050_REG_SMPLRT_DIV, 0x04U) ||
        !writeByte(MPU6050_REG_GYRO_CONFIG, 0x00U)) {
        s_probe_failed = true;
        return false;
    }

    /* 车辆静止时采集零偏，约 0.5 s；完成后才允许 B21 启动。 */
    for (i = 0U; i < MPU6050_CALIBRATION_SAMPLES; ++i) {
        if (!readGyroRate(&rate_deg_s)) {
            s_probe_failed = true;
            return false;
        }
        sum += rate_deg_s;
        delay_cycles(CPUCLK_FREQ / 200U);
    }
    s_gyro_z_bias = sum / (float)MPU6050_CALIBRATION_SAMPLES;
    s_ready = true;
    return true;
}

bool BspMpu6050_Update(float dt_s)
{
    float rate_deg_s;

    if (!s_ready || dt_s <= 0.0f || !readGyroRate(&rate_deg_s)) {
        s_ready = false;
        return false;
    }
    rate_deg_s -= s_gyro_z_bias;
    if (rate_deg_s > -MPU6050_GYRO_DEADZONE_DPS &&
        rate_deg_s < MPU6050_GYRO_DEADZONE_DPS) rate_deg_s = 0.0f;
    if (!s_filter_ready) {
        s_filtered_rate_deg_s = rate_deg_s;
        s_filter_ready = true;
    } else {
        s_filtered_rate_deg_s = MPU6050_FILTER_ALPHA * rate_deg_s +
            (1.0f - MPU6050_FILTER_ALPHA) * s_filtered_rate_deg_s;
    }
    s_yaw_rate_deg_s = s_filtered_rate_deg_s;
    s_yaw_deg += s_yaw_rate_deg_s * dt_s;
    return true;
}

void BspMpu6050_ZeroYaw(void)
{
    s_yaw_deg = 0.0f;
}

bool BspMpu6050_IsReady(void) { return s_ready; }
const char *BspMpu6050_GetBackendName(void)
{
    return s_probe_failed ? "MPU6050-ERR" : "MPU6050";
}
float BspMpu6050_GetYawDeg(void) { return s_yaw_deg; }
float BspMpu6050_GetYawRateDegS(void) { return s_yaw_rate_deg_s; }
