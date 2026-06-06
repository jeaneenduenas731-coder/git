/**
 **************************************************************************************
 * @file    wireless_in_play_mode.h
 * @brief    
 *
 * @author  Pi
 * @version V1.0.0
 *
 * $Created: 2024-12-10 13:06:47$
 *
 * @Copyright (C) 2024, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __WIRELESSIN_PLAY_MODE_H__
#define __WIRELESSIN_PLAY_MODE_H__

#include "type.h"

bool WirelessinPlayInit(void);
bool WirelessinPlayDeinit(void);
void WirelessinPlayRun(uint16_t msgId);

#endif /*__WIRELESSIN_PLAY_MODE_H__*/



