////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:debug.h
//
//		Description	:
//					Define debug ordinary print & debug routine
//
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUG_H__
#define __DEBUG_H__

/**
 * @addtogroup mv_utils
 * @{
 * @defgroup debug debug.h
 * @{
 */
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdio.h>
#include "type.h"
#include "gpio.h"

#define DEBUG_GPIO_UARTPORT_UART0										0
#define DEBUG_GPIO_UARTPORT_UART1										1

#define DEBUG_GPIO_PORT_A												0
#define DEBUG_GPIO_PORT_B												1

#define SET_DEBUG_GPIO_VAL(uart_port,gpio_mode,gpio_port,gpio_index)	((uart_port<<24)|(gpio_mode<<16)|(gpio_port<<8)|(gpio_index))
#define GET_DEBUG_GPIO_UARTPORT(val)									((val>>24)&0x01)
#define GET_DEBUG_GPIO_MODE(val)										((val>>16)&0x0f)
#define GET_DEBUG_GPIO_PORT(val)										((val>>8)&0x01)
#define GET_DEBUG_GPIO_INDEX(val)										((val)&0x1f)
#define GET_DEBUG_GPIO_PIN(val)											(1<<((val)&0x1f))

enum
{
	DEBUG_BAUDRATE_9600 	= 9600,
	DEBUG_BAUDRATE_115200 	= 115200,
	DEBUG_BAUDRATE_256000	= 256000,
	DEBUG_BAUDRATE_1000000	= 1000000,
	DEBUG_BAUDRATE_2000000	= 2000000,
	DEBUG_BAUDRATE_3000000 	= 3000000,
};

enum
{
	DEBUG_TX_A0  = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x05, DEBUG_GPIO_PORT_A, 0),
	DEBUG_TX_A1  = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x05, DEBUG_GPIO_PORT_A, 1),
	DEBUG_TX_A6  = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x07, DEBUG_GPIO_PORT_A, 6),
	DEBUG_TX_A10 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x05, DEBUG_GPIO_PORT_A, 10),
	DEBUG_TX_A18 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x04, DEBUG_GPIO_PORT_A, 18),
	DEBUG_TX_A19 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x03, DEBUG_GPIO_PORT_A, 19),
	DEBUG_TX_A20 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x0d, DEBUG_GPIO_PORT_A, 20),
	DEBUG_TX_A21 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x0c, DEBUG_GPIO_PORT_A, 21),
	DEBUG_TX_A22 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x08, DEBUG_GPIO_PORT_A, 22),
	DEBUG_TX_A23 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x07, DEBUG_GPIO_PORT_A, 23),
	DEBUG_TX_A24 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x07, DEBUG_GPIO_PORT_A, 24),
	DEBUG_TX_A28 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x09, DEBUG_GPIO_PORT_A, 28),
	DEBUG_TX_A29 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x08, DEBUG_GPIO_PORT_A, 29),
	DEBUG_TX_A30 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x08, DEBUG_GPIO_PORT_A, 30),
	DEBUG_TX_A31 = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x08, DEBUG_GPIO_PORT_A, 31),
	DEBUG_TX_B5  = SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x04, DEBUG_GPIO_PORT_B, 5),
};

//用define定义UART RX引脚，方便用宏裁剪代码
#define	DEBUG_RX_A0		SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x01, DEBUG_GPIO_PORT_A, 0)
#define	DEBUG_RX_A1  	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x02, DEBUG_GPIO_PORT_A, 1)
#define	DEBUG_RX_A5  	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART0, 0x02, DEBUG_GPIO_PORT_A, 5)
#define	DEBUG_RX_A9  	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x01, DEBUG_GPIO_PORT_A, 9)
#define	DEBUG_RX_A18 	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x01, DEBUG_GPIO_PORT_A, 18)
#define	DEBUG_RX_A19 	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x01, DEBUG_GPIO_PORT_A, 19)
#define	DEBUG_RX_B4  	SET_DEBUG_GPIO_VAL(DEBUG_GPIO_UARTPORT_UART1, 0x02, DEBUG_GPIO_PORT_B, 4)

