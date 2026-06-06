#include "user_effect_flow_tdm.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA tdm_effect_para =
{
	.user_effect_name = (uint8_t *)"TDM",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_tdm,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_tdm,
	.user_effects_script = (uint8_t *)user_effects_script_tdm,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_tdm_TDM,
	.user_module_parameters = (uint8_t *)user_module_parameters_tdm_TDM,
	.get_user_effects_script_len = get_user_effects_script_len_tdm,
};

const AUDIOEFFECT_SOURCE_SINK_NUM tdm_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = TDM_SOURCE_MIC_SOURCE,
	.remind_source = TDM_SOURCE_REMIND_SOURCE,
	.rec_source = TDM_SOURCE_REC_SOURCE,
	.i2s_tdm0_source = TDM_SOURCE_I2S_TDM0_SOURCE,
	.i2s_tdm1_source = TDM_SOURCE_I2S_TDM1_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = TDM_SINK_DAC0_SINK,
	.app_sink = TDM_SINK_APP_SINK,
	.stereo_sink = TDM_SINK_STEREO_SINK,
	.rec_sink = TDM_SINK_REC_SINK,
	.spdif_sink = TDM_SINK_SPDIF_SINK,
};

const uint8_t tdm_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = TDM_eq1_ADDR,
	[MUSIC_VOLUME_ADJUST] = TDM_gain_control0_ADDR,
	[MIC_VOLUME_ADJUST] = TDM_mic_gain_ADDR,
	[REMIND_VOLUME_ADJUST] = TDM_gain_control3_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = TDM_silence_detector_mic_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = TDM_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = TDM_preGain_ADDR,
};

