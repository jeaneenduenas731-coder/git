/**
 *****************************************************************************
 * @file     otg_device_audio.h
 * @author   Owen
 * @version  V1.0.0
 * @date     24-June-2015
 * @brief    audio device interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __OTG_DEVICE_AUDIO_H__
#define	__OTG_DEVICE_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
	
#include "type.h"
#include "mvintrinsics.h"
#include "mcu_circular_buf.h"
#ifdef CFG_APP_CONFIG
#include "app_config.h"
#endif

#define DB_TO_USBVAL(db)	((int16_t)(db*256))

#define AUDIO_MAX_VOLUME	DB_TO_USBVAL(0)		//0db
#define AUDIO_MIN_VOLUME	DB_TO_USBVAL(-60)	//-60db
#define AUDIO_RES_VOLUME	0x0080				//0.5db

//AUDIO_UAC_20配置只支持AUDIO_MIC
#define AUDIO_UAC_10		10
#define AUDIO_UAC_20		20
#define USB_AUDIO_PROTOCOL	AUDIO_UAC_10

#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
#define USBPID(x)			(USB_PID_BASE + x)
#elif (USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
#define USBPID(x)			(USB_PID_BASE + 0x1000 + x)
#endif

#define PCM16BIT	2
#define PCM24BIT	3
#define PCM32BIT	4

#ifdef CFG_OTG_MODE_MIC_EN
	#define MIC_ALT2_EN			//麦克风第二种PCM格式使能

	#define MIC_FREQ_NUM								2			//采样率个数  MAX: 6
	#define	USBD_AUDIO_MIC_FREQ							48000		//48000 : bits per seconds 麦克风最大采样率
	#define	USBD_AUDIO_MIC_FREQ1						44100		//44100 : bits per seconds
	#define	USBD_AUDIO_MIC_FREQ2						44100		//44100 : bits per seconds
	#define	USBD_AUDIO_MIC_FREQ3						44100		//44100 : bits per seconds
	#define	USBD_AUDIO_MIC_FREQ4						44100		//44100 : bits per seconds
	#define	USBD_AUDIO_MIC_FREQ5						44100		//44100 : bits per seconds

	#define MIC_CHANNELS_NUM							2			//麦克风声道数  1 or 2
	#define	MIC_ALT1_BITS								PCM16BIT	//PCM16BIT or PCM24BIT
	#ifdef MIC_ALT2_EN
		#define	MIC_ALT2_BITS							PCM24BIT	//PCM16BIT or PCM24BIT
	#endif
	#define MIC_TYPE									0x0402		//Headset
#endif

#ifdef CFG_OTG_MODE_AUDIO_EN
	#define SPEAKER_ALT2_EN		//扬声器第二种PCM格式使能

	#define SPEAKER_FREQ_NUM							2			//采样率个数  MAX: 6
	#define	USBD_AUDIO_FREQ								48000		//48000 : bits per seconds 扬声器最大采样率
	#define	USBD_AUDIO_FREQ1							44100		//44100 : bits per seconds
	#define	USBD_AUDIO_FREQ2							44100		//44100 : bits per seconds
	#define	USBD_AUDIO_FREQ3							44100		//44100 : bits per seconds
	#define	USBD_AUDIO_FREQ4							44100		//44100 : bits per seconds
	#define	USBD_AUDIO_FREQ5							44100		//44100 : bits per seconds

	#define PACKET_CHANNELS_NUM							2			//扬声器声道数  1 or 2
	#define	SPEAKER_ALT1_BITS							PCM16BIT	//PCM16BIT or PCM24BIT
	#ifdef SPEAKER_ALT2_EN
		#define	SPEAKER_ALT2_BITS						PCM24BIT	//PCM16BIT or PCM24BIT
	#endif
	#define SPEAKER_TYPE								0x0402		//Headset
#endif

#ifdef CFG_OTG_MODE_AUDIO1_EN
	#define SPEAKER1_ALT2_EN		//扬声器第二种PCM格式使能

	#define SPEAKER1_FREQ_NUM							2			//采样率个数  MAX: 6
	#define	USBD_AUDIO1_FREQ							48000		//48000 : bits per seconds 扬声器最大采样率
	#define	USBD_AUDIO1_FREQ1							44100		//44100 : bits per seconds
	#define	USBD_AUDIO1_FREQ2							44100		//44100 : bits per seconds
	#define	USBD_AUDIO1_FREQ3							44100		//44100 : bits per seconds
	#define	USBD_AUDIO1_FREQ4							44100		//44100 : bits per seconds
	#define	USBD_AUDIO1_FREQ5							44100		//44100 : bits per seconds

	#define PACKET1_CHANNELS_NUM						2			//扬声器声道数  1 or 2
	#define	SPEAKER1_ALT1_BITS							PCM16BIT	//PCM16BIT or PCM24BIT
	#ifdef SPEAKER1_ALT2_EN
		#define	SPEAKER1_ALT2_BITS						PCM24BIT	//PCM16BIT or PCM24BIT
	#endif
	#define SPEAKER1_TYPE								0x0402		//Headset
#endif

//以下参数不需要修改
#ifdef CFG_OTG_MODE_MIC_EN
#define MIC_FREQ_DESCRIPTOR_SIZE					(0x08+MIC_FREQ_NUM*3)
#else
#define MIC_FREQ_DESCRIPTOR_SIZE					0
#define MIC_FREQ_NUM								0
#define	USBD_AUDIO_MIC_FREQ							0
#define	USBD_AUDIO_MIC_FREQ1						0
#define	USBD_AUDIO_MIC_FREQ2						0
#define	USBD_AUDIO_MIC_FREQ3						0
#define	USBD_AUDIO_MIC_FREQ4						0
#define	USBD_AUDIO_MIC_FREQ5						0
#define MIC_CHANNELS_NUM							0
#define	MIC_ALT1_BITS								0
#define MIC_TYPE									0x0402		//Headset
#endif

#ifdef CFG_OTG_MODE_AUDIO_EN
#define SPEAKER_FREQ_DESCRIPTOR_SIZE				(0x08+SPEAKER_FREQ_NUM*3)
#else
#define SPEAKER_FREQ_DESCRIPTOR_SIZE				0
#define SPEAKER_FREQ_NUM							0
#define	USBD_AUDIO_FREQ								0
#define	USBD_AUDIO_FREQ1							0
#define	USBD_AUDIO_FREQ2							0
#define	USBD_AUDIO_FREQ3							0
#define	USBD_AUDIO_FREQ4							0
#define	USBD_AUDIO_FREQ5							0
#define PACKET_CHANNELS_NUM							0
#define	SPEAKER_ALT1_BITS							0
#define SPEAKER_TYPE								0x0402		//Headset
#endif

#ifdef CFG_OTG_MODE_AUDIO1_EN
#define SPEAKER1_FREQ_DESCRIPTOR_SIZE				(0x08+SPEAKER1_FREQ_NUM*3)
#else
#define SPEAKER1_FREQ_DESCRIPTOR_SIZE				0
#define SPEAKER1_FREQ_NUM							0
#define	USBD_AUDIO1_FREQ							0
#define	USBD_AUDIO1_FREQ1							0
#define	USBD_AUDIO1_FREQ2							0
#define	USBD_AUDIO1_FREQ3							0
#define	USBD_AUDIO1_FREQ4							0
#define	USBD_AUDIO1_FREQ5							0
#define PACKET1_CHANNELS_NUM						0
#define	SPEAKER1_ALT1_BITS							0
#define SPEAKER1_TYPE								0x0402		//Headset
#endif

#ifdef MIC_ALT2_EN
	#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
	#define	MIC_ALT2_DESCRIPTOR_SIZE				(0x28+MIC_FREQ_NUM*3)		//描述符长度
	#elif (USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
	#define	MIC_ALT2_DESCRIPTOR_SIZE				0x2E		//描述符长度
	#endif
#else
	#define	MIC_ALT2_BITS							0
	#define	MIC_ALT2_DESCRIPTOR_SIZE				0
#endif

#ifdef SPEAKER_ALT2_EN
	#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
	#define	SPEAKER_ALT2_DESCRIPTOR_SIZE			(0x28+SPEAKER_FREQ_NUM*3)		//描述符长度
	#elif (USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
	#define	SPEAKER_ALT2_DESCRIPTOR_SIZE			0x2E		//描述符长度
	#endif
#else
	#define	SPEAKER_ALT2_BITS						0
	#define	SPEAKER_ALT2_DESCRIPTOR_SIZE			0
#endif

#ifdef SPEAKER1_ALT2_EN
	#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
	#define	SPEAKER1_ALT2_DESCRIPTOR_SIZE			(0x28+SPEAKER1_FREQ_NUM*3)		//描述符长度
	#elif (USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
	#define	SPEAKER1_ALT2_DESCRIPTOR_SIZE			0x2E		//描述符长度
	#endif
#else
	#define	SPEAKER1_ALT2_BITS						0
	#define	SPEAKER1_ALT2_DESCRIPTOR_SIZE			0
#endif

#if (USBD_AUDIO_MIC_FREQ <= 48000)
#define	DEVICE_FS_ISO_IN_MPS		(48000*MIC_CHANNELS_NUM*MAX(MIC_ALT1_BITS,MIC_ALT2_BITS)/1000)
#else
#define	DEVICE_FS_ISO_IN_MPS		(USBD_AUDIO_MIC_FREQ*MIC_CHANNELS_NUM*MAX(MIC_ALT1_BITS,MIC_ALT2_BITS)/1000)
#endif
#if (USBD_AUDIO_FREQ <= 48000)
#define	DEVICE_FS_ISO_OUT_MPS		(48000*PACKET_CHANNELS_NUM*MAX(SPEAKER_ALT1_BITS,SPEAKER_ALT2_BITS)/1000)
#else
#define	DEVICE_FS_ISO_OUT_MPS		(USBD_AUDIO_FREQ*PACKET_CHANNELS_NUM*MAX(SPEAKER_ALT1_BITS,SPEAKER_ALT2_BITS)/1000)
#endif
#if (USBD_AUDIO1_FREQ <= 48000)
#define	DEVICE_FS_ISO_OUT1_MPS		(48000*PACKET1_CHANNELS_NUM*MAX(SPEAKER1_ALT1_BITS,SPEAKER1_ALT2_BITS)/1000)
#else
#define	DEVICE_FS_ISO_OUT1_MPS		(USBD_AUDIO1_FREQ*PACKET1_CHANNELS_NUM*MAX(SPEAKER1_ALT1_BITS,SPEAKER1_ALT2_BITS)/1000)
#endif


#if (DEVICE_FS_ISO_IN_MPS + DEVICE_FS_ISO_OUT_MPS + DEVICE_FS_ISO_OUT1_MPS> 1000)
#error usb带宽不够
#endif

#define AUDIO_CONTROL_MUTE                            0x01
#define AUDIO_CONTROL_VOLUME                          0x02

#define AUDIO_IT_ID                                   0x01
#define AUDIO_FU_ID                                   0x02
#define AUDIO_OT_ID                                   0x03
#define AUDIO_CLK_ID                                  0x08

#define AUDIO_MIC_IT_ID								  0x04
#define AUDIO_MIC_FU_ID								  0x05
#define AUDIO_MIC_SL_ID								  0x06
#define AUDIO_MIC_OT_ID								  0x07
#define AUDIO_MIC_CLK_ID                              0x09

#define AUDIO_SPEAKER_IT_ID							  1
#define AUDIO_SPEAKER_FU_ID							  2		//控制MUTE、VOLUME
#define AUDIO_SPEAKER_OT_ID							  3

#define AUDIO_SPEAKER1_IT_ID						  9
#define AUDIO_SPEAKER1_FU_ID						  10	//控制MUTE、VOLUME
#define AUDIO_SPEAKER1_OT_ID						  11

#define FRQ_MAX_SZE(frq) 			(frq%1000?frq/1000+1:frq/1000)
#define SWAP_BUF_TO_U16(buf) 		((buf[0]) | (buf[1]<<8))
#define SWAP_BUF_TO_U32(buf) 		((buf[0]) | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24))
#define SAMPLE_FREQ_NUM(num)        (uint8_t)(num), (uint8_t)((num >> 8))
#define SAMPLE_FREQ(frq)            (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))
#define SAMPLE_FREQ_4B(frq)         (uint8_t)(frq), (uint8_t)((frq >> 8)), \
									(uint8_t)((frq >> 16)), (uint8_t)((frq >> 24))

#define PCM_LEN_SIZE(a,b,c) 		(FRQ_MAX_SZE(a)*b*c)
#define PCM_LEN_MAX 				MAX(PCM_LEN_SIZE(USBD_AUDIO_FREQ,PCM24BIT,PACKET_CHANNELS_NUM),PCM_LEN_SIZE(USBD_AUDIO_MIC_FREQ,PCM24BIT,MIC_CHANNELS_NUM))
#define OUT_PCM_LEN 				FRQ_MAX_SZE(USBD_AUDIO_FREQ)*PACKET_CHANNELS_NUM*MAX(SPEAKER_ALT1_BITS,SPEAKER_ALT2_BITS)
#define OUT1_PCM_LEN 				FRQ_MAX_SZE(USBD_AUDIO1_FREQ)*PACKET1_CHANNELS_NUM*MAX(SPEAKER1_ALT1_BITS,SPEAKER1_ALT2_BITS)

typedef struct _UsbAudio
{
	uint8_t 				AltSlow;
	uint8_t					AltSet;
	uint8_t					ByteSet;
	uint8_t 				Channels;
	uint8_t 				Mute;
	int16_t					LeftVol;
	int16_t					RightVol;
	int16_t					LeftGain;
	int16_t					RightGain;
	uint32_t				AudioSampleRate;
	uint32_t				FramCount;
	uint32_t				TempFramCount;
	uint32_t 				Accumulator;
#ifdef CFG_RES_AUDIO_USB_VOL_SET_EN
	int16_t					LeftCurGain;
	int16_t					RightCurGain;
#endif
	//缓存FIFO
	MCU_CIRCULAR_CONTEXT 	CircularBuf;
	int16_t*				PCMBuffer;
}UsbAudio;
extern UsbAudio UsbAudioSpeaker1;
extern UsbAudio UsbAudioSpeaker;
extern UsbAudio UsbAudioMic;
extern uint8_t Setup[];
extern uint8_t Request[];

#define SET_CUR		0x2101
#define SET_IDLE	0x210A
#define GET_CUR		0xA181
#define GET_MIN		0xA182
#define GET_MAX		0xA183
#define GET_RES		0xA184

#define SET_CUR_EP	0x2201
#define GET_CUR_EP	0xA281
//uac 2.0
#define GET_RANGE	0xA102
#define GET_CUR_2	0xA101

#define CS_SAM_FREQ_CONTROL 	0x01
#define CS_CLOCK_VALID_CONTROL 	0x02

#define AS_VAL_ALT_SETTINGS_CONTROL 0x02

#define AUDIO_VOL_UP      BIT(0)
#define AUDIO_VOL_DN      BIT(1)
#define AUDIO_PP          BIT(2)
#define AUDIO_NEXT        BIT(3)
#define AUDIO_PREV        BIT(4)
#define AUDIO_STOP        BIT(5)
#define AUDIO_FF          BIT(6)
#define AUDIO_FD          BIT(7)

#define AUDIO_REDIAL 	  BIT(8)
#define AUDIO_HOOK_SWITCH BIT(9)
#define AUDIO_PHONE_MUTE  BIT(10)

void OTG_DeviceAudioSoftwareReset(void);
void OTG_DeviceAudioEpRequest(void);
void OTG_DeviceAudioInterfaceRequest(void);

bool OTG_DeviceAudioSendPcCmd(uint16_t Cmd);
bool OTG_DeviceAudioSendHidKeyDown(uint16_t Cmd);
bool OTG_DeviceAudioSendHidKeyUp(void);

void OnDeviceAudioSendIsoPacket(void);
void OnDeviceAudioRcvIsoPacket(void);
void OnDeviceAudioRcvIsoPacket1(void);

//pc->chip 从缓存区获取数据
uint16_t UsbAudioSpeaker1DataGet(void *Buffer,uint16_t Len);
//pc->chip 获取缓存区数据长度
uint16_t UsbAudioSpeaker1DataLenGet(void);
uint16_t UsbAudioSpeaker1DepthGet(void);

//pc->chip 从缓存区获取数据
uint16_t UsbAudioSpeakerDataGet(void *Buffer,uint16_t Len);
//pc->chip 获取缓存区数据长度
uint16_t UsbAudioSpeakerDataLenGet(void);
uint16_t UsbAudioSpeakerDepthGet(void);


//chip->pc 保存数据到缓存区
uint16_t UsbAudioMicDataSet(void *Buffer,uint16_t Len);
//chip->pc 数据缓存区剩余空间
uint16_t UsbAudioMicSpaceLenGet(void);
uint16_t UsbAudioMicDepthGet(void);
int16_t UsbValToMcuGain(int16_t UsbVal);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
