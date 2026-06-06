/**
 **************************************************************************************
 * @file    dev_detect.c
 * @brief   
 *
 * @author  pi/ken
 * @version V1.0.0
 *
 * $Created: 2020-04-16 
 *
 * @Copyright (C) 2020, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "main_task.h"
#include "gpio.h"
#include "sdio.h"
#include "otg_detect.h"
#include "device_detect.h"
#include "misc.h"
#include "otg_host_standard_enum.h"
#include "otg_device_standard_request.h"
#include "otg_device_audio.h"
#include "otg_device_hcd.h"
#include "otg_host_hcd.h"
#include "irqn.h"
#include "clk.h"
#include "key.h" 
#include "breakpoint.h"
#include "media_play_api.h"
#include "radio_api.h"
#include "rtc_ctrl.h"
#ifdef CFG_FUNC_POWER_MONITOR_EN
#include "power_monitor.h"
#endif
#include "ctrlvars.h"
#include "hdmi_in_api.h"
#include "usb_audio_mode.h"
#include "user_effect_parameter.h"
//static bool IsBackUpFlag = FALSE;
//#ifdef CFG_FUNC_BREAKPOINT_EN
 //TIMER TimerBreakPoint;
//#endif
#ifdef CFG_FUNC_CARD_DETECT
volatile DETECT_STATE CardState = DETECT_STATE_IDLE;
#if FLASH_BOOT_EN
volatile DETECT_STATE CardStateback = DETECT_STATE_IDLE;
#endif
#endif
#ifdef CFG_FUNC_UDISK_DETECT
volatile DETECT_STATE UsbHostState = DETECT_STATE_IDLE;
#if FLASH_BOOT_EN
volatile DETECT_STATE UdiskStateback = DETECT_STATE_IDLE;
#endif
uint8_t UsbHostPortFlag = 0;
#endif
#ifdef CFG_FUNC_USB_DEVICE_DETECT
static DETECT_STATE UsbDeviceState = DETECT_STATE_IDLE;
static uint32_t sDevice_Init_state=0;// 1:inited    0:not init
#endif
#ifdef CFG_LINEIN_DET_EN
static DETECT_STATE LineInState = DETECT_STATE_IDLE;
#endif

extern uint32_t SysemMipsPercent;
extern volatile uint32_t gDeviceCheckTimer;
#ifdef CFG_FUNC_BREAKPOINT_EN
extern MainAppContext	mainAppCt;
#endif
#ifdef CFG_FUNC_CARD_DETECT
extern volatile uint32_t gDeviceCardTimer;
#endif
#ifdef CFG_LINEIN_DET_EN
extern volatile uint32_t gDeviceLineTimer;
#endif

#ifdef HDMI_HPD_CHECK_DETECT_EN
extern volatile uint32_t gDevicehdmiTimer;
#endif

#ifdef CFG_FUNC_BREAKPOINT_EN
extern volatile uint32_t gBreakPointTimer;
#endif
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
extern bool i2c_work;
#endif
#ifdef CFG_FUNC_BREAKPOINT_EN
void BreakPointSave(uint16_t device_msgId);
#endif

#ifdef HDMI_HPD_CHECK_DETECT_EN
static  DETECT_STATE HDMI_Device_State = DETECT_STATE_IDLE;
TIMER	HDMIDetectTimer;
TIMER	HDMIExitTimer;
uint8_t HDMIExitFlg = 0;
uint8_t HDMIResetFlg = 0;
uint8_t	HDMIDetectEnable =1;
#endif

static uint32_t ReadCardDetectPin(void)
{
#ifdef CFG_FUNC_CARD_DETECT
	uint32_t FindCard;
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	static bool FindCard_bak;
#endif
	bool BackupPD;
	uint8_t BackupMode;

#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	if(i2c_work)
		return FindCard_bak;
#endif
	//osMutexLock(SDIOMutex);
	if(osMutexLock_1ms(SDIOMutex) != TRUE)
	{
		FindCard = 2;
		return FindCard;// wait unlock
	}
	SDIO_Clk_Disable();
	BackupMode = GPIO_PortAModeGet(CARD_DETECT_GPIO);
	BackupPD = GPIO_RegOneBitGet(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK);	
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN//开启IO现场恢复时，识别检测时间较长。
	bool BackupIE,BackupOE,BackupPU;
	BackupPU = GPIO_RegOneBitGet(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	BackupIE = GPIO_RegOneBitGet(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
	BackupOE = GPIO_RegOneBitGet(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
	GPIO_RegOneBitClear(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
#endif
	GPIO_PortAModeSet(CARD_DETECT_GPIO, 0x0);
	GPIO_RegOneBitSet(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	GPIO_RegOneBitClear(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK); 
	GPIO_RegOneBitSet(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	__udelay(29);//延时等电平稳定,阻值设定相关。
#endif
	//__udelay(1);//R83阻值设定相关
	if(GPIO_RegOneBitGet(CARD_DETECT_GPIO_IN, CARD_DETECT_BIT_MASK))
	{
		FindCard = 1;
	}
	else
	{
		FindCard = 0;
	}
#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	FindCard_bak = FindCard;
#endif
	GPIO_PortAModeSet(CARD_DETECT_GPIO, BackupMode);
	if(BackupPD)
	{
		GPIO_RegOneBitSet(CARD_DETECT_GPIO_PD, CARD_DETECT_BIT_MASK);	
	}

#ifdef CFG_RADIO_I2C_SD_SAME_PIN_EN
	if(!BackupPU)
	{
		GPIO_RegOneBitClear(CARD_DETECT_GPIO_PU, CARD_DETECT_BIT_MASK);
	}

	if(!BackupIE)
	{
		GPIO_RegOneBitClear(CARD_DETECT_GPIO_IE, CARD_DETECT_BIT_MASK);
	}
	if(BackupOE)
	{
		GPIO_RegOneBitSet(CARD_DETECT_GPIO_OE, CARD_DETECT_BIT_MASK);
	}
#endif

	SDIO_Clk_Eable(); //recover
	osMutexUnlock(SDIOMutex);

	return FindCard;

#else
	return FALSE;
#endif
}
#ifdef CFG_LINEIN_DET_EN
bool IsLineInLink(void)
{
	GPIO_PortAModeSet(LINEIN_DET_BIT_MASK, 0);
	GPIO_RegOneBitClear(LINEIN_DET_GPIO_OE, LINEIN_DET_BIT_MASK);
	GPIO_RegOneBitSet(LINEIN_DET_GPIO_PU, LINEIN_DET_BIT_MASK);
	GPIO_RegOneBitClear(LINEIN_DET_GPIO_PD, LINEIN_DET_BIT_MASK);

	GPIO_RegOneBitSet(LINEIN_DET_GPIO_IE, LINEIN_DET_BIT_MASK);

	if(GPIO_RegOneBitGet(LINEIN_DET_GPIO_IN, LINEIN_DET_BIT_MASK))
	{
		return FALSE;
	}
	return TRUE;	
}
#endif

bool IsHDMILink(void)
{
#ifdef HDMI_HPD_CHECK_DETECT_EN
	//if(GPIO_RegOneBitGet(HDMI_HPD_CHECK_STATUS_IO, HDMI_HPD_CHECK_STATUS_IO_PIN))
	if(HDMI_HPD_StatusGet() == HDMI_HPD_CONNECTED_STATUS)
	{
		return TRUE;
	}
	return FALSE;
#else
	return FALSE;
#endif
}


#ifdef CFG_FUNC_UDISK_DETECT
bool IsUDiskLink(void)
{
	bool IsUDiskLink;
	IsUDiskLink = OTG_PortHostIsLink();
    return IsUDiskLink;
}
#endif


#ifdef CFG_FUNC_USB_DEVICE_DETECT
bool GetUSBDeviceInitState(void)
{
	return sDevice_Init_state; 	
}


void SetUSBDeviceInitState(bool state)
{
	 sDevice_Init_state = state; 	
}

#endif

void InitDeviceDetect(void)
{
#ifdef FUNC_OS_EN
	if(SDIOMutex == NULL)
		SDIOMutex = osMutexCreate();
#endif

#ifdef FUNC_MIC_DET_EN
	IsMicLinkFlag = IsMicInLink();
#endif

#ifdef HDMI_HPD_CHECK_DETECT_EN
		HDMI_HPD_CHECK_IO_INIT();
#endif

	HWDeviceDected_Init();
}

uint32_t DeviceDetect(void)
{
	uint32_t Ret = 0;

	HWDeviceDected();
	
#ifdef CFG_LINEIN_DET_EN
	{
		DETECT_STATE NewLineInState = DETECT_STATE_NONE;
		if(IsLineInLink())
		{
			NewLineInState = DETECT_STATE_IN ;
		}
		else
		{
			NewLineInState = DETECT_STATE_OUT;
		}
		if(LineInState != NewLineInState)
		{
			LineInState = NewLineInState;
			gDeviceLineTimer = DEVICE_LINEIN_DETECT_TIMER;
		}

		if(gDeviceLineTimer == DEVICE_DETECT_ACTTION_POINT)
		{
			APP_DBG("linein State = %d\n",LineInState);
			gDeviceLineTimer = 0;
			if(LineInState == DETECT_STATE_IN)
			{
				Ret |= LINE_IN_EVENT_BIT;
			}
			else
			{
				Ret |= LINE_OUT_EVENT_BIT;
			}
		}
	}
#endif

#ifdef HDMI_HPD_CHECK_DETECT_EN
	{
		DETECT_STATE NewHdmiState = DETECT_STATE_NONE;
		
		if(IsHDMILink())
		{
			NewHdmiState = DETECT_STATE_IN ;
		}
		else
		{
			NewHdmiState = DETECT_STATE_OUT;
		}

		if(HDMI_Device_State != NewHdmiState)
		{
			HDMI_Device_State = NewHdmiState;
			gDevicehdmiTimer = DEVICE_LINEIN_DETECT_TIMER;
		}

		if(gDevicehdmiTimer == DEVICE_DETECT_ACTTION_POINT)
		{
			APP_DBG("hdmi State = %d\n",HDMI_Device_State);
			gDevicehdmiTimer = 0;
			if(HDMI_Device_State == DETECT_STATE_IN)
			{
				Ret |= HDMI_IN_EVENT_BIT;
			}
			else
			{
				Ret |= HDMI_OUT_EVENT_BIT;
			}
		}
	}	
#endif


#ifdef CFG_FUNC_CARD_DETECT	
	{
		DETECT_STATE NewCardState = DETECT_STATE_NONE;
		
		switch(ReadCardDetectPin())
		{
			case 0:
				NewCardState = DETECT_STATE_IN ;
				break;
			case 1:
				NewCardState = DETECT_STATE_OUT ;
				break;
			case 2:
				goto DetectCardExit;
				break;
		}

		if(CardState != NewCardState)
		{
			CardState = NewCardState;
			gDeviceCardTimer = DEVICE_CARD_DETECT_TIMER;
		}

		if(gDeviceCardTimer == DEVICE_DETECT_ACTTION_POINT)
		{
			APP_DBG("Card State = %d\n",CardState);
			gDeviceCardTimer = 0;
			if(CardState == DETECT_STATE_IN)
			{
				CardPortInit(CFG_RES_CARD_GPIO);
				if(SDCard_Init() != NONE_ERR)
				{
					APP_DBG("Card init err.\n");
					return FALSE;
				}
				APP_DBG("SDCard Init Success!\n");
				Ret |= CARD_IN_EVENT_BIT;
			}
			else
			{
				SDCardDeinit(CFG_RES_CARD_GPIO);
				Ret |= CARD_OUT_EVENT_BIT;
			}
		}
	}	
#endif
DetectCardExit:
#ifdef CFG_FUNC_UDISK_DETECT
#ifdef CFG_FUNC_USB_DEVICE_DETECT
	if(UsbDeviceState != DETECT_STATE_IN)
#endif
	{
		DETECT_STATE NewUDiskState = DETECT_STATE_NONE;
		if(OTG_PortHostIsLink())
		{
			NewUDiskState = DETECT_STATE_IN;
		}
		else
		{
			NewUDiskState = DETECT_STATE_OUT;
		}
		
		if(UsbHostState != NewUDiskState)
		{
			UsbHostState = NewUDiskState;
			APP_DBG("UsbHostState = %d\n",UsbHostState) ;
			if(UsbHostState == DETECT_STATE_OUT)
			{
				switch(UsbHostPortFlag)
				{
					case 1:	//u盘
						Ret |= UDISK_OUT_EVENT_BIT;
						break;
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
					case 2: //声卡设备
						UsbHostPlayMixDeinit();
						break;
#endif
					default :
						break;
				}
				UsbHostPortFlag = 0;
				OTG_HostUnInit();
			}
			else
			{
				APP_DBG("OTG Host Init\n");
				OTG_HostFifoInit();
				UsbHostPortFlag = OTG_HostInit();
				switch(UsbHostPortFlag)
				{
					case 1: //u盘
						Ret |= UDISK_IN_EVENT_BIT;
						break;
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
					case 2: //声卡设备
						UsbHostPlayMixInit();
						UsbHostPlayStart();
						break;
#endif
					default :
						break;
				}
			}
		}
	}
#endif

#ifdef CFG_FUNC_USB_DEVICE_DETECT
#ifdef CFG_FUNC_UDISK_DETECT
	if(UsbHostState != DETECT_STATE_IN)
#endif
	{		
		DETECT_STATE NewUDeviceState = DETECT_STATE_NONE;
		if(OTG_PortDeviceIsLink())
		{
			NewUDeviceState = DETECT_STATE_IN;
		}
		else
		{
			NewUDeviceState = DETECT_STATE_OUT;
		}
		if(UsbDeviceState != NewUDeviceState)
		{
			UsbDeviceState = NewUDeviceState;
			APP_DBG("UDevice State = %d\n",UsbDeviceState);
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
			if(UsbDeviceState == DETECT_STATE_OUT)
			{
				SetUSBDeviceInitState(FALSE);
			}
			else
			{
				UsbDevicePlayHardwareInit();
				SetUSBDeviceInitState(TRUE);
			}
#else
			if(UsbDeviceState == DETECT_STATE_OUT)
			{
				Ret |= USB_DEVICE_OUT_EVENT_BIT;
#if defined(CFG_FUNC_AUDIO_EFFECT_EN)&&defined(CFG_COMMUNICATION_BY_USB)
				OTG_DeviceDisConnect();
				SetUSBDeviceInitState(FALSE);
#endif
			}
			else
			{
				Ret |= USB_DEVICE_IN_EVENT_BIT;
#if defined(CFG_FUNC_AUDIO_EFFECT_EN)&&defined(CFG_COMMUNICATION_BY_USB)
				if(((GetModeDefineState(ModeUsbDevicePlay)) ||(!GetModeDefineState(ModeUsbDevicePlay)))
					&& (GetUSBDeviceInitState() == FALSE))
				{
					OTG_DeviceModeSel(HID, USB_VID, USBPID(HID));
					OTG_DeviceFifoInit();
					OTG_DeviceInit();
					SetUSBDeviceInitState(TRUE);
//					gCtrlVars.AutoRefresh = AutoRefresh_ALL_PARA;
				}
#endif
			}
#endif
		}
	}		
#endif

	return Ret;
}


/**
 * @brief	Device servcie init
 * @param	void
 * @return	none
 */
 
 void DeviceServiceInit(void)
{
#ifdef CFG_FUNC_POWER_MONITOR_EN
	PowerMonitorInit();
#endif
#if defined(CFG_RES_ADC_KEY_SCAN) || defined(CFG_RES_IR_KEY_SCAN) || defined(CFG_RES_CODE_KEY_USE)|| defined(CFG_ADC_LEVEL_KEY_EN) || defined(CFG_RES_IO_KEY_SCAN)
	KeyInit();//Init keys
#endif

	InitDeviceDetect();//Init device 
}
static void DevicePlugEventCheck(void)
{
	uint32_t Plug;
	MessageContext		msgSend;

	msgSend.msgId = MSG_NONE;

	Plug = DeviceDetect();
#ifdef CFG_LINEIN_DET_EN //linein msg	
	if(Plug & LINE_IN_EVENT_BIT)
	{
		if(GetSysModeState(ModeLineAudioPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeLineAudioPlay,ModeStateReady);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_LINE_IN;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
	else if(Plug & LINE_OUT_EVENT_BIT)
	{
		if(GetSysModeState(ModeLineAudioPlay) == ModeStateReady)
		{
			SetSysModeState(ModeLineAudioPlay,ModeStateSusend);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_LINE_OUT;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
#endif
#ifdef HDMI_HPD_CHECK_DETECT_EN
	if(Plug & HDMI_IN_EVENT_BIT)
	{
		if(GetSysModeState(ModeHdmiAudioPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeHdmiAudioPlay,ModeStateReady);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_HDMI_IN;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
	else if(Plug & HDMI_OUT_EVENT_BIT)
	{
		if(GetSysModeState(ModeHdmiAudioPlay) == ModeStateReady)
		{
			SetSysModeState(ModeHdmiAudioPlay,ModeStateSusend);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_HDMI_OUT;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
#endif

#ifdef CFG_FUNC_CARD_DETECT	//card msg
	if(Plug & CARD_IN_EVENT_BIT)
	{
#if FLASH_BOOT_EN
		CardStateback = DETECT_STATE_IN;
#endif
		if(GetSysModeState(ModeCardAudioPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeCardAudioPlay,ModeStateReady);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_CARD_IN;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
	else if(Plug & CARD_OUT_EVENT_BIT)
	{
#if FLASH_BOOT_EN
		if(CardStateback == DETECT_STATE_IN)
		{
			SoftFlagDeregister(SoftFlagUpgradeOK);
		}
		CardStateback = DETECT_STATE_OUT;
		SoftFlagDeregister(SoftFlagMvaInCard);//清理mva包标记
#endif
#ifndef CFG_FUNC_APP_USB_CARD_IDLE
		if(GetSysModeState(ModeCardAudioPlay) == ModeStateReady)
		{
			SetSysModeState(ModeCardAudioPlay,ModeStateSusend);
		}
#endif
		msgSend.msgId	= MSG_DEVICE_SERVICE_CARD_OUT;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
#endif
#ifdef CFG_FUNC_UDISK_DETECT	//usb host msg	
	if(Plug & UDISK_IN_EVENT_BIT)
	{
#if FLASH_BOOT_EN
		UdiskStateback = DETECT_STATE_IN;
#endif
		if(GetSysModeState(ModeUDiskAudioPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeUDiskAudioPlay,ModeStateReady);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_U_DISK_IN;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
	else if(Plug & UDISK_OUT_EVENT_BIT)
	{
#if FLASH_BOOT_EN
		if(UdiskStateback == DETECT_STATE_IN)
		{
			SoftFlagDeregister(SoftFlagUpgradeOK);
		}
		UdiskStateback = DETECT_STATE_OUT;
		SoftFlagDeregister(SoftFlagMvaInUDisk);//清理mva包标记
#endif
#ifndef CFG_FUNC_APP_USB_CARD_IDLE
		if(GetSysModeState(ModeUDiskAudioPlay) == ModeStateReady)
		{
			SetSysModeState(ModeUDiskAudioPlay,ModeStateSusend);
		}
#endif
		msgSend.msgId	= MSG_DEVICE_SERVICE_U_DISK_OUT;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
#endif
#ifdef CFG_APP_USB_AUDIO_MODE_EN	
	if(Plug & USB_DEVICE_IN_EVENT_BIT)
	{
		if(GetSysModeState(ModeUsbDevicePlay) == ModeStateSusend)
		{
			SetSysModeState(ModeUsbDevicePlay,ModeStateReady);
		}

		msgSend.msgId	= MSG_DEVICE_SERVICE_USB_DEVICE_IN;
		MessageSend(GetMainMessageHandle(), &msgSend);
		
	}
	else if(Plug & USB_DEVICE_OUT_EVENT_BIT)
	{
		if(GetSysModeState(ModeUsbDevicePlay) == ModeStateReady)
		{
			SetSysModeState(ModeUsbDevicePlay,ModeStateSusend);
		}
		msgSend.msgId	= MSG_DEVICE_SERVICE_USB_DEVICE_OUT;
		MessageSend(GetMainMessageHandle(), &msgSend);
	}
#endif

	if (msgSend.msgId != MSG_NONE)
	{
		//外设插拔重置无信号关机计时器
		#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
		mainAppCt.Silence_Power_Off_Time = 0;
		#endif			
	}
}

void DeviceServicePocess(uint16_t device_msgId)
{
	MessageContext		msgSend;

#ifdef CFG_FUNC_BREAKPOINT_EN
	BreakPointSave(device_msgId);
#endif
#if defined(CFG_LOCK_PAIRED_TXRX) && defined(CFG_SYNC_TO_TX_SDK) //CFG_WIRELESS_EN
	TX_PairedInfoSetFlash();
#elif defined(CFG_LOCK_PAIRED_TXRX)
	RX_PairedInfoSetFlash();
#endif
	if(gDeviceCheckTimer == 0)
	{
		gDeviceCheckTimer = DEVICE_DETECT_TIMER;
		
		DevicePlugEventCheck();
		
#if defined(CFG_RES_ADC_KEY_SCAN) || defined(CFG_RES_IR_KEY_SCAN) || defined(CFG_RES_CODE_KEY_USE)|| defined(CFG_ADC_LEVEL_KEY_EN) || defined(CFG_RES_IO_KEY_SCAN)
		msgSend.msgId = KeyScan();
		if(msgSend.msgId != MSG_NONE)
		{
			//按键消息重置无信号关机计时器
			#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
			mainAppCt.Silence_Power_Off_Time = 0;
			#endif	
			MessageSend(GetMainMessageHandle(), &msgSend);
		}
#endif

#ifdef CFG_FUNC_POWER_MONITOR_EN
		//电池电压采样
		PowerVoltageSampling();
		//电池电量处理
		BatteryScan();
#endif

#ifdef CFG_FUNC_RTC_EN
		RtcStateCtrl();
#endif
#if defined(CFG_FUNC_DEBUG_EN) || defined(CFG_FUNC_USBDEBUG_EN)
		{
			static bool MipsLog = TRUE;
			if((GetSysTick1MsCnt() % MIPS_LOG_INTERVAL) < (MIPS_LOG_INTERVAL / 2))
			{
				if(MipsLog)
				{
					DBG("%s ", GetModeNameStr(mainAppCt.SysCurrentMode));
	#ifdef ENABLE_COUNT_INSTANT_MCPS
					uint16_t InstantVal = InstantMcpsFull();
					if(SysemMipsPercent < 10000 / 2)
					{
						DBG("Fullload:%d mS ", (int)(InstantVal ? InstantVal : MIPS_LOG_INTERVAL));
					}
	#endif
					APP_DBG("MCPS:%d M  ", (int)((10000 - SysemMipsPercent) * (Clock_CoreClockFreqGet() / 1000000)) / 10000);
					APP_DBG("RAM:%d\n", (int)(CFG_CHIP_RAM_SIZE - osPortRemainMem())/1024);
					MipsLog = FALSE;
				}
			}
			else
			{
				MipsLog = TRUE;
			}
		}
#endif
	}
}

#ifdef CFG_FUNC_BREAKPOINT_EN
void BreakPointSave(uint16_t device_msgId)
{
	BP_SYS_INFO *pBpSysInfo;
#if defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN)
	BP_PLAYER_INFO *pBpPlayInfo;
#endif
#ifdef CFG_APP_RADIOIN_MODE_EN
	BP_RADIO_INFO *pBpRadioInfo;
#endif

	switch(device_msgId)
	{
		case MSG_DEVICE_SERVICE_BP_SYS_INFO:
		{
			BP_SYS_INFO sys_info;
			//TimeOutSet(&TimerBreakPoint, 500);
			//printf("hhhhhkkkkk\n");
			pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
			memcpy(&sys_info,pBpSysInfo,sizeof(BP_SYS_INFO));
			pBpSysInfo->CurModuleId  = GetSystemMode();
			pBpSysInfo->MusicVolume  = MusicVolume;
#ifdef CFG_APP_BT_MODE_EN
			pBpSysInfo->HfVolume     = mainAppCt.HfVolume;
#endif
			extern uint8_t GetCurEffectModeState(uint8_t effect_mode);
//			if(mainAppCt.EffectMode != EFFECT_MODE_HFP_AEC || mainAppCt.EffectMode != EFFECT_MODE_TDM)//蓝牙通话模式下，不保存音效模式
			if(GetCurEffectModeState(mainAppCt.EffectMode))
			{
				pBpSysInfo->EffectMode   = mainAppCt.EffectMode;
			}
			pBpSysInfo->MicVolume    = MicVolume;
#ifdef CFG_FUNC_MUSIC_EQ_MODE_EN
			pBpSysInfo->EqMode		 = EqMode;
#endif	
#ifdef CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC
			pBpSysInfo->ReverbStep   = ReverbStep;
#endif
#ifdef CFG_FUNC_MIC_TREB_BASS_EN
			pBpSysInfo->MicBassStep     = MicBassStep;
			pBpSysInfo->MicTrebStep     = MicTrebStep;
#endif
#ifdef CFG_FUNC_MUSIC_TREB_BASS_EN
			pBpSysInfo->MusicBassStep     = MusicBassStep;
			pBpSysInfo->MusicTrebStep     = MusicTrebStep;
#endif
#ifdef CFG_FUNC_SOUND_REMIND
			//pBpSysInfo->SoundRemindOn = mainAppCt.SoundRemindOn;
			//pBpSysInfo->LanguageMode = mainAppCt.LanguageMode;
#endif
#ifdef  VD51_REDMINE_13199
			pBpSysInfo->rgb_mode = GetRgbLedMode();
#endif
#ifdef UserSoftPower
			UserVar_Breakpoint_Write(pBpSysInfo);
#endif
			if(memcmp(&sys_info,pBpSysInfo,sizeof(BP_SYS_INFO)) != 0)
				gBreakPointTimer = DEVICE_BREAK_POINT_TIMER;
			
			
		}
			break;

#if defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN)
		case MSG_DEVICE_SERVICE_BP_PLAYER_INFO:
		{
			BP_PLAYER_INFO play_info;
			if(gMediaPlayer == NULL)
			{
				return;
			}
			//TimeOutSet(&TimerBreakPoint, 500);
			pBpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);
			memcpy(&play_info,pBpPlayInfo,sizeof(BP_PLAYER_INFO));
#if defined(CFG_APP_CARD_PLAY_MODE_EN)
			if(GetSystemMode() == ModeCardAudioPlay && gMediaPlayer != NULL)
			{
				//pBpPlayInfo->PlayCardInfo.PlayTime = 0;
				pBpPlayInfo->PlayCardInfo.DirSect = gMediaPlayer->PlayerFile.dir_sect;
				pBpPlayInfo->PlayCardInfo.FirstClust = gMediaPlayer->PlayerFile.obj.sclust;
				pBpPlayInfo->PlayCardInfo.FileSize = gMediaPlayer->PlayerFile.obj.objsize;
			}
#endif
#if defined(CFG_APP_USB_PLAY_MODE_EN)
			if(GetSystemMode() == ModeUDiskAudioPlay && gMediaPlayer != NULL)
			{
				//pBpPlayInfo->PlayUDiskInfo.PlayTime = 0;
				pBpPlayInfo->PlayUDiskInfo.DirSect = gMediaPlayer->PlayerFile.dir_sect;
				pBpPlayInfo->PlayUDiskInfo.FirstClust = gMediaPlayer->PlayerFile.obj.sclust;
				pBpPlayInfo->PlayUDiskInfo.FileSize = gMediaPlayer->PlayerFile.obj.objsize;
			}
#endif
#ifdef CFG_FUNC_LRC_EN
			pBpPlayInfo->LrcFlag = gMediaPlayer->LrcFlag;
#endif
			pBpPlayInfo->PlayMode = gMediaPlayer->CurPlayMode;
			if(memcmp(&play_info,pBpPlayInfo,sizeof(BP_PLAYER_INFO)) != 0)
				gBreakPointTimer = DEVICE_BREAK_POINT_TIMER;
				
		}
			break;

#ifdef BP_PART_SAVE_TO_NVM
		case MSG_DEVICE_SERVICE_BP_PLAYER_INFO_2NVM:
		{
			pBpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);
#if defined(CFG_APP_CARD_PLAY_MODE_EN)
			if(GetSystemMode() == ModeCardAudioPlay && gMediaPlayer != NULL)
			{
				pBpPlayInfo->PlayCardInfo.PlayTime = (uint16_t)(gMediaPlayer->CurPlayTime);
				pBpPlayInfo->PlayCardInfo.DirSect = gMediaPlayer->PlayerFile.dir_sect;
				pBpPlayInfo->PlayCardInfo.FirstClust = gMediaPlayer->PlayerFile.obj.sclust;
				pBpPlayInfo->PlayCardInfo.FileSize = gMediaPlayer->PlayerFile.obj.objsize;
			}
#endif
#if defined(CFG_APP_USB_PLAY_MODE_EN)
			if(GetSystemMode() == ModeUDiskAudioPlay && gMediaPlayer != NULL)
			{
				pBpPlayInfo->PlayUDiskInfo.PlayTime = (uint16_t)(gMediaPlayer->CurPlayTime);
				pBpPlayInfo->PlayUDiskInfo.DirSect = gMediaPlayer->PlayerFile.dir_sect;
				pBpPlayInfo->PlayUDiskInfo.FirstClust = gMediaPlayer->PlayerFile.obj.sclust;
				pBpPlayInfo->PlayUDiskInfo.FileSize = gMediaPlayer->PlayerFile.obj.objsize;
			}
#endif
			BP_SaveInfo(1);
		}
			return;
#endif
#endif//defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN)

		case MSG_DEVICE_SERVICE_BP_RADIO_INFO:
#ifdef CFG_APP_RADIOIN_MODE_EN
		{
			uint32_t i;
			BP_RADIO_INFO radio_info;
			if(gRadioControl == NULL)
			{
				return;
			}
			APP_DBG("save radio\n");
			//TimeOutSet(&TimerBreakPoint, 500);
			pBpRadioInfo = (BP_RADIO_INFO *)BP_GetInfo(BP_RADIO_INFO_TYPE);
			memcpy(&radio_info,pBpRadioInfo,sizeof(BP_RADIO_INFO));
			//BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
			//BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
			pBpRadioInfo->CurBandIdx = gRadioControl->CurFreqArea<<6;
			pBpRadioInfo->CurFreq = gRadioControl->Freq;
			pBpRadioInfo->StationCount = gRadioControl->ChlCount;

			if(gRadioControl->ChlCount > 0)
			{
				for(i = 0; i < gRadioControl->ChlCount; i++)
				{
					pBpRadioInfo->StationList[i] = gRadioControl->Channel[i];
				}
			}
			if(memcmp(&radio_info,pBpRadioInfo,sizeof(BP_RADIO_INFO)) != 0)	
				gBreakPointTimer = DEVICE_BREAK_POINT_TIMER;
						
		}
#endif
		break;
/*
		case MSG_DEVICE_SERVICE_BP_ALL_INFO:
			IsBackUpFlag = TRUE;
			pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
			pBpPlayInfo = (BP_PLAYER_INFO *)BP_GetInfo(BP_PLAYER_INFO_TYPE);
#ifdef CFG_APP_RADIOIN_MODE_EN
			pBpRadioInfo = (BP_RADIO_INFO *)BP_GetInfo(BP_RADIO_INFO_TYPE);
#endif
			break;
*/
		default:
			break;
	}


	if(gBreakPointTimer == DEVICE_DETECT_ACTTION_POINT)
	{
		gBreakPointTimer = 0;	
#ifdef	CFG_FUNC_OPEN_SLOW_DEVICE_TASK
		{
			extern void SlowDevice_MsgSend(uint16_t msgId);
			SlowDevice_MsgSend(MSG_DEVICE_BREAK_POINT_WRITE);
		}
#else
		//printf("old break point save ......\n");
		BP_SaveInfo(0);
#endif
	}
}
#endif

#ifdef CFG_FUNC_CARD_DETECT
DETECT_STATE GetCardState(void)
{
	return CardState;
}
#endif

#ifdef CFG_FUNC_UDISK_DETECT
DETECT_STATE GetUdiscState(void)
{
	if(UsbHostPortFlag & 0x01)	//U盘
	{
		return DETECT_STATE_IN;
	}
	else
	{
		return DETECT_STATE_OUT;
	}
}
#endif

bool IsMediaPlugOut(void)
{
	int32_t plug_out_count = 0;
	uint32_t detect_time_timer = 300;
#ifdef CFG_FUNC_UDISK_DETECT
	if(GetSystemMode() == ModeUDiskAudioPlay)
	{
		while(detect_time_timer)
		{
			osTaskDelay(1);
			if(GetUdiscState() == DETECT_STATE_OUT)
			{
				if(plug_out_count < 0)
				{
					plug_out_count = 0;
				}
				plug_out_count++;
			}
			else
			{
				plug_out_count--;
			}
			detect_time_timer--;
			if(plug_out_count > 40)break;
		}	
	}
#endif
#ifdef CFG_FUNC_CARD_DETECT
#ifndef CFG_FUNC_UDISK_DETECT
	if(GetSystemMode() == ModeCardAudioPlay)
#else
	else if(GetSystemMode() == ModeCardAudioPlay)
#endif
	{
		detect_time_timer = 350;
		while(detect_time_timer)
		{
			osTaskDelay(1);
			if(CardState == DETECT_STATE_OUT)
			{
				if(plug_out_count < 0)
				{
					plug_out_count = 0;
				}
				plug_out_count++;
			}
			else
			{
				plug_out_count--;
			}
			detect_time_timer--;
			if(plug_out_count > 40)break;
		}	
	}
#endif
	if(plug_out_count > 40)
	{
		APP_DBG("PLAY ERROR, because devicce plug out\n");
		return TRUE;
	}
	
	return FALSE;
}

#include "bt_app_ddb_info.h"

#ifdef CFG_FUNC_OPEN_SLOW_DEVICE_TASK
void SlowDeviceEventProcess(uint16_t device_msgId)
{
	switch(device_msgId)
	{
#ifdef CFG_FUNC_BREAKPOINT_EN
		case MSG_DEVICE_BREAK_POINT_WRITE:
		{
			//printf("store break point data ....................\n");
			BP_SaveInfo(0);
		}
		break;
#endif

#ifdef CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
		case MSG_DEVICE_EFFECT_DATA_WRITE:
		{
			EffectParamFlashUpdata();
		}
		break;
#endif

#ifdef CFG_APP_BT_MODE_EN
		case MSG_DEVICE_BT_LINKED_INFOR_WRITE:
		{
			//printf("store bt link data ....................\n");
			SaveTotalDevRec2Flash(1 + BT_REC_INFO_LEN * MAX_BT_DEVICE_NUM/*one total rec block size*/,
							GetCurTotaBtRecNum());
		}
		break;
	
		case MSG_DEVICE_BT_LINKED_PROFILE_UPDATE:
		{
			//printf("store bt link profile ....................\n");
			//BtDdb_UpgradeLastBtProfile(btManager.remoteAddr, btManager.btDdbLastProfile);
			BtDdb_UpgradeLastBtProfile(btManager.btDdbLastAddr, btManager.btDdbLastProfile);
		}
		break;

		case MSG_DEVICE_BT_LINKED_INFOR_ERASE:
		{
			BtDdb_Erase();
		}
		break;
		case MSG_DEVICE_BT_FREQ_OFFSET_WRITE:
			{
				extern BT_CONFIGURATION_PARAMS		*btStackConfigParams;
				BT_CONFIGURATION_PARAMS		paramsCheck;
				//save to flash
				int8_t ret = BtDdb_SaveBtConfigurationParams(btStackConfigParams);
			
				if(ret)
				{
					//第一次保存异常
					APP_DBG("[BT_OFFSET]update Error!!!\n");
					
					//校验异常,第二次保存数据
					ret = BtDdb_SaveBtConfigurationParams(btStackConfigParams);
					if(ret)
					{
						APP_DBG("[BT_OFFSET2]update Error!!!\n");
						break;
					}
					
					//第二次读取数据
					ret = BtDdb_LoadBtConfigurationParams(&paramsCheck);
					
					if(ret == 0)
					{
						if(paramsCheck.bt_trimValue == btStackConfigParams->bt_trimValue)
						{
							//校验正常
							APP_DBG("$$$[BT_OFFSET] update $$$\n");
						}
						else
						{
							//校验异常
							APP_DBG("!!!!!![BT_OFFSET] update error..... \n");
						}
					}
					else
					{
						//读取异常
						APP_DBG("[BT_OFFSET2]update Error!!!\n");
					}
				}
				else
				{
					//第一次读取校验
					ret = BtDdb_LoadBtConfigurationParams(&paramsCheck);
					if(ret == 0)
					{
						if(paramsCheck.bt_trimValue == btStackConfigParams->bt_trimValue)
						{
							//校验正常
							APP_DBG("$$$[BT_OFFSET] update $$$\n");
						}
						else
						{
							APP_DBG("!!!!!![BT_OFFSET] check error, write agian $$$\n");

							//校验异常,第二次保存数据
							ret = BtDdb_SaveBtConfigurationParams(btStackConfigParams);
							if(ret)
							{
								APP_DBG("[BT_OFFSET2]update Error!!!\n");
								break;
							}
							
							//第二次读取数据
							ret = BtDdb_LoadBtConfigurationParams(&paramsCheck);
							
							if(ret == 0)
							{
								if(paramsCheck.bt_trimValue == btStackConfigParams->bt_trimValue)
								{
									//校验正常
									APP_DBG("$$$[BT_OFFSET] update $$$\n");
								}
								else
								{
									//校验异常
									APP_DBG("!!!!!![BT_OFFSET] update error..... \n");
								}
							}
							else
							{
								//读取异常
								APP_DBG("[BT_OFFSET2]update Error!!!\n");
							}
						}
					}
					else
					{
						APP_DBG("[BT_OFFSET] read error:%d, again $$$\n", ret);
						
						//保存异常,第二次读取数据
						ret = BtDdb_LoadBtConfigurationParams(&paramsCheck);
						if(ret == 0)
						{
							if(paramsCheck.bt_trimValue == btStackConfigParams->bt_trimValue)
							{
								//校验正常
								APP_DBG("[BT_OFFSET] update ok\n");
							}
							else
							{
								//校验异常,第二次保存数据
								APP_DBG("!!!!!![BT_OFFSET] check error, write agian $$$\n");

								ret = BtDdb_SaveBtConfigurationParams(btStackConfigParams);
								if(ret)
								{
									APP_DBG("[BT_OFFSET2]update Error!!!\n");
									return;
								}
								ret = BtDdb_LoadBtConfigurationParams(&paramsCheck);
								
								//第二次读取数据
								if(ret == 0)
								{
									if(paramsCheck.bt_trimValue == btStackConfigParams->bt_trimValue)
									{
										//校验正常
										APP_DBG("$$$[BT_OFFSET] update $$$\n");
									}
									else
									{
										//校验异常
										APP_DBG("!!!!!![BT_OFFSET] update error..... \n");
									}
								}
								else
								{
									//读取异常
									APP_DBG("[BT_OFFSET2]update Error!!!\n");
								}
							}
						}
						else
						{
							APP_DBG("!!!!!![BT_OFFSET] flash read error..... \n");
						}
					}
				}
			}
			break;

		case MSG_DEVICE_BT_NAME_WRITE:
		{
			extern int32_t BtDeviceNameSet(void);
			//更新蓝牙名称到flash
			BtDeviceNameSet();
		}
		break;
#endif
	}

}
#endif



