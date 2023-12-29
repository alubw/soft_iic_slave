#include "slave.h"


uint8_t I2C_Slave_Add = 0;
uint8_t I2C_Slave_num = 0;
uint8_t RX_Num = 0;
uint8_t TX_Num = 0;
uint8_t RX_Add = 0;
uint8_t xdata RX_Buf[10];
uint8_t xdata TX_Buf[10];
uint8_t flag = 0;
uint8_t WorR_flag = 0; //读写标志位
uint8_t read_area = 0;

void INT0_Isr() interrupt 0//SCL
{
  if (P32) //上升                                   //判断上升沿和下降沿
  {
    if (flag == 1) //开始接收
    {
      read_area <<= 1;
      read_area |= I2C_Read_SDA();
      I2C_Slave_num++;
      if (I2C_Slave_num == 8) //接收完地址和读写位共8位
      {
        RX_Add = read_area;
        read_area = 0;
      }
    }
    else if (flag == 2) //回复ack
    {
      if (WorR_flag) //读   (往外发数据)
      {
        read_area = TX_Buf[TX_Num++];
      }
      flag = 3; //
    }
    else if (flag == 4) //地址接收完成开始接收数据 写
    {
      read_area <<= 1;
      read_area |= I2C_Read_SDA();
      I2C_Slave_num++;
      if (I2C_Slave_num == 8) //接收完地址和读写位共8位
      {
        RX_Buf[RX_Num++] = read_area;
        if (RX_Num >= 10) ///溢出
        {
          RX_Num = 0;
        }
        flag = 5;
        read_area = 0;
        I2C_Slave_num = 0;
      }
    }
    else if (flag == 5)
    {
      flag = 4;
    }
    else if (flag == 7)
    {
      if (!I2C_Read_SDA()) //接收到应答ack
      {
        read_area = TX_Buf[TX_Num++];
        if (TX_Num >= 10)
        {
          TX_Num = 0;
          read_area = 0;
        }
        flag = 3;
      }
      else//nack
      {
        I2C_Write_SDA (1);
        TX_Num = 0;
        read_area = 0;
        //
      }
    }
  }
  else//下降
  {
    if (flag == 1)
    {
      if (I2C_Slave_num == 8) //接收完地址和读写位共8位
      {
        I2C_Slave_Add =  RX_Add >> 1; //取得7位地址
        if (I2C_Address == I2C_Slave_Add) //对比7位地址
        {
          WorR_flag =  RX_Add & 0x01; //取得读写标志 0写  1读
          I2C_Write_SDA (0); //拉低sda发送ack
          flag = 2;
          I2C_Slave_num = 0;
        }
        else
        {
          flag = 0;
          read_area = 0;
          I2C_Slave_num = 0;
        }
        read_area = 0;
        I2C_Slave_num = 0;
      }
    }
    else if (flag == 3)
    {
      if (WorR_flag) //读   (往外发数据)
      {
        //I2C_Write_SDA (1); //结束拉低sda发送ack
        if (read_area & 0x80)
          I2C_Write_SDA (1);
        else
          I2C_Write_SDA (0);
        read_area <<= 1;
        I2C_Slave_num++;

        if (I2C_Slave_num == 8)
        {
          flag = 6;
          I2C_Slave_num = 0;
        }
      }
      else//写  (收数据)
      {
        I2C_Write_SDA (1); //结束拉低sda发送ack
        flag = 4;
      }
    }
    else if (flag == 5)
    {
      I2C_Write_SDA (0); //拉低sda发送ack
      flag = 3;
    }
    else if (flag == 6)
    {
      flag = 7;
      I2C_Write_SDA (1);
    }
  }
}


void INT1_Isr() interrupt 2//SDA
{
  if (P33) //上升
  {
    if (I2C_Read_SCL()) //结束信号
    {
      flag = 0;
    }
  }
  else//下降
  {
    if (flag == 0)
    {
      if (I2C_Read_SCL())
      {
        flag = 1; //检测到开始信号
        RX_Num = 0;
        TX_Num = 0;
        read_area = 0;
        I2C_Slave_num = 0;
      }
      else
      {
        //RX_Buf[8] &= ~0x01;
      }
    }
  }
}




/**
 * @brief 写SDA
 * @param H_L:高低电平
 * @return 无
 * @author HZ12138
 * @date 2022-10-21 18:07:18
 */
void I2C_Write_SDA (uint8_t H_L)
{
  P33 = H_L;
}
/**
 * @brief 写SCL
 * @param H_L:高低电平
 * @return 无
 * @author HZ12138
 * @date 2022-10-21 18:07:40
 */
void I2C_Write_SCL (uint8_t H_L)
{
  P32 = H_L;
}
/**
 * @brief 读取SDA
 * @param 无
 * @return SDA的状态
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint8_t I2C_Read_SDA (void)
{
  return P33;
}
/**
 * @brief 读取SCL
 * @param 无
 * @return SDA的状态
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint8_t I2C_Read_SCL (void)
{
  return P32;
}

