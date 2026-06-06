/**
 **************************************************************************************
 * @file    main_task.c
 * @brief   Program Entry 
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
#include "main_task.h"
#include "dma.h"
#include "timer.h"
#include "irqn.h"
#include "watchdog.h"
//#include "deepsleep.h"
#include "breakpoint.h"
#include "remind_sound.h"
#include "audio_vol.h"
#include "device_detect.h"
#include "otg_detect.h"
#include "sadc_interface.h"
#include "ctrlvars.h"
//services
#include "shell.h"
#include "audio_core_service.h"
#include "bt_stack_service.h"
#include "wireless_service.h"
#include "bb_api.h"
#include "bt_config.h"
#include "bt_app_ddb_info.h"
#include "bt_hf_mode.h"
#include "idle_mode.h"
#include "bt_app_connect.h"
#include <components/soft_watchdog/soft_watch_dog.h>
#include "can_func.h"

MainAppContext	mainAppCt;
#ifdef CFG_RES_IR_NUMBERKEY
bool Number_select_flag = 0;
uint16_t Number_value = 0;
TIMER Number_selectTimer;
#endif


extern void CtrlVarsInit(void);
extern void report_up_grate(void);
extern volatile uint32_t gInsertEventDelayActTimer;
#if FLASH_BOOT_EN
void start_up_grate(uint32_t UpdateResource);
#endif
extern void PowerOnModeGenerate(void *BpSysInfo);
#ifdef CFG_APP_HDMIIN_MODE_EN
extern void HDMI_CEC_ActivePowerOff(uint32_t timeout_value);
extern void	HDMI_CEC_DDC_Init(void);
#endif

#define MAIN_APP_TASK_STACK_SIZE		640//512//1024
#ifdef	CFG_FUNC_OPEN_SLOW_DEVICE_TASK
#define MAIN_APP_TASK_PRIO				4
#define MAIN_APP_MSG_TIMEOUT			10	
#else
#define MAIN_APP_TASK_PRIO				3
#define MAIN_APP_MSG_TIMEOUT			1	
#endif
#define MAIN_APP_TASK_SLEEP_PRIO		6 //进入deepsleep 需要相对其他task最高优先级。
#define MAIN_NUM_MESSAGE_QUEUE			20
#define SHELL_TASK_STACK_SIZE			512//1024
#define SHELL_TASK_PRIO					2


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
#ifdef PWM5_LED
	PERIPHERAL_ID_TIMER5,
#endif
#ifdef PWM6_LED
	PERIPHERAL_ID_TIMER6,
#endif
#ifdef PWM7_LED
	PERIPHERAL_ID_TIMER7,
#endif
#ifdef PWM8_LED
	PERIPHERAL_ID_TIMER8,
#endif

#ifdef CFG_DUMP_DEBUG_EN
	CFG_DUMP_UART_TX_DMA_CHANNEL,
#endif

	PERIPHERAL_ID_SDIO_RX,
};

static void MainAppInit(void)
{
	memset(&mainAppCt, 0, sizeof(MainAppContext));
#if FLASH_BOOT_EN
    report_up_grate();
#endif
	mainAppCt.msgHandle = MessageRegister(MAIN_NUM_MESSAGE_QUEUE);
	mainAppCt.state = TaskStateCreating;
	mainAppCt.SysCurrentMode = ModeIdle;
	mainAppCt.SysPrevMode = ModeIdle;
}

static void SysVarInit(void)
{
	int16_t i;

#ifdef CFG_FUNC_BREAKPOINT_EN
	BP_SYS_INFO *pBpSysInfo = NULL;

	pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);

#ifdef  VD51_REDMINE_13199
	SetRgbLedMode(pBpSysInfo->rgb_mode);
#endif

#ifdef UserSoftPower
	UserVar_Breakpoint_Read(pBpSysInfo);
#endif

	mainAppCt.EffectMode = pBpSysInfo->EffectMode;
	mainAppCt.EffectMode = EFFECT_MODE_MUSIC;
	APP_DBG("EffectMode:%d,%d\n", mainAppCt.EffectMode, pBpSysInfo->EffectMode);

	MusicVolume = pBpSysInfo->MusicVolume;
	if((MusicVolume > CFG_PARA_MAX_VOLUME_NUM) || (MusicVolume <= 0))
	{
		MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	}
	MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	APP_DBG("MusicVolume:%d,%d\n", MusicVolume, pBpSysInfo->MusicVolume);
	
	MicVolume = pBpSysInfo->MicVolume;
	if((MicVolume > CFG_PARA_MAX_VOLUME_NUM) || (MicVolume <= 0))
	{
		MicVolume = CFG_PARA_MAX_VOLUME_NUM;
	}
	MicVolume = CFG_PARA_MAX_VOLUME_NUM;
	APP_DBG("MicVolume:%d,%d\n", MicVolume, pBpSysInfo->MicVolume);

	#ifdef CFG_APP_BT_MODE_EN
	mainAppCt.HfVolume = pBpSysInfo->HfVolume;
	if((mainAppCt.HfVolume > CFG_PARA_MAX_VOLUME_NUM) || (mainAppCt.HfVolume <= 0))
	{
		mainAppCt.HfVolume = CFG_PARA_MAX_VOLUME_NUM;
	}
	APP_DBG("HfVolume:%d,%d\n", mainAppCt.HfVolume, pBpSysInfo->HfVolume);
	#endif
	
#ifdef CFG_FUNC_MUSIC_EQ_MODE_EN
	EqMode = pBpSysInfo->EqMode;
	if(EqMode > EQ_MODE_VOCAL_BOOST)
	{
		EqMode = EQ_MODE_FLAT;
	}
//	EqModeSet(mainAppCt.EqMode);
	#ifdef CFG_FUNC_EQMODE_FADIN_FADOUT_EN    
	mainAppCt.EqModeBak = mainAppCt.EqMode;
	mainAppCt.EqModeFadeIn = 0;
	mainAppCt.eqSwitchFlag = 0;
	#endif
	APP_DBG("EqMode:%d,%d\n", EqMode, pBpSysInfo->EqMode);
#endif
#ifdef CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC
	ReverbStep = pBpSysInfo->ReverbStep;
    if((ReverbStep > MAX_MIC_DIG_STEP) || (ReverbStep <= 0))
	{
    	ReverbStep = MAX_MIC_DIG_STEP;
	}
	APP_DBG("ReverbStep:%d,%d\n", ReverbStep, pBpSysInfo->ReverbStep);
#endif
#ifdef CFG_FUNC_MIC_TREB_BASS_EN
    MicBassStep = pBpSysInfo->MicBassStep;
    if((MicBassStep > MAX_MUSIC_DIG_STEP) || (MicBassStep <= 0))
	{
		MicBassStep = 7;
	}
    MicTrebStep = pBpSysInfo->MicTrebStep;
    if((MicTrebStep > MAX_MUSIC_DIG_STEP) || (MicTrebStep <= 0))
	{
		MicTrebStep = 7;
	}
	APP_DBG("MicTrebStep:%d,%d\n", MicTrebStep, pBpSysInfo->MicTrebStep);
	APP_DBG("MicBassStep:%d,%d\n", MicBassStep, pBpSysInfo->MicBassStep);
#endif
#ifdef CFG_FUNC_MUSIC_TREB_BASS_EN	
    MusicBassStep = pBpSysInfo->MusicBassStep;
    if((MusicBassStep > MAX_MUSIC_DIG_STEP) || (MusicBassStep <= 0))
	{
		MusicBassStep = 7;
	}
    MusicTrebStep = pBpSysInfo->MusicTrebStep;
    if((MusicTrebStep > MAX_MUSIC_DIG_STEP) || (MusicTrebStep <= 0))
	{
		MusicTrebStep = 7;
	}
	APP_DBG("MusicTrebStep:%d,%d\n", MusicTrebStep, pBpSysInfo->MusicTrebStep);
	APP_DBG("MusicBassStep:%d,%d\n", MusicBassStep, pBpSysInfo->MusicBassStep);
#endif

#else
	//mainAppCt.appBackupMode = ModeBtAudioPlay;		  
	MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	#ifdef CFG_APP_BT_MODE_EN
	mainAppCt.HfVolume = CFG_PARA_MAX_VOLUME_NUM;
	#endif
	
#ifdef CFG_FUNC_EFFECT_BYPASS_EN
	mainAppCt.EffectMode = EFFECT_MODE_BYPASS;
#else
#ifdef CFG_FUNC_MIC_KARAOKE_EN
	mainAppCt.EffectMode = EFFECT_MODE_HunXiang;
#else
	mainAppCt.EffectMode = EFFECT_MODE_MIC;
#endif
#endif

	MicVolume = CFG_PARA_MAX_VOLUME_NUM;
	
#ifdef CFG_FUNC_MUSIC_EQ_MODE_EN
	mainAppCt.EqMode = EQ_MODE_FLAT;
 	#ifdef CFG_FUNC_EQMODE_FADIN_FADOUT_EN    
    mainAppCt.EqModeBak = mainAppCt.EqMode;
	mainAppCt.eqSwitchFlag = 0;
	#endif
#endif
	//mainAppCt.ReverbStep = MAX_MIC_DIG_STEP;
#ifdef CFG_FUNC_MIC_TREB_BASS_EN
	MicBassStep = 7;
	MicTrebStep = 7;
#endif
#ifdef CFG_FUNC_MUSIC_TREB_BASS_EN	
	MusicBassStep = 7;
	MusicTrebStep = 7;
#endif	

#endif

#ifdef CFG_APP_HDMIIN_MODE_EN
	mainAppCt.hdmiArcOnFlg = 1;
	mainAppCt.hdmiResetFlg = 0;
#endif

#if defined(CFG_APP_BT_MODE_EN) && defined(CFG_WIRELESS_OUT_ON) //CFG_WIRELESS_EN
	SetSysModeState(ModeBtAudioPlay, ModeStateReady);//ModeStateSusend
#endif

#ifdef CFG_FUNC_BREAKPOINT_EN
#ifdef CFG_APP_BT_MODE_EN
	if(SoftFlagGet(SoftFlagUpgradeOK))
	{
		pBpSysInfo->CurModuleId = ModeBtAudioPlay;
		DBG("u or sd upgrade ok ,set mode to ModeBtAudioPlay \n");
	}
#endif
	PowerOnModeGenerate((void *)pBpSysInfo);
#else
	PowerOnModeGenerate(NULL);
#endif


    for(i = 0; i < AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		mainAppCt.gSysVol.AudioSinkVol[i] = CFG_PARA_MAX_VOLUME_NUM;
	}

	for(i = 0; i < AUDIO_CORE_SOURCE_MAX_NUM; i++)
	{
		if(i == MIC_SOURCE_NUM)
		{
			mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
		}
		else if(i == APP_SOURCE_NUM)
		{
			mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
		}
#ifdef CFG_FUNC_REMIND_SOUND_EN
		else if(i == REMIND_SOURCE_NUM)
		{
			#if CFG_PARAM_FIXED_REMIND_VOL
			mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
			#else
			mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = MusicVolume;
			#endif
		}
#endif
		else
		{
			mainAppCt.gSysVol.AudioSourceVol[i] = CFG_PARA_MAX_VOLUME_NUM;		
		}
	}
	mainAppCt.gSysVol.MuteFlag = TRUE;	
	
	#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
	mainAppCt.Silence_Power_Off_Time = 0;
	#endif
}

void SystemTimerInit(void)
{
 	Timer_Config(TIMER2,1000,0);
 	Timer_Start(TIMER2);
 	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
	NVIC_EnableIRQ(Timer2_IRQn);
}

static void SystemInit(void)
{
#ifndef LOSSLESS_DECODER_HIGH_RESOLUTION
	//开启高采样率解码，去掉U盘读数据API里面的延迟
	DelayMsFunc = (DelayMsFunction)vTaskDelay; //提高Os条件下驱动层延时函数精度，非OS默认使用DelayMs
#endif
	DMA_ChannelAllocTableSet((uint8_t*)DmaChannelMap);
#ifdef CFG_DUMP_DEBUG_EN
	DumpUartConfig(TRUE);
#endif
	SarADC_Init();

	PowerOn();

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

 	SystemTimerInit();

#ifdef CFG_FUNC_BREAKPOINT_EN
 	BP_LoadInfo();
#endif

#ifdef CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
 	AudioEffect_CheckFlashEffectParam();
#endif

	SysVarInit();
	
#ifdef CFG_FUNC_BT_OTA_EN
	SoftFlagDeregister((~(SoftFlagUpgradeOK|SoftFlagBtOtaUpgradeOK))&SoftFlagMask);
#else
	SoftFlagDeregister((~SoftFlagUpgradeOK)&SoftFlagMask);
#endif

#ifdef CFG_APP_HDMIIN_MODE_EN
	HDMI_CEC_DDC_Init();
#endif
	///////////////////////////////AudioCore/////////////////////////////////////////
	memset((AudioCoreContext*)&AudioCore, 0, sizeof(AudioCoreContext));

	memset((AudioEffectContext*)&AudioEffect, 0, sizeof(AudioEffectContext));

	CtrlVarsInit();//音频系统硬件变量初始化，系统变量初始化
	
	////Audio Core音量配置
	SystemVolSet();

	AudioCoreServiceCreate(mainAppCt.msgHandle);
#ifdef CFG_FUNC_REMIND_SOUND_EN	
	RemindSoundInit();
#endif

#ifdef CFG_FUNC_ALARM_EN
	mainAppCt.AlarmFlag = FALSE;
#endif

//上电蓝牙模式
#if 0//defined(CFG_WIRELESS_EN) && defined(CFG_WIRELESS_OUT_ON)
	DBG("Wireless stack @pwr on!\n");
	WirelessServiceStart();
#elif defined(CFG_APP_BT_MODE_EN)
	//将蓝牙任务创建移至此处,以便优先申请协议栈使用的内存空间,不影响其他的任务; 开机睡眠时，蓝牙stack再次开，避免上电马上退出。
	if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
		BtStackServiceStart();//蓝牙设备驱动serivce 启动失败时，目前是挂起，无同步消息。
//IRKeyInit();//clk源被改？
#endif

#ifdef CFG_FUNC_CAN_DEMO_EN
	CAN_FuncInit();
#endif
	DeviceServiceInit();
	APP_DBG("MainApp:run\n");

	IdleModeSleepConfig();
}

static void PublicDetect(void)
{
#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
		switch(GetSystemMode())
		{
			// Idle,Slave,HFP,USB Audio不省电关机
			case ModeIdle:
			case ModeTwsSlavePlay:
			case ModeBtHfPlay:
			case ModeUsbDevicePlay:
			mainAppCt.Silence_Power_Off_Time = 0;
			break;

			// BT 连上蓝牙不关机
			case ModeBtAudioPlay:
			if(btManager.btLinkState)
				mainAppCt.Silence_Power_Off_Time = 0;
			break;

			default:
			break;
		}

        if(mainAppCt.Silence_Power_Off_Time >= SILENCE_POWER_OFF_DELAY_TIME)
		{
			mainAppCt.Silence_Power_Off_Time = 0;
			APP_DBG("Silence Power Off!!\n");
			PowerOffMessage();
        }
#endif
}

static void PublicMsgPross(MessageContext msg)
{
	switch(msg.msgId)
	{
		case MSG_AUDIO_CORE_SERVICE_CREATED:	
			APP_DBG("MainApp:AudioCore service created\n");
			AudioCoreServiceStart();
			mainAppCt.state = TaskStateReady;
			break;
		
		case MSG_AUDIO_CORE_SERVICE_STARTED:
			APP_DBG("MainApp:AudioCore service started\n");
			SysModeTaskCreat();
	#ifdef	CFG_FUNC_OPEN_SLOW_DEVICE_TASK
			{
				extern	void CreatSlowDeviceTask(void);
				CreatSlowDeviceTask();
			}
	#endif
			mainAppCt.state = TaskStateRunning;
			break;
#if FLASH_BOOT_EN
		case MSG_UPDATE:
			//if(SoftFlagGet(SoftFlagUpgradeOK))break;
			#ifdef FUNC_UPDATE_CONTROL
			APP_DBG("MainApp:UPDATE MSG\n");
			//设备经过播放，预搜索mva包登记，可升级。拔出时取消登记。
			if(SoftFlagGet(SoftFlagMvaInCard) && GetSystemMode() == ModeCardAudioPlay)
			{
				APP_DBG("MainApp:updata file exist in Card\n");
				start_up_grate(SysResourceCard);
			}
			else if(SoftFlagGet(SoftFlagMvaInUDisk) && GetSystemMode() == ModeUDiskAudioPlay)
			{
				APP_DBG("MainApp:updata file exist in Udisk\n");
				start_up_grate(SysResourceUDisk);
			}
			#endif
			break;
#endif		

#ifdef CFG_APP_IDLE_MODE_EN		
		case MSG_POWER:
		case MSG_POWERDOWN:
		case MSG_DEEPSLEEP:
#ifdef CFG_SOFT_POWER_KEY_EN
		case MSG_SOFT_POWER:
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
			if(GetSystemMode() == ModeHdmiAudioPlay)
			{
				if(SoftFlagGet(SoftFlagDeepSleepMsgIsFromTV) == 0)//非电视端发来的关机
				{
					HDMI_CEC_ActivePowerOff(200);
				}
				SoftFlagDeregister(SoftFlagDeepSleepMsgIsFromTV);
			}
#endif
			if(GetSystemMode() != ModeIdle)
			{			
				if (msg.msgId == MSG_POWER){
					DBG("Main task MSG_POWER\n");
				}else if (msg.msgId == MSG_POWERDOWN){
					DBG("Main task MSG_POWERDOWN\n");
				}else if (msg.msgId == MSG_DEEPSLEEP){
					DBG("Main task MSG_DEEPSLEEP\n");
				}
				SendEnterIdleModeMsg();
			#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
				if(GetSystemMode() == ModeBtHfPlay)
				{
					BtHfModeExit();				
				}
				extern void SetsBtHfModeEnterFlag(uint32_t flag);
				SetsBtHfModeEnterFlag(0);
			#endif		
			
			#ifdef	CFG_IDLE_MODE_POWER_KEY
				if(msg.msgId == MSG_POWERDOWN)
				{
					SoftFlagRegister(SoftFlagIdleModeEnterPowerDown);
				}
			#endif				
			
			#ifdef CFG_IDLE_MODE_DEEP_SLEEP
				if(msg.msgId == MSG_DEEPSLEEP)
				{					
					SoftFlagRegister(SoftFlagIdleModeEnterSleep);
				}
			#endif	
			#ifdef CFG_SOFT_POWER_KEY_EN
				if(msg.msgId == MSG_SOFT_POWER)
				{
					SoftFlagRegister(SoftFlagIdleModeEnterSoftPower);
				}
			#endif
			}
			break;
#else
#ifdef CFG_SOFT_POWER_KEY_EN
		case MSG_SOFT_POWER:
			SoftKeyPowerOff();
			break;
#endif
#endif

#ifdef CFG_APP_BT_MODE_EN
		case MSG_BT_ENTER_DUT_MODE:
			BtEnterDutModeFunc();
			break;

		case MSG_BTSTACK_DEEPSLEEP:
			APP_DBG("MSG_BTSTACK_DEEPSLEEP\n");
			break;

		case MSG_BTSTACK_BB_ERROR:
			APP_DBG("bb and bt stack reset\n");
			BtResetAndKill(FALSE);
			//reset bb and bt stack
			BtStackServiceStart();

			//发起回连
			BtStackServiceMsgSend(MSG_BTSTACK_BB_ERROR_RESTART);

			break;
#ifdef CFG_FUNC_BT_OTA_EN
		case MSG_BT_START_OTA:
			APP_DBG("\nMSG_BT_START_OTA\n");
			start_up_grate(SysResourceBtOTA);
			break;
#endif
#if (BT_HFP_SUPPORT)
			case MSG_DEVICE_SERVICE_ENTER_BTHF_MODE:
			if(GetHfpState(BtCurIndex_Get()) >= BT_HFP_STATE_CONNECTED)
			{
				BtHfModeEnter();
			}
			break;
#endif
#ifdef BT_AUTO_ENTER_PLAY_MODE
		case MSG_BT_A2DP_STREAMING:
			//播放歌曲时,有模式切换需求,则在此消息中开始进行模式切换操作
			if((GetSystemMode() != ModeBtAudioPlay)&&(GetSystemMode() != ModeBtHfPlay))
			{
				MessageContext		msgSend;
				
				APP_DBG("Enter Bt Audio Play Mode...\n");
				//ResourceRegister(AppResourceBtPlay);
				
				// Send message to main app
				msgSend.msgId		= MSG_DEVICE_SERVICE_BTPLAY_IN;
				MessageSend(GetMainMessageHandle(), &msgSend);
			}
			break;
#endif
#endif//#ifdef CFG_APP_BT_MODE_EN
#if defined(CFG_WIRELESS_EN) && (!defined(CFG_WIRELESS_OUT_ON) || defined(CFG_RF_TO_BT))
		case MSG_WIRELESS://stack  create
			if(GetWirelessServiceTaskHandle() == NULL)
			{
	#ifdef CFG_APP_BT_MODE_EN
				APP_DBG("bt stack reset\n");
				BtResetAndKill(FALSE);
	#endif
				DBG("Wireless stack!\n");
				WirelessServiceStart();

				MainTaskMsgSend(MSG_MODE);
			}
			break;

		case MSG_WIRELESS_EXIT: //stack exit
			DBG("MSG_WIRELESS_EXIT\n");
			if(WirelessRFInited())
			{
				WirelessServiceKill();
				MessageContext		newmsg;
				newmsg.msgId = MSG_WIRELESS_EXITED;
				MessageSend(mainAppCt.msgHandle, &newmsg);
#ifdef CFG_APP_BT_MODE_EN
				if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
				{
					//reset bb and bt stack
					BtStackServiceStart();
					BtStackServiceMsgSend(MSG_BTSTACK_RUN_START);
				#ifdef 	CFG_RF_TO_BT
					if(GetSysModeState(ModeBtAudioPlay) == ModeStateSusend)
					{
						SetSysModeState(ModeBtAudioPlay,ModeStateReady);
					}
				#endif
				}
#endif
			}

		break;
		//This case just for BT RF Test,BT canot resume.
		case MSG_WIRELESS_EXITED:
			DBG("MSG_WIRELESS_EXITED\n");
			MainTaskMsgSend(MSG_MODE);
	#if defined(CFG_RF_TO_BT)
			MessageContext		newmsg1;
			newmsg1.msgId = MSG_MODE;//MSG_BT_ENTER_DUT_MODE;
			MessageSend(mainAppCt.msgHandle, &newmsg1);
	#endif
			break;
#endif//CFG_WIRELESS_EN

		default:
#if defined(CFG_WIRELESS_EN)
			if(msg.msgId > MSG_WIRELESS_EVENT && msg.msgId < MSG_WIRELESS_EVENT_END)
			{
				WirelessServiceMsgSend(msg.msgId);
			}
#endif
#if	BT_SOURCE_SUPPORT
			BtSourcePublicMsgPross(msg.msgId);
#endif
			break;
	}		
}

static void MainAppTaskEntrance(void * param)
{
	MessageContext		msg;

	SystemInit();
	while(1)
	{
		MessageRecv(mainAppCt.msgHandle, &msg, MAIN_APP_MSG_TIMEOUT);
		PublicDetect();
		PublicMsgPross(msg);
#ifdef SOFT_WACTH_DOG_ENABLE
		big_dog_feed();
#else
		WDG_Feed();
#endif

#ifdef UserSoftPower
		HeartBeat();
#endif

		#ifdef AUTO_TEST_ENABLE
		extern void AutoTestMain(uint16_t test_msg);
		AutoTestMain(msg.msgId);
		#endif
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
		extern void UsbHostHidDateProcess(void);
		UsbHostHidDateProcess();
#endif
		if(mainAppCt.state == TaskStateRunning)
		{
			DeviceServicePocess(msg.msgId);
			if(msg.msgId != MSG_NONE)
			{
				SysModeGenerate(msg.msgId);
				extern bool AudioEffect_Msg_Check(uint32_t Msg);
				if(!AudioEffect_Msg_Check(msg.msgId))
					MessageSend(GetSysModeMsgHandle(), &msg);
			}
			SysModeChangeTimeoutProcess();
		}
	}
}

/***************************************************************************************
 *
 * APIs
 *
 */
