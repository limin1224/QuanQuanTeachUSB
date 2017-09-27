/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
			
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

UsbCore.c file

���ߣ�����ȦȦ
��������: 2008.06.29
�޸�����: 2009-05-10
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
All rights reserved            
*******************************************************************/

#include <90USB162.H>
#include "config.h"
#include "AT90USB.h"
#include "uart.h"
#include "usbcore.h"
#include "led.h"

uint8 Buffer[16];  //���˵�0�õĻ�����

//USB�豸����ĸ��ֶ�
uint8  bmRequestType;
uint8  bRequest;
uint16 wValue;
uint16 wIndex;
uint16 wLength;
//��ǰ�������ݵ�λ��
uint32 pSendData;
//��Ҫ�������ݵĳ���
uint16 SendLength;
//�Ƿ���Ҫ����0���ݰ��ı�־����USB���ƴ�������ݹ����У�
//�����ص����ݰ��ֽ�������������ʱ������Ϊ���ݹ��̽�����
//��������ֽ�����ʵ����Ҫ���ص��ֽ���������ʵ�ʷ��ص��ֽ�
//���ָպ��Ƕ˵�0��С��������ʱ������Ҫ����һ��0���ȵ����ݰ�
//���������ݹ��̡������������һ����־������������Ƿ���Ҫ����
//һ��0���ȵ����ݰ���
uint8 NeedZeroPacket;

//��ǰ������ֵ��ֻ�������÷�0���ú�
uint8 ConfigValue;

//�˵�1��3�����Ƿ�æ�ı�־������������������ʱ���ñ�־Ϊ�档
//���������п���ʱ���ñ�־Ϊ�١�
uint8 Ep1InIsBusy;
uint8 Ep3InIsBusy;

//USB�豸�������Ķ���
const uint8 DeviceDescriptor[0x12]= //�豸������Ϊ18�ֽ�
{
//bLength�ֶΡ��豸�������ĳ���Ϊ18(0x12)�ֽ�
 0x12,
 
//bDescriptorType�ֶΡ��豸�������ı��Ϊ0x01
 0x01,
 
//bcdUSB�ֶΡ��������ð汾ΪUSB1.1����0x0110��
//������С�˽ṹ�����Ե��ֽ����ȣ���0x10��0x01��
 0x10,
 0x01,
 
//bDeviceClass�ֶΡ����ǲ����豸�������ж����豸�࣬
//���ڽӿ��������ж����豸�࣬���Ը��ֶε�ֵΪ0��
 0x00,
 
//bDeviceSubClass�ֶΡ�bDeviceClass�ֶ�Ϊ0ʱ�����ֶ�ҲΪ0��
 0x00,
 
//bDeviceProtocol�ֶΡ�bDeviceClass�ֶ�Ϊ0ʱ�����ֶ�ҲΪ0��
 0x00,
 
//bMaxPacketSize0�ֶΡ�AT90USB162�Ķ˵�0��СΪ8�ֽڡ�
 0x08,
 
//idVender�ֶΡ�����ID�ţ���������ȡ0x8888������ʵ���á�
//ʵ�ʲ�Ʒ�������ʹ�ó���ID�ţ������USBЭ�����볧��ID�š�
//ע��С��ģʽ�����ֽ����ȡ�
 0x88,
 0x88,
 
//idProduct�ֶΡ���ƷID�ţ������ǵ�һ��ʵ�飬��������ȡ0x2201��
//ע��С��ģʽ�����ֽ�Ӧ����ǰ��
 0x01,
 0x22,
 
//bcdDevice�ֶΡ��������USB���̸տ�ʼ�����ͽ���1.0��ɣ���0x0100��
//С��ģʽ�����ֽ����ȡ�
 0x00,
 0x01,
 
//iManufacturer�ֶΡ������ַ���������ֵ��Ϊ�˷������͹�����
//�ַ��������ʹ�1��ʼ�ɡ�
 0x01,

//iProduct�ֶΡ���Ʒ�ַ���������ֵ���ո�����1�������ȡ2�ɡ�
//ע���ַ�������ֵ��Ҫʹ����ͬ��ֵ��
 0x02,
 
//iSerialNumber�ֶΡ��豸�����к��ַ�������ֵ��
//����ȡ3�Ϳ����ˡ�
 0x03,

//bNumConfigurations�ֶΡ����豸�����е���������
//����ֻ��Ҫһ�����þ����ˣ���˸�ֵ����Ϊ1��
 0x01
};
//////////////////////////�豸���������//////////////////////////////

