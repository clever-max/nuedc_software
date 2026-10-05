#include "demo_mission.h"

#include "bsp/encoder.h"
#include "bsp/motor_pwm.h"
#include "bsp/mpu6050.h"
#include "control/wheel_speed_controller.h"

#include <math.h>

/*
 * 最小操场跑道验证：启动后立即进入第一段弯道，6050 记录首个约 180°
 * 航向变化；随后在线循迹通过直道，检测到第二段弯道后再记录约 180°，
 * 完成后停车。弯道方向由第一次有效 Z 轴角速度自动确定，因此不依赖
 * 6050 的安装正负方向。直道长度由弯道进入检测得到，暂不写死 100 m。
 */
#define CONTROL_PERIOD_MS             (5U)
#define ROUTE_TIMEOUT_MS              (900000U)
#define CURVE_TARGET_DEG              (170.0f)
#define CURVE_SPEED_MM_S              (110.0f)
#define STRAIGHT_SPEED_MM_S           (200.0f)
#define CURVE_START_RATE_DEG_S        (8.0f)
#define CURVE_ENTRY_CONFIRM_MS        (150U)
#define STRAIGHT_MIN_MS               (1000U)
#define LINE_ONLY_DURATION_MS         (30000U)

#define WHEEL_PID_KP                  (0.50f)
#define WHEEL_PID_KI                  (0.20f)
#define WHEEL_PID_KD                  (0.00f)
#define WHEEL_PID_FEED_FORWARD        (1.00f)
#define LINE_PID_KP                   (8.0f)
#define LINE_PID_KI                   (0.0f)
#define LINE_PID_KD                   (0.05f)
#define LINE_CORRECTION_LIMIT_MM_S    (100.0f)
#define LINE_INTEGRAL_LIMIT           (20.0f)

static DemoMissionState s_state = DEMO_MISSION_IDLE;
static DualWheelSpeedController s_wheel_controller;
static uint32_t s_mission_start_tick;
static uint32_t s_stage_start_tick;
static float s_curve_start_yaw;
static float s_target_yaw_deg;
static float s_yaw_rate_deg_s;
static float s_turn_error_deg;
static int8_t s_turn_sign;
static uint32_t s_curve_entry_ms;
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

static uint32_t elapsedStageMs(uint32_t now_tick)
{
    return (now_tick - s_stage_start_tick) * CONTROL_PERIOD_MS;
}

static void enterState(DemoMissionState state, uint32_t now_tick,
    float yaw_deg)
{
    s_state = state;
    s_stage_start_tick = now_tick;
    s_curve_entry_ms = 0U;
    s_turn_error_deg = 0.0f;
    WheelSpeedController_Reset(&s_wheel_controller);

    if (state == DEMO_MISSION_CURVE_1 || state == DEMO_MISSION_CURVE_2) {
        s_curve_start_yaw = yaw_deg;
        s_target_yaw_deg = yaw_deg +
            ((s_turn_sign >= 0) ? CURVE_TARGET_DEG : -CURVE_TARGET_DEG);
        s_turn_error_deg = CURVE_TARGET_DEG;
    } else if (state == DEMO_MISSION_STRAIGHT_TRACK) {
        s_target_yaw_deg = yaw_deg;
    } else if (state == DEMO_MISSION_LINE_ONLY) {
        s_target_yaw_deg = yaw_deg;
    } else {
        s_target_yaw_deg = yaw_deg;
        BspMotor_Coast();
    }
}

static float lineCorrection(float dt_s, float line_error, bool line_valid)
{
    float derivative;
    float correction;

    if (!line_valid || dt_s <= 0.0f) return 0.0f;
    derivative = (line_error - s_previous_line_error) / dt_s;
    s_line_integral += line_error * dt_s;
    if (s_line_integral > LINE_INTEGRAL_LIMIT)
        s_line_integral = LINE_INTEGRAL_LIMIT;
    if (s_line_integral < -LINE_INTEGRAL_LIMIT)
        s_line_integral = -LINE_INTEGRAL_LIMIT;
    correction = LINE_PID_KP * line_error + LINE_PID_KI * s_line_integral +
        LINE_PID_KD * derivative;
    if (correction > LINE_CORRECTION_LIMIT_MM_S)
        correction = LINE_CORRECTION_LIMIT_MM_S;
    if (correction < -LINE_CORRECTION_LIMIT_MM_S)
        correction = -LINE_CORRECTION_LIMIT_MM_S;
    s_previous_line_error = line_error;
    return correction;
}

static void updateWheelOutput(float dt_s, float base_speed_mm_s,
    float line_error, bool line_valid)
{
    int16_t command_a;
    int16_t command_b;
    float correction = lineCorrection(dt_s, line_error, line_valid);

    /* 灰度位置环改变左右轮目标，编码器速度环再生成 PWM。 */
    WheelSpeedController_SetTargets(&s_wheel_controller,
        base_speed_mm_s + correction, base_speed_mm_s - correction);
    WheelSpeedController_Update(&s_wheel_controller,
        (float)s_speed_a_mm_s, (float)s_speed_b_mm_s, dt_s,
        &command_a, &command_b);
    BspMotor_SetCommand(command_a, command_b);
}

static void resetMissionVariables(void)
{
    s_curve_start_yaw = 0.0f;
    s_target_yaw_deg = 0.0f;
    s_yaw_rate_deg_s = 0.0f;
    s_turn_error_deg = 0.0f;
    s_turn_sign = 0;
    s_curve_entry_ms = 0U;
    s_stop_requested = false;
    s_line_error = 0.0f;
    s_line_integral = 0.0f;
    s_line_valid = false;
    s_gray_bits = 0U;
    s_previous_line_error = 0.0f;
    s_speed_a_mm_s = 0;
    s_speed_b_mm_s = 0;
}

