#include "user_effect_flow_music.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA music_effect_para =
{
	.user_effect_name = (uint8_t *)"Music",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_music,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_music,
	.user_effects_script = (uint8_t *)user_effects_script_music,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_music_Music,
	.user_module_parameters = (uint8_t *)user_module_parameters_music_Music,
	.get_user_effects_script_len = get_user_effects_script_len_music,
};

const AUDIOEFFECT_SOURCE_SINK_NUM music_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.app_source = MUSIC_SOURCE_APP_SOURCE,
	.remind_source = MUSIC_SOURCE_REMIND_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = MUSIC_SINK_DAC0_SINK,
};

const uint8_t music_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[MUSIC_VOLUME_ADJUST] = MUSIC_gain_control0_ADDR,
	[REMIND_VOLUME_ADJUST] = MUSIC_gain_control3_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = MUSIC_silence_detector_left_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = MUSIC_silence_detector_right_ADDR,
	[APPMODE_PREGAIN] = MUSIC_preGain_ADDR,
};

