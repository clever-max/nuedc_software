#ifndef CAR_DEMO_MISSION_H
#define CAR_DEMO_MISSION_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DEMO_MISSION_IDLE = 0,
    DEMO_MISSION_CURVE_1,
    DEMO_MISSION_STRAIGHT_TRACK,
    DEMO_MISSION_CURVE_2,
    DEMO_MISSION_LINE_ONLY,
    DEMO_MISSION_DONE,
    DEMO_MISSION_ABORTED
} DemoMissionState;

typedef enum {
    DEMO_LINE_TRACK = 0,
    DEMO_LINE_TURN_LEFT,
    DEMO_LINE_TURN_RIGHT,
    DEMO_LINE_CROSS_PASS,
    DEMO_LINE_LOST
} DemoLineMode;

typedef struct {
    DemoMissionState state;
    uint32_t elapsed_ms;
    float yaw_deg;
    float target_yaw_deg;
    float yaw_rate_deg_s;
    float turn_error_deg;
    bool gyro_ready;
    bool gyro_required;
    const char *gyro_backend;
    float line_error;
    bool line_valid;
    DemoLineMode line_mode;
    uint16_t gray_bits;
    bool gray_bus_ok;
    uint8_t gray_bus_stage;
    uint8_t gray_write_address;
    int32_t speed_a_mm_s;
    int32_t speed_b_mm_s;
    int16_t command_a_permille;
    int16_t command_b_permille;
} DemoMissionSnapshot;

void DemoMission_Init(bool gyro_ready);
bool DemoMission_CanStart(void);
bool DemoMission_IsRunning(void);
bool DemoMission_Start(uint32_t now_tick, float yaw_deg);
bool DemoMission_StartLineOnly(uint32_t now_tick);
void DemoMission_RequestStop(void);
void DemoMission_Update(uint32_t now_tick, float dt_s,
    int32_t speed_a_mm_s, int32_t speed_b_mm_s,
    float yaw_deg, float yaw_rate_deg_s, bool gyro_ready,
    float line_error, bool line_valid, uint16_t gray_bits,
    bool gray_bus_ok, uint8_t gray_bus_stage, uint8_t gray_write_address);
void DemoMission_GetSnapshot(uint32_t now_tick, float yaw_deg,
    float yaw_rate_deg_s, bool gyro_ready, DemoMissionSnapshot *snapshot);

#endif
