/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

UART.C  file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2009.03.07
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#include <AT91SAM7S64.H>
#include "UART.h"
#include "MyType.h"
#include "config.h"

uint8 UartBuffer[BUF_LEN];
uint8 UsbEp2Buffer[BUF_LEN];

uint8 UartBufferOutputPoint;
uint8 UartBufferInputPoint;
uint8 UsbEp2BufferOutputPoint;

uint8 UartByteCount;
uint8 UsbEp2ByteCount;

/********************************************************************
�������ܣ������жϴ���
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Uart0Isr(void) __irq
{
 if(*AT91C_AIC_ISR!=6)return; //���Ǹ��ж�Դ��ֱ�ӷ���
 //*AT91C_AIC_ICCR=1<<6; //ʹ���������жϣ����Զ�����жϣ����ﲻ��  
 //�������0״̬��־
 if((*AT91C_US0_CSR)&(1<<0)) //����ǽ������
 {
  //����һ�ֽ����ݱ����ڻ�������
  UartBuffer[UartBufferInputPoint]=(unsigned char)(*AT91C_US0_RHR);  //��ȡ����
  //������λ������
  UartBufferInputPoint++;
  //����Ѿ����ﻺ����ĩβ�����л�����������ͷ
  if(UartBufferInputPoint>=BUF_LEN)
  {
   UartBufferInputPoint=0;
  }
  //�����ֽ�����1
  UartByteCount++;
 }
 *AT91C_AIC_EOICR=0;    //�жϴ������
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ڳ�ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Uart0Init(void)
{
 //PIOģ��
 *AT91C_PIOA_PDR=(1<<5)|(1<<6);    //PA5,PA6 ��Ϊ����ʹ��
 *AT91C_PIOA_IDR=(1<<5)|(1<<6);    //��ֹPA5��PA6 IO�ж�
 *AT91C_PIOA_PPUER=(1<<5)|(1<<6);  //ʹ��PA5��PA6��������
 *AT91C_PIOA_ASR=(1<<5)|(1<<6);    //���ӵ�A���裨�����ڣ�
 
 //UARTģ��
 *AT91C_US0_CR = (1<<2)    //Reset Receiver
                |(1<<3)    //Reset Transmitter
                |(1<<8);   //Reset Status Bits
                
 *AT91C_US0_CR = (1<<4)    //Receiver Enable
                |(1<<6);   //Transmitter Enable
               
 *AT91C_US0_MR = (0<<0)    //Normal
                |(0<<4)    //MCK
                |(3<<6)    //8 bits
                |(0<<8)    //USART operates in Asynchronous Mode.
                |(4<<9)    //No parity
                |(0<<12)   //1 stop bit
                |(0<<14)   //Normal Mode
                |(0<<16)   //Least Significant Bit is sent/received first.
                |(0<<17)   //CHRL defines character length.
                |(0<<18)   //The USART does not drive the SCK pin.
                |(0<<19)   //16x Oversampling.
                |(1<<20);  //The NACK is not generated.
               
 *AT91C_US0_IER = (1<<0);    //RXRDY Interrupt Enable
 
 *AT91C_US0_BRGR = (Fmck/16/BitRate);  //����ָ���Ĳ�����
 
 *AT91C_US0_RTOR = 0;  //The Receiver Time-out is disabled.
 
 *AT91C_US0_TTGR = 0;  //The Transmitter Timeguard is disabled.
 
 
 //�жϿ���ģ��
 //��ʼ���ж�Դ6������0�жϣ�
 AT91C_AIC_SMR[6] = (0<<0)  //The priority level = 0(lowest)
                   |(3<<5); //Positive edge triggered
 
 AT91C_AIC_SVR[6]=(int)Uart0Isr;  //set ISR entrance address
 
 *AT91C_AIC_IECR=(1<<6);  //Enable source ID 6 interrupt
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ô��ڲ����ʡ�
��ڲ�����NewBitRate: ��Ҫ���õĲ����ʡ�
��    �أ�ʵ�����õĲ����ʡ�
��    ע���ޡ�
********************************************************************/
uint32 UartSetBitRate(uint32 NewBitRate)
{
 *AT91C_US0_BRGR = (Fmck/16/NewBitRate);  //����ָ���Ĳ�����
 NewBitRate = Fmck/16/(*AT91C_US0_BRGR);  //����ʵ�ʵĲ�����
 return NewBitRate;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ô��ڵ�ֹͣλ��
��ڲ�����StopBits��ֹͣλ������
��    �أ�ʵ�����õ�ֹͣλ������
��    ע���ޡ�
********************************************************************/
uint8 UartSetStopBits(uint8 StopBits)
{
 uint32 Reg;
 
 Reg = *AT91C_US0_MR;  //����ԭ����ֵ
 Reg &= ~(3<<12);       //���bit12��13
 Reg |= (StopBits)<<12; //����bit12��13Ϊʵ�ʵ�ֵ��0��ʾ1bit��1��ʾ1.5bits��2��ʾ2bits
 *AT91C_US0_MR = Reg;   //���ûؼĴ���
 return StopBits;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ����ô��ڵ�����λ��
��ڲ�����DataBits������λ��
��    �أ�ʵ����Ƶ�����λ��
��    ע���ޡ�
********************************************************************/
uint8 UartSetDataBits(uint8 DataBits)
{
 uint32 Reg;
 
 Reg = *AT91C_US0_MR;  //����ԭ����ֵ
 Reg &= ~(3<<6);       //���bit6��7
 Reg |= (DataBits-5)<<6; //����bit6��7Ϊʵ�ʵ�ֵ��0��ʾ5bit
 *AT91C_US0_MR = Reg;   //���ûؼĴ���
 return DataBits;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ������ڷ���һ�ֽ����ݡ�
��ڲ�����d: Ҫ���͵��ֽ����ݡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UartPutChar(uint8 d)
{
 while(!((*AT91C_US0_CSR)&(1<<1))); //�ȴ��������
 *AT91C_US0_THR=d;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�����һ���ַ�����
��ڲ�����pd��Ҫ���͵��ַ���ָ�롣
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Prints(uint8 * pd)
{
 while((*pd)!='\0') //�����ַ�����ֱ������0�Ž���
 {
  UartPutChar(*pd); //����һ���ַ�
  pd++;  //�ƶ�����һ���ַ�
 }
}
////////////////////////End of function//////////////////////////////

#ifdef DEBUG1

/********************************************************************
�������ܣ�������ת��ʮ�����ַ������͡�
��ڲ�����x������ʾ��������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void PrintLongInt(uint32 x)
{
 int8 i;
 uint8 display_buffer[10];

 for(i=9;i>=0;i--)
 {
  display_buffer[i]='0'+x%10;
  x/=10;
 }
 for(i=0;i<9;i++)
 {
  if(display_buffer[i]!='0')break;
 }
 for(;i<10;i++)UartPutChar(display_buffer[i]);
}
////////////////////////End of function//////////////////////////////

#endif

const uint8 HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/********************************************************************
�������ܣ�����������ʮ�����Ʒ��͡�
��ڲ����������͵�������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void PrintShortIntHex(uint16 x)
{
 uint8 i;
 uint8 display_buffer[7];
 display_buffer[6]=0;
 display_buffer[0]='0';
 display_buffer[1]='x';
 for(i=5;i>=2;i--) //������ת��Ϊ4���ֽڵ�HEXֵ
 {
  display_buffer[i]=HexTable[(x&0xf)];
  x>>=4;
 }
 Prints(display_buffer);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�����������ʮ�����Ʒ��͡�
��ڲ����������͵�������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void PrintLongIntHex(uint32 x)
{
 uint8 i;
 uint8 display_buffer[11];
 display_buffer[10]=0;
 display_buffer[0]='0';
 display_buffer[1]='x';
 for(i=9;i>=2;i--) //������ת��Ϊ4���ֽڵ�HEXֵ
 {
  display_buffer[i]=HexTable[(x&0xf)];
  x>>=4;
 }
 Prints(display_buffer);
}
////////////////////////End of function//////////////////////////////

#if (defined DEBUG0)||(defined DEBUG1)
/********************************************************************
�������ܣ�����һ��byte�����ݡ�
��ڲ����������͵����ݡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void Printc(uint8 x)
{
 UartPutChar(x);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���HEX��ʽ����һ��byte�����ݡ�
��ڲ����������͵�����
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void PrintHex(uint8 x)
{
 Printc('0');
 Printc('x');
 Printc(HexTable[x>>4]);
 Printc(HexTable[x&0xf]);
 Printc(' ');
}
////////////////////////End of function//////////////////////////////
#endif
