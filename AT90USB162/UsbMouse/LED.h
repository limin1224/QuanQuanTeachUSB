/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
        
LED.h  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-05-10
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/


#ifndef __LED_H__
#define __LED_H__

#include <90USB162.h>

#define OnLed1() PORTB|=(1<<7)
#define OnLed2() PORTB|=(1<<6)

#define OffLed1() PORTB&=~(1<<7)
#define OffLed2() PORTB&=~(1<<6)

extern volatile unsigned char UsbLedBlink;

void LedInit(void);
void DelayXms(unsigned long int x);

#endif
