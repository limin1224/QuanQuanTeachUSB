/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

ShowSpectrum.h file

���ߣ�Computer-lov
��������: 2009-03-04
�޸�����: 2009-03-10
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/


#ifndef __SHOW_SPECTRUM_H__
#define __SHOW_SPECTRUM_H__

#include "MyType.h"
#include "FFT.h"

void ShowSpectrum(void);

extern int16 AudioOutBuf[650];   //��Ҫ����512��
extern uint32 AudioOutByteCount;  //����ֽڼ�����

#endif