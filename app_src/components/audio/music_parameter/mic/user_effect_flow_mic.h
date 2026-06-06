/***************************************************
 * @file     user_effect_flow_mic.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 5.0.6
 * @version V1.2.0 
 * @Created 2025-12-10T16:06:11 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_MIC_H__
#define __USER_EFFECT_FLOW_MIC_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define MIC_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _MIC_roboeffect_io_enum
{
    MIC_SOURCE_MIC_SOURCE,
    MIC_SOURCE_APP_SOURCE,
    MIC_SOURCE_REMIND_SOURCE,
    MIC_SOURCE_REC_SOURCE,

    MIC_SINK_DAC0_SINK,
    MIC_SINK_APP_SINK,
    MIC_SINK_STEREO_SINK,
    MIC_SINK_SPDIF_SINK,
    MIC_SINK_REC_SINK,
} MIC_roboeffect_io_enum;


typedef enum _MIC_roboeffect_effect_list_enum{

    MIC_upmix_1to2_0_ADDR = 0x81,
    MIC_silence_detector_mic_ADDR,
    MIC_preGain_ADDR,
    MIC_silence_detector_music_ADDR,
    MIC_remind_gain_control_ADDR,
    MIC_mic_eq0_ADDR,
    MIC_mic_ns_ADDR,
    MIC_mic_EQ_ADDR,
    MIC_mic_drc_ADDR,
    MIC_mic_gain_ADDR,
    MIC_gain_control0_ADDR,
    MIC_noise_suppressor_expander0_ADDR,
    MIC_gain_control2_ADDR,
    MIC_eq1_ADDR,
    MIC_compander_ADDR,
    MIC_low_level_compressor1_ADDR,
    MIC_harmonic_exciter0_ADDR,
    MIC_mvbass_ADDR,
    MIC_3D_ADDR,
    MIC_eq2_ADDR,
    MIC_music_drc_ADDR,
    MIC_music_EQ_ADDR,
    MIC_gain_control1_ADDR,
    MIC_eq0_ADDR,
    MIC_low_level_compressor0_ADDR,
    MIC_COUNT_ADDR,

} MIC_roboeffect_effect_list_enum;

extern const char chart_name_mic[];

extern const unsigned char user_effects_script_mic[];

extern roboeffect_effect_list_info user_effect_list_mic;

extern const roboeffect_effect_steps_table user_effect_steps_mic;

extern uint32_t get_user_effects_script_len_mic(void);

extern char *parameter_group_name_mic[1];
extern const unsigned char user_effect_parameters_mic_Mic[];
extern const unsigned char user_module_parameters_mic_Mic[];
#endif/*__USER_EFFECT_FLOW_MIC_H__*/
