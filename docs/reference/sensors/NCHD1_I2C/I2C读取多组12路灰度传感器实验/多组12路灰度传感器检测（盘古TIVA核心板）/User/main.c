#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "debug.h"
#include "fpu.h"
#include "gpio.h"
#include "pin_map.h"
#include "rom.h"
#include "sysctl.h"
#include "uart.h"
#include "uartstdio.h"
#include "SystickTime.h"
#include "oled.h"
#include "gray_detection.h"
#include "soft_i2c.h"
#include "nchd12.h"


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的串口终端软件可以观察结果
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", "自主寻迹12路灰度传感器检测");	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", "V1.0");		  /* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", "20240825");	/* 打印例程日期 */
	printf("* 标准库版本 : TM4C123GH6PZT7\r\n");
	printf("* \r\n");	/* 打印一行空格 */
	printf("* QQ    : 3138372165 \r\n");
	printf("* Email : 3138372165@qq.com \r\n");
	printf("* Copyright www.nameless.tech 无名创新\r\n");
	printf("*************************************************************\n\r");
}



void ConfigureUART(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);// Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);// Enable UART0
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);// Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Use the internal 16MHz oscillator as the UART clock source.
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
		UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
}



int fputc(int ch, FILE *f){UARTCharPut(UART0_BASE,ch);	return (ch);}
int fgetc(FILE *f) {int ch=UARTCharGet(UART0_BASE);	return (ch);}
int main(void)
{ 
		ROM_FPUEnable();//使能浮点单元
		ROM_FPULazyStackingEnable();//浮点延迟堆栈,减少中断响应延迟	
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |	SYSCTL_OSC_MAIN);//配置系统时钟
	  initTime();//初始化滴答定时器
	  i2c_CheckDevice(0x40);//灰度传感器IIC初始化
		//pca9555_set_input_mode_all(0x40,0);//初始化P0为输入端口,非必须
		//pca9555_set_input_mode_all(0x40,1);//初始化P1为输入端口,非必须
		delay_ms(500);
    ConfigureUART();//初始化串口0
    PrintfLogo();//串口打印版本信息
	  OLED_Init();//OLED显示屏初始化
    while(1)
    {
			gray_state_multi[0].state=pca9555_read_bit12(0x20<<1);//读取第一组12路灰度传感器数据,A0,A1,A2接低电平,对应地址0x40
			gray_state_multi[1].state=pca9555_read_bit12(0x21<<1);//读取第二组12路灰度传感器数据,A0接高电平,A1,A2接低电平,对应地址0x42
			gray_state_multi[2].state=pca9555_read_bit12(0x22<<1);//读取第三组12路灰度传感器数据,A1接高电平,A0,A2接低电平,对应地址0x44
			gray_state_multi[3].state=pca9555_read_bit12(0x23<<1);//读取第四组12路灰度传感器数据,A0,A1接高电平,A2接低电平,对应地址0x46
			
			Testime t0;
			Test_Period(&t0);
			LCD_clear_L(0,0);display_6_8_string(0,0,"System  Time:     MS");display_6_8_number(80,0,t0.Time_Delta_INT);
			LCD_clear_L(0,1);display_6_8_number(0,1,t0.Now_Time);
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
				//检测结果打印	
				printf("group %d:%d%d%d%d%d%d%d%d%d%d%d%d \t %d \t \n",i, 
							gray_state_multi[i].gray.bit12,
							gray_state_multi[i].gray.bit11,
							gray_state_multi[i].gray.bit10,
							gray_state_multi[i].gray.bit9,
							gray_state_multi[i].gray.bit8,
							gray_state_multi[i].gray.bit7,
							gray_state_multi[i].gray.bit6,
							gray_state_multi[i].gray.bit5,
							gray_state_multi[i].gray.bit4,
							gray_state_multi[i].gray.bit3,
							gray_state_multi[i].gray.bit2,
							gray_state_multi[i].gray.bit1,
							gray_state_multi[i].state);	
			}	
    }
}
