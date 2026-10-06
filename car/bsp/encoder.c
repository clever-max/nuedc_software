#include "ti_msp_dl_config.h"
#include "encoder.h"

#include <stdbool.h>

/*
 * MG513X Hall 编码器按用户资料取 13 PPR、1:28 减速比；当前只统计 A
 * 相上升沿，并用 B 相电平判向，因此每个轮子先按 13*28=364 count/rev
 * 换算。这个比例仍需用实测转一圈校准。
 */
#define WHEEL_DIAMETER_MM             (65.0f)
#define GEAR_RATIO                     (28.0f)
#define HALL_ENCODER_PPR               (13.0f)
#define COUNTS_PER_WHEEL_REV           (HALL_ENCODER_PPR * GEAR_RATIO)
#define MM_PER_ENCODER_COUNT           (3.14159265358979323846f * WHEEL_DIAMETER_MM / COUNTS_PER_WHEEL_REV)
/* 依据供应商 AT8236 例程和当前电机安装方向，左轮 A 上升沿时 B 低为正。 */
#define ENCODER_A_B_HIGH_IS_FORWARD     (0)
/* 右轮 A 上升沿时 B 高为正。 */
#define ENCODER_B_B_HIGH_IS_FORWARD     (1)
#define DEFAULT_SAMPLE_PERIOD_S        (0.005f)
#define SPEED_MEASURE_WINDOW_S         (0.020f)
#define SPEED_FILTER_ALPHA             (0.70f)

static volatile int32_t s_encoderA = 0;
static volatile int32_t s_encoderB = 0;
static int32_t s_previousA = 0;
static int32_t s_previousB = 0;
static int32_t s_windowDeltaA = 0;
static int32_t s_windowDeltaB = 0;
static float s_windowTimeS = 0.0f;
static float s_filteredSpeedA = 0.0f;
static float s_filteredSpeedB = 0.0f;
static bool s_speedReady;

static uint32_t lockInterrupts(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void unlockInterrupts(uint32_t primask)
{
    if (primask == 0U) __enable_irq();
}

void BspEncoder_Reset(void)
{
    /* 启动新任务时同时清零累计位置和测速窗口历史。 */
    uint32_t primask = lockInterrupts();
    s_encoderA = 0;
    s_encoderB = 0;
    s_previousA = 0;
    s_previousB = 0;
    s_windowDeltaA = 0;
    s_windowDeltaB = 0;
    s_windowTimeS = 0.0f;
    s_filteredSpeedA = 0.0f;
    s_filteredSpeedB = 0.0f;
    s_speedReady = false;
    unlockInterrupts(primask);
}

void BspEncoder_GetCounts(int32_t *a, int32_t *b)
{
    uint32_t primask = lockInterrupts();
    if (a != 0) *a = s_encoderA;
    if (b != 0) *b = s_encoderB;
    unlockInterrupts(primask);
}

void BspEncoder_UpdateMeasurements(float dt_s, int32_t *speedA,
    int32_t *speedB, int32_t *positionA, int32_t *positionB)
{
    int32_t countA, countB;
    int32_t deltaA, deltaB;
    uint32_t primask = lockInterrupts();
    countA = s_encoderA;
    countB = s_encoderB;
    unlockInterrupts(primask);

    if (dt_s <= 0.0f) dt_s = DEFAULT_SAMPLE_PERIOD_S;
    /* M 法测速：本窗口新增 count / dt，再换算为轮缘线速度。 */
    deltaA = countA - s_previousA;
    deltaB = countB - s_previousB;
    s_previousA = countA;
    s_previousB = countB;

    /* 13 PPR 在 5 ms 内只有 0～2 个脉冲，直接测速会产生约 112 mm/s
     * 的量化跳变。先聚合 20 ms，再做一阶平滑供速度环使用。 */
    s_windowDeltaA += deltaA;
    s_windowDeltaB += deltaB;
    s_windowTimeS += dt_s;
    if (s_windowTimeS >= SPEED_MEASURE_WINDOW_S) {
        float rawA = (float)s_windowDeltaA * MM_PER_ENCODER_COUNT /
            s_windowTimeS;
        float rawB = (float)s_windowDeltaB * MM_PER_ENCODER_COUNT /
            s_windowTimeS;
        if (!s_speedReady) {
            s_filteredSpeedA = rawA;
            s_filteredSpeedB = rawB;
            s_speedReady = true;
        } else {
            s_filteredSpeedA = SPEED_FILTER_ALPHA * rawA +
                (1.0f - SPEED_FILTER_ALPHA) * s_filteredSpeedA;
            s_filteredSpeedB = SPEED_FILTER_ALPHA * rawB +
                (1.0f - SPEED_FILTER_ALPHA) * s_filteredSpeedB;
        }
        s_windowDeltaA = 0;
        s_windowDeltaB = 0;
        s_windowTimeS = 0.0f;
    }
    if (speedA != 0) *speedA = (int32_t)s_filteredSpeedA;
    if (speedB != 0) *speedB = (int32_t)s_filteredSpeedB;
    if (positionA != 0) *positionA = (int32_t)((float)countA * MM_PER_ENCODER_COUNT);
    if (positionB != 0) *positionB = (int32_t)((float)countB * MM_PER_ENCODER_COUNT);
}

void BspEncoder_IRQHandler(void)
{
    /* ISR 只做最短路径：读取 A 中断、采样 B 判向、清除标志。 */
    uint32_t pendingA = DL_GPIO_getEnabledInterruptStatus(GPIOA,
        ENCODERS_LEFT_ENCODER_A_PIN);
    uint32_t pendingB = DL_GPIO_getEnabledInterruptStatus(GPIOB,
        ENCODERS_RIGHT_ENCODER_A_PIN | MPU6050_INT_DATA_READY_PIN);

    if ((pendingA & ENCODERS_LEFT_ENCODER_A_PIN) != 0U) {
        bool bHigh = DL_GPIO_readPins(GPIOA, ENCODERS_LEFT_ENCODER_B_PIN) != 0U;
        s_encoderA += (bHigh == (ENCODER_A_B_HIGH_IS_FORWARD != 0)) ? 1 : -1;
    }
    if ((pendingB & ENCODERS_RIGHT_ENCODER_A_PIN) != 0U) {
        bool bHigh = DL_GPIO_readPins(GPIOB, ENCODERS_RIGHT_ENCODER_B_PIN) != 0U;
        s_encoderB += (bHigh == (ENCODER_B_B_HIGH_IS_FORWARD != 0)) ? 1 : -1;
    }

    /* PB1 is wired to MPU6050 INT, but the current sensor driver polls I2C.
     * Consume its GPIO edge here so it cannot retrigger GROUP1. */
    if (pendingA != 0U)
        DL_GPIO_clearInterruptStatus(GPIOA, pendingA);
    if (pendingB != 0U)
        DL_GPIO_clearInterruptStatus(GPIOB, pendingB);
}
