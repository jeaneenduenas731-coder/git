/**
 **************************************************************************************
 * @file    audio_vol.c
 * @brief   audio syetem vol set here
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-1-7 15:42:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "dac.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "debug.h"
#include "audio_vol.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "timeout.h"
#include "bt_manager.h"
#include "bt_play_mode.h"
#include "remind_sound.h"
#include "bt_app_avrcp_deal.h"
#if (BT_HFP_SUPPORT)
#include "bt_hf_mode.h"
#include "bt_hf_api.h"
#endif
#include "ctrlvars.h"
#ifdef CFG_FUNC_RTC_EN
#include "rtc_ctrl.h"
#endif
#include "breakpoint.h"

#ifdef CFG_FUNC_REMIND_SOUND_EN
#include "remind_sound.h"
#endif

#include "i2s.h"
#include "delay.h"
#include "hdmi_in_api.h"
#include "user_effect_parameter.h"
#include "communication.h"

extern HDMIInfo *gHdmiCt;

const uint8_t gBtAbsVolTable[17]={
	0x00, 0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f, 0x47,
	0x4f, 0x57, 0x5f, 0x67, 0x6f, 0x77, 0x7f};

const uint8_t gBtAbsVolSetTable[17]={
	0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48,
	0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x7f};

#if (CFG_PARA_MAX_VOLUME_NUM == 32)
static const int16_t VolumeTable[CFG_PARA_MAX_VOLUME_NUM + 1] = {
	-7200, -6300, -5600, -4900, -4400, -4000, -3600, -3200, -2900, -2600,
	-2400, -2200, -2000, -1900, -1800, -1700, -1600, -1500, -1400, -1300,
	-1200, -1100, -1000, -900, -800, -700, -600, -500, -400, -300,
	-200,  -100, 0
};
#elif (CFG_PARA_MAX_VOLUME_NUM == 16)
static const int16_t VolumeTable[CFG_PARA_MAX_VOLUME_NUM + 1] = {
	-7200, -5600, -4400, -3600, -2900, -2400, -2000, -1600, -1300, -1000,
	-800, -600, -400, -300,-200, -100, 0
};
#endif

const int16_t AudioModeDigitalGianTable[][2]=
{
	{ModeBtAudioPlay,		0/*dB*/},
	{ModeUDiskAudioPlay,	0/*dB*/},
	{ModeUsbDevicePlay,		0/*dB*/},
	{ModeCardAudioPlay,		0/*dB*/},
	{ModeLineAudioPlay,		0/*dB*/},
	{ModeI2SInAudioPlay,	0/*dB*/},
	{ModeOpticalAudioPlay,	0/*dB*/},
	{ModeCoaxialAudioPlay,	0/*dB*/},
};

extern uint8_t GetEffectControlIndex(AUDIOEFFECT_EFFECT_CONTROL type);
//app通路数字预增益处理
void AudioAPPDigitalGianProcess(SysModeNumber AppMode)
{
	uint32_t i;
	int16_t preGain = 0;
	uint8_t refresh_addr = GetEffectControlIndex(APPMODE_PREGAIN);

	for(i = 0; i < sizeof(AudioModeDigitalGianTable)/(sizeof(int16_t)*2); i++)
	{
		if(AppMode == AudioModeDigitalGianTable[i][0])
		{
			preGain = AudioModeDigitalGianTable[i][1] * 100;
			DBG("CurMode preGain = %d dB\n", AudioModeDigitalGianTable[i][1]);
		}
	}

	if(refresh_addr)
	{
		roboeffect_set_effect_parameter(AudioEffect.context_memory, refresh_addr, 1, (int16_t *)&preGain);
		AudioEffect_update_local_params(refresh_addr, 1, (int16_t *)&preGain, 2);
	}
}

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
void Audio_BisDACSwtichChannal(uint8_t channal)
{
	int16_t Swtich;
	if(channal == 1)
	{
		Swtich = 0;
	}
	else
	{
		Swtich = 1;
	}

	roboeffect_set_effect_parameter(AudioEffect.context_memory, GetBisEffectRouteSelector(), 0, (int16_t *)&Swtich);
}
#endif

