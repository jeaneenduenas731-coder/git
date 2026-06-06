/*
 * wireless_audio_service.c
 *
 *  Created on: Dec 6, 2024
 *      Author: piwang
 */

#include "type.h"
#include "app_config.h"
#include "bt_config.h"
#include "app_message.h"
#include "irqn.h"
#include "watchdog.h"
#include "clk.h"
#include "main_task.h"
#include "audio_core_service.h"
#include "wireless_service.h"
#include "wireless_bb_api.h"
#include "mcu_circular_buf.h"
#include "audio_association.h"
// #include "wireless_config.h"

#ifdef CFG_WIRELESS_EN


#define WIRELESS_SERVICE_STACK_SIZE		768
#define WIRELESS_SERVICE_PRIO			4
#define WIRELESS_NUM_MESSAGE_QUEUE		20

typedef struct _WirelessServiceContext
{
	xTaskHandle			taskHandle;
	MessageHandle		msgHandle;

#if defined(CFG_WIRELESS_OUT_EN) || (defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
	MCU_CIRCULAR_CONTEXT	AudioOutFifoCt;
	bool AudioOutReady;
#endif

#ifdef CFG_APP_WIRELESSIN_MODE_EN
	bool AudioInReady;
#endif
	bool				RFInited;
	uint8_t				bbErrorMode;
	uint32_t			bbErrorType;

}WirelessServiceContext;

static WirelessServiceContext	*WirelessServiceCt = NULL;

/**************************************************************************
 *
 **************************************************************************/
/**
 * @brief	Get message receive handle of Wireless manager
 * @param	NONE
 * @return	MessageHandle
 */
xTaskHandle GetWirelessServiceTaskHandle(void)
{
	if(!WirelessServiceCt)
		return NULL;

	return WirelessServiceCt->taskHandle;
}

uint8_t GetWirelessServiceTaskPrio(void)
{
	return WIRELESS_SERVICE_PRIO;
}

MessageHandle GetWirelessServiceMsgHandle(void)
{
	if(!WirelessServiceCt)
		return NULL;

	return WirelessServiceCt->msgHandle;
}

void WirelessServiceMsgSend(uint16_t msgId)
{
	MessageContext		msgSend;
	msgSend.msgId = msgId;
	if(WirelessServiceCt)
		MessageSend(WirelessServiceCt->msgHandle, &msgSend);
}

bool WirelessRFInited(void)
{
	if(!WirelessServiceCt || !WirelessServiceCt->RFInited)
		return FALSE;
	return TRUE;
}

extern unsigned char audio_init_isready;
void WirelessInfo(void);
/***********************************************************************************
 * 2.4G wireless任务处理
 **********************************************************************************/
static void WirelessServiceEntrance(void * param)
{
	MessageContext		msgRecv;

/******************方案scheme配置 sync@2.4G main.c***************************/
	SchemeInit();
/**************************************************************/

	APP_DBG("WirelessServiceEntrance *****%s-%s******\n", TURNKEY_NAME,
#ifdef CFG_SYNC_TO_TX_SDK
		"T"
#else
		"R"
#endif
	);
	WirelessServiceCt->RFInited = TRUE;
	while(1)
	{
		MessageRecv(WirelessServiceCt->msgHandle, &msgRecv, 1);

		TargetLock();
		WirelessMsgProcess(msgRecv.msgId);
#if defined(CFG_WIRELESS_OUT_EN) || (defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
		if(device1.ConStatus != CONNECT_NONE)
		{
			WirelessServiceCt->AudioOutReady = TRUE;
		}
		else
		{
			WirelessServiceCt->AudioOutReady = FALSE;
		}
		wireless_out_process();
#endif
#if defined(KEY_REMOTE) && !defined(CFG_SYNC_TO_TX_SDK)
		cmd_process();
#endif
#ifdef CFG_SOC_CALIBRATION_EN
		soc_cal_end_restart();
#endif
		WirelessInfo();
#if defined(CFG_FUNC_DEBUG_EN)//Porting care of
		LogEvent();
#endif
		if(GetAudioCoreServiceState() == TaskStatePaused)
		{
			MessageContext		msgSend;
			msgSend.msgId		= MSG_NONE;
			MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);
		}
	}
}

