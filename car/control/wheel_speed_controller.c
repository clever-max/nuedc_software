#include "wheel_speed_controller.h"

#include <math.h>
#include <stddef.h>

#define SPEED_CONTROLLER_PERIOD_S (0.020f)

/* 左右轮各自维护一套增量 PID，输入为编码器换算的 mm/s，输出为 PWM 千分比。 */
static float clampf(float value, float low, float high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static float clampTargetSpeed(float value)
{
    return clampf(value, -WHEEL_SPEED_MAX_MM_S, WHEEL_SPEED_MAX_MM_S);
}

static void resetPid(WheelSpeedPid *pid)
{
    /* 重新进入一个运动阶段时，用前馈值作为输出初值，避免从零突跳。 */
    pid->output_permille = pid->target_mm_s * pid->feedforward_permille_per_mm_s;
    pid->error_1 = 0.0f;
    pid->error_2 = 0.0f;
}

static int16_t updatePid(WheelSpeedPid *pid, float measured_mm_s,
    float dt_s, float limit)
{
    float error;
    float delta;

    /* 零目标时清空历史，确保停止后积分/微分不会残留。 */
    if (fabsf(pid->target_mm_s) < 0.5f || dt_s <= 0.0f) {
        resetPid(pid);
        return 0;
    }

    error = pid->target_mm_s - measured_mm_s;
    /* 增量式 PID：只计算本周期输出变化，再做总输出限幅。 */
    delta = pid->kp * (error - pid->error_1) +
        pid->ki * dt_s * error +
        (pid->kd / dt_s) * (error - 2.0f * pid->error_1 + pid->error_2);
    pid->output_permille = clampf(pid->output_permille + delta, -limit, limit);
    pid->error_2 = pid->error_1;
    pid->error_1 = error;
    return (int16_t)pid->output_permille;
}

void WheelSpeedController_Init(DualWheelSpeedController *controller,
    float kp, float ki, float kd, float feedforward_permille_per_mm_s,
    float max_output_permille)
{
    if (controller == NULL) return;
    controller->max_output_permille = clampf(max_output_permille, 0.0f, 1000.0f);
    controller->motor_a.kp = kp;
    controller->motor_a.ki = ki;
    controller->motor_a.kd = kd;
    controller->motor_a.feedforward_permille_per_mm_s = feedforward_permille_per_mm_s;
    controller->motor_b.kp = kp;
    controller->motor_b.ki = ki;
    controller->motor_b.kd = kd;
    controller->motor_b.feedforward_permille_per_mm_s = feedforward_permille_per_mm_s;
    controller->motor_a.target_mm_s = 0.0f;
    controller->motor_b.target_mm_s = 0.0f;
    resetPid(&controller->motor_a);
    resetPid(&controller->motor_b);
    controller->update_accumulator_s = 0.0f;
    controller->last_output_a_permille = 0;
    controller->last_output_b_permille = 0;
}

void WheelSpeedController_SetTargets(DualWheelSpeedController *controller,
    float motor_a_mm_s, float motor_b_mm_s)
{
    float target_a;
    float target_b;
    if (controller == NULL) return;
    target_a = clampTargetSpeed(motor_a_mm_s);
    target_b = clampTargetSpeed(motor_b_mm_s);
    /* 目标变化时同步移动前馈基线，避免沿用前一阶段的目标状态。 */
    controller->motor_a.output_permille +=
        controller->motor_a.feedforward_permille_per_mm_s *
        (target_a - controller->motor_a.target_mm_s);
    controller->motor_b.output_permille +=
        controller->motor_b.feedforward_permille_per_mm_s *
        (target_b - controller->motor_b.target_mm_s);
    controller->motor_a.target_mm_s = target_a;
    controller->motor_b.target_mm_s = target_b;
}

void WheelSpeedController_Update(DualWheelSpeedController *controller,
    float measured_a_mm_s, float measured_b_mm_s, float dt_s,
    int16_t *output_a_permille, int16_t *output_b_permille)
{
    if (controller == NULL || output_a_permille == NULL || output_b_permille == NULL) return;
    if (dt_s > 0.0f) controller->update_accumulator_s += dt_s;
    if (controller->update_accumulator_s < SPEED_CONTROLLER_PERIOD_S) {
        *output_a_permille = controller->last_output_a_permille;
        *output_b_permille = controller->last_output_b_permille;
        return;
    }
    dt_s = controller->update_accumulator_s;
    controller->update_accumulator_s = 0.0f;
    controller->last_output_a_permille = updatePid(&controller->motor_a,
        measured_a_mm_s, dt_s, controller->max_output_permille);
    controller->last_output_b_permille = updatePid(&controller->motor_b,
        measured_b_mm_s, dt_s, controller->max_output_permille);
    *output_a_permille = controller->last_output_a_permille;
    *output_b_permille = controller->last_output_b_permille;
}

void WheelSpeedController_Reset(DualWheelSpeedController *controller)
{
    if (controller == NULL) return;
    resetPid(&controller->motor_a);
    resetPid(&controller->motor_b);
    controller->update_accumulator_s = 0.0f;
    controller->last_output_a_permille = 0;
    controller->last_output_b_permille = 0;
}
