#ifndef __OTG_DEVICE_DESCRIPTOR_H__
#define	__OTG_DEVICE_DESCRIPTOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define AUDIO_INTERFACE_DESC_SIZE                     9
#define USB_AUDIO_DESC_SIZ                            0x09
#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE            0x07

#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05
#define USB_SIZ_DEVICE_DESC                     18
#define USB_SIZ_STRING_LANGID                   4

#define AUDIO_DESCRIPTOR_TYPE                         0x21
#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_PROTOCOL_UNDEFINED                      0x00
#define AUDIO_IP_VERSION_02_00                        0x20
#define AUDIO_STREAMING_GENERAL                       0x01
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02
#define AUDIO_STREAMING_ENCODER                       0x03

/* Audio Descriptor Types */
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_HEADER                          0x01
#define AUDIO_CONTROL_INPUT_TERMINAL                  0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL                 0x03
#define AUDIO_CONTROL_FEATURE_UNIT                    0x06
#define AUDIO_CONTROL_CLOCK_SOURCE                    0x0A

/* Audio Function Category Codes */
#define AUDIO_DESKTOP_SPEAKER                         0x01
#define AUDIO_HOME_THEATER                            0x02
#define AUDIO_MICROPHONE                              0x03
#define AUDIO_HEADSET                                 0x04

#define AUDIO_INPUT_TERMINAL_DESC_SIZE                0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE               0x09
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE           0x07

#define AUDIO_20_CLK_SOURCE_DESC_SIZE                 0x08
#define AUDIO_20_IT_DESC_SIZE                         0x11
#define AUDIO_20_OT_DESC_SIZE                         0x0C
#define AUDIO_20_STREAMING_INTERFACE_DESC_SIZE        0x10

#define CONTROL_BITMAP_NONE                           (0x00)
#define CONTROL_BITMAP_RO                             (0x01)
#define CONTROL_BITMAP_PROG                           (0x03)

#define AUDIO_20_CTL_MUTE(bmaControl)                 (bmaControl)
#define AUDIO_20_CTL_VOLUME(bmaControl)               (bmaControl<<2)

#define AUDIO_20_STANDARD_ENDPOINT_DESC_SIZE          0x07
#define AUDIO_20_STREAMING_ENDPOINT_DESC_SIZE         0x08

#define AUDIO_FORMAT_TYPE_I                           0x01
#define AUDIO_FORMAT_TYPE_II                          0x02
#define AUDIO_FORMAT_TYPE_III                         0x03
#define AUDIO_FORMAT_TYPE_IV                          0x04


#define USB_ENDPOINT_TYPE_ISOCHRONOUS                 0x01
#define USB_ENDPOINT_TYPE_ASYNCHRONOUS				  0x05
#define USB_ENDPOINT_TYPE_ADAPTIVE                    0x09
#define USB_ENDPOINT_TYPE_SYNCHRONOUS                 0x0D
#define AUDIO_ENDPOINT_GENERAL                        0x01


#define USER_CONFIG_DESCRIPTOR_SIZE		(SPEAKER1_ALT2_DESCRIPTOR_SIZE+\
										 SPEAKER_ALT2_DESCRIPTOR_SIZE+\
										 MIC_ALT2_DESCRIPTOR_SIZE+\
										 PACKET1_CHANNELS_NUM+\
										 PACKET_CHANNELS_NUM+\
										 MIC_CHANNELS_NUM+\
										 MIC_FREQ_DESCRIPTOR_SIZE+\
										 SPEAKER_FREQ_DESCRIPTOR_SIZE+\
										 SPEAKER1_FREQ_DESCRIPTOR_SIZE)

#define CHANNEL_CONFIG(chn)				(uint8_t)(chn>1?0x03:0x01)			//chn 1 or 2

#define AUDIO_EP_MAX_SZE(frq,chn,bytes) (uint8_t)(((FRQ_MAX_SZE(frq) * chn * bytes)) & 0xFF), \
                                        (uint8_t)((((FRQ_MAX_SZE(frq) * chn * bytes)) >> 8) & 0xFF)

#define DEVICE_TYPE(type)             	(uint8_t)(type), (uint8_t)((type) >> 8)
#define W_TOTAL_LENGTH(num)             (uint8_t)(num), (uint8_t)((num) >> 8)


#define	LOBYTE(w) ((uint8_t)(w))
#define	HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))

//USB设备描述符
extern uint8_t DeviceDescriptor[18];
//音乐控制命令 HID报告描述符
extern const uint8_t AudioCtrlReportDescriptor[67];
//在线调音HID报告描述符
extern const uint8_t HidDataReportDescriptor[36];
extern const uint8_t * const ConfigDescriptorTab[8];
extern const uint8_t * const InterFaceNumTab[8];
//用于在线调音
#define HID_DATA_FUN_EN	1

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
