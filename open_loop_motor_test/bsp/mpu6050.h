#ifndef CAR_BSP_MPU6050_H
#define CAR_BSP_MPU6050_H

/* 原始 MPU6050 专用接口：PB2/PB3 为 I2C1，PB1 为可选 INT。 */
#include <stdbool.h>

bool BspMpu6050_Init(void);
bool BspMpu6050_Update(float dt_s);
void BspMpu6050_ZeroYaw(void);
bool BspMpu6050_IsReady(void);
/* 返回固定后端名 MPU6050，探测失败时返回 MPU6050-ERR。 */
const char *BspMpu6050_GetBackendName(void);
float BspMpu6050_GetYawDeg(void);
float BspMpu6050_GetYawRateDegS(void);

#endif
