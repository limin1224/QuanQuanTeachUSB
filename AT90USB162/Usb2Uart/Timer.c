/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
          
Timer.c  file
         
���ߣ�Computer-lov
��������: 2009-03-06
�޸�����: 2009-05-11
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <90USB162.H>
#include "timer.h"
#include "config.h"
#include "MyType.h"
#include "led.h"
#include "UsbCore.h"

/********************************************************************
�������ܣ���ʱ��1�жϴ�������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
interrupt [TIM0_OVF] void Timer0Isr(void)
{
 static uint8 i,Count;
 TCNT0=0xFF-78; //��ʱ����װ
 Count++;
 if(Count>=25) //ÿ����˸8��
 {
  Count=0;
  if(UsbLedBlink)   //�����Ҫ��˸
  {
   if(i) OnLed1();
   else OffLed1();
   i=!i;
   UsbLedBlink--;
  }
  else
  {
   if(ConfigValue) //�������ֵΪ��0��Led1��˸����
   {
    OnLed1();
   }
   else         //����Led1��˸����
   {
    OffLed1();
   }
  }
 }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ʱ��0��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע������һ������Ϊ5ms�Ķ�ʱ�ж��źš�
********************************************************************/
void Timer0Init(void)
{
 // Timer/Counter 0 initialization
 // Clock source: System Clock
 // Clock value: 15.625 kHz
 // Mode: Normal top=FFh
 // OC0A output: Disconnected
 // OC0B output: Disconnected
 TCCR0A=0x00;
 TCCR0B=0x05;
 TCNT0=0xFF-78;
 OCR0A=0x00;
 OCR0B=0x00; 
 //Timer 1 overflow interrupt is on
 TIMSK0|=0x01;
 #asm("sei");  //���ж�
}
/////////////////////////End of function/////////////////////////////