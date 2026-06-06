/**
 **************************************************************************************
 * @file    main_task.h
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
#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__


#include "type.h"
#include "rtos_api.h"
#include "bt_config.h"
#include "audio_core_api.h"
#include "app_message.h"
#include "timeout.h"
#include "mode_task.h"
#include "mode_task_api.h"
#include "flash_boot.h"
#ifdef CFG_FUNC_DISPLAY_EN
#include "display.h"
#endif
#include "add_status.h"
#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
#include "led_effect.h"
#endif

#define SoftFlagMask			0xFFFFFFFF

#ifdef CFG_RES_IR_NUMBERKEY
extern bool Number_select_flag;
extern uint16_t Number_value;
extern TIMER Number_selectTimer;
#endif

typedef struct _SysVolContext
{
	bool		MuteFlag;	//AudioCore软件mute
	int8_t	 	AudioSourceVol[AUDIO_CORE_SOURCE_MAX_NUM];	//Source增益控制step，小于等于32
	int8_t	 	AudioSinkVol[AUDIO_CORE_SINK_MAX_NUM];		//Sink增益控制step，小于等于32	
	
}SysVolContext;

typedef struct _MainAppContext
{
	MessageHandle		msgHandle;
	TaskState			state;

	SysModeNumber		SysCurrentMode;
	SysModeNumber		SysPrevMode;

/*************************mode common*************************************/
#ifdef CFG_RES_AUDIO_DAC0_EN
	uint32_t			*DACFIFO;
	uint32_t			DACFIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
	uint32_t			*I2SFIFO;
	uint32_t			I2SFIFO_LEN;
#endif

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	uint32_t			*LE_AUDIO_I2S_RX_FIFO;
	uint32_t			LE_AUDIO_I2S_RX_FIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	uint32_t			*I2S_MIX_TX_FIFO;
	uint32_t			I2S_MIX_TX_FIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	uint32_t			*I2S_MIX_RX_FIFO;
	uint32_t			I2S_MIX_RX_FIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	uint32_t			*I2S_MIX2_TX_FIFO;
	uint32_t			I2S_MIX2_TX_FIFO_LEN;
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	uint32_t			*I2S_MIX2_RX_FIFO;
	uint32_t			I2S_MIX2_RX_FIFO_LEN;
#endif


#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	uint32_t			*SPDIF_OUT_FIFO;
	uint32_t			SPDIF_OUT_FIFO_LEN;
#endif

	uint32_t			*ADCFIFO;
/******************************************************************/
//#ifdef CFG_FUNC_DISPLAY_EN
//	bool				DisplaySync;
//#endif
#ifdef CFG_FUNC_ALARM_EN
	uint32_t 			AlarmID;//闹钟ID对应bit位
	bool				AlarmFlag;
	bool				AlarmRemindStart;//闹铃提示音开启标志
	uint32_t 			AlarmRemindCnt;//闹铃提示音循环次数
	#ifdef CFG_FUNC_SNOOZE_EN
	bool				SnoozeOn;
	uint32_t 			SnoozeCnt;// 贪睡时间计数
	#endif
#endif
	SysVolContext		gSysVol;
#ifdef CFG_APP_BT_MODE_EN
    uint8_t     HfVolume;
#endif
	uint8_t     EffectMode;

#ifdef CFG_FUNC_SHUNNING_EN
	uint8_t             ShunningMode;
	uint32_t            aux_out_dyn_gain;
#endif

#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
	uint32_t    Silence_Power_Off_Time;
#endif

#ifdef  CFG_APP_HDMIIN_MODE_EN
	uint8_t  	hdmiArcOnFlg;
	uint8_t     hdmiSourceMuteFlg;
	uint8_t     hdmiResetFlg;
#endif

	uint32_t SoftwareFlag;	// soft flag register
#ifdef CFG_ADC_LEVEL_KEY_EN
	int16_t MicVolumeBak;
#endif

#if BT_SOURCE_SUPPORT
	uint32_t 				*sbc_encode_pcm_buf;
	uint32_t				sbc_encode_pcm_buf_len;
	MCU_CIRCULAR_CONTEXT 	sbc_encode_pcm_context;
