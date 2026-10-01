#include "ti_msp_dl_config.h"
#include "encoder.h"

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
    while (1) {
        DL_GPIO_IIDX pending = DL_GPIO_getPendingInterrupt(ENCODERS_PORT);
        if (pending == DL_GPIO_IIDX_NO_INTR) break;

        switch (pending) {
        case ENCODERS_LEFT_ENCODER_A_IIDX: {
            bool bHigh = DL_GPIO_readPins(ENCODERS_PORT,
                ENCODERS_LEFT_ENCODER_B_PIN) != 0U;
            s_encoderA += (bHigh == (ENCODER_A_B_HIGH_IS_FORWARD != 0)) ? 1 : -1;
            break;
        }
        case ENCODERS_RIGHT_ENCODER_A_IIDX: {
            bool bHigh = DL_GPIO_readPins(ENCODERS_PORT,
                ENCODERS_RIGHT_ENCODER_B_PIN) != 0U;
            s_encoderB += (bHigh == (ENCODER_B_B_HIGH_IS_FORWARD != 0)) ? 1 : -1;
            break;
        }
        default:
            break;
        }
    }
}
