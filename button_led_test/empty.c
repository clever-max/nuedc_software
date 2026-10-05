#include "ti_msp_dl_config.h"

int main(void)
{
    SYSCFG_DL_init();

    /* Tianmengxing onboard LED is connected to PB22 and is active-high. */
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);

    while (1) {
        /* B21 is active-low: pressed = PB21 reads 0. */
        if (DL_GPIO_readPins(GPIO_BUTTONS_PORT, GPIO_BUTTONS_USER_BUTTON_B21_PIN) == 0) {
            DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        } else {
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        }
    }
}
