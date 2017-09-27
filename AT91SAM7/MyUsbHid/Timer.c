/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
          
Timer.c  file
         
���ߣ�Computer-lov
��������: 2009-03-06
�޸�����: 2009-03-06
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <AT91SAM7S64.H>
#include "timer.h"
#include "key.h"
#include "config.h"

/********************************************************************
�������ܣ���ʱ��0��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע������һ��44.1KHz��
********************************************************************
void Timer0Init(void)
{
 *AT91C_TC0_IDR=0xFF;  //Disable interrupts
 
 *AT91C_TC0_CCR=(1<<0)   //Counter Clock Enable
               |(0<<1)
               |(1<<2);  //the counter is reset and the clock is started.
               
 *AT91C_TC0_CMR=(0<<0)  //TIMER_CLOCK1 (MCK/2=48.05M/2=24.025M
               |(0<<3)  //Counter is incremented on rising edge of the clock.
               |(0<<4)  //The clock is not gated by an external signal.
               |(0<<6)  //Counter clock is not stopped when counter reaches RC.
               |(0<<7)  //Counter clock is not disabled when counter reaches RC.
               |(0<<8)  //External Event Edge Selection: none
               |(0<<10)  //External Event Selection: TIOB
               |(0<<12)  //The external event has no effect on the counter and its clock.
               |(2<<13)  //UP mode with automatic trigger on RC Compare
               |(1<<15)  //Waveform Mode is enabled.
               |(1<<16)  //RA Compare Effect on TIOA: set
               |(2<<18)  //RC Compare Effect on TIOA: clear
               |(0<<20)  //External Event Effect on TIOA: none
               |(0<<22)  //Software Trigger Effect on TIOA: none
               |(0<<24)  //RB Compare Effect on TIOB: none
               |(0<<26)  //RC Compare Effect on TIOB: none
               |(0<<28)  //External Event Effect on TIOB: none
               |(0<<30); //Software Trigger Effect on TIOB: none
               
 //Ƶ��Ϊ44.1KHz
 *AT91C_TC0_RC=(Fmck/2)/44100;
 *AT91C_TC0_RA=*AT91C_TC0_RC/2; //����50%ռ�ձȵķ���
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ʱ��1�жϴ�������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Timer1Isr(void) __irq
{
 if(*AT91C_AIC_ISR!=13)return; //���Ǹ��ж�Դ��ֱ�ӷ���
 //*AT91C_AIC_ICCR=1<<13; //ʹ���������жϣ����Զ�����жϣ����ﲻ��
 //�����ʱ��״̬��־
 if(!((*AT91C_TC1_SR)&(1<<4)))return; //����RC�Ƚ��жϣ�����
 KeyScan();  //����ɨ��
 *AT91C_AIC_EOICR=0;    //�жϴ������
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ʱ��1��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע������һ������Ϊ5ms�Ķ�ʱ�ж��źš�
********************************************************************/
void Timer1Init(void)
{
 *AT91C_TC1_IDR=0xFF;   //Disable interrupts
 *AT91C_TC1_IER=(1<<4); //Enables the RC Compare Interrupt.
 
 *AT91C_TC1_CCR=(1<<0)   //Counter Clock Enable
               |(0<<1)
               |(1<<2);  //the counter is reset and the clock is started.
               
 *AT91C_TC1_CMR=(2<<0)  //TIMER_CLOCK3 (MCK/32=48.05M/32 is about 1.5M)
               |(0<<3)  //Counter is incremented on rising edge of the clock.
               |(0<<4)  //The clock is not gated by an external signal.
               |(0<<6)  //Counter clock is not stopped when counter reaches RC.
               |(0<<7)  //Counter clock is not disabled when counter reaches RC.
               |(0<<8)  //External Event Edge Selection: none
               |(0<<10)  //External Event Selection: TIOB
               |(0<<12)  //The external event has no effect on the counter and its clock.
               |(2<<13)  //UP mode with automatic trigger on RC Compare
               |(1<<15)  //Waveform Mode is enabled.
               |(1<<16)  //RA Compare Effect on TIOA: set
               |(2<<18)  //RC Compare Effect on TIOA: clear
               |(0<<20)  //External Event Effect on TIOA: none
               |(0<<22)  //Software Trigger Effect on TIOA: none
               |(0<<24)  //RB Compare Effect on TIOB: none
               |(0<<26)  //RC Compare Effect on TIOB: none
               |(0<<28)  //External Event Effect on TIOB: none
               |(0<<30); //Software Trigger Effect on TIOB: none
               
 //Ƶ��Ϊ200Hz
 *AT91C_TC1_RC=(Fmck/32)/200;
 *AT91C_TC1_RA=*AT91C_TC1_RC/2; //����50%ռ�ձȵķ���
 
 //��ʼ���ж�Դ13����ʱ��1�жϣ�
 AT91C_AIC_SMR[13] = (0<<0)  //The priority level = 0(lowest)
                    |(3<<5); //Positive edge triggered
 
 AT91C_AIC_SVR[13]=(int)Timer1Isr;  //set ISR entrance address
 
 *AT91C_AIC_IECR=(1<<13);  //Enable source ID 13 interrupt
}
/////////////////////////End of function/////////////////////////////