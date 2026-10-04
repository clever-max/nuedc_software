#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "bsp_w25q128.h"

#define delay_ms(X)	delay_cycles((CPUCLK_FREQ/1000)*(X))

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
    char uart_output_buff[50]={0};
    unsigned char read_write_buff[10] = {0};

    SYSCFG_DL_init();

    delay_ms(1000);//等待器件部署

    //读取W25Q128的ID
    sprintf(uart_output_buff,"ID = %X\r\n",W25Q128_readID());
    uart0_send_string(uart_output_buff);

    //读取0地址的5个字节数据到buff
    W25Q128_read(read_write_buff, 0, 5);
    //串口输出读取的数据
    sprintf(uart_output_buff, "read_write_buff = %s\r\n",read_write_buff);
    uart0_send_string(uart_output_buff);

    //往0地址写入5个字节长度的数据 lckfb
    W25Q128_write((uint8_t*)"lckfb", 0, 5);

    delay_ms(10);//等待写入完毕

    //读取0地址的5个字节数据到buff
    W25Q128_read(read_write_buff, 0, 5);

    //串口输出读取的数据
    sprintf(uart_output_buff, "read_write_buff = %s\r\n", read_write_buff);
    uart0_send_string(uart_output_buff);

    while (1) {
    }
}