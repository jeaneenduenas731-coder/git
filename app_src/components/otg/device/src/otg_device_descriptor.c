#include "type.h"
#include "otg_device_hcd.h"
#include "otg_device_descriptor.h"
#include "otg_device_audio.h"
//USB设备描述符
uint8_t DeviceDescriptor[18] =
{
	0x12, 		// bLength
	0x01, 		// bDescriptorType
	0x10, 0x01,	// bcdUSB
	0x00, 		// bDeviceClass
	0x00, 		// bDeviceSubClass
	0x00, 		// bDeviceProtocol
	0x40,		// bMaxPacketSize0
	0x00, 0x00, // idVendor
	0x00, 0x00,	// idProduct
	0x00, 0x01, // bcdDevice
	0x01, 		// iManufacturer
	0x02,		// iProduct
	0x03,		// iSerialNumber
	0x01,		// bNumConfigurations
};

//音乐控制命令 HID报告描述符
const uint8_t AudioCtrlReportDescriptor[67] =
{
	0x05, 0x0C,        // Usage Page (Consumer)
	0x09, 0x01,        // Usage (Consumer Control)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0x01,        //   Report ID (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x01,        //   Report Count (1)
	0x09, 0xE9,        //   Usage (Volume Increment)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xEA,        //   Usage (Volume Decrement)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xCD,        //   Usage (Play/Pause)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xB5,        //   Usage (Scan Next Track)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xB6,        //   Usage (Scan Previous Track)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xB7,        //   Usage (Stop)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xB3,        //   Usage (Fast Forward)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0xB4,        //   Usage (Rewind)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x0B,        //   Usage Page (Telephony)
	0x09, 0x24,        //   Usage (Redial)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x20,        //   Usage (Hook Switch)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x2F,        //   Usage (Phone Mute)
	0x81, 0x06,        //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x05,        //   Report Count (5)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection
};

#if HID_DATA_FUN_EN
//在线调音HID报告描述符
const uint8_t HidDataReportDescriptor[36] =
{
	0x06, 0x00, 0xFF,
	0x0A, 0xAA, 0x55,
	0xA1, 0x01,
	0x15, 0x00,
	0x26, 0xFF, 0x00,
	0x75, 0x08,

	0x96, 0x00, 0x01,
	0x09, 0x01,
	0x81, 0x02,

	0x96, 0x00, 0x01,
	0x09, 0x01,
	0x91, 0x02,

	0x95, 0x08,
	0x09, 0x01,
	0xB1, 0x02,
	0xC0
};
#endif


#define ConfigDescriptor_Tab(x) 	ConfigDescriptor_##x
#define InterFaceNum_Tab(x) 		InterFaceNum_##x


//声卡 只有音频输出
const uint8_t ConfigDescriptor_Tab(AUDIO_ONLY)[] =
{
#if HID_DATA_FUN_EN
	//一共4个接口
	0x09,0x02,W_TOTAL_LENGTH(0x7A+USER_CONFIG_DESCRIPTOR_SIZE+18),0x04,0x01,0x00,0x80,0x32,
#else
	0x09,0x02,W_TOTAL_LENGTH(0x7A+USER_CONFIG_DESCRIPTOR_SIZE),0x03,0x01,0x00,0x80,0x32,
#endif
	//0 audio control Interface
	0x09,0x04,0x00,0x00,0x00,0x01,0x01,0x00,0x00,

	0x09,0x24,0x01,0x00,0x01,W_TOTAL_LENGTH(0x26+PACKET_CHANNELS_NUM),0x01,0x01,
	0x0C,0x24,0x02,0x01,0x01,0x01,0x00,PACKET_CHANNELS_NUM,CHANNEL_CONFIG(PACKET_CHANNELS_NUM),0x00,0x00,0x00,
#if (PACKET_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x03,DEVICE_TYPE(SPEAKER_TYPE),0x00,0x02,0x00,

	//1 audio stream Interface, speeaker
	0x09,0x04,0x01,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x01,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS,SPEAKER_ALT1_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef SPEAKER_ALT2_EN
	0x09,0x04,0x01,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS,SPEAKER_ALT2_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//2 hid 播放控制
	0x09,0x04,0x02,0x00,0x01,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
#if HID_DATA_FUN_EN
	//3 hid data
	0x09,0x04,0x03,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif
};
const uint8_t InterFaceNum_Tab(AUDIO_ONLY)[] = {0xFF,0x00,0x01,0xFF,0x02,0x03};


////声卡 只有MIC输入
const uint8_t ConfigDescriptor_Tab(MIC_ONLY)[] =
{
#if HID_DATA_FUN_EN
	//一共4个接口
	0x09,0x02,W_TOTAL_LENGTH(0x7A+USER_CONFIG_DESCRIPTOR_SIZE+18),0x04,0x01,0x00,0x80,0x32,
#else
	0x09,0x02,W_TOTAL_LENGTH(0x7A+USER_CONFIG_DESCRIPTOR_SIZE),0x03,0x01,0x00,0x80,0x32,
#endif
	//0 audio control Interface
	0x09,0x04,0x00,0x00,0x00,0x01,0x01,0x00,0x00,
	0x09,0x24,0x01,0x00,0x01,W_TOTAL_LENGTH(0x26+MIC_CHANNELS_NUM),0x01,0x01,
	0x0C,0x24,0x02,0x04,DEVICE_TYPE(MIC_TYPE),0x00,MIC_CHANNELS_NUM,CHANNEL_CONFIG(MIC_CHANNELS_NUM),0x00,0x00,0x00,
#if (MIC_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x06,0x01,0x01,0x00,0x05,0x00,

	//1 audio stream Interface, microphone
	0x09,0x04,0x01,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x01,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x06,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT1_BITS,MIC_ALT1_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef MIC_ALT2_EN
	0x09,0x04,0x01,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x06,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT2_BITS,MIC_ALT2_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//2 hid 播放控制
	0x09,0x04,0x02,0x00,0x01,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,

	//3 调音
#if HID_DATA_FUN_EN
	0x09,0x04,0x03,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif
};
const uint8_t InterFaceNum_Tab(MIC_ONLY)[] = {0xFF,0x00,0xFF,0x01,0x02,0x03};

