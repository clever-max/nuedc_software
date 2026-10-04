#include "demo_mission.h"

#include "bsp/encoder.h"
#include "bsp/motor_pwm.h"
#include "control/wheel_speed_controller.h"
#include "bsp/jy61s_uart.h"

#include <math.h>

/* 当前 PID_ONLY_DEMO=1：用编码器+灰度完成 15 s 循迹；陀螺仪状态仍保留
 * 在接口中，后续切回直角转弯版本时只需关闭该开关。 */
#define CONTROL_PERIOD_MS              (5U)
#define PID_ONLY_DEMO                  (1)
#define PID_DEMO_STAGE_MS             (15000U)
#define STRAIGHT_STAGE_MS              (2000U)
#define COAST_STAGE_MS                 (300U)
#define TURN_TIMEOUT_MS                (3000U)
#define TARGET_SPEED_MM_S              (200.0f)
#define TURN_TARGET_DEG                (90.0f)
#define TURN_MIN_SPEED_MM_S            (45.0f)
#define TURN_MAX_SPEED_MM_S            (200.0f)
#define TURN_COMPLETE_TOLERANCE_DEG    (1.5f)
#define TURN_RATE_TOLERANCE_DEG_S      (12.0f)
#define TURN_ANGLE_KP                  (4.0f)
#define TURN_ANGLE_KI                  (0.0f)
#define TURN_ANGLE_KD                  (0.25f)
#define TURN_INTEGRAL_LIMIT            (80.0f)
#define TURN_MIN_RUNTIME_MS            (250U)
#define RIGHT_TURN_YAW_SIGN            (-1.0f)
#define HEADING_KP                     (2.0f)
#define HEADING_KD                     (0.10f)
#define HEADING_CORRECTION_LIMIT_MM_S  (70.0f)

/* Starting values only; tune from UART speed telemetry after wiring is verified. */
#define WHEEL_PID_KP                   (0.50f)
#define WHEEL_PID_KI                   (0.20f)
#define WHEEL_PID_KD                   (0.00f)
#define WHEEL_PID_FEED_FORWARD         (1.00f)
#define LINE_PID_KP                    (8.0f)
#define LINE_PID_KI                    (0.0f)
#define LINE_PID_KD                    (0.05f)
#define LINE_CORRECTION_LIMIT_MM_S     (100.0f)
#define LINE_INTEGRAL_LIMIT            (20.0f)

static DemoMissionState s_state = DEMO_MISSION_IDLE;
static DualWheelSpeedController s_wheel_controller;
static uint32_t s_mission_start_tick;
static uint32_t s_stage_start_tick;
static float s_heading_reference_deg;
static float s_turn_start_yaw_deg;
static float s_target_yaw_deg;
static float s_turn_integral;
static float s_yaw_rate_deg_s;
static float s_turn_error_deg;
static bool s_stop_requested;
static bool s_gyro_ready;
static bool s_gyro_required;
static float s_line_error;
static float s_line_integral;
static bool s_line_valid;
static uint16_t s_gray_bits;
static float s_previous_line_error;
static int32_t s_speed_a_mm_s;
static int32_t s_speed_b_mm_s;

static bool isStraightState(DemoMissionState state)
{
    return state == DEMO_MISSION_STRAIGHT_1 ||
        state == DEMO_MISSION_STRAIGHT_2 ||
        state == DEMO_MISSION_STRAIGHT_3 ||
        state == DEMO_MISSION_STRAIGHT_4;
}

static bool isTurnState(DemoMissionState state)
{
    return state == DEMO_MISSION_TURN_1 ||
        state == DEMO_MISSION_TURN_2 ||
        state == DEMO_MISSION_TURN_3;
}

static bool isCoastState(DemoMissionState state)
{
    return state == DEMO_MISSION_COAST_TO_TURN_1 ||
        state == DEMO_MISSION_COAST_TO_STRAIGHT_2 ||
        state == DEMO_MISSION_COAST_TO_TURN_2 ||
        state == DEMO_MISSION_COAST_TO_STRAIGHT_3 ||
        state == DEMO_MISSION_COAST_TO_TURN_3 ||
        state == DEMO_MISSION_COAST_TO_STRAIGHT_4;
}

