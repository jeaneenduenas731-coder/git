#include "type.h"
#include "reset.h"
#include "irqn.h"
#include "can_func.h"
#include "app_config.h"
#ifdef CFG_FUNC_REMIND_SOUND_EN
#include "remind_sound.h"
#endif

#ifdef CFG_FUNC_CAN_DEMO_EN

#define MAX_RX_LENGTH  32
typedef struct _BYTES_FIFO
{
 	uint32_t		Head;
	uint32_t		Count;
	CAN_DATA_MSG	Msg[MAX_RX_LENGTH];
}BYTES_RX_FIFO;

static volatile BYTES_RX_FIFO  RxMsg;

void can_msg_printf(CAN_DATA_MSG * Msg)
{
	uint8_t len,i;

	DBG("ID:%X ,%s%s\n",Msg->Id,Msg->EFF?"Extend Frame":"Standard Frame",Msg->RTR?",Remote Frame":"\0");

	len = Msg->DATALENGTH;
	if(Msg->RTR == 0)
	{
		DBG("DATA(%d): ",len);
		i = 0;
		while(len-- > 0)
		{
			DBG("%02X ",Msg->Data[i++]);
		}
		DBG("\n");
	}
}

//CAN中断和SPDIF复用一个中断号 22
void SPDIF0_Interrupt(void)
{
	uint8_t index;
	CAN_BIT_INTSTATUS int_flag = CAN_GetIntStatus();

	if(int_flag & CAN_INT_RX_FLAG)
	{
		if(RxMsg.Count < MAX_RX_LENGTH)
		{
			index = (RxMsg.Head + RxMsg.Count) % MAX_RX_LENGTH;
			CAN_RecvISR(&RxMsg.Msg[index]);
			RxMsg.Count++;
		}
		CAN_ClrIntStatus(CAN_INT_RX_FLAG);
	}

	if(int_flag & CAN_INT_DATA_OR)
	{
		uint8_t cnt;

		cnt = CAN_GetRxMsgCnt();
		DBG("Overrun Int! %d\n",cnt);

		while(cnt--)
		{
			if(RxMsg.Count < MAX_RX_LENGTH)
			{
				index = (RxMsg.Head + RxMsg.Count) % MAX_RX_LENGTH;
				CAN_RecvISR(&RxMsg.Msg[index]);
				RxMsg.Count++;
			}
		}
		CAN_SetModeCmd(CAN_MODE_RST_SELECT);
		CAN_SetModeCmd(CAN_MODE_RST_DISABLE);
	}

	if(int_flag & CAN_INT_WAKEUP)
	{
		CAN_ClrIntStatus(CAN_INT_WAKEUP);
	}

	if(int_flag & CAN_INT_TX_FLAG)
	{
		CAN_ClrIntStatus(CAN_INT_TX_FLAG);
	}

	if(int_flag & CAN_INT_BERR)
	{
		CAN_ClrIntStatus(CAN_INT_BERR);
	}

	if(int_flag & CAN_INT_ERR)
	{
		CAN_ClrIntStatus(CAN_INT_ERR);
	}

	if(int_flag & CAN_INT_ERR_PASSIVE)
	{
		CAN_ClrIntStatus(CAN_INT_ERR_PASSIVE);
	}

	if(int_flag & CAN_INT_ARB_LOST)
	{
		CAN_ClrIntStatus(CAN_INT_ARB_LOST);
	}
}

void CAN_FuncInit(void)
{
	CAN_ModuleInit(RATE_500KBPS,CAN_PORT_A3_A4);

	memset(&RxMsg,0,sizeof(RxMsg));

	CAN_IntTypeEnable(CAN_INT_OR_EN | CAN_INT_RX_EN);
	//CAN中断和SPDIF复用一个中断号 22
	NVIC_EnableIRQ(SPDIF_IRQn);//22
//	GIE_ENABLE();
}

void CAN_FuncEntrance(void)
{
	CAN_DATA_MSG	Msg;

	if(RxMsg.Count > 0)	//fifo is not empty
	{
		memcpy(&Msg,&RxMsg.Msg[RxMsg.Head],sizeof(Msg));
		RxMsg.Head = (RxMsg.Head + 1) % MAX_RX_LENGTH;
		RxMsg.Count--;

		can_msg_printf(&Msg);

		switch(Msg.Id)
		{
		case 0x555:
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(Msg.DATALENGTH > 1)
			{
				switch(Msg.Data[0])
				{
				case 0x01:
					RemindSoundServiceItemRequest(SOUND_REMIND_VOLMAX, REMIND_PRIO_ORDER);
					break;
				default:
					break;
				}
			}
#endif
			break;
		default:
			break;
		}
	}
}

#endif