//USB�����������Ķ���
//�ñ�����������������������Ӧ�ü��ϡ������̺��û������豸��
const uint8 ReportDescriptor[]=
{
 //ÿ�п�ʼ�ĵ�һ�ֽ�Ϊ����Ŀ��ǰ׺��ǰ׺�ĸ�ʽΪ��
 //D7~D4��bTag��D3~D2��bType��D1~D0��bSize�����·ֱ��ÿ����Ŀע�͡�
 
 //����һ��ȫ�֣�bTypeΪ1����Ŀ��ѡ����;ҳΪ��ͨ����Generic Desktop Page(0x01)
 //�����һ�ֽ����ݣ�bSizeΪ1����������ֽ����Ͳ�ע���ˣ�
 //�Լ�����bSize���жϡ�
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 
 //����һ���ֲ���bTypeΪ2����Ŀ��˵����������Ӧ�ü�����;�������
 0x09, 0x02, // USAGE (Mouse)
 
 //����һ������Ŀ��bTypeΪ0����Ŀ�������ϣ������������0x01��ʾ
 //�ü�����һ��Ӧ�ü��ϡ�����������ǰ������;ҳ����;����Ϊ
 //��ͨ�����õ���ꡣ
 0xa1, 0x01, // COLLECTION (Application)
 
 //����һ���ֲ���Ŀ��˵����;Ϊָ�뼯��
 0x09, 0x01, //   USAGE (Pointer)
 
 //����һ������Ŀ�������ϣ������������0x00��ʾ�ü�����һ��
 //�������ϣ���;��ǰ��ľֲ���Ŀ����Ϊָ�뼯�ϡ�
 0xa1, 0x00, //   COLLECTION (Physical)
 
 //����һ��ȫ����Ŀ��ѡ����;ҳΪ������Button Page(0x09)��
 0x05, 0x09, //     USAGE_PAGE (Button)
 
 //����һ���ֲ���Ŀ��˵����;����СֵΪ1��ʵ��������������
 0x19, 0x01, //     USAGE_MINIMUM (Button 1)
 
 //����һ���ֲ���Ŀ��˵����;�����ֵΪ3��ʵ����������м���
 0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
 
 //����һ��ȫ����Ŀ��˵�����ص����ݵ��߼�ֵ���������Ƿ��ص��������ֵ����
 //��СΪ0����Ϊ����������Bit����ʾһ�������������СΪ0�����Ϊ1��
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 
 //����һ��ȫ����Ŀ��˵���߼�ֵ���Ϊ1��
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 
 //����һ��ȫ����Ŀ��˵�������������Ϊ������
 0x95, 0x03, //     REPORT_COUNT (3)
 
 //����һ��ȫ����Ŀ��˵��ÿ��������ĳ���Ϊ1��bit��
 0x75, 0x01, //     REPORT_SIZE (1)
 
 //����һ������Ŀ��˵����3������Ϊ1bit�������������ͳ���
 //��ǰ�������ȫ����Ŀ�����壩������Ϊ���룬
 //����Ϊ��Data,Var,Abs��Data��ʾ��Щ���ݿ��Ա䶯��Var��ʾ
 //��Щ�������Ƕ����ģ�ÿ�����ʾһ����˼��Abs��ʾ����ֵ��
 //��������Ľ�����ǣ���һ��������bit0��ʾ����1��������Ƿ��£�
 //�ڶ���������bit1��ʾ����2���Ҽ����Ƿ��£�������������bit2��ʾ
 //����3���м����Ƿ��¡�
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 
 //����һ��ȫ����Ŀ��˵������������Ϊ1��
 0x95, 0x01, //     REPORT_COUNT (1)
 
 //����һ��ȫ����Ŀ��˵��ÿ��������ĳ���Ϊ5bit��
 0x75, 0x05, //     REPORT_SIZE (5)
 
 //����һ������Ŀ�������ã���ǰ������ȫ����Ŀ��֪������Ϊ5bit��
 //����Ϊ1������������Ϊ�����������ص�����һֱ��0����
 //���ֻ��Ϊ�˴���һ���ֽڣ�ǰ������3��bit��������һЩ����
 //���ѣ���������û��ʵ����;�ġ�
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 
 //����һ��ȫ����Ŀ��ѡ����;ҳΪ��ͨ����Generic Desktop Page(0x01)
 0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
 
 //����һ���ֲ���Ŀ��˵����;ΪX��
 0x09, 0x30, //     USAGE (X)
 
 //����һ���ֲ���Ŀ��˵����;ΪY��
 0x09, 0x31, //     USAGE (Y)
 
 //����һ���ֲ���Ŀ��˵����;Ϊ����
 0x09, 0x38, //     USAGE (Wheel)
 
 //��������Ϊȫ����Ŀ��˵�����ص��߼���С�����ֵ��
 //��Ϊ���ָ���ƶ�ʱ��ͨ���������ֵ����ʾ�ģ�
 //���ֵ����˼���ǣ���ָ���ƶ�ʱ��ֻ�����ƶ�����
 //�����ƶ�ʱ��XֵΪ���������ƶ�ʱ��YֵΪ����
 //���ڹ��֣����������Ϲ�ʱ��ֵΪ����
 0x15, 0x81, //     LOGICAL_MINIMUM (-127)
 0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
 
 //����һ��ȫ����Ŀ��˵��������ĳ���Ϊ8bit��
 0x75, 0x08, //     REPORT_SIZE (8)
 
 //����һ��ȫ����Ŀ��˵��������ĸ���Ϊ3����
 0x95, 0x03, //     REPORT_COUNT (3)
 
 //����һ������Ŀ����˵��������8bit���������������õģ�
 //����Ϊ��Data,Var,Rel��Data˵�������ǿ��Ա�ģ�Var˵��
 //��Щ�������Ƕ����ģ�����һ��8bit��ʾX�ᣬ�ڶ���8bit��ʾ
 //Y�ᣬ������8bit��ʾ���֡�Rel��ʾ��Щֵ�����ֵ��
 0x81, 0x06, //     INPUT (Data,Var,Rel)
 
 //��������������Ŀ�����ر�ǰ��ļ����á�
 //���ǿ����������ϣ�����Ҫ�����Ρ�bSizeΪ0�����Ժ���û���ݡ�
 0xc0,       //   END_COLLECTION
 0xc0        // END_COLLECTION
};
//ͨ������ı����������Ķ��壬����֪�����ص����뱨�����4�ֽڡ�
//��һ�ֽڵĵ�3λ������ʾ�����Ƿ��µģ���5λΪ����0�����á�
//�ڶ��ֽڱ�ʾX��ĵı����������ֽڱ�ʾY��ĸı����������ֽڱ�ʾ
//���ֵĸı������������ж϶˵�1��Ӧ��Ҫ��������ĸ�ʽ����ʵ�ʵ�
//������ݡ�
///////////////////////////�������������////////////////////////////