static void enterState(DemoMissionState state, uint32_t now_tick,
    float yaw_deg)
{
    s_state = state;
    s_stage_start_tick = now_tick;
    /* 每个状态入口统一设置计时点、目标和 PID 历史，避免跨阶段残留。 */
    if (isStraightState(state)) {
        s_heading_reference_deg = yaw_deg;
        s_target_yaw_deg = yaw_deg;
        s_turn_error_deg = 0.0f;
        WheelSpeedController_SetTargets(&s_wheel_controller,
            TARGET_SPEED_MM_S, TARGET_SPEED_MM_S);
        WheelSpeedController_Reset(&s_wheel_controller);
    } else if (isTurnState(state)) {
        s_turn_start_yaw_deg = yaw_deg;
        s_target_yaw_deg = yaw_deg + RIGHT_TURN_YAW_SIGN * TURN_TARGET_DEG;
        s_turn_integral = 0.0f;
        s_turn_error_deg = TURN_TARGET_DEG;
        WheelSpeedController_SetTargets(&s_wheel_controller, 0.0f, 0.0f);
        WheelSpeedController_Reset(&s_wheel_controller);
    } else {
        s_turn_error_deg = 0.0f;
        WheelSpeedController_SetTargets(&s_wheel_controller, 0.0f, 0.0f);
        WheelSpeedController_Reset(&s_wheel_controller);
        BspMotor_Coast();
    }
}

static uint32_t elapsedStageMs(uint32_t now_tick)
{
    return (now_tick - s_stage_start_tick) * CONTROL_PERIOD_MS;
}

static void updateWheelOutput(float dt_s, float target_a, float target_b)
{
    int16_t command_a;
    int16_t command_b;
    /* 灰度控制只改变两个轮速目标，最终 PWM 仍由编码器速度环负责。 */
    WheelSpeedController_SetTargets(&s_wheel_controller, target_a, target_b);
    WheelSpeedController_Update(&s_wheel_controller,
        (float)s_speed_a_mm_s, (float)s_speed_b_mm_s, dt_s,
        &command_a, &command_b);
    BspMotor_SetCommand(command_a, command_b);
}

void DemoMission_Init(bool gyro_ready)
{
    s_state = DEMO_MISSION_IDLE;
    s_mission_start_tick = 0U;
    s_stage_start_tick = 0U;
    s_heading_reference_deg = 0.0f;
    s_turn_start_yaw_deg = 0.0f;
    s_target_yaw_deg = 0.0f;
    s_turn_integral = 0.0f;
    s_yaw_rate_deg_s = 0.0f;
    s_turn_error_deg = 0.0f;
    s_stop_requested = false;
    s_gyro_ready = gyro_ready;
    s_gyro_required = PID_ONLY_DEMO == 0;
    s_line_error = 0.0f;
    s_line_integral = 0.0f;
    s_line_valid = false;
    s_gray_bits = 0U;
    s_previous_line_error = 0.0f;
    s_speed_a_mm_s = 0;
    s_speed_b_mm_s = 0;
    WheelSpeedController_Init(&s_wheel_controller,
        WHEEL_PID_KP, WHEEL_PID_KI, WHEEL_PID_KD,
        WHEEL_PID_FEED_FORWARD, 1000.0f);
    BspMotor_Coast();
}

bool DemoMission_CanStart(void)
{
    return s_state == DEMO_MISSION_IDLE ||
        s_state == DEMO_MISSION_DONE || s_state == DEMO_MISSION_ABORTED;
}

bool DemoMission_IsRunning(void)
{
    return s_state != DEMO_MISSION_IDLE &&
        s_state != DEMO_MISSION_DONE && s_state != DEMO_MISSION_ABORTED;
}

bool DemoMission_Start(uint32_t now_tick, float yaw_deg)
{
    if (!DemoMission_CanStart() || (s_gyro_required && !s_gyro_ready)) return false;
    BspEncoder_Reset();
    s_mission_start_tick = now_tick;
    s_stage_start_tick = now_tick;
    s_stop_requested = false;
    s_heading_reference_deg = yaw_deg;
    s_target_yaw_deg = yaw_deg;
    s_turn_integral = 0.0f;
    s_yaw_rate_deg_s = 0.0f;
    s_turn_error_deg = 0.0f;
    s_line_error = 0.0f;
    s_line_integral = 0.0f;
    s_line_valid = false;
    s_gray_bits = 0U;
    s_previous_line_error = 0.0f;
    s_speed_a_mm_s = 0;
    s_speed_b_mm_s = 0;
    enterState(DEMO_MISSION_STRAIGHT_1, now_tick, yaw_deg);
    BspMotor_Coast();
    return true;
}

