#include "add_status.h"
#include "debug.h"
#include "remind_sound.h"
#include "media_play_api.h"
#include "string.h"
#include "bt_manager.h"
#include "main_task.h"
#include "ctrlvars.h"
#include <stdio.h>
#include "user_effect_flow_music.h"
#include "user_effect_parameter.h"
#include "user_effect_flow_Karaoke.h"

#ifdef CFG_SIM_COMM_EN
#include "sim_comm.h"
#endif

#ifdef CFG_UART_EN
#include "UartRev.h"
#endif

#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
#include "led_effect.h"
#endif
UserVar userVar;
static TIMER	DebugTimer;

#ifdef UserSoftPower
void GPIO_Init(void)		//GPIO初始化
{
	#ifdef CFG_LOCK_EN
	LOCK_INIT();
	LOCK_ON;
	#endif
	
	#ifdef CFG_LED1_G
	LED1_G_INIT();
	LED1_G_OFF;
	#endif
	
	#ifdef CFG_LED1_R
	LED1_R_INIT();
	LED1_R_OFF;
	#endif
	
	#ifdef CFG_LED2_G
	LED2_G_INIT();
	LED2_G_OFF;
	#endif
	
	#ifdef CFG_LED1_R
	LED2_R_INIT();
	LED2_R_OFF;
	#endif
	
	#ifdef CFG_SLAVE_EN
	SLAVE_INIT();
	SLAVE_ON;
	#endif

	#ifdef CFG_MUTE_4917_EN
	MUTE_4917_INIT();
	MUTE_4917_ON;
	#endif

	#ifdef CFG_INCHARGE_FULL_EN
	INCHARGE_FULL_INIT();
	#endif

	#ifdef CFG_BT_LED
	BT_LED_INIT();
	BT_LED_OFF;
	#endif
	
	#ifdef	CFG_UART_EN
	Uart_Init();
	#endif
}

void VarInit(void)		//自定义结构体初始化
{
	TimeOutSet(&DebugTimer, 5000);

	userVar.Version = Software_version;

	userVar.ProgramSate = PROGRAM_STARTING;
	TimeOutSet(&userVar.ProgramDelay, 2500);
	
	TimeOutSet(&userVar.FlushTimer, 500);

	TimeOutSet(&userVar.Device_DET_Timer, 200);
	
	#ifdef CFG_AMP_MUTE_STATE_EN
	TimeOutSet(&userVar.AmpMuteTimer, 4000);
	TimeOutSet(&userVar.AmpMuteDelayTimer, 4000);
	#endif
	
	#ifdef CFG_HARD_POWEROFF
	TimeOutSet(&userVar.IsPowerTimer, 200);
	userVar.IsPowerFlag = 0;
	#endif	
	
	#ifdef CFG_POWERON_LET_GO
	TimeOutSet(&userVar.IsLetGoTimer, 3000);
	userVar.IsLetGoFlag = 1;
	#endif
	
	#ifdef Spectrum_EN	//屏谱
	if(!init_audio_spectrum(&userVar.spectrum_data))
	{
		DBG("****init_audio_spectrum:error******\n");
	}
	TimeOutSet(&userVar.fft_wait_timer, 5000);
	#endif

	#ifdef CFG_SIM_COMM_EN
	analog_comm_init();
	#endif
}

void PowerOn(void)
{
	GPIO_Init();
	VarInit();

	#ifdef CFG_USER_IIC_EN
	Device_Init();
	#endif

	#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	pwm_led_Init();
	#endif

	DBG("--PowerOn-- %s_V%d\n", ProjectName, Software_version);
}