#endif
}MainAppContext;

extern MainAppContext	mainAppCt;

enum _SYS_SOFTWARE_FLAG_
{
	SoftFlagNoRemind				=	BIT(0),	//提示音故障
	SoftFlagMediaDevicePlutOut 		= 	BIT(1),
	SoftFlagMvaInCard				=	BIT(2),	//文件预搜索发现SD卡有MVA包 卡拔除时清理
	SoftFlagMvaInUDisk				=	BIT(3),	//文件预搜索发现U盘有Mva包 U盘拔除时清理
	SoftFlagDiscDelayMask			=	BIT(4), //通话模式,蓝牙断开连接,延时播放提示音,即退回到每个模式时播放
	SoftFlagWaitBtRemindEnd			=	BIT(5), //标记来电时等待提示音播放完成再进入通话状态
	SoftFlagDelayEnterBtHf			=	BIT(6), //标记延时进入通话状态
	SoftFlagFrameSizeChange			=	BIT(7), //旨在登记系统帧切换流程这一状态，避免打断。
	SoftFlagBtCurPlayStateMask		=	BIT(8), //标记来电时记录当前蓝牙播放的状态
	SoftFlagTwsRemind				=	BIT(9), //标记tws连接成功事件 等待unmute后提示音开播
	SoftFlagTwsSlaveRemind			=	BIT(10),
	SoftFlagBtOtaUpgradeOK			=	BIT(11),

	SoftFlagIdleModeEnterSleep		=	BIT(12),//标记进入睡眠模式
	SoftFlagIdleModeEnterPowerDown	=	BIT(13),
	SoftFlagMediaModeRead			=	BIT(14),// 进入media mode 读一次U或SD
	SoftFlagMediaNextOrPrev			=	BIT(15),// 0:next 1:prev
	SoftFlagUpgradeOK				=	BIT(16),

	SoftFlagAudioCoreSourceIsDeInit	=	BIT(18),//AudioCoreSource资源已经被删除

	SoftFlagUDiskEnum				=	BIT(19),//u盘枚举标志
	SoftFlagRecording				=	BIT(20),//录音进行标记，禁止后插先播，模式切换需清理
	//标记本次deepsleep消息是否来自于TV
	SoftFlagDeepSleepMsgIsFromTV 	=	BIT(21),
	//标记本次唤醒源是否为CEC唤醒
	SoftFlagWakeUpSouceIsCEC 		= 	BIT(22),
	//标记进入SoftPower
	SoftFlagIdleModeEnterSoftPower 	=	BIT(23),

	//标记切换音效需要重启当前模式
	SoftFlagSysCurModeReboot 		=	BIT(24),
};

#define SoftFlagRegister(SoftEvent)			(mainAppCt.SoftwareFlag |= SoftEvent)
#define SoftFlagDeregister(SoftEvent)		(mainAppCt.SoftwareFlag &= ~SoftEvent)
#define SoftFlagGet(SoftEvent)				(mainAppCt.SoftwareFlag & SoftEvent ? TRUE : FALSE)

#if (BT_AVRCP_VOLUME_SYNC) && defined(CFG_APP_BT_MODE_EN)
	#define CFG_PARA_MAX_VOLUME_NUM		        (16)	//SDK 16 级音量,针对iphone手机蓝牙音量同步功能定制，音量表16级能一一对应手机端音量级别
	#define CFG_PARA_SYS_VOLUME_DEFAULT			(12)	//SDK默认音量
#else
	#define CFG_PARA_MAX_VOLUME_NUM		        (32)	//SDK 32 级音量
	#define CFG_PARA_SYS_VOLUME_DEFAULT			(32)	//SDK默认音量
#endif


int32_t MainAppTaskStart(void);
MessageHandle GetMainMessageHandle(void);
uint32_t GetSystemMode(void);

uint32_t IsBtAudioMode(void);

uint32_t IsIdleModeReady(void);
void PowerOffMessage(void);
void BatteryLowMessage(void);

//uint8_t Get_Resampler_Polyphase(int32_t resampler);
#endif /*__MAIN_TASK_H__*/

