#ifndef CAR_DEMO_MISSION_H
#define CAR_DEMO_MISSION_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DEMO_MISSION_IDLE = 0,
    DEMO_MISSION_RUNNING,
    DEMO_MISSION_DONE,
    DEMO_MISSION_ABORTED
} DemoMissionState;

typedef struct {
    DemoMissionState state;
    uint32_t elapsed_ms;
    int32_t ticks_a;
    int32_t ticks_b;
    int32_t speed_a_mm_s;
    int32_t speed_b_mm_s;
    int32_t position_a_mm;
    int32_t position_b_mm;
    int16_t command_a_permille;
    int16_t command_b_permille;
} DemoMissionSnapshot;

void DemoMission_Init(void);
bool DemoMission_CanStart(void);
bool DemoMission_IsRunning(void);
bool DemoMission_Start(uint32_t now_tick);
void DemoMission_RequestStop(void);
void DemoMission_Update(uint32_t now_tick,
    int32_t speed_a_mm_s, int32_t speed_b_mm_s,
    int32_t position_a_mm, int32_t position_b_mm);
void DemoMission_GetSnapshot(uint32_t now_tick, DemoMissionSnapshot *snapshot);

#endif
