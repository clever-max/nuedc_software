#include "demo_mission.h"

#include "bsp/encoder.h"
#include "bsp/motor_pwm.h"

#define CONTROL_PERIOD_MS (5U)
#define DEMO_DURATION_MS (15000U)
#define DEMO_RAMP_MS (500U)
#define MOTOR_A_PWM_PERMILLE (400U)
#define MOTOR_B_PWM_PERMILLE (190U)

static DemoMissionState s_state = DEMO_MISSION_IDLE;
static uint32_t s_start_tick;
static bool s_stop_requested;
static int32_t s_speed_a_mm_s, s_speed_b_mm_s;
static int32_t s_position_a_mm, s_position_b_mm;

void DemoMission_Init(void)
{
    s_state = DEMO_MISSION_IDLE;
    s_start_tick = 0U;
    s_stop_requested = false;
    s_speed_a_mm_s = s_speed_b_mm_s = 0;
    s_position_a_mm = s_position_b_mm = 0;
    BspMotor_Coast();
}

bool DemoMission_CanStart(void)
{
    return s_state == DEMO_MISSION_IDLE ||
        s_state == DEMO_MISSION_DONE || s_state == DEMO_MISSION_ABORTED;
}

bool DemoMission_IsRunning(void)
{
    return s_state == DEMO_MISSION_RUNNING;
}

bool DemoMission_Start(uint32_t now_tick)
{
    if (!DemoMission_CanStart()) return false;
    BspEncoder_Reset();
    s_speed_a_mm_s = s_speed_b_mm_s = 0;
    s_position_a_mm = s_position_b_mm = 0;
    s_start_tick = now_tick;
    s_stop_requested = false;
    s_state = DEMO_MISSION_RUNNING;
    BspMotor_Coast();
    return true;
}

void DemoMission_RequestStop(void)
{
    if (s_state == DEMO_MISSION_RUNNING) s_stop_requested = true;
    else BspMotor_Coast();
}

void DemoMission_Update(uint32_t now_tick,
    int32_t speed_a_mm_s, int32_t speed_b_mm_s,
    int32_t position_a_mm, int32_t position_b_mm)
{
    uint32_t elapsed_ms;
    uint32_t ramp_permille;
    s_speed_a_mm_s = speed_a_mm_s;
    s_speed_b_mm_s = speed_b_mm_s;
    s_position_a_mm = position_a_mm;
    s_position_b_mm = position_b_mm;

    if (s_state != DEMO_MISSION_RUNNING) {
        BspMotor_Coast();
        return;
    }

    elapsed_ms = (now_tick - s_start_tick) * CONTROL_PERIOD_MS;
    if (s_stop_requested) {
        s_stop_requested = false;
        s_state = DEMO_MISSION_ABORTED;
        BspMotor_Coast();
    } else if (elapsed_ms >= DEMO_DURATION_MS) {
        s_state = DEMO_MISSION_DONE;
        BspMotor_Coast();
    } else {
        ramp_permille = elapsed_ms < DEMO_RAMP_MS ?
            elapsed_ms * 1000U / DEMO_RAMP_MS : 1000U;
        BspMotor_SetCommand(
            (int16_t)(MOTOR_A_PWM_PERMILLE * ramp_permille / 1000U),
            (int16_t)(MOTOR_B_PWM_PERMILLE * ramp_permille / 1000U));
    }
}

void DemoMission_GetSnapshot(uint32_t now_tick, DemoMissionSnapshot *snapshot)
{
    if (snapshot == 0) return;
    snapshot->state = s_state;
    snapshot->elapsed_ms = (now_tick - s_start_tick) * CONTROL_PERIOD_MS;
    BspEncoder_GetCounts(&snapshot->ticks_a, &snapshot->ticks_b);
    snapshot->speed_a_mm_s = s_speed_a_mm_s;
    snapshot->speed_b_mm_s = s_speed_b_mm_s;
    snapshot->position_a_mm = s_position_a_mm;
    snapshot->position_b_mm = s_position_b_mm;
    snapshot->command_a_permille = BspMotor_GetACommand();
    snapshot->command_b_permille = BspMotor_GetBCommand();
}
