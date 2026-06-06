#include "user_effect_flow_bis.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA bis_effect_para =
{
	.user_effect_name = chart_name_bis,
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_bis,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_bis,
	.user_effects_script = (uint8_t *)user_effects_script_bis,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_bis_Bis,
	.user_module_parameters = (uint8_t *)user_module_parameters_bis_Bis,
	.get_user_effects_script_len = get_user_effects_script_len_bis,
};

const AUDIOEFFECT_SOURCE_SINK_NUM bis_mode =
{
	//不要删除，source/sink默认值
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCE映射
	.mic_source = BIS_SOURCE_MIC_SOURCE,
	.app_source = BIS_SOURCE_APP_SOURCE,
	.remind_source = BIS_SOURCE_REMIND_SOURCE,
	.rec_source = BIS_SOURCE_REC_SOURCE,
	.i2s_mix_source = BIS_SOURCE_BIS_I2S,

	//ROBOEFFECT effect SINK映射
	.dac0_sink = BIS_SINK_DAC0_SINK,
	.app_sink = BIS_SINK_APP_SINK,
	.stereo_sink = BIS_SINK_BIS_SINK,
	.rec_sink = BIS_SINK_REC_SINK,
};

const uint8_t bis_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = BIS_mic_eq0_ADDR,
	[MUSIC_VOLUME_ADJUST] = BIS_gain_control0_ADDR,
	[MIC_VOLUME_ADJUST] = BIS_mic_gain_ADDR,
	[REMIND_VOLUME_ADJUST] = BIS_remind_gain_control_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = BIS_silence_detector_mic_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = BIS_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = BIS_preGain_ADDR,
};

//BIS 通道切换 控制节点index
uint8_t GetBisEffectRouteSelector(void)
{
	return BIS_route_selector0_ADDR;
}