void PowerOff(void)
{
	#ifdef CFG_AMP_MUTE_EN
	AMP_MUTE_INIT();
	AMP_MUTE_ON;
	#endif

	#ifdef CFG_AMP_SHDN_EN
	AMP_SHDN_INIT();
	AMP_SHDN_OFF;
	#endif
	
	#ifdef	CFG_UART_EN
	NVIC_DisableIRQ(UART0_IRQn);	  		//UART0启动
	#endif
	
	#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	LedEffectOff(TRUE);
	#endif
	
	#ifdef CFG_LED1_G
	LED1_G_INIT();
	LED1_G_OFF;
	#endif
	
	#ifdef CFG_LED1_R
	LED1_R_INIT();
	LED1_R_OFF;
	#endif
	
	#ifdef CFG_LED2_G
	LED2_G_INIT();
	LED2_G_OFF;
	#endif
	
	#ifdef CFG_LED1_R
	LED2_R_INIT();
	LED2_R_OFF;
	#endif

	#ifdef DISP_DEV_7_LED
	DispInit(1);
	#endif

	#ifdef CFG_BT_LED
	BT_LED_INIT();
	BT_LED_OFF;
	#endif
	
	#ifdef CFG_SLAVE_EN
	SLAVE_INIT();
	SLAVE_OFF;
	#endif

	DBG("-------PowerOff-------\n");
	DelayMs(20);

	userVar.ProgramSate == PROGRAM_DEEPSLEEPING;
	
	#ifdef CFG_LOCK_EN
	LOCK_INIT();
	#ifdef CFG_FUNC_OPTION_CHARGER_DETECT
	if(IsInCharge())
	{
		LOCK_ON;
	}
	else
	#endif
	{
		LOCK_OFF;
	}
	#endif
}

