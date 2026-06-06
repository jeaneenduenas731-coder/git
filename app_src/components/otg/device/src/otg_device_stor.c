/**
 *****************************************************************************
 * @file     device_stor.c
 * @author   Owen
 * @version  V1.0.0
 * @date     7-September-2015
 * @brief    device mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "app_config.h"
#include <string.h>
#include "type.h"
#include "timeout.h"
#include "debug.h"
#include "delay.h"
#include "otg_device_hcd.h"
#include "sd_card.h"
#include "sdio.h"
#include "dma.h"
#include "gpio.h"
#include "reset.h"
#include "mvintrinsics.h"
#ifdef FUNC_OS_EN
#include "rtos_api.h" //add for mutex declare
#endif

#ifdef CFG_OTG_MODE_READER_EN
// SCSI opcodes
#define TEST_UNIT_READY				0x00
#define REQUEST_SENSE				0x03
#define READ_6						0x08
#define WRITE_6						0x0A
#define SEEK_6						0x0B
#define INQUIRY						0x12
#define MODE_SELECT					0x15
#define MODE_SENSE					0x1A
#define START_STOP_UNIT				0x1B
#define ALLOW_MEDIUM_REMOVAL		0x1E
#define	READ_FORMAT_CAPACITY		0x23
#define READ_CAPACITY				0x25
#define READ_10						0x28
#define WRITE_10					0x2A
#define SEEK_10						0x2B
#define VERYFY						0x2F
#define MODE_SELECT_10				0x55
#define MODE_SENSE_10				0x5A
#define READ_12						0xA8
#define WRITE_12					0xAA
#define READ_DISC_STRUCTURE			0xAD
#define DVD_MACHANISM_STATUS  		0xBD
#define REPORT_KEY			  		0xA4
#define SEND_KEY			  		0xA3
#define READ_TOC			  		0x43
#define READ_MSF			  		0xB9



#define READER_UNREADY				0 // 未就绪
#define READER_INIT					1 // 初始化
#define READER_READY				2 // 就绪
#define READER_READ					3 // 读数据
#define READER_WIRTE				4 // 写数据
#define READER_INQUIRY				5 // 查询读卡器信息
#define READER_READ_FORMAT_CAPACITY 6 // 查询格式化容量
#define READER_READ_CAPACITY		7 // 查询磁盘容量
#define READER_NOT_ALLOW_REMOVAL	8 // 不允许删除设备
#define READER_ALLOW_REMOVAL		9 // 允许删除设备


#define	DEVICE_STOR_BLOCK_SIZE	512

extern void OTG_DeviceRequestProcess(void);
extern SD_CARD		    SDCard;

uint32_t CARD_BUF_A[512/4];
uint32_t CARD_BUF_B[512/4];


uint8_t    gInquiryData[] =
{
	0x00, 		//Direct Access Device
	0x80, 		//RMB
	0x02, 		//ISO/ECMA/ANSI
	0x02, 		//Response Data Format
	0x1F, 		//Additional Length
	0x00, 		//Reserved
	0x00, 		//Reserved
	0x00, 		//Reserved
	'M', 'V', 'S', 'I', 0, 0, 0, 0,				//Vendor Information
	'C', 'a', 'r', 'd', ' ', 'R', 'e', 'a', 'd', 'e', 'r', 0, 0, 0, 0, 0,		//Product Identification
	'V', '1', '.', '0'		//Product Revision Level
};

uint8_t gFmtCapacityData[] =
{
	0x00, 0x00, 0x00, 0x08,
	0x00, 0x1D, 0xC3, 0xFF,
	0x03,
	0x00,
	(uint8_t)(DEVICE_STOR_BLOCK_SIZE >> 8),
	(uint8_t)DEVICE_STOR_BLOCK_SIZE
};

uint8_t gCapacityData[] =
{
	0x00, 0x1D, 0xC3, 0xFF,
	0x00,
	0x00,
	(uint8_t)(DEVICE_STOR_BLOCK_SIZE >> 8),
	(uint8_t)DEVICE_STOR_BLOCK_SIZE
};


const uint8_t gModeSenseProtectPage[] = {0x03, 0x00, 0x00, 0x00};

uint8_t	gModeSenseAllPage[] =
{
	0x0B,
	0x00,
	0x00,
	0x08,
	0x00,
	0x02, 0x00, 0x00, 		// block size 131072
	0x00,
	0x00, 0x02, 0x00,
};

const uint8_t gRequestSenseNotReady[] =
{
	0x70, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x00, 0x00, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x00,
	0x00, 0x00
};

const uint8_t gRequestSenseReady[] =
{
	0x70, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
	0x00, 0x00
};

uint8_t gCBW[64];

uint8_t gCSW[] = {'U', 'S', 'B', 'S', 0x08, 0x70, 0xBA, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00};

bool DeviceStorPreventFlag;
TIMER DeviceStorPreventTimer;

bool DeviceStorStoppedFlag;
bool DeviceStorCmdFlag;
TIMER DeviceStorCmdTimer;

bool DeviceStorIsCardInitOK;

static uint8_t sReaderState = READER_UNREADY;

uint8_t GetSdReaderState(void)
{
	return sReaderState;
}

void SetSdReaderState(uint8_t State)
{
	sReaderState = State;
}

//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  获取SD卡连接状态
 * @param  NONE
 * @return 1-成功，0-失败
 */
