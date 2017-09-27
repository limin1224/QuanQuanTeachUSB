/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

main.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-05-10
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <90USB162.H>
#include "SystemClock.h"
#include "LED.h"
#include "Timer.h"
#include "key.h"
#include "uart.h"
#include "UsbCore.h"
#include "AT90USB.h"

#ifdef DEBUG0
const uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******                ��ȦȦ������USB��֮USB���              ******\r\n",
"******                     AT91SAM7S64 CPU                    ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                  ÿ4���ӽ��Զ�����һ���Ҽ���ʾ         ******\r\n",
"******                                                        ******\r\n",
"********************************************************************\r\n",
};
#endif

/********************************************************************
�������ܣ����ݰ���������ر���ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendReport(void)
{
 //��Ҫ���ص�4�ֽڱ���Ļ���
 //Buf[0]��D0���������D1�����Ҽ���D2�����м�������û�У�
 //Buf[1]ΪX�ᣬBuf[2]ΪY�ᣬBuf[3]Ϊ����
 uint8 Buf[4]={0,0,0,0};
 
 //����а�����ס�����Ҳ���KEY3��KEY4�����Ҽ���
 //����KEY3��KEY4�κ�һ�����б䶯�Ļ�������Ҫ���ر���
 if((KeyPress&(~(KEY3|KEY4)))||(KeyUp&(KEY3|KEY4))||(KeyDown&(KEY3|KEY4)))
 {
  if(KeyPress & KEY1) //���KEY1��ס��������Ҫ���ƣ���X��Ϊ��ֵ��
  {
   Buf[1]=-1;  //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY2) //���KEY2��ס��������Ҫ���ƣ���X��Ϊ��ֵ��
  {
   Buf[1]=1;   //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY6) //���KEY6��ס��������Ҫ���ƣ���Y��Ϊ��ֵ��
  {
   Buf[2]=-1;   //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY5)  //���KEY5��ס��������Ҫ���ƣ���Y��Ϊ��ֵ��
  {
   Buf[2]=1;  //����һ�������ƶ�һ����λ��
  }
  if(KeyPress & KEY3)  //������
  {
   Buf[0]|=0x01;  //D0Ϊ������
  }
  if(KeyPress & KEY4)  //����Ҽ�
  {
   Buf[0]|=0x02;  //D1Ϊ����Ҽ�
  }
  //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ4�ֽڡ�
  UsbChipWriteEndpointBuffer(1,4,Buf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
 }
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
 KeyInit();         //���̳�ʼ��
 
 OnLed2(); //��Դָʾ��
  
#if (defined DEBUG0)||(defined DEBUG1)
 Uart1Init();       //����0��ʼ��
#endif

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

  //ʵ���ϣ�������ϲ�û�а�����Ϊ����ʾ�����ڶ�ʱ��0��ÿ��4��
  //ģ��һ�ο��ذ��£�����һ������Ҽ�������Ը���ʵ������������ı��档
  if(KeyUp||KeyDown||KeyPress)  //����û������˰�������ѹס����
  {
   if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Է��ر�������
   {
    if(!Ep1InIsBusy)  //����˵�1����û�д���æ״̬������Է�������
    {
     KeyCanChange=0;  //��ֹ����ɨ��
     if(KeyUp||KeyDown||KeyPress) //����а����¼�����
     {
      SendReport();  //�򷵻ر���
     }
     KeyCanChange=1;  //������ɨ��
    }
   }   
   //���KeyUp��KeyDown
   KeyUp=0;
   KeyDown=0;
  }
 }
}
/////////////////////////End of function/////////////////////////////

