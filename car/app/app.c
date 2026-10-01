#include "app.h"

#include "mission/demo_mission.h"
#include "bsp/encoder.h"
#include "bsp/motor_pwm.h"
#include "protocol/serial_console.h"
#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

#define CONTROL_PERIOD_MS (5U)
#define CONTROL_PERIOD_S (0.005f)
#define TELEMETRY_DIVIDER (20U)

static volatile uint32_t s_control_ticks;
static volatile uint8_t s_pending_ticks;
static volatile uint8_t s_telemetry_divider;
static volatile bool s_telemetry_due;

static uint8_t takePendingTicks(void)
{
    uint32_t primask = __get_PRIMASK();
    uint8_t result;
    __disable_irq();
    result = s_pending_ticks;
    s_pending_ticks = 0U;
    if (primask == 0U) __enable_irq();
    return result;
}

static void processButton(void)
{
    static bool latched;
    bool pressed = DL_GPIO_readPins(START_BUTTON_PORT,
        START_BUTTON_USER_BUTTON_B21_PIN) == 0U;
    if (!pressed) {
        latched = false;
    } else if (!latched) {
        delay_cycles(CPUCLK_FREQ / 100U);
        if (DL_GPIO_readPins(START_BUTTON_PORT,
                START_BUTTON_USER_BUTTON_B21_PIN) == 0U) {
            latched = true;
            if (DemoMission_IsRunning()) DemoMission_RequestStop();
            else (void)DemoMission_Start(s_control_ticks);
        }
    }
}

static void processSerialCommand(void)
{
    SerialCommand command = SerialConsole_PollCommand();
    switch (command) {
    case SERIAL_COMMAND_RUN15:
        if (DemoMission_Start(s_control_ticks)) SerialConsole_WriteText("OK RUN15\r\n");
        else SerialConsole_WriteText("ERR busy\r\n");
        break;
    case SERIAL_COMMAND_STOP:
        DemoMission_RequestStop();
        SerialConsole_WriteText("OK STOP\r\n");
        break;
    case SERIAL_COMMAND_UNKNOWN:
        SerialConsole_WriteText("Commands: RUN15, STOP\r\n");
        break;
    case SERIAL_COMMAND_NONE:
    default:
        break;
    }
}

void App_Init(void)
{
    SYSCFG_DL_init();
    BspMotor_Init();
    DemoMission_Init();
    SerialConsole_Init();

    NVIC_ClearPendingIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_EnableIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(ENCODERS_INT_IRQN);
    NVIC_EnableIRQ(ENCODERS_INT_IRQN);
}

void App_RunOnce(void)
{
    uint8_t elapsed_ticks;
    processSerialCommand();
    processButton();
    elapsed_ticks = takePendingTicks();
    if (elapsed_ticks != 0U) {
        float dt_s = CONTROL_PERIOD_S * (float)elapsed_ticks;
        int32_t speed_a, speed_b, position_a, position_b;
        BspEncoder_UpdateMeasurements(dt_s, &speed_a, &speed_b,
            &position_a, &position_b);
        DemoMission_Update(s_control_ticks,
            speed_a, speed_b, position_a, position_b);
    }
    if (s_telemetry_due) {
        DemoMissionSnapshot snapshot;
        s_telemetry_due = false;
        DemoMission_GetSnapshot(s_control_ticks, &snapshot);
        SerialConsole_PrintTelemetry(&snapshot);
    }
    delay_cycles(CPUCLK_FREQ / 1000U);
}

void App_OnControlTickInterrupt(void)
{
    if (DL_TimerG_getPendingInterrupt(CONTROL_TICK_INST) != DL_TIMER_IIDX_ZERO) return;
    ++s_control_ticks;
    if (s_pending_ticks < UINT8_MAX) ++s_pending_ticks;
    if (++s_telemetry_divider >= TELEMETRY_DIVIDER) {
        s_telemetry_divider = 0U;
        s_telemetry_due = true;
    }
}

void App_OnEncoderInterrupt(void)
{
    if (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) == DL_INTERRUPT_GROUP1_IIDX_GPIOB)
        BspEncoder_IRQHandler();
}

void App_OnUartInterrupt(void)
{
    SerialConsole_IRQHandler();
}

