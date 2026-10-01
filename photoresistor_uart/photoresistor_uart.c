#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdint.h>

static volatile bool g_adc_ready;
static volatile bool g_command_ready;
static volatile uint8_t g_command_index;
static volatile char g_command[20];
static uint16_t g_threshold = 2048U;
static void uart_putc(char c);
static void uart_puts(const char *s);
static void uart_putu(uint32_t value);

static void process_command(void)
{
    uint32_t value = 0U;
    uint8_t i = 0U;
    if (g_command[0] == 'T' && g_command[1] == 'H' &&
        g_command[2] == 'R' && g_command[3] == '=') {
        i = 4U;
        while (g_command[i] >= '0' && g_command[i] <= '9') {
            value = value * 10U + (uint32_t)(g_command[i] - '0');
            i++;
        }
        if (value <= 4095U) {
            g_threshold = (uint16_t)value;
            uart_puts("OK THR="); uart_putu(g_threshold); uart_puts("\r\n");
        } else {
            uart_puts("ERR THR range 0..4095\r\n");
        }
    } else if (g_command[0] == 'G' && g_command[1] == 'E' && g_command[2] == 'T') {
        uart_puts("THR="); uart_putu(g_threshold); uart_puts("\r\n");
    } else {
        uart_puts("ERR command: THR=0..4095 or GET\r\n");
    }
}

static void uart_putc(char c)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, (uint8_t)c);
}

static void uart_puts(const char *s)
{
    while (*s != '\0') {
        uart_putc(*s++);
    }
}

static void uart_putu(uint32_t value)
{
    char digits[10];
    uint32_t i = 0;
    if (value == 0U) {
        uart_putc('0');
        return;
    }
    while (value != 0U) {
        digits[i++] = (char)('0' + (value % 10U));
        value /= 10U;
    }
    while (i != 0U) {
        uart_putc(digits[--i]);
    }
}

int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC_PHOTO_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    uart_puts("photoresistor ready; command THR=0..4095\r\n");

    while (1) {
        if (g_command_ready) {
            process_command();
            g_command_index = 0U;
            g_command_ready = false;
        }
        g_adc_ready = false;
        DL_ADC12_startConversion(ADC_PHOTO_INST);
        while (!g_adc_ready) {
            __WFE();
        }

        uint32_t raw = DL_ADC12_getMemResult(ADC_PHOTO_INST, ADC_PHOTO_ADCMEM_0);
        uint32_t millivolts = (raw * 3300U + 2047U) / 4095U;
        uint32_t do_high = (DL_GPIO_readPins(GPIO_PHOTO_PORT, GPIO_PHOTO_DO_PIN) != 0U);
        uint32_t sw_light = (raw >= g_threshold);
        if (sw_light) {
            DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_USER_LED_PIN);
        } else {
            DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_USER_LED_PIN);
        }

        uart_puts("AO=");
        uart_putu(raw);
        uart_puts("  V=");
        uart_putu(millivolts / 1000U);
        uart_putc('.');
        uart_putu((millivolts % 1000U) / 100U);
        uart_putu((millivolts % 100U) / 10U);
        uart_putu(millivolts % 10U);
        uart_puts(" V  DO=");
        uart_putu(do_high);
        uart_puts(" SW=");
        uart_putu(sw_light);
        uart_puts(" THR=");
        uart_putu(g_threshold);
        uart_puts("\r\n");

        DL_ADC12_enableConversions(ADC_PHOTO_INST);
        /* 20 ms sample/update period: 50 Hz sensor, LED and UART update rate. */
        delay_cycles(CPUCLK_FREQ / 50U);
    }
}

void UART_0_INST_IRQHandler(void)
{
    if (DL_UART_Main_getPendingInterrupt(UART_0_INST) == DL_UART_MAIN_IIDX_RX) {
        char c = (char)DL_UART_Main_receiveData(UART_0_INST);
        if (!g_command_ready) {
            if (c == '\r' || c == '\n') {
                g_command[g_command_index] = '\0';
                g_command_ready = true;
            } else if (g_command_index < (sizeof(g_command) - 1U)) {
                g_command[g_command_index++] = c;
            } else {
                g_command_index = 0U;
            }
        }
    }
}

void ADC_PHOTO_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_PHOTO_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            g_adc_ready = true;
            break;
        default:
            break;
    }
}

