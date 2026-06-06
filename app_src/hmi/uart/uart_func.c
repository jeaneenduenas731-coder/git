#include "type.h"
#include "reset.h"
#include "irqn.h"
#include "uarts_interface.h"
#include "app_config.h"
#include "main_task.h"
#include "mode_task.h"
#include "debug.h"
#include "remind_sound.h"
#include "uart_cmd.h"

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN

#define MAX_LENGTH				200
#define MAX_CMD_DATA_LEN		64

typedef struct _BYTES_FIFO
{
 	uint8_t		Head;
	uint8_t		Count;
	uint8_t		Buf[MAX_LENGTH];
}BYTES_FIFO;

//UART接收BUF
static volatile BYTES_FIFO  UartBuf;
static uint8_t SlaveRxIndex = 0xff;
static uint8_t SlaveHead = 0;
static uint8_t gSlaveCmd[MAX_LENGTH];
static uint8_t gSlaveSend[MAX_LENGTH];

#define CFG_UART_FUNC_TX_PORT 			DEBUG_TX_A6
#define CFG_UART_FUNC_RX_PORT 			DEBUG_RX_A5

static uint8_t BisRole = LE_BIS_NONE;
static uint8_t  GpioIntEnable = 0;

static struct
{
	uint16_t 		cnt;
	uint16_t 		delay;
}CurModeSendAgain = {0};

void SendCurModeToSlave(void)
{
	CurModeSendAgain.cnt 		= 2;  	//重新发送次数
	CurModeSendAgain.delay 		= 200; 	// 发送的间隔

	SendCmdToSlave(CMD_SYS_CUR_MODE,1,&mainAppCt.SysCurrentMode);
}

void CurModeAgainProcess(void)
{
	if(CurModeSendAgain.cnt)
	{
		if(CurModeSendAgain.delay > 0)
		{
			CurModeSendAgain.delay--;
			if(CurModeSendAgain.delay == 0)
			{
				CurModeSendAgain.cnt--;
				SendCmdToSlave(CMD_SYS_CUR_MODE,1,&mainAppCt.SysCurrentMode);
				CurModeSendAgain.delay 		= 200;
			}
		}
	}
}

uint8_t GetBisRole(void)
{
	return BisRole;
}

void UartSbufClr(void)
{
	UartBuf.Head = 0;
	UartBuf.Count = 0;
	SlaveRxIndex = 0xff;
}

void UartDataReceived(uint8_t data)
{
	if(UartBuf.Count < MAX_LENGTH)
		UartBuf.Buf[(UartBuf.Head + (UartBuf.Count++)) % MAX_LENGTH] = data;
}

uint8_t UartDataGet(void)
{
	uint8_t Temp = 0;

	if(UartBuf.Count)			//fifo is not empty
	{
		Temp = UartBuf.Buf[UartBuf.Head];
		UartBuf.Head = (UartBuf.Head + 1) % MAX_LENGTH;
		UartBuf.Count--;
	}
	return Temp;
}

uint8_t CheckUartDataLen(void)
{
	return UartBuf.Count;
}

void UartFuncInit(void)
{
	if(GET_DEBUG_GPIO_PORT(CFG_UART_FUNC_TX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_FUNC_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_FUNC_TX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_FUNC_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_FUNC_TX_PORT));

	if(GET_DEBUG_GPIO_PORT(CFG_UART_FUNC_RX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_FUNC_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_FUNC_RX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_FUNC_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_FUNC_RX_PORT));

	UARTS_Init(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_TX_PORT),2000000, 8, 0, 1);

	UartSbufClr();

	if(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT))
		NVIC_EnableIRQ(UART1_IRQn);
	else
		NVIC_EnableIRQ(UART0_IRQn);
	UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXINT_SET, 1);
	UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXINT_CLR, 1);
	UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXFIFO_CLR, 1);
}

