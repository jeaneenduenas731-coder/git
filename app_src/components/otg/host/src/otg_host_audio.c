/**
 *****************************************************************************
 * @file     otg_host_audio.c
 * @author   Shanks
 * @version  V1.0.0
 * @date     2024.1.12
 * @brief    host mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2024 MVSilicon </center></h2>
 */

#include <string.h>
#include "debug.h"
#include "audio_core_api.h"
#include "otg_host_audio.h"
#include "otg_host_hcd.h"
#include "otg_device_hcd.h"
#include "main_task.h"
#include "irqn.h"
#include "otg_device_audio.h"
#include "otg_host_standard_enum.h"
#include "resampler_polyphase.h"
#undef  OTG_DBG
#define	OTG_DBG(format, ...)		printf(format, ##__VA_ARGS__)

#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE

MCU_CIRCULAR_CONTEXT 	UsbAudioOutFiFoBuf;
MCU_CIRCULAR_CONTEXT 	UsbAudioInFiFoBuf;
MCU_CIRCULAR_CONTEXT 	UsbHidInFiFoBuf;
AUDIO_HandleTypeDef UacAudio;
uint8_t *PcmOutFiFoBuf;
uint8_t *PcmInFiFoBuf;
uint8_t *hidInFiFoBuf;
#ifdef CFG_APP_USB_AUDIO_MODE_EN
extern uint8_t iso_buf[];
#else
uint8_t iso_buf[384];	//48K 32Bit 2Chn
#endif
#define	InterfaceDescriptor		((PUSB_INTERFACE_DESCRIPTOR)OtgHostInfo.UsbInterface[i].pData)

#define	USB_HOST_SINK_NUM		AUDIO_USB_HOST_SINK_NUM
//#define	USB_HOST_SOURCE_NUM		USB_HOST_SOURCE_NUM

uint16_t UacOutPcmDateSet(void *Buffer,uint16_t Len)
{
	MCUCircular_PutData(&UsbAudioOutFiFoBuf, Buffer, Len * sizeof(PCM_DATA_TYPE) * UacAudio.Speaker.NrChannels);
//	APP_DBG("Len %d\n",Len);
	return Len;
}
uint16_t UacOutPcmDateSpaceLenGet(void)
{
	uint16_t NumSamples = 0;
	NumSamples = MCUCircular_GetSpaceLen(&UsbAudioOutFiFoBuf);
	return NumSamples / (sizeof(PCM_DATA_TYPE) * UacAudio.Speaker.NrChannels);
}

uint16_t UacOutPcmDateDepthGet(void)
{
	uint16_t NumSamples = 0;
	NumSamples = UsbAudioOutFiFoBuf.BufDepth;
	return NumSamples / (sizeof(PCM_DATA_TYPE) * UacAudio.Speaker.NrChannels);
}

uint16_t UacInPcmDateGet(void* Buf, uint16_t Len)
{
	uint16_t Length = 0;//Samples
	Length = MCUCircular_GetDataLen(&UsbAudioInFiFoBuf);
	if(Length > Len * (UacAudio.Mic.NrChannels*sizeof(PCM_DATA_TYPE)))
	{
		Length = Len * (UacAudio.Mic.NrChannels*sizeof(PCM_DATA_TYPE));
	}
	MCUCircular_GetData(&UsbAudioInFiFoBuf, Buf, Length);
    return Length / (UacAudio.Mic.NrChannels*sizeof(PCM_DATA_TYPE));
}
uint16_t UacInPcmDateLenGet(void)
{
	uint16_t NumSamples = 0;
    NumSamples = MCUCircular_GetDataLen(&UsbAudioInFiFoBuf);
	return NumSamples / (UacAudio.Mic.NrChannels*sizeof(PCM_DATA_TYPE));
}
uint16_t UacInPcmDateDepthGet(void)
{
	uint16_t NumSamples = 0;
    NumSamples = UsbAudioInFiFoBuf.BufDepth;
	return NumSamples / (UacAudio.Mic.NrChannels*sizeof(PCM_DATA_TYPE));
}

