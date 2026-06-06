#include <string.h>
#include "main_task.h"
#include "remind_sound.h"
#include "audio_effect.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "irqn.h"
#include "reset.h"
#include "rtc.h"
#include "bt_config.h"
#include "bb_api.h"
#include "bt_manager.h"
#include "bt_stack_service.h"
#include "audio_core_service.h"
#include "pmu_powerkey.h"
#include "watchdog.h"

#define MAIN_APP_TASK_SLEEP_PRIO		6 //进入deepsleep 需要相对其他task最高优先级。

#ifdef CFG_APP_IDLE_MODE_EN

#define IDLE_NOT_REQUIRED_MODE	(BIT(ModeIdle) | BIT(ModeBtHfPlay))	

#ifdef CFG_FUNC_REMIND_SOUND_EN
#define CFG_FUNC_REMIND_DEEPSLEEP		//Deepsleep 前播放(sys)提示音
#define CFG_FUNC_REMIND_WAKEUP			//Deepsleep 唤醒以后播放提示音
#endif

extern bool ModeCommonInit(void);
extern void SystemTimerInit(void);

#if defined(CFG_APP_IDLE_MODE_EN)&&defined(CFG_FUNC_REMIND_SOUND_EN)
extern volatile uint32_t gIdleRemindSoundTimeOutTimer;
#endif

#define POWER_ON_IDLE				0
#define NEED_POWER_ON				1
#define WAIT_POWER_ON_REMIND_SOUND	2
#define ENTER_POWER_ON				3

static struct
{
	uint16_t	AutoPowerOnState 	:	2;

#if	defined(CFG_IDLE_MODE_POWER_KEY)
	uint16_t	PowerKeyWakeUpCheckFlag 	: 	1; //上电/powerkey唤醒检查
#endif
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	uint16_t	RemindSoundFlag 	:	1;
	uint16_t	DeepSleepFlag 	:	1;
#endif		
	uint16_t	FristPowerOnFlag 	:	1;

	SysModeNumber	SavePrevMode;
}IdleMode;

void PowerOnRemindSound(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	RemindSoundClearPlay();
#if !defined(CFG_FUNC_REMIND_WAKEUP) && defined(CFG_IDLE_MODE_DEEP_SLEEP)
	if(IdleMode.DeepSleepFlag)
	{
		IdleMode.DeepSleepFlag = FALSE;
		return; 		
	}
#endif	
	IdleMode.RemindSoundFlag = RemindSoundServiceItemRequest(SOUND_REMIND_KAIJI, REMIND_ATTR_NEED_CLEAR_INTTERRUPT_PLAY|REMIND_PRIO_SYS);
	IdleMode.DeepSleepFlag = FALSE;
	gIdleRemindSoundTimeOutTimer = 0;
#endif
}

void PowerDownRemindSound(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN

#if !defined(CFG_FUNC_REMIND_DEEPSLEEP) && defined(CFG_IDLE_MODE_DEEP_SLEEP)
	if(SoftFlagGet(SoftFlagIdleModeEnterSleep))
	{
		return;			
	}
#endif	
	IdleMode.RemindSoundFlag = RemindSoundServiceItemRequest(SOUND_REMIND_GUANJI, REMIND_ATTR_NEED_CLEAR_INTTERRUPT_PLAY|REMIND_PRIO_SYS);
	gIdleRemindSoundTimeOutTimer = 0;
	RemindSoundItemRequestDisable();
#endif
}

bool GetPowerRemindSoundPlayEnd(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	if(IdleMode.RemindSoundFlag)
	{
		if(!RemindSoundIsPlay() || gIdleRemindSoundTimeOutTimer > 2000)
			IdleMode.RemindSoundFlag = FALSE;
		return TRUE;
	}
#endif
	return FALSE;
}

