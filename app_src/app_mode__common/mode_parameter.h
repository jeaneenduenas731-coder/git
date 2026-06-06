#include "mode_task.h"
#include "device_detect.h"
#include "linein_mode.h"
#include "spdif_mode.h"
#include "media_play_mode.h"
#include "bt_play_mode.h"
#include "bt_hf_mode.h"
#include "i2sin_mode.h"
#include "radio_mode.h"
#include "usb_audio_mode.h"
#include "idle_mode.h"
#include "hdmi_in_mode.h"
#include "wirelessin_play_mode.h"

static void SysModeEntrance(void * param);
extern unsigned char TaskDeleteCompleted(void);
extern volatile uint32_t gChangeModeTimeOutTimer;
extern volatile uint32_t gInsertEventDelayActTimer;
extern bool RemindSoundRun(SysModeState ModeState);
extern void WaitUdiskUnlock(void);
extern void SDCardForceExitFuc(void);
extern void AudioCoreServiceResume(void);

const SysModeStrAndRemind ModeNameStr[]=
{
	{ModeIdle,		 		 "Idle"				},
#ifdef CFG_APP_BT_MODE_EN
	{ModeBtAudioPlay,        "BtPlay"   		},
#endif
#ifdef CFG_APP_USB_PLAY_MODE_EN
	{ModeUDiskAudioPlay,     "UDiskPlay"  		},
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	{ModeCardAudioPlay,     "CardPlay",   		},
#endif
#ifdef CFG_APP_LINEIN_MODE_EN
	{ModeLineAudioPlay,     "LinePlay" 			},
#endif
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	{ModeUsbDevicePlay,     "UsbDevicePlay" 	 },
#endif
#ifdef CFG_APP_I2SIN_MODE_EN
	{ModeI2SInAudioPlay,     "I2SINPlay"	 	},
#endif
#ifdef CFG_APP_RADIOIN_MODE_EN
	{ModeRadioAudioPlay,     "RadioPlay" 	 	},
#endif
#if (BT_HFP_SUPPORT && defined(CFG_APP_BT_MODE_EN))
	{ModeBtHfPlay,			"BtHfPlay"			},
#endif
#ifdef CFG_APP_OPTICAL_MODE_EN
	{ModeOpticalAudioPlay,		"OpticalPlay"	},
#endif
#ifdef CFG_APP_COAXIAL_MODE_EN
	{ModeCoaxialAudioPlay,		"CoaxialPlay"	},
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	{ModeHdmiAudioPlay,			"HdmiAudioPlay"	},
#endif
#ifdef CFG_FUNC_RECORDER_EN
#ifdef CFG_APP_USB_PLAY_MODE_EN
	{ModeUDiskPlayBack,			"UDiskPlayBack" },//USB
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	{ModeCardPlayBack,			"ModeCardPlayBack"},//SD
#endif
#endif

#ifdef CFG_APP_WIRELESSIN_MODE_EN
	{ModeWirelessinPlay,		"ModeWirelessinPlay"},//
#endif
#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	{ModeLeAudioSinkPlay,	"LeAudio Sink"},
	{ModeLeAudioLinein,		"LeAudio LineIn"},
	{ModeLeAudioBT,			"LeAudio BT"},
#endif
};

#ifdef CFG_APP_IDLE_MODE_EN
	void DualChipIdleRun(uint16_t msgId);
	bool DualChipIdleInit(void);
	bool DualChipIdleDeinit(void);
#endif
/**
 * @brief mode Struct array
 */
