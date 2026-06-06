#include "user_effect_flow_mic.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA mic_effect_para =
{
	.user_effect_name = (uint8_t *)"Mic",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_mic,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_mic,
	.user_effects_script = (uint8_t *)user_effects_script_mic,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_mic_Mic,
	.user_module_parameters = (uint8_t *)user_module_parameters_mic_Mic,
	.get_user_effects_script_len = get_user_effects_script_len_mic,
};

const AUDIOEFFECT_SOURCE_SINK_NUM mic_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = MIC_SOURCE_MIC_SOURCE,
	.app_source = MIC_SOURCE_APP_SOURCE,
	.remind_source = MIC_SOURCE_REMIND_SOURCE,
	.rec_source = MIC_SOURCE_REC_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = MIC_SINK_DAC0_SINK,
	.app_sink = MIC_SINK_APP_SINK,
	.stereo_sink = MIC_SINK_STEREO_SINK,
	.rec_sink = MIC_SINK_REC_SINK,
	.spdif_sink = MIC_SINK_SPDIF_SINK,
};

const uint8_t mic_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = MIC_mic_eq0_ADDR,
	[MUSIC_VOLUME_ADJUST] = MIC_gain_control0_ADDR,
	[MIC_VOLUME_ADJUST] = MIC_mic_gain_ADDR,
	[REMIND_VOLUME_ADJUST] = MIC_remind_gain_control_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = MIC_silence_detector_mic_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = MIC_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = MIC_preGain_ADDR,
};
