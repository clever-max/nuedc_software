#include "app/app.h"

int main(void)
{
    App_Init();
    for (;;) App_RunOnce();
}

void GROUP1_IRQHandler(void)
{
    App_OnEncoderInterrupt();
}

void DEBUG_UART_INST_IRQHandler(void)
{
    App_OnUartInterrupt();
}

void CONTROL_TICK_INST_IRQHandler(void)
{
    App_OnControlTickInterrupt();
}