volatile SysModeStruct SysMode[]=
{
#ifdef CFG_APP_IDLE_MODE_EN
	{ModeIdle		,			IdleModeInit,			IdleModeDeinit,			IdleModeRun,		ModeStateReady}, // 0 idle 
#endif
#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	{ModeLeAudioSinkPlay,		DualChipIdleInit,		DualChipIdleDeinit,		DualChipIdleRun,	ModeStateSusend},
	{ModeLeAudioLinein,			DualChipIdleInit,		DualChipIdleDeinit,		DualChipIdleRun,	ModeStateReady},
	{ModeLeAudioBT,				DualChipIdleInit,		DualChipIdleDeinit,		DualChipIdleRun,	ModeStateReady},
#endif
#ifdef CFG_APP_BT_MODE_EN
	{ModeBtAudioPlay,			BtPlayInit, 			BtPlayDeinit,			BtPlayRun,			ModeStateInit  },//BT
#endif
#ifdef CFG_FUNC_APP_USB_CARD_IDLE
#ifdef CFG_APP_USB_PLAY_MODE_EN
	{ModeUDiskAudioPlay,		MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateReady },//USB
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	{ModeCardAudioPlay,			MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateReady  },//SD
#endif
#else
#ifdef CFG_APP_USB_PLAY_MODE_EN
	{ModeUDiskAudioPlay,		MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateSusend },//USB
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	{ModeCardAudioPlay,			MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateSusend  },//SD
#endif
#endif
#ifdef CFG_FUNC_RECORDER_EN
#ifdef CFG_APP_USB_PLAY_MODE_EN
	{ModeUDiskPlayBack,			MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateSusend },//USB
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	{ModeCardPlayBack,			MediaPlayInit,			MediaPlayDeinit,		MediaPlayRun,		ModeStateSusend  },//SD
#endif
#endif
#ifdef CFG_APP_LINEIN_MODE_EN
	{ModeLineAudioPlay,			LineInPlayInit,			LineInPlayDeinit,		LineInPlayRun,		ModeStateReady  },// 4 line in
#endif
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	{ModeUsbDevicePlay,			UsbDevicePlayInit,		UsbDevicePlayDeinit,	UsbDevicePlayRun,	ModeStateSusend},// 5-usb device
#endif
#ifdef CFG_APP_I2SIN_MODE_EN
	{ModeI2SInAudioPlay,		I2SInPlayInit,			I2SInPlayDeinit,		I2SInPlayRun,		ModeStateReady },//I2S 
#endif
#ifdef CFG_APP_RADIOIN_MODE_EN
	{ModeRadioAudioPlay,		RadioPlayInit,			RadioPlayDeinit,		RadioPlayRun,		ModeStateReady},//RADIO
#endif
#if (BT_HFP_SUPPORT && defined(CFG_APP_BT_MODE_EN) && (!defined(BT_HFP_MODE_DISABLE)))
	{ModeBtHfPlay,				BtHfInit,				BtHfDeinit,				BtHfRun,			ModeStateSusend},//ModeBtHfPlay
#endif
#ifdef CFG_APP_OPTICAL_MODE_EN
	{ModeOpticalAudioPlay,		SpdifPlayInit,			SpdifPlayDeinit,		SpdifPlayRun,		ModeStateReady},//10-Optical mode
#endif
#ifdef CFG_APP_COAXIAL_MODE_EN
	{ModeCoaxialAudioPlay,		SpdifPlayInit,			SpdifPlayDeinit,		SpdifPlayRun,		ModeStateReady},//11-Optical mode
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	{ModeHdmiAudioPlay,			HdmiInPlayInit,			HdmiInPlayDeinit,		HdmiInPlayRun,		ModeStateReady},//12-hdmi mode
#endif
#ifdef CFG_APP_WIRELESSIN_MODE_EN
	{ModeWirelessinPlay,		WirelessinPlayInit,		WirelessinPlayDeinit,	WirelessinPlayRun,	ModeStateReady},
#endif
};

const uint32_t ModeKeyInvalid[]=// 
{
#ifdef CFG_APP_IDLE_MODE_EN
	ModeIdle,
#endif
#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	ModeLeAudioSinkPlay,
#endif
#if (BT_HFP_SUPPORT && defined(CFG_APP_BT_MODE_EN))
	ModeBtHfPlay,
#endif


};

/** mask all plusin msg but self **/
#define		PLUSIN_MASK_MODE				(BIT(ModeBtHfPlay) | BIT(ModeTwsSlavePlay) | BIT(ModeIdle) | BIT(ModeLeAudioSinkPlay))

/**
 * @brief plug event  array
 */