//USB�������������ϵĶ���
//�����������ܳ���Ϊ9+9+9+7�ֽ�
const uint8 ConfigurationDescriptor[9+9+9+7]=
{
 /***************����������***********************/
 //bLength�ֶΡ������������ĳ���Ϊ9�ֽڡ�
 0x09,
 
 //bDescriptorType�ֶΡ��������������Ϊ0x02��
 0x02,
 
 //wTotalLength�ֶΡ��������������ϵ��ܳ��ȣ�
 //���������������������ӿ��������������������˵��������ȡ�
 sizeof(ConfigurationDescriptor)&0xFF, //���ֽ�
 (sizeof(ConfigurationDescriptor)>>8)&0xFF, //���ֽ�
 
 //bNumInterfaces�ֶΡ������ð����Ľӿ�����ֻ��һ���ӿڡ�
 0x01,
 
 //bConfiguration�ֶΡ������õ�ֵΪ1��
 0x01,
 
 //iConfigurationz�ֶΣ������õ��ַ�������������û�У�Ϊ0��
 0x00,
 
 //bmAttributes�ֶΣ����豸�����ԡ��������ǵİ��������߹���ģ�
 //�������ǲ���ʵ��Զ�̻��ѵĹ��ܣ����Ը��ֶε�ֵΪ0x80��
 0x80,
 
 //bMaxPower�ֶΣ����豸��Ҫ�������������������ǵİ���
 //��Ҫ�ĵ�������100mA�����������������Ϊ100mA������ÿ��λ
 //����Ϊ2mA��������������Ϊ50(0x32)��
 0x32,
 
 /*******************�ӿ�������*********************/
 //bLength�ֶΡ��ӿ��������ĳ���Ϊ9�ֽڡ�
 0x09,
 
 //bDescriptorType�ֶΡ��ӿ��������ı��Ϊ0x04��
 0x04,
 
 //bInterfaceNumber�ֶΡ��ýӿڵı�ţ���һ���ӿڣ����Ϊ0��
 0x00,
 
 //bAlternateSetting�ֶΡ��ýӿڵı��ñ�ţ�Ϊ0��
 0x00,
 
 //bNumEndpoints�ֶΡ���0�˵����Ŀ����USB������Ҫ����
 //�ж϶˵㣨һ������һ�����������˸�ֵΪ2��
 0x02,
 
 //bInterfaceClass�ֶΡ��ýӿ���ʹ�õ��ࡣUSB������HID�࣬
 //HID��ı���Ϊ0x03��
 0x03,
 
 //bInterfaceSubClass�ֶΡ��ýӿ���ʹ�õ����ࡣ��HID1.1Э���У�
 //ֻ�涨��һ�����ࣺ֧��BIOS�������������ࡣ
 //USB���̡�������ڸ����࣬�������Ϊ0x01��
 0x01,
 
 //bInterfaceProtocol�ֶΡ��������Ϊ֧���������������࣬
 //��Э���ѡ�����ͼ��̡����̴���Ϊ0x01��������Ϊ0x02��
 0x01,
 
 //iConfiguration�ֶΡ��ýӿڵ��ַ�������ֵ������û�У�Ϊ0��
 0x00,
 
 /******************HID������************************/
 //bLength�ֶΡ���HID��������ֻ��һ���¼������������Գ���Ϊ9�ֽڡ�
 0x09,
 
 //bDescriptorType�ֶΡ�HID�������ı��Ϊ0x21��
 0x21,
 
 //bcdHID�ֶΡ���Э��ʹ�õ�HID1.1Э�顣ע����ֽ����ȡ�
 0x10,
 0x01,
 
 //bCountyCode�ֶΡ��豸���õĹ��Ҵ��룬����ѡ��Ϊ����������0x21��
 0x21,
 
 //bNumDescriptors�ֶΡ��¼�����������Ŀ������ֻ��һ��������������
 0x01,
 
 //bDescritporType�ֶΡ��¼������������ͣ�Ϊ���������������Ϊ0x22��
 0x22,
 
 //bDescriptorLength�ֶΡ��¼��������ĳ��ȡ��¼�������Ϊ������������
 sizeof(ReportDescriptor)&0xFF,
 (sizeof(ReportDescriptor)>>8)&0xFF,
 
 /**********************����˵�������***********************/
 //bLength�ֶΡ��˵�����������Ϊ7�ֽڡ�
 0x07,
 
 //bDescriptorType�ֶΡ��˵����������Ϊ0x05��
 0x05,
 
 //bEndpointAddress�ֶΡ��˵�ĵ�ַ������ʹ��AT91SAM7������˵�1��
 //D7λ��ʾ���ݷ�������˵�D7Ϊ1����������˵�1�ĵ�ַΪ0x81��
 0x81,
 
 //bmAttributes�ֶΡ�D1~D0Ϊ�˵㴫������ѡ��
 //�ö˵�Ϊ�ж϶˵㡣�ж϶˵�ı��Ϊ3������λ����Ϊ0��
 0x03,
 
 //wMaxPacketSize�ֶΡ��ö˵�����������˵�1��������Ϊ16�ֽڡ�
 //ע����ֽ����ȡ�
 0x10,
 0x00,
 
 //bInterval�ֶΡ��˵��ѯ��ʱ�䣬��������Ϊ10��֡ʱ�䣬��10ms��
 0x0A
};
////////////////////////�����������������//////////////////////////