void OnHostHidRcvIntPacket(void)
{
	uint32_t recvlen;
	uint8_t HidInBuf[65];
	OTG_HostInterruptRead1(&UacAudio.Hid.Pipe,(uint8_t*)&HidInBuf[1],UacAudio.Hid.Pipe.MaxPacketSize,&recvlen,0);
	if(recvlen > 0)
	{
		if(MCUCircular_GetSpaceLen(&UsbHidInFiFoBuf)>recvlen)
		{
			HidInBuf[0] = recvlen;
			MCUCircular_PutData(&UsbHidInFiFoBuf, (uint8_t*)HidInBuf,recvlen+1);
		}
	}
}

void OnHostAudioRcvIsoPacket(void)
{
	uint32_t Len;
	int32_t s;
	uint32_t sample = 0;

	OTG_HostISORead(&UacAudio.Mic.Pipe,iso_buf,UacAudio.Mic.Pipe.MaxPacketSize,&Len,0);
	if(Len != 0)
	{
		if(UacAudio.Mic.ByteSet == PCM16BIT)
		{
			sample = Len/PCM16BIT;
	#ifdef CFG_AUDIO_WIDTH_24BIT
			for(s = sample-1; s >= 0; s--)
			{
	//			memcpy(&iso_buf[s*4+1],&iso_buf[s*2],4);
				iso_buf[s*4+2] = iso_buf[s*2+1];
				iso_buf[s*4+1] = iso_buf[s*2];

				if(iso_buf[s*4+2] & 0x80)
				{
					iso_buf[s*4+3] = 0xff;
				}else{
					iso_buf[s*4+3] = 0x00;
				}
				iso_buf[s*4] = 0x00;
			}
	#endif
		}
		else if(UacAudio.Mic.ByteSet == PCM24BIT)
		{
			sample = Len/PCM24BIT;
	#ifdef CFG_AUDIO_WIDTH_24BIT
			for(s = sample-1; s >= 0; s--)
			{
	//			memcpy(&iso_buf[s*4],&iso_buf[s*3],4);
				iso_buf[s*4+2] = iso_buf[s*3+2];
				iso_buf[s*4+1] = iso_buf[s*3+1];
				iso_buf[s*4] = iso_buf[s*3];

				if(iso_buf[s*4+2] & 0x80)
				{
					iso_buf[s*4+3] = 0xff;
				}else{
					iso_buf[s*4+3] = 0x00;
				}
			}
	#else
			for(s = 0; s < sample; s++)
			{
				iso_buf[s*2] = iso_buf[s*3+1];
				iso_buf[s*2+1] = iso_buf[s*3+2];
			}
	#endif
		}

		MCUCircular_PutData(&UsbAudioInFiFoBuf,  (uint8_t*)iso_buf, sample*sizeof(PCM_DATA_TYPE));
	}
	OTG_HostISOReadStart(&UacAudio.Mic.Pipe);
}

void OnHostAudioSendIsoPacket(void)
{
	int32_t s;
	uint32_t channel = UacAudio.Speaker.NrChannels;
	uint32_t SendLen = 0;

	SendLen = UacAudio.Speaker.frequency/1000;
	UacAudio.Speaker.Accumulator += UacAudio.Speaker.frequency%1000;
	if(UacAudio.Speaker.Accumulator > 1000)
	{
		UacAudio.Speaker.Accumulator -= 1000;
		SendLen += 1;
	}

//	if(UacAudio.Speaker.FramCount < (UacAudio.Speaker.frequency/CFG_PARA_SAMPLE_RATE)*10)
//	{
//		memset(iso_buf,0,SendLen*sizeof(PCM_DATA_TYPE)*channel);
//	}
//	else
	{
		if(MCUCircular_GetDataLen(&UsbAudioOutFiFoBuf) < SendLen*sizeof(PCM_DATA_TYPE)*channel)
		{
			memset(iso_buf,0,SendLen*sizeof(PCM_DATA_TYPE)*channel);
		}
		else
		{
			MCUCircular_GetData(&UsbAudioOutFiFoBuf, iso_buf,SendLen*sizeof(PCM_DATA_TYPE)*channel);
		}
	}

	if(UacAudio.Speaker.ByteSet == PCM16BIT)
	{
#ifdef CFG_AUDIO_WIDTH_24BIT
		int32_t *PcmBuf32 = (int32_t *)iso_buf;
		int16_t *PcmBuf16 = (int16_t *)iso_buf;
		for(s=0; s < SendLen*channel; s++)
		{
			PcmBuf16[s] = PcmBuf32[s] >> 8;
		}
#endif
	}
	else if(UacAudio.Speaker.ByteSet == PCM24BIT)
	{
#ifdef CFG_AUDIO_WIDTH_24BIT
		for(s = 0; s<SendLen*channel; s++)
		{
			memcpy(&iso_buf[s*3],&iso_buf[s*4],4);
		}
#else
		for(s = SendLen*channel-1; s >= 0; s--)
		{
			iso_buf[s*3+2] = iso_buf[s*2+1];
			iso_buf[s*3+1] = iso_buf[s*2];
		}
#endif
	}
	OTG_HostISOWrite(&UacAudio.Speaker.Pipe,iso_buf,SendLen*UacAudio.Speaker.ByteSet*channel,0);
}