bool PowerOn_Delay(uint8_t flag)	//延时开机，1为可以开机，0为不可以
{
	#ifdef DelayPowerON

	uint16_t KeyValue;
	static uint16_t loop=0;
		
	if(flag)
	{
		DelayMs(20);
		KeyValue = GetAdcArvgeVal(ADC_CHANNEL_BK_ADKEY);
		if(KeyValue < 1000) //220KΩ
		{	
			loop++;
		}
		else
		{
			loop=0;
		}

		if(loop >= DelayPowerON)
		{
			loop = 0;
			DBG("-------Allow startup-------ON\n");	
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		for(uint16_t i = 0; i < (DelayPowerON * 1.5); i++)
		{
			DelayMs(20);
			WDG_Feed();
			KeyValue = GetAdcArvgeVal(ADC_CHANNEL_BK_ADKEY);
			if(KeyValue < 1000) //220KΩ
			{
				loop++;

				if(loop >= (DelayPowerON))
				{
					DBG("-------Allow startup-------ON     %d\n", loop);
					loop = 0;
					return TRUE;
				}
				else if(loop == 15)
				{
					i = 0;
				}
			}
			else
			{
				loop=0;
			}
		}

		if(loop < (DelayPowerON-5))
		{
			DBG("-------Reject startup-------OFF   %d\n", loop);	
			loop = 0;
			return FALSE;
		}
		else
		{
			DBG("-------Allow startup-------ON     %d\n", loop);		
			loop = 0;
			return TRUE;
		}
	}
	#endif
	
	return TRUE;
}

void HeartBeat(void)		//心跳函数
{
	#if (defined(CFG_PWM_LED_EN))
	CheckRgbLedEffect();	//灯
	#endif	
	
	#ifdef CFG_UART_EN
	UartHandle();			//串口
	#endif

	//更改程序状态
	if((IsTimeOut(&userVar.ProgramDelay))&&(userVar.ProgramSate == PROGRAM_STARTING))
	{
		userVar.ProgramSate = PROGRAM_RUNNING;				//一般用于避免电位器刚开机，会显示
	}

	CheckSignal();

	//闪烁标志
	for(uint8_t i = 0; i < FLUSH_TIME_MAX; i++)
	{
		if(IsTimeOut(&userVar.FlushTimer[i]))
		{
			ToggleBit(userVar.FlushFlag, i);
			switch (i)
			{
			case FLUSH_TIME_300MS:
				{
					TimeOutSet(&userVar.FlushTimer[i], 300);
				}
				break;
			
			case FLUSH_TIME_500MS:
				{
					TimeOutSet(&userVar.FlushTimer[i], 500);
				}
				break;
			
			case FLUSH_TIME_1S:
				{
					TimeOutSet(&userVar.FlushTimer[i], 1000);
				}
				break;

			default:
				break;
			}
		}
	}
	
	AmpMuteState();
	LED_Swtich();

	//打印专用定时器
	if(IsTimeOut(&DebugTimer))
	{
		TimeOutSet(&DebugTimer, 5000);

		#if 0
		DBG("%s PowerLevel = %d  %d  %d\n", GetModeNameStr(GetSystemMode()), GetPowerLevel(), IsInCharge(), INCHARGE_FULL_DET);
		DBG("%d  %d  %d\n", GetEffectParam(KARAOKE_vocal_cut0_ADDR, 0), GetEffectParam(KARAOKE_reverb0_ADDR, 1), GetEffectParam(KARAOKE_reverb0_ADDR, 3));
		#endif
	}
}

#endif

//蓝牙灯，连接常亮，未连接闪烁
void LED_Swtich(void)
{	
	if(IsTimeOut(&userVar.Device_DET_Timer))
	{
		TimeOutSet(&userVar.Device_DET_Timer,	100);
		
		BT_LED_Switch();
	}
}

void AmpMuteSet(bool flag)	//1为关功放， 0为解功放
{
	#if (defined(CFG_FUNC_DETECT_PHONE_EN) && defined(CFG_AMP_MUTE_EN))
	if(userVar.AmpMuteFlag == 3)
	{
		#ifdef CFG_AMP_MUTE_EN
		AMP_MUTE_ON;
		#endif

		#ifdef CFG_MUTE_4917_EN
		MUTE_4917_ON;
		#endif
		
		return;
	}
	#endif

	#ifdef CFG_FUNC_DETECT_PHONE_EN
	if(!gCtrlVars.EarPhoneOnlin)
	#endif
	{
		#ifdef CFG_AMP_MUTE_EN
		if(flag)
		{
			userVar.AmpMuteFlag = 1;
			
			#ifdef CFG_PWM_EN
			if(userVar.PwmFlag < PWM_STATE_SUB)
				userVar.PwmFlag = PWM_STATE_SUB;
			#elif defined(CFG_AMP_MUTE_EN)
			AMP_MUTE_ON;
			#endif

			#ifdef CFG_MUTE_4917_EN
			MUTE_4917_ON;
			#endif
		}
		else
		{
			userVar.AmpMuteFlag = 0;
			
			#ifdef CFG_PWM_EN
			if(userVar.PwmFlag > PWM_STATE_ADD)
				userVar.PwmFlag = PWM_STATE_ADD;
			#elif defined(CFG_AMP_MUTE_EN)
			AMP_MUTE_OFF;
			#endif

			#ifdef CFG_MUTE_4917_EN
			MUTE_4917_ON;
			#endif
		}
		#endif
	}
	#ifdef CFG_FUNC_DETECT_PHONE_EN
	else
	{
		#ifdef CFG_AMP_MUTE_EN
		if(flag)
		{
			userVar.AmpMuteFlag = 1;
			
			#ifdef CFG_PWM_EN
			if(userVar.PwmFlag < PWM_STATE_SUB)
				userVar.PwmFlag = PWM_STATE_SUB;
			#elif defined(CFG_AMP_MUTE_EN)
			AMP_MUTE_ON;
			#endif

			#ifdef CFG_MUTE_4917_EN
			MUTE_4917_ON;
			#endif
		}
		else
		{
			userVar.AmpMuteFlag = 0;
			
			#ifdef CFG_PWM_EN
			if(userVar.PwmFlag > PWM_STATE_ADD)
				userVar.PwmFlag = PWM_STATE_ADD;
			#elif defined(CFG_AMP_MUTE_EN)
			AMP_MUTE_ON;
			#endif

			#ifdef CFG_MUTE_4917_EN
			MUTE_4917_OFF;
			#endif
		}
		#endif
	}
	#endif
}

void AmpMuteState(void)	//是否开关功放
{
	#ifdef CFG_HARD_POWEROFF
	//硬件检测到可能关机，关功放
	if(userVar.IsPowerFlag && IsTimeOut(&userVar.IsPowerTimer))
	{
		//未关机
		userVar.IsPowerFlag = 0;
		DBG("no deepsleep!\n");
	}
	else if(userVar.IsPowerFlag)
	{
		AmpMuteSet(TRUE);
		return;
	}
	#endif
	
	#ifdef CFG_AMP_MUTE_STATE_EN
	//睡眠，升级，开机完全前，有提示音开功放，否则关功放
	if(userVar.ProgramSate == PROGRAM_DEEPSLEEPING || !IsTimeOut(&userVar.AmpMuteDelayTimer))
	{
		if(GetRemindStateIsPlay())
		{
			AmpMuteSet(FALSE);
		}
		else
		{
			AmpMuteSet(TRUE);
		}
		return;
	}
	
	//多次检测是否有信号，是否开功放
	if(IsTimeOut(&userVar.AmpMuteTimer))
	{
		TimeOutSet(&userVar.AmpMuteTimer, 10);
		
		static uint8_t mute_pre = 0;
		static uint8_t mute_flag = 0;
	
		//一定要开功放的情况
		if(GetSystemMode() == ModeBtHfPlay)
		{		
			userVar.SilenceAmpMuteTime = 0;
			userVar.SilenceAmpUnmuteTime = 2;
			AmpMuteSet(FALSE);
			return ;
		}

		if(GetRemindStateIsPlay())
		{
			userVar.SilenceAmpMuteTime = 0;
			userVar.SilenceAmpUnmuteTime = 2;
		}
		else if(IsAudioPlayerMute())
		{
			userVar.SilenceAmpMuteTime++;
			userVar.SilenceAmpUnmuteTime = 0;
		}
		#ifdef CFG_APP_BT_MODE_EN
		else if(userVar.BtPlay)
		{
			userVar.SilenceAmpMuteTime++;
			userVar.SilenceAmpUnmuteTime = 0;
		}
		#endif
		else if(GetAudioSdct(MUSIC_VOL_TYPE) < SILENCE_THRESHOLD1 && GetAudioSdct(MIC_VOL_TYPE) < SILENCE_THRESHOLD1)
		{
			userVar.SilenceAmpMuteTime++;
			userVar.SilenceAmpUnmuteTime = 0;
		}
		else
		{
			userVar.SilenceAmpMuteTime = 0;
			userVar.SilenceAmpUnmuteTime++;
		}
		
		if(userVar.SilenceAmpUnmuteTime > 1)
		{
			mute_flag = 0;
			userVar.SilenceAmpUnmuteTime = 2;
		}
		else if(userVar.SilenceAmpMuteTime > 150)
		{
			mute_flag = 1;
			userVar.SilenceAmpMuteTime = 151;
		}

		if(mute_pre != mute_flag)
		{
			mute_pre = mute_flag;
			DBG("%s FLAG %d -- %d - %d\n", GetModeNameStr(GetSystemMode()), mute_flag, 
				GetAudioSdct(MUSIC_VOL_TYPE), GetAudioSdct(MIC_VOL_TYPE));
		}

		if(mute_flag)
		{
			AmpMuteSet(TRUE);
		}
		else
		{
			AmpMuteSet(FALSE);
		}
	}
	#endif
}

//检测是否有音乐播放
void CheckSignal(void)
{
	#ifdef CFG_CHECK_SIGNAL_EN
	if(IsAudioPlayerMute() || AudioMusicVolGet() <= 0
		#ifdef CFG_AMP_MUTE_STATE_EN
		|| userVar.AmpMuteFlag
		#endif
		)
	{
		userVar.if_music_play = FALSE;
	}
	else if(GetAudioSdct(MUSIC_VOL_TYPE) > SILENCE_THRESHOLD1)
	{
		//有信号
		userVar.music_play_count = 0;
		userVar.if_music_play = TRUE;
		GetLedSwitchTime1(0);
	}
	else
	{
		userVar.music_play_count++;
		if (userVar.music_play_count > 10)
		{
			userVar.music_play_count = 10;
			userVar.if_music_play = FALSE;
		}
	}
	#endif

	//某些模式无暂停的，用清空音乐通道替代
	#ifdef CFG_AUX_MUTE_EN
	if(!userVar.AuxPlayFlag && GetSystemMode() != ModeLineAudioPlay)
	{
		userVar.AuxPlayFlag = 1;
	}
	#endif

	#ifdef CFG_OPT_MUTE_EN
	if(!userVar.OpticalPlayFlag && GetSystemMode() != ModeOpticalAudioPlay)
	{
		userVar.OpticalPlayFlag = 1;
	}
	#endif

	#ifdef CFG_COA_MUTE_EN
	if(!userVar.CoaxialPlayFlag && GetSystemMode() != ModeCoaxialAudioPlay)
	{
		userVar.CoaxialPlayFlag = 1;
	}
	#endif

	#ifdef CFG_HDMI_MUTE_EN
	if(!userVar.HdmiPlayFlag && GetSystemMode() != ModeHdmiAudioPlay)
	{
		userVar.HdmiPlayFlag = 1;
	}
	#endif
	
	#ifdef CFG_RADIOIN_MUTE_EN
	if((!userVar.RadioPlayFlag && GetSystemMode() != ModeRadioAudioPlay))
	{
		userVar.RadioPlayFlag = 1;
	}
	#endif
}



