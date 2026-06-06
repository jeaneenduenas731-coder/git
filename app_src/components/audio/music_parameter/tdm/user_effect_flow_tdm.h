/***************************************************
 * @file     user_effect_flow_tdm.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 5.0.6
 * @version V1.2.0 
 * @Created 2025-12-10T16:06:12 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_TDM_H__
#define __USER_EFFECT_FLOW_TDM_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define TDM_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _TDM_roboeffect_io_enum
{
    TDM_SOURCE_MIC_SOURCE,
    TDM_SOURCE_I2S_TDM1_SOURCE,
    TDM_SOURCE_REMIND_SOURCE,
    TDM_SOURCE_REC_SOURCE,
    TDM_SOURCE_I2S_TDM0_SOURCE,

    TDM_SINK_DAC0_SINK,
    TDM_SINK_APP_SINK,
    TDM_SINK_STEREO_SINK,
    TDM_SINK_SPDIF_SINK,
    TDM_SINK_REC_SINK,
} TDM_roboeffect_io_enum;


typedef enum _TDM_roboeffect_effect_list_enum{

    TDM_upmix_1to2_0_ADDR = 0x81,
    TDM_preGain_ADDR,
    TDM_gain_control1_ADDR,
    TDM_gain_control3_ADDR,
    TDM_silence_detector_mic_ADDR,
    TDM_silence_detector_music_ADDR,
    TDM_mic_eq0_ADDR,
    TDM_mic_ns_ADDR,
    TDM_mic_EQ_ADDR,
    TDM_mic_drc_ADDR,
    TDM_mic_gain_ADDR,
    TDM_gain_control0_ADDR,
    TDM_noise_suppressor_expander0_ADDR,
    TDM_gain_control2_ADDR,
    TDM_eq1_ADDR,
    TDM_COUNT_ADDR,

} TDM_roboeffect_effect_list_enum;

extern const char chart_name_tdm[];

extern const unsigned char user_effects_script_tdm[];

extern roboeffect_effect_list_info user_effect_list_tdm;

extern const roboeffect_effect_steps_table user_effect_steps_tdm;

extern uint32_t get_user_effects_script_len_tdm(void);

extern char *parameter_group_name_tdm[1];
extern const unsigned char user_effect_parameters_tdm_TDM[];
extern const unsigned char user_module_parameters_tdm_TDM[];
#endif/*__USER_EFFECT_FLOW_TDM_H__*/
