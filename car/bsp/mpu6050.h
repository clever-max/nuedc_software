#ifndef CAR_BSP_MPU6050_H
#define CAR_BSP_MPU6050_H

#include <stdbool.h>

bool BspMpu6050_Init(void);
bool BspMpu6050_Update(float dt_s);
void BspMpu6050_ZeroYaw(void);
bool BspMpu6050_IsReady(void);
const char *BspMpu6050_GetBackendName(void);
float BspMpu6050_GetYawDeg(void);
float BspMpu6050_GetYawRateDegS(void);

#endif
