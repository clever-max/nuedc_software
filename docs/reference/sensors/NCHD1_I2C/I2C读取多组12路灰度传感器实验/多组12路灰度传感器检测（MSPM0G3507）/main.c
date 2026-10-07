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
			gray_state_multi[0].state=pca9555_read_bit12(0x20<<1);//读取第一组12路灰度传感器数据,A0,A1,A2接低电平,对应地址0x40
			gray_state_multi[1].state=pca9555_read_bit12(0x21<<1);//读取第二组12路灰度传感器数据,A0接高电平,A1,A2接低电平,对应地址0x42
			gray_state_multi[2].state=pca9555_read_bit12(0x22<<1);//读取第三组12路灰度传感器数据,A1接高电平,A0,A2接低电平,对应地址0x44
			gray_state_multi[3].state=pca9555_read_bit12(0x23<<1);//读取第四组12路灰度传感器数据,A0,A1接高电平,A2接低电平,对应地址0x46
			
			LCD_clear_L(0,0);display_6_8_string(0,0,"System  Time:     MS");display_6_8_number(80,0,0);
			LCD_clear_L(0,1);display_6_8_number(0,1,0);
			LCD_clear_L(0,2);display_6_8_string(0,2,"left           right");
			for(uint8_t i=0;i<4;i++)
			{
				LCD_clear_L(0,3+i);
				display_6_8_number(0  ,3+i,gray_state_multi[i].gray.bit12);	  display_6_8_number(10 ,3+i,gray_state_multi[i].gray.bit11);
				display_6_8_number(20 ,3+i,gray_state_multi[i].gray.bit10);	  display_6_8_number(30 ,3+i,gray_state_multi[i].gray.bit9);
				display_6_8_number(40 ,3+i,gray_state_multi[i].gray.bit8);	  display_6_8_number(50 ,3+i,gray_state_multi[i].gray.bit7);
				display_6_8_number(60 ,3+i,gray_state_multi[i].gray.bit6);	  display_6_8_number(70 ,3+i,gray_state_multi[i].gray.bit5);
				display_6_8_number(80 ,3+i,gray_state_multi[i].gray.bit4);	  display_6_8_number(90 ,3+i,gray_state_multi[i].gray.bit3);
				display_6_8_number(100,3+i,gray_state_multi[i].gray.bit2);	  display_6_8_number(110,3+i,gray_state_multi[i].gray.bit1);
			}
	}
}