/************************����ID�Ķ���********************/
const uint8 LanguageId[4]=
{
 0x04, //���������ĳ���
 0x03, //�ַ���������
 //0x0409Ϊ��ʽӢ���ID
 0x09,
 0x04
};
////////////////////////����ID���//////////////////////////////////

//**************************************************/
//*********        ��ת���������         **********/
//********* Http://computer00.21ic.org    **********/
//*********        ����: ����ȦȦ         **********/
//*********         ��ӭ���ʹ��          **********/
//*********    ��Ȩ���У�������д������   **********/
//**************************************************/

//http://computer00.21ic.org/user1/2198/archives/2007/42769.html
//�ַ���������ȦȦ��USBר�� Http://group.ednchina.com/93/����Unicode����
//8λС�˸�ʽ
const uint8 ManufacturerStringDescriptor[82]={
 82,         //���������ĳ���Ϊ82�ֽ�
 0x03,       //�ַ��������������ͱ���Ϊ0x03
 0x35, 0x75, //��
 0x11, 0x81, //��
 0x08, 0x57, //Ȧ
 0x08, 0x57, //Ȧ
 0x84, 0x76, //��
 0x55, 0x00, //U
 0x53, 0x00, //S
 0x42, 0x00, //B
 0x13, 0x4e, //ר
 0x3a, 0x53, //��
 0x20, 0x00, // 
 0x48, 0x00, //H
 0x74, 0x00, //t
 0x74, 0x00, //t
 0x70, 0x00, //p
 0x3a, 0x00, //:
 0x2f, 0x00, ///
 0x2f, 0x00, ///
 0x67, 0x00, //g
 0x72, 0x00, //r
 0x6f, 0x00, //o
 0x75, 0x00, //u
 0x70, 0x00, //p
 0x2e, 0x00, //.
 0x65, 0x00, //e
 0x64, 0x00, //d
 0x6e, 0x00, //n
 0x63, 0x00, //c
 0x68, 0x00, //h
 0x69, 0x00, //i
 0x6e, 0x00, //n
 0x61, 0x00, //a
 0x2e, 0x00, //.
 0x63, 0x00, //c
 0x6f, 0x00, //o
 0x6d, 0x00, //m
 0x2f, 0x00, ///
 0x39, 0x00, //9
 0x33, 0x00, //3
 0x2f, 0x00  ///
};
/////////////////////////�����ַ�������/////////////////////////////

