/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

ShowSpectrum.c  file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-03-10
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include "ShowSpectrum.h"
#include "Lcd.h"

OUT_TYPE Re[LENGTH];
OUT_TYPE Im[LENGTH];

unsigned char Peak[64];
unsigned char PeakTime[64];

#define MAX_HEIGHT 39

#define HOLD_TIME 3   //��ֵ����ʱ��

/********************************************************************
�������ܣ�����Ҫ��ʾ�ķ�ֵ���д���
��ڲ�����x�������ꣻy��������ֵ��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void ProcPeak(unsigned char x, unsigned char y)
{
 if(PeakTime[x]>0)
 {
  PeakTime[x]--;
 }
 else
 {
  if(Peak[x]<MAX_HEIGHT)Peak[x]+=2;
 }
 if(y<Peak[x])
 {
  Peak[x]=y; //�޸ķ�ֵ
  PeakTime[x]=HOLD_TIME; //����
 }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�����ѹ����
��ڲ�����y������ķ��ȡ�
��    �أ��ޡ�
��    ע��ѹ����ķ��ȡ�
********************************************************************/
int Compress(int y)
{
 return sqrt(y);  //������ѹ��
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���LCD����ʾƵ�ס�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void ShowSpectrum(void)
{
 int i,x,y,j,p;
 
 //�������źŽ�����С
 for(i=0;i<LENGTH;i++)
 {
  AudioOutBuf[i]/=32;
 }
 
 FftInput(AudioOutBuf);       //λ����
 FftExe(AudioOutBuf,Re,Im);   //��FFT����
  
 LcdCls();    //����
 
 //��ʾX��Y������
 LcdSetPoint(0,35);
 LcdPrints("0V");
 LcdDrawLine(13,39,15,39);
 LcdSetPoint(0,23);
 LcdPrints("1V");
 LcdDrawLine(13,27,15,27);
 LcdSetPoint(0,11);
 LcdPrints("2V");
 LcdDrawLine(13,15,15,15);
 LcdSetPoint(0,0);
 LcdPrints("3V");
 LcdDrawLine(13,3,15,3);
 
 LcdSetPoint(14,40);
 LcdPrints("0");
 LcdSetPoint(13+26,40);
 LcdPrints("3K");
 LcdSetPoint(84-20,40);
 LcdPrints("16K");
 
 //ֱ������ֱ����ʾ
 x=17;
 y=MAX_HEIGHT-(Re[0]*MAX_HEIGHT/LENGTH)/1024;
 LcdDrawLine(x,y,x,MAX_HEIGHT);
 ProcPeak(0,y); //�����ֵ

 //ÿ1����ʾ
 for(i=1;i<14;i++)
 {
  x++;
  y=sqrt(Re[i]*Re[i]+Im[i]*Im[i]);   //����ģֵ
  y/=10;
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y); //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }
 
 //ÿ2����ʾ
 p=14;
 for(i=14;i<24;i++)
 {
  x++;
  y=0;
  for(j=0;j<2;j++)
  {
   y+=sqrt(Re[p]*Re[p]+Im[p]*Im[p]);   //����ģֵ
   p++;
  }
  y/=20; //ȡƽ��ֵ
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y); //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }

 //ÿ3����ʾ
 for(i=24;i<34;i++)
 {
  x++;
  y=0;
  for(j=0;j<3;j++)
  {
   y+=sqrt(Re[p]*Re[p]+Im[p]*Im[p]);   //����ģֵ
   p++;
  }
  y/=30; //ȡƽ��ֵ
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y); //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }

 //ÿ4����ʾ
 for(i=34;i<44;i++)
 {
  x++;
  y=0;
  for(j=0;j<4;j++)
  {
   y+=sqrt(Re[p]*Re[p]+Im[p]*Im[p]);   //����ģֵ
   p++;
  }
  y/=40; //ȡƽ��ֵ
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y); //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }
 
 //ÿ6����ʾ
 for(i=44;i<54;i++)
 {
  x++;
  y=0;
  for(j=0;j<6;j++)
  {
   y+=sqrt(Re[p]*Re[p]+Im[p]*Im[p]);   //����ģֵ
   p++;
  }
  y/=60; //ȡƽ��ֵ
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y); //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }
 
 //ÿ9����ʾ
 for(i=54;i<64;i++)
 {
  x++;
  y=0;
  for(j=0;j<9;j++)
  {
   y+=sqrt(Re[p]*Re[p]+Im[p]*Im[p]);   //����ģֵ
   p++;
  }
  y/=90; //ȡƽ��ֵ
  y=Compress(y);  //ѹ��
  y=MAX_HEIGHT-y;  //����Ϊ��Ļy��λ��
  ProcPeak(i,y);   //�����ֵ
  LcdDrawLine(x,y,x,MAX_HEIGHT);     //����
 }
 
 for(i=0;i<64;i++)
 {
  LcdDrawPoint(i+17,Peak[i]);
 }
 LcdRefresh(); //ˢ��
}
/////////////////////////End of function/////////////////////////////