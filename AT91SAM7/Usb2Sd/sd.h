/******************************************************************
  ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
  
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
		��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
		                     http://computer00.21ic.org
							 
 SD.h  file
	
���ߣ�Computer-lov
��������: 2009-04-07
�޸�����: 2009-04-07
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#ifndef __SD_H__
#define __SD_H__

#include "my_type.h"

#define SECTOR_SIZE 512  //������СΪ512�ֽ�

extern uint8 SdSectorBuf[SECTOR_SIZE]; //��д�����õĻ�����
extern uint32 SdCapacity; //����������ע�ⵥλΪ������

void SdInit(void);
uint32 SdReadOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain);
uint32 SdWriteOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain);

#endif