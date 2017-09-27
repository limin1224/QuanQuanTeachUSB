/******************************************************************
  ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
  
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org
                             
 Flash.c  file
    
���ߣ�Computer-lov
��������: 2009-03-18
�޸�����: 2009-03-26
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include <AT91SAM7S64.H>
#include "Flash.h"
#include "MyType.h"

uint32 FlashCurrentWriteSectorAddr; //��ǰд��������ַ
uint32 FlashCurrentReadSectorAddr;  //��ǰ����������ַ
uint32 FlashNeedWriteBack; //��Ҫ��д�ı�־

uint8 FlashSectorBuf[FLASH_SECTOR_SIZE]; //��д�����õĻ�����

static uint32 FlashBadBlockTable[2][FLASH_BAD_BLOCKS_REMAP+1]; //����������ı��
static uint32 FlashBadBlocksCount;  //�ж��ٸ�����
static uint8 FlashRemapBlockStatus[FLASH_BAD_BLOCKS_REMAP+1]; //������Ӱ���õĿ��״��
static uint32 FlashLastAccessAddr; //���һ�η��ʹ��ĵ�ַ

/********************************************************************
�������ܣ�FLASHд���
��ڲ�����Val��Ҫд������ֵ��
��    �أ��ޡ�
��    ע��û��Ƭѡ���������������߲�ĺ�������Ƭѡ��
********************************************************************/
void FlashWriteCommand(uint8 Cmd)
{
 FlashSetCle(); //ѡ��Ϊ����
 FlashSetPortAsOut();
 FlashWriteByte(Cmd);
 FlashSetPortAsIn();
 FlashClrCle();
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASHд�����ֽڵ�ַ�ĺ�����
��ڲ�����Addr��Ҫд����ֽڵ�ַ��
��    �أ��ޡ�
��    ע��û��Ƭѡ���������������߲�ĺ�������Ƭѡ��ע���ַ�м��0��
********************************************************************/
void FlashWriteAddr4Byte(uint32 Addr)
{
 uint i;
 
 //ע�⣺��FLASH�ĵ�ַ�м���5bit������0
 //������16λ����5λ
 Addr=((Addr<<5)&(~0xFFFF))|(Addr&0x07FF);

 FlashSetAle();  //ALE�øߣ�ѡ��Ϊ��ַ
 FlashSetPortAsOut(); //���ö˿�Ϊ���״̬
 for(i=0;i<4;i++) //д�����ֽڵĵ�ַ
 {
  FlashWriteByte(Addr); //дһ�ֽ�����
  Addr>>=8;
 }
 FlashSetPortAsIn(); //���ö˿�Ϊ����
 FlashClrAle();  //ALE�õ�
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASHд����ֽڵ�ַ�ĺ�����
��ڲ�����Addr��Ҫд����ֽڵ�ַ��
��    �أ��ޡ�
��    ע��û��Ƭѡ���������������߲�ĺ�������Ƭѡ��ֻд��ַ��λ
********************************************************************/
void FlashWriteAddr2Byte(uint32 Addr)
{
 uint i;
 
 //ֻ��Ҫ��11λ
 Addr=Addr&(FLASH_PAGE_SIZE-1);

 FlashSetAle();  //ALE�øߣ�ѡ��Ϊ��ַ
 FlashSetPortAsOut(); //���ö˿�Ϊ���״̬
 for(i=0;i<2;i++)     //д����ֽڵĵ�ַ
 {
  FlashWriteByte(Addr);  //дһ�ֽ�����
  Addr>>=8;
 }
 FlashSetPortAsIn();  //���ö˿�Ϊ����
 FlashClrAle();       //ALE�õ�
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASHд����ֽ�ҳ��ַ�ĺ�����
��ڲ�����Addr��Ҫд����ֽڵ�ַ��
��    �أ��ޡ�
��    ע��û��Ƭѡ���������������߲�ĺ�������Ƭѡ��
********************************************************************/
void FlashWritePageAddr(uint32 Addr)
{
 uint i;
 
 //����ҳ��ַ
 Addr/=FLASH_PAGE_SIZE;

 FlashSetAle();  //ALE�øߣ�ѡ��Ϊ��ַ
 FlashSetPortAsOut(); //���ö˿�Ϊ���״̬
 for(i=0;i<2;i++) //д�����ֽڵĵ�ַ
 {
  FlashWriteByte(Addr); //дһ�ֽ�����
  Addr>>=8;
 }
 FlashSetPortAsIn(); //���ö˿�Ϊ����
 FlashClrAle(); //ALE�õ�
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�Flash��λ��
��ڲ������ޡ�
��    �أ��ޡ�
��    ע��û��Ƭѡ���������������߲�ĺ�������Ƭѡ��
********************************************************************/
void FlashReset(void)
{
 FlashWriteCommand(0xFF); //д��λ����
 FlashWait();  //�ȴ��������
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�Flash��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashInit(void)
{
 *AT91C_PIOA_PER=(1<<2)|(1<<15)|(1<<26)|(1<<27)|(1<<28)|(0xFF<<17);  //Enable Flash IO
 *AT91C_PIOA_OER=(1<<2)|(1<<15)|(1<<26)|(1<<27)|(1<<28);  //Output Enable Flash IO
 *AT91C_PIOA_ODR=(1<<29);  //Output Disable PA29, R/B
 *AT91C_PIOA_SODR=(1<<26)|(1<<27)|(1<<28); //Set IO
 *AT91C_PIOA_OWER=(0xFF<<17);  //ʹ��ֱ�Ӳ����˿ڵ�ģʽ
 FlashClrCle();
 FlashClrAle();
 FlashClrCe(); //ѡ��оƬ 
 FlashReset(); //FLASH��λ
 FlashSetCe(); //�ͷ�оƬ 
 FlashCurrentWriteSectorAddr=-1; //��ʼ����ǰ����д��ַΪ��Ч
 FlashCurrentReadSectorAddr=-1;
 FlashNeedWriteBack=0;  //��ʼ�����û�д
 FlashSwapBlockInit();  //��ʼ��������
 FlashClrCe(); //ѡ��оƬ 
 FlashLoadBadBlockTable(); //��FLASH�м��ػ����
 FlashSetCe(); //�ͷ�оƬ 
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ȡ״̬�Ĵ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע��FLASH����״̬��
********************************************************************/
uint8 FlashReadStatus(void)
{
 uint8 Status;
 FlashWriteCommand(0x70);  //д��ѯ����
 FlashReadByte(Status);    //����״̬
 return Status;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�����һ���顣
��ڲ�����Addr���ֽڵ�ַ��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
uint8 FlashEraseBlock(uint32 Addr)
{
 //���������һ�ֽ�����
 FlashWriteCommand(0x60);
 //д�����ֽڵ�ҳ��ַ
 FlashWritePageAddr(Addr);
 //д���������ڶ��ֽ�
 FlashWriteCommand(0xD0);
 //�ȴ��������
 FlashWait();
 //���ز���״̬
 return FlashReadStatus();
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�ҳд����ĺ�벿�֡�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע��������״̬��
********************************************************************/
uint8 FlashWritePage(void)
{
 FlashWriteCommand(0x10);  //ҳд����
 FlashWait();  //�ȴ�д���
 return FlashReadStatus();
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�����һҳ���ݡ�
��ڲ�����sAddr��Դ��ַ��dAddr��Ŀ���ַ��
��    �أ��ޡ�
��    ע��������״̬��
********************************************************************/
uint8 FlashCopyPage(uint32 sAddr, uint32 dAddr)
{
 //��Դ���ݵ��ڲ�����
 FlashWriteCommand(0x00);
 FlashWriteAddr4Byte(sAddr);
 FlashWriteCommand(0x35);
 FlashWait();
 
 //������д��Ŀ��ҳ
 FlashWriteCommand(0x85);
 FlashWriteAddr4Byte(dAddr);
 FlashWriteCommand(0x10);
 FlashWait();
 return FlashReadStatus();
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ȡ��һ�����õı��ÿ顣
��ڲ������ޡ�
��    �أ��ҵ��Ŀ��ַ��
��    ע���ڸú����л��Ȳ�����ֻ�гɹ������Ĳű����ء�
********************************************************************/
uint32 FlashGetNewRemapBlock(void)
{
 uint32 i,Addr;
 for(i=0;i<FLASH_BAD_BLOCKS_REMAP;i++)
 {
  if(FLASH_BLOCK_OK==FlashRemapBlockStatus[i]) //����ÿ黹δ��
  {
   Addr=FLASH_BAD_BLOCK_REMAP_ADDR+i*FLASH_BLOCK_SIZE; //�����ַ
   if(0x01==(FlashEraseBlock(Addr)&0x01))  //�������ʧ��
   {
    FlashRemapBlockStatus[i]=FLASH_BLOCK_BAD;  //��־�ÿ�Ϊ�Ѿ���
   }
   else //���򣬲����ɹ�
   {
    FlashRemapBlockStatus[i]=FLASH_BLOCK_USED; //��־Ϊ�ÿ��ѱ�ʹ��    
    return Addr; //���ص�ַ
   }
  }
 }
 return -1; //����Ҳ������򷵻�-1��
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���־��ǰ��Ӱ��Ŀ�Ϊ���顣
��ڲ�����Addr��Ҫ��־�Ŀ�ĵ�ַ��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashMarkRemapBlockBad(uint32 Addr)
{
 uint32 i;
 i=(Addr-FLASH_BAD_BLOCK_REMAP_ADDR)/FLASH_BLOCK_SIZE;  //����ƫ����
 if(i>=FLASH_BAD_BLOCKS_REMAP)return; //����
 FlashRemapBlockStatus[i]=FLASH_BLOCK_BAD;  //��־Ϊ�Ѿ���
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ַ����Ӱ�䣬��������á�
��ڲ�����Addr����ҪӰ����ֽڵ�ַ��
��    �أ�Ӱ�����ֽڵ�ַ��
��    ע���ޡ�
********************************************************************/
uint32 FlashAddrRemap(uint32 Addr)
{
 static uint32 CurrentRemapBlockAddr;
 uint32 i,j;
 
 if(0==FlashBadBlocksCount)  //�����������Ϊ0������Ҫ����ֱ�ӷ��ص�ַ
 {
  return Addr;
 }
 
 //������һ�η��ʵĵ�ַ�ͱ��η��ʵĵ�ַ����ͬһ�����ַ����ô����Ҫ����Ӱ��
 if(0==((Addr-FlashLastAccessAddr)&(FLASH_BLOCK_SIZE-1)))
 {
  return CurrentRemapBlockAddr+(Addr&(FLASH_BLOCK_SIZE-1)); //�ɵ�ǰ���ַ���Ͽ���ƫ�Ƶõ�������ַ
 }
 
 FlashLastAccessAddr=Addr; //�������һ�η��ʹ��ĵ�ַ
 
 if(1==FlashBadBlocksCount) //�����������Ϊ1����ֱ��Ӱ��
 {
  if((Addr&(~(FLASH_BLOCK_SIZE-1)))==FlashBadBlockTable[0][0]) //������ַ��ȣ���ֱ��Ӱ��
  {
   CurrentRemapBlockAddr=FlashBadBlockTable[1][0];
   return CurrentRemapBlockAddr+(Addr&(FLASH_BLOCK_SIZE-1)); //�ɵ�ǰ���ַ���Ͽ���ƫ�Ƶõ�������ַ
  }
  else //����Ӱ��
  {
   CurrentRemapBlockAddr=Addr&(~(FLASH_BLOCK_SIZE-1));  //��ȡ��ǰ���ַ
   return Addr;  //ֱ�ӷ���ԭ���ĵ�ַ
  }
 }
 else //������������1
 {
  //�����ַ�ȵ�һ������ĵ�ַ��С���߱����һ������ĵ�ַ����
  //��ô�϶������ǻ��죬����Ҫ����Ӱ��
  if((Addr<FlashBadBlockTable[0][0])
   ||((Addr&(FLASH_BLOCK_SIZE-1))>FlashBadBlockTable[0][FlashBadBlocksCount-1]))
  {
   CurrentRemapBlockAddr=Addr&(~(FLASH_BLOCK_SIZE-1));  //��ȡ��ǰ���ַ
   return Addr;  //ֱ�ӷ���ԭ���ĵ�ַ
  }
  else //���ڻ������䣬ʹ�ö��ֲ�������Ƿ���ҪӰ��
  {
   i=0;
   j=FlashBadBlocksCount-1;
   while(1)
   {
    if((Addr&(~(FLASH_BLOCK_SIZE-1)))==FlashBadBlockTable[0][(i+j)/2]) //�����ȣ���Ӱ��
    {
     CurrentRemapBlockAddr=FlashBadBlockTable[1][(i+j)/2];
     return CurrentRemapBlockAddr+(Addr&(FLASH_BLOCK_SIZE-1)); //�ɵ�ǰ���ַ���Ͽ���ƫ�Ƶõ�������ַ
    }
    if(i==j)break; //���i��j��ȣ����˳�����
    if((Addr&(~(FLASH_BLOCK_SIZE-1)))<FlashBadBlockTable[0][(i+j)/2])  //���С��
    {
     j=(i+j)/2-1; //����ǰ���
    }
    else //�������
    {
     i=(i+j)/2+1; //��������
    }
   }
  }
 }
 //û���ڻ�������ҵ�����˵�����ǻ���
 CurrentRemapBlockAddr=Addr&(~(FLASH_BLOCK_SIZE-1));  //��ȡ��ǰ���ַ
 return Addr;  //ֱ�ӷ���ԭ���ĵ�ַ
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASH���ض�λ�ü��ػ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashLoadBadBlockTable(void)
{
 uint32 i,j,k,Sum,Ok;
 uint8 Data;

 Ok=0; //����Ϊ���ɹ�
 for(i=0;i<FLASH_BLOCKS_TABLE;i++) //����û��׼�������Ŀ�
 {
  //�Ӹÿ������һҳ���ص�һ�ֽڣ����Ƿ�Ϊ0xFF�����Ϊ0xFF����ʾ�ÿ�û��׼������
  FlashWriteCommand(0x00);
  FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - FLASH_PAGE_SIZE);
  FlashWriteCommand(0x30);
  FlashWait(); //�ȴ����ݶ���
  FlashReadByte(Data);
  if(Data==0xFF)  //��ʾ�ÿ����ݻ�δ׼������
  {
   //�Ӹÿ��е����ڶ�ҳ���ص�һ�ֽڣ����Ƿ�Ϊ0�����Ϊ0����ʾ�ÿ��Ѿ�д��������
   FlashWriteCommand(0x00);
   FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - 2*FLASH_PAGE_SIZE);
   FlashWriteCommand(0x30);
   FlashWait(); //�ȴ����ݶ���
   FlashReadByte(Data);
   if(Data==0) //��ʾ������Ч
   {
    FlashReadByte(Data); //����У���
    Sum=Data;
    FlashReadByte(Data); //����У���
    Sum=(Sum<<8)+Data;
    FlashReadByte(Data); //����У���
    Sum=(Sum<<8)+Data;
    FlashReadByte(Data); //����У���
    Sum=(Sum<<8)+Data;
    //�Ӹÿ鿪ʼλ�ö�
    FlashWriteCommand(0x00);
    FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i);
    FlashWriteCommand(0x30);
    FlashWait(); //�ȴ����ݶ���
    //����1�ֽ��Ƿ�Ϊ0
    FlashReadByte(Data);
    if(Data!=0)continue;
    //����2�ֽ��Ƿ�Ϊ0x55
    FlashReadByte(Data);
    if(Data!=0x55)continue;
    //����3�ֽ��Ƿ�Ϊ0xAA
    FlashReadByte(Data);
    if(Data!=0xAA)continue;
    //����4�ֽ��Ƿ�Ϊ0xFF
    FlashReadByte(Data);
    if(Data!=0xFF)continue;
    Sum+=0x1FE;
    
    //����������
    FlashReadByte(Data);
    FlashBadBlocksCount=Data;
    Sum+=Data;
    FlashReadByte(Data);
    FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
    Sum+=Data;
    FlashReadByte(Data);
    FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
    Sum+=Data;
    FlashReadByte(Data);
    FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
    Sum+=Data;
    j=8;
    //���ػ����
    for(k=0;k<sizeof(FlashBadBlockTable[0][0])*FLASH_BAD_BLOCKS_REMAP*2;k++)
    {
     if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ���¶���ҳ
     {
      FlashWriteCommand(0x00);
      FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i+j);
      FlashWriteCommand(0x30);
      FlashWait(); //�ȴ����ݶ���
     }
     FlashReadByte(Data);
     Sum+=Data; //��У���
     ((uint8 *)FlashBadBlockTable)[k]=Data;  //����һ�ֽڵ��������
     j++;
    }
    //������Ӱ������״̬��
    for(k=0;k<sizeof(FlashRemapBlockStatus[0])*FLASH_BAD_BLOCKS_REMAP;k++)
    {
     if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ���¶���ҳ
     {
      FlashWriteCommand(0x00);
      FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i+j);
      FlashWriteCommand(0x30);
      FlashWait(); //�ȴ����ݶ���
     }
     FlashReadByte(Data);
     Sum+=Data; //��У���
     ((uint8 *)FlashRemapBlockStatus)[k]=Data;   //����һ�ֽڵ���Ӱ����״̬����
     j++;
    }
    if(Sum==0) //���У��ɹ�����˵��������ȷ
    {
     Ok=0xFF; //����Ϊ�ɹ�
     break;   //���˳�ѭ��
    }
   }
  }
 }
 
 if(Ok==0) //�������д��ı����Ҳ����õĻ������ȥ׼����������ȥ��
 {
  for(i=0;i<FLASH_BLOCKS_TABLE;i++) //����׼�������Ŀ�
  {
   //�Ӹÿ������һҳ���ص�һ�ֽڣ����Ƿ�Ϊ0�����Ϊ0����ʾ�ÿ��Ѿ�׼��������
   FlashWriteCommand(0x00);
   FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - FLASH_PAGE_SIZE);
   FlashWriteCommand(0x30);
   FlashWait(); //�ȴ����ݶ���
   FlashReadByte(Data);
   if(Data==0x00)  //��ʾ�ÿ�����׼������
   {
    //�Ӹÿ��е����ڶ�ҳ���ص�һ�ֽڣ����Ƿ�Ϊ0�����Ϊ0����ʾ�ÿ��Ѿ�д��������
    FlashWriteCommand(0x00);
    FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - 2*FLASH_PAGE_SIZE);
    FlashWriteCommand(0x30);
    FlashWait(); //�ȴ����ݶ���
    FlashReadByte(Data);
    if(Data==0) //��ʾ������Ч
    {
     FlashReadByte(Data); //����У���
     Sum=Data;
     FlashReadByte(Data); //����У���
     Sum=(Sum<<8)+Data;
     FlashReadByte(Data); //����У���
     Sum=(Sum<<8)+Data;
     FlashReadByte(Data); //����У���
     Sum=(Sum<<8)+Data;
     //�Ӹÿ鿪ʼλ�ö�
     FlashWriteCommand(0x00);
     FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i);
     FlashWriteCommand(0x30);
     FlashWait(); //�ȴ����ݶ���
     //����1�ֽ��Ƿ�Ϊ0
     FlashReadByte(Data);
     if(Data!=0)continue;
     //����2�ֽ��Ƿ�Ϊ0x55
     FlashReadByte(Data);
     if(Data!=0x55)continue;
     //����3�ֽ��Ƿ�Ϊ0xAA
     FlashReadByte(Data);
     if(Data!=0xAA)continue;
     //����4�ֽ��Ƿ�Ϊ0xFF
     FlashReadByte(Data);
     if(Data!=0xFF)continue;
     Sum+=0x1FE;
     
     //����������
     FlashReadByte(Data);
     FlashBadBlocksCount=Data;
     Sum+=Data;
     FlashReadByte(Data);
     FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
     Sum+=Data;
     FlashReadByte(Data);
     FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
     Sum+=Data;
     FlashReadByte(Data);
     FlashBadBlocksCount=(FlashBadBlocksCount<<8)+Data;
     Sum+=Data;
     j=8;
     //���ػ����
     for(k=0;k<sizeof(FlashBadBlockTable[0][0])*FLASH_BAD_BLOCKS_REMAP*2;k++)
     {
      if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ���¶���ҳ
      {
       FlashWriteCommand(0x00);
       FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i+j);
       FlashWriteCommand(0x30);
       FlashWait(); //�ȴ����ݶ���
      }
      FlashReadByte(Data);
      Sum+=Data; //��У���
      ((uint8 *)FlashBadBlockTable)[k]=Data;  //����һ�ֽڵ��������
      j++;
     }
     //������Ӱ������״̬��
     for(k=0;k<sizeof(FlashRemapBlockStatus[0])*FLASH_BAD_BLOCKS_REMAP;k++)
     {
      if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ���¶���ҳ
      {
       FlashWriteCommand(0x00);
       FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i+j);
       FlashWriteCommand(0x30);
       FlashWait(); //�ȴ����ݶ���
      }
      FlashReadByte(Data);
      Sum+=Data; //��У���
      ((uint8 *)FlashRemapBlockStatus)[k]=Data;   //����һ�ֽڵ���Ӱ����״̬����
      j++;
     }
     if(Sum==0) //���У��ɹ�����˵��������ȷ
     {
      FlashSaveBadBlockTable(); //���䱣�浽FLASH��      
      Ok=0xFF; //����Ϊ�ɹ�
      break;   //���˳�ѭ��
     }
    }
   }
  }
 }
 
 if(Ok==0) //�������û�ҵ�����ôֻ�����³�ʼ����
 {
  FlashBadBlocksCount=0; //����������Ϊ0
  for(i=0;i<FLASH_BAD_BLOCKS_REMAP;i++)
  {
   //����Ӱ��鶼����Ϊ�ÿ�
   FlashRemapBlockStatus[i]=FLASH_BLOCK_OK;
   //����Ӱ���ϵ����Ϊ-1
   FlashBadBlockTable[0][i]=-1;
   FlashBadBlockTable[1][i]=-1;
  }
  //���ú�֮�󱣴�����
  FlashSaveBadBlockTable();
 }
 //���õ�ǰ���ʹ��ĵ�ַΪ��Чֵ
 FlashLastAccessAddr=-1;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ����滵���FLASH���ض�λ�á�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashSaveBadBlockTable(void)
{
 uint32 i,j,k,Sum;
 
 for(i=0;i<FLASH_BLOCKS_TABLE;i++) //��־Ϊ׼������
 {
  FlashWriteCommand(0x80);
  FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - FLASH_PAGE_SIZE);
  FlashSetPortAsOut();  //��������Ϊ�����
  FlashWriteByte(0x00);  //����һ�ֽ�����Ϊ0����ʾ׼������
  //ʣ���ֽ�д0xFF
  for(j=1;j<FLASH_PAGE_SIZE;j++)
  {
   FlashWriteByte(0xFF);
  }
  FlashWritePage(); //дҳ
 }
 
 for(i=0;i<FLASH_BLOCKS_TABLE;i++) //�������д��������
 {
  FlashEraseBlock(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i); //����һ��
  FlashWriteCommand(0x80);
  FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i); //д���һ��Ŀ�ʼλ��
  FlashSetPortAsOut();  //��������Ϊ�����
  FlashWriteByte(0x00);  //����1�ֽ�����Ϊ0
  FlashWriteByte(0x55);  //����2�ֽ�����Ϊ0x55
  FlashWriteByte(0xAA);  //����3�ֽ�����Ϊ0xAA
  FlashWriteByte(0xFF);  //����4�ֽ�����Ϊ0xFF
  Sum=0x1FE;
  //����д������������ͳ��У���
  FlashWriteByte((FlashBadBlocksCount>>24)&0xFF);
  Sum+=(FlashBadBlocksCount>>24)&0xFF;
  FlashWriteByte((FlashBadBlocksCount>>16)&0xFF);
  Sum+=(FlashBadBlocksCount>>16)&0xFF;
  FlashWriteByte((FlashBadBlocksCount>>8)&0xFF);
  Sum+=(FlashBadBlocksCount>>8)&0xFF;
  FlashWriteByte((FlashBadBlocksCount)&0xFF);
  Sum+=(FlashBadBlocksCount)&0xFF;
  j=8; //д��8�ֽ�
  //���滵���
  for(k=0;k<sizeof(FlashBadBlockTable[0][0])*FLASH_BAD_BLOCKS_REMAP*2;k++)
  {
   if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ����д��ҳ
   {
    FlashWritePage(); //дҳ
    FlashWriteCommand(0x80);
    FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i + j);
    FlashSetPortAsOut();  //��������Ϊ�����
   }
   Sum+=((uint8 *)FlashBadBlockTable)[k]; //��У���
   FlashWriteByte(((uint8 *)FlashBadBlockTable)[k]);  //дһ�ֽ�
   j++;
  }
  //������Ӱ������״̬��
  for(k=0;k<sizeof(FlashRemapBlockStatus[0])*FLASH_BAD_BLOCKS_REMAP;k++)
  {
   if(0==(j&(FLASH_PAGE_SIZE-1))) //���������ҳ������Ҫ����д��ҳ
   {
    FlashWritePage(); //дҳ
    FlashWriteCommand(0x80);
    FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*i + j);
    FlashSetPortAsOut();  //��������Ϊ�����
   }
   Sum+=((uint8 *)FlashRemapBlockStatus)[k]; //��У���
   FlashWriteByte(((uint8 *)FlashRemapBlockStatus)[k]);  //дһ�ֽ�
   j++;
  }
  for(;0!=(j&(FLASH_PAGE_SIZE-1));j++) //��ʣ�ಿ��д��0xFF
  {
   FlashWriteByte(0xFF);
  }
  FlashWritePage();   //дҳ
  
  //�����д״̬��У���д�뵽�ÿ�ĵ����ڶ�ҳ
  FlashWriteCommand(0x80);
  FlashWriteAddr4Byte(FLASH_BLOCK_TABLE_ADDR + FLASH_BLOCK_SIZE*(i+1) - 2*FLASH_PAGE_SIZE);
  FlashSetPortAsOut();  //��������Ϊ�����
  FlashWriteByte(0x00);  //����һ�ֽ�����Ϊ0����ʾ�Ѿ�д��
  //��У���ȡ����1�������ۼӽ����Ϊ0
  Sum=(~Sum)+1;
  //дУ���
  FlashWriteByte((Sum>>24)&0xFF);
  FlashWriteByte((Sum>>16)&0xFF);
  FlashWriteByte((Sum>>8)&0xFF);
  FlashWriteByte((Sum)&0xFF);
  //ʣ���ֽ�д0xFF
  for(j=5;j<FLASH_PAGE_SIZE;j++)
  {
   FlashWriteByte(0xFF);
  }
  FlashWritePage(); //дҳ
 }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ����»����
��ڲ�����OldAddr���ɵ�ַ��NewAddr��Ӱ��֮��ĵ�ַ��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashUpdateBadBlockTable(uint32 OldAddr,uint32 NewAddr)
{
 uint32 i,j;
 OldAddr&=~(FLASH_BLOCK_SIZE-1); //���һ������ʼ��ַ
 NewAddr&=~(FLASH_BLOCK_SIZE-1);
 if(OldAddr>FLASH_MAX_SECTOR_ADDR) //������ܹ��ﵽ������ַ����˵�������ַ�������Ǳ�����Ӱ�����
 {
  //��Ҫ�ҵ���ԭ��Ӱ���λ��
  for(i=0;i<FlashBadBlocksCount;i++)
  {
   if(OldAddr==FlashBadBlockTable[1][i]) //�����ĳ����ַ�Ǻϣ���˵�����Ǹõ�ַ��
   {
    FlashBadBlockTable[1][i]=NewAddr; //����Ӱ�䵽�µĵ�ַ
    //����ԭ���Ľ���������Ϊ����
    FlashMarkRemapBlockBad(OldAddr);
    break;
   }
  }
 }
 else //˵�������ַ��û�б�Ӱ�����
 {
  //���ұ�����Ŀ��ַ���������뵽ǰ�棬�ź��򣬷�����ֲ��
  for(i=0;i<FlashBadBlocksCount;i++) 
  {
   if(OldAddr<FlashBadBlockTable[0][i]) //�ҵ�������ĵ�ַ
   {
    break;
   }
  }
  for(j=FlashBadBlocksCount;j>i;j--) //������Ĳ��������ƶ����ڳ�һ����λ
  {
   FlashBadBlockTable[0][j]=FlashBadBlockTable[0][j-1];
   FlashBadBlockTable[1][j]=FlashBadBlockTable[1][j-1];
  }
  //����ǰ���Ӱ��д��
  FlashBadBlockTable[0][j]=OldAddr;
  FlashBadBlockTable[1][j]=NewAddr;
  FlashBadBlocksCount++; //����һ���������
 }
 FlashSaveBadBlockTable(); //�洢�����
 //�޸ĵ�ǰ���ʹ��ĵ�ַΪ��Ч��ַ�������´β���ʱ�ͻ�����Ӱ��
 FlashLastAccessAddr=-1;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�FLASH���鴦��
��ڲ�����Addr: �ֽڵ�ַ��
��    �أ��ޡ�
��    ע�������ĵ�ַ��
********************************************************************/
uint32 FlashDealBadBlock(uint32 Addr, uint32 Type)
{
 uint32 i;
 uint32 RemapBlockAddr;
 uint32 SwapBlockAddr;
 while(1)
 {
  RemapBlockAddr=FlashGetNewRemapBlock();
  if(RemapBlockAddr==-1)  //����Ѿ��Ҳ����µĿ��õ��滻Ʒ����ֻ��ֱ�ӷ����ˡ�
  {
   return Addr;
  }
  switch(Type)
  {
   //����ʱ�����Ļ��飬����Ҫ����ǰҳ����������д��
   //ֻ��Ҫ�����µĵ�ַ���ɡ���ַͳһ����󷵻أ����ﲻ�ô���
   case 1:
    goto Exit;
   break;
   
   //����ҳʱ�����Ļ��飬��Ҫ���ÿ���ǰ���ҳ����ǰҳ�ӽ����������¸���
   case 2:
   //�ӽ�����ȥ����ǰ��ҳ�Լ���ǰҳ������
   SwapBlockAddr=FlashGetCurrentSwapBlock();  //��ȡ��ǰ��ʹ�õĽ�����
   //����ǰ���Լ���ǰҳ
   for(i=0;i<(Addr&(FLASH_BLOCK_SIZE-1))/FLASH_PAGE_SIZE+1;i++)
   {
    if(0x01==(FlashCopyPage(SwapBlockAddr+i*FLASH_PAGE_SIZE,RemapBlockAddr+i*FLASH_PAGE_SIZE)&0x01))
    {
     //�������ʧ�ܣ���˵���ÿ������⣬��Ҫ���µĿ�
     goto BadRemapBlock;
    }
   }
   //������ϣ����˳�ѭ��
   goto Exit;
   break;
   
   //д����ʱ�����Ļ��飬��Ҫ���ÿ���ǰ���ҳ�ӽ����������¸��ƣ�
   //����Ҫ����ǰҳ�ӽ������и��Ʋ���������������д�뵽��ǰҳ��
   //�����޷��ٻ�ȡ���������������ˣ�ֻ��ֱ�Ӵ�ԭ����ҳ��������
   case 3:  
   //�ӽ�����ȥ����ǰ��ҳ����
   SwapBlockAddr=FlashGetCurrentSwapBlock();  //��ȡ��ǰ��ʹ�õĽ�����
   //����ǰ���ҳ
   for(i=0;i<(Addr&(FLASH_BLOCK_SIZE-1))/FLASH_PAGE_SIZE;i++)
   {
    if(0x01==(FlashCopyPage(SwapBlockAddr+i*FLASH_PAGE_SIZE,RemapBlockAddr+i*FLASH_PAGE_SIZE)&0x01))
    {
     //�������ʧ�ܣ���˵���ÿ������⣬��Ҫ���µĿ�
     goto BadRemapBlock;
    }
   }
   //���ڵ�ǰҳ��ֻ�ôӸո�д��Ĵ����ַȥ����
   if(0x01==(FlashCopyPage(Addr,RemapBlockAddr+i*FLASH_PAGE_SIZE)&0x01))
   {
    //�������ʧ�ܣ���˵���ÿ������⣬��Ҫ���µĿ�
    goto BadRemapBlock;
   }   
   //������ϣ����˳�ѭ��
   goto Exit;
   break;
   
   default:
   break;
  }
  BadRemapBlock:
  //�������������ʧ�ܣ���Ҫ��־�ÿ��Ѿ���
  FlashMarkRemapBlockBad(RemapBlockAddr);
 }
 Exit:
 //���»����
 FlashUpdateBadBlockTable(Addr,RemapBlockAddr);
 return RemapBlockAddr+(Addr&(FLASH_BLOCK_SIZE-1));
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�������õĽ������ַ��
��ڲ�����Op����Ӧ�Ĳ�����
��    �أ���һ�����õĽ�����ĵ�ַ��
��    ע���ޡ�
********************************************************************/
uint32 FlashManageSwapBlock(uint32 Op)
{
 static uint32 Current;
 static uint8 FlashSwapBlockStatus[FLASH_SWAP_BLOCKS];
 uint32 i;
 
 switch(Op)
 {
  case 0:  //�������Ϊ1����ʾ��ʼ��
   Current=0;
   for(i=0;i<FLASH_SWAP_BLOCKS;i++)
   {
    FlashSwapBlockStatus[i]=0; //��ʼ�����н�����Ϊ�õ�
   }
  break;
  
  case 1: //�������Ϊ1����ʾ��ȡ��һ�����õĽ�����
   while(1)//һֱ���ԣ���������������꣨�����ˣ���ô����ѭ���ˣ�
   {
    Current++;
    if(Current>=FLASH_SWAP_BLOCKS)
    {
     Current=0;
    }
    if(FlashSwapBlockStatus[Current]==0)break; //����ÿ��־Ϊ0����˵��δ��
   }
  break;
  
  case 2: //�������Ϊ2��˵����ȡ��ǰ��������ַ
  break;
  
  case 3: //�������Ϊ3�����õ�ǰ������Ϊ����
   FlashSwapBlockStatus[Current]=FLASH_BLOCK_BAD;
  break;
  
  default:
  break;
 }
 return FLASH_SWAP_BLOCK_ADDR+Current*FLASH_BLOCK_SIZE; //���ؿ��õĽ������ַ
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���һ�����ݸ��Ƶ���������ͬʱ��ԭ���Ŀ�ɾ����
          �����ÿ���Addr����ҳǰ���ҳ�渴�ƻ�ԭ���Ŀ顣
��ڲ�����Addr��Ҫ���Ƴ����Ŀ��ַ��
��    �أ�ԭ����ĵ�ַ��
��    ע������ڸ��ƻ�ȥ�Ĺ����У����ִ���
          ��ô˵��ԭ���Ŀ��Ѿ��𻵣���Ҫ����Ӱ�䵽һ���õĿ顣
          ��ʱ���صĵ�ַ��������Ӱ�����ĵ�ַ��
********************************************************************/
uint32 FlashCopyBlockToSwap(uint32 Addr)
{
 uint32 SwapAddr;
 uint32 i;
 uint32 BlockStartAddr;
 
 BlockStartAddr=(Addr)&(~(FLASH_BLOCK_SIZE-1));  //�������ʼ��ַ
 
 while(1)
 {
  SwapAddr=FlashGetNextSwapBlock(); //��ȡ��һ��������
  if(0x00==(FlashEraseBlock(SwapAddr)&0x01)) //��������ɹ�
  {
   for(i=0;i<FLASH_BLOCK_SIZE/FLASH_PAGE_SIZE;i++)  //����Ӧ��������ҳ���Ƶ���������
   {
    //����һҳ
    if(0x01&FlashCopyPage(BlockStartAddr+i*FLASH_PAGE_SIZE,SwapAddr+i*FLASH_PAGE_SIZE))
    {
     //�������ʧ�ܣ���˵���ý������Ѿ��𻵣�������һ�����õĽ�����
     goto BadSwapBlock;
    }
   }
   //ȫ��������ϣ��������ԭ���Ŀ�
   if(0x01==(FlashEraseBlock(BlockStartAddr)&0x01)) //�������ʧ��
   {
    Addr=FlashDealBadBlock(Addr,1); //�������ʱ�����Ļ���
    BlockStartAddr=(Addr)&(~(FLASH_BLOCK_SIZE-1));  //�������ʼ��ַ
   }
   //��ǰ�沿�ֲ���д����ҳ���ƻ�ȥ
   for(i=0;i<(Addr-BlockStartAddr)/FLASH_PAGE_SIZE;i++)
   {
    //����һҳ
    if(0x01&FlashCopyPage(SwapAddr+i*FLASH_PAGE_SIZE,BlockStartAddr+i*FLASH_PAGE_SIZE))
    {
     //�������ʧ�ܣ�����û���
     //ע��FlashDealBadBlock���ص��ǵ�ǰ���ڲ�����������ַ��
     //��Ҫȡ������ַ����Addrԭ����������ַ�ϳ��µ�������ַ
     Addr=(FlashDealBadBlock(BlockStartAddr+i*FLASH_PAGE_SIZE,2)&(~(FLASH_BLOCK_SIZE-1)))
         +(Addr&(FLASH_BLOCK_SIZE-1));
     BlockStartAddr=(Addr)&(~(FLASH_BLOCK_SIZE-1));  //�������ʼ��ַ
    }
   }
   return Addr; //������ϣ�����
  }
  else //���򣬲���ʧ��
  {
   BadSwapBlock:
   //��־�ÿ����ʱ����
   FlashMarkBadCurrentSwapBlock();
  }
 }
 return Addr;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASH��дһ��������FLASH_SECTOR_SIZE�ֽڣ���
��ڲ�����Addr: �ֽڵ�ַ��pBuf���������ݵĻ�������Remain��Ԥ�ƽ���������Ҫд��������
��    �أ�д���״̬��0���ɹ�����0��ʧ�ܡ�
��    ע����Remain��Ϊ0ʱ����ǰҳ�Լ��ÿ���ʣ�ಿ�ֽ������д��
          ������ݴ��������Ӧ�ý�Remain��0��������д�ء�
********************************************************************/
uint32 FlashWriteOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain)
{
 uint32 i;
 uint32 SwapPageAddr;
 
 FlashClrCe(); //ѡ��оƬ
 if(Addr>FLASH_MAX_SECTOR_ADDR)return 1; //�����ַ������Χ���򷵻�ʧ�ܴ���1��Խ��
 Addr=FlashAddrRemap(Addr); //����Ӱ���ַ
 if((Addr&(~(FLASH_PAGE_SIZE-1)))!=(FlashCurrentWriteSectorAddr&(~(FLASH_PAGE_SIZE-1)))) //�����page
 {
  if(FlashNeedWriteBack) //���ǰ��д�����ݣ�����Ҫ����ǰ������pageд��
  {
   if(FlashWritePage()&0x01) //д��ʧ��
   {
    Addr=FlashDealBadBlock(Addr-FLASH_PAGE_SIZE,3)+FLASH_PAGE_SIZE;  //���鴦��
   }
  }
  if((Addr&(~(FLASH_BLOCK_SIZE-1)))!=(FlashCurrentWriteSectorAddr&(~(FLASH_BLOCK_SIZE-1))))  //�����block������Ҫ�����µĿ飬
  {
   //�ڲ���֮ǰ��Ҫ�Ƚ�ԭ���Ŀ鸴�Ƶ������������ҽ��ÿ�ǰ�沿������д��
   //�ú������˽��������ݸ��Ƶ����������⣬���һ���������ԭ���Ŀ飬Ȼ��ǰ�沿�ָ��ƻ�ԭ���Ŀ�
   Addr=FlashCopyBlockToSwap(Addr);
  }
  //�ӽ������ж�����Ӧ��һҳ
  FlashWriteCommand(0x00);
  FlashWriteAddr4Byte(FlashGetCurrentSwapBlock()+(Addr&(FLASH_BLOCK_SIZE-1)));
  FlashWriteCommand(0x35);
  FlashWait();
  //���д
  FlashWriteCommand(0x85);
  FlashWriteAddr4Byte(Addr); //д4�ֽڵ�ַ
  FlashSetPortAsOut();  //��������Ϊ�����
  for(i=0;i<FLASH_SECTOR_SIZE;i++)
  {
   FlashWriteByte(pBuf[i]);
  }
  FlashSetPortAsIn(); //��������Ϊ�����
  FlashNeedWriteBack=1; //��Ҫд��
 }
 else  //û�г���һҳ��ַ����ֱ��д����
 {
  //���д
  FlashWriteCommand(0x85);
  FlashWriteAddr2Byte(Addr);
  FlashSetPortAsOut();  //��������Ϊ�����
  for(i=0;i<FLASH_SECTOR_SIZE;i++)
  {
   FlashWriteByte(pBuf[i]);
  }
  FlashSetPortAsIn(); //��������Ϊ�����
  FlashNeedWriteBack=1; //��Ҫд��
 }
 FlashCurrentWriteSectorAddr=Addr; //���汾�ε�ַ 
 if(Remain==0) //ʣ��������Ϊ0��������д�ˣ���Ҫд��
 {
  if(FlashNeedWriteBack) //���ǰ��д�����ݣ�����Ҫ����ǰ������pageд��
  {
   if(FlashWritePage()&0x01) //д��ʧ��
   {
    Addr=FlashDealBadBlock(Addr,3);  //���鴦��
   }
  }
  //����ʣ��ҳ��
  Remain=(((Addr+FLASH_BLOCK_SIZE)&(~(FLASH_BLOCK_SIZE-1)))-(Addr&(~(FLASH_PAGE_SIZE-1))))/FLASH_PAGE_SIZE-1;
  //�����ڽ������е���ʼҳ��ַ
  SwapPageAddr=FlashGetCurrentSwapBlock()+(Addr&(FLASH_BLOCK_SIZE-1));
  
  for(i=0;i<Remain;i++)  //���ÿ��ڱ����ڽ�������ʣ�ಿ��ҳ�����ݸ��ƻظÿ�
  {
   Addr+=FLASH_PAGE_SIZE;   //����һҳ��ʼд
   SwapPageAddr+=FLASH_PAGE_SIZE;   
   if(0x01==(FlashCopyPage(SwapPageAddr,Addr)&0x01)) //�������ʧ��
   {
    Addr=FlashDealBadBlock(Addr,2);  //������
   }
  }
  FlashNeedWriteBack=0; //�����Ҫд�ر�־
  FlashCurrentWriteSectorAddr=-1;
 }
 FlashSetCe(); //�ͷ�FLASHоƬ
 return 0;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASH�ж���һ�������ݱ��浽�������С�
��ڲ�����Addr: �ֽڵ�ַ��pBuf���������ݵĻ�������Remain��Ԥ�ƽ���������Ҫ����������
��    �أ���ȡ��״̬��0���ɹ�����0��ʧ�ܡ�
��    ע����Remain��Ϊ0ʱ�������浱ǰ��ַ�Ա�����ļ�������ǰҳ������Ϊ0ʱ��
          ���õ�ǰ����ַΪ��Ч���Ӷ��´ζ�ʱ��������ʹ�ö�������ݴ�flash�ж��뵽ҳ���档
********************************************************************/
uint32 FlashReadOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain)
{
 uint32 i;
 FlashClrCe(); //ѡ��оƬ
 if(Addr>FLASH_MAX_SECTOR_ADDR)return 1; //�����ַ������Χ���򷵻�ʧ�ܴ���1��Խ��
 Addr=FlashAddrRemap(Addr); //����Ӱ���ַ
 if((Addr&(~(FLASH_PAGE_SIZE-1)))
    !=(FlashCurrentReadSectorAddr&(~(FLASH_PAGE_SIZE-1)))) //�����page
 {
  //�����ҳ�ģ���д����������
  FlashWriteCommand(0x00);
  FlashWriteAddr4Byte(Addr);
  FlashWriteCommand(0x30);
  FlashWait(); //�ȴ����ݶ���
 }
 else
 {
  //���û�п�ҳ�������ֱ�Ӷ�
  FlashWriteCommand(0x05);
  FlashWriteAddr2Byte(Addr);
  FlashWriteCommand(0xE0);
  FlashWait(); //�ȴ����ݶ���
 }
 for(i=0;i<FLASH_SECTOR_SIZE;i++)
 {
  FlashReadByte(pBuf[i]);  //��һ�ֽ�����
 }
 FlashCurrentReadSectorAddr=Addr; //���浱ǰ�����ĵ�ַ
 if(Remain==0) //���������Ŷ�����ô�����õ�ǰ�����ĵ�ַΪ��Чֵ
 {
  FlashCurrentReadSectorAddr=-1;
 }
 FlashSetCe(); //�ͷ�����
 return 0;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���FLASH��ID�š�
��ڲ�����Buf������ID�ŵĻ�������
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void FlashReadId(uint8 *Buf)
{
 uint8 i;
 FlashClrCe();
 FlashWriteCommand(0x90);
 FlashSetAle();  //ALE�øߣ�ѡ��Ϊ��ַ
 FlashSetPortAsOut(); //���ö˿�Ϊ���״̬
 FlashWriteByte(0); //дһ�ֽ�����
 FlashSetPortAsIn(); //���ö˿�Ϊ����
 FlashClrAle();      //ALE�õ�
 for(i=0;i<5;i++)  //��5�ֽڵ�ID
 {
  FlashReadByte(Buf[i]);
 }
 FlashSetCe();
}
/////////////////////////End of function/////////////////////////////
