#ifndef CAR_APP_H
#define CAR_APP_H

void App_Init(void);
void App_RunOnce(void);
void App_OnControlTickInterrupt(void);
void App_OnUartInterrupt(void);
void App_OnEncoderInterrupt(void);

#endif
