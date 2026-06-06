/**
 *******************************************************************************
 * @file    sw_uart.c
 * @brief	software uart driver. When hardware uart pins is occupied as other
 *          functions, this software uart can output debug info. This software
 *          uart only has TX function.

 * @author  Sam
 * @version V1.0.0

 * $Created: 2018-03-13 16:14:05$
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *******************************************************************************
 */
#include "type.h"
#include "gpio.h"

#ifdef FUNC_OS_EN
#include "rtos_api.h"
osMutexId SwUARTMutex = NULL;
#endif

#include "sw_uart.h"

#ifdef CFG_APP_CONFIG
#include "app_config.h"
#endif

#ifdef SW_UART_IO_PORT
#if (CFG_SW_UART_BANDRATE == 115200)
#if SYS_CORE_SET_MODE == CORE_USER_MODE
#define SW_DELAY	40
#else
#define SW_DELAY	47
#endif
#elif(CFG_SW_UART_BANDRATE == 512000)
#if SYS_CORE_SET_MODE == CORE_USER_MODE
#define SW_DELAY	9
#else
#define SW_DELAY	11
#endif
#else
#if SYS_CORE_SET_MODE == CORE_USER_MODE
#define SW_DELAY	9
#else
#define SW_DELAY	11
#endif
#endif


#if (SW_UART_IO_PORT == GPIO_A_IN)
#define SW_PORT			GPIO_A_IN
#define SW_OUT_REG		0x40040004
#else
#define SW_PORT			GPIO_B_IN
#define SW_OUT_REG		0x40040034
#endif

#define SW_PIN_MASK		(1<<SW_UART_IO_PORT_PIN_INDEX)

//校验位选择，分别对应 0:无校验 1:奇校验 2:偶校验
#define SW_UART_PARITY_BITS_TYPE		0

#include "remap.h"
#define	SW_UART_TCM_SIZE		4
uint8_t IsSwUartActedAsUARTFlag = 0;
static	uint8_t tcm_buf[(SW_UART_TCM_SIZE+1)*1024];

void SwUartDelay(unsigned int us);
void SwUartTxTcmInit(void)
{
	uint32_t StartAddr;
	uint32_t TCMStartAddr;

	TCMStartAddr = (((uint32_t)tcm_buf + 1024) / 1024) * 1024;
	StartAddr = (((uint32_t)SwUartDelay - 1024)/ 1024) *1024;

	Remap_AddrRemapDisable(ADDR_REMAP2);
	memcpy((void*)TCMStartAddr, (void*)StartAddr, SW_UART_TCM_SIZE*1024);
	Remap_AddrRemapSet(ADDR_REMAP2, StartAddr, TCMStartAddr, SW_UART_TCM_SIZE);

//	printf("Tcm_Swdelay: %x-%x ,%x-%x\n",StartAddr,SwUartDelay, TCMStartAddr,tcm_buf);
}
/**
 * @brief  Init specified IO as software uart's TX.
 *
 *         Any other divided frequency is the same with last example.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 * @param  PinIndex:  0 ~ 31, select which gpio io to use.
 *         for example, if PortIndex = SWUART_GPIO_PORT_A, PinIndex = 10,
 *         GPIO_A10 is used as software uart's TX.
 * @param  BaudRate, can be 460800, 256000, 115200, 57600 or 38400
 *
 * @return None.
 */
void SwUartTxInit(uint8_t PortIndex, uint8_t PinIndex, uint32_t BaudRate)
{
	GPIO_RegOneBitSet(SW_PORT + 1, SW_PIN_MASK);//Must output high as default!
	GPIO_RegOneBitClear(SW_PORT + 5, SW_PIN_MASK);//Input disable
	GPIO_RegOneBitSet(SW_PORT + 6, SW_PIN_MASK);//Output enable

	SwUartTxTcmInit();
}

 /**
 * @brief  Deinit uart TX to default gpio.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 *     @arg  SWUART_GPIO_PORT_C
 * @param  PinIndex:  0 ~ 31, select which gpio io to deinit.
 * @return None.
 */
void SwUartTxDeinit(uint8_t PortIndex, uint8_t PinIndex)
{
	GPIO_RegOneBitClear(SW_PORT + 1, SW_PIN_MASK);//OUTPUT = 0;
	GPIO_RegOneBitSet(SW_PORT + 5, SW_PIN_MASK);//IE = 1
	GPIO_RegOneBitClear(SW_PORT + 6, SW_PIN_MASK);//OE = 0
}

