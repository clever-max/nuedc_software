#ifndef CAR_BSP_GRAY_SENSOR_H
#define CAR_BSP_GRAY_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    /* bits 为原始 12 路状态，position 为线路位置，valid 表示本次读取成功且有线。 */
    uint16_t bits;
    float position;
    bool valid;
} GraySensorSample;

void BspGraySensor_Init(void);
bool BspGraySensor_Read(GraySensorSample *sample);

#endif
