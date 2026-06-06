#include "user_effect_flow_MICUSBAI.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA micusbAI_effect_para =
{
	.user_effect_name = (uint8_t *)"MICUSBAI",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_MICUSBAI,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_MICUSBAI,
	.user_effects_script = (uint8_t *)user_effects_script_MICUSBAI,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_MICUSBAI_MICUSBAI,
	.user_module_parameters = (uint8_t *)user_module_parameters_MICUSBAI_MICUSBAI,
	.get_user_effects_script_len = get_user_effects_script_len_MICUSBAI,
};

const AUDIOEFFECT_SOURCE_SINK_NUM micusbAI_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = MICUSBAI_SOURCE_MIC_SOURCE,
	.app_source = MICUSBAI_SOURCE_APP_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = MICUSBAI_SINK_DAC0_SINK,
	.app_sink = MICUSBAI_SINK_APP_SINK,
};
