#include "type.h"
#include "can_interface.h"
#include "reset.h"
#include "irqn.h"

static const uint8_t Can_BaudRate[RATE_MAX_TABLE][5] =
{
// PHSEG1	PHSEG2	SAM   TQ_BRP	SJW
	{12,	5,		1,		11,		0 },	//50K
	{12,	5,		1,		5,		0 },	//100K
	{10,	3,		1,		5,		0 },	//125K
	{10,	3,		1,		2,		1 },	//250K
	{15,	6,		1,		0,		1 },	//500K
	{7,		2,		1,		0,		1 },	//1M
};

void CAN_ModuleInit(CAN_BAUDRATE baudrate,CAN_PORT_MODE port)
{
	CAN_INIT_STRUCT can_init;

	CAN_PortSelect(port);
	CAN_ClkSelect(CAN_CLK_OSC_24M);

	Reset_FunctionReset(CAN_FUNC_SEPA);

	CAN_SetModeCmd(CAN_MODE_RST_SELECT);

	can_init.PHSEG1  = Can_BaudRate[baudrate][0];
	can_init.PHSEG2  = Can_BaudRate[baudrate][1];
	can_init.SAM     = Can_BaudRate[baudrate][2];
	can_init.TQ_BRP	 = Can_BaudRate[baudrate][3];
	can_init.SJW	 = Can_BaudRate[baudrate][4];
    //Acceptance code and mask
	can_init.CAN_ACPC= 0x00;
	can_init.CAN_ACPM= 0xffffffff;
	CAN_Init(&can_init);

	//CAN_IntTypeEnable(CAN_INT_RX_EN | CAN_INT_WAKEUP_EN);
	//CAN中断和SPDIF复用一个中断号 22
	//NVIC_EnableIRQ(SPDIF_IRQn);//22
	//GIE_ENABLE();

	CAN_RXTX_ERR_CNT cnt;
	cnt.ERR_WRN_LMT = 0x80;
	cnt.RX_ERR_CNT  = 0;
	cnt.TX_ERR_CNT  = 0;
	CAN_SetRxTxErrCnt(&cnt);

	//CAN_SetModeCmd(CAN_MODE_ABOM_DISABLE);
	//CAN_SetModeCmd(CAN_MODE_AUWK_MODE_DISABLE);
	//CAN_SetModeCmd(CAN_MODE_AUWK_MODE);
	//CAN_SetModeCmd(CAN_MODE_SLEEP_SEL);
	CAN_SetModeCmd(CAN_MODE_RST_DISABLE);
}

void CanDataSendTest(void)
{
	CAN_DATA_MSG Msg;

	Msg.Id		= 0x55;
	Msg.EFF 	= 0;  	// 1 - Extend frame 0 - Standard frame
	Msg.RTR 	= 0;	// 1 - remote frame 0 - normal frame
	Msg.Data[0]	= 0x10;
	Msg.Data[1]	= 0x20;
	Msg.Data[2]	= 0x30;
	Msg.Data[3]	= 0x40;
	Msg.Data[4]	= 0x50;
	Msg.Data[5]	= 0x60;
	Msg.Data[6]	= 0x70;
	Msg.Data[7]	= 0x80;
	Msg.DATALENGTH = 8;

	CAN_Send(&Msg,500);
}

#if 0
//CAN中断和SPDIF复用一个中断号 22
void SPDIF0_Interrupt(void)
{
	CAN_DATA_MSG msg;
	uint8_t index;
	CAN_BIT_INTSTATUS int_flag = CAN_GetIntStatus();

	if(int_flag & CAN_INT_RX_FLAG)
	{
		//接收数据
		CAN_RecvISR(&msg);
		CAN_ClrIntStatus(CAN_INT_RX_FLAG);
	}

	if(int_flag & CAN_INT_DATA_OR)
	{
		//数据溢出，接收fifo中数据然后RST
		uint8_t cnt;
		cnt = CAN_GetRxMsgCnt();

		while(cnt--)
		{
			CAN_RecvISR(&msg);
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
#endif