const DeviceEventMsgTable DeviceEventMsgTableArray[]=
{
//plug event msg 								support mode										enter mode
#if (BT_HFP_SUPPORT && defined(CFG_APP_BT_MODE_EN) && (!defined(BT_HFP_MODE_DISABLE)))
{MSG_DEVICE_SERVICE_BTHF_IN,        			~(PLUSIN_MASK_MODE),					        	ModeBtHfPlay},
{MSG_DEVICE_SERVICE_BTHF_OUT,					BIT(ModeBtHfPlay),									ENTERR_PREV_MODE},
#endif

#ifndef CFG_FUNC_APP_USB_CARD_IDLE
#ifdef CFG_APP_USB_PLAY_MODE_EN
{MSG_DEVICE_SERVICE_U_DISK_IN,					~(BIT(ModeUDiskAudioPlay) | PLUSIN_MASK_MODE),		ModeUDiskAudioPlay},
{MSG_DEVICE_SERVICE_U_DISK_OUT,					BIT(ModeUDiskAudioPlay),							ENTERR_PREV_MODE},
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
{MSG_DEVICE_SERVICE_CARD_IN, 					~(BIT(ModeCardAudioPlay) | PLUSIN_MASK_MODE),		ModeCardAudioPlay},
{MSG_DEVICE_SERVICE_CARD_OUT, 					BIT(ModeCardAudioPlay),								ENTERR_PREV_MODE},
#endif
#endif

{MSG_DEVICE_SERVICE_USB_DEVICE_IN,				~(BIT(ModeUsbDevicePlay)| PLUSIN_MASK_MODE),		ModeUsbDevicePlay},
{MSG_DEVICE_SERVICE_USB_DEVICE_OUT,				BIT(ModeUsbDevicePlay), 							ENTERR_PREV_MODE},

{MSG_DEVICE_SERVICE_LINE_IN, 					~(BIT(ModeLineAudioPlay) | PLUSIN_MASK_MODE),		ModeLineAudioPlay},
{MSG_DEVICE_SERVICE_LINE_OUT, 					BIT(ModeLineAudioPlay),								ENTERR_PREV_MODE},

{MSG_DEVICE_SERVICE_TWS_SLAVE_CONNECTED, 		~(PLUSIN_MASK_MODE),								ModeTwsSlavePlay},
{MSG_DEVICE_SERVICE_TWS_SLAVE_DISCONNECT, 		BIT(ModeTwsSlavePlay),								ENTERR_PREV_MODE},

#ifdef CFG_FUNC_RECORDER_EN
{MSG_DEVICE_SERVICE_U_DISK_BACK_IN, 			~(BIT(ModeUDiskPlayBack) | PLUSIN_MASK_MODE),		ModeUDiskPlayBack},
{MSG_DEVICE_SERVICE_U_DISK_BACK_OUT, 			BIT(ModeUDiskPlayBack),								ENTERR_PREV_MODE},

{MSG_DEVICE_SERVICE_CARD_BACK_IN, 				~(BIT(ModeCardPlayBack) | PLUSIN_MASK_MODE),		ModeCardPlayBack},
{MSG_DEVICE_SERVICE_CARD_BACK_OUT, 				BIT(ModeCardPlayBack),								ENTERR_PREV_MODE},
#endif

#ifdef CFG_APP_HDMIIN_MODE_EN
{MSG_DEVICE_SERVICE_HDMI_IN, 					~(BIT(ModeHdmiAudioPlay) | PLUSIN_MASK_MODE),		ModeHdmiAudioPlay},
{MSG_DEVICE_SERVICE_HDMI_OUT, 					BIT(ModeHdmiAudioPlay),								ENTERR_PREV_MODE},
#endif

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
{MSG_DEVICE_SERVICE_BIS_SLAVE_CONNECTED, 		~(BIT(ModeLeAudioSinkPlay) | PLUSIN_MASK_MODE),		ModeLeAudioSinkPlay},
{MSG_DEVICE_SERVICE_BIS_SLAVE_DISCONNECT, 		BIT(ModeLeAudioSinkPlay),							ENTERR_PREV_MODE},
#endif

};

const SysModeInputFunction  ModeInputFunction=
{

#ifdef CFG_FUNC_REMIND_SOUND_EN 
	RemindSoundRun,
#else
	NULL,
#endif
#ifdef CFG_APP_USB_PLAY_MODE_EN
	WaitUdiskUnlock,
#else
	NULL,
#endif
#ifdef CFG_APP_CARD_PLAY_MODE_EN
	SDCardForceExitFuc,
#else
	NULL,
#endif
	AudioCoreServiceResume,
};

#define MODE_STR_AND_REMIND_MAX_NUMBER (sizeof(ModeNameStr)/sizeof(SysModeStrAndRemind))
#define SYS_MODE_MAX_NUMBER (sizeof(SysMode)/sizeof(SysModeStruct))
#define MODE_KEY_INVALID_MAX_NUMBER (sizeof(ModeKeyInvalid)/sizeof(uint32_t))
#define SYS_DEVICE_DETECT_MAX_NUMBER (sizeof(DeviceEventMsgTableArray)/sizeof(DeviceEventMsgTable))



