#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "soft_i2c.h"
#include "nchd12.h"
#include "gray_detection.h"



int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	i2c_CheckDevice(0x40);//灰度传感器IIC初始化
	while(1)
	{
		LCD_clear_L(0,0);display_6_8_string(0,0,"System  Time:  MS");
		LCD_clear_L(0,1);display_6_8_number(0,1,0);           display_6_8_string(70,1,"10mm");
																													display_6_8_string(100,1,"20mm");	
		LCD_clear_L(0,2);display_6_8_string(0,2,"gray_state");display_6_8_number(70,2,gray_status[0]);
																													display_6_8_number(100,2,gray_status[1]);
		LCD_clear_L(0,3);
		display_6_8_number(0,3,gray_state.gray.bit12);
		display_6_8_number(10,3,gray_state.gray.bit11);
		display_6_8_number(20,3,gray_state.gray.bit10);
		display_6_8_number(30,3,gray_state.gray.bit9);
		display_6_8_number(40,3,gray_state.gray.bit8);
		display_6_8_number(50,3,gray_state.gray.bit7);
		display_6_8_number(60,3,gray_state.gray.bit6);
		display_6_8_number(70,3,gray_state.gray.bit5);
		display_6_8_number(80,3,gray_state.gray.bit4);
		display_6_8_number(90,3,gray_state.gray.bit3);
		display_6_8_number(100,3,gray_state.gray.bit2);
		display_6_8_number(110,3,gray_state.gray.bit1);
		LCD_clear_L(0,4);display_6_8_string(0,4,"left           right");
		
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PIN);//IO电平翻转
		gray_state.state=pca9555_read_bit12(0x20<<1);//通过I2C方式读取12路灰度传感器数据
		gpio_input_check_channel_12_linewidth_10mm();//针对10mm引导线的偏差提取
		gpio_input_check_channel_12_linewidth_20mm();//针对20mm引导线的偏差提取

	}
}
