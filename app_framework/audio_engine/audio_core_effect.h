#ifndef __AUDIO_CORE_EFFECT_H__
#define __AUDIO_CORE_EFFECT_H__

#include "roboeffect_api.h"

/**
 * @brief Effect parameters for roboeffect engine
 */
typedef struct _AUDIOEFFECT_EFFECT_PARA
{
	roboeffect_effect_list_info 	*user_effect_list;
	roboeffect_effect_steps_table 	*user_effect_steps;
	uint8_t 						*user_effects_script;
	uint8_t 						*user_effect_name;

	uint8_t 						*user_effect_parameters;
	uint8_t 						*user_module_parameters;
	uint32_t 						(*get_user_effects_script_len)(void);
}AUDIOEFFECT_EFFECT_PARA;

typedef struct _AudioeffectContext
{
	uint8_t *context_memory;
	AUDIOEFFECT_EFFECT_PARA *cur_effect_para;
	uint8_t *user_effect_parameters;
	uint16_t user_effects_script_len;
	int32_t audioeffect_memory_size;
	uint32_t audioeffect_frame_size;
	uint8_t effect_mode_expected;
	uint8_t effect_count;
	uint8_t effect_addr;
	uint8_t effect_enable;
#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
	bool	EffectFlashUseFlag;
#endif
	//ROBOEFFECT_ERROR_CODE roboeffect_ret;
}AudioEffectContext;

#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
	#define	EFFECT_FLASH_MODE_NAME	"Flash_"
#endif

extern 	AudioEffectContext 	AudioEffect;

#endif
