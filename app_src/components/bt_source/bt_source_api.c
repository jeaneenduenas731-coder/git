#include "type.h"
#include "app_config.h"
#include "bt_config.h"
#include "app_message.h"
#include "irqn.h"
#include "watchdog.h"
#include "clk.h"
#include "remind_sound.h"

#include "bb_api.h"
#include "bt_em_config.h"
#include "bt_app_init.h"
#include "bt_stack_service.h"
#include "bt_stack_memory.h"

#include "bt_common_api.h"
#include "bt_manager.h"
#include "main_task.h"
#include "bt_interface.h"
#include "audio_core_service.h"

#include "bt_app_ddb_info.h"
#include "bt_app_connect.h"
#include "bt_app_common.h"
#include "bt_play_mode.h"

#if BT_SOURCE_SUPPORT

extern uint8_t A2dpSourceDisconnect(void);
volatile uint32_t gSwitchSourceAndSink = A2DP_SET_SOURCE;

static uint8_t BtSourceGetNameCnt = 0;

void SetBtSinkSourceRole(uint32_t role)
{
	BtDisconnectWaitComplete();
	vTaskDelay(50);
	//gSwitchSourceAndSink=role;
	BtSourceInquiryStop();
	BtPowerOff();
	vTaskDelay(10);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	vTaskDelay(50);

	gSwitchSourceAndSink=role;
	BtStackServiceStart();
}

void SendQuiryCommand(void)
{
	//		return;
	//	if(mainAppCt.state == TaskStateRunning ||(mainAppCt.state == TaskStatePausing))
	if(gSwitchSourceAndSink == A2DP_SET_SOURCE)
	{
		if((GetSourceA2dpState() < BT_A2DP_STATE_CONNECTED)&&(GetSourceHfgState() < BT_HFP_STATE_CONNECTED) && (GetAvrcpState(0) < BT_AVRCP_STATE_CONNECTED))
		{
			APP_DBG("Inquiry start\n");//BKDBKD
			BtSourceInquiryStart();
			BTInquiry(INQUIRY_MODE_EXTENDED);//, A2DP_INQUIRY_TIMEOUT);
		}
	}
}

void BtSourceNameGetChack(void)
{
	if(btManager.GetNameFlag == 2)
	{
		BtSourceGetNameCnt++;
	}
	else
	{
		BtSourceGetNameCnt = 0;
	}
	if(BtSourceGetNameCnt>=200)//200ms
	{
		APP_DBG("BtSourceNameGetChack\n");
		btManager.GetNameFlag = 0;
		BtSourceGetNameCnt = 0;
		BtSourceNameRequest();
	}
}


uint8_t SetBtSourceDefaultAccessMode(void)
{
	if(gSwitchSourceAndSink == A2DP_SET_SOURCE)
	{
#ifdef SOURCE_ACCESS_MODE_ENABLE
		GetBtManager()->btAccessModeEnable = POWER_ON_BT_ACCESS_MODE_SET;
#else
		GetBtManager()->btAccessModeEnable = BT_ACCESSBLE_CONNECTABLEONLY;
#endif
		return TRUE;
	}
	return FALSE;
}

void BtSourceBtModeInit(void)
{
#ifdef	SUPPORT_SOURCE_SINK_AUTO_SWITCH_FUNC
	if(gSwitchSourceAndSink==A2DP_SET_SOURCE)
	{
		SetBtSinkSourceRole(A2DP_SET_SINK);
	}
	BtFastPowerOn();
	AudioCoreSinkDisable(AUDIO_BT_SOURCE_SINK_NUM);//sink模式关闭source通道
#endif
}

void BtSourceBtModeDeinit(void)
{
#ifdef	SUPPORT_SOURCE_SINK_AUTO_SWITCH_FUNC
	if(GetSysModeState(ModeBtHfPlay) != ModeStateInit && GetSysModeState(ModeTwsSlavePlay) != ModeStateInit)
	{
		SetBtSinkSourceRole(A2DP_SET_SOURCE);
	}
	if(GetSysModeState(ModeBtHfPlay) != ModeStateInit && GetSysModeState(ModeTwsSlavePlay) != ModeStateInit)
	{
		BtFastPowerOff();
	}
#endif
}

