/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
   
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

SCSI.c file

���ߣ�����ȦȦ
��������: 2008.08.15
�޸�����: 2009.03.16
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/


#include "MyType.H"
#include "UsbCore.h"
#include "AT91SAMxUSB.h"
#include "SCSI.h"
#include "Uart.h"
#include "config.h"
#include "Led.h"
#include "SD.h"

//����˵�1��������Ϊ64�ֽ�
#define EP1_SIZE 64

//����˵�2��������Ϊ64�ֽ�
#define EP2_SIZE 64

uint32 ByteAddr;  //�ֽڵ�ַ

uint8 * pEp1SendData;
uint8 * pEp2ReceiveBuf;

uint32 Ep1DataLength;
uint32 Ep2DataLength;

//INQUIRY������Ҫ���ص�����
//���������INQUIRY������Ӧ���ݸ�ʽ
const uint8 DiskInf[36]=
{
 0x00, //�����豸
 0x80, //�������λD7ΪRMB��RMB=0����ʾ�����Ƴ��豸�����RMB=1����Ϊ���Ƴ��豸��
 0x00, //���ְ汾��0
 0x01, //������Ӧ��ʽ
 0x1F, //�������ݳ��ȣ�Ϊ31�ֽ�
 0x00, //����
 0x00, //����
 0x00, //����
 0xB5,0xE7,0XC4,0xD4,0xC8,0xA6,0xC8,0xA6, //���̱�ʶ��Ϊ�ַ���������ȦȦ��
 
 //��Ʒ��ʶ��Ϊ�ַ���������USB-SD��������
 0xD7,0xF6,0xB5,0xC4,0x55,0x53,0x42,0x2D,0x53,0x44,0xB6,0xC1,0xBF,0xA8,0xC6,0xF7,
 0x31,0x2E,0x30,0x31 //��Ʒ�汾�ţ�Ϊ1.01
};

//READ_FORMAT_CAPACITIES������Ҫ���ص�����
//���������READ_FORMAT_CAPACITIES������Ӧ���ݸ�ʽ
const uint8 MaximumCapacity[12]=
{
 0x00, 0x00, 0x00, //����
 0x08,  //�����б���
 0x01, 0x00, 0x00, 0x00,  //����(���֧��8GB)
 0x03, //����������Ϊ3����ʾ���֧�ֵĸ�ʽ������
 //ÿ���СΪ512�ֽ�
 (SECTOR_SIZE>>16),(SECTOR_SIZE>>8),SECTOR_SIZE
};

//READ_CAPACITY������Ҫ���ص�����
uint8 DiskCapacity[8]=
{
 //�ܹ����ʵ�����߼����ַ
 0,
 0,
 0,
 0,
 //��Ĵ�С
 (SECTOR_SIZE>>24),
 (SECTOR_SIZE>>16),
 (SECTOR_SIZE>>8),
 SECTOR_SIZE
};

//REQUEST SENSE������Ҫ���ص����ݣ���ʼ��Ϊ��Ч����
//��ο��������ݽṹ�Ľ���
uint8 SenseData[18]=
{
 0x70, //������룬�̶�Ϊ0x70
 0x00, //����
 0x05, //Sense KeyΪ0x05����ʾ��Ч����ILLEGAL REQUEST��
 0x00, 0x00, 0x00, 0x00, //InformationΪ0
 0x0A, //�������ݳ���Ϊ10�ֽ�
 0x00, 0x00, 0x00, 0x00, //����
 0x20, //Additional Sense Code(ASC)Ϊ0x20����ʾ��Ч��������루INVALID COMMAND OPERATION CODE��
 0x00, //Additional Sense Code Qualifier(ASCQ)Ϊ0
 0x00, 0x00, 0x00, 0x00 //����
};

