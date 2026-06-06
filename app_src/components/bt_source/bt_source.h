/**
 **************************************************************************************
 * @file    bt_hf_api.h
 * @brief	bt source function
 *
 * @author  bkd
 * @version V1.0.0
 *
 * $Created: 2020-11-18 
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __BT_SOURCE_H__
#define __BT_SOURCE_H__

extern void BtSourceInit(void);
extern uint16_t SbcSourceEncode(void* InBuf, uint16_t InLen);
extern void SbcSourceSendData(void);
extern uint16_t GetBtSourceFifoSpaceLength(void);

extern void BtSourceReInquiryChack(void);
extern void BtSourceNameGetChack(void);
extern uint8_t SetBtSourceDefaultAccessMode(void);
void BtSourceBtModeInit(void);
void BtSourceBtModeDeinit(void);
uint8_t BtSourceReconnectDevExcute(void);
void BtSourcePublicMsgPross(uint32_t msgID);

void BtSourceEventInquiryComplete(void);
void BtSourceEventCommonLinkConnectIND(void * para);
void BtSourceEventCommonGetRemdevNameTimeout(void * para);
void BtSourceEventCommonInquiryResult(void * para);
void BtSourceGetRemoteName(void * para);

extern volatile uint32_t gSwitchSourceAndSink;

//app
void BthfgOutgoingWait(char *number);		//呼出等待
void BthfgAnswer(uint8_t flag);				//接听电话
void BthfgCallStateClear(void);				//清除当前通话状态
void BthfgIncomingWait(char *number);
void BthfgOutgoingWait(char *number);		//呼出等待

#endif /*__BT_HF_API_H__*/



