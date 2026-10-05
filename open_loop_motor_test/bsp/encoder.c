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
#define ENCODER_A_B_HIGH_IS_FORWARD     (1)
#define ENCODER_B_B_HIGH_IS_FORWARD     (1)
#define DEFAULT_SAMPLE_PERIOD_S        (0.005f)

static volatile int32_t s_encoderA = 0;
static volatile int32_t s_encoderB = 0;
static int32_t s_previousA = 0;
static int32_t s_previousB = 0;

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

    if (speedA != 0) *speedA = (int32_t)((float)deltaA * MM_PER_ENCODER_COUNT / dt_s);
    if (speedB != 0) *speedB = (int32_t)((float)deltaB * MM_PER_ENCODER_COUNT / dt_s);
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