/************************** DEBUG**********************************
*下面为打印调试信息的接口,共有两个:
*1.打印前缀接口 : 
*  1)如果需要在打印时输出模块前缀 ,请调用 APP_DBG(),
*  2)需要关闭某模块的调试信息,可以用注释屏蔽 XXX_MODULE_DEBUG的宏
*	此模块宏,是根据如下规则 来划分的 : 
*	共划分为如下9个模块,
*	其中7个模块在Apps目录:分为media play,bt,hdmi_in,Main_task,usb_audio_mode,waiting_mode模块
*	另外两个模块是:Device模块、Services模块
*	剩下统一放在：DEB模块
*
*2.不打印前缀接口 : 调用 DBG()
*************************************************************/
#define	APP_DBG(format, ...)	printf(format, ##__VA_ARGS__)
#define	DBG(format, ...)		printf(format, ##__VA_ARGS__)
#define	OTG_DBG(format, ...)	//printf(format, ##__VA_ARGS__)
#define	BT_DBG(format, ...)		printf(format, ##__VA_ARGS__)

/**指针检查，按需开success分支 ;或扩展return;**/
#define PT_CHECK(Pt)					if(Pt == NULL) 	{DBG(#Pt" NULL!!!!!\n");}
/**NULL时状态 或 **/
#define PT_CHECK_RET(Pt, StatusNullOr)		if(Pt == NULL) 	{DBG(#Pt" NULL!!!!!\n"); StatusNullOr &= TRUE;}

#define MALLOC_CHECK(Pt, Len)					if(Pt == NULL) 	{DBG(#Pt" malloc L:%d fail!!!!!\n", (int)(Len));} 							else	{DBG(#Pt" addr:0x%lx L:%d\n", (uint32_t)Pt, (int)(Len));}
/**NULL时状态 或 **/
#define MALLOC_CHECK_RET(Pt, Len, StatusNullOr)	if(Pt == NULL) 	{DBG(#Pt" malloc L:%d fail!!!!!\n", (int)(Len)); StatusNullOr &= TRUE;} 	else	{DBG(#Pt" addr:0x%lx L:%d\n", (uint32_t)Pt, (int)(Len));}

int DbgUartInit(int Which, unsigned int BaudRate, unsigned char DatumBits, unsigned char Parity, unsigned char StopBits);


/************************** TOGGLE DEBUG**********************************
 *
 * 调试时序，debug.c/.h提供IO toggle配置序列
 * 1. DBG_IO_TGL TGL_PORT_LIST为映射序列0~n，
 * 2、TGL_OFFSET OS或Int toggle序列偏移起始编号，下文编号为X
 * 3、INT_TGL 中断进出时间片, 占用TGL_INT_LIST + 1序号，led X ~ X+n
 * 4、TASK_TGL Os任务进出时间片，TGL_TASK_LIST名字表为序号，排在INT_TGL之后
 * 测试IO,常用于程序执行时序可视化，标准SDK不使用
 * 输出高低电平，上升沿，下降沿，翻转等
 * 调用DbgPortInit初始化端口配置
 *************************************************************/
//#define DBG_IO_TGL //使用IO toggle调试 逻分观测 配合 DbgPortInit()

