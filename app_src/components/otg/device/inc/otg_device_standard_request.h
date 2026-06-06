

#ifndef __OTG_DEVICE_STANDARD_H__
#define	__OTG_DEVICE_STANDARD_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 
#include "type.h"

#define USB_DT_DEVICE					1
#define USB_DT_CONFIG					2
#define USB_DT_STRING					3
#define USB_DT_INTERFACE				4
#define USB_DT_ENDPOINT					5
#define USB_DT_DEVICE_QUALIFIER			6
#define USB_HID_REPORT					0x22

#define USB_REQ_GET_STATUS				0
#define USB_REQ_CLEAR_FEATURE			1
#define USB_REQ_SET_FEATURE				3
#define USB_REQ_SET_ADDRESS				5
#define USB_REQ_GET_DESCRIPTOR			6
#define USB_REQ_SET_DESCRIPTOR			7
#define USB_REQ_GET_CONFIGURATION		8
#define USB_REQ_SET_CONFIGURATION		9
#define USB_REQ_GET_INTERFACE			10
#define USB_REQ_SET_INTERFACE			11
#define USB_REQ_SYNCH_FRAME				12

#define MSC_INTERFACE_NUM				0
#define AUDIO_ATL_INTERFACE_NUM			1
#define AUDIO_SRM_OUT_INTERFACE_NUM		2
#define AUDIO_SRM_IN_INTERFACE_NUM		3
#define HID_CTL_INTERFACE_NUM			4
#define HID_DATA_INTERFACE_NUM			5
#define AUDIO1_ATL_INTERFACE_NUM		6
#define AUDIO1_SRM_OUT_INTERFACE_NUM	7

#define SET_REPORT						0x09
#define GET_REPORT						0x01
#define REPORT_TYPE_INPUT				0x01
#define REPORT_TYPE_OUTPUT				0x02
#define REPORT_TYPE_FEATURE				0x03

void OTG_DeviceModeSel(uint8_t Mode,uint16_t UsbVid,uint16_t UsbPid);
void OTG_DeviceRequestProcess(void);
void OTG_DeviceSendResp(uint16_t Resp, uint8_t n);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__OTG_DEVICE_STANDARD_H__

