/**
 **************************************************************************************
 * @file    audio_core.c
 * @brief   audio core 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include <nds32_intrinsic.h>
#include "main_task.h"
#include "audio_core_service.h"
#include "ctrlvars.h"
#include "audio_effect.h"
#include "mcu_circular_buf.h"
#include "beep.h"
#include "dma.h"

#ifdef CFG_APP_BT_MODE_EN
#include "bt_config.h"
#include "bt_play_api.h"
#include "bt_manager.h"
#if BT_HFP_SUPPORT
#include "bt_hf_api.h"
#endif
#endif
#include "dac_interface.h"
#include "audio_vol.h"
#include "user_effect_parameter.h"

/*******************************************************************************************************************************
 *
 *				 |***GetAdapter***|	  			 |***********CoreProcess***********|			  |***SetAdapter***|
 * ************	 ******************	 **********	 ***********************************  **********  ******************  **********
 *	SourceFIFO*->*SRCFIFO**SRAFIFO*->*PCMFrame*->*PreGainEffect**MixNet**GainEffect*->*PCMFrame*->*SRAFIFO**SRCFIFO*->*SinkFIFO*
 * ************  ******************	 **********	 ***********************************  **********  ******************  **********
 * 				 |*Context*|																			 |*Context*|
 *
 *******************************************************************************************************************************/

typedef enum
{
	AC_RUN_CHECK,//用于检测是否需要暂停任务，如果需要暂停任务，则停留再该状态
	AC_RUN_GET,
	AC_RUN_PROC,
	AC_RUN_PUT,
}AudioCoreRunState;

static AudioCoreRunState AudioState = AC_RUN_CHECK;
AudioCoreContext		AudioCore;

extern uint32_t gSysTick;

void AudioCoreSourcePcmFormatConfig(uint8_t Index, uint16_t Format)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Channels = Format;
	}
}

void AudioCoreSourceEnable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Enable = TRUE;
	}
}

void AudioCoreSourceDisable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Enable = FALSE;
	}
}

bool AudioCoreSourceIsEnable(uint8_t Index)
{
	return AudioCore.AudioSource[Index].Enable;
}

void AudioCoreSourceMute(uint8_t Index, bool IsLeftMute, bool IsRightMute)
{
	if(IsLeftMute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = TRUE;
	}
	if(IsRightMute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = TRUE;
	}
}

void AudioCoreSourceUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute)
{
	if(IsLeftUnmute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = FALSE;
	}
	if(IsRightUnmute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = FALSE;
	}
}

bool AudioCoreSourceMuteStatusGet(uint8_t Index)
{
	return AudioCore.AudioSource[Index].LeftMuteFlag | AudioCore.AudioSource[Index].RightMuteFlag;
}

void AudioCoreSourceConfig(uint8_t Index, AudioCoreSource* Source)
{
	memcpy(&AudioCore.AudioSource[Index], Source, sizeof(AudioCoreSource));
}

void AudioCoreSinkEnable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = TRUE;
}

void AudioCoreSinkDisable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = FALSE;
}

bool AudioCoreSinkIsEnable(uint8_t Index)
{
	return AudioCore.AudioSink[Index].Enable;
}

void AudioCoreSinkConfig(uint8_t Index, AudioCoreSink* Sink)
{
	memcpy(&AudioCore.AudioSink[Index], Sink, sizeof(AudioCoreSink));
}


void AudioCoreProcessConfig(AudioCoreProcessFunc AudioEffectProcess)
{
	AudioCore.AudioEffectProcess = AudioEffectProcess;
}

///**
// * @func        AudioCoreConfig
// * @brief       AudioCore参数块，本地化API
// * @param       AudioCoreContext *AudioCoreCt
// * @Output      None
// * @return      None
// * @Others      外部配置的参数块，复制一份到本地
// */
//void AudioCoreConfig(AudioCoreContext *AudioCoreCt)
//{
//	memcpy(&AudioCore, AudioCoreCt, sizeof(AudioCoreContext));
//}

bool AudioCoreInit(void)
{
	DBG("AudioCore init\n");
	return TRUE;
}

void AudioCoreDeinit(void)
{
	AudioState = AC_RUN_CHECK;
}

/**
 * @func        AudioCoreRun
 * @brief       音源拉流->音效处理+混音->推流
 * @param       None
 * @Output      None
 * @return      None
 * @Others      当前由audioCoreservice任务保障此功能有效持续。
 * Record
 */
extern uint32_t 	IsAudioCorePause;
extern uint32_t 	IsAudioCorePauseMsgSend;
void AudioProcessMain(void);
__attribute__((optimize("Og")))
void AudioCoreRun(void)
{
	bool ret;
	switch(AudioState)
	{
		case AC_RUN_CHECK:
			if(IsAudioCorePause == TRUE)
			{
				if(IsAudioCorePauseMsgSend == TRUE)
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_AUDIO_CORE_HOLD;
					MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);

					IsAudioCorePauseMsgSend = FALSE;
				}
				return;
			}
		case AC_RUN_GET:
			AudioCoreIOLenProcess();
