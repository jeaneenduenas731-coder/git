/**
 **************************************************************************************
 * @file    bt_stack_memory.h
 * @brief   
 *
 * @author  kk
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __BT_STACK_MEMORY_H__
#define __BT_STACK_MEMORY_H__

#include "type.h"
#include "bt_config.h"


/*****************************************************************
 * ram config
 *****************************************************************/
typedef struct _BT_STACK_MEM_ALLOC_
{
	uint8_t	BtBaseMem[9000]; //common

#if (BT_A2DP_SUPPORT)		//a2dp+avrcp
	uint8_t	BtA2DPMem[15500];
#endif

#if 1//(BLE_SUPPORT)
	uint8_t	BtBleMem[2400];
#endif

#ifdef BT_AUDIO_AAC_ENABLE
	uint8_t	BtAacMem[900];
#endif

#if ((BT_AVRCP_VOLUME_SYNC) || (BT_AVRCP_PLAYER_SETTING) ||(BT_SOURCE_SUPPORT))
	uint8_t	BtAvrcpTGMem[6300];
#endif

#if (BT_HFP_SUPPORT)
	uint8_t	BtHfpMem[5800];
#endif

#if (BT_SPP_SUPPORT)
	uint8_t	BtSppMem[700];
#endif

#if BT_SOURCE_SUPPORT
	uint8_t	BtSourceMem[400];
#endif

#if (BT_HFG_SUPPORT)
	uint8_t	BtHFGMem[1024];
#endif

#if (BT_OBEX_SUPPORT)
	uint8_t	BtOBEXMem[400];
#endif

#if (BT_PBAP_SUPPORT)
	uint8_t	BtPbapMem[900];
#endif

}BT_STACK_MEM_ALLOC;


#define BT_STACK_MEM_SIZE		sizeof(BT_STACK_MEM_ALLOC)

#endif //__BT_STACK_MEMORY_H__

