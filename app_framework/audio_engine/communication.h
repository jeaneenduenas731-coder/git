/**
 **************************************************************************************
 * @file    communication.h
 * @brief   
 *
 * @author  Castle Cai
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "roboeffect_api.h"

#define STREAM_CLIPS_LEN 240
extern uint32_t get_user_effects_script_len(void);
extern uint32_t get_user_effects_script_len_1(void);
extern const uint8_t effect_property_for_display[];

//v3 package 
#define V3_PACKAGE_TYPE_LIB           	0
#define V3_PACKAGE_TYPE_PARAM           1
#define V3_PACKAGE_TYPE_STREAM          2
#define V3_PACKAGE_TYPE_EFFECT_CHAIN    3
#define V3_PACKAGE_TYPE_STEP_TABLE      4
#define V3_PACKAGE_TYPE_EFFECT_NAME     5
#define V3_PACKAGE_TYPE_EFFECT_ID       6
#define V3_PACKAGE_EFFECT_ALL_PARAM_EFFECT		9
#define V3_PACKAGE_EFFECT_ALL_PARAM_CODEC 		10
#define V3_PACKAGE_EFFECT_SOURCESINK_STATUS		11

void AudioEffect_CommunicationQueue_Init();

void AudioEffect_CommunicationQueue_Deinit();

bool AudioEffect_CommunicationQueue_Send(uint32_t sendMsgType);

uint32_t AudioEffect_CommunicationQueue_Recv();

#endif/*__COMMUNICATION_H__*/