/********************************************************************
�������ܣ�����sense����Ϊ��Ч���
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SetSenseDataInvalidCommand(void)
{
 uint i;
 for(i=0;i<18;i++)
 {
  SenseData[i]=0;
 }
 SenseData[0]=0x70; //������룬�̶�Ϊ0x70
 SenseData[2]=0x05; //Sense KeyΪ0x05����ʾ��Ч����ILLEGAL REQUEST��
 SenseData[7]=0x0A; //�������ݳ���Ϊ10�ֽ�
 SenseData[12]=0x20; //Additional Sense Code(ASC)Ϊ0x20����ʾ��Ч��������루INVALID COMMAND OPERATION CODE��
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ�����sense����Ϊ�޴���
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SetSenseDataNoSense(void)
{
 uint i;
 for(i=0;i<18;i++)
 {
  SenseData[i]=0;
 }
 SenseData[0]=0x70; //������룬�̶�Ϊ0x70
 SenseData[2]=0x00; //Sense KeyΪ0x00����ʾ�޴���(NO SENSE)
 SenseData[7]=0x0A; //�������ݳ���Ϊ10�ֽ�
 SenseData[12]=0x00; //Additional Sense Code(ASC)Ϊ0
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
�������ܣ���CBW�л�ȡ�������ݵ��ֽ�����
��ڲ������ޡ�
��    �أ���Ҫ������ֽ�����
��    ע���ޡ�
********************************************************************/
uint32 GetDataTransferLength(void)
{
 uint32 Len;
 
 //CBW[8]~CBW[11]Ϊ���䳤�ȣ�С�˽ṹ��
 
 Len=CBW[11];
 Len=Len*256+CBW[10];
 Len=Len*256+CBW[9];
 Len=Len*256+CBW[8];
 
 return Len;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���CBW�л�ȡ�߼����ַLBA���ֽ�����
��ڲ������ޡ�
��    �أ��߼����ַLBA��
��    ע���ޡ�
********************************************************************/
uint32 GetLba(void)
{
 uint32 Lba;
 
 //����д����ʱ��CBW[17]~CBW[20]Ϊ�߼����ַ����˽ṹ��
 
 Lba=CBW[17];
 Lba=Lba*256+CBW[18];
 Lba=Lba*256+CBW[19];
 Lba=Lba*256+CBW[20];
 
 return Lba;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����CSW��
��ڲ�����Residue��ʣ���ֽ�����Status������ִ�е�״̬��
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void SetCsw(uint32 Residue, uint8 Status)
{
 //����CSW��ǩ������ʵ���Բ���ÿ�ζ����õģ�
 //��ʼ��ʼ������һ�ξ����ˣ�����ÿ�ζ�����
 CSW[0]='U';
 CSW[1]='S';
 CSW[2]='B';
 CSW[3]='S';
 
 //����dCBWTag��CSW��dCSWTag��ȥ
 CSW[4]=CBW[4];
 CSW[5]=CBW[5];
 CSW[6]=CBW[6];
 CSW[7]=CBW[7];
    
 //ʣ���ֽ���
 CSW[8]=Residue&0xFF;
 CSW[9]=(Residue>>8)&0xFF;
 CSW[10]=(Residue>>16)&0xFF;
 CSW[11]=(Residue>>24)&0xFF;
 
 //����ִ�е�״̬��0��ʾ�ɹ���1��ʾʧ�ܡ�
 CSW[12]=Status;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ���ȡ�������ݺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void GetDiskData(void)
{
 if(0==(ByteAddr&(SECTOR_SIZE-1)))  //�����������������
 {
  //��Sd���ж���һ������
  SdReadOneSector(ByteAddr,SdSectorBuf,Ep1DataLength/SECTOR_SIZE-1);
  pEp1SendData=SdSectorBuf;  //ָ�򻺳����׵�ַ
 }
 ByteAddr+=EP1_SIZE; //�����ֽڵ�ַ��ÿ�η����������ȵ�����
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�������ͨ���˵�1���͡�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע�����������ݳ���Ϊ0�����Ҵ������ݽ׶�ʱ�����Զ�����CSW��
********************************************************************/
void Ep1SendData(void)
{
 if(Ep1DataLength==0) //�����Ҫ���͵����ݳ���Ϊ0
 {
  OffLed3();  //�ر�LED3
  if(TransportStage==DATA_STAGE) //���Ҵ������ݽ׶�
  {
   //��ֱ�ӽ���״̬�׶�
   TransportStage=STATUS_STAGE;
   Ep1DataLength=sizeof(CSW); //���ݳ���ΪCSW�Ĵ�С
   pEp1SendData=CSW; //���ص�����ΪCSW
  }
  else
  {
   return; //�����״̬�׶ε����ݷ�����ϣ��򷵻�
  }
 }
 
#ifdef DEBUG0
 if(TransportStage==STATUS_STAGE)
 {
  Prints("״̬�׶Ρ�\r\n");
 }
#endif

 //���Ҫ���͵ĳ��ȱȶ˵�1������Ҫ�࣬��ֶ��������
 if(Ep1DataLength>EP1_SIZE)
 {
  //���Ͷ˵�1��󳤶��ֽ�
  UsbChipWriteEndpointBuffer(1,EP1_SIZE,pEp1SendData);
  //ָ���ƶ�EP1_SIZE�ֽ�
  pEp1SendData+=EP1_SIZE;
  Ep1DataLength-=EP1_SIZE;
  //�����READ(10)������������ݽ׶Σ�����Ҫ��ȡ��������
  if((CBW[15]==READ_10)&&(TransportStage==DATA_STAGE))
  {
   GetDiskData(); //��ȡ��������
  }
 }
 else
 {
  //����ȫ��������
  UsbChipWriteEndpointBuffer(1,(uint8)Ep1DataLength,pEp1SendData);
  Ep1DataLength=0;  //���䳤��Ϊ0
  //��������ݷ�����ϣ���������������Э���״̬�׶�
  if(TransportStage==DATA_STAGE)
  {
   TransportStage=STATUS_STAGE;
   Ep1DataLength=sizeof(CSW); //���ݳ���ΪCSW�Ĵ�С
   pEp1SendData=CSW; //���ص�����ΪCSW
  }
  else if(TransportStage==STATUS_STAGE) //�����״̬�׶���ϣ�����뵽����׶�
  {
   TransportStage=COMMAND_STAGE;  //���뵽����׶�
  }
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�����SCSI����ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע����Ȼ��SCSI�������ʵ��ʹ�õ���UFI���
********************************************************************/
void ProcScsiCommand(void)
{
 OnLed3();  //���������׶Σ�LED3����
 TransportStage=DATA_STAGE; //���뵽���ݽ׶�
 //CBW��ƫ����Ϊ15���ֶ�Ϊ���������
 switch(CBW[15])
 {
  case INQUIRY:  //INQUIRY����
  #ifdef DEBUG0
   Prints("��ѯ����������ݣ�\r\n");
  #endif
   pEp1SendData=(uint8 *)DiskInf; //���ش�����Ϣ
   Ep1DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
   SetCsw(Ep1DataLength-sizeof(DiskInf),0); //����ʣ���ֽ����Լ�״̬�ɹ�
   if(Ep1DataLength>sizeof(DiskInf)) //�����������ݱ�ʵ�ʵ�Ҫ��
   {
    Ep1DataLength=sizeof(DiskInf); //��ֻ����ʵ�ʵĳ���
   }
   SetSenseDataNoSense();  //����SenseΪ�޴���
   Ep1SendData(); //��������
  break;
  
  case READ_FORMAT_CAPACITIES: //����ʽ������
  #ifdef DEBUG0
   Prints("����ʽ����������������ݣ�\r\n");
  #endif
   pEp1SendData=(uint8 *)MaximumCapacity; //��������ʽ��������Ϣ
   Ep1DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
   SetCsw(Ep1DataLength-sizeof(MaximumCapacity),0); //����ʣ���ֽ����Լ�״̬�ɹ�
   if(Ep1DataLength>sizeof(MaximumCapacity)) //�����������ݱ�ʵ�ʵ�Ҫ��
   {
    Ep1DataLength=sizeof(MaximumCapacity); //��ֻ����ʵ�ʵĳ���
   }
   SetSenseDataNoSense();  //����SenseΪ�޴���
   Ep1SendData(); //��������
  break;
  
  case READ_CAPACITY: //����������
  #ifdef DEBUG0
   Prints("����������������ݣ�\r\n");
  #endif
   //������Ŀ��Ҫ����2��������������������дʱ����
   if(SdCapacity!=0) //���������Ϊ0
   {
    DiskCapacity[0]=(SdCapacity-2)>>24;
    DiskCapacity[1]=(SdCapacity-2)>>16;
    DiskCapacity[2]=(SdCapacity-2)>>8;
    DiskCapacity[3]=(SdCapacity-2);
    //����������С
    DiskCapacity[4]=(SECTOR_SIZE>>24);
    DiskCapacity[5]=(SECTOR_SIZE>>16);
    DiskCapacity[6]=(SECTOR_SIZE>>8);
    DiskCapacity[7]=SECTOR_SIZE;
   }
   else //�������Ϊ0
   {
    uint32 i;
    for(i=0;i<8;i++)
    {
     DiskCapacity[i]=0;
    }
   }
   pEp1SendData=(uint8 *)DiskCapacity; //���ش�������
   Ep1DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
   SetCsw(Ep1DataLength-sizeof(DiskCapacity),0); //����ʣ���ֽ����Լ�״̬�ɹ�
   if(Ep1DataLength>sizeof(DiskCapacity)) //�����������ݱ�ʵ�ʵ�Ҫ��
   {
    Ep1DataLength=sizeof(DiskCapacity); //��ֻ����ʵ�ʵĳ���
   }
   SetSenseDataNoSense();  //����SenseΪ�޴���
   Ep1SendData(); //��������
  break;
  
  case READ_10: //READ(10)����
  #ifdef DEBUG0
   Prints("READ(10)����������ݣ�\r\n");
  #endif
   Ep1DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
   ByteAddr=GetLba()*SECTOR_SIZE; //��ȡ�ֽڵ�ַ���ֽڵ�ַΪ�߼����ַ����ÿ���С
   SetCsw(0,0); //����ʣ���ֽ���Ϊ0��״̬�ɹ�
   SetSenseDataNoSense();  //����SenseΪ�޴���
   GetDiskData(); //��ȡ��Ҫ���ص�����
   Ep1SendData(); //��������
  break;
  
  case WRITE_10: //WRITE(10)����
  #ifdef DEBUG0
   Prints("WRITE(10)���������ݣ�\r\n");
  #endif

   Ep2DataLength=GetDataTransferLength(); //��ȡ��Ҫд���ݵĳ���
   pEp2ReceiveBuf=SdSectorBuf; //׼����������
   ByteAddr=GetLba()*SECTOR_SIZE; //��ȡ�ֽڵ�ַ���ֽڵ�ַΪ�߼����ַ����ÿ���С
   SetSenseDataNoSense();  //����SenseΪ�޴���
   SetCsw(0,0); //����ʣ���ֽ���Ϊ0��״̬�ɹ�
  break;
  
  case PREVENT_ALLOW_MEDIUM_REMOVAL:  //�Ƿ���Ƴ�
  #ifdef DEBUG0
   Prints("PREVENT_ALLOW_MEDIUM_REMOVAL�������SENSE���ݣ�\r\n");
  #endif
   //CBW[19]�����λ��ʾ�Ƿ�����Ƴ����̣�0��ʾ���ԣ�1��ʾ�����ԡ�
   //������������LED��ʾ��������Ϊ1ʱ��LED4����������Ϊ0ʱ��LED4��
   if(CBW[19]&0x01) 
   {
    OnLed4();
   }
   else
   {
    OffLed4();
   }
   Ep1DataLength=0; //���ó���Ϊ0���������ݽ�����CSW
   SetSenseDataNoSense();  //����SenseΪ�޴���
   SetCsw(0,0); //����CSWΪ�ɹ�
   Ep1SendData(); //����CSW
  break;
  
  case VERIFY:  //У�飬������Ϊ�������ǶԵ�
  #ifdef DEBUG0
   Prints("VERIFY�������SENSE���ݣ�\r\n");
  #endif
   Ep1DataLength=0; //���ó���Ϊ0���������ݽ�����CSW
   SetSenseDataNoSense();  //����SenseΪ�޴���
   SetCsw(0,0); //����CSWΪ�ɹ�
   Ep1SendData(); //����CSW
  break;
  
  case REQUEST_SENSE: //������ѯ��ǰһ������ִ��ʧ�ܵ�ԭ��
  #ifdef DEBUG0
   Prints("REQUEST SENSE�������SENSE���ݣ�\r\n");
  #endif
   pEp1SendData=(uint8 *)SenseData; //����̽������
   Ep1DataLength=GetDataTransferLength(); //��ȡ��Ҫ���صĳ���
   SetCsw(Ep1DataLength-sizeof(SenseData),0); //����ʣ���ֽ����Լ�״̬�ɹ�
   if(Ep1DataLength>sizeof(SenseData)) //�����������ݱ�ʵ�ʵ�Ҫ��
   {
    Ep1DataLength=sizeof(SenseData); //��ֻ����ʵ�ʵĳ���
   }
   Ep1SendData(); //��������
  break;
  
  case TEST_UNIT_READY: //���Դ����Ƿ�׼����
   Ep1DataLength=0; //���ó���Ϊ0���������ݽ�����CSW
   SetSenseDataNoSense();  //����SenseΪ�޴���
   SetCsw(0,0); //����CSWΪ�ɹ�
   Ep1SendData(); //����CSW
  break;
  
  default: //��������ϣ�����ִ��ʧ��
   if(CBW[12]&0x80) Ep1DataLength=1; //���Ϊ������������㷵��1�ֽ�
   else Ep1DataLength=0; //����Ϊ������������ó���Ϊ0��ֱ�ӷ���CSW
   SetCsw(GetDataTransferLength()-Ep1DataLength,1); //����CSWΪʧ��
   SetSenseDataInvalidCommand();  //����Ϊ��Ч����
   Ep1SendData(); //����CSW
  break;
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�����������ݡ�
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void ProcScsiOutData(void)
{
 uint8 Len;
 //���˵�2����
 Len=UsbChipReadEndpointBuffer(2,EP2_SIZE,pEp2ReceiveBuf);
 //����˵㻺����
 UsbChipClearBuffer(2); 
 Ep2DataLength-=Len; //����ʣ�೤��
 pEp2ReceiveBuf+=Len; //�������ջ�����
 ByteAddr+=Len; //������ַ
 if(0==(ByteAddr&(SECTOR_SIZE-1))) //�����ַ������������������Ҫд���ݵ�Sd��
 {
  //дһ���������ݡ�ע���ַҪ�����������Ŀ�ʼ��
  SdWriteOneSector(ByteAddr-SECTOR_SIZE,SdSectorBuf,Ep2DataLength/SECTOR_SIZE);
  pEp2ReceiveBuf=SdSectorBuf; //���ý��ջ�������ַ
 }
 
 //���ݴ�����ϣ����뵽״̬�׶�
 if(Ep2DataLength==0)
 {
  Ep1DataLength=0;
  //��ʱEp1DataLengthΪ0�����Ҵ������ݽ׶Σ����÷������ݺ���������CSW
  Ep1SendData();
 }
}
////////////////////////End of function//////////////////////////////