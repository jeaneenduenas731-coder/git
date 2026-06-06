#include "user_effect_flow_bypass.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA bypass_effect_para =
{
	.user_effect_name = (uint8_t *)"Bypass",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_bypass,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_bypass,
	.user_effects_script = (uint8_t *)user_effects_script_bypass,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_bypass_Bypass,
	.user_module_parameters = (uint8_t *)user_module_parameters_bypass_Bypass,
	.get_user_effects_script_len = get_user_effects_script_len_bypass,
};

const AUDIOEFFECT_SOURCE_SINK_NUM bypass_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = BYPASS_SOURCE_MIC_SOURCE,
	.app_source = BYPASS_SOURCE_APP_SOURCE,
	.remind_source = BYPASS_SOURCE_REMIND_SOURCE,
	.rec_source = BYPASS_SOURCE_REC_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = BYPASS_SINK_DAC0_SINK,
	.app_sink = BYPASS_SINK_APP_SINK,
	.stereo_sink = BYPASS_SINK_STEREO_SINK,
	.rec_sink = BYPASS_SINK_REC_SINK,
	.spdif_sink = BYPASS_SINK_SPDIF_SINK,
};
