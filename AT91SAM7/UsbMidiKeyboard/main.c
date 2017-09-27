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

const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******            21IC DIY U��ѧϰ�� ֮USB MIDI�����豸       ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                   �밴K1-K6�ֱ���в���                ******\r\n",
"******           K6:1 K5:2 K4:3 K1:4 K2: 5 K3:�����Զ���      ******\r\n",
"********************************************************************\r\n",
};

/********************************************************************
�������ܣ����ݰ����������Note On��Ϣ�ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendNoteOnMsg(void)
{
 //4�ֽڵĻ�����
 uint8 Buf[4];
 
 //Note On��Ϣ��һ�ֽڹ̶�Ϊ0x09���ڶ��ֽ�Ϊ0x9n��nΪͨ���ţ�
 //�����ֽ�Ϊ0xKK��KΪ���ߣ��������ֽ�Ϊ0xVV��VΪ���ȣ���
 
 Buf[0]=0x09; //Note On��Ϣ�İ�ͷ
 Buf[1]=0x90; //��ͨ��0�Ϸ���Note On��Ϣ
 Buf[3]=0x7F; //��������Ϊ���
 
 if(KeyDown&KEY6)
 {
  Buf[2]=60;  //C����1����������ΪC����������C��
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyDown&=~KEY6; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyDown&KEY5)
 {
  Buf[2]=62;  //C����2����������ΪD����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyDown&=~KEY5; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyDown&KEY4)
 {
  Buf[2]=64;  //C����3����������ΪE����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyDown&=~KEY4; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyDown&KEY1)
 {
  Buf[2]=65;  //C����4����������ΪF����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyDown&=~KEY1; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyDown&KEY2)
 {
  Buf[2]=67;  //C����5����������ΪG����
  //ͨ���˵�2����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyDown&=~KEY2; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 //����а���������رն�Ӧ����
 Buf[3]=0x00;  //��������Ϊ0
 
 if(KeyUp&KEY6)
 {
  Buf[2]=60;  //C����1����������ΪC����������C��
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyUp&=~KEY6; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyUp&KEY5)
 {
  Buf[2]=62;  //C����2����������ΪD����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyUp&=~KEY5; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyUp&KEY4)
 {
  Buf[2]=64;  //C����3����������ΪE����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyUp&=~KEY4; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyUp&KEY1)
 {
  Buf[2]=65;  //C����4����������ΪF����
  //ͨ���˵�1����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyUp&=~KEY1; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 }
 
 if(KeyUp&KEY2)
 {
  Buf[2]=67;  //C����5����������ΪG����
  //ͨ���˵�2����4�ֽ�MIDI�¼�����
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
  KeyUp&=~KEY2; //�����Ӧ�İ���
  return;  //����һ��������ͷ���
 } 
}
////////////////////////End of function//////////////////////////////

void PlaySong(void);

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
 int i;

 int InterruptSource;
 
 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��
 LedInit();         //LED��Ӧ�Ĺܽų�ʼ��
 LcdInit();         //LCD��ʼ��
 AdcInit();         //ADC��ʼ��
 Timer1Init();      //��ʱ��1��ʼ������������10ms�Ķ�ʱɨ���ź�
 KeyInit();         //���̳�ʼ��
 Uart0Init();       //����0��ʼ��

 for(i=0;i<17;i++)   //��ʾͷ��Ϣ
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
     if(KeyPress==(KEY3))  //�����סKEY3�����Զ���������
     {
      KeyCanChange=1; //������ɨ��
      PlaySong(); //���Ÿ���
     }
     if(KeyUp||KeyDown) //����а����¼�����
     {
      SendNoteOnMsg();  //�򷵻�MIDI Note On��Ϣ
     }
     KeyCanChange=1;  //������ɨ��
    }
   }
   LcdRefresh();  //ˢ��LCD��ʾ
  }
 }
}
/////////////////////////End of function/////////////////////////////

