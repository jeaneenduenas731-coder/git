#include <nds32_utils_math.h>
#include <string.h>
#include <math.h>
#include "roboeffect_api.h"
#include "user_defined_effect_api.h"
#include "user_effect_parameter.h"
#include "nn_denoise_api.h"
#include "main_task.h"
#include "bt_config.h"
#include "breakpoint.h"
#include "auto_gen_msg_process.h"

extern AUDIOEFFECT_SOURCE_SINK_NUM * get_user_effect_source_sink(void);
extern uint8_t GetEffectControlIndex(AUDIOEFFECT_EFFECT_CONTROL type);

int16_t * AudioEffectGetAllParameter(AUDIOEFFECT_EFFECT_CONTROL effect)
{
	uint8_t addr;
	if(AudioEffect.context_memory == NULL)
		return NULL;
	addr = GetEffectControlIndex(effect);
	if(addr == 0)
		return NULL;

	return (int16_t *)roboeffect_get_effect_parameter(AudioEffect.context_memory, addr, 0xff);
}

void AudioEffect_GetAudioEffectValue(void)
{
#ifdef CFG_FUNC_MIC_KARAOKE_EN
	param_echo *Echoparam = (param_echo *)AudioEffectGetAllParameter(ECHO_PARAM);
	if(Echoparam)
	{
		APP_DBG("echo fc:0x%x\n", Echoparam->cutoff_frequency);
		APP_DBG("echo delay:0x%x\n", Echoparam->delay);
		APP_DBG("echo dry:0x%x\n", Echoparam->dry);
		APP_DBG("echo attenuation:0x%x\n", Echoparam->attenuation);
		APP_DBG("echo max_delay:0x%x\n", Echoparam->max_delay);
		APP_DBG("echo quality_mode:0x%x\n", Echoparam->high_quality_enable);
		APP_DBG("echo wet:0x%x\n", Echoparam->wet);
	}
#endif
}

uint8_t AudioEffect_effect_status_Get(uint8_t effect_addr)
{
	if(AudioEffect.context_memory == NULL)
		return 0;
	if(!AudioEffect_effectAddr_check(effect_addr))
		return 0;

	return roboeffect_get_effect_status(AudioEffect.context_memory, effect_addr);
}