//�ַ�������ȦȦ������USB�� ֮USB��ꡱ��Unicode����
//8λС�˸�ʽ
const uint8 ProductStringDescriptor[36]={
 36,         //���������ĳ���Ϊ36�ֽ�
 0x03,       //�ַ��������������ͱ���Ϊ0x03
 0x0a, 0x30, //��
 0x08, 0x57, //Ȧ
 0x08, 0x57, //Ȧ
 0x59, 0x65, //��
 0x60, 0x4f, //��
 0xa9, 0x73, //��
 0x55, 0x00, //U
 0x53, 0x00, //S
 0x42, 0x00, //B
 0x0b, 0x30, //��
 0x20, 0x00, // 
 0x4b, 0x4e, //֮
 0x55, 0x00, //U
 0x53, 0x00, //S
 0x42, 0x00, //B
 0x20, 0x9f, //��
 0x07, 0x68  //��
};
////////////////////////��Ʒ�ַ�������////////////////////////////

//�ַ�����2009-05-10����Unicode����
//8λС�˸�ʽ
const uint8 SerialNumberStringDescriptor[22]={
22,         //���������ĳ���Ϊ22�ֽ�
0x03,       //�ַ��������������ͱ���Ϊ0x03
0x32, 0x00, //2
0x30, 0x00, //0
0x30, 0x00, //0
0x39, 0x00, //9
0x2d, 0x00, //-
0x30, 0x00, //0
0x35, 0x00, //5
0x2d, 0x00, //-
0x31, 0x00, //1
0x30, 0x00  //0
};
//////////////////////��Ʒ���к��ַ�������/////////////////////////

