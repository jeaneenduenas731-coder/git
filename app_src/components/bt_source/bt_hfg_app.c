/**
 *******************************************************************************
 * @file    bt_hfg_app.c
 * @author  KK
 * @version V1.0.1
 * @date    30-Dec-2021
 * @brief   Hfg callback events and actions
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "type.h"
#include "debug.h"
#ifdef CFG_APP_CONFIG
#include "app_config.h"
#include "main_task.h"
//#include "mode_switch_api.h"
#endif
#include "bt_config.h"

#include "bt_manager.h"
//#include "bt_app_interface.h"
#include "app_message.h"
#include "bt_manager.h"
#include "delay.h"

#if BT_HFG_SUPPORT
#include "bt_hfg_api.h"
#include "bt_hfp_api.h"
#include "bt_interface.h"

void BtHfgSetCurrNumber(char *number);
void BtHfgSetCallState(uint8_t dir, uint8_t state);
extern void AppA2dpSuspendStream(void);
extern void AppA2dpStartStream(void);

static void SetSourceHfgState(BT_HFP_STATE state)
{
	GetBtManager()->hfgState = state;
}

BT_HFP_STATE GetSourceHfgState(void)
{
	return GetBtManager()->hfgState;
}

#define HFG_IND_CALL 2		  //0: no call  1: active call
#define HFG_IND_CALL_SETUP 3 //1: incoming  2: outgoing

uint8_t testBuf[120];
/****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfgCallback(BT_HFG_CALLBACK_EVENT event, BT_HFG_CALLBACK_PARAMS * param)
{
	switch(event)
	{
		case BT_STACK_EVENT_HFG_CONNECTED:
			{
				APP_DBG("Hfg Connected : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
						(param->params.bd_addr)[0],
						(param->params.bd_addr)[1],
						(param->params.bd_addr)[2],
						(param->params.bd_addr)[3],
						(param->params.bd_addr)[4],
						(param->params.bd_addr)[5]);
				SetSourceHfgState(BT_HFP_STATE_CONNECTED);
				if((param->params.bd_addr)[0] || (param->params.bd_addr)[1] || (param->params.bd_addr)[2] 
					|| (param->params.bd_addr)[3] || (param->params.bd_addr)[4] || (param->params.bd_addr)[5])
				{
					extern void BtSourceCheckConn(uint8_t *Addr,uint32_t timeout);
					memcpy(btManager.remoteAddr, param->params.bd_addr, 6);
					BtSourceCheckConn(btManager.remoteAddr,3500);

					extern void BtReconnectDevStop(void);
					BtReconnectDevStop();
				}
			    Hfgmicset(10);
			}
			break;

		case BT_STACK_EVENT_HFG_DISCONNECTED:
			{
				//BtHfgHangUp();
				SetSourceHfgState(BT_HFP_STATE_NONE);
				APP_DBG("Hfg disconnect\n");
				hfgSetIndicator(HFG_IND_CALL_SETUP,0);
				hfgSetIndicator(HFG_IND_CALL,0);
				if(GetSystemMode() == ModeBtHfPlay)
				{
					extern void BtHfModeExit(void);
					BtHfModeExit();
				}
				BthfgCallStateClear();
			}
			break;

		case BT_STACK_EVENT_HFG_FEATURE:
			{
				APP_DBG("Hfg feature %x\n", (unsigned int)param->params.features);
			}
			break;


		case BT_STACK_EVENT_CALL_NUMBER:
			APP_DBG("BT_STACK_EVENT_CALL_NUMBER:%s\n",param->params.scoReceivedData);
//			BtHfgCreateCall();
			BthfgOutgoingWait((char *)param->params.scoReceivedData);
		break;

		case BT_STACK_EVENT_HFG_HANG_UP:
			APP_DBG("BT_STACK_EVENT_HFG_HANG_UP\n");
			BtHfgHangUp();
			break;

		case BT_STACK_EVENT_HFG_ANSWER_CALL:
			APP_DBG("BT_STACK_EVENT_HFG_ANSWER_CALL\n");
			BthfgAnswer(0);//接听呼入电话
			break;

		case BT_STACK_EVENT_HFG_CALL_HOLD:
			APP_DBG("BT_STACK_EVENT_HFG_CALL_HOLD\n");
			BthfgAnswer(0);//接听呼入电话
			break;

		case BT_STACK_EVENT_HFG_SCO_CONNECTED:
			APP_DBG("BT_STACK_EVENT_HFG_SCO_CONNECTED\n");
			SetSourceHfgState(BT_HFP_STATE_ACTIVE);
			SetScoConnectFlag(TRUE);

			if(GetSystemMode() != ModeBtHfPlay)
			{
				AppA2dpSuspendStream();
				extern void BtHfModeEnter(void);
				BtHfModeEnter();
			}
			break;

		case BT_STACK_EVENT_HFG_SCO_DISCONNECTED:
			APP_DBG("BT_STACK_EVENT_HFG_SCO_DISCONNECTED\n");
			SetSourceHfgState(BT_HFP_STATE_CONNECTED);
			SetScoConnectFlag(FALSE);

			if(GetSystemMode() == ModeBtHfPlay)
			{
				extern void BtHfModeExit(void);
				BtHfModeExit();
				AppA2dpStartStream();
			}
			break;

		case BT_STACK_EVENT_HFG_GENERATE_DTMF:
			APP_DBG("BT_STACK_EVENT_HFG_GENERATE_DTMF %d\n",param->params.DtmfDate);
			break;

		case BT_STACK_EVENT_HFG_SCO_DATA_RECEIVED:

			btManager.hfpScoCodecType[0] = HFP_AUDIO_DATA_PCM;
			if(SaveHfpScoDataToBuffer)
				SaveHfpScoDataToBuffer(param->params.scoReceivedData,param->paramsLen);
			else
			{
				memset(&testBuf,0,param->paramsLen);
				HfgSendScoData((uint8_t *)&testBuf, param->paramsLen);
			}
//			APP_DBG("BT_STACK_EVENT_HFG_SCO_DATA_RECEIVED\n");
			break;

/***************************************************************************************/
		default:
			break;
	}
}