bool DeviceStorIsCardLink(void)
{
	if(SDCard_Detect() != NONE_ERR)
	{
		DeviceStorIsCardInitOK = FALSE;
	}
	else if(!DeviceStorIsCardInitOK)	//当前有卡连接，并且尚未初始化过，则初始化卡
	{
		if(SDCard.CardInit == SD_INITED)
		{
			DeviceStorIsCardInitOK = TRUE;
		}
		else
		{
			DeviceStorIsCardInitOK = FALSE;
		}
	}

	return DeviceStorIsCardInitOK;
}

////////////////////////////Mass Storage Request//////////////////////////////////////
/**
 * @brief  发送CSW
 * @param  Status CSW状态
 * @return NONE
 */
void DeviceStorSendCSW(uint8_t Status)
{
	gCSW[12] = Status;
	OTG_DeviceBulkSend(DEVICE_BULK_IN_EP,gCSW, sizeof(gCSW), 100000);
}

/**
 * @brief  处理ReadFmtCapacity命令
 * @param  NONE
 * @return NONE
 */
void DeviceStorReadFmtCapacity(void)
{
	uint32_t Temp = SDCard_CapacityGet();
	if(!DeviceStorIsCardLink())
	{
		OTG_DeviceStallSend(DEVICE_BULK_IN_EP);
#ifdef FUNC_OS_EN
		osTaskDelay(10);
#else
		WaitMs(10);
#endif
		OTG_DeviceRequestProcess();
		DeviceStorSendCSW(1);
		return;
	}

	((uint32_t*)&gFmtCapacityData[4])[0] = CpuToBe32(Temp);
	OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, gFmtCapacityData, sizeof(gFmtCapacityData), 10000);
	DeviceStorSendCSW(0);
}

/**
 * @brief  处理ReadCapacity命令
 * @param  NONE
 * @return NONE
 */
extern uint8_t Setup[];

void DeviceStorReadCapacity(void)
{
	uint32_t Temp = SDCard_CapacityGet() - 1;
	if(!DeviceStorIsCardLink())
	{
		OTG_DeviceStallSend(DEVICE_BULK_IN_EP);
#ifdef FUNC_OS_EN
		osTaskDelay(10);
#else
		WaitMs(10);
#endif
		OTG_DeviceRequestProcess();
		memset(Setup, 0, 8);
		DeviceStorSendCSW(1);
		return;
	}

	((uint32_t*)&gCapacityData[0])[0] = CpuToBe32(Temp);
	OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, gCapacityData, sizeof(gCapacityData), 1000);
	DeviceStorSendCSW(0);
}

/**
 * @brief  处理Read10命令
 * @param  NONE
 * @return NONE
 */
void DeviceStorRead10(void)// bkd change sdio1 to 0
{
	TIMER Timer;
	uint32_t ReqLBA = Be32ToCpu(*(uint32_t*)(&gCBW[17]));
	uint16_t ReqCnt = Be16ToCpu(*(uint16_t*)(&gCBW[22]));
	if(!SDCard.IsSDHC)
	{
		ReqLBA *= SD_BLOCK_SIZE;
	}
#ifdef FUNC_OS_EN
	osMutexLock(SDIOMutex);
#endif
	DMA_ChannelChange(PERIPHERAL_ID_SDIO_TX,PERIPHERAL_ID_SDIO_RX);
	DMA_ChannelDisable(PERIPHERAL_ID_SDIO_RX);
	DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT);
	DMA_BlockConfig(PERIPHERAL_ID_SDIO_RX);
	DMA_BlockBufSet(PERIPHERAL_ID_SDIO_RX, (ReqCnt % 2) ? CARD_BUF_A : CARD_BUF_B, SD_BLOCK_SIZE);
#ifdef CFG_SDIO_BYTE_MODE_ENABLE
	DMA_ConfigDwidth(PERIPHERAL_ID_SDIO_RX, DMA_DWIDTH_BYTE);
