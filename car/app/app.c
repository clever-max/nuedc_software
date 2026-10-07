#include "app.h"

#include "mission/demo_mission.h"
#include "bsp/motor_pwm.h"
#include "bsp/encoder.h"
#include "bsp/mpu6050.h"
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
static uint8_t s_button_demo_stage;
static bool s_first_line_mission_active;
static bool s_rectangle_mission_active;

enum {
    BUTTON_DEMO_LINE_PENDING = 0U,
    BUTTON_DEMO_RECTANGLE_PENDING = 1U,
    BUTTON_DEMO_COMPLETE = 2U
};

static bool startLineMission(void)
{
    /* 当前验证版本完全不依赖陀螺仪，B21 直接启动灰度循迹。 */
    bool started = DemoMission_StartLineOnly(s_control_ticks);
    if (started && s_button_demo_stage == BUTTON_DEMO_LINE_PENDING)
        s_first_line_mission_active = true;
    return started;
}

static bool startRectangleMission(void)
{
    return DemoMission_StartRectangle(s_control_ticks);
}

static bool startButtonMission(void)
{
    if (s_button_demo_stage == BUTTON_DEMO_LINE_PENDING)
        return startLineMission();
    if (s_button_demo_stage == BUTTON_DEMO_RECTANGLE_PENDING) {
        bool started = startRectangleMission();
        if (started) s_rectangle_mission_active = true;
        return started;
    }
    return false;
}

static void updateButtonDemoStage(void)
{
    /* Only a completed first run unlocks the second B21 press.  An aborted
     * first run remains retryable and cannot accidentally skip to the turn run. */
    if (s_first_line_mission_active &&
        s_button_demo_stage == BUTTON_DEMO_LINE_PENDING &&
        DemoMission_GetState() == DEMO_MISSION_DONE) {
        s_button_demo_stage = BUTTON_DEMO_RECTANGLE_PENDING;
        s_first_line_mission_active = false;
        SerialConsole_WriteText("READY RECT30; B21 starts rectangle demo\r\n");
    }
    if (s_rectangle_mission_active &&
        DemoMission_GetState() == DEMO_MISSION_DONE) {
        s_button_demo_stage = BUTTON_DEMO_COMPLETE;
        s_rectangle_mission_active = false;
        SerialConsole_WriteText("RECT30 complete; B21 demo finished\r\n");
    }
}

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
            } else {
                bool rectangle_start =
                    s_button_demo_stage == BUTTON_DEMO_RECTANGLE_PENDING;
                bool started = startButtonMission();
                if (!started) {
                    BspMotor_Coast();
                    SerialConsole_WriteText("ERR B21 demo complete\r\n");
                    return;
                }
                BspBuzzer_Start(220U);
                if (rectangle_start)
                    SerialConsole_WriteText("OK B21 RECT30\r\n");
                else
                    SerialConsole_WriteText("OK B21 LINE30\r\n");
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
        if (startLineMission()) {
            SerialConsole_WriteText(command == SERIAL_COMMAND_RUNPID ? "OK RUNPID\r\n" : "OK RUN15\r\n");
            BspBuzzer_Start(220U);
        } else {
            SerialConsole_WriteText("ERR busy\r\n");
        }
        break;
    case SERIAL_COMMAND_RUNRECT:
        if (startRectangleMission()) {
            if (s_button_demo_stage == BUTTON_DEMO_RECTANGLE_PENDING)
                s_rectangle_mission_active = true;
            SerialConsole_WriteText("OK RUNRECT\r\n");
            BspBuzzer_Start(220U);
        } else {
            SerialConsole_WriteText("ERR busy\r\n");
        }
        break;
    case SERIAL_COMMAND_STOP:
        DemoMission_RequestStop();
        BspBuzzer_Start(120U);
        SerialConsole_WriteText("OK STOP\r\n");
        break;
    case SERIAL_COMMAND_UNKNOWN:
        SerialConsole_WriteText("Commands: RUNPID, RUN15, RUNRECT, STOP\r\n");
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
    /* 本验证版本不访问 MPU6050，避免未接传感器阻塞启动。 */
    DemoMission_Init(false);
    SerialConsole_Init();
    s_button_demo_stage = BUTTON_DEMO_LINE_PENDING;
    s_first_line_mission_active = false;
    s_rectangle_mission_active = false;
    SerialConsole_WriteText("MPU6050=DISABLED; B21: LINE30 then RECT30\r\n");

    NVIC_ClearPendingIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_EnableIRQ(CONTROL_TICK_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_EnableIRQ(DEBUG_UART_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(ENCODERS_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(ENCODERS_GPIOA_INT_IRQN);
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
}

void App_RunOnce(void)
{
    uint8_t elapsed_ticks;
    /* 前台循环保持非阻塞：先处理命令，再消费定时器积累的控制 tick。 */
    updateButtonDemoStage();
    processSerialCommand();
    processButton();
    elapsed_ticks = takePendingTicks();
    if (elapsed_ticks != 0U) {
        float dt_s = 0.005f * (float)elapsed_ticks;
        int32_t speed_a, speed_b, position_a, position_b;
        GraySensorSample gray;
        /* 一个控制周期内依次采集编码器、灰度和 MPU6050，并执行一次控制。 */
        BspEncoder_UpdateMeasurements(dt_s, &speed_a, &speed_b,
            &position_a, &position_b);
        BspGraySensor_Read(&gray);
        DemoMission_Update(s_control_ticks, dt_s, speed_a, speed_b,
            0.0f, 0.0f, false, gray.position, gray.valid, gray.bits,
            gray.bus_ok, gray.bus_stage, gray.write_address);
        BspBuzzer_Update();
    }
    if (s_telemetry_due) {
        DemoMissionSnapshot snapshot;
        s_telemetry_due = false;
        DemoMission_GetSnapshot(s_control_ticks, 0.0f, 0.0f, false, &snapshot);
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

void App_OnEncoderInterrupt(void)
{
    BspEncoder_IRQHandler();
}

