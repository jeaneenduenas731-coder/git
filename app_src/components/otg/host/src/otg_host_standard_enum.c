 /**
 *****************************************************************************
 * @file     otg_host_stor_request.c
 * @author   owen
 * @version  V1.0.0
 * @date     7-September-2015
 * @brief    otg host standard enum driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2015 MVSilicon </center></h2>
 */

#include <string.h>

#include "type.h"
#include "debug.h"
#include "delay.h"
#include "app_config.h"
//#include "otg_hal.h"
#include "otg_host_hcd.h"
#include "otg_host_standard_enum.h"
//#include "otg_host_udisk.h"
#ifdef FUNC_OS_EN
#include "rtos_api.h"
#endif
extern int printf(const char *fmt, ...);
#undef  OTG_DBG
#define	OTG_DBG(format, ...)		//printf(format, ##__VA_ARGS__)  //bkd mark

//uint8_t gMaxPacketSize0 = 8;//EP0最大包长度，默认为64  //zsq
#ifndef FUNC_OS_EN
static uint8_t DesCriptorBuffer[MAX_DESCRIPTOR_LEN];//512 BYTE
#endif
OTG_HOST_INFO OtgHostInfo;

bool OTG_HostGetDescriptor(uint8_t Type, uint8_t Index, uint8_t* Buf, uint16_t Size)
{
	USB_CTRL_SETUP_REQUEST SetupPacket;	
	uint8_t CmdGetDescriptor[8] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00};
	CmdGetDescriptor[2] = Index;
	CmdGetDescriptor[3] = Type;
	CmdGetDescriptor[6] = Size;
	CmdGetDescriptor[7] = Size>>8;

	if(Type == USB_DT_STRING)
	{
		CmdGetDescriptor[4] = 0x09;
		CmdGetDescriptor[5] = 0x04;
	}
	memcpy((uint8_t *)&SetupPacket,CmdGetDescriptor,8);
	if(OTG_HostControlRead(SetupPacket,Buf,Size,1000) == HOST_NONE_ERR)
	{
		return TRUE;
	}
	return FALSE;
}

