#ifndef CAR_BSP_GRAY_SENSOR_H
#define CAR_BSP_GRAY_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    /* bits 为原始 12 路状态，position 为线路位置，valid 表示本次读取成功且有线。 */
    uint16_t bits;
    float position;
    bool valid;
    /* bus_ok 用于区分“总线读通但当前没有黑线”和“I²C 没有应答”。 */
    bool bus_ok;
    /* 0=未探测，1=地址 NACK，2=寄存器 NACK，3=读地址 NACK，4=数据阶段。 */
    uint8_t bus_stage;
    /* PCA9555 兼容 8 位写地址，默认 0x40；支持地址焊盘改动后的扫描结果。 */
    uint8_t write_address;
} GraySensorSample;

void BspGraySensor_Init(void);
bool BspGraySensor_Read(GraySensorSample *sample);

#endif
