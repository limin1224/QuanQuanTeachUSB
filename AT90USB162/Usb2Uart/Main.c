/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

main.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-05-11
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <90USB162.H>
#include "SystemClock.h"
#include "LED.h"
#include "Timer.h"
#include "uart.h"
#include "UsbCore.h"
#include "AT90USB.h"

#ifdef DEBUG0
const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******              ��ȦȦ������USB��֮ USBת����             ******\r\n",
"******                      AT90USB162 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                     ��򿪴��ڽ��в���                 ******\r\n",
"******                                                        ******\r\n",
"********************************************************************\r\n",
};
#endif

/********************************************************************
�������ܣ������ڻ������е����ݷ��͵��˵�3�ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendUartDataToEp3(void)
{
 uint8 Len;
 //��ʱ��ֹ�����жϣ���ֹUartByteCount���ж����޸Ķ����²�ͬ��
 #asm("cli");
 Len=UartByteCount;   //��ȡ����
 //��鳤���Ƿ�Ϊ0�����û���յ����ݣ�����Ҫ����ֱ�ӷ���
 if(Len==0)
 {
  #asm("sei"); //���ж�
  return;
 }
 //���Len�ֽڸ������Ƿ��Խ�˻������߽磬�����Խ�ˣ���ô����ֻ����
 //��Խ�߽�֮ǰ�����ݣ�ʣ������������´η��͡����򣬿���һ�η���ȫ����
 if((Len+UartBufferOutputPoint)>BUF_LEN)
 {
  Len=BUF_LEN-UartBufferOutputPoint;
 }
 //�޸Ļ����������ֽ���
 UartByteCount-=Len;
 
 //��������Դ򿪴����ж���
 #asm("sei");
 
 //������д�뵽�˵�3���뻺����
 UsbChipWriteEndpointBuffer(3,Len,UartBuffer+UartBufferOutputPoint);
 //�޸�������ݵ�λ��
 UartBufferOutputPoint+=Len;
 //����Ѿ����ﻺ����ĩβ�������ûؿ�ͷ
 if(UartBufferOutputPoint>=BUF_LEN)
 {
  UartBufferOutputPoint=0;
 }
 Ep3InIsBusy=1;
}
////////////////////////End of function//////////////////////////////

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

 #asm("cli");
 
 MCUSR=0;
 MCUSR &= ~(1 << 3);
 WDTCSR |= (1<<4) | (1<<3);
 /* Turn off WDT */
 WDTCSR = 0x00;

 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��
 LedInit();         //LED��Ӧ�Ĺܽų�ʼ��

 Timer0Init();      //��ʱ��0��ʼ������������5ms�Ķ�ʱɨ���ź�
 
 OnLed2(); //��Դָʾ��
  
 Uart1Init();       //����0��ʼ��

#ifdef DEBUG0 
 for(i=0;i<17;i++)   //��ʾͷ��Ϣ
 {
  Prints(HeadTable[i]);
 }
#endif
 
 UsbChipInit();  //��ʼ��USB����
 
 while(1)
 {
  if(UDINT&(1<<0)) //SUSPI - Suspend Interrupt Flag
  {
   UDINT=~(1<<0); //����ж�
   UsbBusSuspend(); //���߹����жϴ���
  }
  if(UDINT&(1<<3)) //EORSTI - End Of Reset Interrupt Flag
  {
   UDINT=~(1<<3); //����ж�
   UsbBusReset();   //���߸�λ�жϴ���
  }
  UENUM=0; //ѡ��˵�0
  if(UEINTX&(3<<2))  //FIFOCON - FIFO Control Bit //�����SETUP����������ݵ�
  {
   UsbEp0Out();     //�˵�0����жϴ���
  }
  if((SendLength!=0)||(NeedZeroPacket)) //����˵�0������Ҫ����
  {
   if(UEINTX&(1<<0)) //���Ҷ˵�0����������
   { 
    UsbEp0In();      //�˵�0�����жϴ���
   }
  }
  UENUM=1; //ѡ��˵�1
  if(UEINTX&(1<<0))
  {
   UsbEp1In();      //�˵�1�����жϴ���
  }
  UENUM=2; //ѡ��˵�2
  if(UEINTX&(1<<2))
  {
   UsbEp2Out();     //�˵�2����жϴ���
  }
  UENUM=3; //ѡ��˵�3
  if(UEINTX&(1<<0))
  {
   UsbEp3In();      //�˵�3�����жϴ���
  }
  UENUM=4; //ѡ��˵�4
  if(UEINTX&(1<<2))
  {
   UsbEp4Out();     //�˵�4����жϴ���
  }

  if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Դ�������
  {   
   if(Ep3InIsBusy==0)  //����˵�3���У����ʹ������ݵ��˵�3
   {
    SendUartDataToEp3();  //���ú��������������ݷ��͵��˵�3
   }
   if(UsbEp4ByteCount!=0) //�˵�4���ջ������л�������δ���ͣ����͵�����
   {
    //����һ�ֽڵ�����
    UartPutChar(UsbEp4Buffer[UsbEp4BufferOutputPoint]);
    UsbEp4BufferOutputPoint++; //����λ�ú���1
    UsbEp4ByteCount--;   //����ֵ��1
   }
  }
 }
}
/////////////////////////End of function/////////////////////////////