extern void osTaskDelay(uint32_t Cnt);
bool OTG_HostSetAddress(uint8_t Address)
{
	USB_CTRL_SETUP_REQUEST SetupPacket;		
	uint8_t CmdSetAddress[8] = {0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

	CmdSetAddress[2] = Address;
	memcpy((uint8_t *)&SetupPacket,CmdSetAddress,8);
	
	if(OTG_HostControlWrite(SetupPacket, NULL, 0,1000) != HOST_NONE_ERR)
	{
#ifdef FUNC_OS_EN
		osTaskDelay(20);
#else
		WaitMs(20);
#endif
		if(OTG_HostControlWrite(SetupPacket, NULL, 0,1000) != HOST_NONE_ERR)
		{
#ifdef FUNC_OS_EN
			osTaskDelay(20);
#else
			WaitMs(20);
#endif
			if(OTG_HostControlWrite(SetupPacket, NULL, 0,1000) != HOST_NONE_ERR)
			{
				return FALSE;
			}
		}
	}
	
	OTG_HostAddressSet(Address);
#ifdef FUNC_OS_EN
	osTaskDelay(20);
#else
	WaitMs(20);//等待Device将ADDRESS设置OK
#endif
	return TRUE;
}

/**
 * @brief  设置配置
 * @param  ConfigurationNum 配置号
 * @return 1-成功，0-失败
 */
bool OTG_HostSetConfiguration(uint8_t ConfigurationNum)
{
	USB_CTRL_SETUP_REQUEST SetupPacket;	
	uint8_t CmdSetConfig[8] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
	CmdSetConfig[2] = ConfigurationNum;
	memcpy((uint8_t *)&SetupPacket,CmdSetConfig,8);

	if(OTG_HostControlWrite(SetupPacket, NULL, 0, 1000) != HOST_NONE_ERR)
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief  设置接口
 * @param  InterfaceNum 接口号
 * @return 1-成功，0-失败
 */
bool OTG_HostSetInterface(uint8_t InterfaceNum)
{
	USB_CTRL_SETUP_REQUEST SetupPacket;	
	uint8_t CmdSetInterface[8] = {0x01, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	CmdSetInterface[4] = InterfaceNum;
	memcpy((uint8_t *)&SetupPacket,CmdSetInterface,8);

	if(OTG_HostControlWrite(SetupPacket, NULL, 0, 1000) != HOST_NONE_ERR)
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief  设置Ipod充电接口
 * @param  Current 充电电流   0~500ma
 * @return 1-成功，0-失败
 */
bool OTG_HostSetIpodChargeCurrent(int16_t Current)
{
	USB_CTRL_SETUP_REQUEST SetupPacket;
	uint8_t CmdSetCurrent[8] = {0x40, 0x40, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00};
	CmdSetCurrent[4] = (uint8_t)(Current);
	CmdSetCurrent[5] = (uint8_t)(Current >> 8);
	memcpy((uint8_t *)&SetupPacket,CmdSetCurrent,8);
	if(OTG_HostControlWrite(SetupPacket, NULL, 0, 1000) != HOST_NONE_ERR)
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief  host枚举初始化
 * @param  NONE
 * @return 1-枚举成功，0-枚举失败
 */
bool OTG_HostEnumDevice(void)
{
	uint16_t	DescriptorLen;
	uint8_t		DescriptorType;
	uint8_t		InterfaceNum = 0;
	uint16_t	i;
	uint8_t	*pBuf;
	OTG_DBG("HOST Start enum Device\n");
	OTG_DBG("Get Device  Descriptor\n");
	if(!OTG_HostGetDescriptor(USB_DT_DEVICE, 0, (uint8_t*)&OtgHostInfo.DeviceDesCriptor, 8))
	{
		if(!OTG_HostGetDescriptor(USB_DT_DEVICE, 0, (uint8_t*)&OtgHostInfo.DeviceDesCriptor, 8))
		{
			if(!OTG_HostGetDescriptor(USB_DT_DEVICE, 0, (uint8_t*)&OtgHostInfo.DeviceDesCriptor, 8))
			{
				OTG_DBG("GET DESCRIPTOR Error\n");
				return FALSE;
			}
		}
	}

	gMaxPacketSize0 = OtgHostInfo.DeviceDesCriptor.bMaxPacketSize0;
	DescriptorLen = OtgHostInfo.DeviceDesCriptor.bLength;
	OTG_DBG("Set Device  Address\n");
	if(!OTG_HostSetAddress(1))
	{
		return FALSE;
	}
	OTG_DBG("Get All Device  Descriptor\n");
	if(!OTG_HostGetDescriptor(USB_DT_DEVICE, 0, (uint8_t *)&OtgHostInfo.DeviceDesCriptor, DescriptorLen))
	{
		return FALSE;
	}
	
	//设备描述符
	OTG_DBG("Device DeviceDesCriptor:\n");
	OTG_DBG("bLength:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bLength);
	OTG_DBG("bDescriptorType:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bDescriptorType);
	OTG_DBG("bcdUSB:\t%04X \n",OtgHostInfo.DeviceDesCriptor.bcdUSB);
	OTG_DBG("bDeviceClass:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bDeviceClass);
	OTG_DBG("bDeviceSubClass:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bDeviceSubClass);
	OTG_DBG("bDeviceProtocol:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bDeviceProtocol);
	OTG_DBG("bMaxPacketSize0:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bMaxPacketSize0);
	OTG_DBG("idVendor:\t%04X \n",OtgHostInfo.DeviceDesCriptor.idVendor);
	OTG_DBG("idProduct:\t%04X \n",OtgHostInfo.DeviceDesCriptor.idProduct);
	OTG_DBG("bcdDevice:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bcdDevice);
	OTG_DBG("iManufacturer:\t%02X \n",OtgHostInfo.DeviceDesCriptor.iManufacturer);
	OTG_DBG("iProduct:\t%02X \n",OtgHostInfo.DeviceDesCriptor.iProduct);
	OTG_DBG("iSerialNumber:\t%02X \n",OtgHostInfo.DeviceDesCriptor.iSerialNumber);
	OTG_DBG("bNumConfigurations:\t%02X \n",OtgHostInfo.DeviceDesCriptor.bNumConfigurations);

	if(OtgHostInfo.DeviceDesCriptor.idVendor == 0x05AC)			//Apple
	{
		OTG_HostSetIpodChargeCurrent(500);
	}

	if(OtgHostInfo.DeviceDesCriptor.bNumConfigurations != 1)
	{
		return FALSE;
	}

	OTG_DBG("Get Device ConfigCriptor\n");
	if(!OTG_HostGetDescriptor(USB_DT_CONFIG, 0, (uint8_t *)&OtgHostInfo.ConfigDesCriptor, 9))
	{
		return FALSE;
	}

	DescriptorLen = OtgHostInfo.ConfigDesCriptor.wTotalLength;

#ifdef FUNC_OS_EN
	OtgHostInfo.DesCriptorBuffer = osPortMalloc(DescriptorLen);
	if(!OtgHostInfo.DesCriptorBuffer)
	{
		OTG_DBG("DesCriptorBuffer ERROR\n");
		return FALSE;
	}
#else
	if(DescriptorLen > MAX_DESCRIPTOR_LEN)
	{
		OTG_DBG("DesCriptorBuffer ERROR %ld byte\n",DescriptorLen);
		return FALSE;
	}
	OtgHostInfo.DesCriptorBuffer = DesCriptorBuffer;
#endif
	memset(OtgHostInfo.DesCriptorBuffer, 0, DescriptorLen);

	OTG_DBG("Get All Device ConfigCriptor %ld byte\n",DescriptorLen);
	if(!OTG_HostGetDescriptor(USB_DT_CONFIG, 0, OtgHostInfo.DesCriptorBuffer, DescriptorLen))
	{
		return FALSE;
	}
	
	if(OtgHostInfo.ConfigDesCriptor.bNumInterfaces > MAXINTERFACE)
	{
		return FALSE;
	}
	
	pBuf = OtgHostInfo.DesCriptorBuffer + 9;
	OtgHostInfo.UsbInterfaceNum = 0;
	i = 0;
	OTG_DBG("解析配置描述符\n");
	while(1)
	{
		DescriptorLen = pBuf[i];
		if(DescriptorLen == 0)return FALSE;// bkd add for dead loop
		DescriptorType = pBuf[i+1];
		if((DescriptorType == USB_DT_INTERFACE) && (pBuf[i + 3] == 0))//接口atl setting = 0，表示一个新的接口
		{
			InterfaceNum = pBuf[i+2];
			OtgHostInfo.UsbInterface[InterfaceNum].pData = pBuf + i;
			OtgHostInfo.UsbInterface[InterfaceNum].Length = 0;
			OtgHostInfo.UsbInterfaceNum++;
		}
		OtgHostInfo.UsbInterface[InterfaceNum].Length = OtgHostInfo.UsbInterface[InterfaceNum].Length + DescriptorLen;
		i = i + DescriptorLen;
		if(i > OtgHostInfo.ConfigDesCriptor.wTotalLength)return FALSE;// bkd add for dead loop
		if(i == (OtgHostInfo.ConfigDesCriptor.wTotalLength - 9))
		{
			if(OtgHostInfo.UsbInterfaceNum == OtgHostInfo.ConfigDesCriptor.bNumInterfaces)
			{
				OTG_DBG("解析配置描述符 ok\n");
				if(OTG_HostSetConfiguration(OtgHostInfo.ConfigDesCriptor.bConfigurationValue))
				{
					return TRUE;
				}
				return FALSE;
			}
			OTG_DBG("解析配置描述符 Error 1\n");
			return FALSE;
		}
		else if(i > OtgHostInfo.ConfigDesCriptor.wTotalLength)
		{
			OTG_DBG("解析配置描述符 Error\n");
			return FALSE;			
		}
	}
}


extern bool UDiskInit();
/**
 * @brief  usb host device init
 * @param  NONE
 * @return 每个bit表示一个功能
 */
uint8_t OTG_HostInit(void)
{
	uint8_t FunFlage = 0;
	OTG_HostControlInit();
	if(!OTG_HostEnumDevice())
	{
		OTG_DBG("HostEnumDevice() false!\n");
		return FunFlage;
	}
	
	

	if(UDiskInit() == 1)//枚举MASSSTORAGE接口
	{
		FunFlage = FunFlage | 0x01;
	}
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
	if(UsbHostPlayHardwareInit())
	{
		FunFlage = FunFlage | 0x02;
	}
#endif
//	if(!HostEnumMouseDevice())////枚举HID接口
//	{
//		DBG("mouseStartDevice() false!\n");
//		FunFlage = FunFlage | 0x02;
//	}
	
	return FunFlage;
}

uint8_t OTG_HostUnInit(void)
{
#ifdef FUNC_OS_EN
	if(OtgHostInfo.DesCriptorBuffer)
	{
		OTG_DBG("DesCriptorBuffer\n");
		osPortFree(OtgHostInfo.DesCriptorBuffer);
		OtgHostInfo.DesCriptorBuffer = 0;
	}
#endif
}
