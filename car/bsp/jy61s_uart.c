#include "jy61s_uart.h"

#include "ti_msp_dl_config.h"

#include <stdint.h>

/* JY61S UART 每帧 11 字节：0x55、类型、8 字节数据、1 字节校验和。 */
#define JY61S_FRAME_LENGTH       (11U)
#define JY61S_HEADER             (0x55U)
#define JY61S_GYRO_FRAME         (0x52U)
#define JY61S_GYRO_DPS_PER_LSB   (2000.0f / 32768.0f)
#define JY61S_CALIBRATION_SAMPLES (100U)
#define JY61S_NO_FRAME_TIMEOUT_S  (0.50f)

static volatile uint8_t s_frame[JY61S_FRAME_LENGTH];
static volatile uint8_t s_frame_length;
static volatile uint32_t s_gyro_sequence;
static volatile int16_t s_latest_gyro_z_raw;

static bool s_ready;
static uint32_t s_consumed_sequence;
static uint32_t s_calibration_count;
static float s_calibration_sum;
static float s_gyro_z_bias;
static float s_yaw_deg;
static float s_yaw_rate_deg_s;
static float s_time_since_frame_s;

static bool frameChecksumValid(void)
{
    uint32_t i;
    uint8_t sum = 0U;
    for (i = 0U; i < JY61S_FRAME_LENGTH - 1U; ++i)
        sum = (uint8_t)(sum + s_frame[i]);
    return sum == s_frame[JY61S_FRAME_LENGTH - 1U];
}

bool BspJy61sUart_Init(void)
{
    /* 传感器数据通过中断接收，前台只负责校准和积分。 */
    s_frame_length = 0U;
    s_gyro_sequence = 0U;
    s_latest_gyro_z_raw = 0;
    s_ready = false;
    s_consumed_sequence = 0U;
    s_calibration_count = 0U;
    s_calibration_sum = 0.0f;
    s_gyro_z_bias = 0.0f;
    s_yaw_deg = 0.0f;
    s_yaw_rate_deg_s = 0.0f;
    s_time_since_frame_s = 0.0f;
    return true;
}

void BspJy61sUart_IRQHandler(void)
{
    /* 字节状态机可以从任意位置重新同步到 0x55 帧头。 */
    while (!DL_UART_Main_isRXFIFOEmpty(JY61_UART_INST)) {
        uint8_t value = DL_UART_Main_receiveData(JY61_UART_INST);
        if (s_frame_length == 0U) {
            if (value == JY61S_HEADER) s_frame[s_frame_length++] = value;
            continue;
        }
        s_frame[s_frame_length++] = value;
        if (s_frame_length < JY61S_FRAME_LENGTH) continue;

        /* 只缓存 0x52 角速度包，0x51/0x53/0x54 仍被完整消费。 */
        if (frameChecksumValid() && s_frame[1] == JY61S_GYRO_FRAME) {
            s_latest_gyro_z_raw = (int16_t)(((uint16_t)s_frame[7] << 8) |
                s_frame[6]);
            ++s_gyro_sequence;
        }
        s_frame_length = 0U;
    }
    DL_UART_Main_clearInterruptStatus(JY61_UART_INST,
        DL_UART_MAIN_INTERRUPT_RX);
}

bool BspJy61sUart_Update(float dt_s)
{
    uint32_t sequence;
    int16_t raw;
    float rate_deg_s;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    sequence = s_gyro_sequence;
    raw = s_latest_gyro_z_raw;
    if (primask == 0U) __enable_irq();

    if (dt_s <= 0.0f) return false;
    /* 超过 500 ms 没有新角速度帧就撤销 ready，防止用旧数据继续控制。 */
    if (sequence == s_consumed_sequence) {
        s_time_since_frame_s += dt_s;
        if (s_time_since_frame_s > JY61S_NO_FRAME_TIMEOUT_S)
            s_ready = false;
        return s_ready;
    }
    s_consumed_sequence = sequence;
    s_time_since_frame_s = 0.0f;
    rate_deg_s = (float)raw * JY61S_GYRO_DPS_PER_LSB;

    /* 上电前 100 个有效样本用于静止零偏校准，车辆必须保持不动。 */
    if (!s_ready) {
        s_calibration_sum += rate_deg_s;
        if (++s_calibration_count >= JY61S_CALIBRATION_SAMPLES) {
            s_gyro_z_bias = s_calibration_sum /
                (float)JY61S_CALIBRATION_SAMPLES;
            s_ready = true;
            s_yaw_deg = 0.0f;
        }
        return false;
    }

    s_yaw_rate_deg_s = rate_deg_s - s_gyro_z_bias;
    if (s_yaw_rate_deg_s > -0.5f && s_yaw_rate_deg_s < 0.5f)
        s_yaw_rate_deg_s = 0.0f;
    s_yaw_deg += s_yaw_rate_deg_s * dt_s;
    return true;
}

void BspJy61sUart_ZeroYaw(void) { s_yaw_deg = 0.0f; }
bool BspJy61sUart_IsReady(void) { return s_ready; }
const char *BspJy61sUart_GetBackendName(void) { return "UART2"; }
float BspJy61sUart_GetYawDeg(void) { return s_yaw_deg; }
float BspJy61sUart_GetYawRateDegS(void) { return s_yaw_rate_deg_s; }