void AudioEffect_effect_enable(uint8_t effect_addr, uint8_t enable)
{
	AudioEffect.effect_addr = effect_addr;
	AudioEffect.effect_enable = enable;

	MessageContext msgSend;
	msgSend.msgId = MSG_EFFECTREINIT;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

uint8_t AudioCoreSourceToRoboeffect(int8_t source)
{
	AUDIOEFFECT_SOURCE_SINK_NUM *param = get_user_effect_source_sink();
	uint8_t source_id = AUDIOCORE_SOURCE_SINK_ERROR;

	switch (source)
	{
		case MIC_SOURCE_NUM:
			source_id = param->mic_source;
			break;
		case APP_SOURCE_NUM:
			source_id = param->app_source;
			break;
		case REMIND_SOURCE_NUM:
			source_id = param->remind_source;
			break;
		case PLAYBACK_SOURCE_NUM:
			source_id = param->rec_source;
			break;
		case I2S_MIX_SOURCE_NUM:
			source_id = param->i2s_mix_source;
			break;
		case I2S_MIX2_SOURCE_NUM:
			source_id = param->i2s_mix2_source;
			break;
		case USB_SOURCE_NUM:
			source_id = param->usb_source;
			break;
		case USB_HOST_SOURCE_NUM:
			source_id = param->usb_host_source;
			break;
		case LINEIN_MIX_SOURCE_NUM:
			source_id = param->linein_mix_source;
			break;
		case I2S_TDM0_SOURCE_NUM:
			source_id = param->i2s_tdm0_source;
			break;
		case I2S_TDM1_SOURCE_NUM:
			source_id = param->i2s_tdm1_source;
			break;
		default:
			break;// handle error
	}

	if(AudioEffect.context_memory == NULL ||
	   (roboeffect_get_source_buffer(AudioEffect.context_memory, source_id) == NULL)) //source not exist or CH_NONE
	{
		return AUDIOCORE_SOURCE_SINK_ERROR;
	}

	return source_id;
}

uint8_t AudioCoreSinkToRoboeffect(int8_t sink)
{
	AUDIOEFFECT_SOURCE_SINK_NUM *param = get_user_effect_source_sink();
	uint8_t sink_id = AUDIOCORE_SOURCE_SINK_ERROR;

	switch (sink)
	{
#ifdef CFG_RES_AUDIO_DAC0_EN
		case AUDIO_DAC0_SINK_NUM:
			sink_id = param->dac0_sink;
			break;
#endif			
#if	(defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)) || defined(CFG_APP_USB_AUDIO_MODE_EN)
		case AUDIO_APP_SINK_NUM:
			sink_id = param->app_sink;
			break;
#endif

#ifdef CFG_FUNC_RECORDER_EN
		case AUDIO_RECORDER_SINK_NUM:
			sink_id = param->rec_sink;
			break;
#endif
#if defined(CFG_RES_AUDIO_I2SOUT_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
		case AUDIO_STEREO_SINK_NUM:
			sink_id = param->stereo_sink;
			break;
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
		case AUDIO_I2S_MIX_OUT_SINK_NUM:
			sink_id = param->i2s_mix_sink;
			break;
#endif
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
		case AUDIO_SPDIF_SINK_NUM:
			sink_id = param->spdif_sink;
			break;
#endif
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
		case AUDIO_USB_HOST_SINK_NUM:
			sink_id = param->usb_host_mix_sink;
			break;
#endif
		default:
			// handle error
			break;
	}

	if(AudioEffect.context_memory == NULL ||
	   roboeffect_get_sink_buffer(AudioEffect.context_memory, sink_id) == NULL) //source not exist or CH_NONE
	{
		return AUDIOCORE_SOURCE_SINK_ERROR;
	}

	return sink_id;
}

void AudioEffect_update_local_params(uint8_t addr, uint8_t param_index, int16_t *param_input, uint8_t param_len)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect.user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;

//	DBG("input: 0x%x\n", *(uint16_t *)param_input);
	while(data_len)
	{
		if(*params == addr)
		{
			params += (param_index * 2 + 3);
//			DBG("before: 0x%x\n", *(uint16_t *)params);
//			*(uint16_t *)params = *(uint16_t *)param_input;
			memcpy((uint16_t *)params, (uint16_t *)param_input, param_len);
//			DBG("addr:0x%x,index:%d,local:0x%x, len:%d\n", addr, param_index, *(uint16_t *)params, param_len);
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

void AudioEffect_update_local_effect_status(uint8_t addr, uint8_t effect_enable)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect.user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;
	while(data_len)
	{
		if(*params == addr)
		{
			params += 2;
			*params = effect_enable;
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

void AudioEffect_update_local_block_params(uint8_t addr)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect.user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;
	const uint8_t *p = (uint8_t *)roboeffect_get_effect_parameter(AudioEffect.context_memory, addr, 0xFF);
//	uint8_t i = 0;

	while(data_len)
	{
		if(*params == addr)
		{
			params++;
			len = *params;
			params+=2;
//			for(; i < len; i ++)
//			{
//				DBG("0x%x, 0x%x\n", *(params + i), *(p + i));
//			}
			memcpy(params, p, len - 1);
//			DBG("addr:0x%x,param:0x%x, len:0x%x\n", addr, *(uint16_t *)params, len);
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

uint16_t get_user_effect_parameters_len(uint8_t *user_effect_parameters)
{
	uint8_t b1 = user_effect_parameters[0];
	uint8_t b2 = user_effect_parameters[1];
    return ((b2 << 8) | b1) + 2;
}

bool AudioEffect_effectAddr_check(uint8_t addr)
{
	if(addr < 0x81 || addr > (AudioEffect.cur_effect_para->user_effect_list->count + 0x80))
		return FALSE;
//	if(!roboeffect_get_effect_status(AudioEffect.context_memory, addr))
//			return FALSE;
	return TRUE;
}

#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
uint32_t get_EffectParamFlash_WriteAddr(void)
{
	uint32_t offset = 1024 * CFG_EFFECT_PARAM_IN_FLASH_SIZE;
	uint32_t flashCnt = 0;
	for(uint8_t i = 0; i < ((EFFECT_MODE_COUNT - 1) * 4); i++)
	{
		if(i % 4 < 2)
		{
			SpiFlashRead(get_effect_data_addr() + i * 4, (uint8_t*)&flashCnt, 4, 1);
			//DBG("flashCnt = %d\n", flashCnt);
			offset = (flashCnt < offset) ? flashCnt : offset;
		}
	}
	return offset;
}

#ifdef CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
#define CFG_FLASH_SECTOR_SIZE		(4096)//4KB
bool AudioEffect_FlashWrite(uint32_t Addr, uint8_t *Buffer, uint32_t Length)
{
	static uint8_t EffectParamFlahBuf[CFG_FLASH_SECTOR_SIZE] ={0};
    uint32_t sectorIndex = (Addr - get_effect_data_addr()) / CFG_FLASH_SECTOR_SIZE;
    uint32_t spaceLen = get_effect_data_addr() + (sectorIndex + 1) * CFG_FLASH_SECTOR_SIZE - Addr;
    uint32_t writeLen = spaceLen >= Length ? Length : spaceLen;
    uint32_t writeOffset = Addr - get_effect_data_addr() - sectorIndex * CFG_FLASH_SECTOR_SIZE;
    if (SpiFlashRead(get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE, EffectParamFlahBuf, CFG_FLASH_SECTOR_SIZE, 1) == FLASH_NONE_ERR)
    {
        SpiFlashErase(SECTOR_ERASE, (get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE) / 4096, 1);
    }
    memcpy(&EffectParamFlahBuf[writeOffset], Buffer, writeLen);
    if (SpiFlashWrite(get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE, EffectParamFlahBuf, CFG_FLASH_SECTOR_SIZE, 1) != FLASH_NONE_ERR)
    {
        APP_DBG("AudioEffect_FlashWrite ERROR!\n");
    	return FALSE;
    }
    if (writeLen < Length)
	{
		AudioEffect_FlashWrite(Addr + writeLen, &Buffer[writeLen], Length - writeLen);
	}
    return TRUE;
}
#endif

void EffectParamFlashUpdata(void)
{
//	SPI_FLASH_ERR_CODE ret = 0;
	int32_t FlashAddr = get_effect_data_addr();
	uint32_t FlashWriteOffset = 0;
	uint32_t effectHwCfgOffset = 0;
	uint32_t effectParamOffset = 0;
	uint32_t HwCfgCRC = 0;
	uint32_t ParamCRC = 0;

	if (((SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16), (uint8_t*)&effectHwCfgOffset, 4 ,1)  == FLASH_NONE_ERR)
				&& (effectHwCfgOffset != 0xffffffff))
		&& ((SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16) + 4, (uint8_t*)&effectParamOffset, 4 ,1)  == FLASH_NONE_ERR)
				&& (effectParamOffset != 0xffffffff)))
	{
		DBG("EffectData FlashAdd = %lx\n", FlashAddr);
		DBG("effectHwCfgOffset = %ld\n", effectHwCfgOffset);
		DBG("effectParamOffset = %ld\n", effectParamOffset);
	}
	else
	{
		FlashWriteOffset = get_EffectParamFlash_WriteAddr();
		if (FlashWriteOffset < (((EFFECT_MODE_COUNT - 1) * 16) + sizeof(gCtrlVars.HwCt)
				+ get_user_effect_parameters_len(AudioEffect.user_effect_parameters)))
		{
			APP_DBG("Flash space is not enough!!!\n");
			return;
		}

		effectHwCfgOffset = FlashWriteOffset - sizeof(gCtrlVars.HwCt);
		effectParamOffset = effectHwCfgOffset - get_user_effect_parameters_len(AudioEffect.user_effect_parameters);

		DBG("EffectData FlashAdd = %lx\n", FlashAddr);
		DBG("FlashWriteOffset = %ld\n", FlashWriteOffset);
		DBG("effectHwCfgOffset = %ld\n", effectHwCfgOffset);
		DBG("effectParamOffset = %ld\n", effectParamOffset);
	}
	HwCfgCRC = AudioEffect_GetUserHWCfgCRCLen((uint8_t*)&gCtrlVars.HwCt);
	ParamCRC = AudioEffect_GetUserEffectParamCRCLen(AudioEffect.user_effect_parameters);
//	DBG("HwCfgCRC = %ld\n", HwCfgCRC);
//	DBG("ParamCRC = %ld\n", ParamCRC);

	//write data
	if (AudioEffect_FlashWrite(FlashAddr + effectHwCfgOffset, (uint8_t*)&gCtrlVars.HwCt, sizeof(gCtrlVars.HwCt))
		&& AudioEffect_FlashWrite(FlashAddr + effectParamOffset, (uint8_t*)AudioEffect.user_effect_parameters,
				get_user_effect_parameters_len(AudioEffect.user_effect_parameters)))
	{
		AudioEffect_FlashWrite(FlashAddr + (mainAppCt.EffectMode - 1) * 16, (uint8_t*)&effectHwCfgOffset, 4);
		AudioEffect_FlashWrite(FlashAddr + (mainAppCt.EffectMode - 1) * 16 + 4, (uint8_t*)&effectParamOffset, 4);
		AudioEffect_FlashWrite(FlashAddr + (mainAppCt.EffectMode - 1) * 16 + 8, (uint8_t*)&HwCfgCRC, 4);
		AudioEffect_FlashWrite(FlashAddr + (mainAppCt.EffectMode - 1) * 16 + 12, (uint8_t*)&ParamCRC, 4);
		APP_DBG("EffectParamFlashUpdata ok!\n");
	}
	else
	{
		APP_DBG("EffectParamFlashUpdata Error!\n");
	}
}

bool AudioEffect_GetFlashHwCfg(uint8_t effectMode, HardwareConfigContext *hw_ct)
{
	uint32_t offset = 0;
	uint32_t HwCfgCRC = 0;
	if ((SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16), (uint8_t*)&offset, 4 ,1) == FLASH_NONE_ERR)
			&& (SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16) + 8, (uint8_t*)&HwCfgCRC, 4 ,1) == FLASH_NONE_ERR))
	{
		if ((offset != 0xffffffff) && (SpiFlashRead(get_effect_data_addr() + offset, (uint8_t*)hw_ct, sizeof(gCtrlVars.HwCt) ,1) == FLASH_NONE_ERR)
				&& (HwCfgCRC == AudioEffect_GetUserHWCfgCRCLen((uint8_t*)hw_ct))
				&& AudioEffect_HwCfg_check(hw_ct))
		{
			return TRUE;
		}
	}
	DBG("flash read HwCt err\n");
	return FALSE;
}