#ifdef CFG_APP_BT_MODE_EN
			//audio core检测:蓝牙模式下数据播空监控
			extern void BtPlayACBtMonitor(uint16_t empty_flag);
			BtPlayACBtMonitor(SOURCE_BIT_GET(AudioCore.FrameReady, APP_SOURCE_NUM));
#endif
			ret = AudioCoreSourceSync();
			if(ret == FALSE)
			{
				return;
			}

		case AC_RUN_PROC:
			AudioProcessMain();
			AudioState = AC_RUN_PUT;

		case AC_RUN_PUT:
			ret = AudioCoreSinkSync();
			if(ret == FALSE)
			{
//				AudioCoreIOLenProcess();
				return;
			}
			AudioState = AC_RUN_CHECK;
			break;
		default:
			break;
	}
}

//音效处理函数，主入口
//将mic通路数据剥离出来统一处理
//mic通路数据和具体模式无关
//提示音通路无音效，剥离后在sink端混音。
void AudioProcessMain(void)
{
	AudioCoreSourceMuteApply();
#ifdef CFG_FUNC_RECORDER_EN
	if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].Channels == 1)
		{
			uint16_t i;
			for(i = SOURCEFRAME(PLAYBACK_SOURCE_NUM) * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}
#endif
	if(AudioCore.AudioSource[APP_SOURCE_NUM].Active == TRUE)////music buff
	{
		#if (BT_HFP_SUPPORT) && defined(CFG_APP_BT_MODE_EN)
		if(GetSystemMode() != ModeBtHfPlay)
		#endif
		{
			if(AudioCore.AudioSource[APP_SOURCE_NUM].Channels == 1)
			{
				uint16_t i;
				for(i = SOURCEFRAME(APP_SOURCE_NUM) * 2 - 1; i > 0; i--)
				{
					AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i / 2];
				}
			}
		}
	}	
	if(AudioCore.AudioSource[USB_HOST_SOURCE_NUM].Active == TRUE)////music buff
	{
		if(AudioCore.AudioSource[USB_HOST_SOURCE_NUM].Channels == 1)
		{
			uint16_t i;
			for(i = SOURCEFRAME(USB_HOST_SOURCE_NUM) * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[USB_HOST_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[USB_HOST_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}
#if defined(CFG_FUNC_REMIND_SOUND_EN)
	if(AudioCore.AudioSource[REMIND_SOURCE_NUM].Active == TRUE)////remind buff
	{
		if(AudioCore.AudioSource[REMIND_SOURCE_NUM].Channels == 1)
		{
			uint16_t i;
			for(i = SOURCEFRAME(REMIND_SOURCE_NUM) * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}	
#endif


	if(AudioCore.AudioEffectProcess != NULL)
	{
		AudioCore.AudioEffectProcess((AudioCoreContext*)&AudioCore);
	}
	
#ifdef CFG_FUNC_BEEP_EN
    if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].Active == TRUE)   ////dacx buff
	{
		Beep(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf, SINKFRAME(AUDIO_DAC0_SINK_NUM));
	}
#endif
    AudioCoreSinkMuteApply();
}

//音量淡入淡出
#define MixerFadeVolume(a, b, c)  	\
    if(a > b + c)		    \
	{						\
		a -= c;				\
	}						\
	else if(a + c < b)	   	\
	{						\
		a += c;				\
	}						\
	else					\
	{						\
		a = b;				\
	}


void AudioCoreSourceMuteApply(void)
{
	uint32_t i;
	uint8_t j;
	uint16_t LeftVol, RightVol, TargetVol, LeftVolStep, RightVolStep;
	bool mute;

	for(j = 0; j < AUDIO_CORE_SOURCE_MAX_NUM; j++)
	{
		mute = AudioCore.AudioSource[j].LeftMuteFlag || AudioCore.AudioSource[j].RightMuteFlag || mainAppCt.gSysVol.MuteFlag
				|| IsAudioCorePause || (!AudioCore.AudioSource[j].FrameReady);
		if((!AudioCore.AudioSource[j].Active) || (!AudioCore.AudioSource[j].Enable) || (mute == AudioCore.AudioSource[j].MuteFlagbk))
		{
			if(AudioCoreSourceToRoboeffect(j) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				if(!AudioCore.AudioSource[j].Active || !AudioCore.AudioSource[j].Enable
						|| mainAppCt.gSysVol.MuteFlag || AudioCore.AudioSource[j].LeftMuteFlag)
				{
					memset(roboeffect_get_source_buffer(AudioEffect.context_memory, AudioCoreSourceToRoboeffect(j)),
										0, roboeffect_get_buffer_size(AudioEffect.context_memory));
					AudioCore.AudioSource[j].FrameReady = FALSE;
				}
			}
			AudioCore.AudioSource[j].MuteFlagbk = mute;
			continue;
		}

		TargetVol = mute ? 0:4096;
		LeftVol = 4096 - TargetVol;
		RightVol = LeftVol;
		LeftVolStep = 4096 / SOURCEFRAME(j) + (4096 % SOURCEFRAME(j) ? 1 : 0);
		RightVolStep = LeftVolStep;
#ifdef CFG_AUDIO_WIDTH_24BIT
		if(AudioCore.AudioSource[j].BitWidth == PCM_DATA_24BIT_WIDTH //24bit 数据
		 || AudioCore.AudioSource[j].BitWidthConvFlag     //在AudioCoreSourceGet扩充到24bit
			)
		{
			if(AudioCore.AudioSource[j].Channels == 2)
			{
				for(i=0; i < SOURCEFRAME(j); i++)
				{
					AudioCore.AudioSource[j].PcmInBuf[2 * i + 0] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (24)-1);
					AudioCore.AudioSource[j].PcmInBuf[2 * i + 1] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 1]) * RightVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSource[j].Channels == 1)
			{
				for(i=0; i<SOURCEFRAME(j); i++)
				{
					AudioCore.AudioSource[j].PcmInBuf[i] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[i]) * LeftVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		else
#endif
		{
			int16_t * PcmInBuf = (int16_t *)AudioCore.AudioSource[j].PcmInBuf;
			if(AudioCore.AudioSource[j].Channels == 2)
			{
				for(i=0; i < SOURCEFRAME(j); i++)
				{
					PcmInBuf[2 * i + 0] = __nds32__clips((((int32_t)PcmInBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (16)-1);
					PcmInBuf[2 * i + 1] = __nds32__clips((((int32_t)PcmInBuf[2 * i + 1]) * RightVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSource[j].Channels == 1)
			{
				for(i=0; i<SOURCEFRAME(j); i++)
				{
					PcmInBuf[i] = __nds32__clips((((int32_t)PcmInBuf[i]) * LeftVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		AudioCore.AudioSource[j].MuteFlagbk = mute;
	}
}

void AudioCoreSinkMuteApply(void)
{
	uint32_t i;
	uint8_t j;
	uint16_t LeftVol, RightVol, TargetVol, LeftVolStep, RightVolStep;
	bool mute;

	for(j = 0; j < AUDIO_CORE_SINK_MAX_NUM; j++)
	{
		mute = AudioCore.AudioSink[j].LeftMuteFlag || AudioCore.AudioSink[j].RightMuteFlag || mainAppCt.gSysVol.MuteFlag || IsAudioCorePause;
		if((!AudioCore.AudioSink[j].Active) || (!AudioCore.AudioSink[j].Enable) || (mute == AudioCore.AudioSink[j].MuteFlagbk))
		{
			if(AudioCoreSinkToRoboeffect(j) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				if(!AudioCore.AudioSink[j].Active || !AudioCore.AudioSink[j].Enable
						|| mainAppCt.gSysVol.MuteFlag || AudioCore.AudioSink[j].LeftMuteFlag)
				{
					memset(roboeffect_get_sink_buffer(AudioEffect.context_memory, AudioCoreSinkToRoboeffect(j)),
										0, roboeffect_get_buffer_size(AudioEffect.context_memory));
				}
			}
			AudioCore.AudioSink[j].MuteFlagbk = mute;
			continue;
		}

		TargetVol = mute ? 0:4096;
		LeftVol = 4096 - TargetVol;
		RightVol = LeftVol;
		LeftVolStep = 4096 / SINKFRAME(j) + (4096 % SINKFRAME(j) ? 1 : 0);
		RightVolStep = LeftVolStep;

#ifdef CFG_AUDIO_WIDTH_24BIT
		if(AudioCore.AudioSink[j].BitWidth == PCM_DATA_24BIT_WIDTH //24bit 数据
		 //|| AudioCore.AudioSink[j].BitWidthConvFlag     //在AudioCoreSinkSet扩充到24bit,此时还是16bit
			)
		{
			if(AudioCore.AudioSink[j].Channels == 2)
			{
				for(i=0; i < SINKFRAME(j); i++)
				{
					AudioCore.AudioSink[j].PcmOutBuf[2 * i + 0] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (24)-1);
					AudioCore.AudioSink[j].PcmOutBuf[2 * i + 1] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[2 * i + 1]) * RightVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSink[j].Channels == 1)
			{
				for(i=0; i<SINKFRAME(j); i++)
				{
					AudioCore.AudioSink[j].PcmOutBuf[i] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[i]) * LeftVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		else
#endif
		{
			int16_t * PcmOutBuf = (int16_t *)AudioCore.AudioSink[j].PcmOutBuf;
			if(AudioCore.AudioSink[j].Channels == 2)
			{
				for(i=0; i < SINKFRAME(j); i++)
				{
					PcmOutBuf[2 * i + 0] = __nds32__clips((((int32_t)PcmOutBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (16)-1);
					PcmOutBuf[2 * i + 1] = __nds32__clips((((int32_t)PcmOutBuf[2 * i + 1]) * RightVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSink[j].Channels == 1)
			{
				for(i=0; i<SINKFRAME(j); i++)
				{
					PcmOutBuf[i] = __nds32__clips((((int32_t)PcmOutBuf[i]) * LeftVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		AudioCore.AudioSink[j].MuteFlagbk = mute;
	}
}
