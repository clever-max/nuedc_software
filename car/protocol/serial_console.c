#include "serial_console.h"

#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_CAPACITY (24U)

static volatile char s_rx_buffer[COMMAND_CAPACITY];
static volatile uint8_t s_rx_length;
static volatile bool s_command_ready;

static void putChar(char value)
{
    DL_UART_Main_transmitDataBlocking(DEBUG_UART_INST, (uint8_t)value);
}

static void putU32(uint32_t value)
{
    char digits[10];
    uint32_t count = 0U;
    if (value == 0U) { putChar('0'); return; }
    while (value != 0U) { digits[count++] = (char)('0' + value % 10U); value /= 10U; }
    while (count != 0U) putChar(digits[--count]);
}

static void putI32(int32_t value)
{
    if (value < 0) { putChar('-'); putU32((uint32_t)(-(int64_t)value)); }
    else putU32((uint32_t)value);
}

static const char *stateName(DemoMissionState state)
{
    switch (state) {
    case DEMO_MISSION_IDLE: return "IDLE";
    case DEMO_MISSION_RUNNING: return "RUN";
    case DEMO_MISSION_DONE: return "DONE";
    case DEMO_MISSION_ABORTED: return "ABORT";
    default: return "?";
    }
}

void SerialConsole_Init(void)
{
    s_rx_length = 0U;
    s_command_ready = false;
    SerialConsole_WriteText("open-loop slow demo UART0 115200 8N1\r\n");
    SerialConsole_WriteText("B21 or RUN15 starts 15 s; B21 or STOP aborts\r\n");
}

void SerialConsole_WriteText(const char *text)
{
    while (*text != '\0') putChar(*text++);
}

SerialCommand SerialConsole_PollCommand(void)
{
    char command[COMMAND_CAPACITY];
    uint8_t length = 0U;
    uint32_t primask;
    if (!s_command_ready) return SERIAL_COMMAND_NONE;
    primask = __get_PRIMASK();
    __disable_irq();
    while (length < COMMAND_CAPACITY - 1U && s_rx_buffer[length] != '\0') {
        char value = s_rx_buffer[length];
        if (value >= 'a' && value <= 'z') value = (char)(value - 'a' + 'A');
        command[length++] = value;
    }
    command[length] = '\0';
    s_command_ready = false;
    s_rx_length = 0U;
    if (primask == 0U) __enable_irq();
    if (length == 5U && command[0]=='R' && command[1]=='U' && command[2]=='N' && command[3]=='1' && command[4]=='5') return SERIAL_COMMAND_RUN15;
    if (length == 4U && command[0]=='S' && command[1]=='T' && command[2]=='O' && command[3]=='P') return SERIAL_COMMAND_STOP;
    return SERIAL_COMMAND_UNKNOWN;
}

void SerialConsole_PrintTelemetry(const DemoMissionSnapshot *snapshot)
{
    if (snapshot == 0) return;
    SerialConsole_WriteText("ms="); putU32(snapshot->elapsed_ms);
    SerialConsole_WriteText(" state="); SerialConsole_WriteText(stateName(snapshot->state));
    SerialConsole_WriteText(" ticks="); putI32(snapshot->ticks_a); putChar('/'); putI32(snapshot->ticks_b);
    SerialConsole_WriteText(" speed_mm_s="); putI32(snapshot->speed_a_mm_s); putChar('/'); putI32(snapshot->speed_b_mm_s);
    SerialConsole_WriteText(" pos_mm="); putI32(snapshot->position_a_mm); putChar('/'); putI32(snapshot->position_b_mm);
    SerialConsole_WriteText(" pwm_permille="); putI32(snapshot->command_a_permille); putChar('/'); putI32(snapshot->command_b_permille);
    SerialConsole_WriteText("\r\n");
}

void SerialConsole_IRQHandler(void)
{
    if (DL_UART_Main_getPendingInterrupt(DEBUG_UART_INST) != DL_UART_MAIN_IIDX_RX) return;
    while (!DL_UART_Main_isRXFIFOEmpty(DEBUG_UART_INST)) {
        char value = (char)DL_UART_Main_receiveData(DEBUG_UART_INST);
        if (s_command_ready) continue;
        if (value == '\r') continue;
        if (value == '\n') {
            s_rx_buffer[s_rx_length] = '\0';
            s_command_ready = true;
        } else if (s_rx_length < COMMAND_CAPACITY - 1U) {
            s_rx_buffer[s_rx_length++] = value;
        } else {
            s_rx_length = 0U;
        }
    }
}
