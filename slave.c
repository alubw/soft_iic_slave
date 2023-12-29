#include "slave.h"


uint8_t I2C_Slave_Add = 0;
uint8_t I2C_Slave_num = 0;
uint8_t RX_Num = 0;
uint8_t TX_Num = 0;
uint8_t RX_Add = 0;
uint8_t xdata RX_Buf[10];
uint8_t xdata TX_Buf[10];
uint8_t flag = 0;
uint8_t WorR_flag = 0; //��д��־λ
uint8_t read_area = 0;

void INT0_Isr() interrupt 0//SCL
{
  if (P32) //����                                   //�ж������غ��½���
  {
    if (flag == 1) //��ʼ����
    {
      read_area <<= 1;
      read_area |= I2C_Read_SDA();
      I2C_Slave_num++;
      if (I2C_Slave_num == 8) //�������ַ�Ͷ�дλ��8λ
      {
        RX_Add = read_area;
        read_area = 0;
      }
    }
    else if (flag == 2) //�ظ�ack
    {
      if (WorR_flag) //��   (���ⷢ����)
      {
        read_area = TX_Buf[TX_Num++];
      }
      flag = 3; //
    }
    else if (flag == 4) //��ַ������ɿ�ʼ�������� д
    {
      read_area <<= 1;
      read_area |= I2C_Read_SDA();
      I2C_Slave_num++;
      if (I2C_Slave_num == 8) //�������ַ�Ͷ�дλ��8λ
      {
        RX_Buf[RX_Num++] = read_area;
        if (RX_Num >= 10) ///���
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
      if (!I2C_Read_SDA()) //���յ�Ӧ��ack
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
  else//�½�
  {
    if (flag == 1)
    {
      if (I2C_Slave_num == 8) //�������ַ�Ͷ�дλ��8λ
      {
        I2C_Slave_Add =  RX_Add >> 1; //ȡ��7λ��ַ
        if (I2C_Address == I2C_Slave_Add) //�Ա�7λ��ַ
        {
          WorR_flag =  RX_Add & 0x01; //ȡ�ö�д��־ 0д  1��
          I2C_Write_SDA (0); //����sda����ack
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
      if (WorR_flag) //��   (���ⷢ����)
      {
        //I2C_Write_SDA (1); //��������sda����ack
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
      else//д  (������)
      {
        I2C_Write_SDA (1); //��������sda����ack
        flag = 4;
      }
    }
    else if (flag == 5)
    {
      I2C_Write_SDA (0); //����sda����ack
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
  if (P33) //����
  {
    if (I2C_Read_SCL()) //�����ź�
    {
      flag = 0;
    }
  }
  else//�½�
  {
    if (flag == 0)
    {
      if (I2C_Read_SCL())
      {
        flag = 1; //��⵽��ʼ�ź�
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
 * @brief дSDA
 * @param H_L:�ߵ͵�ƽ
 * @return ��
 * @author HZ12138
 * @date 2022-10-21 18:07:18
 */
void I2C_Write_SDA (uint8_t H_L)
{
  P33 = H_L;
}
/**
 * @brief дSCL
 * @param H_L:�ߵ͵�ƽ
 * @return ��
 * @author HZ12138
 * @date 2022-10-21 18:07:40
 */
void I2C_Write_SCL (uint8_t H_L)
{
  P32 = H_L;
}
/**
 * @brief ��ȡSDA
 * @param ��
 * @return SDA��״̬
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint8_t I2C_Read_SDA (void)
{
  return P33;
}
/**
 * @brief ��ȡSCL
 * @param ��
 * @return SDA��״̬
 * @author HZ12138
 * @date 2022-10-21 18:07:56
 */
uint8_t I2C_Read_SCL (void)
{
  return P32;
}