void AudioEffect_SourceGain_Update(uint8_t source, uint8_t vol)
{
	if(AudioEffect.context_memory == NULL)
		return;

	uint8_t effect_addr = 0;
	extern uint8_t GetEffectControlIndex(AUDIOEFFECT_EFFECT_CONTROL type);

	switch(source)
	{
		case APP_SOURCE_NUM:
			effect_addr = GetEffectControlIndex(MUSIC_VOLUME_ADJUST);
			break;
#ifdef CFG_FUNC_REMIND_SOUND_EN
		case REMIND_SOURCE_NUM:
			effect_addr = GetEffectControlIndex(REMIND_VOLUME_ADJUST);
			break;
#endif
		case MIC_SOURCE_NUM:
			effect_addr = GetEffectControlIndex(MIC_VOLUME_ADJUST);
			break;

#ifdef CFG_FUNC_RECORDER_EN
		case PLAYBACK_SOURCE_NUM:

			break;
#endif
#ifdef CFG_FUNC_MIC_KARAOKE_EN
#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
		case I2S_MIX_SOURCE_NUM:
			break;
#endif
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
			case USB_SOURCE_NUM:
				break;
#endif
#ifdef CFG_FUNC_LINEIN_MIX_MODE
			case LINEIN_MIX_SOURCE_NUM:
				break;
#endif
#endif
			default:
				break;
	}

	if(vol > CFG_PARA_MAX_VOLUME_NUM)
		vol = CFG_PARA_MAX_VOLUME_NUM;
	if(effect_addr != 0)
	{
		roboeffect_set_effect_parameter(AudioEffect.context_memory, effect_addr, 1, (int16_t *)&VolumeTable[vol]);
		AudioEffect_update_local_params(effect_addr, 1, (int16_t *)&VolumeTable[vol], 2);
	}

	gCtrlVars.AutoRefresh = effect_addr;
	AudioEffect_CommunicationQueue_Send(gCtrlVars.AutoRefresh);

#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
#endif
}

uint8_t BtAbsVolume2VolLevel(uint8_t absValue)
{
	uint8_t i;
	for(i=0;i<=16;i++)
	{
		if(absValue == gBtAbsVolTable[i])
			return i;

		if(absValue < gBtAbsVolTable[i])
			return (i-1);
	}
	if(i>16) i=16;
	return i;
}

uint8_t BtLocalVolLevel2AbsVolme(uint8_t localValue)
{
	return gBtAbsVolSetTable[localValue];
}

void HardWareMuteOrUnMute(void)
{
	mainAppCt.gSysVol.MuteFlag = !mainAppCt.gSysVol.MuteFlag;
#ifdef CFG_RES_AUDIO_DAC0_EN
	if(AudioCoreSinkIsEnable(AUDIO_DAC0_SINK_NUM))
	{
		AudioDAC_SoftMute(DAC0, mainAppCt.gSysVol.MuteFlag, mainAppCt.gSysVol.MuteFlag);
	}
#endif
#if CFG_RES_MIC_SELECT
	if(AudioCoreSourceIsEnable(MIC_SOURCE_NUM))
	{
		AudioADC_SoftMute(ADC1_MODULE, mainAppCt.gSysVol.MuteFlag, mainAppCt.gSysVol.MuteFlag);
	}
#endif
}

bool IsAudioPlayerMute(void)
{
	return mainAppCt.gSysVol.MuteFlag;
}

uint8_t AudioMusicVolGet(void)
{
	return mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM];
}