/***************************************************************************
 **************************************************************************/
void BtHfgCreateCall(void)
{
	APP_DBG("~~BtHfgCreateCall\n");
	if(GetSourceHfgState() >= BT_HFP_STATE_CONNECTED)
	{
		//hfgSetIndicator(HFG_IND_CALL,1);
		if(hfgSetIndicator(HFG_IND_CALL_SETUP,2)!=1)//outgoing
		{
			APP_DBG("~~HfgAudioConnect\n");
			HfgAudioConnect();//audio sco connect
		}
	}
}

void BthfgIncomingWait(char *number)//呼入等待
{
	if(GetSourceHfgState() < BT_HFP_STATE_CONNECTED) return;
	SetSourceHfgState(BT_HFP_STATE_INCOMING);
	APP_DBG("IncomingWait\n");
	hfgSetIndicator(HFG_IND_CALL_SETUP,1);
	HfgSetCallIndex(1);
	BtHfgSetCallState(1, 4);
	BtHfgSetCurrNumber(number);
	WaitMs(10);
	HfgRing();
	WaitMs(10);
	Hfgincoming();
	HfgAudioConnect();//audio sco connect
}

void BthfgOutgoingWait(char *number)//呼出等待
{
	if(GetSourceHfgState() < BT_HFP_STATE_CONNECTED) return;
	SetSourceHfgState(BT_HFP_STATE_OUTGOING);
	APP_DBG("OutgoingWait\n");
	hfgSetIndicator(HFG_IND_CALL_SETUP,2);
	HfgSetCallIndex(1);
	BtHfgSetCallState(0, 2);
	BtHfgSetCurrNumber(number);
	HfgAudioConnect();//audio sco connect
}

void BthfgAnswer(uint8_t flag)//接听电话
{
	if(GetSourceHfgState() < BT_HFP_STATE_CONNECTED) return;
	APP_DBG("BthfgAnswer\n");
	if(hfgSetIndicator(HFG_IND_CALL,1)!=1)
	{
//		HfgAudioConnect();//audio sco connect
	}
	if(flag)
	{
		BtHfgSetCallState(0, 0);//接听呼出电话
	}
	else
	{
		BtHfgSetCallState(1, 0);//接听呼入电话
		hfgSetIndicator(HFG_IND_CALL_SETUP,0);
	}
}

void BthfgCallStateClear(void)//清除当前通话状态
{
	HfgSetCallIndex(0);
	HfgSetCallState(0xff, 0xff);
	BtHfgSetCurrNumber("0");
}

void BtHfgSetCallState(uint8_t dir, uint8_t state)
{
	HfgSetCallState(dir, state);
}

void BtHfgSetCurrNumber(char *number)
{
	HfgSetCurrNumber(number);
}

void BtHfgHangUp(void)
{
	if(GetSourceHfgState() > BT_HFP_STATE_CONNECTED)
	{
//		ScoReConnFlag = 0;
//		WaitMs(100);
		if(!hfgSetIndicator(HFG_IND_CALL_SETUP,0)!=1)//
		{
			WaitMs(1);
			//APP_DBG("~~HfgAudioDiconnect\n");
			HfgAudioDiconnect();//audio sco disconnect
		}
		BthfgCallStateClear();
		WaitMs(10);
		hfgSetIndicator(HFG_IND_CALL,0);
	}
}

#endif