/************************************************************************************
 * @brief	wireless service initial.
 * @param	NONE
 * @return
 ***********************************************************************************/
static bool WirelessServiceInit(void)
{
	APP_DBG("wireless service init.\n");

	WirelessServiceCt = (WirelessServiceContext*)osPortMalloc(sizeof(WirelessServiceContext));
	if(WirelessServiceCt == NULL)
	{
		DBG("WirelessServiceCt err\n");
		return FALSE;
	}
	memset(WirelessServiceCt, 0, sizeof(WirelessServiceContext));
//	//rfsend_buffer?
//	WirelessConfigParams = osPortMalloc(RFPACKET_FRAME_LEN(RFAUDIO_LEN_PER_FREME));
//	if(WirelessConfigParams == NULL)
//	{
//		return FALSE;
//	}
//	memset(WirelessConfigParams, 0, RFPACKET_FRAME_LEN(RFAUDIO_LEN_PER_FREME));

#if defined(CFG_WIRELESS_OUT_EN)||(defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
	uint8_t *OutFifo = osPortMalloc(WIRELESS_OUT_FIFO_LEN);
	if(OutFifo == NULL)
	{
		osPortFree(WirelessServiceCt);
		DBG("Wireless outfifo err\n");
		WirelessServiceCt = NULL;
		return FALSE;
	}
	MCUCircular_Config(&WirelessServiceCt->AudioOutFifoCt, OutFifo, WIRELESS_OUT_FIFO_LEN);
#endif

	WirelessServiceCt->msgHandle = MessageRegister(WIRELESS_NUM_MESSAGE_QUEUE);
	if(WirelessServiceCt->msgHandle == NULL)
	{
		DBG("Wireless msgHandle err\n");
		return FALSE;
	}
#ifdef CFG_SYNC_TO_TX_SDK
	wireless_audio_init_tx();
#else
	wireless_audio_init_rx();
#endif
	return TRUE;
}

/************************************************************************************
 * @brief	Start Wireless service.
 * @param	NONE
 * @return
 ***********************************************************************************/
