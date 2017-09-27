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

#ifdef DEBUG0
const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******               21IC DIY U��ѧϰ�� ֮USBת����           ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"********************************************************************\r\n",
};
#endif

/********************************************************************
�������ܣ������ڻ������е����ݷ��͵��˵�1�ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendUartDataToEp1(void)
{
 uint8 Len;
 
 //��ʱ��ֹ�����жϣ���ֹUartByteCount���ж����޸Ķ����²�ͬ��
 *AT91C_US0_IDR = (1<<0); //RXRDY Interrupt Disable
 //�����ڻ��������յ����ֽ������Ƴ���
 Len=UartByteCount;
 //��鳤���Ƿ�Ϊ0�����û���յ����ݣ�����Ҫ����ֱ�ӷ���
 if(Len==0)
 {
  *AT91C_US0_IER = (1<<0);    //�ǵô򿪴����ж�
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
 *AT91C_US0_IER = (1<<0);    //RXRDY Interrupt Enable
 
 //������д�뵽�˵�1���뻺����
 UsbChipWriteEndpointBuffer(1,Len,UartBuffer+UartBufferOutputPoint);
 //�޸�������ݵ�λ��
 UartBufferOutputPoint+=Len;
 //����Ѿ����ﻺ����ĩβ�������ûؿ�ͷ
 if(UartBufferOutputPoint>=BUF_LEN)
 {
  UartBufferOutputPoint=0;
 }
  Ep1InIsBusy=1;
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

 int InterruptSource;
 
 SystemClockInit(); //ϵͳʱ�ӳ�ʼ��

 Uart0Init();       //����0��ʼ��

#ifdef DEBUG0
 for(i=0;i<15;i++)   //��ʾͷ��Ϣ
 {
  Prints(HeadTable[i]);
 }
#endif

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
  if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Է��غͷ��ʹ�������
  {   
   if(Ep1InIsBusy==0)  //����˵�1���У����ʹ������ݵ��˵�1
   {
    SendUartDataToEp1();  //���ú��������������ݷ��͵��˵�1
   }
   if(UsbEp2ByteCount!=0) //�˵�2���ջ������л�������δ���ͣ����͵�����
   {
    //����һ�ֽڵ�����
    UartPutChar(UsbEp2Buffer[UsbEp2BufferOutputPoint]);
    UsbEp2BufferOutputPoint++; //����λ�ú���1
    UsbEp2ByteCount--;   //����ֵ��1
   }
  }
 }
}
/////////////////////////End of function/////////////////////////////

