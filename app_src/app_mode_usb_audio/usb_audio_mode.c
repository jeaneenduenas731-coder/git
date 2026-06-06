/**
 **************************************************************************************
 * @file    usb_audio_mode.c
 * @brief
 *
 * @author  Owen
 * @version V1.0.0
 *
 * $Created: 2018-04-27 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "main_task.h"
#include "device_detect.h"
#include "otg_detect.h"
#include "otg_device_stor.h"
#include "otg_device_hcd.h"
#include "otg_device_audio.h"
#include "otg_device_standard_request.h"
#include "ctrlvars.h"
#include "remind_sound.h"
#include "audio_vol.h"
#include "audio_effect.h"
#include "irqn.h"
#include "dma.h"
#include "clk.h"
#ifdef CFG_APP_USB_AUDIO_MODE_EN

static const uint8_t DmaChannelMap[6] = {
	//DMA默认配置不要删除，DMA数量不足6个的时候作为填充用
	DMA_CFG_TABLE_DEFAULT_INIT

#ifdef CFG_RES_AUDIO_DAC0_EN
	PERIPHERAL_ID_AUDIO_DAC0_TX,
#endif

#if CFG_RES_MIC_SELECT
	PERIPHERAL_ID_AUDIO_ADC1_RX,
#endif

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	SPDIF_OUT_DMA_ID,
#endif

#if (I2S_ALL_DMA_CH_CFG & I2S0_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S0_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_RX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_RX,
#endif

#ifdef CFG_COMMUNICATION_BY_UART
	CFG_FUNC_COMMUNICATION_TX_DMA_PORT,
	CFG_FUNC_COMMUNICATION_RX_DMA_PORT,
#endif

#ifdef CFG_UPGRADE_BY_UART
	CFG_FUNC_UPGRADE_RX_DMA_PORT,
	CFG_FUNC_UPGRADE_TX_DMA_PORT,
#endif

#ifdef CFG_FUNC_LINEIN_MIX_MODE
	PERIPHERAL_ID_AUDIO_ADC0_RX,
#endif

#ifdef CFG_DUMP_DEBUG_EN
	CFG_DUMP_UART_TX_DMA_CHANNEL,
#endif

	PERIPHERAL_ID_SDIO_RX,
};

void UsbDevicePlayResRelease(void)
{
#ifdef CFG_OTG_MODE_AUDIO1_EN
	if(UsbAudioSpeaker1.PCMBuffer != NULL)
	{
		APP_DBG("UsbAudioSpeaker1.PCMBuffer free\n");
		osPortFree(UsbAudioSpeaker1.PCMBuffer);
		UsbAudioSpeaker1.PCMBuffer = NULL;
	}
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	if(UsbAudioSpeaker.PCMBuffer != NULL)
	{
		APP_DBG("UsbAudioSpeaker.PCMBuffer free\n");
		osPortFree(UsbAudioSpeaker.PCMBuffer);
		UsbAudioSpeaker.PCMBuffer = NULL;
	}
#endif
#ifdef CFG_OTG_MODE_MIC_EN
	if(UsbAudioMic.PCMBuffer != NULL)
	{
		APP_DBG("UsbAudioMic.PCMBuffer free\n");
		osPortFree(UsbAudioMic.PCMBuffer);
		UsbAudioMic.PCMBuffer = NULL;
	}
#endif

}

bool UsbDevicePlayResMalloc(uint16_t SampleLen)
{
	uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);
	uint32_t buf_len;

	APP_DBG("UsbDevicePlayResMalloc %u\n", SampleLen);
//pc->chip
#ifdef CFG_OTG_MODE_AUDIO1_EN
	//Speaker FIFO
	if(USBD_AUDIO1_FREQ > sampleRate)
		buf_len = SampleLen * 16 * (USBD_AUDIO1_FREQ / sampleRate);
	else
		buf_len = SampleLen * 16 * (sampleRate / USBD_AUDIO1_FREQ);

	UsbAudioSpeaker1.PCMBuffer = osPortMalloc(buf_len);
	if(UsbAudioSpeaker1.PCMBuffer == NULL)
	{
		APP_DBG("UsbAudioSpeaker1.PCMBuffer memory error\n");
		return FALSE;
	}
	memset(UsbAudioSpeaker1.PCMBuffer, 0, buf_len);
	MCUCircular_Config(&UsbAudioSpeaker1.CircularBuf, UsbAudioSpeaker1.PCMBuffer, buf_len);
#endif//end of CFG_RES_USB_IN_EN

#ifdef CFG_OTG_MODE_AUDIO_EN
	//Speaker FIFO
	if(USBD_AUDIO_FREQ > sampleRate)
		buf_len = SampleLen * 16 * (USBD_AUDIO_FREQ / sampleRate);
	else
		buf_len = SampleLen * 16 * (sampleRate / USBD_AUDIO_FREQ);

	UsbAudioSpeaker.PCMBuffer = osPortMalloc(buf_len);
	if(UsbAudioSpeaker.PCMBuffer == NULL)
	{
		APP_DBG("UsbAudioSpeaker.PCMBuffer memory error\n");
		return FALSE;
	}
	memset(UsbAudioSpeaker.PCMBuffer, 0, buf_len);
	MCUCircular_Config(&UsbAudioSpeaker.CircularBuf, UsbAudioSpeaker.PCMBuffer, buf_len);
#endif//end of CFG_RES_USB_IN_EN

#ifdef CFG_OTG_MODE_MIC_EN
	//MIC FIFO
	if(USBD_AUDIO_MIC_FREQ > sampleRate)
		buf_len = SampleLen * 16 * (USBD_AUDIO_MIC_FREQ / sampleRate);
	else
		buf_len = SampleLen * 16 * (sampleRate / USBD_AUDIO_MIC_FREQ);

	UsbAudioMic.PCMBuffer = osPortMalloc(buf_len);
	if(UsbAudioMic.PCMBuffer == NULL)
	{
		APP_DBG("UsbAudioMic.PCMBuffer memory error\n");
		return FALSE;
	}
	memset(UsbAudioMic.PCMBuffer, 0, buf_len);
	MCUCircular_Config(&UsbAudioMic.CircularBuf, UsbAudioMic.PCMBuffer,buf_len);
#endif///end of CFG_REGS_AUDIO_USB_OUT_EN

	return TRUE;
}

bool UsbDevicePlayResInit(void)
{
	//Core Source1 para
	AudioCoreIO	AudioIOSet;
#ifdef CFG_OTG_MODE_AUDIO1_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
#if !defined(CFG_PARA_AUDIO_USB_IN_SYNC)
#if !defined(CFG_PARA_AUDIO_USB_IN_SRC)
	AudioIOSet.Adapt = STD;
#else
	AudioIOSet.Adapt = SRC_ONLY;
#endif
#else //需微调   启用硬件时需要和Out协同
#if !defined(CFG_PARA_AUDIO_USB_IN_SRC)
	AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;
#else
	AudioIOSet.Adapt = SRC_SRA;//STD;//SRC_ADJUST;
#endif
#endif
	AudioIOSet.Sync = TRUE;
	AudioIOSet.Channels = PACKET1_CHANNELS_NUM;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = UsbAudioSpeaker1DataGet;
	AudioIOSet.LenGetFunc = UsbAudioSpeaker1DataLenGet;
	AudioIOSet.Depth = UsbAudioSpeaker1DepthGet();
	AudioIOSet.LowLevelCent = 40;
	AudioIOSet.HighLevelCent = 60;
	if(UsbAudioSpeaker1.AudioSampleRate) //已枚举
	{
		AudioIOSet.SampleRate = UsbAudioSpeaker1.AudioSampleRate;
	}
	else
	{
		AudioIOSet.SampleRate = AudioCoreMixSampleRateGet(DefaultNet);//初始值
	}
//#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
//	AudioOutSampleRateSet(AudioIOSet.SampleRate);
//#endif
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
#endif
	if(!AudioCoreSourceInit(&AudioIOSet, USB_AUDIO_SOURCE1_NUM))
	{
		DBG("Usbin source error!\n");
		return FALSE;
	}
	AudioCoreSourceAdjust(USB_AUDIO_SOURCE1_NUM, TRUE);//仅在init通路配置微调后，通路使能时 有效

#endif

#ifdef CFG_OTG_MODE_AUDIO_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
#if !defined(CFG_PARA_AUDIO_USB_IN_SYNC)
#if !defined(CFG_PARA_AUDIO_USB_IN_SRC)
	AudioIOSet.Adapt = STD;
#else
	AudioIOSet.Adapt = SRC_ONLY;
#endif
#else //需微调   启用硬件时需要和Out协同
#if !defined(CFG_PARA_AUDIO_USB_IN_SRC)
	AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;
#else
#if defined(CFG_FUNC_USB_ADJUST_UNION_EN) && !(defined(CFG_WIRELESS_EN) && defined(CFG_WIRELESS_OUT_ON))
	AudioIOSet.Adapt = SRC_ADJUST;//SRC_ADJUST;
#else
	AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;
#endif
#endif
#endif
	AudioIOSet.Sync = TRUE;
	AudioIOSet.Channels = PACKET_CHANNELS_NUM;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = UsbAudioSpeakerDataGet;
	AudioIOSet.LenGetFunc = UsbAudioSpeakerDataLenGet;
	AudioIOSet.Depth = UsbAudioSpeakerDepthGet();
	AudioIOSet.LowLevelCent = 40;
	AudioIOSet.HighLevelCent = 60;
	if(UsbAudioSpeaker.AudioSampleRate) //已枚举
	{
		AudioIOSet.SampleRate = UsbAudioSpeaker.AudioSampleRate;
	}
	else
	{
		AudioIOSet.SampleRate = AudioCoreMixSampleRateGet(DefaultNet);//初始值
	}
#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
	AudioOutSampleRateSet(AudioIOSet.SampleRate);
#endif
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
#endif
	if(!AudioCoreSourceInit(&AudioIOSet, USB_AUDIO_SOURCE_NUM))
	{
		DBG("Usbin source error!\n");
		return FALSE;
	}
	AudioCoreSourceAdjust(USB_AUDIO_SOURCE_NUM, TRUE);//仅在init通路配置微调后，通路使能时 有效

#endif
#ifdef CFG_OTG_MODE_MIC_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
#if !defined(CFG_PARA_AUDIO_USB_OUT_SYNC)
#if !defined(CFG_PARA_AUDIO_USB_OUT_SRC)
	AudioIOSet.Adapt = STD;
#else
	AudioIOSet.Adapt = SRC_ONLY;
#endif
#else //需微调   启用硬件时需要和Out协同
#if !defined(CFG_PARA_AUDIO_USB_OUT_SRC)
	AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;
#else
#if defined(CFG_FUNC_USB_ADJUST_UNION_EN) && !(defined(CFG_WIRELESS_EN) && defined(CFG_WIRELESS_OUT_ON))
	AudioIOSet.Adapt = SRC_ADJUST;//SRC_ADJUST;
#else
	AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;
#endif
#endif
#endif
	AudioIOSet.Sync = FALSE;//FALSE;//
	AudioIOSet.Channels = MIC_CHANNELS_NUM;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = UsbAudioMicDataSet;
	AudioIOSet.LenGetFunc = UsbAudioMicSpaceLenGet;
	AudioIOSet.Depth = UsbAudioMicDepthGet();
	AudioIOSet.LowLevelCent = 40;
	AudioIOSet.HighLevelCent = 60;

	if(UsbAudioMic.AudioSampleRate) //已枚举
	{
		AudioIOSet.SampleRate = UsbAudioMic.AudioSampleRate;
	}
	else
	{
		AudioIOSet.SampleRate = USBD_AUDIO_MIC_FREQ1;//初始值
	}
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
#endif
	if(!AudioCoreSinkInit(&AudioIOSet, USB_AUDIO_SINK_NUM))
	{
		DBG("Usbout sink error!\n");
	}
	AudioCoreSinkAdjust(USB_AUDIO_SINK_NUM,TRUE);
#endif
	return TRUE;
}

//usb声卡模式硬件相关初始化
void UsbDevicePlayHardwareInit(void)
{
	if(!SoftFlagGet(SoftFlagSysCurModeReboot))
	{
		OTG_DeviceModeSel(CFG_PARA_USB_MODE, USB_VID, USBPID(CFG_PARA_USB_MODE));
#ifdef CFG_OTG_MODE_READER_EN
		OTG_DeviceStorInit();
#endif
		OTG_DeviceFifoInit();
		OTG_DeviceInit();
		NVIC_EnableIRQ(Usb_IRQn);
	}
}

//USB声卡模式参数配置，资源初始化
bool UsbDevicePlayInit(void)
{
	APP_DBG("UsbDevice Play Init\n");
	//1st：DMA通道初始化
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);
	//2st：usb声卡相关硬件配置
	UsbDevicePlayHardwareInit();
	//3st：配置系统标准通路
	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	//4st：usb声卡通路资源申请及配置
	if(!UsbDevicePlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("UsbDevicePlayResMalloc Res Error!\n");
		return FALSE;
	}
	if(!UsbDevicePlayResInit())
	{
		APP_DBG("UsbDevicePlayResInit Res Error!\n");
		return FALSE;
	}
	//Core Process
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioMusicProcess);
#else
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioBypassProcess);
#endif

	AudioCodecGainUpdata();//update hardware config

#ifdef CFG_COMMUNICATION_BY_USB
	SetUSBDeviceInitState(TRUE);
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(RemindSoundServiceItemRequest(SOUND_REMIND_SHENGKAM, REMIND_PRIO_NORMAL) == FALSE)
	{
		if(IsAudioPlayerMute() == TRUE)
		{
			HardWareMuteOrUnMute();
		}
	}
#endif
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM, CFG_PARA_MAX_VOLUME_NUM); //设置到最大

#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceUnmute(USB_AUDIO_SOURCE_NUM,TRUE,TRUE);
#endif

#ifndef CFG_FUNC_REMIND_SOUND_EN
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
#endif
	return TRUE;
}
void UsbDevicePlayRun(uint16_t msgId)
{
	switch(msgId)
	{
		case MSG_HID_KEY1_DOWN:
			APP_DBG("KEY1_DOWN\n");
			OTG_DeviceAudioSendHidKeyDown(AUDIO_VOL_UP);
			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}
			break;

		case MSG_HID_KEY2_DOWN:
			APP_DBG("KEY2_DOWN\n");
			OTG_DeviceAudioSendHidKeyDown(AUDIO_VOL_DN);
			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}
			break;

		case MSG_HID_KEY3_DOWN:
			APP_DBG("KEY3_DOWN\n");
			OTG_DeviceAudioSendHidKeyDown(AUDIO_PP);
			break;

		case MSG_HID_KEY_UP:
			APP_DBG("KEY_UP\n");
			OTG_DeviceAudioSendHidKeyUp();
			break;

		case MSG_PLAY_PAUSE:
			APP_DBG("Play Pause\n");
			OTG_DeviceAudioSendPcCmd(AUDIO_PP);
			break;

		case MSG_PRE:
			APP_DBG("PRE Song\n");
			OTG_DeviceAudioSendPcCmd(AUDIO_PREV);
			break;

		case MSG_NEXT:
			APP_DBG("next Song\n");
			OTG_DeviceAudioSendPcCmd(AUDIO_NEXT);
			break;

		case MSG_MUSIC_VOLUP:
			APP_DBG("VOLUP\n");
			OTG_DeviceAudioSendPcCmd(AUDIO_VOL_UP);
			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}
			break;

		case MSG_MUSIC_VOLDOWN:
			APP_DBG("VOLDOWN\n");
			OTG_DeviceAudioSendPcCmd(AUDIO_VOL_DN);
			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}
			break;

		default:
			CommonMsgProccess(msgId);
			break;
	}
#ifdef USB_CRYSTA_FREE_EN
	Clock_USBCrystaFreeAdjustProcess();
#endif
	OTG_DeviceRequestProcess();
#ifdef CFG_OTG_MODE_READER_EN
	OTG_DeviceStorProcess();
#endif
}

bool UsbDevicePlayDeinit(void)
{
	APP_DBG("UsbDevice Play Deinit\n");
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceMute(USB_AUDIO_SOURCE1_NUM,TRUE,TRUE);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceMute(USB_AUDIO_SOURCE_NUM,TRUE,TRUE);
#endif
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	
	PauseAuidoCore();	

	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig((void*)AudioNoAppProcess);
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceDisable(USB_AUDIO_SOURCE1_NUM);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceDisable(USB_AUDIO_SOURCE_NUM);
#endif
#ifdef CFG_OTG_MODE_MIC_EN
	AudioCoreSinkDisable(USB_AUDIO_SINK_NUM);
#endif
	if(!SoftFlagGet(SoftFlagSysCurModeReboot))
	{
		if(!OTG_PortDeviceIsLink())
		{
			APP_DBG("OTG_DeviceDisConnect\n");
			OTG_DeviceDisConnect();
	#ifdef CFG_COMMUNICATION_BY_USB
			SetUSBDeviceInitState(FALSE);
	#endif
		}
		else
		{
			APP_DBG("OTG_DeviceModeSel HID\n");
			OTG_DeviceModeSel(HID, USB_VID, USBPID(HID));
			OTG_DeviceFifoInit();
			OTG_DeviceInit();
		}

		//恢复其他模式的音量值
		AudioMusicVolSet(MusicVolume);
	}
	//NVIC_DisableIRQ(Usb_IRQn);
	UsbDevicePlayResRelease();
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceDeinit(USB_AUDIO_SOURCE1_NUM);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceDeinit(USB_AUDIO_SOURCE_NUM);
#endif
#ifdef CFG_OTG_MODE_MIC_EN
	AudioCoreSinkDeinit(USB_AUDIO_SINK_NUM);
#endif
	ModeCommonDeinit();//通路全部释放

	return TRUE;
}

bool UsbDevicePlayMixInit(void)
{
	APP_DBG("UsbDevice Play Mix Init\n");

	if(!UsbDevicePlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("UsbDevicePlayResMalloc Res Error!\n");
		return FALSE;
	}
	UsbDevicePlayResInit();
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceUnmute(USB_AUDIO_SOURCE1_NUM,TRUE,TRUE);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceUnmute(USB_AUDIO_SOURCE_NUM,TRUE,TRUE);
#endif
	return TRUE;
}
bool UsbDevicePlayMixDeinit(void)
{
	APP_DBG("UsbDevice Play Mix Deinit\n");
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceMute(USB_AUDIO_SOURCE1_NUM,TRUE,TRUE);
	AudioCoreSourceDisable(USB_AUDIO_SOURCE1_NUM);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceMute(USB_AUDIO_SOURCE_NUM,TRUE,TRUE);
	AudioCoreSourceDisable(USB_AUDIO_SOURCE_NUM);
#endif
#ifdef CFG_OTG_MODE_MIC_EN
	AudioCoreSinkDisable(USB_AUDIO_SINK_NUM);
#endif
	UsbDevicePlayResRelease();
#ifdef CFG_OTG_MODE_AUDIO1_EN
	AudioCoreSourceDeinit(USB_AUDIO_SOURCE1_NUM);
#endif
#ifdef CFG_OTG_MODE_AUDIO_EN
	AudioCoreSourceDeinit(USB_AUDIO_SOURCE_NUM);
#endif
#ifdef CFG_OTG_MODE_MIC_EN
	AudioCoreSinkDeinit(USB_AUDIO_SINK_NUM);
#endif
	return TRUE;
}
#endif //CFG_APP_USB_AUDIO_MODE_EN

