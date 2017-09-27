/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

KEY.C  file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2009.03.07
�汾��V1.2
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#include <AT91SAM7S64.H>
#include "Key.h"
#include "MyType.h"

volatile uint8  KeyCurrent,KeyOld,KeyNoChangedTime;
volatile uint8  KeyPress;
volatile uint8  KeyDown,KeyUp,KeyLast;

volatile uint8 KeyCanChange;

/********************************************************************
�������ܣ����̳�ʼ��
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void KeyInit(void)
{
 KeyPress=0;       //�ް�������
 KeyNoChangedTime=0;
 KeyOld=0;
 KeyCurrent=0;
 KeyLast=0;
 KeyDown=0;
 KeyUp=0;
 KeyCanChange=1;   //�����ֵ�ı�
}
/*******************************************************************/

/********************************************************************
�������ܣ���ȡ����ֵ��
��ڲ������ޡ�
��    �أ��ޡ�
��    ע������ֵ��λͼ��
********************************************************************/
uint8 KeyGetValue(void)
{
 int AdcKeyValue;
 AdcKeyValue=*AT91C_ADC_CDR4;  //����ADC���
 *AT91C_ADC_CR=(1<<1);         //������һ��ת��
 
 //�Լ��̵�ADCֵ���д���
 if((AdcKeyValue>KEY0_VALUE-KEY_ERROR_RANGE)
  &&(AdcKeyValue<KEY0_VALUE+KEY_ERROR_RANGE))return 0; //�ް�������
  
 if(AdcKeyValue>KEY1_VALUE-KEY_ERROR_RANGE)return KEY1;  //KEY1����
 
 if((AdcKeyValue>KEY2_VALUE-KEY_ERROR_RANGE)
  &&(AdcKeyValue<KEY2_VALUE+KEY_ERROR_RANGE))return KEY2;  //KEY2����

 if((AdcKeyValue>KEY3_VALUE-KEY_ERROR_RANGE)
  &&(AdcKeyValue<KEY3_VALUE+KEY_ERROR_RANGE))return KEY3;  //KEY3����
  
 if((AdcKeyValue>KEY4_VALUE-KEY_ERROR_RANGE)
  &&(AdcKeyValue<KEY4_VALUE+KEY_ERROR_RANGE))return KEY4;  //KEY4����
  
 if((AdcKeyValue>KEY5_VALUE-KEY_ERROR_RANGE)
  &&(AdcKeyValue<KEY5_VALUE+KEY_ERROR_RANGE))return KEY5;  //KEY5����
  
 if((AdcKeyValue<KEY6_VALUE+KEY_ERROR_RANGE))return KEY6;  //KEY6����
 
 //�������������0
 return 0;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�����ɨ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע�����жϺ�����ÿ5ms����һ�Ρ�
********************************************************************/
void KeyScan(void)
{
 if(!KeyCanChange)return;     //������ڴ�����������ɨ����� 

 //��ʼ����ɨ��
 //���水��״̬����ǰ�������
 //KeyCurrent�ܹ���8��bit
 //��ĳ�����ذ���ʱ����Ӧ��bitΪ1
 KeyCurrent=KeyGetValue(); //��ȡ��ֵ

 if(KeyCurrent!=KeyOld)  //�������ֵ���ȣ�˵��������������˸ı�
 {
  KeyNoChangedTime=0;       //���̰���ʱ��Ϊ0
  KeyOld=KeyCurrent;        //���浱ǰ�������
  return;  //����
 }
 else
 {
  KeyNoChangedTime++;      //����ʱ���ۼ�
  if(KeyNoChangedTime>=1)  //�������ʱ���㹻
  {
   KeyNoChangedTime=1;
   KeyPress=KeyOld;      //���水��
   KeyDown|=(~KeyLast)&(KeyPress); //����°��µļ�
   KeyUp|=KeyLast&(~KeyPress);     //������ͷŵļ�
   KeyLast=KeyPress;               //���浱ǰ�������
  }
 }
}
/*******************************************************************/


