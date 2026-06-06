/**
 **************************************************************************************
 * @file    mode_task.h
 * @brief  sys_mode
 *
 * @author  kenbu/bkd
 * @version V0.0.1
 *
 * $Created: 2021-02-26
 *
 * @Copyright (C) 2021, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __MODE_TASK_H__
#define __MODE_TASK_H__
#include "rtos_api.h"
#include "app_message.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define ResourceCard	1
#define ResourceUDisk	2
#define CHANGE_MODE_TIMEOUT_COUNT			600//MS
#define CHANGE_MODE_SYS_REMIND				2000

typedef struct _SysModeContext
{
	xTaskHandle			taskHandle;
	MessageHandle		msgHandle;
}SysModeContext;

typedef enum
{
	SysResourceCard=1,
	SysResourceUDisk,
	SysResourceUsbDevice,
	SysResourceBtOTA,
}ResourceType;

typedef enum
{
	ModeStateSusend	= 0,// mode not run by mode key
	ModeStateReady,//mode can run by mode key
	ModeStateInit,//mode run after init immediately
	ModeStateRunning,
	ModeStateDeinit,// mode exit
}SysModeState;

typedef enum
{
	ModeIdle = 0,
	ModeBtAudioPlay,
	ModeUDiskAudioPlay,
	ModeCardAudioPlay,
	ModeLineAudioPlay,
	ModeUsbDevicePlay,//5
	
	ModeI2SInAudioPlay,// 6
	ModeRadioAudioPlay,
	ModeBtHfPlay, 
	ModeTwsSlavePlay,
	ModeOpticalAudioPlay,// 10

	ModeCoaxialAudioPlay,
	ModeHdmiAudioPlay,
	ModeWirelessinPlay, //¹ØÁª CFG_APP_WIRELESSIN_MODE_EN + CFG_WIRELESS_EN
	ModeUDiskPlayBack,
	ModeCardPlayBack,

	ModeLeAudioSinkPlay,
	ModeLeAudioLinein,
	ModeLeAudioBT,

	SysModeMax,
}SysModeNumber;
#define ENTERR_PREV_MODE	SysModeMax

typedef struct _SysModeStruct
{

SysModeNumber ModeNumber;
bool (*SysModeInit)(void);
bool (*SysModeDeInit)(void);
void (*SysModeRun)(uint16_t);
SysModeState ModeState;

}SysModeStruct;


typedef struct _SysModeInputFunction
{

bool (*RemindRun)(SysModeState);
void (*UdiskUnlock)(void);
void (*SDCardForceExit)(void);
void (*AudioCoreResume)(void);

}SysModeInputFunction;

typedef struct _SysModeStrAndRemind
{
SysModeNumber ModeNumber;
char ModeName[16];
}SysModeStrAndRemind;


MessageHandle GetSysModeMsgHandle(void);
void SetSysModeState(SysModeNumber sys_mode,SysModeState sys_mode_state);
SysModeState GetSysModeState(SysModeNumber sys_mode);
void SysModeTaskCreat(void);
void SysModeGenerate(uint16_t Msg);
void SysModeChangeTimeoutProcess(void);
void SysModeEnter(SysModeNumber SetMode);
void SendModeKeyMsg(void);
bool GetModeDefineState(SysModeNumber sys_mode);
bool SysCurModeReboot(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__MODE_TASK_H__

