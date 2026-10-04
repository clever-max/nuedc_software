#include "serial_console.h"

#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_CAPACITY (24U)

/* UART0 接收只组装换行命令；命令含义在前台 App_RunOnce 中执行。 */
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

static void putHex16(uint16_t value)
{
    static const char digits[] = "0123456789ABCDEF";
    putChar(digits[(value >> 12) & 0x0FU]);
    putChar(digits[(value >> 8) & 0x0FU]);
    putChar(digits[(value >> 4) & 0x0FU]);
    putChar(digits[value & 0x0FU]);
}

static void putI32(int32_t value)
{
    if (value < 0) { putChar('-'); putU32((uint32_t)(-(int64_t)value)); }
    else putU32((uint32_t)value);
}

static void putFixed1(float value)
{
    bool negative = value < 0.0f;
    uint32_t scaled = (uint32_t)(negative ? -value * 10.0f : value * 10.0f);
    if (negative) putChar('-');
    putU32(scaled / 10U);
    putChar('.');
    putChar((char)('0' + scaled % 10U));
}

static const char *stateName(DemoMissionState state)
{
    switch (state) {
    case DEMO_MISSION_IDLE: return "IDLE";
    case DEMO_MISSION_STRAIGHT_1:
    case DEMO_MISSION_STRAIGHT_2:
    case DEMO_MISSION_STRAIGHT_3:
    case DEMO_MISSION_STRAIGHT_4: return "FWD";
    case DEMO_MISSION_COAST_TO_TURN_1:
    case DEMO_MISSION_COAST_TO_STRAIGHT_2:
    case DEMO_MISSION_COAST_TO_TURN_2:
    case DEMO_MISSION_COAST_TO_STRAIGHT_3:
    case DEMO_MISSION_COAST_TO_TURN_3:
    case DEMO_MISSION_COAST_TO_STRAIGHT_4: return "COAST";
    case DEMO_MISSION_TURN_1:
    case DEMO_MISSION_TURN_2:
    case DEMO_MISSION_TURN_3: return "TURN";
    case DEMO_MISSION_DONE: return "DONE";
    case DEMO_MISSION_ABORTED: return "ABORT";
    default: return "?";
    }
}

void SerialConsole_Init(void)
{
    s_rx_length = 0U;
    s_command_ready = false;
    SerialConsole_WriteText("12ch gray line-follow + encoder PID demo UART0 115200 8N1\r\n");
    SerialConsole_WriteText("B21/RUNPID/RUN15: gray line 15s; STOP aborts\r\n");
    SerialConsole_WriteText("wheel target 200 mm/s; gyro disabled in this demo\r\n");
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
    /* 临界区内复制 ISR 缓冲区，释放中断后再比较命令字符串。 */
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
    if (length == 6U && command[0]=='R' && command[1]=='U' && command[2]=='N' && command[3]=='P' && command[4]=='I' && command[5]=='D') return SERIAL_COMMAND_RUNPID;
    if (length == 4U && command[0]=='S' && command[1]=='T' && command[2]=='O' && command[3]=='P') return SERIAL_COMMAND_STOP;
    return SERIAL_COMMAND_UNKNOWN;
}

void SerialConsole_PrintTelemetry(const DemoMissionSnapshot *snapshot)
{
    if (snapshot == 0) return;
    /* 遥测字段保持单行，方便 Python/串口工具按空格切分。 */
    SerialConsole_WriteText("ms="); putU32(snapshot->elapsed_ms);
    SerialConsole_WriteText(" state="); SerialConsole_WriteText(stateName(snapshot->state));
    SerialConsole_WriteText(" gyro=");
    if (!snapshot->gyro_required) SerialConsole_WriteText("OFF");
    else SerialConsole_WriteText(snapshot->gyro_ready ? "OK" : "ERR");
    SerialConsole_WriteText(" backend="); SerialConsole_WriteText(snapshot->gyro_backend);
    SerialConsole_WriteText(" line="); putFixed1(snapshot->line_error);
    SerialConsole_WriteText(snapshot->line_valid ? "(OK)" : "(LOST)");
    SerialConsole_WriteText(" gray=0x"); putHex16(snapshot->gray_bits);
    SerialConsole_WriteText(" yaw="); putFixed1(snapshot->yaw_deg);
    SerialConsole_WriteText(" target_yaw="); putFixed1(snapshot->target_yaw_deg);
    SerialConsole_WriteText(" yaw_rate="); putFixed1(snapshot->yaw_rate_deg_s);
    SerialConsole_WriteText(" turn_err="); putFixed1(snapshot->turn_error_deg);
    SerialConsole_WriteText(" speed="); putI32(snapshot->speed_a_mm_s);
    putChar('/'); putI32(snapshot->speed_b_mm_s);
    SerialConsole_WriteText(" pwm_permille="); putI32(snapshot->command_a_permille); putChar('/'); putI32(snapshot->command_b_permille);
    SerialConsole_WriteText("\r\n");
}

void SerialConsole_IRQHandler(void)
{
    if (DL_UART_Main_getPendingInterrupt(DEBUG_UART_INST) != DL_UART_MAIN_IIDX_RX) return;
    /* 先清 RX 中断源，再排空 FIFO，避免接收一个字节后反复进入 ISR。 */
    DL_UART_Main_clearInterruptStatus(DEBUG_UART_INST,
        DL_UART_MAIN_INTERRUPT_RX);
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
