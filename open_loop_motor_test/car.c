#include "app/app.h"
#include "ti_msp_dl_config.h"

/* 主入口只负责调用应用层；外设初始化、任务调度和控制逻辑都放在模块中。 */
int main(void)
{
    App_Init();
    for (;;) App_RunOnce();
}

void GROUP1_IRQHandler(void)
{
    /* GPIOA/GPIOB 共用 Group1，中断源由编码器 BSP 读取并清除。 */
    App_OnEncoderInterrupt();
}

void DEBUG_UART_INST_IRQHandler(void)
{
    /* UART1 是外置 CH340 的电脑调试命令和遥测通道。 */
    App_OnUartInterrupt();
}

void CONTROL_TICK_INST_IRQHandler(void)
{
    /* 5 ms 定时器 ISR 只记账，耗时控制在前台循环中执行。 */
    App_OnControlTickInterrupt();
}
