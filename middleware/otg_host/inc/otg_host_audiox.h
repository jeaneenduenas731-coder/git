/**
 *****************************************************************************
 * @file     otg_host_audiox.h
 * @author   Shanks
 * @version  V1.0.0
 * @date     2024.1.11
 * @brief    host audio V1.0 module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 MVSilicon </center></h2>
 */
#ifndef __OTG_HOST_AUDIOX_H__
#define __OTG_HOST_AUDIOX_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <string.h>
#include <otg_host.h>
#include "otg_host_hcd.h"
#include "mvintrinsics.h"
typedef struct
{
	PIPE_INFO           Pipe;				//端点信息
	uint8_t             supported;			//是否支持
	uint8_t             interface;			//接口
	uint8_t             AltSettings;		//接口设置

	uint8_t				ResamplerEn;

	uint32_t 			Accumulator;

	uint32_t            frequency;
	uint8_t				NrChannels;
	uint8_t				ByteSet;
}USBH_AudioStreamFormatInfo;

typedef USBH_StatusTypeDef(*USBH_AudioSampleRateCheckCallback)(uint32_t FreqIn,uint32_t FreqOut);

void USBH_AudioSampleRateCheckCallbackSet(USBH_AudioSampleRateCheckCallback func);
USBH_StatusTypeDef USBH_AudioClassDevEnum(USBH_AudioStreamFormatInfo *Speaker,USBH_AudioStreamFormatInfo *Mic);
USBH_StatusTypeDef USBH_AudioSetVolume(USBH_AudioStreamFormatInfo *Stream,uint8_t Volume);
USBH_StatusTypeDef USBH_AudioSetVolumeIncrement(USBH_AudioStreamFormatInfo *Stream,uint8_t stepPercent);
USBH_StatusTypeDef USBH_AudioSetVolumeDecrement(USBH_AudioStreamFormatInfo *Stream,uint8_t stepPercent);
USBH_StatusTypeDef USBH_AudioSetSampleFreq(USBH_AudioStreamFormatInfo *Stream, uint32_t frequency);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__UDISK_H__

