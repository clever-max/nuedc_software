#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>


#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */


void _bsp_analog_i2c_start(void);
void _bsp_analog_i2c_stop(void);
void _bsp_analog_i2c_nack(void);
void _bsp_analog_i2c_ack(void);
uint8_t _bsp_analog_i2c_wait_ack(void);
uint8_t _bsp_analog_i2c_read_byte(void);
void _bsp_analog_i2c_send_byte(uint8_t data);
void _bsp_analog_i2c_send_byte_nask(uint8_t data);

uint8_t i2c_CheckDevice(uint8_t _Address);


	
#endif


