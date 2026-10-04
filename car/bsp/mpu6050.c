/* 保留的 JY61S/MPU I2C 兼容驱动：部分板卡释放原始 MPU6050 总线，部分
 * WIT 变体使用 0x50 高级寄存器协议。当前灰度 PID Demo 不调用本模块，
 * 但文件保留给后续陀螺仪闭环版本使用。
 *
 * JY61S-family compatibility: some boards release a raw MPU6050 bus in IIC
 * mode (0x68/0x69), while WIT high-level variants expose 16-bit registers at
 * 0x50.  The driver probes both forms and keeps the route-facing API the same. */
#include "mpu6050.h"

#include "ti_msp_dl_config.h"

#include <stdint.h>

#define MPU6050_ADDRESS_LOW          (0x68U)
#define MPU6050_ADDRESS_HIGH         (0x69U)
#define WITMOTION_ADDRESS            (0x50U)
#define MPU6050_REG_SMPLRT_DIV       (0x19U)
#define MPU6050_REG_CONFIG           (0x1AU)
#define MPU6050_REG_GYRO_CONFIG      (0x1BU)
#define MPU6050_REG_PWR_MGMT_1       (0x6BU)
#define MPU6050_REG_WHO_AM_I         (0x75U)
#define MPU6050_REG_GYRO_ZOUT_H      (0x47U)
#define WITMOTION_REG_GZ             (0x39U)
#define MPU6050_GYRO_LSB_PER_DPS     (131.0f)
#define WITMOTION_GYRO_DPS_PER_LSB   (2000.0f / 32768.0f)
#define MPU6050_I2C_TIMEOUT          (100000U)
#define MPU6050_CALIBRATION_SAMPLES  (100U)

static bool s_ready;
static uint8_t s_i2c_address;
typedef enum {
    SENSOR_BACKEND_NONE = 0,
    SENSOR_BACKEND_RAW_MPU6050,
    SENSOR_BACKEND_WIT_REGISTERS
} SensorBackend;
static SensorBackend s_backend;
static bool s_probe_failed;
static float s_gyro_z_bias;
static float s_yaw_deg;
static float s_yaw_rate_deg_s;

