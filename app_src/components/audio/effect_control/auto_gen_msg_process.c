
/***************************************************
 * @file    auto_gen_msg_process.c                      
 * @brief   auto generated                          
 * @author  auto generated
 * @version V1.1.0                                 
 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/

#include "stdio.h"
#include "type.h"
#include "roboeffect_config.h"
#include "roboeffect_api.h"

#include "auto_gen_msg_process.h"
#include "adc_key.h"
#include "user_effect_parameter.h"

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
#include "user_effect_flow_Karaoke.h"
#endif

#ifdef CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC

extern AudioEffectContext AudioEffect;

static inline int16_t NDS32_CLIPS_ADD(int16_t a, int16_t b)
{
    return __nds32__clips((int32_t)a + b, (16)-1);
}

static inline int16_t NDS32_CLIPS_SUB(int16_t a, int16_t b)
{
    return __nds32__clips((int32_t)a - b, (16)-1);
}

int16_t MicBassStep = 0;
int16_t MicTrebStep = 0;
int16_t MusicBassStep = 0;
int16_t MusicTrebStep = 0;
int16_t ReverbStep = 0;


#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicBassStep_none_msg_effect_sync_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 6, &MicBassStep_table[MicBassStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicTrebStep_none_msg_effect_sync_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 11, &MicTrebStep_table[MicTrebStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicBassStep_none_msg_effect_sync_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 6, &MusicBassStep_table[MusicBassStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicTrebStep_none_msg_effect_sync_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 11, &MusicTrebStep_table[MusicTrebStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_none_msg_effect_sync_delay(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 10, 19, 29, 39, 48, 58, 68, 77, 87, 97, 106, 116, 126, 135, 145, 155, 165, 174, 184, 194, 203, 213, 223, 232, 242, 252, 261, 271, 281, 290, 300, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 2, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_none_msg_effect_sync_attenuation(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_none_msg_effect_sync_wet(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_none_msg_effect_sync_room(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 3, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicBassStep_down_stop_msg_mic_bass_dw_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(--MicBassStep < 0) MicBassStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 6, &MicBassStep_table[MicBassStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicBassStep_up_stop_msg_mic_bass_up_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(++MicBassStep >= 16) MicBassStep = 16-1;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 6, &MicBassStep_table[MicBassStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_dw_delay(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 10, 19, 29, 39, 48, 58, 68, 77, 87, 97, 106, 116, 126, 135, 145, 155, 165, 174, 184, 194, 203, 213, 223, 232, 242, 252, 261, 271, 281, 290, 300, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 2, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_dw_attenuation(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_dw_wet(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_dw_room(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 3, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_up_delay(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 10, 19, 29, 39, 48, 58, 68, 77, 87, 97, 106, 116, 126, 135, 145, 155, 165, 174, 184, 194, 203, 213, 223, 232, 242, 252, 261, 271, 281, 290, 300, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 2, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_up_attenuation(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_echo0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_echo0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_up_wet(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 1, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_up_room(void)
{
	//param set type is STEPS
	static const int16_t ReverbStep_table[32] = {0, 2, 4, 6, 8, 10, 12, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 50, 52, 54, 56, 58, 60, 62, 64, };
	if(++ReverbStep >= 32) ReverbStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_reverb0_ADDR, 3, &ReverbStep_table[ReverbStep]);
	return (uint8_t)KARAOKE_reverb0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicTrebStep_down_stop_msg_mic_treb_dw_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(--MicTrebStep < 0) MicTrebStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 11, &MicTrebStep_table[MicTrebStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq7_MicTrebStep_up_stop_msg_mic_treb_up_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(++MicTrebStep >= 16) MicTrebStep = 16-1;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq7_ADDR, 11, &MicTrebStep_table[MicTrebStep]);
	return (uint8_t)KARAOKE_eq7_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicBassStep_down_stop_msg_music_bass_dw_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(--MusicBassStep < 0) MusicBassStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 6, &MusicBassStep_table[MusicBassStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicBassStep_up_stop_msg_music_bass_up_filter1_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicBassStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(++MusicBassStep >= 16) MusicBassStep = 16-1;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 6, &MusicBassStep_table[MusicBassStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicTrebStep_down_stop_msg_music_treb_dw_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(--MusicTrebStep < 0) MusicTrebStep = 0;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 11, &MusicTrebStep_table[MusicTrebStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
static uint8_t Karaoke_eq0_MusicTrebStep_up_stop_msg_music_treb_up_filter2_gain(void)
{
	//param set type is STEPS
	static const int16_t MusicTrebStep_table[16] = {-700, -600, -500, -400, -300, -200, -100, 0, 100, 200, 300, 400, 500, 600, 700, 800, };
	if(++MusicTrebStep >= 16) MusicTrebStep = 16-1;
	roboeffect_set_effect_parameter(AudioEffect.context_memory, KARAOKE_eq0_ADDR, 11, &MusicTrebStep_table[MusicTrebStep]);
	return (uint8_t)KARAOKE_eq0_ADDR;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#ifdef CFG_FLOWCHART_KARAOKE_ENABLE
uint8_t msg_process_Karaoke(int msg)
{
	uint8_t flush_address = 0;
	switch (msg) {
		case MSG_EFFECT_SYNC:
			flush_address = Karaoke_eq7_MicBassStep_none_msg_effect_sync_filter1_gain();
			flush_address = Karaoke_eq7_MicTrebStep_none_msg_effect_sync_filter2_gain();
			flush_address = Karaoke_eq0_MusicBassStep_none_msg_effect_sync_filter1_gain();
			flush_address = Karaoke_eq0_MusicTrebStep_none_msg_effect_sync_filter2_gain();
			flush_address = Karaoke_echo0_ReverbStep_none_msg_effect_sync_delay();
			flush_address = Karaoke_echo0_ReverbStep_none_msg_effect_sync_attenuation();
			flush_address = Karaoke_reverb0_ReverbStep_none_msg_effect_sync_wet();
			flush_address = Karaoke_reverb0_ReverbStep_none_msg_effect_sync_room();
			flush_address = 2;
			break;
		case MSG_MIC_BASS_DW:
			flush_address = Karaoke_eq7_MicBassStep_down_stop_msg_mic_bass_dw_filter1_gain();
			break;
		case MSG_MIC_BASS_UP:
			flush_address = Karaoke_eq7_MicBassStep_up_stop_msg_mic_bass_up_filter1_gain();
			break;
		case MSG_MIC_EFFECT_DW:
			flush_address = Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_dw_delay();
			flush_address = Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_dw_attenuation();
			flush_address = Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_dw_wet();
			flush_address = Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_dw_room();
			flush_address = 2;
			break;
		case MSG_MIC_EFFECT_UP:
			flush_address = Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_up_delay();
			flush_address = Karaoke_echo0_ReverbStep_up_loop_msg_mic_effect_up_attenuation();
			flush_address = Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_up_wet();
			flush_address = Karaoke_reverb0_ReverbStep_up_loop_msg_mic_effect_up_room();
			flush_address = 2;
			break;
		case MSG_MIC_TREB_DW:
			flush_address = Karaoke_eq7_MicTrebStep_down_stop_msg_mic_treb_dw_filter2_gain();
			break;
		case MSG_MIC_TREB_UP:
			flush_address = Karaoke_eq7_MicTrebStep_up_stop_msg_mic_treb_up_filter2_gain();
			break;
		case MSG_MUSIC_BASS_DW:
			flush_address = Karaoke_eq0_MusicBassStep_down_stop_msg_music_bass_dw_filter1_gain();
			break;
		case MSG_MUSIC_BASS_UP:
			flush_address = Karaoke_eq0_MusicBassStep_up_stop_msg_music_bass_up_filter1_gain();
			break;
		case MSG_MUSIC_TREB_DW:
			flush_address = Karaoke_eq0_MusicTrebStep_down_stop_msg_music_treb_dw_filter2_gain();
			break;
		case MSG_MUSIC_TREB_UP:
			flush_address = Karaoke_eq0_MusicTrebStep_up_stop_msg_music_treb_up_filter2_gain();
			break;
		default:
			break;
	}
	return flush_address;
}
#endif /*CFG_FLOWCHART_KARAOKE_ENABLE*/

#endif /*CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC*/