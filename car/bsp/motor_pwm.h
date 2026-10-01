#ifndef CAR_BSP_MOTOR_PWM_H
#define CAR_BSP_MOTOR_PWM_H
#include <stdint.h>
void BspMotor_Init(void);
void BspMotor_SetCommand(int16_t motorA_permille, int16_t motorB_permille);
void BspMotor_Coast(void);
int16_t BspMotor_GetACommand(void);
int16_t BspMotor_GetBCommand(void);
#endif
