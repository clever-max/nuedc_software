#ifndef CAR_BSP_ENCODER_H
#define CAR_BSP_ENCODER_H
#include <stdint.h>
void BspEncoder_Reset(void);
void BspEncoder_GetCounts(int32_t *a, int32_t *b);
void BspEncoder_UpdateMeasurements(float dt, int32_t *speedA, int32_t *speedB, int32_t *posA, int32_t *posB);
void BspEncoder_IRQHandler(void);
#endif
