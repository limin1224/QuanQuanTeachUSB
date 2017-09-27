/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
			
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        ��лPCB�����̡�������԰�� http://bbs.cepark.com/

main.c file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2012.01.26
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#include <AT89X52.H>  //ͷ�ļ�
#include "Key.h"
#include "Led.h"
#include "UART.h"
#include "pdiusbd12.h"
#include "UsbCore.h"
#include "config.h"
#include <math.h>

#define PI 3.1415926

#undef SHOW_MORE_MSG

#ifdef SHOW_MORE_MSG //��ʡROM�ռ�
code uint8 HeadTable[][74]={
"********************************************************************\r\n",
"******             ��ȦȦ������USB��֮ USB������              ******\r\n",
"******                      AT89S52 CPU                       ******\r\n",
"******                  �������ڣ�",__DATE__,"                 ******\r\n",
"******                   ����ʱ�䣺",__TIME__,"                   ******\r\n",
"******                    ���ߣ�����ȦȦ                      ******\r\n",
"******                    ��ӭ�������ߵ�                      ******\r\n",
"******           USBר����http://group.ednchina.com/93/       ******\r\n",
"******      BLOG1��http://www.ednchina.com/blog/computer00    ******\r\n",
"******                BLOG2��http://computer00.21ic.org       ******\r\n",
"******                  �밴K1-K8�ֱ���в���                 ******\r\n",
"******    K1:�ص�����  K2:��ֱ��   K3:��������  K4:��������   ******\r\n",
"******    K5:��Բ      K6:�������� K7:��        K8:��         ******\r\n",
"********************************************************************\r\n",
};
#endif

