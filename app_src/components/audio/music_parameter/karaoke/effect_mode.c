#include "user_effect_flow_Karaoke.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA HunXiang_effect_para =
{
	.user_effect_name = (uint8_t *)"HunXiang",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_HunXiang,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_HunXiang,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA DianYin_effect_para =
{
	.user_effect_name = (uint8_t *)"DianYin",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_DianYin,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_DianYin,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA MoYin_effect_para =
{
	.user_effect_name = (uint8_t *)"MoYin",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_MoYin,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_MoYin,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA HanMai_effect_para =
{
	.user_effect_name = (uint8_t *)"HanMai",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_HanMai,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_HanMai,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA NanBianNv_effect_para =
{
	.user_effect_name = (uint8_t *)"NanBianNv",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_NanBianNv,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_NanBianNv,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA NvBianNan_effect_para =
{
	.user_effect_name = (uint8_t *)"NvBianNan",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_NvBianNan,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_NvBianNan,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_EFFECT_PARA WaWaYin_effect_para =
{
	.user_effect_name = (uint8_t *)"WaWaYin",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_Karaoke,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_Karaoke,
	.user_effects_script = (uint8_t *)user_effects_script_Karaoke,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_Karaoke_WaWaYin,
	.user_module_parameters = (uint8_t *)user_module_parameters_Karaoke_WaWaYin,
	.get_user_effects_script_len = get_user_effects_script_len_Karaoke
};

const AUDIOEFFECT_SOURCE_SINK_NUM Karaoke_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = KARAOKE_SOURCE_MIC_SOURCE,
	.app_source = KARAOKE_SOURCE_APP_SOURCE,
	.remind_source = KARAOKE_SOURCE_REMIND_SOURCE,
	.rec_source = KARAOKE_SOURCE_REC_SOURCE,
	.usb_source = KARAOKE_SOURCE_USB_SOURCE,
	.i2s_mix_source = KARAOKE_SOURCE_I2S_MIX_SOURCE,
	.i2s_mix2_source = KARAOKE_SOURCE_I2S_MIX2_SOURCE,
	.linein_mix_source = KARAOKE_SOURCE_LINEIN_MIX_SOURCE,


	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = KARAOKE_SINK_DAC0_SINK,
	.app_sink = KARAOKE_SINK_APP_SINK,
	.stereo_sink = KARAOKE_SINK_STEREO_SINK,
	.rec_sink = KARAOKE_SINK_REC_SINK,
	.i2s_mix_sink = KARAOKE_SINK_I2S_MIX_SINK,
	.spdif_sink = KARAOKE_SINK_SPDIF_SINK,
};

const uint8_t Karaoke_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = KARAOKE_eq0_ADDR,
	[_3D_ENABLE] = KARAOKE_3D_ADDR,
	[ECHO_PARAM] = KARAOKE_echo0_ADDR,
	[MUSIC_VOLUME_ADJUST] = KARAOKE_gain_control0_ADDR,
	[MIC_VOLUME_ADJUST] = KARAOKE_gain_control1_ADDR,
	[REMIND_VOLUME_ADJUST] = KARAOKE_gain_control13_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = KARAOKE_silence_detector0_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = KARAOKE_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = KARAOKE_preGain_ADDR,
};