void AudioMusicVolUp(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if(GetSystemMode() == ModeBtHfPlay)
	{
		if(mainAppCt.HfVolume < CFG_PARA_MAX_VOLUME_NUM)
		{
			mainAppCt.HfVolume++;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	}
	else
#endif
	{
		if(MusicVolume < CFG_PARA_MAX_VOLUME_NUM)
		{
			MusicVolume++;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}else
		{
#ifdef CFG_FUNC_REMIND_SOUND_EN
//			RemindSoundServiceItemRequest(SOUND_REMIND_VOLMAX, REMIND_SOUND_INTTERRUPT_PLAY);
#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	}
	
	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

#ifdef CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = MusicVolume;
	#endif
	APP_DBG("REMIND_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(REMIND_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
#endif
#ifdef CFG_FUNC_RECORDER_EN
	mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM] = MusicVolume;
	APP_DBG("PLAYBACK_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(PLAYBACK_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == ModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetSysModeMsgHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT)
	if(GetSystemMode() == ModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	}
#endif
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	if(GetSystemMode() == ModeHdmiAudioPlay)
	{
		gHdmiCt->hdmiActiveReportVolUpDownflag = 2;
	}
#endif
}

void AudioMusicVolDown(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if(GetSystemMode() == ModeBtHfPlay)
	{
		if(mainAppCt.HfVolume > 0)
		{
			mainAppCt.HfVolume--;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
		mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	}
	else
#endif
	{
		if(MusicVolume > 0)
		{
			MusicVolume--;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	}
	
	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

#ifdef CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = MusicVolume;
	#endif
	APP_DBG("REMIND_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(REMIND_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
#endif
#ifdef CFG_FUNC_RECORDER_EN
	mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM] = MusicVolume;
	APP_DBG("PLAYBACK_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(PLAYBACK_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == ModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetSysModeMsgHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT)
	if(GetSystemMode() == ModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	}
#endif
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	if(GetSystemMode() == ModeHdmiAudioPlay)
	{
		gHdmiCt->hdmiActiveReportVolUpDownflag = 2;
	}
#endif
}

void AudioMusicVolSet(uint8_t musicVol)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(musicVol > CFG_PARA_MAX_VOLUME_NUM)
		MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		MusicVolume = musicVol;
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = MusicVolume;
	#endif
#endif
//	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

	AudioEffect_SourceGain_Update(APP_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
}

#ifdef CFG_APP_BT_MODE_EN
void AudioHfVolSet(uint8_t HfVol)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
	if(HfVol > CFG_PARA_MAX_VOLUME_NUM)
		mainAppCt.HfVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		mainAppCt.HfVolume = HfVol;
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	
	APP_DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
}
#endif

#if CFG_RES_MIC_SELECT
void AudioMicVolUp(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(MicVolume < CFG_PARA_MAX_VOLUME_NUM)
	{
		MicVolume++;
		#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
		#endif
	}
    mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
	APP_DBG("MIC_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(MIC_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
}

void AudioMicVolDown(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(MicVolume > 0)
	{
		MicVolume--;
		#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
		#endif
	}
    mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
	APP_DBG("MIC_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(MIC_SOURCE_NUM, mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
}
#endif

void SystemVolSet(void)
{
	uint32_t i;

	for(i=0; i<AUDIO_CORE_SOURCE_MAX_NUM; i++)
	{
		AudioEffect_SourceGain_Update(i, mainAppCt.gSysVol.AudioSourceVol[i]);
	}
#ifdef CFG_FUNC_SHUNNING_EN
	mainAppCt.aux_out_dyn_gain = mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM];
#endif
}

void SystemVolSync(void)
{
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
}

#ifdef CFG_ADC_LEVEL_KEY_EN	
/*
****************************************************************
* 电位器消息接收处理函数
*说明:
*    AdcLevelCh和app_config.h中选择的GPIO有如下对应关系:

       #define  ADCLEVL_CHANNEL_MAP            (ADC_GPIOA20|ADC_GPIOA21|ADC_GPIOA22)
       
       ADC_GPIOA20对应AdcLevelCh为1；
       ADC_GPIOA21对应AdcLevelCh为2；
       ADC_GPIOA22对应AdcLevelCh为3；
****************************************************************
*/
void AdcLevelMsgProcess(uint16_t Msg)//Sliding resistance
{
	uint16_t AdcLevelCh, AdcValue;

    if( (Msg > MSG_ADC_LEVEL_MSG_START)&&(Msg < MSG_ADC_LEVEL_MSG_END) )
	{				
		AdcLevelCh      =   Msg&0xff00;
		AdcLevelCh      -=  MSG_ADC_LEVEL_MSG_START;
		AdcLevelCh      >>= 8;

		AdcValue      =   Msg &0x00ff;
        APP_DBG("AdcLevelCh = %d\n", AdcLevelCh);
		APP_DBG("AdcValue = %d\n",AdcValue);

		switch(AdcLevelCh)
		{
			case 1://ADC LEVEL Channel 1
				break;
				
			case 2://ADC LEVEL Channel 2
				break;
				
			case 3://ADC LEVEL Channel 3
//				mainAppCt.MicBassStepBak = 15 - AdcValue/2;
//				mainAppCt.MicTrebStepBak = AdcValue/2;
				//MicBassTrebAjust(mainAppCt.MicBassStep, mainAppCt.MicTrebStep);
				break;

			case 4://ADC LEVEL Channel 4
			    #if CFG_RES_MIC_SELECT
				mainAppCt.MicVolumeBak = AdcValue;
				APP_DBG("MicVolumeBak = %d\n", mainAppCt.MicVolumeBak);
			    //mainAppCt.gSysVol.AudioSourceVol[0] = mainAppCt.MicVolume = AdcValue;
				//APP_DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
				//AudioEffect_SourceGain_Update(0, mainAppCt.gSysVol.AudioSourceVol[0]);
				#endif
				break;
			case 5://ADC LEVEL Channel 5
				
				break;
			case 6://ADC LEVEL Channel 6
				
				break;
			case 7://ADC LEVEL Channel 7
				
				break;
			case 8://ADC LEVEL Channel 8
				
				break;
			case 9://ADC LEVEL Channel 9
				
				break;
			case 10://ADC LEVEL Channel 10
				
				break;
			case 11://ADC LEVEL Channel 11
				
				break;
		}
	}
}
#endif

#ifdef  CFG_APP_HDMIIN_MODE_EN
void HDMISourceMute(void)
{
	mainAppCt.hdmiSourceMuteFlg = 1;
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
	//APP_DBG("hdmi mute\n");
}

void HDMISourceUnmute(void)
{
	mainAppCt.hdmiSourceMuteFlg = 0;
	AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
	//APP_DBG("hdmi unmute\n");
}

bool IsHDMISourceMute(void)
{
	return mainAppCt.hdmiSourceMuteFlg;
}
#endif
