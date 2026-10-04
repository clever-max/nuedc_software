#include "ti/driverlib/dl_adc12.h"
#include "ti_msp_dl_config.h"
#include "stdio.h"

void uart0_send_string(char* str);//串口发送字符串
unsigned int adc_getValue(void);//读取ADC的数据

int main(void)
{
    char output_buff[50] = {0};
    unsigned int adc_value = 0;
    float voltage_value = 0;

    SYSCFG_DL_init();

    uart0_send_string("adc Demo start\r\n");
    while (1)
    {
        //获取ADC数据
        adc_value = adc_getValue();
        sprintf(output_buff, "adc value:%d\r\n", adc_value);
        uart0_send_string(output_buff);

        //将ADC采集的数据换算为电压
        voltage_value = adc_value/4095.0*3.3;
        sprintf(output_buff, "voltage value:%.2f\r\n", voltage_value);
        uart0_send_string(output_buff);

        delay_cycles(32000000);
    }
}

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
//读取ADC的数据
unsigned int adc_getValue(void)
{
    unsigned int gAdcResult = 0;

    //使能ADC转换
    DL_ADC12_enableConversions(ADC_VOLTAGE_INST);
    //软件触发ADC开始转换
    DL_ADC12_startConversion(ADC_VOLTAGE_INST);

    //如果当前状态 不是 空闲状态
    while (DL_ADC12_getStatus(ADC_VOLTAGE_INST) != DL_ADC12_STATUS_CONVERSION_IDLE );

    //清除触发转换状态
    DL_ADC12_stopConversion(ADC_VOLTAGE_INST);
    //失能ADC转换
    DL_ADC12_disableConversions(ADC_VOLTAGE_INST);

    //获取数据
    gAdcResult = DL_ADC12_getMemResult(ADC_VOLTAGE_INST, ADC_VOLTAGE_ADCMEM_ADC_CH0);

    return gAdcResult;
}