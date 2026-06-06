/**
 **************************************************************************************
 * @file    split_gain.c
 * @brief   interface for user defined effect algorithm
 *
 * @author  Castle Cai
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nds32_utils_math.h>
#include "roboeffect_api.h"
#include "user_defined_effect_api.h"
#include "nn_denoise_api.h"

/**
 * ai denoise begin
*/
bool roboeffect_ai_denoise_init_if(void *node)
{
	uint8_t *context_ptr, *scratch;
	roboeffect_user_defined_effect_info info;
	ai_denoise_struct *ai_info;
	void *run_ram;

	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct
	*/
	ai_info = info.context_memory;

	/**
	 * clear user defined algorithm context
	*/
	memset(ai_info, 0x00, sizeof(ai_denoise_struct) + nn_denoise_query_mem_size(info.sample_rate, AI_FRAME_TIME, NN_MODE_DENOISE_M3));

	/**
	 *get user defined algorithm context
	*/
	context_ptr = ((uint8_t*)info.context_memory) + sizeof(ai_denoise_struct);

	/**
	 * get scratch memory
	*/
	scratch = info.scratch_memory;


	run_ram = roboeffect_user_defined_malloc(node, nn_denoise_query_mem_ram_size());
	/**
	 * initialize 
	*/
	ai_info->p_nn_denoise = nn_denoise_init(context_ptr, scratch, info.sample_rate, AI_FRAME_TIME, NN_MODE_DENOISE_M3);
	nn_denoise_set_ram(ai_info->p_nn_denoise, run_ram);

	/**
	 * set default parameters
	*/
	nn_set_max_suppress(ai_info->p_nn_denoise, (float)info.parameters[0]);

	ai_info->block_size = info.sample_rate * AI_FRAME_TIME / 1000;

	return TRUE;
}

bool roboeffect_ai_denoise_config_if(void *node, int16_t *new_param, uint8_t param_num, uint8_t len)
{
	// int ret;  // unused variable
	uint8_t method_flag = 0;
	roboeffect_user_defined_effect_info info;
	ai_denoise_struct *ai_info;

	/**
	 * check parameters and update to effect instance
	*/
	if(ROBOEFFECT_ERROR_OK > roboeffect_user_defined_params_check(node, new_param, param_num, len, &method_flag))
	{
		return FALSE;
	}

	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct
	*/
	ai_info = info.context_memory;

	if((method_flag & METHOD_CFG_1) && info.is_active)
	{
		// printf("call METHOD_CFG_1 func: %d\n", info.parameters[0]);
		nn_set_max_suppress(ai_info->p_nn_denoise, (float)info.parameters[0]);
	}

	return TRUE;
}


bool roboeffect_ai_denoise_apply_if(void *node, int16_t *pcm_in1, int16_t *pcm_in2, int16_t *pcm_out, int32_t n)
{
	// int i;  // unused variable
	roboeffect_user_defined_effect_info info;
	ai_denoise_struct *ai_info;
	// int32_t ret, block, clip_size;  // unused variables
	int32_t block, clip_size;
	
	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct
	*/
	ai_info = info.context_memory;
	
	/**
	 * apply data
	*/
	clip_size = ai_info->block_size;
	block = n / clip_size;
	if(block > 0)
	{
		while(block--)
		{
			nds32_convert_q15_f32(pcm_in1, ai_info->float_buffer_in, clip_size);
			nn_denoise_process(ai_info->p_nn_denoise, ai_info->float_buffer_in, ai_info->float_buffer_out);
			nds32_convert_f32_q15(ai_info->float_buffer_out, pcm_out, clip_size);

			pcm_in1 += clip_size;
			pcm_out += clip_size;
			// printf("%d, ", block);
		}
	}

	return TRUE;
}


int32_t roboeffect_ai_denoise_memory_size_if(roboeffect_memory_size_query *query, roboeffect_memory_size_response *response)
{
	if(query->ch_num != CH_MONO)
		return FALSE;

	if(query->data_width != BITS_16)
		return FALSE;

	if(query->sample_rate != 16000 && query->sample_rate != 32000)
		return FALSE;

	response->context_memory_size = ALIGN4(sizeof(ai_denoise_struct) + nn_denoise_query_mem_size(query->sample_rate, AI_FRAME_TIME, NN_MODE_DENOISE_M3));
	response->additional_memory_size = ALIGN4(nn_denoise_query_mem_ram_size());
	response->scratch_memory_size = ALIGN4(nn_denoise_query_scratch_size(query->sample_rate, AI_FRAME_TIME, NN_MODE_DENOISE_M3));

	// printf("ai size: %d, %d, %d\n", response->context_memory_size, response->additional_memory_size, response->scratch_memory_size);

	return TRUE;
}