bool AudioEffect_GetFlashEffectParam(uint8_t effectMode,  uint8_t *effect_param)
{
	uint32_t effectHwCfgOffset = 0;
	uint32_t effectParamOffset = 0;
	uint32_t ParamCRC = 0;
	if ((SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16), (uint8_t*)&effectHwCfgOffset, 4 ,1) == FLASH_NONE_ERR)
			&& (SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16) + 4, (uint8_t*)&effectParamOffset, 4 ,1) == FLASH_NONE_ERR)
			&& (SpiFlashRead(get_effect_data_addr() + ((mainAppCt.EffectMode - 1) * 16) + 12, (uint8_t*)&ParamCRC, 4 ,1) == FLASH_NONE_ERR)
			&& (effectHwCfgOffset - effectParamOffset == get_user_effect_parameters_len(effect_param)))
	{
		if ((SpiFlashRead(get_effect_data_addr() + effectParamOffset, (uint8_t*)effect_param, get_user_effect_parameters_len(effect_param) ,1) == FLASH_NONE_ERR)
				&& (ParamCRC == AudioEffect_GetUserEffectParamCRCLen(effect_param))
				&& AudioEffect_effectParam_check(effectMode, effect_param))
		{
				return TRUE;
		}
	}
	DBG("flash read EffectParam err\n");
	return FALSE;
}