bool WirelessServiceStart(void)
{
	bool		ret = TRUE;

//	memset((uint8_t*)BB_EM_MAP_ADDR, 0, BB_EM_SIZE);//clear em erea


	ret = WirelessServiceInit();

	if(ret)
	{
		WirelessServiceCt->taskHandle = NULL;

		xTaskCreate(WirelessServiceEntrance,
					"WirelessStack",
					WIRELESS_SERVICE_STACK_SIZE,
					NULL,
					WIRELESS_SERVICE_PRIO,
					&WirelessServiceCt->taskHandle);
		if(WirelessServiceCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}

	if(!ret)
		APP_DBG("Wireless service create fail!\n");

//	if(wireless2_GetSleepMode())
//	{
//		DBG("*******wireless2 Active\n");
//		wireless2_ModeActive();
//	}
	return ret;
}

/************************************************************************************
 * @brief	Kill wireless service.
 * @param	NONE
 * @return
 ***********************************************************************************/
bool WirelessServiceKill(void)
{
	int32_t ret = 0;
	if(WirelessServiceCt == NULL)
	{
		return FALSE;
	}
#if defined(CFG_WIRELESS_OUT_EN) || (defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
	WirelessServiceCt->AudioOutReady = FALSE;
	if(WirelessServiceCt->AudioOutFifoCt.CircularBuf)
	{
		osPortFree(WirelessServiceCt->AudioOutFifoCt.CircularBuf);
		WirelessServiceCt->AudioOutFifoCt.CircularBuf = NULL;
	}
#endif
	wireless2_SetRfStopMode();
	DBG("Wireless2 RF Stop!\n");
	NVIC_DisableIRQ(BT_IRQn);

	//Wireless Service
	//Msgbox
	if(WirelessServiceCt->msgHandle)
	{
		MessageDeregister(WirelessServiceCt->msgHandle);
		WirelessServiceCt->msgHandle = NULL;
	}

	//task
	if(WirelessServiceCt->taskHandle)
	{
		vTaskDelete(WirelessServiceCt->taskHandle);
		WirelessServiceCt->taskHandle = NULL;
	}

//	if(WirelessConfigParams)
//	{
//		osPortFree(WirelessConfigParams);
//		WirelessConfigParams = NULL;
//	}

	if(WirelessServiceCt)
	{
		osPortFree(WirelessServiceCt);
		WirelessServiceCt = NULL;
	}
	audio_init_isready = 0;

	APP_DBG("!!WirelessServiceCt\n");

	return TRUE;
}

uint8_t BtWirelessFlagGet(void)
{
	if(GetWirelessServiceTaskHandle() != NULL)
	{
		return 1;
	}
	return 0;
}

/***wireless audio module api preset ***/
__attribute__((weak)) uint16_t WirelessInSpaceLenGet(void)
{
	return 0;
}

__attribute__((weak)) uint16_t WirelessInDataLenGet(void)
{
	return 0;
}

__attribute__((weak)) uint16_t WirelessInDataSet(int16_t Buf, uint32_t Samples)
{
	return 0;
}
#if defined(CFG_WIRELESS_OUT_EN) || (defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
uint16_t WirelessOutDataLenGet(void)
{
	if(GetWirelessServiceTaskHandle() != NULL && WirelessServiceCt->AudioOutReady)
	{
		return MCUCircular_GetDataLen(&WirelessServiceCt->AudioOutFifoCt) / WIRELESS_OUT_SAMPLES_BYTE;
	}
	return 0;
}

uint16_t WirelessOutDataGet(int16_t *Buf, uint32_t Samples)
{
	if(GetWirelessServiceTaskHandle() != NULL && WirelessServiceCt->AudioOutReady)
	{
		return MCUCircular_GetData(&WirelessServiceCt->AudioOutFifoCt, Buf, Samples * WIRELESS_OUT_SAMPLES_BYTE) / WIRELESS_OUT_SAMPLES_BYTE;
	}
	return 0;
}

uint16_t WirelessOutSpaceLenGet(void)
{
	if(GetWirelessServiceTaskHandle() != NULL && WirelessServiceCt->AudioOutReady)
	{
		return MCUCircular_GetSpaceLen(&WirelessServiceCt->AudioOutFifoCt) / WIRELESS_OUT_SAMPLES_BYTE;
	}
	return WIRELESS_OUT_FIFO_SAMPLES - 1;
}

uint16_t WirelessOutDataSet(int16_t *Buf, uint32_t Samples)
{
	if(GetWirelessServiceTaskHandle() != NULL && WirelessServiceCt->AudioOutReady)
	{
		MCUCircular_PutData(&WirelessServiceCt->AudioOutFifoCt, Buf, Samples * WIRELESS_OUT_SAMPLES_BYTE);
		return Samples;
	}
	return 0;
}
#endif

/****************************************************************************************
 *                 wireless_config.h与app_config.h组合宏编译警告
 ****************************************************************************************/
#if defined(CFG_WIRELESS_OUT_EN) && defined(CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000)
	#error	"Audio Out samplerate conflict!!!"
#endif

/*开播水位不低于：1.5倍，假定音效帧256, 1.5 * 音效帧 / 传输帧，以免定比帧失期*/
#if (DECODE_CH != 0) && (defined(CFG_FUNC_AUDIO_EFFECT_EN) && !defined(CFG_FUNC_EFFECT_BYPASS_EN)) && (WIRELESS_RECV_FIFO_THRHLD * 2 < ((3 * 256 / ONE_FRAME)))
	#error	"Wireless receive THRHLD error for effect frame256!!!"
#elif (DECODE_CH != 0) && (!defined(CFG_FUNC_AUDIO_EFFECT_EN) || defined(CFG_FUNC_EFFECT_BYPASS_EN)) && (WIRELESS_RECV_FIFO_THRHLD * 2 < ((3 * RFPACK_NAUDIO)))
	#error	"Wireless receive THRHLD error for OS!!!"
#endif

#if defined(BP_SAVE_TO_FLASH)
	#warning "wireless disconnect @ save flash!!!"
#endif

#if defined(CFG_FUNC_AUDIO_EFFECT_EN) && !defined(CFG_FUNC_EFFECT_BYPASS_EN) && defined(CFG_AUDIO_WIDTH_24BIT) && (defined(WIRELESS_TURNKEY1_4) || defined(WIRELESS_TURNKEY4_1))
	#warning "Dac maybe noise @ effect graphic mix!!!"
#endif
#endif//#ifdef CFG_WIRELESS_EN
