/***************************************************
 * @file     user_effect_flow_bis.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 5.0.6
 * @version V1.2.0 
 * @Created 2025-12-10T16:06:10 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_BIS_H__
#define __USER_EFFECT_FLOW_BIS_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define BIS_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _BIS_roboeffect_io_enum
{
    BIS_SOURCE_MIC_SOURCE,
    BIS_SOURCE_APP_SOURCE,
    BIS_SOURCE_REMIND_SOURCE,
    BIS_SOURCE_REC_SOURCE,
    BIS_SOURCE_BIS_I2S,

    BIS_SINK_APP_SINK,
    BIS_SINK_DAC0_SINK,
    BIS_SINK_BIS_SINK,
    BIS_SINK_REC_SINK,
} BIS_roboeffect_io_enum;


typedef enum _BIS_roboeffect_effect_list_enum{

    BIS_freq_shifter0_ADDR = 0x81,
    BIS_howling_guard0_ADDR,
    BIS_upmix_1to2_0_ADDR,
    BIS_silence_detector_mic_ADDR,
    BIS_preGain_ADDR,
    BIS_silence_detector_music_ADDR,
    BIS_route_selector0_ADDR,
    BIS_remind_gain_control_ADDR,
    BIS_gain_control1_ADDR,
    BIS_mic_eq0_ADDR,
    BIS_mic_ns_ADDR,
    BIS_mic_EQ_ADDR,
    BIS_mic_drc_ADDR,
    BIS_reverb0_ADDR,
    BIS_mic_gain_ADDR,
    BIS_gain_control0_ADDR,
    BIS_noise_suppressor_expander0_ADDR,
    BIS_compander_ADDR,
    BIS_low_level_compressor1_ADDR,
    BIS_harmonic_exciter0_ADDR,
    BIS_mvbass_ADDR,
    BIS_3D_ADDR,
    BIS_eq2_ADDR,
    BIS_music_drc_ADDR,
    BIS_music_EQ_ADDR,
    BIS_COUNT_ADDR,

} BIS_roboeffect_effect_list_enum;

extern const char chart_name_bis[];

extern const unsigned char user_effects_script_bis[];

extern roboeffect_effect_list_info user_effect_list_bis;

extern const roboeffect_effect_steps_table user_effect_steps_bis;

extern uint32_t get_user_effects_script_len_bis(void);

extern char *parameter_group_name_bis[1];
extern const unsigned char user_effect_parameters_bis_Bis[];
extern const unsigned char user_module_parameters_bis_Bis[];
#endif/*__USER_EFFECT_FLOW_BIS_H__*/
