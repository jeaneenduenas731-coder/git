/**
 **************************************************************************************
 * @file    split_gain.h
 * @brief   interface for user defined effect algorithm
 *
 * @author  Castle Cai
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include "roboeffect_api.h"
#include "roboeffect_config.h"


#define AI_MEMORY_SIZE 70976
#define AI_SCRATCH_SIZE 10296
#define AI_RAM_SIZE 54988
#define AI_FRAME_TIME 10 //@ms



typedef struct _ai_denoise_struct
{
	void *p_nn_denoise;
	uint16_t block_size;//sample
	float float_buffer_in[320];
	float float_buffer_out[320];
} ai_denoise_struct;


typedef struct _user_gain_struct
{
	int16_t data_a;
	int16_t data_b;
} user_gain_struct;
