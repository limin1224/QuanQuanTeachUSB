/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

main.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-03-07
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
"******             21IC DIY U��ѧϰ�� ֮USB����               ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                  �밴K1-K8�ֱ���в���                 ******\r\n",
"******          K1:Caps Lock��  K2:Num Lock��  K3:��ĸa��     ******\r\n",
"******          K4:����1��      K5:��Shift��   K6:��Ctrl��    ******\r\n",
"******  ע�⣺���ּ�������С���̣������Num Lock�������ּ�  ******\r\n",
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
 //��Ҫ���ص�8�ֽڱ���Ļ���
 //ͨ�������������Ķ��弰HID��;���ĵ�����֪Buf[0]��D0����Ctrl����
 //D1����Shift����D2����Alt����D3����GUI����Window������
 //D4����Ctrl��D5����Shift��D6����Alt��D7����GUI����
 //Buf[1]������ֵΪ0��Buf[2]~Buf[7]Ϊ��ֵ����������6����
 //��������������ͨ�����ֻ��4������˲��ᳬ��6����
 //����ʵ�ʵļ��̣����������̫��ʱ�������6���ֽڶ�Ϊ0xFF��
 //��ʾ���µļ�̫�࣬�޷���ȷ���ء�
 
 uint8 Buf[8]={0,0,0,0,0,0,0,0};
 //������Ҫ���ض��������������Ҫ����һ�����������浱ǰ��λ�á�
 uint8 i=2;
 
 //���ݲ�ͬ�İ����������뱨��
 if(KeyPress & KEY6) //���KEY6��ס
 {
  Buf[0]|=0x01;  //KEY6Ϊ��Ctrl����
 }
 if(KeyPress & KEY5) //���KEY5��ס
 {
  Buf[0]|=0x02;  //KEY5Ϊ��Shift����
 }
 if(KeyPress & KEY4) //���KEY4��ס
 {
  Buf[i]=0x59;  //KEY4Ϊ����С����1����
  i++;  //�л����¸�λ�á�
 }
 if(KeyPress & KEY3)  //���KEY3��ס
 {
  Buf[i]=0x04;  //KEY3Ϊ��ĸa����
  i++;  //�л����¸�λ�á�
 }
 if(KeyPress & KEY2)  //���KEY2��ס
 {
  Buf[i]=0x53;  //KEY8Ϊ����С���̹����л�����
  i++;  //�л����¸�λ�á�
 }
 if(KeyPress & KEY1)  //���KEY1��ס
 {
  Buf[i]=0x39;  //KEY1Ϊ��/Сд�л�����
 }

 //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ8�ֽڡ�
 UsbChipWriteEndpointBuffer(1,8,Buf);
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