static bool waitIdle(void)
{
    uint32_t timeout = MPU6050_I2C_TIMEOUT;
    while ((DL_I2C_getControllerStatus(MPU6050_I2C_INST) &
            DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) {
        if (timeout-- == 0U) return false;
    }
    return (DL_I2C_getControllerStatus(MPU6050_I2C_INST) &
            DL_I2C_CONTROLLER_STATUS_ERROR) == 0U;
}

static bool writeRegisters(uint8_t reg, const uint8_t *data, uint32_t length)
{
    uint8_t packet[16];
    uint32_t total = length + 1U;
    uint32_t i;
    if (total > sizeof(packet)) return false;
    packet[0] = reg;
    for (i = 0U; i < length; ++i) packet[i + 1U] = data[i];
    if (!waitIdle()) return false;
    DL_I2C_flushControllerTXFIFO(MPU6050_I2C_INST);
    if (DL_I2C_fillControllerTXFIFO(MPU6050_I2C_INST, packet, total) != total)
        return false;
    DL_I2C_startControllerTransfer(MPU6050_I2C_INST, s_i2c_address,
        DL_I2C_CONTROLLER_DIRECTION_TX, total);
    return waitIdle();
}

static bool readRegisters(uint8_t reg, uint8_t *data, uint32_t length)
{
    uint32_t count = 0U;
    uint32_t timeout = MPU6050_I2C_TIMEOUT;
    if (length == 0U) return true;
    if (!writeRegisters(reg, 0, 0U)) return false;
    if (!waitIdle()) return false;
    DL_I2C_flushControllerRXFIFO(MPU6050_I2C_INST);
    DL_I2C_startControllerTransfer(MPU6050_I2C_INST, s_i2c_address,
        DL_I2C_CONTROLLER_DIRECTION_RX, length);
    while (timeout-- != 0U) {
        while (!DL_I2C_isControllerRXFIFOEmpty(MPU6050_I2C_INST) &&
            count < length) {
            data[count++] = DL_I2C_receiveControllerData(MPU6050_I2C_INST);
        }
        if ((DL_I2C_getControllerStatus(MPU6050_I2C_INST) &
                DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) return false;
        if (count == length &&
            (DL_I2C_getControllerStatus(MPU6050_I2C_INST) &
                DL_I2C_CONTROLLER_STATUS_IDLE) != 0U) return true;
    }
    return false;
}

static bool writeByte(uint8_t reg, uint8_t value)
{
    return writeRegisters(reg, &value, 1U);
}

static bool readGyroRate(float *rate_deg_s)
{
    uint8_t data[2];
    int16_t raw;
    uint8_t register_address;
    if (s_backend == SENSOR_BACKEND_RAW_MPU6050) {
        register_address = MPU6050_REG_GYRO_ZOUT_H;
    } else if (s_backend == SENSOR_BACKEND_WIT_REGISTERS) {
        register_address = WITMOTION_REG_GZ;
    } else {
        return false;
    }
    if (!readRegisters(register_address, data, 2U)) return false;
    if (s_backend == SENSOR_BACKEND_RAW_MPU6050) {
        raw = (int16_t)(((uint16_t)data[0] << 8) | data[1]);
        *rate_deg_s = (float)raw / MPU6050_GYRO_LSB_PER_DPS;
    } else {
        raw = (int16_t)(((uint16_t)data[1] << 8) | data[0]);
        *rate_deg_s = (float)raw * WITMOTION_GYRO_DPS_PER_LSB;
    }
    return true;
}

bool BspMpu6050_Init(void)
{
    uint8_t who = 0U;
    uint8_t wit_data[2];
    float sum = 0.0f;
    uint32_t i;
    float rate_deg_s;
    bool raw_mpu_found = false;

    s_ready = false;
    s_backend = SENSOR_BACKEND_NONE;
    s_probe_failed = false;
    s_i2c_address = MPU6050_ADDRESS_LOW;
    s_gyro_z_bias = 0.0f;
    s_yaw_deg = 0.0f;
    s_yaw_rate_deg_s = 0.0f;
    DL_I2C_resetControllerTransfer(MPU6050_I2C_INST);
    if (readRegisters(MPU6050_REG_WHO_AM_I, &who, 1U) &&
        (who == 0x68U || who == 0x69U)) {
        raw_mpu_found = true;
    } else {
        s_i2c_address = MPU6050_ADDRESS_HIGH;
        DL_I2C_resetControllerTransfer(MPU6050_I2C_INST);
        if (readRegisters(MPU6050_REG_WHO_AM_I, &who, 1U) &&
            (who == 0x68U || who == 0x69U)) {
            raw_mpu_found = true;
        }
    }

    if (raw_mpu_found) {
        s_backend = SENSOR_BACKEND_RAW_MPU6050;
        if (!writeByte(MPU6050_REG_PWR_MGMT_1, 0x00U)) goto init_failed;
        if (!writeByte(MPU6050_REG_SMPLRT_DIV, 0x04U)) goto init_failed;
        if (!writeByte(MPU6050_REG_CONFIG, 0x03U)) goto init_failed;
        if (!writeByte(MPU6050_REG_GYRO_CONFIG, 0x00U)) goto init_failed;
    } else {
        /* WIT high-level IIC mode: 0x39 is GZ, little-endian, scaled as
         * signed 16-bit full-scale ±2000 deg/s. */
        s_i2c_address = WITMOTION_ADDRESS;
        DL_I2C_resetControllerTransfer(MPU6050_I2C_INST);
        if (!readRegisters(WITMOTION_REG_GZ, wit_data, 2U)) goto init_failed;
        s_backend = SENSOR_BACKEND_WIT_REGISTERS;
    }

    for (i = 0U; i < MPU6050_CALIBRATION_SAMPLES; ++i) {
        if (!readGyroRate(&rate_deg_s)) goto init_failed;
        sum += rate_deg_s;
        delay_cycles(CPUCLK_FREQ / 500U);
    }
    s_gyro_z_bias = sum / (float)MPU6050_CALIBRATION_SAMPLES;
    s_ready = true;
    return true;

init_failed:
    s_ready = false;
    s_probe_failed = true;
    return false;
}

bool BspMpu6050_Update(float dt_s)
{
    float rate_deg_s;
    if (!s_ready || dt_s <= 0.0f || !readGyroRate(&rate_deg_s)) {
        s_ready = false;
        return false;
    }
    s_yaw_rate_deg_s = rate_deg_s - s_gyro_z_bias;
    if (s_yaw_rate_deg_s > -0.5f && s_yaw_rate_deg_s < 0.5f)
        s_yaw_rate_deg_s = 0.0f;
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
    if (s_backend == SENSOR_BACKEND_RAW_MPU6050) return "RAW";
    if (s_backend == SENSOR_BACKEND_WIT_REGISTERS) return "WIT50";
    if (s_probe_failed) return "FAIL";
    return "NONE";
}
float BspMpu6050_GetYawDeg(void) { return s_yaw_deg; }
float BspMpu6050_GetYawRateDegS(void) { return s_yaw_rate_deg_s; }
