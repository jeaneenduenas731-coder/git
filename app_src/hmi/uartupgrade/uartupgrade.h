/**
 **************************************************************************************
 * @file    uartupgrade.h
 * @brief
 *
 * @author  Jarvis
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __UARTUPGRADE_H__
#define __UARTUPGRADE_H__

#include <stdint.h>
#include <string.h>
#include "ctrlvars.h"


#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

typedef struct {
	uint8_t len;
	uint8_t cmd;
	uint8_t cmd_data[255];
}UartUpCmd;

#define USB_UPB5_HANDSHAKE  0x00
#define USB_UPB5_HANDSHAKE_ACK  0x01
#define USB_UPG1TX_HANDSHAKE  0x10
#define USB_UPG1TX_HANDSHAKE_ACK  0x11
#define USB_UPG1RX_HANDSHAKE  0x20
#define USB_UPG1RX_HANDSHAKE_ACK  0x21
#define USB_UPB5TX_HANDSHAKE  0x50
#define USB_UPB5TX_HANDSHAKE_ACK  0x51
#define USB_UPB5RX_HANDSHAKE  0x60
#define USB_UPB5RX_HANDSHAKE_ACK  0x61
#define USB_UP_WRITEDATE  0x30
#define USB_UP_WRITEDATE_ACK  0x31
#define UART_UP_WRITEDATE_ACK_ERR  0x32
#define USB_UP_END  0x40
#define USB_UP_END_ACK  0x41

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__UARTUPGRADE_H__