void  HidInputReportCallbackFunction(USBH_HidReportCallbackInfo * data)
{
//	printf("HidInputReportCallbackFunction\n");
//	printf("id : 0x%x\n",data->report_id);
//	printf("usage : 0x%x\n",data->report_usage);
//	printf("value : 0x%x\n",data->report_value);

	static uint8_t VolumeIncrement = 0;
	static uint8_t VolumeDecrement = 0;
	static uint8_t PlayPause = 0;
	static uint8_t NextTrack = 0;
	static uint8_t PrevTrack = 0;

    switch (data->report_usage)
    {
    case HID_CONSUMER<<16|HID_CONSUMER_VOLUME_INCREMENT:
    	if(VolumeIncrement != data->report_value)
    	{
    		VolumeIncrement = data->report_value;
        	if(data->report_value)
        	{
        		printf("VOLUME_INCREMENT DOWN\n");
    			USBH_AudioSetVolumeIncrement(&UacAudio.Speaker,5);	//Increment 5%
        	}
        	else
        	{
        		printf("VOLUME_INCREMENT UP\n");
        	}
    	}
        break;
    case HID_CONSUMER<<16|HID_CONSUMER_VOLUME_DECREMENT:
		if(VolumeDecrement != data->report_value)
		{
			VolumeDecrement = data->report_value;
			if(data->report_value)
			{
				printf("VOLUME_DECREMENT DOWN\n");
				USBH_AudioSetVolumeDecrement(&UacAudio.Speaker,5);	// Decrement 5%
			}
			else
			{
				printf("VOLUME_DECREMENT UP\n");
			}
		}
        break;
    case HID_CONSUMER<<16|HID_CONSUMER_PLAY_PAUSE:
		if(PlayPause != data->report_value)
		{
			PlayPause = data->report_value;
			if(data->report_value)
			{
				printf("PLAY_PAUSE DOWN\n");
				MessageContext	msgSend;
				msgSend.msgId		= MSG_PLAY_PAUSE;
				MessageSend(GetSysModeMsgHandle(), &msgSend);
			}
			else
			{
				printf("PLAY_PAUSE UP\n");
			}
		}
        break;
    case HID_CONSUMER<<16|HID_CONSUMER_NEXT_TRACK:
    	if(NextTrack != data->report_value)
    	{
    		NextTrack = data->report_value;
        	if(data->report_value)
        	{
        		printf("NEXT_TRACK DOWN\n");
				MessageContext	msgSend;
				msgSend.msgId		= MSG_NEXT;
				MessageSend(GetSysModeMsgHandle(), &msgSend);
        	}
        	else
        	{
        		printf("NEXT_TRACK UP\n");
        	}
    	}
        break;
    case HID_CONSUMER<<16|HID_CONSUMER_PREV_TRACK:
		if(PrevTrack != data->report_value)
		{
			PrevTrack = data->report_value;
			if(data->report_value)
			{
				printf("PREV_TRACK DOWN\n");
				MessageContext	msgSend;
				msgSend.msgId		= MSG_PRE;
				MessageSend(GetSysModeMsgHandle(), &msgSend);
			}
			else
			{
				printf("PREV_TRACK UP\n");
			}
		}
        break;
    default:
        break;
    }
}
void UsbHostHidDateProcess(void)
{
	uint8_t Length = 0;//Samples
	uint8_t hidLength = 0;//Samples
	uint8_t Buf[65];
	Length = MCUCircular_GetDataLen(&UsbHidInFiFoBuf);
	if(Length > 0)
	{
		hidLength = UsbHidInFiFoBuf.CircularBuf[UsbHidInFiFoBuf.R];
		if(Length >= hidLength + 1)
		{
			Length = hidLength + 1;
			MCUCircular_GetData(&UsbHidInFiFoBuf, &Buf, Length);
//			int i;
//			APP_DBG("HID_LEN: %d HID_DATA:",Buf[0]);
//			for(i=1;i<Length;i++)
//			{
//				APP_DBG("%02x ",Buf[i]);
//			}
//			APP_DBG("\n");
			USBH_HidInputReportDataDecode(&Buf[1],hidLength);
		}
	}
}

