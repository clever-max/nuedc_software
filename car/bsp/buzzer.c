#include "buzzer.h"

#include "ti_msp_dl_config.h"

static uint16_t s_remaining_ticks;
static uint8_t s_phase;

void BspBuzzer_Init(void)
{
    /* 无源蜂鸣器上电保持低电平，避免复位瞬间误响。 */
    s_remaining_ticks = 0U;
    s_phase = 0U;
    DL_GPIO_clearPins(BEEPER_PORT, BEEPER_PASSIVE_BUZZER_PIN);
}

void BspBuzzer_Start(uint16_t duration_ms)
{
    /* 控制周期为 5 ms，使用非阻塞 tick 计时。 */
    s_remaining_ticks = (uint16_t)((duration_ms + 4U) / 5U);
    s_phase = 0U;
}

void BspBuzzer_Update(void)
{
    /* 每两个控制 tick 翻转一次 GPIO，形成低频提示音。 */
    if (s_remaining_ticks == 0U) {
        DL_GPIO_clearPins(BEEPER_PORT, BEEPER_PASSIVE_BUZZER_PIN);
        return;
    }
    if ((s_phase++ & 1U) != 0U)
        DL_GPIO_togglePins(BEEPER_PORT, BEEPER_PASSIVE_BUZZER_PIN);
    if (--s_remaining_ticks == 0U)
        DL_GPIO_clearPins(BEEPER_PORT, BEEPER_PASSIVE_BUZZER_PIN);
}
