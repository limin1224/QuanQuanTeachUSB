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
#include "led.h"
#include "lcd.h"
#include "ShowSpectrum.h"

const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******            21IC DIY U��ѧϰ�� ֮USB ��Ƶ����豸       ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******           ͨ��USB�������Ƶ��Ȼ����FFT������ʾƵ��     ******\r\n",
"******  ������Ƶ�豸��ѡ�и��豸Ȼ�󲥷�һ�������ļ����в���  ******\r\n",
"********************************************************************\r\n",
};

/********************************************************************
�������ܣ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void main(void)
{
 uint8 Ep2NoDataOut=0;  //��־�Ƿ����������
 
 uint16 OldFrameNumber=0; //��һ�α����֡��
 
 int i;

 int InterruptSource;
 
 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��
 LedInit();         //LED��Ӧ�Ĺܽų�ʼ��
 LcdInit();         //LCD��ʼ��
 Uart0Init();       //����0��ʼ��

 InitBitRev();  //��ʼ��FFT�õ�λ��ת�õı��
 
 for(i=0;i<17;i++)   //��ʾͷ��Ϣ
 {
  Prints(HeadTable[i]);
 }

 UsbChipInit();  //��ʼ��USB����
 
 while(1)
 {
  InterruptSource=(*AT91C_UDP_ISR)&(0x0F|(1<<8)|(1<<12)|(1<<11)); //ȡ����Ҫ���ж�
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
    if(AT91C_UDP_CSR[0]&(1<<0))  //����Ƕ˵�0�������
    {
     UsbEp0In();     //�˵�0�����жϴ���
    }
   }
   if(InterruptSource&(1<<11))  //SOF�ж�
   {
    *AT91C_UDP_ICR=1<<11; //����ж�
    UsbBusSof(); //SOF�жϴ���
   }
   if(ConfigValue!=0)
   {
    if(InterruptSource&(1<<1))
    {
     UsbEp1In();      //�˵�1�����жϴ���
    }
    if(InterruptSource&(1<<2))
    {
     UsbEp2Out();     //�˵�2����жϴ���
     Ep2NoDataOut=0;  //˵�����������
     if(AudioOutByteCount>=(LENGTH)*2)  //���������������LENGTH��
     {
      AudioOutByteCount=0;  //������0
      ShowSpectrum();       //��ʾƵ����LCD��
     }
    }
    if(InterruptSource&(1<<3))
    {
     UsbEp3In();      //�˵�3�����жϴ���
    }
    if((uint16)(CurrentFrameNumber-OldFrameNumber)>=8)  //ÿ8��֡����һ��
    {
     OldFrameNumber=CurrentFrameNumber;  //���浱ǰ֡��
     if(Ep2NoDataOut)  //���û�����ݣ���
     {
      //��������������������С
      for(i=0;i<LENGTH;i++)
      {
       AudioOutBuf[i]/=2;
      }
      AudioOutByteCount=0;  //������0
      ShowSpectrum();       //��ʾƵ��
     }
     Ep2NoDataOut=1;  //����Ϊû���������
    }
   }
  }
 }
}
/////////////////////////End of function/////////////////////////////

