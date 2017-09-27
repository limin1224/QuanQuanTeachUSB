/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
          ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                               http://computer00.21ic.org
                               
FFT.h file
                              
���ߣ�Computer-lov
��������: 2009-03-06
�޸�����: 2009-03-06
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#ifndef __FFT_H__
#define __FFT_H__

#include <math.h>

//������FFT�ĵ���
#define LENGTH 512
//������ô����FFT��Ҫ���ٸ�������λ
//bL=log2(LENGTH)
#define bL 9

#define IN_TYPE  short int
#define OUT_TYPE long int
#define LEN_TYPE short int

#define PI 3.1415926535897932384626433832795

void InitBitRev(void);
void FftInput(IN_TYPE *pIn);
void FftExe(IN_TYPE *pIn, OUT_TYPE *pRe, OUT_TYPE *pIm);
void DftExe(IN_TYPE *pIn, OUT_TYPE *pRe, OUT_TYPE *pIm);

#endif