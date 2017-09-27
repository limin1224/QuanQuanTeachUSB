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
#include "MyType.h"

#define Fmck 48050000L

extern int16 AudioOutBuf1[32];  //������1
extern int16 AudioOutBuf2[32];  //������2

extern int16 *pAudioData;  //ָ��ǰʹ�õ��ĸ�������

extern uint32 AudioDataPoint;  //ָ��ǰ���ݵ�λ��

/********************************************************************
�������ܣ���ʱ��0�жϴ�������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Timer0Isr(void) __irq
{
 if(*AT91C_AIC_ISR!=12)return; //���Ǹ��ж�Դ��ֱ�ӷ���
 //*AT91C_AIC_ICCR=1<<12; //ʹ���������жϣ����Զ�����жϣ����ﲻ��
 //�����ʱ��״̬��־
 if(!((*AT91C_TC0_SR)&(1<<4)))return; //����RC�Ƚ��жϣ�����
 
 //������Ƶ�����޸�PWMռ�ձȣ�ƫ��0x8000����ֻȡ��8λ
 *AT91C_PWMC_CH0_CUPDR=((pAudioData[AudioDataPoint]+0x8000)>>8)&0xFF;
 AudioDataPoint++; //�л�����һ������
 if(AudioDataPoint>=22) //���ݰ����꣬�л�����һ�����ݰ�
 {
  AudioDataPoint=0;
  if(pAudioData==AudioOutBuf1)
  {
   pAudioData=AudioOutBuf2;
  }
  else
  {
   pAudioData=AudioOutBuf1;
  }
 }
 *AT91C_AIC_EOICR=0;    //�жϴ������
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ʱ����ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע����ʱ�ж�Ƶ��Ϊ22kHz
********************************************************************/
void Timer0Init(void)
{
 *AT91C_TC0_IDR=0xFF;  //Disable interrupts
 *AT91C_TC0_IER=(1<<4); //Enables the RC Compare Interrupt.
 
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
               
 //Ƶ��Ϊ22kHz
 *AT91C_TC0_RC=(Fmck/2)/22000;
 *AT91C_TC0_RA=*AT91C_TC0_RC/2; //����50%ռ�ձȵķ���
 
 //��ʼ���ж�Դ12����ʱ��0�жϣ�
 AT91C_AIC_SMR[12] = (0<<0)  //The priority level = 0(lowest)
                    |(3<<5); //Positive edge triggered
 
 AT91C_AIC_SVR[12]=(int)Timer0Isr;  //set ISR entrance address
 
 *AT91C_AIC_IECR=(1<<12);  //Enable source ID 13 interrupt
}
/////////////////////////End of function/////////////////////////////

