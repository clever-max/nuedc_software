#include "ti_msp_dl_config.h"

/* Tianmengxing onboard LED: PB22, active high; PWM output is TIMG8-C1. */
#define PWM_PERIOD_TICKS 1000U
#define DUTY_STEP        10U
#define FADE_STEP_DELAY  800000U

int main(void)
{
    uint32_t duty;

    SYSCFG_DL_init();
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_PERIOD_TICKS - 1U,
        GPIO_PWM_0_C1_IDX);
    DL_TimerG_startCounter(PWM_0_INST);

    while (1) {
        for (duty = PWM_PERIOD_TICKS - 1U; duty >= DUTY_STEP; duty -= DUTY_STEP) {
            DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, GPIO_PWM_0_C1_IDX);
            delay_cycles(FADE_STEP_DELAY);
        }
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, 1U, GPIO_PWM_0_C1_IDX);
        delay_cycles(FADE_STEP_DELAY);

        for (duty = 1U; duty < PWM_PERIOD_TICKS - 1U; duty += DUTY_STEP) {
            DL_TimerG_setCaptureCompareValue(PWM_0_INST, duty, GPIO_PWM_0_C1_IDX);
            delay_cycles(FADE_STEP_DELAY);
        }
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_PERIOD_TICKS - 1U,
            GPIO_PWM_0_C1_IDX);
        delay_cycles(FADE_STEP_DELAY);
    }
}