void DemoMission_RequestStop(void)
{
    if (DemoMission_IsRunning()) s_stop_requested = true;
    else BspMotor_Coast();
}

void DemoMission_Update(uint32_t now_tick, float dt_s,
    int32_t speed_a_mm_s, int32_t speed_b_mm_s,
    float yaw_deg, float yaw_rate_deg_s, bool gyro_ready,
    float line_error, bool line_valid, uint16_t gray_bits)
{
    uint32_t elapsed_ms;
    float heading_error;
    float correction;
    float turn_progress;
    float turn_rate;
    float turn_speed;
    float turn_error;

    s_speed_a_mm_s = speed_a_mm_s;
    s_speed_b_mm_s = speed_b_mm_s;
    s_yaw_rate_deg_s = yaw_rate_deg_s;
    s_gyro_ready = gyro_ready;
    s_line_error = line_error;
    s_line_valid = line_valid;
    s_gray_bits = gray_bits;
    if (s_stop_requested || (s_gyro_required && !s_gyro_ready)) {
        s_stop_requested = false;
        s_state = DEMO_MISSION_ABORTED;
        WheelSpeedController_Reset(&s_wheel_controller);
        BspMotor_Coast();
        return;
    }

    elapsed_ms = elapsedStageMs(now_tick);
    if (!s_gyro_required) {
        float correction = 0.0f;
        if (line_valid) {
            /* 位置误差的 P/D 修正：误差为正表示线路在右侧，左轮加速、右轮减速。 */
            float derivative = (line_error - s_previous_line_error) / dt_s;
            s_line_integral += line_error * dt_s;
            if (s_line_integral > LINE_INTEGRAL_LIMIT)
                s_line_integral = LINE_INTEGRAL_LIMIT;
            if (s_line_integral < -LINE_INTEGRAL_LIMIT)
                s_line_integral = -LINE_INTEGRAL_LIMIT;
            correction = LINE_PID_KP * line_error +
                LINE_PID_KI * s_line_integral + LINE_PID_KD * derivative;
            if (correction > LINE_CORRECTION_LIMIT_MM_S)
                correction = LINE_CORRECTION_LIMIT_MM_S;
            if (correction < -LINE_CORRECTION_LIMIT_MM_S)
                correction = -LINE_CORRECTION_LIMIT_MM_S;
            s_previous_line_error = line_error;
        } else {
            s_line_integral = 0.0f;
            s_previous_line_error = line_error;
        }
        updateWheelOutput(dt_s, TARGET_SPEED_MM_S + correction,
            TARGET_SPEED_MM_S - correction);
        if (elapsed_ms >= PID_DEMO_STAGE_MS)
            enterState(DEMO_MISSION_DONE, now_tick, yaw_deg);
        return;
    }

    if (isStraightState(s_state)) {
        heading_error = s_heading_reference_deg - yaw_deg;
        correction = HEADING_KP * heading_error - HEADING_KD * yaw_rate_deg_s;
        if (correction > HEADING_CORRECTION_LIMIT_MM_S)
            correction = HEADING_CORRECTION_LIMIT_MM_S;
        if (correction < -HEADING_CORRECTION_LIMIT_MM_S)
            correction = -HEADING_CORRECTION_LIMIT_MM_S;
        updateWheelOutput(dt_s, TARGET_SPEED_MM_S - correction,
            TARGET_SPEED_MM_S + correction);
        if (elapsed_ms >= STRAIGHT_STAGE_MS) {
            if (s_state == DEMO_MISSION_STRAIGHT_1)
                enterState(DEMO_MISSION_COAST_TO_TURN_1, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_STRAIGHT_2)
                enterState(DEMO_MISSION_COAST_TO_TURN_2, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_STRAIGHT_3)
                enterState(DEMO_MISSION_COAST_TO_TURN_3, now_tick, yaw_deg);
            else
                enterState(DEMO_MISSION_DONE, now_tick, yaw_deg);
        }
        return;
    }

    if (isCoastState(s_state)) {
        BspMotor_Coast();
        if (elapsed_ms >= COAST_STAGE_MS) {
            if (s_state == DEMO_MISSION_COAST_TO_TURN_1)
                enterState(DEMO_MISSION_TURN_1, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_COAST_TO_TURN_2)
                enterState(DEMO_MISSION_TURN_2, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_COAST_TO_TURN_3)
                enterState(DEMO_MISSION_TURN_3, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_COAST_TO_STRAIGHT_2)
                enterState(DEMO_MISSION_STRAIGHT_2, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_COAST_TO_STRAIGHT_3)
                enterState(DEMO_MISSION_STRAIGHT_3, now_tick, yaw_deg);
            else
                enterState(DEMO_MISSION_STRAIGHT_4, now_tick, yaw_deg);
        }
        return;
    }

    if (isTurnState(s_state)) {
        turn_progress = RIGHT_TURN_YAW_SIGN * (yaw_deg - s_turn_start_yaw_deg);
        turn_rate = RIGHT_TURN_YAW_SIGN * yaw_rate_deg_s;
        turn_error = TURN_TARGET_DEG - turn_progress;
        s_turn_error_deg = turn_error;
        s_turn_integral += turn_error * dt_s;
        if (s_turn_integral > TURN_INTEGRAL_LIMIT)
            s_turn_integral = TURN_INTEGRAL_LIMIT;
        if (s_turn_integral < -TURN_INTEGRAL_LIMIT)
            s_turn_integral = -TURN_INTEGRAL_LIMIT;
        turn_speed = TURN_ANGLE_KP * turn_error +
            TURN_ANGLE_KI * s_turn_integral - TURN_ANGLE_KD * turn_rate;
        if (turn_speed > TURN_MAX_SPEED_MM_S)
            turn_speed = TURN_MAX_SPEED_MM_S;
        if (turn_speed < -TURN_MAX_SPEED_MM_S)
            turn_speed = -TURN_MAX_SPEED_MM_S;
        if (fabsf(turn_error) > TURN_COMPLETE_TOLERANCE_DEG &&
            fabsf(turn_speed) < TURN_MIN_SPEED_MM_S) {
            turn_speed = (turn_error > 0.0f) ?
                TURN_MIN_SPEED_MM_S : -TURN_MIN_SPEED_MM_S;
        }
        updateWheelOutput(dt_s, turn_speed, -turn_speed);
        if (elapsed_ms >= TURN_MIN_RUNTIME_MS &&
            fabsf(turn_error) <= TURN_COMPLETE_TOLERANCE_DEG &&
            fabsf(turn_rate) <= TURN_RATE_TOLERANCE_DEG_S) {
            if (s_state == DEMO_MISSION_TURN_1)
                enterState(DEMO_MISSION_COAST_TO_STRAIGHT_2, now_tick, yaw_deg);
            else if (s_state == DEMO_MISSION_TURN_2)
                enterState(DEMO_MISSION_COAST_TO_STRAIGHT_3, now_tick, yaw_deg);
            else
                enterState(DEMO_MISSION_COAST_TO_STRAIGHT_4, now_tick, yaw_deg);
        } else if (elapsed_ms >= TURN_TIMEOUT_MS) {
            s_state = DEMO_MISSION_ABORTED;
            WheelSpeedController_Reset(&s_wheel_controller);
            BspMotor_Coast();
        }
        return;
    }

    BspMotor_Coast();
}

void DemoMission_GetSnapshot(uint32_t now_tick, float yaw_deg,
    float yaw_rate_deg_s, bool gyro_ready, DemoMissionSnapshot *snapshot)
{
    if (snapshot == 0) return;
    snapshot->state = s_state;
    snapshot->elapsed_ms = (now_tick - s_mission_start_tick) * CONTROL_PERIOD_MS;
    snapshot->yaw_deg = yaw_deg;
    snapshot->target_yaw_deg = s_target_yaw_deg;
    snapshot->yaw_rate_deg_s = yaw_rate_deg_s;
    snapshot->turn_error_deg = s_turn_error_deg;
    snapshot->gyro_ready = gyro_ready;
    snapshot->gyro_required = s_gyro_required;
    snapshot->gyro_backend = BspJy61sUart_GetBackendName();
    snapshot->line_error = s_line_error;
    snapshot->line_valid = s_line_valid;
    snapshot->gray_bits = s_gray_bits;
    snapshot->speed_a_mm_s = s_speed_a_mm_s;
    snapshot->speed_b_mm_s = s_speed_b_mm_s;
    snapshot->command_a_permille = BspMotor_GetACommand();
    snapshot->command_b_permille = BspMotor_GetBCommand();
}