int32_t MainAppTaskStart(void)
{
	MainAppInit();
#ifdef CFG_FUNC_SHELL_EN
	shell_init();
	xTaskCreate(mv_shell_task, "SHELL", SHELL_TASK_STACK_SIZE, NULL, SHELL_TASK_PRIO, NULL);
#endif
	xTaskCreate(MainAppTaskEntrance, "MainApp", MAIN_APP_TASK_STACK_SIZE, NULL, MAIN_APP_TASK_PRIO, NULL);
	return 0;
}

MessageHandle GetMainMessageHandle(void)
{
	return mainAppCt.msgHandle;
}


uint32_t GetSystemMode(void)
{
	return mainAppCt.SysCurrentMode;
}

void SamplesFrameUpdataMsg(void)//发现帧变化，发送消息
{
	MessageContext		msgSend;
	APP_DBG("SamplesFrameUpdataMsg\n");

	msgSend.msgId		= MSG_AUDIO_CORE_FRAME_SIZE_CHANGE;
    MessageSend(mainAppCt.msgHandle, &msgSend);
}

void EffectUpdataMsg(void)
{
	MessageContext		msgSend;
	APP_DBG("EffectUpdataMsg\n");

	msgSend.msgId		= MSG_AUDIO_CORE_EFFECT_CHANGE;
	MessageSend(mainAppCt.msgHandle, &msgSend);
}

