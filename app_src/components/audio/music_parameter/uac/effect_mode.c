#include "user_effect_flow_uac.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA uac_effect_para =
{
	.user_effect_name = (uint8_t *)"Uac",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_uac,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_uac,
	.user_effects_script = (uint8_t *)user_effects_script_uac,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_uac_Uac,
	.user_module_parameters = (uint8_t *)user_module_parameters_uac_Uac,
	.get_user_effects_script_len = get_user_effects_script_len_uac,
};

const AUDIOEFFECT_SOURCE_SINK_NUM uac_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = UAC_SOURCE_MIC_SOURCE,
	.app_source = UAC_SOURCE_APP_SOURCE,
	.remind_source = UAC_SOURCE_REMIND_SOURCE,
	.rec_source = UAC_SOURCE_REC_SOURCE,
	.usb_host_source = UAC_SOURCE_USB_HOST_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = UAC_SINK_DAC0_SINK,
	.app_sink = UAC_SINK_APP_SINK,
	.stereo_sink = UAC_SINK_STEREO_SINK,
	.rec_sink = UAC_SINK_REC_SINK,
	.spdif_sink = UAC_SINK_SPDIF_SINK,
	.usb_host_mix_sink = UAC_SINK_USB_HOST_SINK,
};

const uint8_t uac_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = UAC_mic_eq0_ADDR,
	[MUSIC_VOLUME_ADJUST] = UAC_preGain_ADDR,
	[MIC_VOLUME_ADJUST] = UAC_mic_gain_ADDR,
	[REMIND_VOLUME_ADJUST] = UAC_remind_gain_control_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = UAC_silence_detector_mic_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = UAC_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = UAC_preGain_ADDR,
};