#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
//声卡 MIC输入和AUDIO输出
const uint8_t ConfigDescriptor_Tab(AUDIO_MIC)[] =
{
#if HID_DATA_FUN_EN
	//一共4个接口
	0x09,0x02,W_TOTAL_LENGTH(0xC1+USER_CONFIG_DESCRIPTOR_SIZE+18),0x05,0x01,0x00,0x80,0x32,
#else
	0x09,0x02,W_TOTAL_LENGTH(0xC1+USER_CONFIG_DESCRIPTOR_SIZE),0x04,0x01,0x00,0x80,0x32,
#endif
	//0 audio control Interface
	0x09,0x04,0x00,0x00,0x00,0x01,0x01,0x00,0x00,
	0x0A,0x24,0x01,0x00,0x01,W_TOTAL_LENGTH(0x44+PACKET_CHANNELS_NUM+MIC_CHANNELS_NUM),0x02,0x01,0x02,
	0x0C,0x24,0x02,0x01,0x01,0x01,0x00,PACKET_CHANNELS_NUM,CHANNEL_CONFIG(PACKET_CHANNELS_NUM),0x00,0x00,0x00,
#if (PACKET_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x03,DEVICE_TYPE(SPEAKER_TYPE),0x00,0x02,0x00,
	0x0C,0x24,0x02,0x04,DEVICE_TYPE(MIC_TYPE),0x00,MIC_CHANNELS_NUM,CHANNEL_CONFIG(MIC_CHANNELS_NUM),0x00,0x00,0x00,
#if (MIC_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x06,0x01,0x01,0x00,0x05,0x00,
	//1 audio stream Interface, speeaker
	0x09,0x04,0x01,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x01,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS,SPEAKER_ALT1_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef SPEAKER_ALT2_EN
	0x09,0x04,0x01,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS,SPEAKER_ALT2_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//2 audio stream Interface, microphone
	0x09,0x04,0x02,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x02,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x06,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT1_BITS,MIC_ALT1_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef MIC_ALT2_EN
	0x09,0x04,0x02,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x06,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT2_BITS,MIC_ALT2_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//3 hid 播放控制
	0x09,0x04,0x03,0x00,0x01,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
#if HID_DATA_FUN_EN
	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif
};
#elif (USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
//声卡 MIC输入和AUDIO输出
const uint8_t ConfigDescriptor_Tab(AUDIO_MIC)[] =
{
	/* Configuration 1 */
	0x09,                                 /* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
#if HID_DATA_FUN_EN
	W_TOTAL_LENGTH(0x108+(PACKET_CHANNELS_NUM*4)+(MIC_CHANNELS_NUM*4)+SPEAKER_ALT2_DESCRIPTOR_SIZE+MIC_ALT2_DESCRIPTOR_SIZE+18),
	0x05,                                 /* bNumInterfaces */
#else
	W_TOTAL_LENGTH(0x108+(PACKET_CHANNELS_NUM*4)+(MIC_CHANNELS_NUM*4)+SPEAKER_ALT2_DESCRIPTOR_SIZE+MIC_ALT2_DESCRIPTOR_SIZE),
	0x04,                                 /* bNumInterfaces */
#endif
	0x01,                                 /* bConfigurationValue */
	0x00,                                 /* iConfiguration */
	0xC0,                                 /* bmAttributes BUS Powered*/
	0x32,                                 /* bMaxPower = 100 mA*/
	/* 09 byte*/

	0x08,
	0x0B,
	0x00,
	0x03,
	0x01,
	0x00,
	0x20,
	0x00,
	/* 08 byte */

	/* Standard AC Interface Descriptor */
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x00,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* Class-specific AC Interface Descriptor */
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
	0x00,                                 /* bcdADC */
	0x02,                                 /* 2.00 */
	AUDIO_HEADSET,                        /* bCategory */
	(103+(PACKET_CHANNELS_NUM*4)+(MIC_CHANNELS_NUM*4)), //119                                /* wTotalLength */
	0x00,
	0x00,                                 /* bmControls */
	/* 09 byte*/

	/* USB HeadSet Clock Source Descriptor */
	AUDIO_20_CLK_SOURCE_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_CLOCK_SOURCE,           /* bDescriptorSubtype */
	AUDIO_CLK_ID,                         /* bClockID */
	0x03,                                 /* bmAttributes */
	0x07,                                 /* bmControls TODO */
	0x00,                                 /* bAssocTerminal */
	0x00,                                 /* iClockSource */
	/* 08 byte*/

	/* USB HeadSet Clock Source Descriptor */
	AUDIO_20_CLK_SOURCE_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_CLOCK_SOURCE,           /* bDescriptorSubtype */
	AUDIO_MIC_CLK_ID,                     /* bClockID */
	0x03,                                 /* bmAttributes */
	0x07,                                 /* bmControls TODO */
	0x00,                      			  /* bAssocTerminal */
	0x00,                                 /* iClockSource */
	/* 08 byte*/

	/* USB HeadSet Input Terminal Descriptor */
	AUDIO_20_IT_DESC_SIZE,                /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
	AUDIO_IT_ID,                          /* bTerminalID */
	0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0201 */
	0x01,
	0x00,                                 /* bAssocTerminal */
	AUDIO_CLK_ID,                         /* bCSourceID*/
	PACKET_CHANNELS_NUM,                  /* bNrChannels */
	CHANNEL_CONFIG(PACKET_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	0x00,                                 /* bmControls */
	0x00,
	0x00,                                 /* iTerminal */
	/* 17 byte*/
#if (PACKET_CHANNELS_NUM == 1)
	/* USB HeadSet Audio Feature Unit Descriptor */
	0x0E,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
	AUDIO_FU_ID,                          /* bUnitID */
	AUDIO_IT_ID,                          /* bSourceID */
	AUDIO_20_CTL_MUTE(CONTROL_BITMAP_PROG),/* bmaControls(0) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(1) */
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iFeature */
	/* 18 byte*/
#else
	/* USB HeadSet Audio Feature Unit Descriptor */
	0x12,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
	AUDIO_FU_ID,                          /* bUnitID */
	AUDIO_IT_ID,                          /* bSourceID */
	AUDIO_20_CTL_MUTE(CONTROL_BITMAP_PROG),/* bmaControls(0) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(1) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(2) */
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iFeature */
	/* 18 byte*/
#endif
	/*USB HeadSet Output Terminal Descriptor */
	AUDIO_20_OT_DESC_SIZE,      			/* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
	AUDIO_OT_ID,                          /* bTerminalID */
	0x02,                                 /* wTerminalType  0x0301: Speaker, 0x0302: Headphone*/
	0x03,
	0x00,                                 /* bAssocTerminal */
	AUDIO_FU_ID,                          /* bSourceID */
	AUDIO_CLK_ID,                         /* bCSourceID */
	0x00,                                 /* bmControls */
	0x00,
	0x00,                                 /* iTerminal */
	/* 12 byte*/

 /* USB HeadSet Input Terminal Descriptor */
	AUDIO_20_IT_DESC_SIZE,                /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
	AUDIO_MIC_IT_ID,                      /* bTerminalID */
	0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0201 */
	0x02,								  //mic
	0x00,                                 /* bAssocTerminal */
	AUDIO_MIC_CLK_ID,                     /* bCSourceID*/
	MIC_CHANNELS_NUM,                     /* bNrChannels */
	CHANNEL_CONFIG(MIC_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	0x00,                                 /* bmControls */
	0x00,
	0x00,                                 /* iTerminal */
	/* 17 byte*/

#if (MIC_CHANNELS_NUM == 1)
	/* USB HeadSet Audio Feature Unit Descriptor */
	0x0E,		                                /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      		/* bDescriptorType */
	AUDIO_CONTROL_FEATURE_UNIT,           		/* bDescriptorSubtype */
	AUDIO_MIC_FU_ID,                          	/* bUnitID */
	AUDIO_MIC_IT_ID,                          	/* bSourceID */
	AUDIO_20_CTL_MUTE(CONTROL_BITMAP_PROG),		/* bmaControls(0) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(1) */
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iFeature */
	/* 18 byte*/
#else
	/* USB HeadSet Audio Feature Unit Descriptor */
	0x12,                                		/* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      		/* bDescriptorType */
	AUDIO_CONTROL_FEATURE_UNIT,           		/* bDescriptorSubtype */
	AUDIO_MIC_FU_ID,                          	/* bUnitID */
	AUDIO_MIC_IT_ID,                          	/* bSourceID */
	AUDIO_20_CTL_MUTE(CONTROL_BITMAP_PROG),		/* bmaControls(0) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(1) */
	0x00,
	0x00,
	0x00,
	AUDIO_20_CTL_VOLUME(CONTROL_BITMAP_PROG),/* bmaControls(2) */
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iFeature */
	/* 18 byte*/
#endif
	/*USB HeadSet Output Terminal Descriptor */
	AUDIO_20_OT_DESC_SIZE,      		  /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
	AUDIO_MIC_OT_ID,                      /* bTerminalID */
	0x01,                                 /* wTerminalType  USB_STREAMING*/
	0x01,
	0x00,                                 /* bAssocTerminal */
	AUDIO_MIC_FU_ID,                      /* bSourceID */
	AUDIO_MIC_CLK_ID,                     /* bCSourceID */
	0x00,                                 /* bmControls */
	0x00,
	0x00,                                 /* iTerminal */
	/* 12 byte*/

	/* USB HeadSet Standard AS Interface Descriptor - Audio Streaming Zero Bandwidth */
	/* Interface 1, Alternate Setting 0                                              */
	AUDIO_INTERFACE_DESC_SIZE,  		  /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x01,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
	/* Interface 1, Alternate Setting 1                                           */
	AUDIO_INTERFACE_DESC_SIZE,  		  /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x01,                                 /* bInterfaceNumber */
	0x01,                                 /* bAlternateSetting */
	0x01,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB HeadSet Audio Streaming Interface Descriptor */
	AUDIO_20_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
	AUDIO_IT_ID,                          /* 0x01: bTerminalLink */
	0x00,                                 /* bmControls */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	0x01,                                 /* bmFormats PCM */
	0x00,
	0x00,
	0x00,
	PACKET_CHANNELS_NUM,                  /* bNrChannels */
	CHANNEL_CONFIG(PACKET_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	/* 16 byte*/

	/* USB Speaker Audio Type I Format Interface Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	SPEAKER_ALT1_BITS,                    /* bSubslotSize */
	SPEAKER_ALT1_BITS*8,                  /* bBitResolution */
	/* 6 byte*/

	/* Endpoint 1 - Standard Descriptor */
	AUDIO_20_STANDARD_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
	DEVICE_ISO_OUT_EP,                    /* bEndpointAddress 3 out endpoint for Audio */
	USB_ENDPOINT_TYPE_SYNCHRONOUS,           /* bmAttributes */
	AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,SPEAKER_ALT1_BITS,PACKET_CHANNELS_NUM),    /* XXXX wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
	0x01,                                 /* bInterval */
	/* 07 byte*/

	/* Endpoint - Audio Streaming Descriptor*/
	AUDIO_20_STREAMING_ENDPOINT_DESC_SIZE,/* bLength */
	AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
	AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
	0x00,                                 /* bmAttributes */
	0x00,                                 /* bmControls */
	0x00,                                 /* bLockDelayUnits */
	0x00,                                 /* wLockDelay */
	0x00,
	/* 08 byte*/
#ifdef SPEAKER_ALT2_EN
	/* Interface 1, Alternate Setting 2                                           */
	AUDIO_INTERFACE_DESC_SIZE,  		  /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x01,                                 /* bInterfaceNumber */
	0x02,                                 /* bAlternateSetting */
	0x01,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB HeadSet Audio Streaming Interface Descriptor */
	AUDIO_20_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
	AUDIO_IT_ID,                          /* 0x01: bTerminalLink */
	0x00,                                 /* bmControls */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	0x01,                                 /* bmFormats PCM */
	0x00,
	0x00,
	0x00,
	PACKET_CHANNELS_NUM,                  /* bNrChannels */
	CHANNEL_CONFIG(PACKET_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	/* 16 byte*/

	/* USB Speaker Audio Type I Format Interface Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	SPEAKER_ALT2_BITS,                    /* bSubslotSize */
	SPEAKER_ALT2_BITS*8,                  /* bBitResolution */
	/* 6 byte*/

	/* Endpoint 1 - Standard Descriptor */
	AUDIO_20_STANDARD_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
	DEVICE_ISO_OUT_EP,                    /* bEndpointAddress 3 out endpoint for Audio */
	USB_ENDPOINT_TYPE_SYNCHRONOUS,           /* bmAttributes */
	AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,SPEAKER_ALT2_BITS,PACKET_CHANNELS_NUM),    /* XXXX wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
	0x01,                                 /* bInterval */
	/* 07 byte*/

	/* Endpoint - Audio Streaming Descriptor*/
	AUDIO_20_STREAMING_ENDPOINT_DESC_SIZE,/* bLength */
	AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
	AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
	0x00,                                 /* bmAttributes */
	0x00,                                 /* bmControls */
	0x00,                                 /* bLockDelayUnits */
	0x00,                                 /* wLockDelay */
	0x00,
	/* 08 byte*/
#endif

	/* USB HeadSet Standard AS Interface Descriptor - Audio Streaming Zero Bandwidth */
	/* Interface 2, Alternate Setting 0                                              */
	AUDIO_INTERFACE_DESC_SIZE,  			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x02,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB MIC Standard AS Interface Descriptor - Audio Streaming Operational */
	/* Interface 2, Alternate Setting 1                                           */
	AUDIO_INTERFACE_DESC_SIZE,  			/* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x02,                                 /* bInterfaceNumber */
	0x01,                                 /* bAlternateSetting */
	0x01,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB HeadSet Audio Streaming Interface Descriptor */
	AUDIO_20_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
	AUDIO_MIC_OT_ID,                      /* 0x01: bTerminalLink */
	0x00,                                 /* bmControls */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	0x01,                                 /* bmFormats PCM */
	0x00,
	0x00,
	0x00,
	MIC_CHANNELS_NUM,                     /* bNrChannels */
	CHANNEL_CONFIG(MIC_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	/* 16 byte*/

	/* USB MIC Audio Type I Format Interface Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	MIC_ALT1_BITS,                        /* bSubslotSize */
	MIC_ALT1_BITS*8,                      /* bBitResolution */
	/* 6 byte*/

	/* Endpoint 1 - Standard Descriptor */
	AUDIO_20_STANDARD_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
	DEVICE_ISO_IN_EP,                     /* bEndpointAddress 3 out endpoint for Audio */
	USB_ENDPOINT_TYPE_SYNCHRONOUS,           /* bmAttributes */
	AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT1_BITS),//AUDIO_MIC_SZE(USBD_AUDIO_MIC_FREQ),    /* XXXX wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
	0x01,                                 /* bInterval */
	/* 07 byte*/

	/* Endpoint - Audio Streaming Descriptor*/
	AUDIO_20_STREAMING_ENDPOINT_DESC_SIZE,/* bLength */
	AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
	AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
	0x00,                                 /* bmAttributes */
	0x00,                                 /* bmControls */
	0x00,                                 /* bLockDelayUnits */
	0x00,                                 /* wLockDelay */
	0x00,
	/* 08 byte*/
#ifdef MIC_ALT2_EN
	/* USB MIC Standard AS Interface Descriptor - Audio Streaming Operational */
	/* Interface 2, Alternate Setting 2                                           */
	AUDIO_INTERFACE_DESC_SIZE,  		  /* bLength 0x09 */
	USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
	0x02,                                 /* bInterfaceNumber */
	0x02,                                 /* bAlternateSetting */
	0x01,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
	AUDIO_IP_VERSION_02_00,               /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB HeadSet Audio Streaming Interface Descriptor */
	AUDIO_20_STREAMING_INTERFACE_DESC_SIZE,  /* bLength 0x10*/
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
	AUDIO_MIC_OT_ID,                      /* 0x01: bTerminalLink */
	0x05,                                 /* bmControls */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	0x01,                                 /* bmFormats PCM */
	0x00,
	0x00,
	0x00,
	MIC_CHANNELS_NUM,                     /* bNrChannels */
	CHANNEL_CONFIG(MIC_CHANNELS_NUM),
	0x00,
	0x00,
	0x00,
	0x00,                                 /* iChannelNames */
	/* 16 byte*/

	/* USB MIC Audio Type I Format Interface Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
	AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
	MIC_ALT2_BITS,                        /* bSubslotSize */
	MIC_ALT2_BITS*8,                      /* bBitResolution */
	/* 6 byte*/

	/* Endpoint 1 - Standard Descriptor */
	AUDIO_20_STANDARD_ENDPOINT_DESC_SIZE, /* bLength 0x07*/
	USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
	DEVICE_ISO_IN_EP,                     /* bEndpointAddress 3 out endpoint for Audio */
	USB_ENDPOINT_TYPE_SYNCHRONOUS,        /* bmAttributes */
	AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT2_BITS),//AUDIO_MIC_SZE(USBD_AUDIO_MIC_FREQ),    /* XXXX wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
	0x01,                                 /* bInterval */
	/* 07 byte*/

	/* Endpoint - Audio Streaming Descriptor*/
	AUDIO_20_STREAMING_ENDPOINT_DESC_SIZE,/* bLength 0x08*/
	AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
	AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
	0x00,                                 /* bmAttributes */
	0x00,                                 /* bmControls */
	0x00,                                 /* bLockDelayUnits */
	0x00,                                 /* wLockDelay */
	0x00,
	/* 08 byte*/
#endif
	//3 hid 播放控制
	0x09,0x04,0x03,0x00,0x01,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
//	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
	0x07,0x05,DEVICE_INT_IN_EP,0x03,0x03,0x00,0x01,
#if HID_DATA_FUN_EN
	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif

};
#endif
const uint8_t InterFaceNum_Tab(AUDIO_MIC)[] = {0xFF,0x00,0x01,0x02,0x03,0x04};

const uint8_t ConfigDescriptor_Tab(AUDIO_MIC_AUDIO)[] =
{
#if HID_DATA_FUN_EN
	//一共4个接口
	0x09,0x02,W_TOTAL_LENGTH(0xC8+USER_CONFIG_DESCRIPTOR_SIZE+18+0x58),0x07,0x01,0x00,0x80,0x32,
#else
	0x09,0x02,W_TOTAL_LENGTH(0xC8+USER_CONFIG_DESCRIPTOR_SIZE+0x58),0x06,0x01,0x00,0x80,0x32,
#endif
	//0 audio control Interface
	0x09,0x04,0x00,0x00,0x00,0x01,0x01,0x00,0x00,
	0x0A,0x24,0x01,0x00,0x01,W_TOTAL_LENGTH(0x4B+PACKET_CHANNELS_NUM+MIC_CHANNELS_NUM),0x02,0x01,0x02,
	0x0C,0x24,0x02,0x01,0x01,0x01,0x00,PACKET_CHANNELS_NUM,CHANNEL_CONFIG(PACKET_CHANNELS_NUM),0x00,0x00,0x00,
#if (PACKET_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x03,DEVICE_TYPE(SPEAKER_TYPE),0x00,0x02,0x00,
	0x0C,0x24,0x02,0x04,DEVICE_TYPE(MIC_TYPE),0x00,MIC_CHANNELS_NUM,CHANNEL_CONFIG(MIC_CHANNELS_NUM),0x00,0x00,0x00,
#if (MIC_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x02,0x00,
#endif
	0x07,0x24,0x05,0x06,0x01,0x05,0x00,
	0x09,0x24,0x03,0x07,0x01,0x01,0x00,0x06,0x00,

	//1 audio stream Interface, speeaker
	0x09,0x04,0x01,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x01,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS,SPEAKER_ALT1_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef SPEAKER_ALT2_EN
	0x09,0x04,0x01,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
	SPEAKER_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS,SPEAKER_ALT2_BITS*8,SPEAKER_FREQ_NUM,
#if (SPEAKER_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_FREQ5),
#endif
#if (SPEAKER_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_FREQ4),
#endif
#if (SPEAKER_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_FREQ3),
#endif
#if (SPEAKER_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_FREQ2),
#endif
#if (SPEAKER_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_FREQ1),
#endif
#if (SPEAKER_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_FREQ,PACKET_CHANNELS_NUM,SPEAKER_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//2 audio stream Interface, microphone
	0x09,0x04,0x02,0x00,0x00,0x01,0x02,0x00,0x00,

	0x09,0x04,0x02,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x07,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT1_BITS,MIC_ALT1_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT1_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#ifdef MIC_ALT2_EN
	0x09,0x04,0x02,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x07,0x01,0x01,0x00,
	MIC_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_ALT2_BITS,MIC_ALT2_BITS*8,MIC_FREQ_NUM,
#if (MIC_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ5),
#endif
#if (MIC_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ4),
#endif
#if (MIC_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ3),
#endif
#if (MIC_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ2),
#endif
#if (MIC_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
#endif
#if (MIC_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),
#endif
	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_EP_MAX_SZE(USBD_AUDIO_MIC_FREQ,MIC_CHANNELS_NUM,MIC_ALT2_BITS), 0x01, 0x00, 0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
	//3 hid 播放控制
	0x09,0x04,0x03,0x00,0x01,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
#if HID_DATA_FUN_EN
	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif

	//声卡2  len 0x69 +31 + SPEAKER1_CHANNELS_NUM
	0x09,0x04,0x05,0x00,0x00,0x01,0x01,0x00,0x04,
	0x09,0x24,0x01,0x00,0x01,W_TOTAL_LENGTH(0x26+PACKET1_CHANNELS_NUM),0x01,0x06,
	0x0C,0x24,0x02,0x09,0x01,0x01,0x00,PACKET1_CHANNELS_NUM,CHANNEL_CONFIG(PACKET1_CHANNELS_NUM),0x00,0x00,0x00,
#if(SPEAKER1_CHANNELS_NUM == 1)
	0x09,0x24,0x06,0x0A,0x09,0x01,0x01,0x02,0x00,
#else
	0x0A,0x24,0x06,0x0A,0x09,0x01,0x01,0x02,0x02,0x00,
#endif
	0x09,0x24,0x03,0x0B,DEVICE_TYPE(SPEAKER1_TYPE),0x00,0x0A,0x00,

	//audio stream Interface
	0x09,0x04,0x06,0x00,0x00,0x01,0x02,0x00,0x00,
	0x09,0x04,0x06,0x01,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x09,0x01,0x01,0x00,
	SPEAKER1_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET1_CHANNELS_NUM,SPEAKER1_ALT1_BITS,SPEAKER1_ALT1_BITS*8,SPEAKER1_FREQ_NUM,
#if (SPEAKER1_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ5),
#endif
#if (SPEAKER1_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ4),
#endif
#if (SPEAKER1_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ3),
#endif
#if (SPEAKER1_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ2),
#endif
#if (SPEAKER1_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ1),
#endif
#if (SPEAKER1_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ),
#endif
	0x09,0x05,DEVICE_ISO_OUT_EP1,0x09,AUDIO_EP_MAX_SZE(USBD_AUDIO1_FREQ,PACKET1_CHANNELS_NUM,SPEAKER1_ALT1_BITS),0x01,0x00,0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,

#ifdef SPEAKER1_ALT2_EN
	0x09,0x04,0x06,0x02,0x01,0x01,0x02,0x00,0x00,
	0x07,0x24,0x01,0x09,0x01,0x01,0x00,
	SPEAKER1_FREQ_DESCRIPTOR_SIZE,0x24,0x02,0x01,PACKET1_CHANNELS_NUM,SPEAKER1_ALT2_BITS,SPEAKER1_ALT2_BITS*8,SPEAKER1_FREQ_NUM,
#if (SPEAKER1_FREQ_NUM >= 6)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ5),
#endif
#if (SPEAKER1_FREQ_NUM >= 5)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ4),
#endif
#if (SPEAKER1_FREQ_NUM >= 4)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ3),
#endif
#if (SPEAKER1_FREQ_NUM >= 3)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ2),
#endif
#if (SPEAKER1_FREQ_NUM >= 2)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ1),
#endif
#if (SPEAKER1_FREQ_NUM >= 1)
	SAMPLE_FREQ(USBD_AUDIO1_FREQ),
#endif
	0x09,0x05,DEVICE_ISO_OUT_EP1,0x09,AUDIO_EP_MAX_SZE(USBD_AUDIO1_FREQ,PACKET1_CHANNELS_NUM,SPEAKER1_ALT2_BITS),0x01,0x00,0x00,
	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
#endif
};
const uint8_t InterFaceNum_Tab(AUDIO_MIC_AUDIO)[] = {0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06};

const uint8_t ConfigDescriptor_Tab(READER)[] =
{

#if HID_DATA_FUN_EN
	//一共4个接口
	0x09,0x02,W_TOTAL_LENGTH(0x32),0x02,0x01,0x00,0x80,0x32,
#else
	//一共1个接口
	0x09,0x02,W_TOTAL_LENGTH(0x20),0x01,0x01,0x00,0x80,0x32,
#endif
	//mass-storage Interface
	0x09,0x04,0x00,0x00,0x02,0x08,0x06,0x50,0x00,
	0x07,0x05,DEVICE_BULK_IN_EP,0x02, DEVICE_FS_BULK_IN_MPS, 0x00,0x01,
	0x07,0x05,DEVICE_BULK_OUT_EP,0x02,DEVICE_FS_BULK_OUT_MPS,0x00,0x01,

#if HID_DATA_FUN_EN
	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
#endif
};
const uint8_t InterFaceNum_Tab(READER)[] = {0x00,0xFF,0xFF,0xFF,0xFF,0x04};


////声卡 只有音频输出
//const uint8_t ConfigDescriptor_Tab(AUDIO_READER)[] =
//{
//#if HID_DATA_FUN_EN
//	//一共4个接口
//	0x09,0x02,(156+23),0x00,0x05,0x01,0x00,0x80,0x32,
//#else
//	0x09,0x02,(156-18+23),0x00,0x04,0x01,0x00,0x80,0x32,
//#endif
//
//	//0 mass-storage Interface
//	0x09,0x04,0x00,0x00,0x02,0x08,0x06,0x50,0x00,
//	0x07,0x05,DEVICE_BULK_IN_EP,0x02, DEVICE_FS_BULK_IN_MPS, 0x00,0x01,
//	0x07,0x05,DEVICE_BULK_OUT_EP,0x02,DEVICE_FS_BULK_OUT_MPS,0x00,0x01,
//
//	//1 audio control Interface
//	0x09,0x04,0x01,0x00,0x00,0x01,0x01,0x00,0x00,
//
//	0x09,0x24,0x01,0x00,0x01,/*0x4F*/40,0x00,0x01,0x02,
//	0x0C,0x24,0x02,0x01,0x01,0x01,0x00,0x02,0x03,0x00,0x00,0x00,
//	0x0A,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x02,0x00,
//	0x09,0x24,0x03,0x03,0x01,0x03,0x00,0x02,0x00,
//
//	//2 audio stream Interface, speeaker
//	0x09,0x04,0x02,0x00,0x00,0x01,0x02,0x00,0x00,
//	0x09,0x04,0x02,0x01,0x01,0x01,0x02,0x00,0x00,
//	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
//	0x0E,0x24,0x02,0x01,PACKET_CHANNELS_NUM,PACKET_BYTES,PACKET_SAMPLE_BITS,0x02,SAMPLE_FREQ(USBD_AUDIO_FREQ),SAMPLE_FREQ(USBD_AUDIO_FREQ1),
//	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_PACKET_SZE(USBD_AUDIO_FREQ), 0x01, 0x00, 0x00,
//	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
//
//	//3 hid 播放控制
//	0x09,0x04,0x03,0x00,0x01,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
//	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
//
//#if HID_DATA_FUN_EN
//	//4 hid data
//	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
//#endif
//};
//const uint8_t InterFaceNum_Tab(AUDIO_READER)[] = {0x00,0x01,0x02,0xFF,0x03,0x04};
//
//////声卡 只有MIC输入
//const uint8_t ConfigDescriptor_Tab(MIC_READER)[] =
//{
//#if HID_DATA_FUN_EN
//	//一共4个接口
//	0x09,0x02,(163+23),0x00,0x05,0x01,0x00,0x80,0x32,
//#else
//	0x09,0x02,(163-18+23),0x00,0x04,0x01,0x00,0x80,0x32,
//#endif
//	//0 mass-storage Interface
//	0x09,0x04,0x00,0x00,0x02,0x08,0x06,0x50,0x00,
//	0x07,0x05,DEVICE_BULK_IN_EP,0x02, DEVICE_FS_BULK_IN_MPS, 0x00,0x01,
//	0x07,0x05,DEVICE_BULK_OUT_EP,0x02,DEVICE_FS_BULK_OUT_MPS,0x00,0x01,
//
//	//1 audio control Interface
//	0x09,0x04,0x01,0x00,0x00,0x01,0x01,0x00,0x00,
//	0x09,0x24,0x01,0x00,0x01,47,0x00,0x01,0x02,
//	0x0C,0x24,0x02,0x04,0x01,0x02,0x00,0x02,0x03,0x00,0x00,0x00,
//	0x0A,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x02,0x00,
//	0x07,0x24,0x05,0x06,0x01,0x05,0x00,
//	0x09,0x24,0x03,0x07,0x01,0x01,0x00,0x06,0x00,
//
//	//2 audio stream Interface, microphone
//	0x09,0x04,0x02,0x00,0x00,0x01,0x02,0x00,0x00,
//	0x09,0x04,0x02,0x01,0x01,0x01,0x02,0x00,0x00,
//	0x07,0x24,0x01,0x07,0x01,0x01,0x00,
//	0x0E,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_BYTES,MIC_SAMPLE_BITS,0x02,SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
//	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_MIC_SZE(USBD_AUDIO_MIC_FREQ), 0x01, 0x00, 0x00,
//	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
//
//	//3 hid 播放控制
//	0x09,0x04,0x03,0x00,0x01,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
//	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
//#if HID_DATA_FUN_EN
//	0x09,0x04,0x04,0x00,0x00,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
//#endif
//};
//
//const uint8_t InterFaceNum_Tab(MIC_READER)[] = {0x00,0x01,0xFF,0x02,0x03,0x04};
//
//
//
////声卡 MIC输入和AUDIO输出
//const uint8_t ConfigDescriptor_Tab(AUDIO_MIC_READER)[] =
//{
//#if HID_DATA_FUN_EN
//	//一共4个接口
//	0x09,0x02,17,0x01,0x06,0x01,0x00,0x80,0x32,
//#else
//	0x09,0x02,255,0x00,0x05,0x01,0x00,0x80,0x32,
//#endif
//	//0 mass-storage Interface
//	0x09,0x04,0x00,0x00,0x02,0x08,0x06,0x50,0x00,
//	0x07,0x05,DEVICE_BULK_IN_EP,0x02, DEVICE_FS_BULK_IN_MPS, 0x00,0x01,
//	0x07,0x05,DEVICE_BULK_OUT_EP,0x02,DEVICE_FS_BULK_OUT_MPS,0x00,0x01,
//
//	//0 audio control Interface
//	0x09,0x04,0x01,0x00,0x00,0x01,0x01,0x00,0x00,
//	0x0A,0x24,0x01,0x00,0x01,0x4F,0x00,0x02,0x02,0x03,
//	0x0C,0x24,0x02,0x01,0x01,0x01,0x00,0x02,0x03,0x00,0x00,0x00,
//	0x0A,0x24,0x06,0x02,0x01,0x01,0x01,0x02,0x02,0x00,
//	0x09,0x24,0x03,0x03,0x01,0x03,0x00,0x02,0x00,
//	0x0C,0x24,0x02,0x04,0x01,0x02,0x00,0x02,0x03,0x00,0x00,0x00,
//	0x0A,0x24,0x06,0x05,0x04,0x01,0x01,0x02,0x02,0x00,
//	0x07,0x24,0x05,0x06,0x01,0x05,0x00,
//	0x09,0x24,0x03,0x07,0x01,0x01,0x00,0x06,0x00,
//
//	//1 audio stream Interface, speeaker
//	0x09,0x04,0x02,0x00,0x00,0x01,0x02,0x00,0x00,
//	0x09,0x04,0x02,0x01,0x01,0x01,0x02,0x00,0x00,
//	0x07,0x24,0x01,0x01,0x01,0x01,0x00,
//	0x0E,0x24,0x02,0x01,PACKET_CHANNELS_NUM,PACKET_BYTES,PACKET_SAMPLE_BITS,0x02,SAMPLE_FREQ(USBD_AUDIO_FREQ),SAMPLE_FREQ(USBD_AUDIO_FREQ1),
//	0x09, 0x05, DEVICE_ISO_OUT_EP, 0x09, AUDIO_PACKET_SZE(USBD_AUDIO_FREQ), 0x01, 0x00, 0x00,
//	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
//
//	//2 audio stream Interface, microphone
//	0x09,0x04,0x03,0x00,0x00,0x01,0x02,0x00,0x00,
//	0x09,0x04,0x03,0x01,0x01,0x01,0x02,0x00,0x00,
//	0x07,0x24,0x01,0x07,0x01,0x01,0x00,
//	0x0E,0x24,0x02,0x01,MIC_CHANNELS_NUM,MIC_BYTES,MIC_SAMPLE_BITS,0x02,SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ),SAMPLE_FREQ(USBD_AUDIO_MIC_FREQ1),
//	0x09, 0x05, DEVICE_ISO_IN_EP, 0x09, AUDIO_MIC_SZE(USBD_AUDIO_MIC_FREQ), 0x01, 0x00, 0x00,
//	0x07,0x25,0x01,0x01,0x00,0x00,0x00,
//
//	//3 hid 播放控制
//	0x09,0x04,0x04,0x00,0x01,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(AudioCtrlReportDescriptor),0x00,
//	0x07,0x05,DEVICE_INT_IN_EP,0x03,DEVICE_FS_INT_IN_MPS,0x00,0x01,
//#if HID_DATA_FUN_EN
//	0x09,0x04,0x05,0x00,0x00,0x03,0x00,0x00,0x00,
//	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00,
//#endif
//};
//const uint8_t InterFaceNum_Tab(AUDIO_MIC_READER)[] = {0x00,0x01,0x02,0x03,0x04,0x05};


//HID数据传输
const uint8_t ConfigDescriptor_Tab(HID)[] =
{
	//一共1个接口
	0x09, 0x02, 0x1B, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,

#if HID_DATA_FUN_EN
	//HID自定数据传输接口
	0x09,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
	0x09,0x21,0x01,0x02,0x00,0x01,0x22,sizeof(HidDataReportDescriptor),0x00
#endif
};
const uint8_t InterFaceNum_Tab(HID)[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0x00};

const uint8_t * const ConfigDescriptorTab[8] =
{
	[HID] 				= ConfigDescriptor_Tab(HID),
	[AUDIO_ONLY] 		= ConfigDescriptor_Tab(AUDIO_ONLY),
	[MIC_ONLY] 			= ConfigDescriptor_Tab(MIC_ONLY),
	[AUDIO_MIC] 		= ConfigDescriptor_Tab(AUDIO_MIC),
	[AUDIO_MIC_AUDIO] 	= ConfigDescriptor_Tab(AUDIO_MIC_AUDIO),
	[READER] 			= ConfigDescriptor_Tab(READER),
//	[MIC_READER] 		= ConfigDescriptor_Tab(MIC_READER),
//	[AUDIO_MIC_READER] 	= ConfigDescriptor_Tab(AUDIO_MIC_READER),
};

const uint8_t * const InterFaceNumTab[8] =
{
	[HID] 				= InterFaceNum_Tab(HID),
	[AUDIO_ONLY] 		= InterFaceNum_Tab(AUDIO_ONLY),
	[MIC_ONLY] 			= InterFaceNum_Tab(MIC_ONLY),
	[AUDIO_MIC] 		= InterFaceNum_Tab(AUDIO_MIC),
	[AUDIO_MIC_AUDIO] 	= InterFaceNum_Tab(AUDIO_MIC_AUDIO),
	[READER] 			= InterFaceNum_Tab(READER),
//	[MIC_READER] 		= InterFaceNum_Tab(MIC_READER),
//	[AUDIO_MIC_READER] 	= InterFaceNum_Tab(AUDIO_MIC_READER),

};