void IdleModeConfig(void)
{
	IdleMode.AutoPowerOnState	= NEED_POWER_ON;
#ifdef CFG_IDLE_MODE_POWER_KEY
#if 0
	PMU_PowerKey8SResetSet();
#else
	PMU_PowerKeyDisable();
#endif
	SystemPowerKeyIdleModeInit();
#endif
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	IdleMode.RemindSoundFlag 	= FALSE;
	IdleMode.DeepSleepFlag = FALSE;
#endif
	IdleMode.FristPowerOnFlag = TRUE;
}

void IdleModeConfigPowerOn(void)
{
	userVar.ProgramSate = PROGRAM_RUNNING;
	IdleMode.AutoPowerOnState	= NEED_POWER_ON;
	#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	LedEffectSwitch(LED_MODE_POWER_ON, FALSE, FALSE);
	#endif

	//For OTG check
#if defined(CFG_FUNC_UDISK_DETECT)
#if defined(CFG_FUNC_USB_DEVICE_DETECT)
 	OTG_PortSetDetectMode(1,1);
#else
 	OTG_PortSetDetectMode(1,0);
#endif
#else
#if defined(CFG_FUNC_USB_DEVICE_DETECT)
 	OTG_PortSetDetectMode(0,1);
#endif
#endif
}

void IdleModeConfigPowerOff(void)
{
	userVar.ProgramSate = PROGRAM_POWERON_SLEEPING;
	IdleMode.FristPowerOnFlag = FALSE;
}

void IdleModeSleepConfig(void)
{
	APP_DBG("上电进入睡眠\n");
	SoftFlagDeregister(SoftFlagWakeUpSouceIsCEC);
	userVar.ProgramSate = PROGRAM_POWERON_SLEEPING;
	IdleMode.AutoPowerOnState	= POWER_ON_IDLE;
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	IdleMode.RemindSoundFlag 	= FALSE;
	IdleMode.DeepSleepFlag = FALSE;
#endif
	IdleMode.FristPowerOnFlag = TRUE;
}

bool IdleModeInit(void)
{
	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	//Core Process
	//AudioCoreProcessConfig((void*)AudioNoAppProcess);
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioMusicProcess);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif

	DBG("Idle Mode Init\n");
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if((GetA2dpState(BtCurIndex_Get()) >= BT_A2DP_STATE_CONNECTED)
		|| (GetHfpState(BtCurIndex_Get()) >= BT_HFP_STATE_CONNECTED)
		|| (GetAvrcpState(BtCurIndex_Get()) >= BT_AVRCP_STATE_CONNECTED))
	{
		//手动断开
		BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_DISCONNECT_DEV_CTRL);
	}
#endif	

#ifdef  CFG_FUNC_REMIND_SOUND_EN	
	if(!IdleMode.FristPowerOnFlag)
	{
		PowerDownRemindSound();
	}
#endif	
	DBG("Idle Mode run\n");
#ifdef CFG_APP_BT_MODE_EN
	if((!IdleMode.FristPowerOnFlag) && sys_parameter.bt_BackgroundType == BT_BACKGROUND_POWER_ON)
		BtFastPowerOff();
#endif
	IdleMode.FristPowerOnFlag = FALSE;
	return TRUE;
}


bool IdleModeDeinit(void)
{

	DBG("Idle Mode Deinit\n");

	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	
	PauseAuidoCore();
	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig((void*)AudioNoAppProcess);
	ModeCommonDeinit();//通路全部释放
#ifdef	CFG_IDLE_MODE_POWER_KEY
	SoftFlagDeregister(SoftFlagIdleModeEnterPowerDown);
#endif				
#ifdef CFG_IDLE_MODE_DEEP_SLEEP
	SoftFlagDeregister(SoftFlagIdleModeEnterSleep);
#endif	
#ifdef CFG_SOFT_POWER_KEY_EN
	SoftFlagDeregister(SoftFlagIdleModeEnterSoftPower);
#endif
#ifdef CFG_APP_BT_MODE_EN
	if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
	{
		BtStackServiceMsgSend(MSG_BTSTACK_RUN_START);
		if(sys_parameter.bt_BackgroundType == BT_BACKGROUND_POWER_ON)
			BtFastPowerOn();
	}
#endif
	return TRUE;
}

