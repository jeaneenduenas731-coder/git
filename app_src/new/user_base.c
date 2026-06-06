#include "debug.h"
#include "delay.h"
#include "main_task.h"
#include "ctrlvars.h"
#include <string.h>
#include "display.h"
#include "bt_play_mode.h"
#include "bt_manager.h"
#include "user_effect_parameter.h"
#include "remind_sound.h"
#include "user_effect_flow_Karaoke.h"
#include "breakpoint.h"
#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
#include "led_effect.h"
#endif

// 自定义参数保存读取
void UserVar_Breakpoint_Read(BP_SYS_INFO* pBpSysInfo)
{
//**********************************************//
#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	userVar.led_mode_bak = pBpSysInfo->led_mode;
	if(userVar.led_mode_bak < LED_MODE_CHANGE1 || userVar.led_mode_bak >= LED_MODE_CLEAR_LED)
	{
		userVar.led_mode_bak = LED_MODE_CHANGE1;
	}
	APP_DBG("led_mode_bak:%d,%d\n", userVar.led_mode_bak, pBpSysInfo->led_mode);
#endif
}

// 自定义参数保存写入
void UserVar_Breakpoint_Write(BP_SYS_INFO * pBpSysInfo)
{
#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	if((userVar.led_mode > LED_MODE_POWER_ON) && (userVar.led_mode < LED_MODE_CLEAR_LED))
	{
		pBpSysInfo->led_mode = userVar.led_mode;
	}
	else if((userVar.led_mode_bak > LED_MODE_POWER_ON) && (userVar.led_mode_bak < LED_MODE_CLEAR_LED))
	{
		pBpSysInfo->led_mode = userVar.led_mode_bak;
	}
	else
	{
		pBpSysInfo->led_mode = 1;
	}
#endif
}

void DisplayTaskSend(uint32_t msgId)
{
	#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext  msgSend;
	msgSend.msgId = msgId;
	MessageSend(GetMainMessageHandle(), &msgSend);
	#endif
}

uint16_t  GetAdcArvgeVal(uint32_t adc_ch)
{
	#define AVRGE_CAL_COUNT   5
    uint16_t AdcSumVal = 0;
    for (uint8_t i = 0; i < AVRGE_CAL_COUNT; i++)
    {
        AdcSumVal += ADC_SingleModeDataGet(adc_ch);
		DelayUs(10);
    }

    return (AdcSumVal/AVRGE_CAL_COUNT);
}

//获取对应通道信号强度 1音乐 2麦 3吉他 4DAC0
uint32_t GetAudioSdct(LED_TYPE channel)
{
	extern int16_t * AudioEffectGetAllParameter(AUDIOEFFECT_EFFECT_CONTROL effect);
	param_silence_detector *Param = NULL;
	switch (channel)
	{
	case MUSIC_VOL_TYPE:
		Param = AudioEffectGetAllParameter(MUSIC_SILENCE_DETECTOR_PARAM);
		if(Param)
		{
			return Param->pcm_amplitude;
		}
		return 0;
		
	case MIC_VOL_TYPE:
		Param = AudioEffectGetAllParameter(MIC_SILENCE_DETECTOR_PARAM);
		if(Param)
		{
			return Param->pcm_amplitude;
		}
		return 0;
		
	case GUITAR_VOL_TYPE:
		return 0;
		
	case MONITOR_VOL_TYPE:
		return 0;
		
	case RECORD_VOL_TYPE:
		return 0;
		
	default:
		return 0;
	}
}

//***********基础函数定义*******************************//
uint32_t GetLedSwitchTime(uint8_t flag)
{
	uint16_t level = GetAudioSdct(MUSIC_VOL_TYPE);

	if(!AudioMusicVolGet() || IsAudioPlayerMute()
		#ifdef CFG_CHECK_SIGNAL_EN
		|| !userVar.if_music_play
		#endif
		)
	{
		return 0;
	}

	if(flag == 0)
	{
		switch (level)
		{
			case 0 ... 50:
				return 0;
				
			case 51 ... 1024:
				return (((level - 51) * 4 / (1024 - 51)) + 1);
			
			case 1025 ... 3824:
				return (((level - 1025) * 4 / (3824 - 1025)) + 6);
			
			case 3825 ... 6480:
				return (((level - 3825) * 4 / (6480 - 3825)) + 11);
			
			case 6481 ... 14336:
				return (((level - 6481) * 4 / (14336 - 6481)) + 16);
			
			case 14337 ... 19968:
				return (((level - 14337) * 4 / (19968 - 14337)) + 21);
			
			default:
				return 26;
		}
	}
	else if(flag == 1)
	{
		switch (level)
		{
			case 0 ... 50:
				return 0;
				
			case 51 ... 1023:
				return (((level - 51) * 4 / (1023 - 51)) + 1);
			
			case 1024 ... 3095:
				return (((level - 1024) * 4 / (3095 - 1024)) + 6);
			
			case 3096 ... 5119:
				return (((level - 3096) * 4 / (5119 - 3096)) + 11);
			
			case 5120 ... 7157:
				return (((level - 5119) * 4 / (7157 - 5119)) + 16);
			
			case 7168 ... 9215:
				return (((level - 7168) * 4 / (9215 - 7168)) + 21);
			
			default:
				return 26;
		}
	}
	
	return 0;
}