#endif
	DMA_ChannelEnable(PERIPHERAL_ID_SDIO_RX);
	SDIO_MultiBlockDisable();
	SDIO_AutoKillRXClkEnable();
	SDIO_SingleBlockConfig(SDIO_DIR_RX,SD_BLOCK_SIZE);
	SDIO_DataTransfer(1);
	SDIO_CmdSend(CMD18_READ_MULTIPLE_BLOCK, ReqLBA, 20);
	TimeOutSet(&Timer, 250);
	while(!DMA_InterruptFlagGet(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT) && !IsTimeOut(&Timer));
	if(!IsTimeOut(&Timer))//for SD Plus out
	{
		DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT);

		while(ReqCnt-- > 1)
		{
			SDIO_DataTransfer(0);
			DMA_ChannelDisable(PERIPHERAL_ID_SDIO_RX);
			DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT);
			DMA_BlockConfig(PERIPHERAL_ID_SDIO_RX);
			DMA_BlockBufSet(PERIPHERAL_ID_SDIO_RX, (ReqCnt % 2) ? CARD_BUF_A : CARD_BUF_B, SD_BLOCK_SIZE);//0,2,4,6 A 1357B
#ifdef CFG_SDIO_BYTE_MODE_ENABLE
			DMA_ConfigDwidth(PERIPHERAL_ID_SDIO_RX, DMA_DWIDTH_BYTE);
#endif
			DMA_ChannelEnable(PERIPHERAL_ID_SDIO_RX);
			SDIO_DataTransfer(1);
			SDIO_ClkEnable();
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, (ReqCnt % 2) ? (uint8_t*)CARD_BUF_B : (uint8_t*)CARD_BUF_A, 512, 10000);//0,2,4,6 B 1357A
			TimeOutSet(&Timer, 250);
			while(!DMA_InterruptFlagGet(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT) && !IsTimeOut(&Timer))
			{
				;
			}
			if(IsTimeOut(&Timer))//for SD Plus out
			{
				break;
			}
			DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_RX, DMA_DONE_INT);
		}
		OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, (ReqCnt % 2) ? (uint8_t*)CARD_BUF_B : (uint8_t*)CARD_BUF_A, 512, 10000);
		SDIO_DataTransfer(0);
		SDIO_ClkEnable();
		SDIO_CmdDoneCheckBusy(1);
		SDIO_CmdSend(CMD12_STOP_TRANSMISSION, 0, 300);
	}
	SDIO_CmdDoneCheckBusy(0);
	SDIO_DataTransfer(0);
	SDIO_ClkDisable();

	DeviceStorSendCSW(0);
	Reset_FunctionReset(SDIO_FUNC_SEPA);
#ifdef FUNC_OS_EN
	osMutexUnlock(SDIOMutex);
#endif
}

/**
 * @brief  处理Write10命令
 * @param  NONE
 * @return NONE
 */
