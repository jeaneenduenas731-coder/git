/**
 **************************************************************************************
 * @file    adc_key.h
 * @brief   adc key 
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-1-11 18:40:00$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


#ifndef __ADC_KEY_H__
#define __ADC_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include "type.h"
#include "key.h"

uint8_t GetAdcKeyIndex(void);

KeyScanMsg AdcKeyScan(void);

void AdcKeyInit(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__ADC_KEY_H__

