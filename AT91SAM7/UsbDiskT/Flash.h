/******************************************************************
  ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
  
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
                             
 Flash.h  file
    
���ߣ�Computer-lov
��������: 2009-03-18
�޸�����: 2009-03-18
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#ifndef __FLASH_H__
#define __FLASH_H__

#include "MyType.h"

#define FlashGetRB() ((*AT91C_PIOA_PDSR)&(1<<29))

#define FlashGetData() (((*AT91C_PIOA_PDSR)>>17)&0xFF)

/*
#define FlashSetData(Val) {*AT91C_PIOA_CODR=(0xFF<<17); \
                           *AT91C_PIOA_SODR=(((uint32)((Val)&0xFF))<<17);}
*/

#define FlashSetData(Val) {*AT91C_PIOA_ODSR=(((uint32)(Val))<<17);}

#define FlashSetPortAsIn()  *AT91C_PIOA_ODR=(0xFF<<17)
#define FlashSetPortAsOut() *AT91C_PIOA_OER=(0xFF<<17)

#define FlashSetCle() *AT91C_PIOA_SODR=(1<<15)
#define FlashClrCle() *AT91C_PIOA_CODR=(1<<15)

#define FlashSetAle() *AT91C_PIOA_SODR=(1<<2)
#define FlashClrAle() *AT91C_PIOA_CODR=(1<<2)

#define FlashSetWe()  *AT91C_PIOA_SODR=(1<<26)
#define FlashClrWe()  *AT91C_PIOA_CODR=(1<<26)

#define FlashSetRe()  *AT91C_PIOA_SODR=(1<<27)
#define FlashClrRe()  *AT91C_PIOA_CODR=(1<<27)

#define FlashSetCe()  *AT91C_PIOA_SODR=(1<<28)
#define FlashClrCe()  *AT91C_PIOA_CODR=(1<<28)

#define FlashWait() while(FlashGetRB()==0)

//��һ�ֽ�
#define FlashReadByte(Val) {FlashClrRe(); \
                            Val=FlashGetData(); \
                            FlashSetRe();}

//дһ�ֽ�
#define FlashWriteByte(Val) {FlashClrWe(); \
                             FlashSetData((Val)); \
                             FlashSetWe();}


//FLASH����ܹ��ﵽ�ĵ�ַ����128M
#define FLASH_MAX_ADDR 0x7FFFFFF

//FLASH���С��Ϊ128KB
#define FLASH_BLOCK_SIZE 0x20000

//FLASHҳ��С��Ϊ2KB
#define FLASH_PAGE_SIZE 0x800


//������С
#define FLASH_SECTOR_SIZE 0x200

//�����������Ŀ���
#define FLASH_SWAP_BLOCKS 10

//������������Ӱ��Ŀ���
#define FLASH_BAD_BLOCKS_REMAP 50

//���滵���Ŀ���
#define FLASH_BLOCKS_TABLE 3

//����ܹ��ﵽ��������ַ
#define FLASH_MAX_SECTOR_ADDR (FLASH_MAX_ADDR-(FLASH_BAD_BLOCKS_REMAP+FLASH_SWAP_BLOCKS+FLASH_BLOCKS_TABLE)*FLASH_BLOCK_SIZE)

//����������ʼ��ַ
#define FLASH_SWAP_BLOCK_ADDR (FLASH_MAX_ADDR+1-FLASH_SWAP_BLOCKS*FLASH_BLOCK_SIZE)

//��Ӱ�仵�����ʼ��ַ
#define FLASH_BAD_BLOCK_REMAP_ADDR (FLASH_MAX_ADDR+1-(FLASH_BAD_BLOCKS_REMAP+FLASH_SWAP_BLOCKS)*FLASH_BLOCK_SIZE)

//���屣�滵������ʼ��ַ
#define FLASH_BLOCK_TABLE_ADDR (FLASH_MAX_ADDR+1-(FLASH_BAD_BLOCKS_REMAP+FLASH_SWAP_BLOCKS+FLASH_BLOCKS_TABLE)*FLASH_BLOCK_SIZE)

//Ӱ������״̬��������
#define FLASH_BLOCK_OK    0
#define FLASH_BLOCK_BAD   1
#define FLASH_BLOCK_USED  2

void FlashReset(void);
void FlashLoadBadBlockTable(void);
void FlashInit(void);
uint8 FlashEraseBlock(uint32 Addr);
uint32 FlashManageSwapBlock(uint32 Op);
uint32 FlashWriteOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain);
uint32 FlashReadOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain);
void FlashReadId(uint8 *Buf);
void FlashSaveBadBlockTable(void);


/********************************************************************
�������ܣ���ȡ��һ�����õĽ������ַ��
��ڲ������ޡ�
��    �أ���һ�����õĽ�����ĵ�ַ��
��    ע���ޡ�
********************************************************************/
#define FlashSwapBlockInit()  FlashManageSwapBlock(0)
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ȡ��һ�����õĽ������ַ��
��ڲ������ޡ�
��    �أ���һ�����õĽ�����ĵ�ַ��
��    ע���ޡ�
********************************************************************/
#define FlashGetNextSwapBlock()  FlashManageSwapBlock(1)
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ȡ��ǰ����ʹ�õĽ������ַ��
��ڲ������ޡ�
��    �أ���һ�����õĽ�����ĵ�ַ��
��    ע���ޡ�
********************************************************************/
#define FlashGetCurrentSwapBlock()  FlashManageSwapBlock(2)
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���־��ǰ������Ϊ���顣
��ڲ������ޡ�
��    �أ���һ�����õĽ�����ĵ�ַ��
��    ע���ޡ�
********************************************************************/
#define FlashMarkBadCurrentSwapBlock()  FlashManageSwapBlock(3)
/////////////////////////End of function/////////////////////////////

extern uint8 FlashSectorBuf[FLASH_SECTOR_SIZE];

#endif