#if GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT)
void UART1_Interrupt(void)
#else
void UART0_Interrupt(void)
#endif
{
	uint8_t rxdata,i;

    if(UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXSTAT_GET, 1) & 0x01)
    {
    	for(i=0;i<4;i++)
    	{
    		if(!UARTS_RecvByte(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),&rxdata))
    			break;
    	 	 UartDataReceived(rxdata);
    	}
        UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXINT_CLR, 1);
	}

	if( UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RX_ERR_INT_GET, 0)
		&& (UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXSTAT_GET, 0) & 0x1C))//ERROR INT
	{
		UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RX_ERR_CLR, 1);
//		UARTS_IOCTL(0,UART_IOCTL_TXFIFO_CLR, 1);
		UARTS_IOCTL(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_RX_PORT),UART_IOCTL_RXFIFO_CLR, 1);
//		DBG("\nUART_IOCTL_RX_ERR_INT_GET\n");
    }
}

static uint8_t UartSlaveRcvCmd(void)
{
	uint8_t Temp;
	uint8_t CheckSum;
	uint8_t i;
	uint16_t len;

	len = CheckUartDataLen();
	while(len > 0)
//	while(CheckUartDataLen() > 0)
	{
		Temp = UartDataGet();
		len--;
		if(SlaveRxIndex == 0xff)
		{
			if(Temp==0x55)
			{
				SlaveHead = Temp;
			}
			else if( (SlaveHead==0x55)&&(Temp==0xaa) )
			{
				SlaveRxIndex =0;
			}
		}
		else
		{
			SlaveHead=0;
			gSlaveCmd[SlaveRxIndex++] = Temp;		//Cmd + Data[] + CS
			//如果长度域超过最大长度，则重新接收
			if((gSlaveCmd[0] > MAX_CMD_DATA_LEN))
			{
				SlaveRxIndex = 0xff;
				continue;
			}
			if(SlaveRxIndex >= gSlaveCmd[0] + 3)	//CS接收完
			{
				CheckSum = 0;
				for(i = 0; i < SlaveRxIndex; i++)
				{
					CheckSum += gSlaveCmd[i];
				}
				SlaveRxIndex = 0xff;
				if(CheckSum == 0)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	return 0;
}

void UartCmdTask(void)
{
	MessageContext		msgSend;

	if(UartSlaveRcvCmd())
	{
//		DBG("UartCmdTask: %x\n",gSlaveCmd[1]);
		switch(gSlaveCmd[1])
		{
			default:
				break;
			case CMD_LE_BIS_SRC_START:
				BisRole = LE_BIS_SRC;
				Audio_BisDACSwtichChannal(1);
				break;
			case CMD_LE_BIS_SRC_STOP:
				BisRole = LE_BIS_NONE;
				if(mainAppCt.SysCurrentMode != ModeIdle &&
				   mainAppCt.SysCurrentMode != ModeLeAudioSinkPlay &&
				   mainAppCt.SysCurrentMode != ModeLeAudioLinein &&
				   mainAppCt.SysCurrentMode != ModeLeAudioBT)
					Audio_BisDACSwtichChannal(0);
				break;
			case CMD_LE_BIS_SNK_START:
				LeAudioSinkI2sInModeEnter();
				BisRole = LE_BIS_SINK;
				break;
			case CMD_LE_BIS_SNK_STOP:
				LeAudioSinkI2sInModeExit();
				BisRole = LE_BIS_NONE;
				break;
			case CMD_SYS_SWITCH_MODE:
				msgSend.msgId		= MSG_MODE;
				MessageSend(GetMainMessageHandle(), &msgSend);
				break;
			case CMD_SYS_GET_CUR_MODE:
				SendCurModeToSlave();
				break;
			case CMD_SYS_START_OK:
				LeAudioIntEnable();
				GpioIntEnable = 1;
				break;
			case CMD_SYS_CUR_MODE:
				break;
			case CMD_SYS_I2S_OUT_DATA_LEN:
				{
					uint16_t  temp;
					extern void LeAduio_BisDacPcmDropLen(uint16_t  len);
					temp = gSlaveCmd[2];
					temp <<= 8;
					temp += gSlaveCmd[3];
					LeAduio_BisDacPcmDropLen(temp);
				}
				break;
			case CMD_SYS_VOL_SET:
				{
					extern int16_t MusicVolume;
					int16_t Volume = gSlaveCmd[3];
					Volume <<= 8;
					Volume |= gSlaveCmd[2];
					if(Volume != MusicVolume)
					{
						AudioMusicVolSet(Volume);
					}
				}
				break;
			case CMD_REMIND_SOUND_BT_CONNECT:
				RemindSoundServiceItemRequest(SOUND_REMIND_CONNECT, REMIND_PRIO_SYS|REMIND_ATTR_NEED_HOLD_PLAY);
				break;
			case CMD_REMIND_SOUND_BT_DISCONNECT:
				RemindSoundServiceItemRequest(SOUND_REMIND_DISCONNE, REMIND_PRIO_SYS|REMIND_ATTR_NEED_HOLD_PLAY);
				break;
		}
		if(!GpioIntEnable)
		{
			GpioIntEnable = 1;
			LeAudioIntEnable();
			DBG("------LeAudioIntEnable!!\n");
		}
	}
}

void SendCmdToSlave(uint8_t cmd,uint8_t len,uint8_t *buf)
{
	uint8_t CheckSum = 0 ;
	uint8_t i ;

	gSlaveSend[0] = 0x55 ;
	gSlaveSend[1] = 0xAA ;
	gSlaveSend[2] = len ;
	gSlaveSend[3] = cmd ;

	if(len > 0)
	{
		for(i=1;i<=len;i++)
			gSlaveSend[3+i] = buf[i-1];
	}

	for(i = 2 ; i < len+4; i++)
	{
		CheckSum += gSlaveSend[i] ;
	}
	gSlaveSend[i] = (0 - CheckSum);

	for(i=0;i<(gSlaveSend[2]+5);i++)
		UARTS_SendByte_In_Interrupt(GET_DEBUG_GPIO_UARTPORT(CFG_UART_FUNC_TX_PORT),gSlaveSend[i]);
}

void UART_FuncEntrance(uint16_t Msg)
{
	UartCmdTask();
	CurModeAgainProcess();
	switch(Msg)
	{
	default:
		break;
	case MSG_LE_BIS_SRC_START:
		SendCmdToSlave(CMD_LE_BIS_SRC_START,0,NULL);
		break;
	case MSG_LE_BIS_SRC_STOP:
		SendCmdToSlave(CMD_LE_BIS_SRC_STOP,0,NULL);
		break;
	case MSG_LE_BIS_SNK_START:
		SendCmdToSlave(CMD_LE_BIS_SNK_START,0,NULL);
		break;
	case MSG_LE_BIS_SNK_STOP:
		SendCmdToSlave(CMD_LE_BIS_SNK_STOP,0,NULL);
		break;
	}
}

void LeAudioSinkI2sInModeEnter(void)
{
	MessageContext		msgSend;
	extern volatile uint32_t gInsertEventDelayActTimer;

	APP_DBG("Enter LeAudioSink ModeDualChipIdle\n");

	if(GetSysModeState(ModeLeAudioSinkPlay) == ModeStateSusend)
	{
		SetSysModeState(ModeLeAudioSinkPlay,ModeStateReady);
	}
	msgSend.msgId = MSG_DEVICE_SERVICE_BIS_SLAVE_CONNECTED;
	gInsertEventDelayActTimer = 0;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

void LeAudioSinkI2sInModeExit(void)
{
	MessageContext		msgSend;

	APP_DBG("Exit LeAudioSink ModeDualChipIdle\n");
	msgSend.msgId = MSG_DEVICE_SERVICE_BIS_SLAVE_DISCONNECT;
	MessageSend(GetMainMessageHandle(), &msgSend);

}

#endif


