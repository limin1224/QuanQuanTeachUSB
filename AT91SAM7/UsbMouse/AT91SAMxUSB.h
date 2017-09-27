/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

AT91SAMxUSB.H  file

���ߣ�����ȦȦ
��������: 2008.06.27
�޸�����: 2009.03.08
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/


#ifndef __AT91SAMXUSB_H__
#define __AT91SAMXUSB_H__

#include "MyType.h"

//��������
void UsbDisconnect(void); //USB�Ͽ�����
void UsbConnect(void);    //USB����
void UsbChipInit(void);
void UsbChipResetEndpoint(void);
void UsbChipSetConfig(uint8 Value);
void UsbChipWriteAddress(uint8 Addr);
void UsbChipSetAddressStatus(uint8 Value);
int  UsbChipIsSetup(uint8 Endp);
uint8 UsbChipReadEndpointBuffer(uint8 Endp, uint8 Len, uint8 *Buf);
uint8 UsbChipWriteEndpointBuffer(uint8 Endp, uint8 Len, uint8 *Buf);
void UsbChipClearBuffer(uint8 Endp);
void UsbChipAcknowledgeSetup(uint8 Endp);
void UsbChipSetAddress(uint8 Addr);
void UsbChipSetEndpointEnable(uint8 Enable);

#endif