void DemoMission_Init(bool gyro_ready)
{
    s_state = DEMO_MISSION_IDLE;
    s_mission_start_tick = 0U;
    s_stage_start_tick = 0U;
    resetMissionVariables();
    s_gyro_ready = gyro_ready;
    s_gyro_required = gyro_ready;
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
    if (!DemoMission_CanStart() || !s_gyro_ready) return false;
    BspEncoder_Reset();
    resetMissionVariables();
    s_mission_start_tick = now_tick;
    s_stage_start_tick = now_tick;
    s_gyro_ready = true;
    enterState(DEMO_MISSION_CURVE_1, now_tick, yaw_deg);
    BspMotor_Coast();
    return true;
}

bool DemoMission_StartLineOnly(uint32_t now_tick)
{
    if (!DemoMission_CanStart()) return false;
    BspEncoder_Reset();
    resetMissionVariables();
    s_mission_start_tick = now_tick;
    s_stage_start_tick = now_tick;
    s_gyro_required = false;
    s_gyro_ready = false;
    enterState(DEMO_MISSION_LINE_ONLY, now_tick, 0.0f);
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
    float signed_rate;
    float progress;

    s_speed_a_mm_s = speed_a_mm_s;
    s_speed_b_mm_s = speed_b_mm_s;
    s_yaw_rate_deg_s = yaw_rate_deg_s;
    s_gyro_ready = gyro_ready;
    s_line_error = line_error;
    s_line_valid = line_valid;
    s_gray_bits = gray_bits;

    /* 空闲、完成或中止状态持续写入零 PWM，防止外设复位后的残留输出。 */
    if (s_state == DEMO_MISSION_IDLE || s_state == DEMO_MISSION_DONE ||
        s_state == DEMO_MISSION_ABORTED) {
        BspMotor_Coast();
        return;
    }

    if (s_stop_requested || (s_gyro_required && !s_gyro_ready) ||
        (now_tick - s_mission_start_tick) * CONTROL_PERIOD_MS >=
            ROUTE_TIMEOUT_MS) {
        s_stop_requested = false;
        s_state = DEMO_MISSION_ABORTED;
        WheelSpeedController_Reset(&s_wheel_controller);
        BspMotor_Coast();
        return;
    }

    elapsed_ms = elapsedStageMs(now_tick);
    if (s_state == DEMO_MISSION_CURVE_1) {
        updateWheelOutput(dt_s, CURVE_SPEED_MM_S, line_error, line_valid);
        if (s_turn_sign == 0 &&
            fabsf(yaw_rate_deg_s) >= CURVE_START_RATE_DEG_S) {
            s_turn_sign = (yaw_rate_deg_s >= 0.0f) ? 1 : -1;
            s_target_yaw_deg = s_curve_start_yaw +
                (float)s_turn_sign * CURVE_TARGET_DEG;
        }
        if (s_turn_sign != 0) {
            progress = (float)s_turn_sign * (yaw_deg - s_curve_start_yaw);
            s_turn_error_deg = CURVE_TARGET_DEG - progress;
            if (progress >= CURVE_TARGET_DEG) {
                enterState(DEMO_MISSION_STRAIGHT_TRACK, now_tick, yaw_deg);
            }
        }
        return;
    }

    if (s_state == DEMO_MISSION_STRAIGHT_TRACK) {
        updateWheelOutput(dt_s, STRAIGHT_SPEED_MM_S, line_error, line_valid);
        /* 直道至少运行一秒，再用角速度确认第二个弯道已经开始。 */
        signed_rate = (s_turn_sign == 0) ? 0.0f :
            (float)s_turn_sign * yaw_rate_deg_s;
        if (elapsed_ms >= STRAIGHT_MIN_MS &&
            signed_rate >= CURVE_START_RATE_DEG_S) {
            s_curve_entry_ms += CONTROL_PERIOD_MS;
            if (s_curve_entry_ms >= CURVE_ENTRY_CONFIRM_MS) {
                enterState(DEMO_MISSION_CURVE_2, now_tick, yaw_deg);
            }
        } else {
            s_curve_entry_ms = 0U;
        }
        return;
    }

    if (s_state == DEMO_MISSION_CURVE_2) {
        updateWheelOutput(dt_s, CURVE_SPEED_MM_S, line_error, line_valid);
        progress = (s_turn_sign == 0) ? 0.0f :
            (float)s_turn_sign * (yaw_deg - s_curve_start_yaw);
        s_turn_error_deg = CURVE_TARGET_DEG - progress;
        if (s_turn_sign != 0 && progress >= CURVE_TARGET_DEG) {
            enterState(DEMO_MISSION_DONE, now_tick, yaw_deg);
        }
        return;
    }

    if (s_state == DEMO_MISSION_LINE_ONLY) {
        updateWheelOutput(dt_s, STRAIGHT_SPEED_MM_S, line_error, line_valid);
        if (elapsed_ms >= LINE_ONLY_DURATION_MS) {
            s_state = DEMO_MISSION_DONE;
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
    snapshot->gyro_backend = BspMpu6050_GetBackendName();
    snapshot->line_error = s_line_error;
    snapshot->line_valid = s_line_valid;
    snapshot->gray_bits = s_gray_bits;
    snapshot->speed_a_mm_s = s_speed_a_mm_s;
    snapshot->speed_b_mm_s = s_speed_b_mm_s;
    snapshot->command_a_permille = BspMotor_GetACommand();
    snapshot->command_b_permille = BspMotor_GetBCommand();
}