/********************************************************************
�������ܣ����߹����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbBusSuspend(void)
{
#ifdef DEBUG0
 Prints("USB���߹���\r\n");
#endif
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ����߸�λ�жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbBusReset(void)
{
#ifdef DEBUG0
 Prints("USB���߸�λ��\r\n");
#endif
 UsbChipResetEndpoint(); //��λ�˵�
 ConfigValue=0; //����ֵ��ʼ��Ϊ0
 UsbChipSetConfig(0);  //����оƬ������ֵΪ0
 NeedZeroPacket=0;
 SendLength=0;
 Ep1InIsBusy=0;  //��λ��˵�1���뻺�������С�
 Ep3InIsBusy=0;  //��λ��˵�3���뻺�������С�
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ�����pData��SendLength�����ݷ��͵��˵�0�ĺ�����
��ڲ������ޡ�
��    �أ��ޡ�
��    ע����pSendData�����8λ����ָ������ͣ������8λΪ0x01ʱ��ָ��RAM����
          ����ָ��FLASH����
********************************************************************/ 
void UsbEp0SendData(void)
{
 //������д���˵���ȥ׼������
 //д֮ǰҪ���ж�һ����Ҫ���͵������Ƿ�ȶ˵�0
 //��󳤶ȴ���������˵��С����һ��ֻ�ܷ���
 //�����������ݡ��˵�0����������DeviceDescriptor[7]
 if(SendLength>DeviceDescriptor[7])
 {
  //���������ȷ���
  if((pSendData&0xFF000000)==0x01000000)  //RAM�е�����
  {
   UsbChipWriteEndpointBuffer(0,DeviceDescriptor[7],(uint8 *)(pSendData&0xFFFF));
  }
  else //FLASH�е�����
  {
   DataInFlash_WriteEndpointBuffer(0,DeviceDescriptor[7],(flash uint8 *)(pSendData&0xFFFF));
  }
  //���ͺ�ʣ���ֽ�������������
  SendLength-=DeviceDescriptor[7];
  //����һ�κ�ָ��λ��Ҫ����
  pSendData+= DeviceDescriptor[7];
 }
 else
 {
  if(SendLength!=0)
  {
   //����������������ֱ�ӷ���
   if((pSendData&0xFF000000)==0x01000000)  //RAM�е�����
   {
    UsbChipWriteEndpointBuffer(0,SendLength,(uint8 *)(pSendData&0xFFFF));
   }
   else //FLASH�е�����
   {
    DataInFlash_WriteEndpointBuffer(0,SendLength,(flash uint8 *)(pSendData&0xFFFF));
   }
   //������Ϻ�SendLength���ȱ�Ϊ0
   SendLength=0;
  }
  else //���Ҫ���͵����ݰ�����Ϊ0
  {
   if(NeedZeroPacket==1) //�����Ҫ����0��������
   {
    UsbChipWriteEndpointBuffer(0,0,0); //����0�������ݰ�
    NeedZeroPacket=0; //����Ҫ����0�������ݰ���־
   }
  }
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�0����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp0Out(void)
{
 int32 L;
#ifdef DEBUG0
 Prints("USB�˵�0����жϡ�\r\n");
#endif
 //�ж��Ƿ��ǽ�����
 if(UsbChipIsSetup(0))
 {
  L=UsbChipReadEndpointBuffer(0,16,Buffer); //��������������
  UsbChipAcknowledgeSetup(0); //Ӧ��������ͬʱ����˵㻺����
  //UsbChipClearBuffer(0);      //�建������������Ӧ������ʱ����������������ﲻ��
  if(L!=8) //����8�ֽڵı�׼����ֱ�ӷ���
   return;
  
  //������������豸����ĸ��ֶ���
  bmRequestType=Buffer[0];
  bRequest=Buffer[1];
  wValue=Buffer[2]+(((uint16)Buffer[3])<<8);
  wIndex=Buffer[4]+(((uint16)Buffer[5])<<8);
  wLength=Buffer[6]+(((uint16)Buffer[7])<<8);
  //����Ĵ����жϾ�������󣬲����ݲ�ͬ�����������ز���
  //���D7λΪ1����˵������������
  if((bmRequestType&0x80)==0x80)
  {
   //����bmRequestType��D6~5λɢת��D6~5λ��ʾ���������
   //0Ϊ��׼����1Ϊ������2Ϊ��������
   switch((bmRequestType>>5)&0x03)
   {
    case 0:  //��׼����
     #ifdef DEBUG0
      Prints("USB��׼��������");
     #endif
     //USBЭ�鶨���˼�����׼������������ʵ����Щ��׼���󼴿�
     //����Ĵ�����bRequest�У��Բ�ͬ������������ɢת
     //��ʵ�ϣ����ǻ���Ҫ�Խ����߽���ɢת����Ϊ��ͬ�����������
     //�ǲ�һ���ġ���������bmRequestType��D4~D0λ�ж��塣
     //��������Ϊ�˼򻯲�������Щ��ʡ���˶Խ����ߵ��жϡ�
     //�����ȡ������������ֻ����������������������
     switch(bRequest)
     {
      case GET_CONFIGURATION: //��ȡ����
       #ifdef DEBUG0
        Prints("��ȡ���á�\r\n");
       #endif
      break;
      
      case GET_DESCRIPTOR:  //��ȡ������
       #ifdef DEBUG0
        Prints("��ȡ����������");
       #endif
       //�����������ͽ���ɢת������ȫ���豸��
       //��׼����ֻ֧�ַ��͵��豸���豸�����á��ַ�������������
       switch((wValue>>8)&0xFF)
        {
         case DEVICE_DESCRIPTOR: //�豸������
          #ifdef DEBUG0
           Prints("�豸��������\r\n");
          #endif
          pSendData=(uint32)DeviceDescriptor;  //��Ҫ���͵�����
          //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ�����
          //������������豸��������������ݳ��Ⱦ���
          //DeviceDescriptor[0]���������ı�ʵ�ʵĳ���
          //��ôֻ����ʵ�ʳ��ȵ�����
          if(wLength>DeviceDescriptor[0])
          {
           SendLength=DeviceDescriptor[0];
           if(SendLength%DeviceDescriptor[7]==0) //���Ҹպ������������ݰ�ʱ
           {
            NeedZeroPacket=1; //��Ҫ����0���ȵ����ݰ�
           }
          }
          else
          {
           SendLength=wLength;
          }
          //������ͨ��EP0����
          UsbEp0SendData();
         break;
         
         case CONFIGURATION_DESCRIPTOR:  //����������
          #ifdef DEBUG0
           Prints("������������\r\n");
          #endif
          pSendData=(uint32)ConfigurationDescriptor; //��Ҫ���͵�����Ϊ����������
          //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ�����
          //������������������������ϣ�������ݳ��Ⱦ���
          //ConfigurationDescriptor[3]*256+ConfigurationDescriptor[2]��
          //�������ı�ʵ�ʵĳ�����ôֻ����ʵ�ʳ��ȵ�����
          SendLength=ConfigurationDescriptor[3];
          SendLength=SendLength*256+ConfigurationDescriptor[2];
          if(wLength>SendLength)
          {
           if(SendLength%DeviceDescriptor[7]==0) //���Ҹպ������������ݰ�ʱ
           {
            NeedZeroPacket=1; //��Ҫ����0���ȵ����ݰ�
           }
          }
          else
          {
           SendLength=wLength;
          }
          //������ͨ��EP0����
          UsbEp0SendData();
         break;
         
         case STRING_DESCRIPTOR:  //�ַ���������
          #ifdef DEBUG0
           Prints("�ַ���������");
          #endif
          switch(wValue&0xFF)  //����wValue�ĵ��ֽڣ�����ֵ��ɢת
          {
           case 0:  //��ȡ����ID
            #ifdef DEBUG0
             Prints("(����ID)��\r\n");
            #endif
            pSendData=(uint32)LanguageId;
            SendLength=LanguageId[0];
           break;
           
           case 1:  //�����ַ���������ֵΪ1����������Ϊ�����ַ���
           #ifdef DEBUG0
             Prints("(��������)��\r\n");
            #endif
            pSendData=(uint32)ManufacturerStringDescriptor;
            SendLength=ManufacturerStringDescriptor[0];
           break;
           
           case 2:  //��Ʒ�ַ���������ֵΪ2����������Ϊ��Ʒ�ַ���
           #ifdef DEBUG0
             Prints("(��Ʒ����)��\r\n");
            #endif
            pSendData=(uint32)ProductStringDescriptor;
            SendLength=ProductStringDescriptor[0];
           break;
           
           case 3:  //��Ʒ���кŵ�����ֵΪ3����������Ϊ���к�
           #ifdef DEBUG0
             Prints("(��Ʒ���к�)��\r\n");
            #endif
            pSendData=(uint32)SerialNumberStringDescriptor;
            SendLength=SerialNumberStringDescriptor[0];
           break;
           
           default :
            #ifdef DEBUG0
             Prints("(δ֪������ֵ)��\r\n");
            #endif
            //����δ֪����ֵ�����󣬷���һ��0���ȵİ�
            SendLength=0;
            NeedZeroPacket=1;
           break;
          }
          //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ�����
          //�������ı�ʵ�ʵĳ�����ôֻ����ʵ�ʳ��ȵ�����
          if(wLength>SendLength)
          {
           if(SendLength%DeviceDescriptor[7]==0) //���Ҹպ������������ݰ�ʱ
           {
            NeedZeroPacket=1; //��Ҫ����0���ȵ����ݰ�
           }
          }
          else
          {
           SendLength=wLength;
          }
          //������ͨ��EP0����
          UsbEp0SendData();         
         break;

         case REPORT_DESCRIPTOR:  //����������
          #ifdef DEBUG0
           Prints("������������\r\n");
          #endif
          pSendData=(uint32)ReportDescriptor; //��Ҫ���͵�����Ϊ����������
          SendLength=sizeof(ReportDescriptor); //��Ҫ���ص����ݳ���         
          //�ж�������ֽ����Ƿ��ʵ����Ҫ���͵��ֽ�����
          //�������ı�ʵ�ʵĳ�����ôֻ����ʵ�ʳ��ȵ�����
          if(wLength>SendLength)
          {
           if(SendLength%DeviceDescriptor[7]==0) //���Ҹպ������������ݰ�ʱ
           {
            NeedZeroPacket=1; //��Ҫ����0���ȵ����ݰ�
           }
          }
          else
          {
           SendLength=wLength;
          }
          //������ͨ��EP0����
          UsbEp0SendData();
         break;
                 
         default:  //����������
          #ifdef DEBUG0
           Prints("���������������������룺");
           PrintHex((wValue>>8)&0xFF);
           Prints("\r\n");
          #endif
         break;
        }
       break;
      
      case GET_INTERFACE: //��ȡ�ӿ�
       #ifdef DEBUG0
        Prints("��ȡ�ӿڡ�\r\n");
       #endif
      break;
      
      case GET_STATUS: //��ȡ״̬
       #ifdef DEBUG0
        Prints("��ȡ״̬��\r\n");
       #endif
      break;
      
      case SYNCH_FRAME: //ͬ��֡
       #ifdef DEBUG0
        Prints("ͬ��֡��\r\n");
       #endif
      break;
      
      default:  //δ����ı�׼����
       #ifdef DEBUG0
        Prints("����δ����ı�׼��������\r\n");
       #endif       
      break;
     }
    break;
    
    case 1:  //������
     #ifdef DEBUG0
      Prints("USB����������\r\n");
     #endif
    break;
    
    case 2:  //��������
     #ifdef DEBUG0
      Prints("USB������������\r\n");
     #endif
    break;
    
    default: //δ�������������ֻ��ʾһ��������Ϣ��
     #ifdef DEBUG0
      Prints("����δ�������������\r\n");
     #endif
    break;
   }
  }
  //����˵�����������
  else //if(bmRequestType&0x80==0x80)֮else
  {
   //����bmRequestType��D6~5λɢת��D6~5λ��ʾ���������
   //0Ϊ��׼����1Ϊ������2Ϊ��������
   switch((bmRequestType>>5)&0x03)
   {
    case 0:  //��׼����
     #ifdef DEBUG0
      Prints("USB��׼�������");
     #endif
     //USBЭ�鶨���˼�����׼�����������ʵ����Щ��׼���󼴿�
     //����Ĵ�����bRequest�У��Բ�ͬ������������ɢת
     switch(bRequest)
     {
      case CLEAR_FEATURE: //�������
       #ifdef DEBUG0
        Prints("������ԡ�\r\n");
       #endif
      break;
      
      case SET_ADDRESS:  //���õ�ַ
       #ifdef DEBUG0
        Prints("���õ�ַ����ַΪ��");
        PrintHex(wValue&0xFF); //��ʾ�����õĵ�ַ
        Prints("\r\n");
       #endif
       //���õ�ַû�����ݹ��̣�ֱ�ӽ��뵽״̬���̣�����һ��0���ȵ����ݰ�
       SendLength=0;
       NeedZeroPacket=1;
       //������ͨ��EP0����
       UsbEp0SendData();
       UsbChipWriteAddress(wValue&0xFF); //wValue�еĵ��ֽ������õĵ�ֵַ
      break;
      
      case SET_CONFIGURATION: //��������
       #ifdef DEBUG0
        Prints("�������á�\r\n");
       #endif
       //wValue�ĵ��ֽ�Ϊ���õ�ֵ�������ֵΪ��0������ʹ�ܷ�0�˵㡣        
       //���浱ǰ����ֵ
       ConfigValue=wValue&0xFF;
       //����оƬ��configֵ     
       UsbChipSetConfig(ConfigValue); 
         
       //ʹ�ܷ�0�˵㡣��0�˵�ֻ��������Ϊ��0�����ú����ʹ�ܡ�
       UsbChipSetEndpointEnable(ConfigValue);
      
       //����һ��0���ȵ�״̬���ݰ�
       SendLength=0;
       NeedZeroPacket=1;
       //������ͨ��EP0����
       UsbEp0SendData();
      break;
      
      case SET_DESCRIPTOR: //����������
       #ifdef DEBUG0
        Prints("������������\r\n");
       #endif
      break;
      
      case SET_FEATURE: //��������
       #ifdef DEBUG0
        Prints("�������ԡ�\r\n");
       #endif
      break;

      case SET_INTERFACE: //���ýӿ�
       #ifdef DEBUG0
        Prints("���ýӿڡ�\r\n");
       #endif
      break;      
      
      default:  //δ����ı�׼����
       #ifdef DEBUG0
        Prints("����δ����ı�׼�������\r\n");
        Prints("δ֪����\r\n");        
       #endif
        //ֻ��Ҫ����һ��0���ȵ����ݰ�����
        SendLength=0;
        NeedZeroPacket=1;
        //������ͨ��EP0����
        UsbEp0SendData(); 
      break;
     }
    break;
    
    case 1:  //������
     #ifdef DEBUG0
      Prints("USB���������");
     #endif
     switch(bRequest)
     {
      case SET_IDLE:
       #ifdef DEBUG0
        Prints("���ÿ��С�\r\n");
       #endif
       //ֻ��Ҫ����һ��0���ȵ����ݰ�����
       SendLength=0;
       NeedZeroPacket=1;
       //������ͨ��EP0����
       UsbEp0SendData();
      break;
      
      default:
       #ifdef DEBUG0
        Prints("δ֪����\r\n");
       #endif        
        //ֻ��Ҫ����һ��0���ȵ����ݰ�����
        SendLength=0;
        NeedZeroPacket=1;
        //������ͨ��EP0����
        UsbEp0SendData();
      break;
     }
    break;
    
    case 2:  //��������
     #ifdef DEBUG0
      Prints("USB�����������\r\n");
     #endif
    break;
    
    default: //δ�������������ֻ��ʾһ��������Ϣ��
     #ifdef DEBUG0
      Prints("����δ������������\r\n");
     #endif
    break;
   }
  }
 }
 //��ͨ�������
 else //�ж�setup��֮else
 {
  UsbChipReadEndpointBuffer(0,16,Buffer);
  //UsbChipClearBuffer(0);
  UEINTX=~(1<<2); //���RXOUTI��������жϱ�־�Լ�������
 }
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�0�����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp0In(void)
{
#ifdef DEBUG0
// Prints("USB�˵�0�����жϡ�\r\n");
#endif
 
 //����ʣ����ֽ���
 UsbEp0SendData();
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�1�����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp1In(void)
{
#ifdef DEBUG0
 Prints("USB�˵�1�����жϡ�\r\n");
#endif
 //����˵�1����������жϱ�־λ
 UEINTX=~(1<<0);
 //�˵�1���봦�ڿ���״̬
 Ep1InIsBusy=0;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�2����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp2Out(void)
{
 uint8 Buf[2]; //��������2�ֽڵ�������棬����LED��
#ifdef DEBUG0
 Prints("USB�˵�2����жϡ�\r\n");
#endif
 //����˵�2������жϱ�־λ
 UEINTX=~(1<<2);
 
 //�Ӷ˵�2����������1�ֽ�����
 UsbChipReadEndpointBuffer(2,2,Buf);
 //����˵㻺����
 UsbChipClearBuffer(2);
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�3�����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp3In(void)
{
#ifdef DEBUG0
 Prints("USB�˵�3�����жϡ�\r\n");
#endif
 //����˵�3����������жϱ�־λ
 UEINTX=~(1<<0);
 //�˵�3���봦�ڿ���״̬
 Ep3InIsBusy=0;
}
////////////////////////End of function//////////////////////////////

/********************************************************************
�������ܣ��˵�4����жϴ���������
��ڲ������ޡ�
��    �أ��ޡ�
��    ע���ޡ�
********************************************************************/
void UsbEp4Out(void)
{
#ifdef DEBUG0
 Prints("USB�˵�4����жϡ�\r\n");
#endif
 //����˵�4������жϱ�־λ
 UEINTX=~(1<<2);
}
////////////////////////End of function//////////////////////////////