void DeviceStorWrite10(void)
{
	TIMER Timer;
	uint32_t ReqLBA = Be32ToCpu(*(uint32_t*)(&gCBW[17]));
	uint16_t ReqCnt = Be16ToCpu(*(uint16_t*)(&gCBW[22]));
	uint32_t DataLength = 0;
	if(!SDCard.IsSDHC)
	{
		ReqLBA *= SD_BLOCK_SIZE;
	}
#ifdef FUNC_OS_EN
	osMutexLock(SDIOMutex);
#endif
	Reset_FunctionReset(SDIO_FUNC_SEPA);
	while(SDIO_IsDataLineBusy());
	SDIO_ClearClkHalt();
	SDIO_MultiBlockDisable();
	SDIO_AutoKillTXClkEnable();
	SDIO_DataTransfer(0);
	OTG_DeviceBulkReceive(DEVICE_BULK_OUT_EP, (ReqCnt%2) ? (uint8_t*)CARD_BUF_A : (uint8_t*)CARD_BUF_B, 512, &DataLength, 10000);
	SDIO_CmdSend(CMD25_WRITE_MULTIPLE_BLOCK, ReqLBA, 50);
	while(ReqCnt--)
	{
		while(SDIO_IsDataLineBusy())
		{
			;
		}
		SDIO_DataTransfer(0);
		DMA_ChannelChange(PERIPHERAL_ID_SDIO_RX,PERIPHERAL_ID_SDIO_TX);
		DMA_ChannelDisable(PERIPHERAL_ID_SDIO_TX);
		DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_TX, DMA_DONE_INT);
		DMA_BlockConfig(PERIPHERAL_ID_SDIO_TX);
		DMA_BlockBufSet(PERIPHERAL_ID_SDIO_TX, (uint8_t*)((ReqCnt%2) ? (uint8_t*)CARD_BUF_B : (uint8_t*)CARD_BUF_A), SD_BLOCK_SIZE);
#ifdef CFG_SDIO_BYTE_MODE_ENABLE
		DMA_ConfigDwidth(PERIPHERAL_ID_SDIO_TX, DMA_DWIDTH_BYTE);
#endif
		DMA_ChannelEnable(PERIPHERAL_ID_SDIO_TX);
		SDIO_SingleBlockConfig(SDIO_DIR_TX,SD_BLOCK_SIZE);
		SDIO_DataTransfer(1);
		SDIO_ClkEnable();

		if(ReqCnt != 0)
		{
			OTG_DeviceBulkReceive(DEVICE_BULK_OUT_EP, (ReqCnt % 2) ? (uint8_t*)CARD_BUF_A : (uint8_t*)CARD_BUF_B, 512, &DataLength, 10000);
		}
		TimeOutSet(&Timer, 500);
		while(!IsTimeOut(&Timer) && (!DMA_InterruptFlagGet(PERIPHERAL_ID_SDIO_TX, DMA_DONE_INT) || !SDIO_DataIsDone()))
		{
				;
		}
		if(IsTimeOut(&Timer))//for SD Plus out
		{
			DBG("sdio Write10 time out!!!\n");
			break;
		}
		DMA_InterruptFlagClear(PERIPHERAL_ID_SDIO_TX, DMA_DONE_INT);
	}
	SDIO_DataTransfer(0);
	SDIO_ClkDisable();

	SDIO_ClearClkHalt();
	SDIO_CmdDoneCheckBusy(1);
	SDIO_CmdSend(CMD12_STOP_TRANSMISSION, 0, 300);
	SDIO_CmdDoneCheckBusy(0);
#ifdef FUNC_OS_EN
	osMutexUnlock(SDIOMutex);
#endif
	DeviceStorSendCSW(0);
	Reset_FunctionReset(SDIO_FUNC_SEPA);
	while(SDIO_IsDataLineBusy())
	{
		DBG("SDIO_IsDataLineBusy 2\n");
	}
	//DBG("6");
}

/**
 * @brief  处理ModeSense命令
 * @param  NONE
 * @return NONE
 */
extern uint8_t Setup[];
void DeviceStorModeSense(void)
{
	if(!DeviceStorIsCardLink())
	{
		OTG_DeviceStallSend(DEVICE_BULK_IN_EP);
#ifdef FUNC_OS_EN
		osTaskDelay(10);
#else
		WaitMs(10);
#endif
		OTG_DeviceRequestProcess();
		memset(Setup,0,8);
		DeviceStorSendCSW(1);
		return;
	}

	((uint32_t*)&gModeSenseAllPage[4])[0] = CpuToBe32(SDCard_CapacityGet());

	switch(gCBW[17])
	{
		case 0x1C:		//mode sense, Timer and Protect Page
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, (uint8_t*)gModeSenseProtectPage, sizeof(gModeSenseProtectPage), 10000);
			DeviceStorSendCSW(0);
			break;

		case 0x3F:		//mode sense, Return all pages
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, gModeSenseAllPage, MIN(sizeof(gModeSenseAllPage),gCBW[19]), 10000);
			DeviceStorSendCSW(0);
			break;

		default:
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, 0, 0, 10000);
			DeviceStorSendCSW(0);
			break;
	}
}

/**
 * @brief  判断当前是否处于保护状态
 * @param  NONE
 * @return 1-保护，0-非保护
 */