void SendEnterIdleModeMsg(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId = MSG_ENTER_IDLE_MODE;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

void SendQuitIdleModeMsg(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId = MSG_QUIT_IDLE_MODE;
	MessageSend(GetMainMessageHandle(), &msgSend);	
}

extern bool GetRemindSoundItemDisable(void);
extern void IdlePrevModeSet(SysModeNumber mode);
void IdleModeRun(uint16_t msgId)
{
#ifdef CFG_IDLE_MODE_POWER_KEY
	if(SoftFlagGet(SoftFlagIdleModeEnterPowerDown)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
	 && (!GetPowerRemindSoundPlayEnd())
#endif
	 )
	{
		extern bool GetPowerKeyVaildState(void);
		SoftFlagDeregister(SoftFlagIdleModeEnterPowerDown);
		if(GetPowerKeyVaildState())
		{
			SystemPowerDown();
		}
		else
		{
			PMU_PowerKeyStateClear();
			PMU_PowerKeyShortPressStateClear();
			PMU_PowerKeyLongPressStateClear();
			IdleMode.AutoPowerOnState = NEED_POWER_ON;
		}
	}
#endif	

#ifdef CFG_IDLE_MODE_DEEP_SLEEP
	if(SoftFlagGet(SoftFlagIdleModeEnterSleep)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
		&& (!GetPowerRemindSoundPlayEnd())
#endif
	)
	{

		SoftFlagDeregister(SoftFlagIdleModeEnterSleep);
	#ifdef  CFG_FUNC_REMIND_SOUND_EN
		IdleMode.DeepSleepFlag = TRUE;
	#endif	
		PauseAuidoCore();
		UBaseType_t pri = uxTaskPriorityGet(NULL);
#if defined(CFG_APP_BT_MODE_EN)
		if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
		{
			vTaskDelay(50);
			//bb reset
			BtResetAndKill(TRUE);
		}
#endif
		vTaskPrioritySet(NULL, MAIN_APP_TASK_SLEEP_PRIO);//设定最高优先级
		
 		NVIC_DisableIRQ(Timer2_IRQn);

#ifdef CFG_FUNC_DEBUG_USE_TIMER
		extern void PrintfAllInBuffer(void);
		PrintfAllInBuffer();
#endif
	
		DeepSleeping();
		
		SystemTimerInit();

#ifdef CFG_FUNC_DEBUG_USE_TIMER
		{
			extern uint8_t uart_switch ;
			uart_switch = 1;
		}
#endif

#if defined(CFG_APP_BT_MODE_EN)
		if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
		{
			WDG_Feed();
			// BtStackServiceStart();
            BtPowerOn();
            WDG_Feed();
		}
#endif
#ifdef CFG_RES_IR_KEY_SCAN
		extern void IRKeyInit(void);
		IRKeyInit();//清除多余的按键
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
		{
			extern void AudioI2sOutParamsSet(void);
			AudioI2sOutParamsSet();
		}
#endif
		vTaskPrioritySet(NULL, pri);
		AudioCoreServiceResume();
		osTaskDelay(10);// for printf 
	
		IdleMode.AutoPowerOnState = NEED_POWER_ON;
	}
#endif	

#ifdef CFG_SOFT_POWER_KEY_EN
	if(SoftFlagGet(SoftFlagIdleModeEnterSoftPower)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
		&& (!GetPowerRemindSoundPlayEnd())
#endif
	)
	{
		extern void SoftKeyPowerOff(void);
		SoftFlagDeregister(SoftFlagIdleModeEnterSoftPower);
		SoftKeyPowerOff();
		IdleMode.AutoPowerOnState = POWER_ON_IDLE;
	}
#endif
	switch(IdleMode.AutoPowerOnState)
	{
		case NEED_POWER_ON:
			PowerOnRemindSound();
			IdleMode.AutoPowerOnState = WAIT_POWER_ON_REMIND_SOUND;
			break;
		case WAIT_POWER_ON_REMIND_SOUND:
			if(!GetPowerRemindSoundPlayEnd())
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	
			break;
		case ENTER_POWER_ON:
			IdleMode.AutoPowerOnState = POWER_ON_IDLE;
			SendQuitIdleModeMsg();
			break;
		case POWER_ON_IDLE:
			break;
		default:
			IdleMode.AutoPowerOnState = POWER_ON_IDLE;
			break;
	}

	switch(msgId)
	{
		case MSG_POWER:
		case MSG_POWERDOWN:
		case MSG_DEEPSLEEP:
#ifdef CFG_SOFT_POWER_KEY_EN
		case MSG_SOFT_POWER:
#endif
#ifdef CFG_IDLE_MODE_POWER_KEY
			if(SoftFlagGet(SoftFlagIdleModeEnterPowerDown)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
			 && (!GetPowerRemindSoundPlayEnd())
#endif
			 )
				break;
#endif
			if(IdleMode.AutoPowerOnState == POWER_ON_IDLE && (!GetPowerRemindSoundPlayEnd()))
				IdleMode.AutoPowerOnState = NEED_POWER_ON;
			break;
		case MSG_DEVICE_SERVICE_U_DISK_IN:	//插U盘开机
			IdlePrevModeSet(ModeUDiskAudioPlay);
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(GetRemindSoundItemDisable())
				IdleMode.AutoPowerOnState = NEED_POWER_ON;	//播放开机提示音，然后进入模式
			else if(RemindSoundIsPlay())	//已经在播放提示音，等待播放完
				IdleMode.AutoPowerOnState = WAIT_POWER_ON_REMIND_SOUND;
			else
#endif
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	//直接进入模式，不需要播放开机提示音
			break;
		case MSG_DEVICE_SERVICE_CARD_IN:
			break;
		case MSG_DEVICE_SERVICE_USB_DEVICE_IN://插声卡开机
			IdlePrevModeSet(ModeUsbDevicePlay);
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(GetRemindSoundItemDisable())
				IdleMode.AutoPowerOnState = NEED_POWER_ON;	//播放开机提示音，然后进入模式
			else
#endif
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	//直接进入模式，不需要播放开机提示音
			break;
		default:
			CommonMsgProccess(msgId);
			break;
	}

}


void IdlePrevModeSet(SysModeNumber mode)
{
	IdleMode.SavePrevMode = mode;
}

SysModeNumber IdlePrevModeGet(void)
{
	return IdleMode.SavePrevMode;
}

extern osMutexId SysModeMutex;
void IdleModeEnter(void)
{
	if(GetSysModeState(ModeIdle) == ModeStateInit || GetSysModeState(ModeIdle) == ModeStateRunning )
		return;
	osMutexLock(SysModeMutex);
	if(IDLE_NOT_REQUIRED_MODE & BIT(mainAppCt.SysCurrentMode))
		IdleMode.SavePrevMode = mainAppCt.SysPrevMode;
	else
		IdleMode.SavePrevMode = mainAppCt.SysCurrentMode;
	if(GetSysModeState(ModeIdle) == ModeStateSusend)
	{
		SetSysModeState(ModeIdle,ModeStateReady);
	}
	SysModeEnter(ModeIdle);
	osMutexUnlock(SysModeMutex);
	APP_DBG("enter idle mode\n");
}

void IdleModeExit(void)
{
	if(IDLE_NOT_REQUIRED_MODE & BIT(IdleMode.SavePrevMode))
		IdleMode.SavePrevMode = ModeBtAudioPlay;
	IdleMode.SavePrevMode = ModeLineAudioPlay;
	osMutexLock(SysModeMutex);
	SysModeEnter(IdleMode.SavePrevMode);
	osMutexUnlock(SysModeMutex);
	APP_DBG("exit idle mode\n");
}

#else

void IdlePrevModeSet(SysModeNumber mode)
{
	mode = mode;
}

void IdleModeEnter(void)
{

}

void IdleModeExit(void)
{

}

#endif


