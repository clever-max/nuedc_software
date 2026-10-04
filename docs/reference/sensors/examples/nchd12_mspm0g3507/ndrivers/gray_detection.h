#ifndef __GRAY_DETECTION_H
#define __GRAY_DETECTION_H



typedef struct
{
	uint8_t bit1	:1;
	uint8_t bit2	:1;
	uint8_t bit3	:1;
	uint8_t bit4	:1;
	uint8_t bit5	:1;
	uint8_t bit6	:1;
	uint8_t bit7	:1;
	uint8_t bit8	:1;
	uint8_t bit9	:1;
	uint8_t bit10	:1;
	uint8_t bit11	:1;
	uint8_t bit12	:1;
	uint8_t bit13	:1;
	uint8_t bit14	:1;
	uint8_t bit15	:1;
	uint8_t bit16	:1;
}gray_flags;


typedef union
{
	uint16_t state;
	gray_flags gray;
}_gray_state; 


void gpio_input_init(void);
void gpio_input_check_channel_12_linewidth_10mm(void);
void gpio_input_check_channel_12_linewidth_20mm(void);


extern float gray_status[2],gray_status_backup[2][20];
extern uint32_t gray_status_worse;
extern _gray_state gray_state; 

#endif



//		case 0x0001:gray_status[1]=-11; break;									//000000000001b
//		case 0x0003:gray_status[1]=-10; break;									//000000000011b
//		case 0x0007:gray_status[1]=-9;  break;									//000000000111b
//		case 0x0006:gray_status[1]=-8;  break;									//000000000110b
//		case 0x000E:gray_status[1]=-7;  break;									//000000001110b
//		case 0x000C:gray_status[1]=-6;  break;									//000000001100b
//		case 0x001C:gray_status[1]=-5;  break;									//000000011100b
//		case 0x0018:gray_status[1]=-4;  break;									//000000011000b
//		case 0x0038:gray_status[1]=-3;  break;									//000000111000b
//		case 0x0030:gray_status[1]=-2;  break;									//000000110000b
//		case 0x0070:gray_status[1]=-1;  break;									//000001110000b
//		case 0x0060:gray_status[1]= 0;  break;									//000001100000b
//		case 0x00E0:gray_status[1]= 1;  break;									//000011100000b
//		case 0x00C0:gray_status[1]= 2;  break;									//000011000000b
//		case 0x01C0:gray_status[1]= 3;  break;									//000111000000b
//		case 0x0180:gray_status[1]= 4;  break;									//000110000000b
//		case 0x0380:gray_status[1]= 5;  break;									//001110000000b
//		case 0x0300:gray_status[1]= 6;  break;									//001100000000b
//		case 0x0700:gray_status[1]= 7;  break;									//011100000000b
//		case 0x0600:gray_status[1]= 8;  break;									//011000000000b
//		case 0x0E00:gray_status[1]= 9;  break;									//111000000000b
//		case 0x0C00:gray_status[1]= 10; break;									//110000000000b
//		case 0x0800:gray_status[1]= 11; break;									//100000000000b
















