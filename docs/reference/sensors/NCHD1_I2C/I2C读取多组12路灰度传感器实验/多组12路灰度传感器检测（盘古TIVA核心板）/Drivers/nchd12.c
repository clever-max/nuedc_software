#include "soft_i2c.h"
#include "nchd12.h"

uint16_t pcf8575_read_bit12(uint8_t slave_num)
{ 
	uint8_t	hdata,ldata;
	uint16_t bit12;
	i2c_Start();
	i2c_SendByte(slave_num | HOST_READ_COMMAND);
	uint8_t ack=i2c_WaitAck();
	ldata=i2c_ReadByte(1);
	hdata=i2c_ReadByte(0);
	i2c_Stop();
	bit12 = (uint16_t)(hdata<<8 | ldata)&0x0fff;
	return bit12;
}





//pca9555写寄存器值
uint8_t pca9555_write_byte(uint8_t addr, uint8_t command, uint8_t write_register_data)
{
    i2c_Start();
    i2c_SendByte(addr);	//写从机地址
    i2c_WaitAck();
    i2c_SendByte(command);	//写要写的寄存器地址
    i2c_WaitAck();
    i2c_SendByte(write_register_data);//写入数据
    i2c_WaitAck();
    i2c_Stop();
    return SUCCESS;
}

/*
 * pca9555读取寄存器值
 *
 * addr 读取地址
 * read_register_data 要读取的寄存器
 * read_data 读取数据存放地址
 *
 *
 * 返回值：读取成功返回SUCCESS  失败返回ERROR
 *
 * */
uint8_t pca9555_read_byte(uint8_t slave_num, uint8_t addr, uint8_t read_register_data, uint8_t *read_data)
{
    i2c_Start();
    i2c_SendByte(slave_num);			//	写入从机地址
    i2c_WaitAck();
    i2c_SendByte(read_register_data); //写入要读取的寄存器地址
    i2c_WaitAck();
    i2c_Start(); /* 开始接收数据 */
    i2c_SendByte(addr);			   //发送从机地址并设置为读取
    i2c_WaitAck();
    *read_data = i2c_ReadByte(0);
    i2c_Stop();
    return SUCCESS;
}


/*
 * 设置指定GPIO的模式
 *
 * slave_num  需要操作的从机设备
 * gpio_port  gpio端口  端口0/1
 * gpio_num   哪一个GPIO
 *
 * 返回值：void
 * */
void pca9555_set_output_mode(uint8_t slave_num, uint8_t gpio_port,  uint8_t gpio_num)
{
    uint8_t register_original_data = 0;
    if(gpio_port > 1 || gpio_num > 0x80)  return;
    if(gpio_port == 0)
    {
        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, CONFIG_PORT_REGISTER0, &register_original_data);//读取原来的设置
        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER0, register_original_data & (~gpio_num));//在不影响原来设置的情况下修改配置寄存器的设置
    }
    else if(gpio_port == 1)
    {
        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, CONFIG_PORT_REGISTER1, &register_original_data);
        pca9555_write_byte( slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER1, register_original_data & (~gpio_num));
    }
}

void pca9555_set_output_mode_all(uint8_t slave_num, uint8_t gpio_port)
{
    uint8_t register_original_data = 0x00;
    if(gpio_port > 1)  return;
    if(gpio_port == 0)
    {
        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER0, register_original_data);
    }
    else if(gpio_port == 1)
    {
        pca9555_write_byte( slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER1, register_original_data);
    }
}

/*
 * 设置GPIO为输入模式
 *
 * slave_num  需要操作的从机设备
 * gpio_port  gpio端口  端口0/1
 * gpio_num   哪一个GPIO
 *
 * 返回值：void
 **/
void pca9555_set_input_mode(uint8_t slave_num, uint8_t gpio_port,  uint8_t gpio_num)
{
    uint8_t register_original_data = 0;
    if(gpio_port > 1 || gpio_num > 0x80)  return;
    if(gpio_port == 0)
    {
        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, CONFIG_PORT_REGISTER0, &register_original_data);
        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER0, register_original_data | gpio_num);
    }
    else if(gpio_port == 1)
    {

        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, CONFIG_PORT_REGISTER1, &register_original_data);

        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER1, register_original_data | gpio_num);//参考的文章里给的代码这里使用的是&，但其实这里应该用|才对啊，不然另外7个全变0了
    }
}

