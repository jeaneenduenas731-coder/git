/**
 **************************************************************************************
 * @file    mutlibyte_cmd.h
 * @brief   mutlibyte_cmd
 *
 * @author
 * @version V0.0.1
 *
 * $Created: 2024-08-13
 *
 * @Copyright (C) 2024, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


#ifndef __MULTIBYTE_CMD_H__
#define __MULTIBYTE_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include "type.h"

#define CMD_LEN		8

typedef struct _MultiByteCMD
{
    uint8_t result;
    uint8_t len; 	//最大8字节
    uint8_t DataBuf[CMD_LEN];//按8字节申请
}MultiByteCMD;

extern uint8_t user_CmdBuf[8];


uint8_t wireless_user_cmd_tx_result_get(void);

void wireless_user_cmd_tx_result_clean(void);

//TX
void wireless_user_cmd_data_updata(uint8_t* cmd_data, uint8_t len);

void wireless_user_cmd_tx_result_callback(unsigned char result);

void wireless_user_cmd_send(void);

//RX
void wireless_user_cmd_rx_int(void);

void wireless_user_cmd_rx_data_get(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__MULTIBYTE_CMD_H__
