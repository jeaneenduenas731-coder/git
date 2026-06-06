/***************************************************
 * @file     user_effect_flow_bypass.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 5.0.6
 * @version V1.2.0 
 * @Created 2025-12-10T16:06:10 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_BYPASS_H__
#define __USER_EFFECT_FLOW_BYPASS_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define BYPASS_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _BYPASS_roboeffect_io_enum
{
    BYPASS_SOURCE_MIC_SOURCE,
    BYPASS_SOURCE_APP_SOURCE,
    BYPASS_SOURCE_REMIND_SOURCE,
    BYPASS_SOURCE_REC_SOURCE,

    BYPASS_SINK_APP_SINK,
    BYPASS_SINK_DAC0_SINK,
    BYPASS_SINK_STEREO_SINK,
    BYPASS_SINK_SPDIF_SINK,
    BYPASS_SINK_REC_SINK,
} BYPASS_roboeffect_io_enum;


typedef enum _BYPASS_roboeffect_effect_list_enum{

    BYPASS_upmix_1to2_0_ADDR = 0x81,
    BYPASS_mic_gain_ADDR,
    BYPASS_gain_control1_ADDR,
    BYPASS_gain_control0_ADDR,
    BYPASS_COUNT_ADDR,

} BYPASS_roboeffect_effect_list_enum;

extern const char chart_name_bypass[];

extern const unsigned char user_effects_script_bypass[];

extern roboeffect_effect_list_info user_effect_list_bypass;

extern const roboeffect_effect_steps_table user_effect_steps_bypass;

extern uint32_t get_user_effects_script_len_bypass(void);

extern char *parameter_group_name_bypass[1];
extern const unsigned char user_effect_parameters_bypass_Bypass[];
extern const unsigned char user_module_parameters_bypass_Bypass[];
#endif/*__USER_EFFECT_FLOW_BYPASS_H__*/
