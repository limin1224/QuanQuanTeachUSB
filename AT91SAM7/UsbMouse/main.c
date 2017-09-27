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
"******               21IC DIY U��ѧϰ�� ֮USB���             ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                  �밴K1-K6�ֱ���в���                 ******\r\n",
"******              K1:�������  K2:�������  K3:������     ******\r\n",
"******              K6:�������  K5:�������  K4:����Ҽ�     ******\r\n",
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
 //��Ҫ���ص�4�ֽڱ���Ļ���
 //Buf[0]��D0���������D1�����Ҽ���D2�����м�������û�У�
 //Buf[1]ΪX�ᣬBuf[2]ΪY�ᣬBuf[3]Ϊ����
 uint8 Buf[4]={0,0,0,0};
 
 //����а�����ס�����Ҳ���KEY3��KEY4�����Ҽ���
 //����KEY3��KEY4�κ�һ�����б䶯�Ļ�������Ҫ���ر���
 if((KeyPress&(~(KEY3|KEY4)))||(KeyUp&(KEY3|KEY4))||(KeyDown&(KEY3|KEY4)))
 {
  if(KeyPress & KEY1) //���KEY1��ס��������Ҫ���ƣ���X��Ϊ��ֵ��
  {
   Buf[1]=-1;  //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY2) //���KEY2��ס��������Ҫ���ƣ���X��Ϊ��ֵ��
  {
   Buf[1]=1;   //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY6) //���KEY6��ס��������Ҫ���ƣ���Y��Ϊ��ֵ��
  {
   Buf[2]=-1;   //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY5)  //���KEY5��ס��������Ҫ���ƣ���Y��Ϊ��ֵ��
  {
   Buf[2]=1;  //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY3)  //������
  {
   Buf[0]|=0x01;  //D0Ϊ������
  }
  if(KeyPress & KEY4)  //����Ҽ�
  {
   Buf[0]|=0x02;  //D1Ϊ����Ҽ�
  }
  //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ4�ֽڡ�
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
 }
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

  if(KeyUp||KeyDown||KeyPress)  //����û������˰�������ѹס����
  {
   if(KeyUp||KeyDown)
   {
    DispKey(); //��LCD����ʾ�������
    LcdRefresh();  //ˢ��LCD��ʾ
   }
   if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Է��ر�������
   {
    if(!Ep1InIsBusy)  //����˵�1����û�д���æ״̬������Է�������
    {
     KeyCanChange=0;  //��ֹ����ɨ��
     if(KeyUp||KeyDown||KeyPress) //����а����¼�����
     {
      SendReport();  //�򷵻ر���
     }
     KeyCanChange=1;  //������ɨ��
    }
   }   
   //���KeyUp��KeyDown
   KeyUp=0;
   KeyDown=0;
  }
 }
}
/////////////////////////End of function/////////////////////////////