uint32_t IsBtAudioMode(void)
{
	return (GetSysModeState(ModeBtAudioPlay) == ModeStateRunning);
}

uint32_t IsBtHfMode(void)
{
	return (GetSysModeState(ModeBtHfPlay) == ModeStateRunning);
}

uint32_t IsIdleModeReady(void)
{
	if(GetModeDefineState(ModeIdle))
	{
		if(GetSysModeState(ModeIdle) == ModeStateInit || GetSysModeState(ModeIdle) == ModeStateRunning )
			return 1;
	}
	return 0;
}


void PowerOffMessage(void)
{
	MessageContext		msgSend;

#if	defined(CFG_IDLE_MODE_POWER_KEY) && (POWERKEY_MODE == POWERKEY_MODE_PUSH_BUTTON)
	msgSend.msgId = MSG_POWERDOWN;
	APP_DBG("msgSend.msgId = MSG_DEVICE_SERVICE_POWERDOWN\n");
#elif defined(CFG_SOFT_POWER_KEY_EN)
	msgSend.msgId = MSG_SOFT_POWER;
	APP_DBG("msgSend.msgId = MSG_DEVICE_SERVICE_SOFT_POWEROFF\n");
#elif	defined(CFG_IDLE_MODE_DEEP_SLEEP) 
	msgSend.msgId = MSG_DEEPSLEEP;
	APP_DBG("msgSend.msgId = MSG_DEVICE_SERVICE_DEEPSLEEP\n");
#else
	msgSend.msgId = MSG_POWER;
	APP_DBG("msgSend.msgId = MSG_POWER\n");
#endif

	MessageSend(GetMainMessageHandle(), &msgSend);
}

void BatteryLowMessage(void)
{
	MessageContext		msgSend;

	APP_DBG("msgSend.msgId = MSG_DEVICE_SERVICE_BATTERY_LOW\n");
	msgSend.msgId = MSG_DEVICE_SERVICE_BATTERY_LOW;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

#ifdef UserSoftPower
void MainTaskMsgSend(uint32_t msgId)
{
	MessageContext		msgSend;

	APP_DBG("MainTaskMsgSend = %d\n", msgId);
	msgSend.msgId = msgId;
	MessageSend(GetMainMessageHandle(), &msgSend);
}
#endif
