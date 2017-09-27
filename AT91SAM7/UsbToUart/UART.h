/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

UART.H  file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2008.07.10
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#ifndef __UART_C__
#define __UART_C__

#include "MyType.h"

void Uart0Init(void);
void UartPutChar(uint8);
void Prints(uint8 *);
void PrintLongInt(uint32);
void PrintShortIntHex(uint16 x);
void Printc(uint8);
void PrintHex(uint8 x);
void PrintLongIntHex(uint32 x);
uint32 UartSetBitRate(uint32 NewBitRate);
uint8 UartSetStopBits(uint8 StopBits);
uint8 UartSetDataBits(uint8 DataBits);

//���建�����Ĵ�С������ʹ��64�ֽڡ�
//����RAMС�ĵ�Ƭ���������޸ĸö���������RAM��ʹ������
//���ǻ�Ҫ�ǵ��޸Ķ˵���󳤶ȣ�����������ݿ��ܻ������
#define BUF_LEN 64

extern uint8 UartBuffer[BUF_LEN];
extern uint8 UsbEp2Buffer[BUF_LEN];
extern uint8 UartBufferOutputPoint;
extern uint8 UartBufferInputPoint;
extern uint8 UartByteCount;
extern uint8 UsbEp2ByteCount;
extern uint8 UsbEp2BufferOutputPoint;

#endif
