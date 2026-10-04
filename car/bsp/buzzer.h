#ifndef CAR_BSP_BUZZER_H
#define CAR_BSP_BUZZER_H

#include <stdint.h>

/* 蜂鸣器接口按 5 ms 控制 tick 非阻塞运行。 */
void BspBuzzer_Init(void);
void BspBuzzer_Start(uint16_t duration_ms);
void BspBuzzer_Update(void);

#endif
