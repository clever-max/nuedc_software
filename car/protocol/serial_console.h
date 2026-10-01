#ifndef CAR_SERIAL_CONSOLE_H
#define CAR_SERIAL_CONSOLE_H

#include "mission/demo_mission.h"

typedef enum {
    SERIAL_COMMAND_NONE = 0,
    SERIAL_COMMAND_RUN15,
    SERIAL_COMMAND_STOP,
    SERIAL_COMMAND_UNKNOWN
} SerialCommand;

void SerialConsole_Init(void);
SerialCommand SerialConsole_PollCommand(void);
void SerialConsole_WriteText(const char *text);
void SerialConsole_PrintTelemetry(const DemoMissionSnapshot *snapshot);
void SerialConsole_IRQHandler(void);

#endif

