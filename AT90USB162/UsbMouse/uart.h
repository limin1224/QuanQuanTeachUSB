/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

UART.H  file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2009-05-10
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#include "config.h"

#if (defined DEBUG0)||(defined DEBUG1)

#ifndef __UART_C__
#define __UART_C__

#include "MyType.h"

void Uart1Init(void);
void UartPutChar(uint8);
void Prints(flash uint8 *);
void PrintLongInt(uint32);
void PrintShortIntHex(uint16 x);
void Printc(uint8);
void PrintHex(uint8 x);
void PrintLongIntHex(uint32 x);

#endif

#endif
