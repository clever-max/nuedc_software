#include "ti_msp_dl_config.h"
#include "soft_i2c.h"




#define _i2c_read_sda()        ((NCHD12_PORT_PORT->DIN31_0 & NCHD12_PORT_SDA_PIN )>0 ? 0x01 : 0x00)
#define _i2c_sda_high()   		DL_GPIO_setPins(NCHD12_PORT_PORT,   NCHD12_PORT_SDA_PIN)
#define _i2c_sda_low()  			DL_GPIO_clearPins(NCHD12_PORT_PORT, NCHD12_PORT_SDA_PIN)
#define _i2c_scl_high()   		DL_GPIO_setPins(NCHD12_PORT_PORT,   NCHD12_PORT_SCL_PIN)
#define _i2c_scl_low()  			DL_GPIO_clearPins(NCHD12_PORT_PORT, NCHD12_PORT_SCL_PIN)


/**********************************************************************************************
 *名    称：static void I2C_GPIO_Configuration(void)
 *
 *参    数：无
 *
 *返 回 值：无 
 *
 *描    述：I2C IO口配置
 *********************************************************************************************/
static void I2C_GPIO_Configuration(void)
{	
	DL_GPIO_setPins(NCHD12_PORT_PORT,   NCHD12_PORT_SDA_PIN);
	DL_GPIO_setPins(NCHD12_PORT_PORT,   NCHD12_PORT_SCL_PIN);
  _bsp_analog_i2c_stop();
}


void _i2c_sda_out(void)
{
	DL_GPIO_initDigitalOutput(NCHD12_PORT_SDA_IOMUX);
	DL_GPIO_enableOutput(NCHD12_PORT_PORT, NCHD12_PORT_SDA_PIN);
}
void _i2c_sda_in(void)
{
	//DL_GPIO_initDigitalInput(NCHD12_PORT_SDA_IOMUX);
	DL_GPIO_initDigitalInputFeatures(NCHD12_PORT_SDA_IOMUX,DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}


static void _analog_i2c_delay()
{
	uint8_t i;

	/*　
	 	下面的时间是通过安富莱AX-Pro逻辑分析仪测试得到的。
		CPU主频72MHz时，在内部Flash运行, MDK工程不优化
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
        
    IAR工程编译效率高，不能设置为7
	*/
	for (i = 0; i < 10; i++);
}

void _bsp_analog_i2c_start(void)
{
    /*    _____
     *SDA      \_____________
     *    __________
     *SCL           \________
     */
		_i2c_sda_out();
    _i2c_sda_high();
	  _analog_i2c_delay();
    _i2c_scl_high();
    _analog_i2c_delay();
    _i2c_sda_low();
    _analog_i2c_delay();
    _i2c_scl_low();
    _analog_i2c_delay();
}


void _bsp_analog_i2c_stop(void)
{
    /*               _______
     *SDA __________/
     *          ____________
     *SCL _____/
     */
		_i2c_sda_out();
		_i2c_scl_low();//add
    _i2c_sda_low();
    _i2c_scl_high();
    _analog_i2c_delay();
    _i2c_sda_high();
    _analog_i2c_delay();
}


uint8_t _bsp_analog_i2c_wait_ack(void)
{
    uint32_t timeout = 0;
	  _i2c_scl_low();
	  _i2c_sda_high();
		_i2c_sda_in();
    _analog_i2c_delay();
    _i2c_scl_high();
    _analog_i2c_delay();
    while(_i2c_read_sda())
    {
        timeout++;
        if(timeout > 100)//2000
        {
					_bsp_analog_i2c_stop();
          return 1;
        }
    }
    _i2c_scl_low();
    _analog_i2c_delay();
    return 0;
}


void _bsp_analog_i2c_ack(void)
{
	/*           ____
	 *SCL ______/    \______
	 *    ____         _____
	 *SDA     \_______/
	 */
	_i2c_sda_out();
	_i2c_scl_low();
	_i2c_sda_high();
	_analog_i2c_delay();
	_i2c_sda_low();
	_analog_i2c_delay();
	_i2c_scl_high();
	_analog_i2c_delay();
	_i2c_scl_low();
	_analog_i2c_delay();
	_i2c_sda_high();
}



void _bsp_analog_i2c_nack(void)
{
    /*           ____
     *SCL ______/    \______
     *    __________________
     *SDA
     */
	  _i2c_scl_low();
		_i2c_sda_out();
    _i2c_sda_high();
    _analog_i2c_delay();
    _i2c_scl_high();
    _analog_i2c_delay();
    _i2c_scl_low();
    _analog_i2c_delay();
}


void _bsp_analog_i2c_send_byte(uint8_t data)
{
	uint8_t i;
	_i2c_sda_out();
	_i2c_scl_low();
	for(i = 0; i < 8; i++)
	{
		if((data & 0x80) >> 7)	_i2c_sda_high();
		else	_i2c_sda_low();
		data <<= 1;
		_analog_i2c_delay();
		_i2c_scl_high();
		_analog_i2c_delay();
		_i2c_scl_low();
	}
	_bsp_analog_i2c_wait_ack();
}

void _bsp_analog_i2c_send_byte_nask(uint8_t data)
{
	uint8_t i;
	_i2c_sda_out();
	_i2c_scl_low();
	for(i = 0; i < 8; i++)
	{
		if((data & 0x80) >> 7)	_i2c_sda_high();
		else	_i2c_sda_low();
		data <<= 1;
		_analog_i2c_delay();
		_i2c_scl_high();
		_analog_i2c_delay();
		_i2c_scl_low();
	}
}


uint8_t _bsp_analog_i2c_read_byte(void)
{
		_i2c_sda_high();
		_i2c_sda_in();
    uint8_t i, data = 0;
    for(i = 0; i < 8; i++ )
    {
        data <<= 1;
        _i2c_scl_high();
				_analog_i2c_delay();
        if(_i2c_read_sda())
        {
            data++;
        }
        _i2c_scl_low();
        _analog_i2c_delay();
    }
    return data;
}





uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	I2C_GPIO_Configuration();	
	_bsp_analog_i2c_start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	_bsp_analog_i2c_send_byte_nask(_Address | I2C_WR);
	ucAck = _bsp_analog_i2c_wait_ack();	/* 检测设备的ACK应答 */
	_bsp_analog_i2c_stop();			/* 发送停止信号 */
	return ucAck;
}