uint8_t BtSourceReconnectDevExcute(void)
{
	extern unsigned char A2dpSourceConnect(unsigned char * addr);
	extern uint32_t GetSourceSupportProfiles(void);
	uint32_t reconnectProfile = 0;
	if(gSwitchSourceAndSink == A2DP_SET_SOURCE)//source
	{
		reconnectProfile = GetSourceSupportProfiles();
		reconnectProfile &= (BT_PROFILE_SUPPORTED_HFG | BT_PROFILE_SUPPORTED_A2DP | BT_PROFILE_SUPPORTED_AVRCP);

		if((reconnectProfile & BT_PROFILE_SUPPORTED_A2DP)&&(GetSourceA2dpState() < BT_A2DP_STATE_CONNECTED))
		{

			A2dpSourceConnect(btManager.conRemoteAddr);
			APP_DBG("--A2dpSourceConnect :	[%02x:%02x:%02x:%02x:%02x:%02x]\n",
					btManager.conRemoteAddr[0],
					btManager.conRemoteAddr[1],
					btManager.conRemoteAddr[2],
					btManager.conRemoteAddr[3],
					btManager.conRemoteAddr[4],
					btManager.conRemoteAddr[5]
				);
		}
		else if((reconnectProfile & BT_PROFILE_SUPPORTED_AVRCP)&&(GetAvrcpState(0) != BT_AVRCP_STATE_CONNECTED))
		{
			reconnectProfile &= ~(BT_PROFILE_SUPPORTED_A2DP);

			AvrcpConnect(0, GetBtManager()->conRemoteAddr);
			APP_DBG("--AvrcpConnect :  [%02x:%02x:%02x:%02x:%02x:%02x]\n",
					GetBtManager()->conRemoteAddr[0],
					GetBtManager()->conRemoteAddr[1],
					GetBtManager()->conRemoteAddr[2],
					GetBtManager()->conRemoteAddr[3],
					GetBtManager()->conRemoteAddr[4],
					GetBtManager()->conRemoteAddr[5]
				);
		}
#if BT_HFG_SUPPORT == 1
		else if((reconnectProfile & BT_PROFILE_SUPPORTED_HFG)&&(GetSourceHfgState() < BT_HFP_STATE_CONNECTED))
		{
			reconnectProfile &= ~(BT_PROFILE_SUPPORTED_AVRCP | BT_PROFILE_SUPPORTED_A2DP);
			HfgConnect(GetBtManager()->conRemoteAddr);
			APP_DBG("--HfgConnect :  [%02x:%02x:%02x:%02x:%02x:%02x]\n",
					GetBtManager()->conRemoteAddr[0],
					GetBtManager()->conRemoteAddr[1],
					GetBtManager()->conRemoteAddr[2],
					GetBtManager()->conRemoteAddr[3],
					GetBtManager()->conRemoteAddr[4],
					GetBtManager()->conRemoteAddr[5]
				);
			//hfg只连接一次
			reconnectProfile &= ~(BT_PROFILE_SUPPORTED_HFG | BT_PROFILE_SUPPORTED_A2DP | BT_PROFILE_SUPPORTED_AVRCP);
		}
#endif
		else
		{
			reconnectProfile &= ~(BT_PROFILE_SUPPORTED_HFG | BT_PROFILE_SUPPORTED_A2DP | BT_PROFILE_SUPPORTED_AVRCP);

#ifdef BT_RECONNECTION_FUNC// source
			BtStopReconnect();
#endif
		}

		return TRUE;
	}
	return FALSE;
}


void BtSourcePublicMsgPross(uint32_t msgID)
{
	switch(msgID)
	{
	default:
		break;
	case MSG_BT_SOURCE_INQUIRY:
		if(gSwitchSourceAndSink != A2DP_SET_SOURCE)
			break;
		BtSourceInquiryStart();
		if(mainAppCt.state == TaskStateRunning ||(mainAppCt.state == TaskStatePausing))
		{
			if(GetSourceA2dpState()<BT_A2DP_STATE_CONNECTED)
			{
				APP_DBG("Inquiry start\n");
				BTInquiry(INQUIRY_MODE_EXTENDED);
			}
		}
		break;
	case MSG_BT_SOURCE_DISCONNECT:
		if(GetSourceA2dpState()>=BT_A2DP_STATE_CONNECTED)
		{
			A2dpSourceDisconnect();
		}
		break;
	case MSG_BT_SOURCE_CONNECT:
		{
			APP_DBG("MSG_BT_SOURCE_CONNECT\n");
		}
		break;

	case MSG_BT_SOURCE_SINK_SWITCH:
		{
			APP_DBG("MSG_BT_SOURCE_SINK_SWITCH\n");
			if(gSwitchSourceAndSink == A2DP_SET_SINK)
			{
				SetBtSinkSourceRole(A2DP_SET_SOURCE);
			}
			else
			{
				SetBtSinkSourceRole(A2DP_SET_SINK);
			}
		}
		break;

#if BT_HFG_SUPPORT
	case MSG_BT_SOURCE_HFG_OUTGOING:
		BthfgOutgoingWait("12345678900");
		break;
	case MSG_BT_SOURCE_HFG_INCOMING:
		BthfgIncomingWait("12345678911");
		break;

	case MSG_BT_SOURCE_HFG_ANSWER:
		BthfgAnswer(0);
		break;

	case MSG_BT_SOURCE_HFG_HANG_UP:
		BtHfgHangUp();
		break;
#endif
	}
}

#ifdef SOURCE_AUTO_INQUIRY_AND_CONNECT
TIMER QuiryDelayTimer;

void QuiryDelayTimerSet(uint32_t timeout)
{
	TimeOutSet(&QuiryDelayTimer,timeout);
}

void BtSourceReInquiryChack(void)
{
	extern uint8_t gBtSourceInquiryStart;
	if(!gBtSourceInquiryStart && btManager.btReconnectTimer.timerFlag == TIMER_UNUSED)
	{
		if(IsTimeOut(&QuiryDelayTimer))
		{
			TimeOutSet(&QuiryDelayTimer,3000);
			if(gSwitchSourceAndSink == A2DP_SET_SOURCE)
			{
				APP_DBG("BtSourceReInquiryChack Inquiry\n");
				SendQuiryCommand();
			}
		}
	}
	else
	{
		TimeOutSet(&QuiryDelayTimer,3000);
	}
}
#endif


#endif
