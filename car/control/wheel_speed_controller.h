#ifndef CAR_WHEEL_SPEED_CONTROLLER_H
#define CAR_WHEEL_SPEED_CONTROLLER_H

#include <stdint.h>

/* 所有任务的轮缘目标速度最终都不能超过此值。 */
#define WHEEL_SPEED_MAX_MM_S (360.0f)

typedef struct {
    float kp;
    float ki;
    float kd;
    float feedforward_permille_per_mm_s;
    float target_mm_s;
    float output_permille;
    float error_1;
    float error_2;
} WheelSpeedPid;

typedef struct {
    WheelSpeedPid motor_a;
    WheelSpeedPid motor_b;
    float max_output_permille;
    float update_accumulator_s;
    int16_t last_output_a_permille;
    int16_t last_output_b_permille;
} DualWheelSpeedController;

/* 增益、前馈和 PWM 限幅由任务层提供；本模块不隐藏硬件标定参数。 */
void WheelSpeedController_Init(DualWheelSpeedController *controller,
    float kp, float ki, float kd, float feedforward_permille_per_mm_s,
    float max_output_permille);
void WheelSpeedController_SetTargets(DualWheelSpeedController *controller,
    float motor_a_mm_s, float motor_b_mm_s);
void WheelSpeedController_Update(DualWheelSpeedController *controller,
    float measured_a_mm_s, float measured_b_mm_s, float dt_s,
    int16_t *output_a_permille, int16_t *output_b_permille);
void WheelSpeedController_Reset(DualWheelSpeedController *controller);

#endif
