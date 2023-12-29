#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "public_host_iic.h"
#include "delay.h"
#include "math.h"

#define I2C_Address 0x33//0x66
#define IIC_READ_SDA P33
#define IIC_READ_SCL P32

extern uint8_t RX_Num;
extern uint8_t TX_Num;
extern uint8_t I2C_Add;
extern uint8_t xdata RX_Buf[10];
extern uint8_t xdata TX_Buf[10];

void slaver_wait_start();
uint8_t slaver_read_data();

void I2C_Write_SDA(uint8_t H_L);
void I2C_Write_SCL(uint8_t H_L);
uint8_t I2C_Read_SDA(void);
uint8_t I2C_Read_SCL(void);


#endif