/***************************************************
 * @file     user_effect_flow_hfp.h  
 * @brief   auto generated  
 * @author  ACPWorkbench: 3.21.9
 * @version V1.2.0 
 * @Created 2025-12-12T17:33:23 

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_HFP_H__
#define __USER_EFFECT_FLOW_HFP_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define HFP_ROBOEFFECT_LIB_VER "2.36.0"

typedef enum _HFP_roboeffect_io_enum
{
    HFP_SOURCE_MIC_SOURCE,
    HFP_SOURCE_APP_SOURCE,
    HFP_SOURCE_REMIND_SOURCE,
    HFP_SOURCE_REC_SOURCE,

    HFP_SINK_DAC0_SINK,
    HFP_SINK_APP_SINK,
    HFP_SINK_STEREO_SINK,
    HFP_SINK_SPDIF_SINK,
    HFP_SINK_REC_SINK,
} HFP_roboeffect_io_enum;


typedef enum _HFP_roboeffect_effect_list_enum{

    HFP_silence_detector_mic_ADDR = 0x81,
    HFP_aec0_ADDR,
    HFP_noise_suppressor_blue0_ADDR,
    HFP_mic_EQ_ADDR,
    HFP_mic_drc_ADDR,
    HFP_mic_gain_ADDR,
    HFP_preGain_ADDR,
    HFP_silence_detector_music_ADDR,
    HFP_music_preEQ_ADDR,
    HFP_music_drc_ADDR,
    HFP_pcm_delay0_ADDR,
    HFP_music_gain_ADDR,
    HFP_upmix_1to2_0_ADDR,
    HFP_remind_gain_control_ADDR,
    HFP_COUNT_ADDR,

} HFP_roboeffect_effect_list_enum;

extern const char chart_name_hfp[];

extern const unsigned char user_effects_script_hfp[];

extern roboeffect_effect_list_info user_effect_list_hfp;

extern const roboeffect_effect_steps_table user_effect_steps_hfp;

extern uint32_t get_user_effects_script_len_hfp(void);

extern char *parameter_group_name_hfp[1];
extern const unsigned char user_effect_parameters_hfp_Hfp[];
extern const unsigned char user_module_parameters_hfp_Hfp[];
#endif/*__USER_EFFECT_FLOW_HFP_H__*/