/**
 * @Brief	make sw uart baudrate automatic  adaptation
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void SWUartBuadRateAutoAdap(char PreFq, char CurFq)
{

}


/**
 * @brief  Delay to keep tx's level for some time
 * @param  void
 * @return None.
 */
uint32_t Clock_SysClockFreqGet(void); //clk.h
//__attribute__((section(".tcm_section"), optimize("Og")))
void SwUartDelay(unsigned int us)//200ns
{
	int i;
	for(i=0;i<us;i++)
	{
		__asm __volatile__(
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"	
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"			
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"
		"nop\n""nop\n""nop\n"	
		"nop\n"
		);	
	}

}
inline void GIE_DISABLE(void);
inline bool GIE_STATE_GET();
inline void GIE_ENABLE(void);
/**
* @brief  Send 1 byte
* @param  c: byte to be send
* @return None
*/
//__attribute__((section(".tcm_section"), optimize("Og")))
void SwUartSendByte(uint8_t c)
{
	uint8_t i;
	uint8_t Cnt = 0;
#ifndef	CFG_FUNC_LED_REFRESH
	bool ret;

	ret = GIE_STATE_GET();
	GIE_DISABLE();
#else
	InterruptLevelSet(1);
#endif
	(*(volatile unsigned long *) SW_OUT_REG) &= ~(SW_PIN_MASK);
	
		// *((unsigned long *)(0x40040000 + (GPIO_A_IE << 2))) &= ~GPIOA2;

		// *((unsigned long *)(0x40040000 + (GPIO_A_OE << 2))) |= GPIOA2;

		// *((unsigned long *)(0x40040000 + (GPIO_A_OUT << 2))) |= GPIOA2;
		SwUartDelay(SW_DELAY);

		// *((unsigned long *)(0x40040000 + (GPIO_A_IE << 2))) &= ~GPIOA2;

		// *((unsigned long *)(0x40040000 + (GPIO_A_OE << 2))) |= GPIOA2;

		// *((unsigned long *)(0x40040000 + (GPIO_A_OUT << 2))) &= ~GPIOA2;
	for(i=0; i<8; i++)
	{
		if(c & 0x01)
		{
			//GpioSetRegBits(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
			(*(volatile unsigned long *) SW_OUT_REG) |= SW_PIN_MASK;
			Cnt++;
		}
		else
		{
			//GpioClrRegBits(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
			(*(volatile unsigned long *) SW_OUT_REG) &= ~(SW_PIN_MASK);
		}
		SwUartDelay(SW_DELAY);
		c >>= 1;
	}
#if (SW_UART_PARITY_BITS_TYPE == 1)	//1:奇校验 2:偶校验
	if(Cnt % 2)//偶数
	{
		(*(volatile unsigned long *) SW_OUT_REG) &= ~(SW_PIN_MASK);
	}
	else
	{
		(*(volatile unsigned long *) SW_OUT_REG) |= SW_PIN_MASK;
	}
	SwUartDelay(SW_DELAY);
#elif (SW_UART_PARITY_BITS_TYPE == 2) //1:奇校验 2:偶校验
	if(Cnt % 2)//偶数
	{
		(*(volatile unsigned long *) SW_OUT_REG) |= SW_PIN_MASK;
	}
	else
	{
		(*(volatile unsigned long *) SW_OUT_REG) &= ~(SW_PIN_MASK);
	}
	SwUartDelay(SW_DELAY);
#endif

	(*(volatile unsigned long *) SW_OUT_REG) |= SW_PIN_MASK;
	SwUartDelay(SW_DELAY);
#ifndef	CFG_FUNC_LED_REFRESH
	if(ret)
	{
		GIE_ENABLE();
	}
#else
	InterruptLevelRestore();
#endif
}

/**
 * @brief  Send data from buffer
 * @param  Buf: Buffer address
 * @param  BufLen: Length of bytes to be send
 * @return None
 */
void SwUartSend(uint8_t* Buf, uint32_t BufLen)
{
	while(BufLen--)
	{
		SwUartSendByte(*Buf++);
	}
}

void EnableSwUartAsFuart(bool EnableFlag)
{
	IsSwUartActedAsUARTFlag = EnableFlag;
}

#endif

