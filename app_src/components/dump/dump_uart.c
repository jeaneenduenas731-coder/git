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
#include "type.h"
#include "app_config.h"
#include "timeout.h"
#include "rtos_api.h"
#include "app_message.h"
#include "debug.h"
#include "clk.h"
#include "main_task.h"
#include "timer.h"
#include "otg_detect.h"
#include "ctrlvars.h"
#include "communication.h"
#include "ff.h"
#include "mvstdio.h"
#include "blue_aec.h"
#include "otg_host_hcd.h"
#include "dma.h"
#include "rom.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "debug.h"

#ifdef CFG_DUMP_DEBUG_EN

#define ONE_BLOCK_WRITE 512

MemHandle aec_debug_fifo;
static uint8_t aec_debug_raw_buf[4096*2];
uint8_t aec_temp_buf[ONE_BLOCK_WRITE];
int16_t aec_temp_buf1[256*2];
uint8_t hfp_disconnect = 0;
uint8_t gDumpProcessStop = 0;//dump stop flag

void sbc_store_process(uint8_t *sbc, uint16_t Len)
{
	int i;
	static uint8_t cnn = 0;
	DBG(".");
	if(hfp_disconnect != 2)
		return;
	for(i=0; i<Len; i++)
	{
		aec_temp_buf1[i] = sbc[i];
	}
	if(mv_mremain(&aec_debug_fifo) >= Len)
	{
		mv_mwrite(aec_temp_buf1, 1, Len, &aec_debug_fifo);
	}
	else
	{
		if(cnn == 0)
		{
			APP_DBG("\n-F-\n");
			cnn = 100;
		}
		else
		{
			cnn--;
		}

	}
}
void sco_store_process(uint8_t *sco, uint16_t Len)
{
	int i;
	static uint8_t cnn = 0;
	DBG(".");
	if(hfp_disconnect != 2)
		return;
	for(i=0; i<Len; i++)
	{
		aec_temp_buf1[i] = sco[i];
	}
	if(mv_mremain(&aec_debug_fifo) >= Len)
	{
		mv_mwrite(aec_temp_buf1, 1, Len, &aec_debug_fifo);
	}
	else
	{
		if(cnn == 0)
		{
			APP_DBG("\n-F-\n");
			cnn = 100;
		}
		else
		{
			cnn--;
		}

	}
}
void aec_store_process(uint8_t *aec, uint16_t Len)
{
	int i;
	static uint8_t cnn = 0;
	// DBG(".");
	if(hfp_disconnect != 2)
		return;
	if(mv_mremain(&aec_debug_fifo) >= Len)
	{
		mv_mwrite(aec, 1, Len, &aec_debug_fifo);
	}
	else
	{
		if(cnn == 0)
		{
			APP_DBG("\n-F-\n");
			cnn = 100;
		}
		else
		{
			cnn--;
		}

	}
}

uint8_t DumpBuf[4096];
void DumpUartConfig(bool InitBandRate)
{
	if(GET_DEBUG_GPIO_PORT(CFG_DUMP_UART_TX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(1 << GET_DEBUG_GPIO_INDEX(CFG_DUMP_UART_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_DUMP_UART_TX_PORT));
	else
		GPIO_PortBModeSet(1 << GET_DEBUG_GPIO_INDEX(CFG_DUMP_UART_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_DUMP_UART_TX_PORT));

	if(InitBandRate)
	{
		UARTS_Init(GET_DEBUG_GPIO_UARTPORT(CFG_DUMP_UART_TX_PORT), CFG_DUMP_UART_BANDRATE, 8, 0, 1);
	}
	UARTS_DMA_TxInit(GET_DEBUG_GPIO_UARTPORT(CFG_DUMP_UART_TX_PORT),DumpBuf,4096,4096/2,NULL);

}

void dumpUartSend(uint8_t *buf,uint16_t buflen)
{
	uint8_t Hend[6];
	uint16_t Crc;
	static uint8_t frameid;
	TIMER DumpTimer;

	//dump 异常，停止发送
	if(gDumpProcessStop)
		return;
	
	TimeOutSet(&DumpTimer, 1000);
	while(DMA_CircularSpaceLenGet(CFG_DUMP_UART_TX_DMA_CHANNEL) < buflen+8){
		if(IsTimeOut(&DumpTimer))
		{
			uint32_t id;
			id = DMA_ChannelNumGet(CFG_DUMP_UART_TX_DMA_CHANNEL);
			APP_DBG("DUMP_ERR PERIPHERAL_ID_UART0_TX ID %ld\r\n",id);    //注意查看DMA通道是否在其他位置被重新设置了。DMA_ChannelAllocTableSet
			return;
		}
	}
	Hend[0] = 0xA5;
	Hend[1] = 0xA5;
	Hend[2] = 0xA5;
	Hend[3] = frameid++;
	Hend[4] = buflen>>8;
	Hend[5] = buflen;
	Crc = ROM_CRC16(buf,buflen,0);
//	printf("Crc 0x%x\r\n",Crc);
	DMA_CircularDataPut(CFG_DUMP_UART_TX_DMA_CHANNEL, Hend, 6);
	DMA_CircularDataPut(CFG_DUMP_UART_TX_DMA_CHANNEL, buf, buflen);
	DMA_CircularDataPut(CFG_DUMP_UART_TX_DMA_CHANNEL, &Crc, 2);
}

void aec_data_dump_task(void * param)
{
	mv_mopen(&aec_debug_fifo, aec_debug_raw_buf, sizeof(aec_debug_raw_buf)-4, NULL);
	hfp_disconnect = 2;
	while(1)
	{
		if(hfp_disconnect == 2)
		{
			if(mv_msize(&aec_debug_fifo) >= ONE_BLOCK_WRITE)
			{
				// DBG("*");
				mv_mread(aec_temp_buf, 1, ONE_BLOCK_WRITE, &aec_debug_fifo);
				dumpUartSend(aec_temp_buf,ONE_BLOCK_WRITE);
			}
		}
		vTaskDelay(1);
	}
}

#endif //end of CFG_DUMP_DEBUG_EN



