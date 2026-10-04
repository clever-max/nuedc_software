#ifndef CAR_BSP_JY61S_UART_H
#define CAR_BSP_JY61S_UART_H

#include <stdbool.h>

/* JY61S UART2 驱动：接收中断解析帧，前台完成校准和 yaw 积分。 */
bool BspJy61sUart_Init(void);
bool BspJy61sUart_Update(float dt_s);
void BspJy61sUart_IRQHandler(void);
void BspJy61sUart_ZeroYaw(void);
bool BspJy61sUart_IsReady(void);
const char *BspJy61sUart_GetBackendName(void);
float BspJy61sUart_GetYawDeg(void);
float BspJy61sUart_GetYawRateDegS(void);

#endif
