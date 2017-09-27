/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
          
ADC.c  file
        
���ߣ�Computer-lov
��������: 2009-03-06
�޸�����: 2009-03-06
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <AT91SAM7S64.H>
#include "ADC.h"

/********************************************************************
�������ܣ�ADC��ʼ��������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void AdcInit(void)
{
 AdcReset();
 *AT91C_ADC_MR=(0<<0)  //Hardware trigger selected by TRGSEL field is disabled.
              |(0<<1)  //TIOA Ouput of the Timer Counter Channel 0
              |(0<<4)  //10-bit resolution
              |(0<<5)  //Normal Mode
              |(4<<8)  //Prescaler Rate Selection, ADCClock = MCK/10 = 4.8MHz
              |(24<<16) //Startup Time=(24+1)*8/ADCClock is about 20us.
              |(5<<24);  //Sample & Hold Time = 5/ADCClock is about 1us.
              
 *AT91C_ADC_CHDR=0xFF; //Disable all channel.             
 *AT91C_ADC_CHER=(1<<4);  //Enable CH4, as key input.
 *AT91C_ADC_IDR=0xFFFFF;  //Disable all ADC interrupts.
 AdcStartConversion(); //�������һ��ת��
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ����ת������ȡһ��ADC��
��ڲ�����ch��ͨ���š�
��    �أ�ADCֵ��
��    ע���ޡ�
********************************************************************/
int AdcRead(void)
{
 AdcStartConversion(); //�������ת��
 while(!((*AT91C_ADC_SR)&(1<<16)));  //�ȴ�ת�����
 return *AT91C_ADC_LCDR;  //��ȡADCֵ
}
/////////////////////////End of function/////////////////////////////


/********************************************************************
�������ܣ�����DMA��
��ڲ�����Addr: �������ݵĻ�������ַ��Count��Ҫ���յ�������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void AdcSetupDma(short int *Addr, int Count)
{
 *AT91C_ADC_RPR=(unsigned int)Addr;   //���õ�ַ
 *AT91C_ADC_RCR=Count;  //��������
 *AT91C_ADC_PTCR=(1<<0);  //��������
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ��ȴ�ADC��DMA������ɡ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void AdcWaitDmaFinish(void)
{
 while((*AT91C_ADC_RCR)!=0); //��δ������ʱ���ȴ�
}
/////////////////////////End of function/////////////////////////////
