#include "user_effect_flow_TwoAudio.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA TwoAudio_effect_para =
{
	.user_effect_name = (uint8_t *)"TwoAudio",
	.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_TwoAudio,
	.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_TwoAudio,
	.user_effects_script = (uint8_t *)user_effects_script_TwoAudio,
	.user_effect_parameters = (uint8_t *)user_effect_parameters_TwoAudio_Mic,
	.user_module_parameters = (uint8_t *)user_module_parameters_TwoAudio_Mic,
	.get_user_effects_script_len = get_user_effects_script_len_TwoAudio,
};

const AUDIOEFFECT_SOURCE_SINK_NUM TwoAudio_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = TWOAUDIO_SOURCE_MIC_SOURCE,
	.app_source = TWOAUDIO_SOURCE_APP_SOURCE,
	.remind_source = TWOAUDIO_SOURCE_REMIND_SOURCE,
	.rec_source = TWOAUDIO_SOURCE_REC_SOURCE,
	.usb_source = TWOAUDIO_SOURCE_USB_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = TWOAUDIO_SINK_DAC0_SINK,
	.app_sink = TWOAUDIO_SINK_APP_SINK,
	.stereo_sink = TWOAUDIO_SINK_STEREO_SINK,
	.rec_sink = TWOAUDIO_SINK_REC_SINK,
	.spdif_sink = TWOAUDIO_SINK_SPDIF_SINK,
};

const uint8_t TwoAudio_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = TWOAUDIO_mic_eq0_ADDR,
	[MUSIC_VOLUME_ADJUST] = TWOAUDIO_preGain_ADDR,
	[MIC_VOLUME_ADJUST] = TWOAUDIO_mic_gain_ADDR,
	[REMIND_VOLUME_ADJUST] = TWOAUDIO_remind_gain_control_ADDR,
	[MIC_SILENCE_DETECTOR_PARAM] = TWOAUDIO_silence_detector_mic_ADDR,
	[MUSIC_SILENCE_DETECTOR_PARAM] = TWOAUDIO_silence_detector_music_ADDR,
	[APPMODE_PREGAIN] = TWOAUDIO_preGain_ADDR,
};
