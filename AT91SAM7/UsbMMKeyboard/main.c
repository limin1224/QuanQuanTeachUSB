/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

main.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-03-09
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <AT91SAM7S64.H>
#include "SystemClock.h"
#include "LED.h"
#include "LCD.h"
#include "ADC.H"
#include "Timer.h"
#include "key.h"
#include "uart.h"
#include "UsbCore.h"
#include "AT91SAMxUSB.h"

const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******            21IC DIY U��ѧϰ�� ֮USB��ý�����          ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                  �밴K1-K6�ֱ���в���                 ******\r\n",
"******              K1:������  K2:����        K3:һ������     ******\r\n",
"******              K6:������  K5:����/ֹͣ   K4:����         ******\r\n",
"******                                                        ******\r\n",
"********************************************************************\r\n",
};

/********************************************************************
�������ܣ����ݰ���������ر���ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendReport(void)
{
 //��Ҫ���ص�9�ֽڱ���Ļ��壨һ�ֽڱ���ID�Ӽ���8�ֽڱ��棩
 //���û������豸�ı���ֻ��1�ֽڣ�����һ�ֽڱ���ID�ܹ�2�ֽڣ�9�ֽڹ��ˡ�
 
 uint8 Buf[9]={0,0,0,0,0,0,0,0,0}; 
 
 //���ݲ�ͬ�İ����������뱨��
 
 if(0) //��ͨ���̹����ڴ˷���
 {
  Buf[0]=0x01;  //��һ�ֽ�Ϊ����ID�����̱���IDΪ1��
  UsbChipWriteEndpointBuffer(1,9,Buf);
 }
 else  //�û������豸��������
 {
  Buf[0]=0x02;  //��һ�ֽ�Ϊ����ID���û������豸����IDΪ2��
  if(KeyPress & KEY6) //���KEY6����
  {
   Buf[1]|=0x01;  //KEY6Ϊ�������͡�
  }
  if(KeyPress & KEY1) //���KEY1����
  {
   Buf[1]|=0x02;  //KEY1Ϊ����������
  }
  if(KeyPress & KEY2) //���KEY2����
  {
   Buf[1]|=0x04;  //KEY2Ϊ�������ء�
  }
  if(KeyPress & KEY5)  //���KEY5����
  {
   Buf[1]=0x08;  //KEY5Ϊ����/ֹͣ��
  }
  if(KeyPress & KEY3)  //���KEY3��ס
  {
   Buf[1]|=0x10;  //KEY3Ϊ����ҳ��
  }
  if(KeyPress & KEY4)  //���KEY4��ס
  {
   Buf[1]|=0x20;  //KEY4Ϊϵͳ������
  }
  //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ3�ֽڡ�
  UsbChipWriteEndpointBuffer(1,2,Buf);
 }
 Ep1InIsBusy=1;  //���ö˵�æ��־��
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���LCD����ʾ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void DispKey(void)
{
 int i;
 LcdCls();
 for(i=0;i<6;i++)
 {
  LcdSetPoint(0,8);
  if(KeyDown&(1<<i))
  {
   LcdPrints("Key ");
   LcdPutChar('1'+i);
   LcdPrints(" down");
  }
  LcdSetPoint(0,0);
  if(KeyUp&(1<<i))
  {
   LcdPrints("Key ");
   LcdPutChar('1'+i);
   LcdPrints(" up");
  }
 }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void main(void)
{
 int i;
 int InterruptSource;
 
 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��
 LedInit();         //LED��Ӧ�Ĺܽų�ʼ��
 LcdInit();         //LCD��ʼ��
 AdcInit();         //ADC��ʼ��
 Timer1Init();      //��ʱ��1��ʼ������������10ms�Ķ�ʱɨ���ź�
 KeyInit();         //���̳�ʼ��
 Uart0Init();       //����0��ʼ��
 
 for(i=0;i<19;i++)   //��ʾͷ��Ϣ
 {
  Prints(HeadTable[i]);
 }
 
 UsbChipInit();  //��ʼ��USB����
 
 while(1)
 {
  InterruptSource=(*AT91C_UDP_ISR)&(0x0F|(1<<8)|(1<<12)); //ȡ����Ҫ���ж�
  if(InterruptSource) //������ӵ��жϷ���
  {
   if(InterruptSource&(1<<8))
   {
    *AT91C_UDP_ICR=1<<8; //����ж�
    UsbBusSuspend(); //���߹����жϴ���
   }
   if(InterruptSource&(1<<12))
   {
    *AT91C_UDP_ICR=1<<12; //����ж�
    UsbBusReset();   //���߸�λ�жϴ���
   }
   if(InterruptSource&(1<<0))
   {
    if(AT91C_UDP_CSR[0]&((1<<1)|(1<<2)|(1<<6)))  //�����SETUP��������δ�յ�
    {
     UsbEp0Out();     //�˵�0����жϴ���
    }
    if(AT91C_UDP_CSR[0]&(1<<0)) //����Ƕ˵�0�������
    {
     UsbEp0In();     //�˵�0�����жϴ���
    }
   }
   if(InterruptSource&(1<<1))
   {
    UsbEp1In();      //�˵�1�����жϴ���
   }
   if(InterruptSource&(1<<2))
   {
    UsbEp2Out();     //�˵�2����жϴ���
   }
  }
  
  if(KeyUp||KeyDown)  //����û������˰���
  {
   DispKey(); //��LCD����ʾ�������
   if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Է��ر�������
   {
    if(!Ep1InIsBusy)  //����˵�1����û�д���æ״̬������Է�������
    {
     KeyCanChange=0;  //��ֹ����ɨ��
     if(KeyUp||KeyDown) //����а����¼�����
     {
      SendReport();  //�򷵻ر���
     }
     KeyCanChange=1;  //������ɨ��
    }
   }
   //���KeyUp��KeyDown
   KeyUp=0;
   KeyDown=0;
   LcdRefresh();  //ˢ��LCD��ʾ
  }
 }
}
/////////////////////////End of function/////////////////////////////

