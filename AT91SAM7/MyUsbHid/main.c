/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

main.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-03-10
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <AT91SAM7S64.H>
#include "SystemClock.h"
#include "uart.h"
#include "UsbCore.h"
#include "AT91SAMxUSB.h"
#include "config.h"
#include "adc.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "lcd.h"

#ifdef DEBUG0
const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******            21IC DIY U��ѧϰ�� ֮�û��Զ���HID�豸      ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******          ����λ��Ӧ�������Ȼ��K1-K6�ֱ���в���   ******\r\n",
"********************************************************************\r\n",
};
#endif

/********************************************************************
�������ܣ����ݰ���������ر���ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendReport(void)
{
 //��Ҫ���ص�8�ֽڱ���Ļ��塣���ڱ����Գ����У�ֻʹ��ǰ5�ֽڡ�
 uint8 Buf[8]={0,0,0,0,0,0,0,0};
 
 //ÿ����һ�����ݣ���Count����һ��
 Count++;
 
 //���ݲ�ͬ�İ����������뱨�档���ｫ8������������ڵ�һ�ֽڡ�
 Buf[0]=KeyPress;
 
 //����Count��ֵ���ñ���ĵڶ��������ֽڡ�
 Buf[1]=(Count&0xFF);       //����ֽ�
 Buf[2]=((Count>>8)&0xFF);  //�ε��ֽ�
 Buf[3]=((Count>>16)&0xFF); //�θ��ֽ�
 Buf[4]=((Count>>24)&0xFF); //����ֽ�
 
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

 for(i=0;i<6;i++)
 {
  LcdSetPoint(0,0);
  if(KeyDown&(1<<i))
  {
   LcdPrints("Key ");
   LcdPutChar('1'+i);
   LcdPrints(" down  ");
  }
  LcdSetPoint(0,0);
  if(KeyUp&(1<<i))
  {
   LcdPrints("Key ");
   LcdPutChar('1'+i);
   LcdPrints(" up    ");
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
#ifdef DEBUG0
 int i;
#endif

 int InterruptSource;
 
 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��
 LedInit();         //LED��Ӧ�Ĺܽų�ʼ��
 LcdInit();         //LCD��ʼ��
 AdcInit();         //ADC��ʼ��
 Timer1Init();      //��ʱ��1��ʼ������������10ms�Ķ�ʱɨ���ź�
 KeyInit();         //���̳�ʼ��
 Uart0Init();       //����0��ʼ��

#ifdef DEBUG0
 for(i=0;i<16;i++)   //��ʾͷ��Ϣ
 {
  Prints(HeadTable[i]);
 }
#endif

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
   if(InterruptSource&(1<<3))
   {
    UsbEp3In();     //�˵�3�����жϴ���
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

