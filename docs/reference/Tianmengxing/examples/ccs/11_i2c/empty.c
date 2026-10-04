#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "bsp_sht20.h"

#define T_ADDR     0xf3   // 温度
#define PH_ADDR    0xf5   // 湿度

//串口发送字符串
void uart0_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
         //当串口0忙的时候等待，不忙的时候再发送传进来的字符
        while( DL_UART_isBusy(UART_0_INST) == true );
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        DL_UART_Main_transmitData(UART_0_INST, *str++);
    }
}

int main(void)
{
    char output_buff[100]={0};
    float TEMP, PH;
    //开发板初始化
    SYSCFG_DL_init();

    uart0_send_string("SHT20 Start!!\r\n");
    while(1)
    {
        TEMP = SHT20_Read(T_ADDR);  //获取温度数据
        PH   = SHT20_Read(PH_ADDR); //获取湿度数据

        //格式化字符串
        sprintf(output_buff, "温度 = %.2f ℃, 湿度 = %.0f %%RH\r\n", TEMP, PH);
        //向串口发送温湿度数据的情况
        uart0_send_string(output_buff);

        delay_cycles(CPUCLK_FREQ);//延时
    }
}