/********************************************************************
�������ܣ��жϴ�������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void ProcessInterrupt(void)
{
 int8 InterruptSource;
 if(D12GetIntPin() == 0) //������жϷ���
 {
  //���жϷ���������֮
  D12WriteCommand(READ_INTERRUPT_REGISTER);  //д���жϼĴ���������
  InterruptSource=D12ReadByte(); //���ص�һ�ֽڵ��жϼĴ���
  if(InterruptSource&0x80)UsbBusSuspend(); //���߹����жϴ���
  if(InterruptSource&0x40)UsbBusReset();   //���߸�λ�жϴ���
  if(InterruptSource&0x01)UsbEp0Out();     //�˵�0����жϴ���
  if(InterruptSource&0x02)UsbEp0In();      //�˵�0�����жϴ���
  if(InterruptSource&0x04)UsbEp1Out();     //�˵�1����жϴ���
  if(InterruptSource&0x08)UsbEp1In();      //�˵�1�����жϴ���
  if(InterruptSource&0x10)UsbEp2Out();     //�˵�2����жϴ���
  if(InterruptSource&0x20)UsbEp2In();      //�˵�2�����жϴ���
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��������㴥���¼���
��ڲ�����x��x�����ꣻy��y�����ꣻtouch���Ƿ�����
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SinglePointReport(uint16 x, uint16 y, uint8 touch)
{
 //��Ҫ���ص�6�ֽڱ���Ļ���
 //Buf[0]Ϊ����ID��ֵΪREPORTID_STOUCH��
 //Buf[1]��D0���������D1�����Ҽ���D2�����м�
 //Buf[2]ΪX����ֽڣ�Buf[3]ΪX����ֽڣ�
 //Buf[4]ΪY����ֽڣ�Buf[5]ΪY����ֽڡ�
 uint8 Buf[6]={0,0,0,0,0,0};
 Buf[0] = REPORTID_STOUCH;
 Buf[1] = touch;
 Buf[2] = x & 0xFF;
 Buf[3] = (x >> 8) & 0xFF;
 Buf[4] = y & 0xFF;
 Buf[5] = (y >> 8) & 0xFF;
 while(Ep1InIsBusy) //�ȴ�֮ǰ�����ݷ������
 {
  ProcessInterrupt();  //�����ж�
 }
 if(ConfigValue == 0) return;
 //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ6�ֽڡ�
 D12WriteEndpointBuffer(3, 6, Buf);
 Ep1InIsBusy=1;  //���ö˵�æ��־��
}
////////////////////////End of function//////////////////////////////

#define MoveTo(x,y) SinglePointReport(x, y, 0)
#define LineTo(x,y) SinglePointReport(x, y, 1)

/********************************************************************
�������ܣ���Բ�ĺ�����
��ڲ�����x��x�����ꣻy��y�����ꣻr���뾶
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void DrawCirle(uint16 x, uint16 y, uint16 r)
{
 uint16 i, px, py;
 MoveTo(x + r, y);
 LineTo(x + r, y);
 for(i = 0; i < 360; i ++)
 {
  px = x + r * cos((i * 1.0) / 180 * PI);
  py = y + r * sin((i * 1.0) / 180 * PI);
  LineTo(px, py);
 }
 MoveTo(x + r, y);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����������ߵĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void DrawSinCurve(void)
{
 uint16 px, py;
 MoveTo(4000, 2000);
 LineTo(4000, 2000);
 LineTo(400, 2000);
 for(px = 400; px <= 4000; px += 10)
 {
  py = 2000 - 1000 * sin(((px - 400) / 2.5) / 180 * PI);
  LineTo(px, py);
 }
 MoveTo(px, py);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ݰ���������ر���ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SendReport(void)
{
 if(KeyDown & (KEY1))
 {
  MoveTo(4096 / 2, 4096 /2); //�ƶ�����Ļ����
 }
 if(KeyDown & (KEY2)) //��ֱ��
 {
  MoveTo(1000, 1000); //�ƶ�����1000��1000��
  LineTo(1000, 1000); //��ʼ����
  LineTo(3000, 3000); //���ߵ���3000��3000��
  MoveTo(3000, 3000); //�ɿ�������
 }
 if(KeyDown & (KEY3)) //��������
 {
  MoveTo(2000, 1000); //�ƶ�����2000��1000��
  LineTo(2000, 1000); //��ʼ����
  LineTo(1000, 3000); //���ߵ���1000��3000��
  LineTo(3000, 3000); //���ߵ���3000��3000��
  LineTo(2000, 1000); //���ߵ���2000��1000��
  MoveTo(2000, 1000); //�ɿ�������
 }
 if(KeyDown & (KEY4)) //��������
 {
  MoveTo(1000, 1000); //�ƶ�����1000��1000��
  LineTo(1000, 1000); //��ʼ����
  LineTo(1000, 3000); //���ߵ���1000��3000��
  LineTo(3000, 3000); //���ߵ���3000��3000��
  LineTo(3000, 1000); //���ߵ���3000��1000��
  LineTo(1000, 1000); //���ߵ���1000��1000��
  MoveTo(1000, 1000); //�ɿ�������
 }
 if(KeyDown & (KEY5))
 {
  DrawCirle(2000, 2000, 1000); //��һ��Բ���ڣ�2000��2000�����뾶Ϊ1000��Բ
 }
 if(KeyDown & (KEY6))
 {
  DrawSinCurve(); //����������
 }
 //�ǵ����KeyUp��KeyDown
 KeyUp=0;
 KeyDown=0;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�������㴥���¼���
��ڲ�����x��x���������飻y��y���������飻
          s��״̬���飬�����Ƿ������Ƿ���Ч��n�������ĵ�����
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointReport(uint16 *x, uint16 *y, uint8 *s, uint8 n)
{
 uint8 i;
 uint8 ReportBuf[14]; //�ܹ�Ϊ14�ֽڣ���1�ֽ�Ϊ����ID��
 //��2�ֽ�Ϊ��һ��״̬����3�ֽ�Ϊ��һ��Ĵ���ID�ţ�
 //��4��5�ֽ�Ϊ��һ��x�ᣬ��6��7�ֽ�Ϊ��һ��y�᣻
 //��8�ֽ�Ϊ�ڶ���״̬����9�ֽ�Ϊ�ڶ���Ĵ���ID�ţ�
 //��10��11�ֽ�Ϊ�ڶ���x�ᣬ��12��13�ֽ�Ϊ�ڶ���y�᣻
 //��14�ֽ�Ϊ��ǰ�����ĵ��������Ƕ���ı����У�ÿ��ֻ��
 //����2���㣬������������㣬�����������Ӷ���ı��棬
 //��ʱ����ı���Ĵ���������Ҫ����Ϊ0��
 if(n == 0) return;
 if(n > MAX_TOUCH_POINT) //����������֧�ֵĵ�������ֻ����������
 {
  n = MAX_TOUCH_POINT;
 }
 ReportBuf[0] = REPORTID_MTOUCH; //��㱨��ı���IDΪREPORTID_MTOUCH
 for(i = 0; i < n;) //�ֱ��͸�����
 {
  ReportBuf[1] = s[i]; //״̬
  ReportBuf[2] = i + 1; //ID��
  ReportBuf[3] = x[i] & 0xFF; //X���8λ
  ReportBuf[4] = (x[i] >> 8) & 0xFF; //X���8λ
  ReportBuf[5] = y[i] & 0xFF; //Y���8λ
  ReportBuf[6] = (y[i] >> 8) & 0xFF; //Y���8λ
  if(i == 0) //��һ����
  {
   ReportBuf[13] = n; //�����ĵ���
  }
  else //������������Ϊ0
  {
   ReportBuf[13] = 0;
  }
  i ++;
  if(i < n) //����������Ҫ����
  {
   ReportBuf[7] = s[i]; //״̬
   ReportBuf[8] = i + 1; //ID��
   ReportBuf[9] = x[i] & 0xFF; //X���8λ
   ReportBuf[10] = (x[i] >> 8) & 0xFF; //X���8λ
   ReportBuf[11] = y[i] & 0xFF; //Y���8λ
   ReportBuf[12] = (y[i] >> 8) & 0xFF; //Y���8λ
   i ++;
  }
  else //û�и����������Ҫ���ͣ��������0
  {
   uint8 j;
   for(j = 7; j < 13; j++)
   {
    ReportBuf[j] = 0;
   }
  }
  while(Ep1InIsBusy) //�ȴ�֮ǰ�����ݷ������
  {
   ProcessInterrupt();  //�����ж�
  }
  if(ConfigValue == 0) return;
  //����׼�����ˣ�ͨ���˵�1���أ�����Ϊ14�ֽڡ�
  D12WriteEndpointBuffer(3, 14, ReportBuf);
  Ep1InIsBusy=1;  //���ö˵�æ��־��
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ģʽ�����Ļ���롣
��ڲ�����touch: 0��ʾ�ɿ���1��ʾ������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointGoToCenter(uint8 touch)
{
 uint16 x, y;
 uint8 s;
 x = 4096 / 2;
 y = 4096 / 2;
 if(touch)
 {
  s = 0x07; //���
 }
 else
 {
  s = 0x00; //�ɿ�
 }
 MultiPointReport(&x, &y, &s, 1);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ģʽ��һ���߶Ρ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointDrawLine()
{
 uint16 x, y;
 uint8 s;
 x = 1000;
 y = 1000;
 s = 0x06; //������Ч�������޵��
 MultiPointReport(&x, &y, &s, 1); //�ƶ���(1000, 1000)
 s = 0x07; //������Ч�����е��
 MultiPointReport(&x, &y, &s, 1); //��ʼ����
 x = 3000;
 y = 3000;
 MultiPointReport(&x, &y, &s, 1); //����(3000, 3000)
 s = 0x00; //ֹͣ����
 MultiPointReport(&x, &y, &s, 1);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�ͬʱ�����������Ρ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointDrawTriangle()
{
 uint16 x[2], y[2];
 uint8 s[2];
 x[0] = 2000;
 y[0] = 1000;
 s[0] = 0x06;
 x[1] = x[0];
 y[1] = y[0] + 500;
 s[1] = 0x06;
 MultiPointReport(x, y, s, 2); //�ƶ������
 s[0] = 0x07;
 s[1] = 0x07;
 MultiPointReport(x, y, s, 2); //��ʼ����
 x[0] = 1000;
 y[0] = 3000;
 x[1] = x[0] + 500 * cos(30 * PI / 180);
 y[1] = y[0] - 500 * sin(30 * PI / 180);
 MultiPointReport(x, y, s, 2); //�����������
 x[0] = 3000;
 y[0] = 3000;
 x[1] = x[0] - 500 * cos(30 * PI / 180);
 y[1] = y[0] - 500 * sin(30 * PI / 180);
 MultiPointReport(x, y, s, 2); //���ױ�������
 x[0] = 2000;
 y[0] = 1000;
 x[1] = x[0];
 y[1] = y[0] + 500;
 MultiPointReport(x, y, s, 2); //���ұ�������
 s[0] = 0;
 s[1] = 0;
 MultiPointReport(x, y, s, 2); //ֹͣ����
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�ͬʱ���ĸ������Ρ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointDrawSquare()
{
 uint16 x[4], y[4];
 uint8 s[4];
 uint8 i;
 x[0] = 1000;
 y[0] = 1000;
 s[0] = 0x06;
 for(i = 1; i < 4; i ++)
 {
  x[i] = x[i - 1] + 100;
  y[i] = y[i - 1] + 100;
  s[i] = s[0];
 }
 MultiPointReport(x, y, s, 4); //�ƶ������
 for(i = 0; i < 4; i ++)
 {
  s[i] = 0x07;
 }
 MultiPointReport(x, y, s, 4); //��ʼ����
 y[0] = 3000;
 y[1] = 3000 - 100;
 y[2] = 3000 - 200;
 y[3] = 3000 - 300;
 MultiPointReport(x, y, s, 4); //�����4����
 x[0] = 3000;
 x[1] = 3000 - 100;
 x[2] = 3000 - 200;
 x[3] = 3000 - 300;
 MultiPointReport(x, y, s, 4); //�����4����
 y[0] = 1000;
 y[1] = 1000 + 100;
 y[2] = 1000 + 200;
 y[3] = 1000 + 300;
 MultiPointReport(x, y, s, 4); //���ұ�4����
 x[0] = 1000;
 x[1] = 1000 + 100;
 x[2] = 1000 + 200;
 x[3] = 1000 + 300;
 MultiPointReport(x, y, s, 4); //���ϱ�4����
 for(i = 0; i < 4; i ++)
 {
  s[i] = 0;
 }
 MultiPointReport(x, y, s, 4); //ֹͣ����
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�ͬʱ������Բ��
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointDrawCircle(void)
{
 #define CENTER_X 2000
 #define CENTER_Y 2000
 #define R0       1000
 #define R1       800
 #define R2       600
 
 uint16 x[3], y[3];
 uint8 s[3];
 uint16 i;
 float vsin, vcos;
 
 x[0] = CENTER_X + R0;
 x[1] = CENTER_X + R1;
 x[2] = CENTER_X + R2;
 for(i = 0; i < 3; i ++)
 {
  y[i] = CENTER_Y;
  s[i] = 0x06;
 }
 MultiPointReport(x, y, s, 3); //�ƶ������
 for(i = 0; i < 3; i ++)
 {
  s[i] = 0x07;
 }
 MultiPointReport(x, y, s, 3); //��ʼ����
 for(i = 0; i < 360; i ++)
 {
  vsin = sin((i * 1.0) / 180 * PI);
  vcos = cos((i * 1.0) / 180 * PI);
  x[0] = CENTER_X + R0 * vcos;
  y[0] = CENTER_Y + R0 * vsin;
  x[1] = CENTER_X + R1 * vcos;
  y[1] = CENTER_Y + R1 * vsin;
  x[2] = CENTER_X + R2 * vcos;
  y[2] = CENTER_Y + R2 * vsin;
  MultiPointReport(x, y, s, 3); //����
 }
 for(i = 0; i < 3; i ++)
 {
  s[i] = 0x00;
 }
 MultiPointReport(x, y, s, 3); //ֹͣ����
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�ͬʱ�����ҡ��������ߡ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointDrawSinCosCurve()
{
 uint16 x[2], y[2];
 uint8 s[2];
 uint16 i;
 x[0] = 400;
 y[0] = 2000;
 x[1] = 400;
 y[1] = 2000;
 s[0] = 0x06;
 s[1] = 0x06;
 MultiPointReport(x, y, s, 2); //�ƶ������
 s[0] = 0x07;
 s[1] = 0x07;
 MultiPointReport(x, y, s, 2); //��ʼ����
 for(i = 400; i <= 4000; i += 10)
 {
  x[0] = i;
  x[1] = i;
  y[0] = 2000 - 1000 * sin(((i - 400) / 2.5) / 180 * PI);
  y[1] = 2000 - 1000 * cos(((i - 400) / 2.5) / 180 * PI);
  MultiPointReport(x, y, s, 2); //����
 }
 s[0] = 0x00;
 s[1] = 0x00;
 MultiPointReport(x, y, s, 2); //ֹͣ����
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����ݰ���������ض�㱨��ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void MultiPointSendReport(void)
{
 if(KeyUp & (KEY1))
 {
  MultiPointGoToCenter(0); //�ɿ�
 }
 if(KeyPress & (KEY1)) //����KEY1�������Ҽ�
 {
  MultiPointGoToCenter(1); //���ģʽ�����Ļ����
 }
 if(KeyDown & (KEY2))
 {
  MultiPointDrawLine(); //���ģʽ��һ��ֱ��
 }
 if(KeyDown & (KEY3))
 {
  MultiPointDrawTriangle(); //ͬʱ������������
 }
 if(KeyDown & (KEY4))
 {
  MultiPointDrawSquare(); //ͬʱ���ĸ�������
 }
 if(KeyDown & (KEY5))
 {
  MultiPointDrawCircle(); //ͬʱ������Բ
 }
 if(KeyDown & (KEY6))
 {
  MultiPointDrawSinCosCurve(); //ͬʱ�����ҡ���������
 }
 //�ǵ����KeyUp��KeyDown
 KeyUp=0;
 KeyDown=0;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void main(void)  //������
{
#ifdef SHOW_MORE_MSG //��ʡROM�ռ�
 uint8 i;
#endif

 uint16 id;
 
 EA=1; //���ж�
 InitKeyboard(); //��ʼ������
 InitUART();  //��ʼ������
 
#ifdef SHOW_MORE_MSG //��ʡROM�ռ�
 for(i=0;i<18;i++)	  //��ʾ��Ϣ
 {
  Prints(HeadTable[i]);
 }
#endif
 
 id=D12ReadID();

 Prints("Your D12 chip\'s ID is: ");
 PrintShortIntHex(id);

#ifdef SHOW_MORE_MSG //��ʡROM�ռ�
 if(id==0x1012)
 {
  Prints(". ID is correct! Congratulations!\r\n\r\n");
 }
 else
 {
  Prints(". ID is incorrect! What a pity!\r\n\r\n");
 }
#endif

 UsbDisconnect();  //�ȶϿ�USB����
 UsbConnect();  //��USB������
 ConfigValue=0; //����ֵ��ʼ��Ϊ0
 
 while(1)  //��ѭ��
 {
  ProcessInterrupt(); //�����ж�
  if(ConfigValue!=0) //����Ѿ�����Ϊ��0�����ã�����Է��ر�������
  {
   LEDs=~KeyPress;  //���ð���8��LED��ʾ����״̬������ʱ��
   if(!Ep1InIsBusy)  //����˵�1����û�д���æ״̬������Է�������
   {
    KeyCanChange=0;  //��ֹ����ɨ��
    if(KeyUp || KeyDown || KeyPress) //����а����¼�����
    {
     if(TouchMode == MULTI_TOUCH_MODE) //��㴥��ģʽ
     {
      MultiPointSendReport();  //�򷵻ض�㱨��
     }
     else //���򵥵�ģʽ
     {
      SendReport();  //�򷵻ص��㱨��
     }
    }
    KeyCanChange=1;  //������ɨ��
   }
  }
 }
}
////////////////////////End of function//////////////////////////////
