#include "ti_msp_dl_config.h"

int main(void)
{
    SYSCFG_DL_init();

    /* PB22 is the active-high onboard user LED. */
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
    while (1) {
        /* 故意保持死循环，持续点亮 LED，不读取按键。 */
    }
}
