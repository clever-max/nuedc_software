#ifndef CAR_BSP_ENCODER_H
#define CAR_BSP_ENCODER_H
#include <stdint.h>
/* 编码器 BSP：A 相上升沿计数、B 相判向，并换算轮缘线速度。 */
void BspEncoder_Reset(void);
void BspEncoder_GetCounts(int32_t *a, int32_t *b);
void BspEncoder_UpdateMeasurements(float dt, int32_t *speedA, int32_t *speedB, int32_t *posA, int32_t *posB);
void BspEncoder_IRQHandler(void);
#endif