bool AudioEffect_CheckFlashEffectParam(void)
{
	uint32_t flashParam[EFFECT_MODE_COUNT * 4];
	AUDIOEFFECT_EFFECT_PARA *para;
	uint8_t i, index;

	if ((SpiFlashRead(get_effect_data_addr(), (uint8_t*)flashParam, EFFECT_MODE_COUNT * 16 ,1) == FLASH_NONE_ERR))
	{
		for(i = 0; i < EFFECT_MODE_COUNT; i++)
		{
//			DBG("### %d - Hwcfg:%d, param:%d\n", i, flashParam[i*2], flashParam[i*2 + 1]);
			para = get_user_effect_parameters(i);
			if((flashParam[i*4] != 0xffffffff) && (flashParam[i*4 + 1] != 0xffffffff)
				&& (flashParam[i*4 + 2] != 0xffffffff) && (flashParam[i*4 + 3] != 0xffffffff)
				&& (flashParam[i*4] - flashParam[i*4 + 1] != get_user_effect_parameters_len(para->user_effect_parameters)))
			{
//				DBG("flash read effect_mode(%d)EffectParam err %d\n", i, get_user_effect_parameters_len(para->user_effect_parameters));
				DBG("EffectParam in flash changed, erase all\n");
				for(index = 0; index < ((1024 * CFG_EFFECT_PARAM_IN_FLASH_SIZE) / CFG_FLASH_SECTOR_SIZE); index++)
				{
					SpiFlashErase(SECTOR_ERASE, (get_effect_data_addr() + index * CFG_FLASH_SECTOR_SIZE) /4096 , 1);
				}
				return FALSE;
			}
		}
	}
	return TRUE;
}
#endif

