/***************************************************
 * @file     user_effect_flow_TwoAudio.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 5.0.6
 * @version V1.2.0 
 * @Created 2025-12-10T16:06:13 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_TWOAUDIO_H__
#define __USER_EFFECT_FLOW_TWOAUDIO_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define TWOAUDIO_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _TWOAUDIO_roboeffect_io_enum
{
    TWOAUDIO_SOURCE_MIC_SOURCE,
    TWOAUDIO_SOURCE_APP_SOURCE,
    TWOAUDIO_SOURCE_USB_SOURCE,
    TWOAUDIO_SOURCE_REMIND_SOURCE,
    TWOAUDIO_SOURCE_REC_SOURCE,

    TWOAUDIO_SINK_DAC0_SINK,
    TWOAUDIO_SINK_APP_SINK,
    TWOAUDIO_SINK_STEREO_SINK,
    TWOAUDIO_SINK_SPDIF_SINK,
    TWOAUDIO_SINK_REC_SINK,
} TWOAUDIO_roboeffect_io_enum;


typedef enum _TWOAUDIO_roboeffect_effect_list_enum{

    TWOAUDIO_upmix_1to2_0_ADDR = 0x81,
    TWOAUDIO_silence_detector_mic_ADDR,
    TWOAUDIO_preGain_ADDR,
    TWOAUDIO_silence_detector_music_ADDR,
    TWOAUDIO_remind_gain_control_ADDR,
    TWOAUDIO_mic_eq0_ADDR,
    TWOAUDIO_mic_ns_ADDR,
    TWOAUDIO_mic_EQ_ADDR,
    TWOAUDIO_mic_drc_ADDR,
    TWOAUDIO_mic_gain_ADDR,
    TWOAUDIO_gain_control0_ADDR,
    TWOAUDIO_noise_suppressor_expander0_ADDR,
    TWOAUDIO_gain_control2_ADDR,
    TWOAUDIO_eq1_ADDR,
    TWOAUDIO_compander_ADDR,
    TWOAUDIO_low_level_compressor1_ADDR,
    TWOAUDIO_harmonic_exciter0_ADDR,
    TWOAUDIO_mvbass_ADDR,
    TWOAUDIO_3D_ADDR,
    TWOAUDIO_eq2_ADDR,
    TWOAUDIO_music_drc_ADDR,
    TWOAUDIO_music_EQ_ADDR,
    TWOAUDIO_gain_control1_ADDR,
    TWOAUDIO_eq0_ADDR,
    TWOAUDIO_low_level_compressor0_ADDR,
    TWOAUDIO_COUNT_ADDR,

} TWOAUDIO_roboeffect_effect_list_enum;

extern const char chart_name_TwoAudio[];

extern const unsigned char user_effects_script_TwoAudio[];

extern roboeffect_effect_list_info user_effect_list_TwoAudio;

extern const roboeffect_effect_steps_table user_effect_steps_TwoAudio;

extern uint32_t get_user_effects_script_len_TwoAudio(void);

extern char *parameter_group_name_TwoAudio[1];
extern const unsigned char user_effect_parameters_TwoAudio_Mic[];
extern const unsigned char user_module_parameters_TwoAudio_Mic[];
#endif/*__USER_EFFECT_FLOW_TWOAUDIO_H__*/
