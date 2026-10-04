#include "app.h"

#include "mission/demo_mission.h"
#include "bsp/motor_pwm.h"
#include "bsp/encoder.h"
#include "bsp/jy61s_uart.h"
#include "bsp/gray_sensor.h"
#include "bsp/buzzer.h"
#include "protocol/serial_console.h"
#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdint.h>

#define TELEMETRY_DIVIDER (20U)

/* 中断只递增计数或收字节，避免在 ISR 中执行 I2C、PID 和串口打印。 */
static volatile uint32_t s_control_ticks;
static volatile uint8_t s_pending_ticks;
static volatile uint8_t s_telemetry_divider;
static volatile bool s_telemetry_due;

static uint8_t takePendingTicks(void)
{
    uint32_t primask = __get_PRIMASK();
    uint8_t result;
    /* 读取并清零待处理 tick 必须原子完成，避免丢失定时器事件。 */
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
        /* B21 为低电平有效；这里用约 10 ms 延时做消抖。 */
        delay_cycles(CPUCLK_FREQ / 100U);
        if (DL_GPIO_readPins(START_BUTTON_PORT,
                START_BUTTON_USER_BUTTON_B21_PIN) == 0U) {
            latched = true;
            if (DemoMission_IsRunning()) {
                DemoMission_RequestStop();
                BspBuzzer_Start(120U);
            } else if (DemoMission_Start(s_control_ticks, BspJy61sUart_GetYawDeg())) {
                BspBuzzer_Start(220U);
            }
        }
    }
}

static void processSerialCommand(void)
{
    SerialCommand command = SerialConsole_PollCommand();
    /* 串口命令和 B21 共用同一个任务状态机，避免两套启动逻辑分叉。 */
    switch (command) {
    case SERIAL_COMMAND_RUN15:
    case SERIAL_COMMAND_RUNPID:
        if (DemoMission_Start(s_control_ticks, BspJy61sUart_GetYawDeg())) {
            SerialConsole_WriteText(command == SERIAL_COMMAND_RUNPID ? "OK RUNPID\r\n" : "OK RUN15\r\n");
            BspBuzzer_Start(220U);
        } else if (!BspJy61sUart_IsReady())
            SerialConsole_WriteText("ERR gyro\r\n");
        else SerialConsole_WriteText("ERR busy\r\n");
        break;
    case SERIAL_COMMAND_STOP:
        DemoMission_RequestStop();
        BspBuzzer_Start(120U);
        SerialConsole_WriteText("OK STOP\r\n");
        break;
    case SERIAL_COMMAND_UNKNOWN:
        SerialConsole_WriteText("Commands: RUNPID, RUN15, STOP\r\n");
        break;
    case SERIAL_COMMAND_NONE:
    default:
        break;
    }
}

void App_Init(void)
{
    /* SysConfig 先完成所有时钟、引脚和外设初始化，再初始化业务模块。 */
    SYSCFG_DL_init();
    BspMotor_Init();
    BspGraySensor_Init();
    BspBuzzer_Init();
    (void)BspJy61sUart_Init();
    DemoMission_Init(BspJy61sUart_IsReady());
    SerialConsole_Init();

    NVIC_ClearPendingIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_EnableIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(JY61_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(JY61_UART_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(ENCODERS_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(ENCODERS_GPIOA_INT_IRQN);
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
}

void App_RunOnce(void)
{
    uint8_t elapsed_ticks;
    /* 前台循环保持非阻塞：先处理命令，再消费定时器积累的控制 tick。 */
    processSerialCommand();
    processButton();
    elapsed_ticks = takePendingTicks();
    if (elapsed_ticks != 0U) {
        float dt_s = 0.005f * (float)elapsed_ticks;
        int32_t speed_a, speed_b, position_a, position_b;
        GraySensorSample gray;
        /* 一个控制周期内依次采集编码器、灰度和 JY61S，并执行一次控制。 */
        BspEncoder_UpdateMeasurements(dt_s, &speed_a, &speed_b,
            &position_a, &position_b);
        BspGraySensor_Read(&gray);
        BspJy61sUart_Update(dt_s);
        DemoMission_Update(s_control_ticks, dt_s, speed_a, speed_b,
            BspJy61sUart_GetYawDeg(), BspJy61sUart_GetYawRateDegS(),
            BspJy61sUart_IsReady(), gray.position, gray.valid, gray.bits);
        BspBuzzer_Update();
    }
    if (s_telemetry_due) {
        DemoMissionSnapshot snapshot;
        s_telemetry_due = false;
        DemoMission_GetSnapshot(s_control_ticks, BspJy61sUart_GetYawDeg(),
            BspJy61sUart_GetYawRateDegS(), BspJy61sUart_IsReady(), &snapshot);
        SerialConsole_PrintTelemetry(&snapshot);
    }
    delay_cycles(CPUCLK_FREQ / 1000U);
}

void App_OnControlTickInterrupt(void)
{
    if (DL_TimerG_getPendingInterrupt(CONTROL_TICK_INST) != DL_TIMER_IIDX_ZERO) return;
    ++s_control_ticks;
    /* 遥测每 20 个 tick 输出一次，即约 10 Hz。 */
    if (s_pending_ticks < UINT8_MAX) ++s_pending_ticks;
    if (++s_telemetry_divider >= TELEMETRY_DIVIDER) {
        s_telemetry_divider = 0U;
        s_telemetry_due = true;
    }
}

void App_OnUartInterrupt(void)
{
    SerialConsole_IRQHandler();
}

void App_OnGyroUartInterrupt(void)
{
    BspJy61sUart_IRQHandler();
}

void App_OnEncoderInterrupt(void)
{
    BspEncoder_IRQHandler();
}

