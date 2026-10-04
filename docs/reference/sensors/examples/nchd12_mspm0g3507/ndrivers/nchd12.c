#include "soft_i2c.h"
#include "nchd12.h"


uint16_t pcf8575_read_bit12(uint8_t slave_num)
{ 
	uint8_t	hdata,ldata;
	uint16_t bit12;
	_bsp_analog_i2c_start();
	_bsp_analog_i2c_send_byte_nask(slave_num | HOST_READ_COMMAND);
	uint8_t ack=_bsp_analog_i2c_wait_ack();
	ldata=_bsp_analog_i2c_read_byte();
	_bsp_analog_i2c_ack();
	hdata=_bsp_analog_i2c_read_byte();
	_bsp_analog_i2c_nack();
	_bsp_analog_i2c_stop();
	bit12 = (uint16_t)(hdata<<8 | ldata)&0x0fff;
	return bit12;
}

uint16_t pca9555_read_bit12(uint8_t slave_num)
{ 
	uint8_t	hdata,ldata;
	uint16_t bit12;
	_bsp_analog_i2c_start();
	_bsp_analog_i2c_send_byte_nask(slave_num);			//	写入从机地址
	_bsp_analog_i2c_wait_ack();
	_bsp_analog_i2c_send_byte_nask(INPUT_PORT_REGISTER0); //写入要读取的寄存器地址
	_bsp_analog_i2c_wait_ack();
	_bsp_analog_i2c_start(); /* 开始接收数据 */
	_bsp_analog_i2c_send_byte_nask(slave_num  | HOST_READ_COMMAND);			   //发送从机地址并设置为读取
	_bsp_analog_i2c_wait_ack();
	ldata=_bsp_analog_i2c_read_byte();
	_bsp_analog_i2c_ack();
	hdata=_bsp_analog_i2c_read_byte();
	_bsp_analog_i2c_nack();
	_bsp_analog_i2c_stop();
	bit12 = (uint16_t)(hdata<<8 | ldata)&0x0fff;
	return bit12;
}

