#include "ti_msp_dl_config.h"
#include "motor_pwm.h"

#define PWM_PERIOD_COUNTS         (3200U)
#define PWM_MAX_PERMILLE          (1000U)
#define MOTOR_A_FORWARD_AIN1_HIGH (1)
#define MOTOR_B_FORWARD_BIN1_HIGH (1)
#define MOTOR_PWM_AIN1_INDEX      (DL_TIMER_CC_1_INDEX)
#define MOTOR_PWM_AIN2_INDEX      (DL_TIMER_CC_0_INDEX)
#define MOTOR_PWM_BIN1_INDEX      (DL_TIMER_CC_0_INDEX)
#define MOTOR_PWM_BIN2_INDEX      (DL_TIMER_CC_1_INDEX)

static int16_t s_motorACommand = 0;
static int16_t s_motorBCommand = 0;

static uint32_t dutyPermilleToCompare(uint32_t dutyPermille)
{
    if (dutyPermille > PWM_MAX_PERMILLE) dutyPermille = PWM_MAX_PERMILLE;
    return dutyPermille * PWM_PERIOD_COUNTS / PWM_MAX_PERMILLE;
}

static void setMotorAInputs(uint32_t ain1Permille, uint32_t ain2Permille)
{
    DL_Timer_setCaptureCompareValue(MOTOR_A_PWM_INST,
        dutyPermilleToCompare(ain1Permille), MOTOR_PWM_AIN1_INDEX);
    DL_Timer_setCaptureCompareValue(MOTOR_A_PWM_INST,
        dutyPermilleToCompare(ain2Permille), MOTOR_PWM_AIN2_INDEX);
}

static void setMotorBInputs(uint32_t bin1Permille, uint32_t bin2Permille)
{
    DL_Timer_setCaptureCompareValue(MOTOR_B_PWM_INST,
        dutyPermilleToCompare(bin1Permille), MOTOR_PWM_BIN1_INDEX);
    DL_Timer_setCaptureCompareValue(MOTOR_B_PWM_INST,
        dutyPermilleToCompare(bin2Permille), MOTOR_PWM_BIN2_INDEX);
}

static uint32_t magnitude(int16_t command)
{
    return (uint32_t)(command < 0 ? -(int32_t)command : command);
}

static void setMotorACommand(int16_t command)
{
    uint32_t variable = PWM_MAX_PERMILLE - magnitude(command);
    if (command == 0U) {
        setMotorAInputs(0U, 0U);
    } else if ((command > 0) == (MOTOR_A_FORWARD_AIN1_HIGH != 0)) {
        setMotorAInputs(PWM_MAX_PERMILLE, variable);
    } else {
        setMotorAInputs(variable, PWM_MAX_PERMILLE);
    }
}

static void setMotorBCommand(int16_t command)
{
    uint32_t variable = PWM_MAX_PERMILLE - magnitude(command);
    if (command == 0U) {
        setMotorBInputs(0U, 0U);
    } else if ((command > 0) == (MOTOR_B_FORWARD_BIN1_HIGH != 0)) {
        setMotorBInputs(PWM_MAX_PERMILLE, variable);
    } else {
        setMotorBInputs(variable, PWM_MAX_PERMILLE);
    }
}

void BspMotor_Init(void)
{
    BspMotor_Coast();
}

void BspMotor_Coast(void)
{
    s_motorACommand = 0;
    s_motorBCommand = 0;
    setMotorAInputs(0U, 0U);
    setMotorBInputs(0U, 0U);
}

void BspMotor_SetCommand(int16_t motorA_permille, int16_t motorB_permille)
{
    if (motorA_permille > (int16_t)PWM_MAX_PERMILLE) motorA_permille = PWM_MAX_PERMILLE;
    if (motorA_permille < -(int16_t)PWM_MAX_PERMILLE) motorA_permille = -(int16_t)PWM_MAX_PERMILLE;
    if (motorB_permille > (int16_t)PWM_MAX_PERMILLE) motorB_permille = PWM_MAX_PERMILLE;
    if (motorB_permille < -(int16_t)PWM_MAX_PERMILLE) motorB_permille = -(int16_t)PWM_MAX_PERMILLE;

    s_motorACommand = motorA_permille;
    s_motorBCommand = motorB_permille;
    setMotorACommand(motorA_permille);
    setMotorBCommand(motorB_permille);
}

int16_t BspMotor_GetACommand(void) { return s_motorACommand; }
int16_t BspMotor_GetBCommand(void) { return s_motorBCommand; }
