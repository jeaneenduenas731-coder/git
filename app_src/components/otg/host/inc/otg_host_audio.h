/**
 *****************************************************************************
 * @file     otg_host_audio10.h
 * @author   Shanks
 * @version  V1.0.0
 * @date     2024.1.11
 * @brief    host audio V1.0 module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 MVSilicon </center></h2>
 */
#ifndef __OTG_HOST_AUDIO_H__
#define __OTG_HOST_AUDIO_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <string.h>
#include "otg_host_audiox.h"
#include "otg_host_hid.h"

typedef struct _AUDIO_Process
{
	USBH_AudioStreamFormatInfo   		Speaker;
	USBH_AudioStreamFormatInfo   		Mic;
	HID_ClassSpecificDescTypedef		Hid;
}
AUDIO_HandleTypeDef;

bool UsbHostPlayHardwareInit(void);
bool UsbHostPlayMixInit(void);
bool UsbHostPlayMixDeinit(void);
void UsbHostPlayStart(void);
void UsbHostHidDateProcess(void);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__UDISK_H__

