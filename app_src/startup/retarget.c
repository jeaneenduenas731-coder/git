/*
 * retarget.c
 *
 *  Created on: Mar 8, 2017
 *      Author: peter
 */

#include <stdio.h>
#include "uarts_interface.h"
#include "type.h"
#include "remap.h"
#if 1//def CFG_APP_CONFIG
#include "app_config.h"
#endif
uint8_t DebugPrintPort = UART_PORT0;

#include "uarts.h"
#include "sw_uart.h"
#include "rtos_api.h"
#include "mcu_circular_buf.h"

#ifdef CFG_FUNC_DEBUG_USE_TIMER
MCU_CIRCULAR_CONTEXT log_CircularBuf;
uint8_t log_buf[4096];
volatile uint8_t uart_switch = 0;
static uint16_t sQueuePrintfLock = 0;
#endif

int DbgUartInit(int Which, unsigned int BaudRate, unsigned char DatumBits, unsigned char Parity, unsigned char StopBits)
{
#ifdef CFG_FUNC_DEBUG_EN
	DebugPrintPort = Which;
	if(DebugPrintPort == UART_PORT0 || DebugPrintPort == UART_PORT1)
	{
		UARTS_Init(DebugPrintPort,BaudRate, DatumBits,  Parity,  StopBits);
	}
	else
	{
		//uartfun = (uartfun)SwUartSend;
	}
#ifdef CFG_FUNC_DEBUG_USE_TIMER
	MCUCircular_Config(&log_CircularBuf,log_buf,sizeof(log_buf));
#endif
#endif
	return 0;
}
#ifdef CFG_FUNC_USBDEBUG_EN
uint8_t 	usb_buffer[4096];
MCU_CIRCULAR_CONTEXT usb_fifo;

void usb_hid_printf_init(void)
{
	MCUCircular_Config(&usb_fifo,usb_buffer,sizeof(usb_buffer));
}
#endif
extern uint8_t IsSwUartActedAsUARTFlag;
__attribute__((used))
int putchar(int c)
{
#ifdef CFG_FUNC_DEBUG_EN
#ifdef CFG_USE_SW_UART
	if(IsSwUartActedAsUARTFlag)
	{
		if((unsigned char)c == '\n')
		{
			const char lfca[2] = "\r\n";
			SwUartSend((unsigned char*)lfca, 2);
		}
		else
		{
			SwUartSend((unsigned char*)&c, 1);
		}
	}
	return c;
#endif
#ifdef CFG_FUNC_DEBUG_USE_TIMER
	if(uart_switch)
	{
		if(sQueuePrintfLock)return;
		sQueuePrintfLock = 1;
		{
			uint8_t buf[1];
			buf[0] = c;
			if(MCUCircular_GetSpaceLen(&log_CircularBuf)>=1)
			{
				MCUCircular_PutData_Printf(&log_CircularBuf,buf,1);

			}
		}
		sQueuePrintfLock = 0;
	}
	else
#endif
	{
		if (c == '\n')
		{
			UARTS_SendByte_In_Interrupt(DebugPrintPort, '\r');
			UARTS_SendByte_In_Interrupt(DebugPrintPort, '\n');

			//UART0_SendByte('\r');
			//UART0_SendByte('\n');
		}
		else
		{
			UARTS_SendByte_In_Interrupt(DebugPrintPort, (uint8_t)c);
			//UART0_SendByte((uint8_t)c);
		}
	}
#endif
	return c;
}

#ifdef CFG_FUNC_DEBUG_USE_TIMER
void uart_log_out(void)
{
	int i;
	if(uart_switch == 0)
		return;
	if(DebugPrintPort == UART_PORT0)
	{
		uint32_t reg_status = *(uint32_t*)0x40005014;
		for(i=0;i<4;i++)
		{
			if(reg_status&0x100)//tx_data_int
			{
				*(uint8_t*)0x4000501C &= ~4;//tx_int_clr
			}
			if(reg_status&0x200)//tx_mcu_allow
			{
				if(MCUCircular_GetDataLen(&log_CircularBuf))
				{
					uint8_t tab[1];
					MCUCircular_GetData_Printf(&log_CircularBuf,tab,1);
					*(uint32_t*)0x40005018 = tab[0];
				}
			}
			if(reg_status&0x100)//tx_data_int
			{
				*(uint8_t*)0x4000501C &= ~4;//tx_int_clr
			}
		}
	}
	else
	{
		uint32_t reg_status = *(uint32_t*)0x40006014;
		for(i=0;i<4;i++)
		{
			if(reg_status&0x100)//tx_data_int
			{
				*(uint8_t*)0x4000601C &= ~4;//tx_int_clr
			}
			if(reg_status&0x200)//tx_mcu_allow
			{
				if(MCUCircular_GetDataLen(&log_CircularBuf))
				{
					uint8_t tab[1];
					MCUCircular_GetData_Printf(&log_CircularBuf,tab,1);
					*(uint32_t*)0x40006018 = tab[0];
				}
			}
			if(reg_status&0x100)//tx_data_int
			{
				*(uint8_t*)0x4000601C &= ~4;//tx_int_clr
			}
		}
	}
}

void PrintfAllInBuffer(void)
{
	uart_switch = 0;
	while(MCUCircular_GetDataLen(&log_CircularBuf))
	{
		uint8_t data;
		MCUCircular_GetData_Printf(&log_CircularBuf,&data,1);
		putchar(data);
	}

}
#endif

__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{
#ifdef CFG_FUNC_DEBUG_EN	
	int i;
	//usb_hid.usb_len = size;

		for (i = 0; i < size; i++)
		{
			putchar(buf[i]);
		}
#endif

#ifdef CFG_FUNC_USBDEBUG_EN
	MCUCircular_PutData(&usb_fifo,(void*)buf,size);
#endif		
}
