#include "ti_msp_dl_config.h"

int main(void)
{
    SYSCFG_DL_init();

    /* LED1 on LP-MSPM0G3507 is active-low. */
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);

    while (1) {
    }
}