uint32_t GetLedSwitchTime1(uint8_t flag)
{	
	static uint8_t count = 0;
	static uint32_t tmp_power[30] = {0};
	static uint32_t sum_power = 0;
	static uint32_t MeanLevel = 0;

	if(!AudioMusicVolGet() || IsAudioPlayerMute()
		#ifdef CFG_CHECK_SIGNAL_EN
		|| !userVar.if_music_play
		#endif
		)
	{
		memset(tmp_power, 0, sizeof(uint32_t) * 30);
		count = 0;
		sum_power = 0;
		MeanLevel = 0;
		return 0;
	}

	uint16_t level = GetAudioSdct(MUSIC_VOL_TYPE);
	sum_power += level;

	sum_power -= tmp_power[count];

	tmp_power[count] = level;
	count++;
	count = count%30;
	MeanLevel = sum_power/30;

	if(flag == 0)
	{
		if(level < 50)
		{
			return 0;
		}
		else if (level < MeanLevel*19/30)
		{
			return 1 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*23/30)
		{
			return 6 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*27/30)
		{
			return 11 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel)
		{
			return 16 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/27)
		{
			return 21 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/23)
		{
			return 26 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/19)
		{
			return 31 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else
		{
			return 36;
		}
	}
	else if(flag == 1)
	{
		if(level < 50)
		{
			return 0;
		}
		else if (level < MeanLevel*19/30)
		{
			return 1 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*23/30)
		{
			return 6 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*27/30)
		{
			return 11 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel)
		{
			return 16 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/27)
		{
			return 21 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/23)
		{
			return 26 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else if (level < MeanLevel*30/19)
		{
			return 31 + GetRandomNum((uint16_t)GetSysTick1MsCnt(), 4);
		}
		else
		{
			return 36;
		}
	}
	else if(flag == 2)
	{
		if(level < 50)
		{
			return 0;
		}
		else if (level < MeanLevel*26/30)
		{
			return 1;
		}
		else if (level < MeanLevel*28/30)
		{
			return 2;
		}
		else if (level < MeanLevel*29/30)
		{
			return 3;
		}
		else if (level < MeanLevel)
		{
			return 4;
		}
		else if (level < MeanLevel*30/29)
		{
			return 5;
		}
		else if (level < MeanLevel*30/28)
		{
			return 6;
		}
		else if (level < MeanLevel*30/26)
		{
			return 7;
		}
		else
		{
			return 8;
		}
	}

	return 0;
}

/**
 * @brief  获取LED切换时间等级（基于音频音量与前30次平均值的比例）
 * @param  flag: 预留参数（无实际作用）
 * @retval 0-10：0=静音/低音量，1-10=音量相对平均值的比例等级
 * @note   优化点：整数运算、修正均值计算、除零保护、简化逻辑、提升可读性
 */
uint32_t GetLedSwitchTime2(uint8_t flag)
{    
	// 重命名：更直观的变量名（原tmp_power→audio_samples，sum_power→sum_samples）
	static uint8_t sample_idx = 0;                  // 采样索引（0-29循环）
	static uint32_t audio_samples[30] = {0};        // 前30次音频采样缓存
	static uint32_t sum_samples = 0;                // 前30次采样总和
	static uint32_t mean_samples = 0;           // 前30次采样平均值

	if(!AudioMusicVolGet() || IsAudioPlayerMute()
		#ifdef CFG_CHECK_SIGNAL_EN
		|| !userVar.if_music_play
		#endif
		)
	{
        memset(audio_samples, 0, sizeof(audio_samples));
        sample_idx = 0;
        sum_samples = 0;
        mean_samples = 0;
		return 0;
	}
    uint16_t curr_level = GetAudioSdct(MUSIC_VOL_TYPE);
    
    // 3. 先更新采样缓存和总和，再计算最新平均值（修正核心逻辑）
    sum_samples -= audio_samples[sample_idx];       // 移除旧采样值
    audio_samples[sample_idx] = curr_level;         // 存入新采样值
    sum_samples += audio_samples[sample_idx];       // 累加新采样值
    sample_idx = (sample_idx + 1) % 30;             // 循环索引（0-29）

    // 4. 计算平均值（除零保护：均值为0时设为1，避免后续乘法异常）
    mean_samples = (sum_samples == 0) ? 1 : (sum_samples / 30);

    // 5. 低音量过滤：小于50返回0（保持原有逻辑）
    if(curr_level < 50)
    {
		//DBG("lv %d %d\n", mean_samples, curr_level);
        return 0;
    }
	
    // 6. 浮点数改整数运算（乘以100，用百分比判断，无精度丢失）
    uint32_t ratio = (uint32_t)curr_level * 100 / mean_samples; // 音量/均值 × 100（百分比）
    
    // 7. 简化等级判断（替代10个else if，逻辑等价，代码更简洁）
    uint32_t level = 0;

	if(flag == 0)
	{
		if(ratio < 50)      	level = 1;   // <50% → 1
		else if(ratio < 60) 	level = 2;   // 50%~60% → 2
		else if(ratio < 70) 	level = 3;   // 60%~70% → 3
		else if(ratio < 80) 	level = 4;   // 70%~80% → 4
		else if(ratio < 90) 	level = 5;   // 80%~90% → 5
		else if(ratio < 100)	level = 6;   // 90%~100% → 6
		else if(ratio < 110)	level = 7;   // 100%~110% → 7
		else if(ratio < 120)	level = 8;   // 110%~120% → 8
		else if(ratio < 130)	level = 9;   // 120%~130% → 9
		else                	level = 10;  // ≥130% → 10
	}
	else if(flag == 1)
	{
		if(ratio < 50)      	level = 1;   // <50% → 1
		else if(ratio < 70) 	level = 2;   // 70%~80% → 4
		else if(ratio < 90) 	level = 3;   // 80%~90% → 5
		else if(ratio < 100)	level = 4;   // 90%~100% → 6
		else if(ratio < 110)	level = 5;   // 100%~110% → 7
		else if(ratio < 120)	level = 6;   // 110%~120% → 8
		else if(ratio < 130)	level = 7;   // 120%~130% → 9
		else                	level = 8;  // ≥130% → 10
	}

    //DBG("lv %d %d (ratio:%d%%, level:%d)\n", mean_samples, curr_level, ratio, level);
    return level;
}

#ifndef CFG_USER_IIC_EN
void DigAmpSetMute(bool mute)
{
	return;
}
#endif


/*****************校验码计算函数*********************/
#ifndef CFG_UART_EN
uint16_t UserCheckSum(uint8_t *checkbuff, uint16_t buflen, uint8_t checktype)
{
	if(checktype == CHECK_TYPE_CRC16_MODBUS)
	{
		uint16_t crc = 0xFFFF;
		for (uint16_t i = 0; i < buflen; i++) 
		{
			crc ^= checkbuff[i];
			for (uint8_t j = 0; j < 8; j++) 
			{
				if (crc & 0x0001) 
					crc = (crc >> 1) ^ 0xA001;
				else 
					crc >>= 1;
			}
		}
		return crc;
	}
	else if(checktype == CHECK_TYPE_CRC8)
	{
		return 0;
	}
	else if (checktype == CHECK_TYPE_SUM8)
	{
		uint16_t sum = 0;
		while(buflen--)
		{
			sum += *(checkbuff + buflen);
		};
		return (sum & 0xFF);
	}
	else if (checktype == CHECK_TYPE_SUM16)
	{
		uint32_t sum = 0;
		while(buflen--)
		{
			sum += *(checkbuff + buflen);
		};
		return (sum & 0xFFFF);
	}
}
#endif

void BT_LED_Switch(void)
{
	if(GetSystemMode() == ModeBtAudioPlay)
	{
		userVar.BT_Flag = GetBtLinkState(0);
		if(userVar.BT_Flag)// || GetBit(userVar.FlushFlag, FLUSH_TIME_500MS))
		{
			#ifdef CFG_BT_LED
			BT_LED_ON;
			#endif

			#ifdef CFG_LED1_G
			LED1_G_OFF;
			#endif
			#ifdef CFG_LED1_R
			LED1_R_ON;
			#endif

			#ifdef CFG_LED2_G
			LED2_G_OFF;
			#endif
			#ifdef CFG_LED2_R
			LED2_R_ON;
			#endif
		}
		else
		{
			#ifdef CFG_BT_LED
			BT_LED_OFF;
			#endif

			#ifdef CFG_LED1_G
			LED1_G_OFF;
			#endif
			#ifdef CFG_LED1_R
			LED1_R_OFF;
			#endif

			#ifdef CFG_LED2_G
			LED2_G_OFF;
			#endif
			#ifdef CFG_LED2_R
			LED2_R_OFF;
			#endif
		}

	}
	else if(GetSystemMode() == ModeLineAudioPlay)
	{
		#ifdef CFG_LED1_G
		LED1_G_ON;
		#endif
		#ifdef CFG_LED1_R
		LED1_R_OFF;
		#endif

		#ifdef CFG_LED2_G
		LED2_G_ON;
		#endif
		#ifdef CFG_LED2_R
		LED2_R_OFF;
		#endif
		userVar.BT_Flag = 0;
	}
	else if(GetSystemMode() == ModeIdle)
	{
		#ifdef CFG_LED1_G
		LED1_G_OFF;
		#endif
		#ifdef CFG_LED1_R
		LED1_R_OFF;
		#endif

		#ifdef CFG_LED2_G
		LED2_G_OFF;
		#endif
		#ifdef CFG_LED2_R
		LED2_R_OFF;
		#endif
		userVar.BT_Flag = 0;
	}
	else
	{
		#ifdef CFG_BT_LED
		if(userVar.FlushFlag)
			BT_LED_ON;
		else
			BT_LED_OFF;
		#endif
		userVar.BT_Flag = 0;
	}
}


