void pca9555_set_input_mode_all(uint8_t slave_num, uint8_t gpio_port)
{
    uint8_t register_original_data = 0xff;
    if(gpio_port > 1 )  return;
    if(gpio_port == 0)
    {
        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER0, register_original_data);
    }
    else if(gpio_port == 1)
    {
        pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, CONFIG_PORT_REGISTER1, register_original_data);
    }
}

/*
 * 设置GPIO输出状态
 *
 * slave_num  需要操作的从机设备
 * gpio_port  gpio端口  端口0/1
 * gpio_num   哪一个GPIO
 * status     输出状态
 *
 * 返回值：void
 **/
void pca9555_set_gpio_output_status(uint8_t slave_num, uint8_t gpio_port,  uint8_t gpio_num, uint8_t status)
{
    uint8_t register_original_data = 0;
    if(gpio_port > 1 || gpio_num > 0x80)  return;
    if(gpio_port == 0)
    {
        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, OUTPUT_PORT_REGISTER0, &register_original_data);
        if(status == 1)
        {
            pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, OUTPUT_PORT_REGISTER0, register_original_data | gpio_num);
        }
        else
        {
            pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, OUTPUT_PORT_REGISTER0, register_original_data & (~gpio_num));
        }
    }
    else if(gpio_port == 1)
    {
        pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, OUTPUT_PORT_REGISTER1, &register_original_data);
        if(status == 1)
        {
            pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, OUTPUT_PORT_REGISTER1, register_original_data | gpio_num);
        }
        else
        {
            pca9555_write_byte(slave_num | HOST_WRITE_COMMAND, OUTPUT_PORT_REGISTER1, register_original_data & (~gpio_num));
        }
    }
}

/*
 * 获取GPIO状态
 *
 * slave_num  需要操作的从机设备
 * gpio_port  gpio端口  端口0/1
 * gpio_num   哪一个GPIO
 *
 * 返回值：GPIO状态
 **/
uint8_t dc1,dc2=0;
uint8_t pca9555_get_gpio_status(uint8_t slave_num, uint8_t gpio_port,  uint8_t gpio_num)
{
    uint8_t register_original_data = 0;
    uint8_t gpio_status = 0;
    if(gpio_port > 1 || gpio_num > 0x80)
    {
        return 2;
    }
    if(gpio_port == 0)
    {

        dc1=pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, INPUT_PORT_REGISTER0, &register_original_data);

    }
    else if(gpio_port == 1)
    {
        dc2=pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, INPUT_PORT_REGISTER1, &register_original_data);
    }
    switch(gpio_num)
    {
        case 0x01:  gpio_status = register_original_data & gpio_num;break;
        case 0x02:  gpio_status = (register_original_data & gpio_num) >> 1;break;
        case 0x04:  gpio_status = (register_original_data & gpio_num) >> 2;break;
        case 0x08:  gpio_status = (register_original_data & gpio_num) >> 3;break;
        case 0x10:  gpio_status = (register_original_data & gpio_num) >> 4;break;
        case 0x20:  gpio_status = (register_original_data & gpio_num) >> 5;break;
        case 0x40:  gpio_status = (register_original_data & gpio_num) >> 6;break;
        case 0x80:  gpio_status = (register_original_data & gpio_num) >> 7;break;
        default:  	;
    }
    return gpio_status;
}


uint8_t pca9555_get_port_value(uint8_t slave_num, uint8_t gpio_port)
{
    uint8_t register_original_data = 0;
    if(gpio_port == 0)
    {

        dc1=pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, INPUT_PORT_REGISTER0, &register_original_data);

    }
    else if(gpio_port == 1)
    {
        dc2=pca9555_read_byte(slave_num, slave_num  | HOST_READ_COMMAND, INPUT_PORT_REGISTER1, &register_original_data);
    }
    return register_original_data;
}

uint16_t pca9555_read_bit12(uint8_t slave_num)
{ 
	uint8_t	hdata,ldata;
	uint16_t bit12;
	i2c_Start();
	i2c_SendByte(slave_num);			//	写入从机地址
	i2c_WaitAck();
	i2c_SendByte(INPUT_PORT_REGISTER0); //写入要读取的寄存器地址
	i2c_WaitAck();
	i2c_Start(); /* 开始接收数据 */
	i2c_SendByte(slave_num  | HOST_READ_COMMAND);			   //发送从机地址并设置为读取
	i2c_WaitAck();
	ldata = i2c_ReadByte(1);
	hdata = i2c_ReadByte(0);
	i2c_Stop();
	bit12 = (uint16_t)(hdata<<8 | ldata)&0x0fff;
	return bit12;
}



