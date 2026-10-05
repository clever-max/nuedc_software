#include "app.h"

#include "bsp/motor_pwm.h"
#include "bsp/buzzer.h"
#include "protocol/serial_console.h"
#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

/* 独立电机/驱动板验证：不读取编码器、灰度或陀螺仪。 */
#define OPEN_LOOP_PWM_PERMILLE   (250)
#define OPEN_LOOP_DURATION_TICKS (1000U) /* 5 ms × 1000 = 5 s */

static volatile uint32_t s_ticks;
static bool s_running;
static uint32_t s_start_tick;

static void startTest(void)
{
    if (s_running) return;
    s_running = true;
    s_start_tick = s_ticks;
    BspMotor_SetCommand(OPEN_LOOP_PWM_PERMILLE, OPEN_LOOP_PWM_PERMILLE);
    BspBuzzer_Start(180U);
    SerialConsole_WriteText("OK OPENLOOP pwm=250 duration=5s\r\n");
}

static void stopTest(void)
{
    s_running = false;
    BspMotor_Coast();
    BspBuzzer_Start(100U);
    SerialConsole_WriteText("OPENLOOP STOP\r\n");
}

static void processButton(void)
{
    static bool latched;
    bool pressed = DL_GPIO_readPins(START_BUTTON_PORT,
        START_BUTTON_USER_BUTTON_B21_PIN) == 0U;
    if (!pressed) latched = false;
    else if (!latched) {
        latched = true;
        if (s_running) stopTest();
        else startTest();
    }
}

static void processSerial(void)
{
    SerialCommand command = SerialConsole_PollCommand();
    if (command == SERIAL_COMMAND_RUN15 || command == SERIAL_COMMAND_RUNPID)
        startTest();
    else if (command == SERIAL_COMMAND_STOP)
        stopTest();
}

void App_Init(void)
{
    SYSCFG_DL_init();
    BspMotor_Init();
    BspBuzzer_Init();
    s_ticks = 0U;
    s_running = false;
    s_start_tick = 0U;
    SerialConsole_Init();
    SerialConsole_WriteText("OPENLOOP MOTOR TEST: no encoder/PID/gyro/gray\r\n");
    SerialConsole_WriteText("B21 or RUN15 starts 250/1000 for 5 seconds\r\n");
    NVIC_ClearPendingIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_EnableIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(DEBUG_UART_INST_INT_IRQN);
}

void App_RunOnce(void)
{
    processSerial();
    processButton();
    if (s_running && (s_ticks - s_start_tick) >= OPEN_LOOP_DURATION_TICKS)
        stopTest();
    BspBuzzer_Update();
    delay_cycles(CPUCLK_FREQ / 1000U);
}

void App_OnControlTickInterrupt(void)
{
    if (DL_TimerG_getPendingInterrupt(CONTROL_TICK_INST) == DL_TIMER_IIDX_ZERO)
        ++s_ticks;
}

void App_OnUartInterrupt(void) { SerialConsole_IRQHandler(); }
void App_OnEncoderInterrupt(void) { }
