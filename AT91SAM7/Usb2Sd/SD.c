/******************************************************************
  ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
  
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
		��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
		                     http://computer00.21ic.org
							 
 SD.c  file
	
���ߣ�Computer-lov
��������: 2009-04-07
�޸�����: 2009-04-08
�汾��V1.0
��Ȩ���У�����ؾ���
Copyright(C) Computer-lov 2009-2019
All rights reserved
*******************************************************************/

#include "SD.h"
#include <AT91SAM7S64.H>

uint8 SdCid[16];  //����16�ֽڵ�CID
uint8 SdCsd[16];  //����16�ֽڵ�CSD
uint32 SdCapacity; //����������ע�ⵥλΪ������
uint32 SdNAC;  //����NAC����Ŀ
uint8 SdSectorBuf[SECTOR_SIZE]; //��д�����õĻ�����
uint8 SdMulBlockReadCmdSent; //�Ƿ��͹���������ı�־
uint8 SdMulBlockWriteCmdSent; //�Ƿ��͹����д����ı�־

/********************************************************************
�������ܣ�SPI��ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע��������Ӧ��IOģʽ������SPIģʽ��
********************************************************************/
void SpiInit(void)
{
 //PA11����Ϊ��ͨIO�ڣ�������Ϊ����ڣ������ߵ�ƽ
 *AT91C_PIOA_PER=0x01<<11;
 //��ֹ�������������������
 *AT91C_PIOA_ODR=0x01<<11;
 //PA12,PA13,PA14 ��Ϊ����ʹ��
 *AT91C_PIOA_PDR=(0x0E<<11);
 //��ֹPA11,PA12,PA13,PA14 IO�ж�
 *AT91C_PIOA_IDR=(0x0F<<11);
 //ʹ��PA11,PA12,PA13,PA14��������
 *AT91C_PIOA_PPUER=(0x0F<<11);
 //���ӵ�A����(��SPI��)
 *AT91C_PIOA_ASR=(0x0F<<11);
 *AT91C_SPI_CR=(1<<0);   //SPI Enable
 *AT91C_SPI_MR= (1<<0)   //SPI is in Master mode,
               |(0<<1)   //Fixed Peripheral Select,
               |(0<<2)   //The chip selects are directly
                         //connected to a peripheral device.
               |(1<<4)   //Mode fault detection is disabled.
               |(0<<7)   //Local loopback path disabled.
               |(0<<16)  //Peripheral Chip Select = 0
               |(6<<24);  //Delay Between Chip Selects = 6
 *AT91C_SPI_IDR=0x3F;  //��ֹ����SPI�ж�
 AT91C_SPI_CSR[0]= (1<<0) //CPOL=1,
                  |(0<<1) //NCPHA=0
                  |(1<<3) //The Peripheral Chip Select does
                          //not rise after the last transfer is achieved.
                  |(0<<4) //8bits Per Transfer
                  |(2<<8) //SCBR: Serial Clock Baud Rate = 2.
                          //SPI Clock is 48MHz/2=24MHz
                  |(0<<16) //DLYBS: Delay Before SPCK = 0
                  |(0<<24);//DLYBCT: Delay Between Consecutive Transfers = 0
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�ͨ��SPI����һ�ֽ����ݣ������ض��������ݡ�
��ڲ�����Value: Ҫ��������ݡ�
��    �أ�������һ�ֽ����ݡ�
��    ע���ޡ�
********************************************************************/
uint8 SpiWriteReadByte(uint8 Value)
{
 *AT91C_SPI_TDR=Value;  //��������
 while(!((*AT91C_SPI_SR)&0x01));  //�ȴ����ݽ������
 return (uint8)(*AT91C_SPI_RDR);
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���ȡ��Ӧ״̬��
��ڲ������ޡ�
��    �أ�״̬��
��    ע���ޡ�
********************************************************************/
uint8 SdReadResponse(void)
{
 uint32 i;
 uint8 Res;
 for(i=0;i<9;i++)  //���ȴ�8������
 {
  Res=SpiWriteReadByte(0xFF);
  if(!(Res&0x80))return Res;  //������λΪ0���򷵻�
 }
 return 0xFF;  //�������Ӧ��ʱ���򷵻�0xFF
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ��ȴ����ݶ�����
��ڲ������ޡ�
��    �أ�״̬��0xFF����ʱ����0xFE����ɡ�
��    ע���ޡ�
********************************************************************/
uint8 SdWaitRead(void)
{
 uint32 i;
 uint8 Res;
 for(i=0;i<SdNAC;i++)  //�ȴ����ݶ���
 {
  Res=SpiWriteReadByte(0xFF);
  if(Res==0xFE)return Res;  //���Ϊ0xFE���򷵻�
 }
 return 0xFF;  //�������Ӧ��ʱ���򷵻�0xFF
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�SD�����ֳ�ʼ����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע�� ��ʼ��SPI��дCMD0���뵽SPIģʽ��
********************************************************************/
void SdInit(void)
{
 uint32 i;
 uint8 Res;
 
 SpiInit(); //оƬ��SPI��������ʼ��
 
 for(i=0;i<16;i++) //��ʼ��CID��CSD
 {
  SdCid[i]=0;
  SdCsd[i]=0;
 }
 
 //��ʼ����û�з��͹�����д����
 SdMulBlockReadCmdSent=0;
 SdMulBlockWriteCmdSent=0;
 
 //��ʼ������Ϊ0
 SdCapacity=0;
 
 for(i=0;i<100/8;i++) //��CS0Ϊ1״̬�·��ʹ���74��ȫ1ʱ��
 {
  SpiWriteReadByte(0xFF);
 }
 *AT91C_PIOA_PDR=(0x01<<11);    //PA11�л���SPI״̬
 
 //����CMD0�����ʱCSΪ0����������SPIģʽ
 SpiWriteReadByte(0x40|0);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x95);
 Res=SdReadResponse();
 SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ�� 
 if(Res!=0x01) return;  //��ʱӦ�÷���0x01��������ǣ��򷵻�
 //���Լ��
 while(1)
 {
  //����CMD55����л����û�����
  SpiWriteReadByte(0x40|55);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0xFF);
  Res=SdReadResponse();
  SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��  

  //����ACMD41������
  SpiWriteReadByte(0x40|41);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0xFF);
  Res=SdReadResponse();
  SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��
  if(Res==0x00) break;  //���Ϊ0�����ʾ���Ѽ���
  if(Res!=0x01) return; //����״̬�����ִ��󣬷���
 }
 
 SdNAC=9; //��ʼ������ΪNCR��ֵ
 //������CSD
 //����CMD9���׼����ȡCSD
 SpiWriteReadByte(0x40|9);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0xFF);
 Res=SdReadResponse();
 if(Res!=0x00) return;  //��ʱӦ�÷���0x00��������ǣ��򷵻�
 if(SdWaitRead()!=0xFE)return; //�����ʱ���򷵻�
 for(i=0;i<16;i++) //��ȡ16�ֽڵ�CSD����
 {
  SdCsd[i]=SpiWriteReadByte(0xFF);
 }
 for(i=0;i<3;i++) //��ȡ2�ֽ�CRC������һ�ֽ����ʱ��
 {
  SpiWriteReadByte(0xFF);
 }
 
 //������������������
 SdCapacity=1;
 for(i=0;i<((SdCsd[5]&0x0F)-9);i++) //ÿ�������Ĵ�С(READ_BL_LEN)
 {
  SdCapacity*=2;
 }
 for(i=0;i<(((SdCsd[9]&0x03)<<1)+((SdCsd[10]>>7)&0x01)+2);i++) //C_SIZE_MULT
 {
  SdCapacity*=2;
 }
 SdCapacity*=(((uint32)(SdCsd[6]&0x03))<<10)
            +(((uint32)SdCsd[7])<<2)
            +(((SdCsd[8]>>6)&0x03))
            +1;
            
 //������ʱTAAC
 SdNAC=1;
 for(i=0;i<(SdCsd[1]&0x07);i++)
 {
  SdNAC*=10;
 }
 switch((SdCsd[1]>>3)&0x0F)
 {
  case 0:
  case 1:
  break;
  case 2:
   SdNAC*=1.2;
  break;
  case 3:
   SdNAC*=1.3;
  break;
  default:
   SdNAC*=(((SdCsd[1]>>3)&0x0F)-1)*0.5;
  break;
 }
 //SdNAC��λΪns����Ҫ���任��Ϊ�ֽ���
 SdNAC/=1000; //����Ϊus
 SdNAC*=24/8;   //ʱ��Ϊ24MHz��ÿ���ֽ�8��ʱ��
 //��Ҫ����NSAC
 SdNAC+=100*SdCsd[2]/8;

 //������CID
 //����CMD10���׼����ȡCID
 SpiWriteReadByte(0x40|10);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0x00);
 SpiWriteReadByte(0xFF);
 Res=SdReadResponse();
 if(Res!=0x00) return;  //��ʱӦ�÷���0x00��������ǣ��򷵻�
 if(SdWaitRead()!=0xFE)return; //�����ʱ���򷵻�
 for(i=0;i<16;i++) //��ȡ16�ֽڵ�CID����
 {
  SdCid[i]=SpiWriteReadByte(0xFF);
 }
 for(i=0;i<3;i++) //��ȡ2�ֽ�CRC������һ�ֽ����ʱ��
 {
  SpiWriteReadByte(0xFF);
 }
 
 //���ÿ鳤�ȣ�ΪSECTOR_SIZE�ֽ�
 //����CMD16������ÿ鳤��
 SpiWriteReadByte(0x40|16);
 SpiWriteReadByte((SECTOR_SIZE>>24)&0xFF);
 SpiWriteReadByte((SECTOR_SIZE>>16)&0xFF);
 SpiWriteReadByte((SECTOR_SIZE>>8)&0xFF);
 SpiWriteReadByte((SECTOR_SIZE)&0xFF);
 SpiWriteReadByte(0xFF);
 Res=SdReadResponse();
 SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ�� 
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ����������е�һ��������д�뵽SD���С�
��ڲ�����Addr: �ֽڵ�ַ��pBuf���������ݵĻ�������
          Remain��Ԥ�ƽ���������Ҫд��������
��    �أ�д��״̬��0���ɹ�����0��ʧ�ܡ�
��    ע����Remain��Ϊ0ʱ����ֱ��д�����ݡ���RemainΪ0ʱ��
          �����ͽ���д���
********************************************************************/
uint32 SdWriteOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain)
{
 uint32 i;
 uint8 Res;
 
 //���֮ǰû�з��Ͷ��д�������һ��
 if(!SdMulBlockWriteCmdSent)
 {
  SdMulBlockWriteCmdSent=0xFF; //��־��дΪ�ѷ���
  //����CMD25���д�����
  SpiWriteReadByte(0x40|25);
  SpiWriteReadByte((Addr>>24)&0xFF);
  SpiWriteReadByte((Addr>>16)&0xFF);
  SpiWriteReadByte((Addr>>8)&0xFF);
  SpiWriteReadByte((Addr)&0xFF);
  SpiWriteReadByte(0xFF);
  Res=SdReadResponse();
  SpiWriteReadByte(0xFF); //NWR
  //��ʱӦ�÷���0x00��������ǣ��򷵻�ʧ����1
  if(Res!=0x00)
  {
   return 0x01;
  }
 }

 //дStart Block
 SpiWriteReadByte(0xFC);
 //д����
 for(i=0;i<SECTOR_SIZE;i++)
 {
  SpiWriteReadByte(pBuf[i]); //дһ�ֽ�����
 }
 //д2�ֽڵ�CRC16
 SpiWriteReadByte(0xFF);
 SpiWriteReadByte(0xFF);
 Res=SpiWriteReadByte(0xFF); //������Ӧ
 SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��
 while(SpiWriteReadByte(0xFF)!=0xFF); //����æʱ���ȴ�
 if(Remain==0) //��������һ��д������ֹͣдStop Tran
 {
  SdMulBlockWriteCmdSent=0x00; //��־��д����Ϊδ����
  //����Stop Tran��ֹͣд����
  SpiWriteReadByte(0xFD);
  SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��
  while(SpiWriteReadByte(0xFF)!=0xFF); //�ȴ�æ����
 }
 if((Res&0x1F)!=0x05)
  return 0x02; //����ʧ�ܴ���2
 return 0;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���Sd���ж���һ�������ݱ��浽�������С�
��ڲ�����Addr: �ֽڵ�ַ��pBuf���������ݵĻ�������
          Remain��Ԥ�ƽ���������Ҫ����������
��    �أ���ȡ��״̬��0���ɹ�����0��ʧ�ܡ�
��    ע����Remain��Ϊ0ʱ����ֱ�Ӷ�ȡ���ݡ���RemainΪ0ʱ�������ͽ��������
********************************************************************/
uint32 SdReadOneSector(uint32 Addr, uint8 * pBuf, uint32 Remain)
{
 uint32 i;
 uint8 Res;
 //���֮ǰû�з��Ͷ����������һ��
 if(!SdMulBlockReadCmdSent)
 {
  SdMulBlockReadCmdSent=0xFF; //��־�������Ϊ�ѷ���
  //����CMD18����������
  SpiWriteReadByte(0x40|18);
  SpiWriteReadByte((Addr>>24)&0xFF);
  SpiWriteReadByte((Addr>>16)&0xFF);
  SpiWriteReadByte((Addr>>8)&0xFF);
  SpiWriteReadByte((Addr)&0xFF);
  SpiWriteReadByte(0xFF);
  Res=SdReadResponse();
  //��ʱӦ�÷���0x00��������ǣ��򷵻�ʧ����1
  if(Res!=0x00) return 0x01;
 }
 //�ȴ����ݷ��أ������ʱ���򷵻�2
 if(SdWaitRead()!=0xFE)return 0x02;
 for(i=0;i<SECTOR_SIZE;i++)
 {
  pBuf[i]=SpiWriteReadByte(0xFF); //��ȡһ�ֽ�����
 }
 //����2�ֽڵ�CRC16
 SpiWriteReadByte(0xFF);
 SpiWriteReadByte(0xFF);
 if(Remain==0) //��������һ�ζ�������ֹͣ������CMD12
 {
  SdMulBlockReadCmdSent=0x00; //��־�������Ϊδ����
  //����CMD12���ֹͣ������
  SpiWriteReadByte(0x40|12);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0x00);
  SpiWriteReadByte(0xFF);
  SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��
  Res=SdReadResponse();
  SpiWriteReadByte(0xFF); //����һ�ֽ����ʱ��
  while(SpiWriteReadByte(0xFF)!=0xFF); //�ȴ�æ����
  //��ʱӦ�÷���0x00��������ǣ��򷵻�ʧ����1
  if(Res!=0x00) return 0x01;
 }
 return 0;
}
/////////////////////////End of function/////////////////////////////