//total data length  	---- 2 Bytes
//Effect Version		---- 3 Bytes
//Roboeffect Version  	---- 3 Bytes
//ACPWorkbench V3.8.15以后版本导出的参数增加了3字节的Roboeffect Version + 3rd part data
//使用的时候注意参数的版本，修改对应的偏移
EffectValidParamUnit AudioEffect_GetUserEffectValidParam(uint8_t *effect_param)
{
	EffectValidParamUnit unit;
	uint16_t third_data_len = *(uint16_t *)(effect_param + 8);

	unit.params_first_address = effect_param + 8 + third_data_len + 2;
	unit.data_len = *(uint16_t *)effect_param - 8 - third_data_len;

	return unit;
}

uint32_t AudioEffect_GetUserEffectParamCRCLen(uint8_t *effect_param)
{
	EffectValidParamUnit unit;
	uint16_t third_data_len = *(uint16_t *)(effect_param + 8);
	uint32_t crcLen = 0;

	unit.params_first_address = effect_param + 8 + third_data_len + 2;
	uint8_t *params = unit.params_first_address;
	unit.data_len = *(uint16_t *)effect_param - 8 - third_data_len;

	for(uint32_t i = 0; i < unit.data_len; i++)
	{
		crcLen += params[i];
	}

	return crcLen;
}

uint32_t AudioEffect_GetUserHWCfgCRCLen(uint8_t *effect_param)
{
	uint32_t crcLen = 0;
	uint32_t paramsLen = sizeof(gCtrlVars.HwCt);

	for(uint32_t i = 0; i < paramsLen; i++)
	{
		crcLen += effect_param[i];
	}

	return crcLen;
}

bool AudioEffect_effectParam_check(uint8_t effectMode,  uint8_t *effect_param)
{
	AUDIOEFFECT_EFFECT_PARA *para = get_user_effect_parameters(effectMode);
	uint8_t *context = NULL;
	int32_t memory_size = roboeffect_estimate_memory_size(para->user_effect_steps, para->user_effect_list, effect_param);
	if(memory_size < 0)
	{
		DBG("AudioEffect_effectParam_check memory_size failed. %ld \n", memory_size);
		return FALSE;
	}
	else
	{
		context = osPortMallocFromEnd(memory_size);
		if(context == NULL)
		{
			DBG("AudioEffect_effectParam_check failed, memory not enough!\n");
			return FALSE;
		}
	}
	ROBOEFFECT_ERROR_CODE ret = roboeffect_init(context, memory_size, para->user_effect_steps, para->user_effect_list, effect_param);
	if(ROBOEFFECT_ERROR_OK !=  ret)
	{
		DBG("AudioEffect_effectParam_check failed. %d \n", ret);
		osPortFree(context);
		return FALSE;
	}

	osPortFree(context);
	return TRUE;
}

bool AudioEffect_HwCfg_check(HardwareConfigContext *hw_ct)
{
    uint32_t total_length = 0;
    const uint8_t *ptr = (uint8_t *)hw_ct;
    const uint8_t *end = (uint8_t *)hw_ct + sizeof(HardwareConfigContext);

    while (ptr < end)
    {

        uint8_t control_code = *ptr++;
        if (control_code < 0x03 || control_code > 0x0D)
        {
            return FALSE;
        }
        uint8_t block_length = *ptr++;
        uint32_t block_total = 2 + block_length;
        total_length += block_total;
        ptr += block_length;
    }

    if (total_length == sizeof(HardwareConfigContext))
    {
        return TRUE;;
    }
    return FALSE;
}
