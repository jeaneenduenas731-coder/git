#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "app_config.h"
#include "audio_association.h"
#include "log_info.h"
#include "dac_interface.h"
#include "mcu_circular_buf.h"
#include "sbc_encoder.h"

//this file manage log for event @ interrupt or state/info @ period

/***Event mS for trace£¬ÇÐÎðÆµ·±--×èÈûÏµÍ³****/
/***Select£ºFrameTime/10ms/100mS**/
#define	 EVENT_PERIOD		(1 + (RFPACK_NAUDIO * ONE_FRAME * 1000) / SAMPLE_RATE )//mS
/***Info S for monitor**/
#define	 INFO_PERIOD		1000//mS
/***Statis M for record**/
#define	 STATIS_PERIOD		100000//mS

uint32_t LogEventWord;
uint32_t LogInfoWord;

#if defined(CFG_FUNC_DEBUG_EN) && defined(CFG_WIRELESS_EN)//porting care of

TIMER EventTimer;
TIMER InfoTimer;
TIMER StatisTimer;

void LogTimerInit(void)
{
	TimeOutSet(&EventTimer, EVENT_PERIOD);
	TimeOutSet(&InfoTimer, INFO_PERIOD);
	TimeOutSet(&StatisTimer, STATIS_PERIOD);
}

#if defined(LOG_LOST_TRH) && DECODE_CH != 0
	uint8_t IDList[LOG_LOST_TRH][2];
	uint8_t IDListW = 0;
#endif

#if	defined(LOG_RECV_DELAY) && DECODE_CH != 0
	uint32_t RecvDelay = 0;
#endif
void LogEvent(void)
{
	uint8_t i, j;
	if(IsTimeOut(&EventTimer) && LogEventWord)
	{
		for(i = 0; i < ELOG_NUM; i++)
		{
			switch(LogEventWord & BIT(i))
			{
	#if defined(LOG_LOST_TRH) && DECODE_CH != 0
				case BIT(ELOG_LOST_SEVERAL):
				{
					for(j = 0; j < LOG_LOST_TRH; j++)
						DBG("[%d]%03u@%03u ", j, IDList[(IDListW + j) % LOG_LOST_TRH][0], IDList[(IDListW + j) % LOG_LOST_TRH][1]);
					DBG("\n");
					break;
				}
	#endif
				default:
					break;
			}
		}
		LogEventWord = 0;
		TimeOutSet(&EventTimer, EVENT_PERIOD);
	}

	if(IsTimeOut(&InfoTimer))
	{
		for(i = 0; i < ILOG_NUM; i++)
		{
			switch(LogInfoWord & BIT(i))
			{
	#if ENCODE_CH != 0
				case BIT(ILOG_SEND_EMPTY):
						DBG("SendPack Empty!!!\n");
					break;
	#endif
				default:
					break;
			}
		}
	#if defined(LOG_RECV_SYNC_RST_EN) && DECODE_CH != 0
		{
			static uint16_t Dev1RstCnt = 0;
			if(device1.ConStatus != CONNECT_NONE && Dev1RstCnt != device1.RstNum)
			{
				Dev1RstCnt = device1.RstNum;
				DBG("LRst\n");
			}
		}
	#endif
	#ifdef DEBUG_LOG_AUDIO_MCPS_EN
		extern uint32_t CycCnt;
		extern uint32_t LastDecodeCyc;
		extern uint32_t LastEncodeCyc;
		extern uint16_t g_frame_size;
		static uint16_t McpsMax = 0;
		uint16_t McpsVal = CycCnt * (SAMPLE_RATE / g_frame_size) / 1000000 + 30;//(ERRPack 20MCPS + RF 10MCPS = 30);
		if(McpsVal  > McpsMax)
		{
			McpsMax = McpsVal;
			DBG("AudioMax:%d M\n", McpsVal);
		}
	#endif

		LogInfoWord = 0;
		TimeOutSet(&InfoTimer, INFO_PERIOD);
	}


	if(IsTimeOut(&StatisTimer))
	{
		TimeOutSet(&StatisTimer, STATIS_PERIOD);
	#if	DECODE_CH != 0
		if(device1.ConStatus != CONNECT_NONE)
		{
		#if	defined(LOG_RECV_DELAY)
			DBG("Recv Delay:%u\n", RecvDelay);
		#endif
			DBG("Dev1 Play:%lu Recv:%lu PL:%lu Rst:%u\n", device1.PlayFrame, device1.RecvNum, device1.PlcFrame, device1.RstNum);
		}
		#ifndef CFG_SYNC_TO_TX_SDK //porting care of
		if(device2.ConStatus != CONNECT_NONE)
		{
			DBG("Dev2 Play:%lu Recv:%lu PL:%lu Rst:%u\n", device2.PlayFrame, device2.RecvNum, device2.PlcFrame, device2.RstNum);
		}
		#endif
	#endif
	}
}
#endif //defined(DEBUG_LOG_EN) || defined(CFG_FUNC_USBDEBUG_EN)

/**********************************ASS_DBG API********************/
#if defined(DEBUG_PACKET_ID_EN) || defined(LOG_LOST_TRH) || defined(LOG_RECV_DELAY) && DECODE_CH != 0
	__attribute__((section(".tcm_section")))
	void AssDbgRecvPacket_isr(uint8_t Device, uint8_t *Buf)
	{
	#if defined(LOG_LOST_TRH)
		IDList[IDListW][0] = Device1Info.RecvID;
		IDList[IDListW][1] = Device1Info.CurID;
		IDListW = (IDListW+ 1) % LOG_LOST_TRH;
	#endif
	#if defined(LOG_RECV_DELAY)
		extern MCU_CIRCULAR_CONTEXT         MicLeftCircularBuf;
		RecvDelay = MCUCircular_GetDataLen(&MicLeftCircularBuf)/(SBC_DEC_LEN_PER_FREME + PACKET_CNT_LEN) * ONE_FRAME;
		RecvDelay += AudioDAC0_DataLenGet();
	#endif
	}
	void AssDbgPlayIndex(uint8_t Device)
	{
	#if defined(LOG_LOST_TRH)
		if(Device1Info.MissPackCnt == LOG_LOST_TRH / 2)
		{
			SET_EVENT_FLAG(ELOG_LOST_SEVERAL);
		}
	#endif
	}
	//config£ºASS_DBG_ID_MISS
	void AssDbgMissGet(uint8_t Device)
	{
		if(Device==DEVICE1_MASK)
		{
			;
		}
		if(Device==DEVICE2_MASK)
		{
			;
		}
	}
#endif