bool DeviceStorIsPrevent(void)
{
	if((!DeviceStorPreventFlag) || IsTimeOut(&DeviceStorPreventTimer))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/**
 * @brief  判断PC端是否执行了“弹出”操作
 * @param  NONE
 * @return 1-已弹出，0-未弹出
 */
bool DeviceStorIsStopped(void)
{
	if(DeviceStorStoppedFlag)
	{
		return TRUE;
	}
	else if(DeviceStorCmdFlag && IsTimeOut(&DeviceStorCmdTimer))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * @brief  读卡器功能初始化
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceStorInit(void)
{
	sReaderState = READER_INIT;
//	OTG_DeviceInit();
	DeviceStorStoppedFlag = FALSE;
	DeviceStorCmdFlag = FALSE;
	sReaderState = READER_READY;
}

/**
 * @brief  读卡器模式命令处理总入口
 * @param  NONE
 * @return NONE
 */
extern uint8_t Setup[];
void OTG_DeviceStorProcess(void)
{
	uint32_t DataLen = 0;
	
	if(OTG_DeviceBulkReceive(DEVICE_BULK_OUT_EP, (uint8_t*)&gCBW, 31, &DataLen, 1) != DEVICE_NONE_ERR)
	{
		return;
	}
	if(memcmp(gCBW, "USBC", 4))
	{
		DBG("(memcmp(gCBW, USBC, 4) != 0\n");
		return;
	}

	// set CSW tag
	gCSW[4] = gCBW[4];
	gCSW[5] = gCBW[5];
	gCSW[6] = gCBW[6];
	gCSW[7] = gCBW[7];

	switch(gCBW[15])
	{
		case INQUIRY:
			//DBG("INQUIRY\n");
			sReaderState = READER_INQUIRY;
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, gInquiryData, sizeof(gInquiryData), 10000);
			OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, gCSW, sizeof(gCSW), 10000);
			break;

		case READ_FORMAT_CAPACITY:
			//DBG("READ_FORMAT_CAPACITY\n");
			sReaderState = READER_READ_FORMAT_CAPACITY;
			DeviceStorReadFmtCapacity();
			break;

		case READ_CAPACITY:
			//DBG("READ_CAPACITY\n");
			sReaderState = READER_READ_CAPACITY;
			DeviceStorReadCapacity();
			break;

		case READ_10:
			//DBG("R\n");//DBG("R10\n");
			sReaderState = READER_READ;
			DeviceStorRead10();
			break;

		case WRITE_10:
			//DBG("W\n");//DBG("W10\n");
			sReaderState = READER_WIRTE;
			DeviceStorWrite10();
			TimeOutSet(&DeviceStorPreventTimer, 2000);
			break;

		case TEST_UNIT_READY:
			if(!DeviceStorIsCardLink())
			{
				sReaderState = READER_UNREADY;
				DeviceStorStoppedFlag = FALSE;	//卡被拔除后，清楚“弹出标志”，以便下次插上卡时，能够被电脑识别到
				DeviceStorSendCSW(1);
			}
			else
			{
				if(DeviceStorStoppedFlag)		//卡未拔除，但在电脑上执行了“弹出”命令
				{
					sReaderState = READER_UNREADY;
					DeviceStorSendCSW(1);
				}
				else
				{
					sReaderState = READER_READY;
					DeviceStorSendCSW(0);
				}
			}
			break;

		case MODE_SENSE:
			//DBG("MODE_SENSE\n");
			DeviceStorModeSense();
			break;

		case ALLOW_MEDIUM_REMOVAL:
			//DBG("ALLOW_MEDIUM_REMOVAL\n");
			if((gCBW[19] == 0) && DeviceStorIsCardLink())
			{
				//DBG("allow\n");
				sReaderState = READER_NOT_ALLOW_REMOVAL;
				DeviceStorPreventFlag = FALSE;
				DeviceStorSendCSW(0);
			}
			else
			{
				//DBG("prevent\n");
				sReaderState = READER_ALLOW_REMOVAL;
				DeviceStorPreventFlag = TRUE;
				DeviceStorSendCSW(0);
			}
			break;

		case REQUEST_SENSE:
			//DBG("REQUEST_SENSE\n");
			if((!DeviceStorIsCardLink()) || DeviceStorStoppedFlag)
			{
				OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, (uint8_t*)gRequestSenseNotReady, sizeof(gRequestSenseNotReady), 10000);
			}
			else
			{
				OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, (uint8_t*)gRequestSenseReady, sizeof(gRequestSenseReady), 10000);
			}
			DeviceStorSendCSW(0);
			break;

		case VERYFY:
			//DBG("VERYFY\n");
			DeviceStorSendCSW(0);
			break;

		case START_STOP_UNIT:
			//DBG("START_STOP_UNIT\n");
			DeviceStorSendCSW(0);
			DeviceStorStoppedFlag = TRUE;
			sReaderState = READER_UNREADY;
			break;

		default:
			DBG("default\n");
			uint32_t temp = 0;
			temp = ((gCBW[8]) | (gCBW[9]<<8) | (gCBW[10]<<16) | (gCBW[11]<<24));
			if(temp > 0)
			{
				DBG("CMD %x send DATA %ld\n",gCBW[15],temp);
				OTG_DeviceBulkSend(DEVICE_BULK_IN_EP, NULL, 0, 10000);
			}
			DeviceStorSendCSW(1);
			break;
	}

	//如果1.2秒没有接收到命令，则认为PC端已安全删除设备
	DeviceStorCmdFlag = TRUE;
	TimeOutSet(&DeviceStorCmdTimer, 1500);
}
#endif