#ifdef DBG_IO_TGL
	/**启动对应端口和序号 0~n-1，规避端口复用**/
	#define TGL_PORT_LIST	{{'A', 0}, {'A', 1}, {'A', 2}, {'A', 3}, {'A', 4}, {'A', 5}, {'A', 6}, {'A', 7}, {'A', 15}, {'A', 16}, {'A', 17}, {'A', 20}, {'A', 21}, {'A', 22}, {'A', 23}, {'A', 24}, {'A', 28},{'A', 29}}
	/** {'A', 18}, {'A', 19}**/
	#define	TGL_OFFSET			0//INT_TGL或TASK_TGL起始序列编号,即X配置，基于TGL_PORT_LIST,

	/**中断进出Toggle 表征执行时间片
	 * 	修改OS_Trap_Int_Comm @crt0.S
		!TGL_INT_COM
		jral $r1
		改为
		TGL_INT_COM
		!jral $r1
	 ********************************/
	#define INT_TGL

	#ifdef INT_TGL
		#include "irqn.h"
		/**中断号toggle序列，其他中断用后续一IO
		 * 序列不为空，示例B5,参见enum IRQn_Type @irqn.h
		 * ***************************/
		#define	TGL_INT_LIST	{BT_IRQn, Usb_IRQn}
		void dbg_int_in(uint32_t int_num);
		void dbg_int_out(uint32_t int_num);
	#endif

	/**OS任务切换IO toggle，表征执行时间片
	 * 开INT_TGL时，排在TGL_INT_LIST + 1之后
	 *******************************/
	#define TASK_TGL

	#ifdef TASK_TGL
		/**toggle的task Name头部字符串;序列不为空，占用TGL_TASK_LIST对应端口数**/
		#define TGL_TASK_LIST	{{"AudioCore"}, {"SysMode"}, {"MainApp"}, {"BtStack"}, {"IDLE"}}
		void DbgTaskTGL_in();
		void DbgTaskTGL_out();
	#endif
#endif //DBG_IO_TGL

#define PORT_IN_REG(X)			(X=='A'? GPIO_A_IN : GPIO_B_IN)
#define PORT_OUT_REG(X)			(X=='A'? GPIO_A_OUT : GPIO_B_OUT)
#define PORT_SET_REG(X)			(X=='A'? GPIO_A_SET : GPIO_B_SET)
#define PORT_CLR_REG(X)			(X=='A'? GPIO_A_CLR : GPIO_B_CLR)
#define PORT_TGL_REG(X)			(X=='A'? GPIO_A_TGL : GPIO_B_TGL)
#define PORT_IE_REG(X)			(X=='A'? GPIO_A_IE : GPIO_B_IE)
#define PORT_OE_REG(X)			(X=='A'? GPIO_A_OE : GPIO_B_OE)
#define PORT_DS_REG(X)			(X=='A'? GPIO_A_DS : GPIO_B_DS)
#define PORT_PU_REG(X)			(X=='A'? GPIO_A_PU : GPIO_B_PU)
#define PORT_PD_REG(X)			(X=='A'? GPIO_A_PD : GPIO_B_PD)
#define PORT_ANA_REG(X)			(X=='A'? GPIO_A_ANA_EN : GPIO_B_ANA_EN)
#define PORT_PULLDOWN_REG(X)	(X=='A'? GPIO_A_PULLDOWN : GPIO_B_PULLDOWN)
#define PORT_CORE_REG(X)		(X=='A'? GPIO_A_CORE_OUT_MASK : GPIO_B_CORE_OUT_MASK)
#define PORT_DMA_REG(X)			(X=='A'? GPIO_A_DMA_OUT_MASK : GPIO_B_DMA_OUT_MASK)
#define SET_MOD_REG(X, I, Mode)	((X)=='A'? (GPIO_PortAModeSet((I), (Mode))) : GPIO_PortBModeSet((I), (Mode)))

void DbgPortInit(void);
void DbgPortOn(uint8_t Index);//是否生效取决于TGL_PORT_LIST
void DbgPortOff(uint8_t Index);
void DbgPortToggle(uint8_t Index);
void DbgPortRise(uint8_t Index);
void DbgPortDown(uint8_t Index);
bool DbgPortGet(uint8_t Index);//TRUE:高电平，FALSE:低电平，index无效时 默认FALSE

#ifdef __cplusplus
}
#endif//__cplusplus

/**
 * @}
 * @}
 */
 
#endif //__DBG_H__ 