USBH_StatusTypeDef USBH_AudioSampleRateCheck(uint32_t FreqIn,uint32_t FreqOut)
{
	extern bool GetRatioEnum(uint32_t Scale1000,int32_t *src_ratio);
	int32_t src_ratio;
	bool ret;
	ret = GetRatioEnum(1000 * FreqOut / FreqIn,&src_ratio);
	APP_DBG("FreqIn %ld,FreqOut %ld GetRatioEnum %ld\n",FreqIn,FreqOut,src_ratio);
	if(!ret && src_ratio == RESAMPLER_POLYPHASE_SRC_RATIO_UNSUPPORTED)
	{
		return USBH_NOT_SUPPORTED;
	}
	return USBH_OK;
}

bool UsbHostPlayHardwareInit(void)
{
	uint32_t i;
	//默认参数配置 -- 如果声卡不支持则会选择其他配置。
	UacAudio.Mic.ByteSet = PCM16BIT;
	UacAudio.Mic.frequency = AudioCore.SampleRate[DefaultNet];

	UacAudio.Speaker.ByteSet = PCM16BIT;
	UacAudio.Speaker.frequency = AudioCore.SampleRate[DefaultNet];

	//采样率转换支持检测回调
	USBH_AudioSampleRateCheckCallbackSet(USBH_AudioSampleRateCheck);

	//声卡枚举
	if(USBH_AudioClassDevEnum(&UacAudio.Speaker,&UacAudio.Mic) != USBH_OK)
	{
		OTG_DBG("声卡不支持\n");
		return FALSE;
	}
	if(UacAudio.Speaker.supported)
	{
		USBH_AudioSetVolume(&UacAudio.Speaker,100);	//100%
	//	USBH_AudioSetVolumeDB(&UacAudio.Speaker,0);	//0db
	}
	if(UacAudio.Mic.supported)
	{
		USBH_AudioSetVolume(&UacAudio.Mic,100);		//100%
	//	USBH_AudioSetVolumeDB(&UacAudio.Mic,0);		//0db
	}

	if(!UacAudio.Speaker.supported && !UacAudio.Mic.supported)
	{
		OTG_DBG("声卡端点不支持\n");
		return FALSE;
	}

	//HID枚举
	for(i=0;i<OtgHostInfo.ConfigDesCriptor.bNumInterfaces;i++)
	{
		//解析HID控制接口
		if(InterfaceDescriptor->bInterfaceClass == USB_CLASS_HID)
		{
			USBH_HidDescriptorParse(&UacAudio.Hid,i,(OtgHostInfo.UsbInterface[i].pData),(OtgHostInfo.UsbInterface[i].Length));
		}
	}

	if(!UacAudio.Hid.HidInterfaceNum)
	{
		UacAudio.Hid.supported = 0;
		return TRUE;
	}
	OTG_DBG("UacAudio.Hid.wDescriptorLength %d\n",UacAudio.Hid.wDescriptorLength);
	UacAudio.Hid.ReportDescriptor = (uint8_t*)osPortMalloc(UacAudio.Hid.wDescriptorLength);
	if(UacAudio.Hid.ReportDescriptor == NULL)
	{
		OTG_DBG("UacAudio.Hid.ReportDescriptor == NULL\n");
		return TRUE;
	}
	for(i = 0;i<UacAudio.Hid.HidInterfaceNum;i++)
	{
		if(UacAudio.Hid.Interface[i].supported == 1)
		{
			USBH_HidSetIdle(UacAudio.Hid.Interface[i].interface);
			USBH_HidGetReport(UacAudio.Hid.Interface[i].interface,UacAudio.Hid.ReportDescriptor,UacAudio.Hid.Interface[i].wDescriptorLength);
			USBH_HidReportDescriptorParse(UacAudio.Hid.ReportDescriptor,UacAudio.Hid.Interface[i].wDescriptorLength);
			if(USBH_HidInputReportHasUsage(HID_CONSUMER))	//寻找 Usage Page (Consumer)
			{
				USBH_HidInputReportCallbackRegister(HidInputReportCallbackFunction);
				memcpy(&UacAudio.Hid.Pipe,&UacAudio.Hid.Interface[i].Pipe,sizeof(PIPE_INFO));
				UacAudio.Hid.supported = 1;
				break;
			}
		}
	}
	return TRUE;
}
void UsbHostPlayStart(void)
{
	NVIC_EnableIRQ(Usb_IRQn);
	if(UacAudio.Hid.supported == 1)
	{
		OTG_EndpointInterruptEnable(HOST_INT_IN_EP,OnHostHidRcvIntPacket);
		OTG_DBG("UacAudio.Hid.Pipe.EpNum %d\n",UacAudio.Hid.Pipe.EpNum);
		OTG_DBG("UacAudio.Hid.Pipe.MaxPacketSize %d\n",UacAudio.Hid.Pipe.MaxPacketSize);
		OTG_HostInterruptReadStart(&UacAudio.Hid.Pipe);
		OTG_DBG("UacAudio.Hid.supported\n");
	}
	if(UacAudio.Mic.supported == 1)
	{
		OTG_EndpointInterruptEnable(HOST_ISO_IN_EP,OnHostAudioRcvIsoPacket);
		OTG_DBG("UacAudio.Mic.Pipe.EpNum %d\n",UacAudio.Mic.Pipe.EpNum);
		OTG_DBG("UacAudio.Mic.Pipe.MaxPacketSize %d\n",UacAudio.Mic.Pipe.MaxPacketSize);
//		UacAudio.Mic.Pipe.MaxPacketSize = 288; //bp10芯片需要
		OTG_HostISOReadStart(&UacAudio.Mic.Pipe);
		AudioCoreSourceEnable(USB_HOST_SOURCE_NUM);
	}
	if(UacAudio.Speaker.supported == 1)
	{
		OTG_EndpointInterruptEnable(HOST_ISO_OUT_EP|0x80,OnHostAudioSendIsoPacket);
		OTG_DBG("UacAudio.Speaker.Pipe.EpNum %d\n",UacAudio.Speaker.Pipe.EpNum);
		OTG_DBG("UacAudio.Speaker.Pipe.MaxPacketSize %d\n",UacAudio.Speaker.Pipe.MaxPacketSize);
		OTG_HostISOWrite(&UacAudio.Speaker.Pipe,iso_buf,UacAudio.Speaker.frequency/1000*UacAudio.Speaker.ByteSet*UacAudio.Speaker.NrChannels,0);
		AudioCoreSinkEnable(USB_HOST_SINK_NUM);
	}
}
void UsbHostPlayResInit(void)
{
	AudioCoreIO	AudioIOSet;
	if(UacAudio.Mic.supported == 1)
	{
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
		if(UacAudio.Mic.ResamplerEn)
		{
			AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;
		}
		else
		{
			AudioIOSet.Adapt = SRA_ONLY;//STD;
		}
		AudioIOSet.Sync = FALSE;
		AudioIOSet.Channels = UacAudio.Mic.NrChannels;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = UacInPcmDateGet;
		AudioIOSet.LenGetFunc = UacInPcmDateLenGet;
		AudioIOSet.Depth = UacInPcmDateDepthGet();
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.SampleRate = UacAudio.Mic.frequency;
		DBG("USB_HOST_SOURCE_NUM SampleRate %ld\n",AudioIOSet.SampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, USB_HOST_SOURCE_NUM))
		{
			DBG("Usbin source error!\n");
		}
		AudioCoreSourceAdjust(USB_HOST_SOURCE_NUM, TRUE);//仅在init通路配置微调后，通路使能时 有效
	}

	if(UacAudio.Speaker.supported == 1)
	{
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
		if(UacAudio.Speaker.ResamplerEn)
		{
			AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;
		}
		else
		{
			AudioIOSet.Adapt = SRA_ONLY;//STD;
		}
		AudioIOSet.Sync = FALSE;
		AudioIOSet.Channels = UacAudio.Speaker.NrChannels;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = UacOutPcmDateSet;
		AudioIOSet.LenGetFunc = UacOutPcmDateSpaceLenGet;
		AudioIOSet.Depth = UacOutPcmDateDepthGet();
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.SampleRate = UacAudio.Speaker.frequency;
		DBG("USB_HOST_SINK_NUM SampleRate %ld\n",AudioIOSet.SampleRate);
#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
#endif
		if(!AudioCoreSinkInit(&AudioIOSet, USB_HOST_SINK_NUM))
		{
			DBG("Usbout sink error!\n");
		}
		AudioCoreSinkAdjust(USB_HOST_SINK_NUM,TRUE);
	}
}
bool UsbHostPlayResMalloc(uint16_t SampleLen)
{
	APP_DBG("UsbHostPlayResMalloc %u\n", SampleLen);
	if(UacAudio.Mic.supported == 1)
	{
		PcmInFiFoBuf = (uint8_t*)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
		if(PcmInFiFoBuf == NULL)
		{
			APP_DBG("PcmInFiFoBuf memory error\n");
			return FALSE;
		}
		memset(PcmInFiFoBuf, 0, SampleLen *  sizeof(PCM_DATA_TYPE) * 2 * 2);
		MCUCircular_Config(&UsbAudioInFiFoBuf, PcmInFiFoBuf, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	}
	if(UacAudio.Speaker.supported == 1)
	{
		PcmOutFiFoBuf = (uint8_t*)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
		if(PcmOutFiFoBuf == NULL)
		{
			APP_DBG("PcmOutFiFoBuf memory error\n");
			return FALSE;
		}
		memset(PcmOutFiFoBuf, 0, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
		MCUCircular_Config(&UsbAudioOutFiFoBuf, PcmOutFiFoBuf, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	}

	if(UacAudio.Hid.supported == 1)
	{
		hidInFiFoBuf = (uint8_t*)osPortMalloc(64);
		if(hidInFiFoBuf == NULL)
		{
			APP_DBG("PcmOutFiFoBuf memory error\n");
			return FALSE;
		}
		memset(hidInFiFoBuf, 0, 64);
		MCUCircular_Config(&UsbHidInFiFoBuf, hidInFiFoBuf,64);
	}
	return TRUE;
}

void UsbHostPlayResRelease(void)
{
	if(PcmOutFiFoBuf != NULL)
	{
		APP_DBG("PcmOutFiFoBuf\n");
		osPortFree(PcmOutFiFoBuf);
	}
	if(PcmInFiFoBuf != NULL)
	{
		APP_DBG("PcmInFiFoBuf\n");
		osPortFree(PcmInFiFoBuf);
	}

	if(UacAudio.Hid.ReportDescriptor != NULL)
	{
		APP_DBG("Hid.ReportDescriptor\n");
		osPortFree(UacAudio.Hid.ReportDescriptor);
	}
	memset(&UacAudio,0,sizeof(UacAudio));
	APP_DBG("UsbHostPlayResRelease\n");
}
bool UsbHostPlayMixInit(void)
{
	if(!UsbHostPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("UsbHostPlayResMalloc Res Error!\n");
		return FALSE;
	}
	UsbHostPlayResInit();

	AudioCoreSourceUnmute(USB_HOST_SOURCE_NUM,TRUE,TRUE);
	return TRUE;
}
bool UsbHostPlayMixDeinit(void)
{
	APP_DBG("Usb Host Play Mix Deinit\n");
	AudioCoreSourceMute(USB_HOST_SOURCE_NUM,TRUE,TRUE);
	AudioCoreSourceDisable(USB_HOST_SOURCE_NUM);
	AudioCoreSinkDisable(USB_HOST_SINK_NUM);

	UsbHostPlayResRelease();

	AudioCoreSourceDeinit(USB_HOST_SOURCE_NUM);
	AudioCoreSinkDeinit(USB_HOST_SINK_NUM);
	return TRUE;
}
#endif
