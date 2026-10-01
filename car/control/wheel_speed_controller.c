#include "wheel_speed_controller.h"

#include <math.h>
#include <stddef.h>

static float clampf(float value, float low, float high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static void resetPid(WheelSpeedPid *pid)
{
    pid->output_permille = 0.0f;
    pid->error_1 = 0.0f;
    pid->error_2 = 0.0f;
}

static int16_t updatePid(WheelSpeedPid *pid, float measured_mm_s,
    float dt_s, float limit)
{
    float error;
    float delta;

    if (fabsf(pid->target_mm_s) < 0.5f || dt_s <= 0.0f) {
        resetPid(pid);
        return 0;
    }

    error = pid->target_mm_s - measured_mm_s;
    delta = pid->kp * (error - pid->error_1) +
        pid->ki * dt_s * error +
        (pid->kd / dt_s) * (error - 2.0f * pid->error_1 + pid->error_2);
    pid->output_permille = clampf(pid->output_permille + delta, -limit, limit);
    pid->error_2 = pid->error_1;
    pid->error_1 = error;
    return (int16_t)pid->output_permille;
}

void WheelSpeedController_Init(DualWheelSpeedController *controller,
    float kp, float ki, float kd, float max_output_permille)
{
    if (controller == NULL) return;
    controller->max_output_permille = clampf(max_output_permille, 0.0f, 1000.0f);
    controller->motor_a.kp = kp;
    controller->motor_a.ki = ki;
    controller->motor_a.kd = kd;
    controller->motor_b.kp = kp;
    controller->motor_b.ki = ki;
    controller->motor_b.kd = kd;
    controller->motor_a.target_mm_s = 0.0f;
    controller->motor_b.target_mm_s = 0.0f;
    resetPid(&controller->motor_a);
    resetPid(&controller->motor_b);
}

void WheelSpeedController_SetTargets(DualWheelSpeedController *controller,
    float motor_a_mm_s, float motor_b_mm_s)
{
    if (controller == NULL) return;
    controller->motor_a.target_mm_s = motor_a_mm_s;
    controller->motor_b.target_mm_s = motor_b_mm_s;
}

void WheelSpeedController_Update(DualWheelSpeedController *controller,
    float measured_a_mm_s, float measured_b_mm_s, float dt_s,
    int16_t *output_a_permille, int16_t *output_b_permille)
{
    if (controller == NULL || output_a_permille == NULL || output_b_permille == NULL) return;
    *output_a_permille = updatePid(&controller->motor_a, measured_a_mm_s,
        dt_s, controller->max_output_permille);
    *output_b_permille = updatePid(&controller->motor_b, measured_b_mm_s,
        dt_s, controller->max_output_permille);
}

void WheelSpeedController_Reset(DualWheelSpeedController *controller)
{
    if (controller == NULL) return;
    resetPid(&controller->motor_a);
    resetPid(&controller->motor_b);
}
