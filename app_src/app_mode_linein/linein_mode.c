/**
 **************************************************************************************
 * @file    linein_mode.c
 * @brief   
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-12-26 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "main_task.h"
#include "dma.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "remind_sound.h"
#include "audio_effect.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "reset.h"

#if  defined(CFG_APP_LINEIN_MODE_EN)||defined(CFG_FUNC_LINEIN_MIX_MODE)

#define LINEIN_PLAY_TASK_STACK_SIZE		512//1024
#define LINEIN_PLAY_TASK_PRIO			3
#define LINEIN_NUM_MESSAGE_QUEUE		10

#ifdef CFG_APP_LINEIN_MODE_EN
#define LINEIN_SOURCE_NUM				APP_SOURCE_NUM
#else
#define LINEIN_SOURCE_NUM				LINEIN_MIX_SOURCE_NUM
#endif

typedef struct _LineInPlayContext
{
	//xTaskHandle 		taskHandle;
	//MessageHandle		msgHandle;

	uint32_t			*ADCFIFO;			//ADC的DMA循环fifo
	uint32_t			ADCFIFO_len;		//ADC的DMA循环fifo len
//	AudioCoreContext 	*AudioCoreLineIn;

	//play
	uint32_t 			SampleRate; //带提示音时，如果不重采样，要避免采样率配置冲突
}LineInPlayContext;


static const uint8_t sDmaChannelMap[6] = {
	//DMA默认配置不要删除，DMA数量不足6个的时候作为填充用
	DMA_CFG_TABLE_DEFAULT_INIT

#ifdef CFG_RES_AUDIO_DAC0_EN
	PERIPHERAL_ID_AUDIO_DAC0_TX,
#endif

#if CFG_RES_MIC_SELECT
	PERIPHERAL_ID_AUDIO_ADC1_RX,
#endif

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	SPDIF_OUT_DMA_ID,
#endif

#if (I2S_ALL_DMA_CH_CFG & I2S0_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S0_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_RX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_RX,
#endif

#ifdef CFG_COMMUNICATION_BY_UART
	CFG_FUNC_COMMUNICATION_TX_DMA_PORT,
	CFG_FUNC_COMMUNICATION_RX_DMA_PORT,
#endif

#ifdef CFG_DUMP_DEBUG_EN
	CFG_DUMP_UART_TX_DMA_CHANNEL,
#endif
#ifdef PWM5_LED
	PERIPHERAL_ID_TIMER5,
#endif
#ifdef PWM6_LED
	PERIPHERAL_ID_TIMER6,
#endif
#ifdef PWM7_LED
	PERIPHERAL_ID_TIMER7,
#endif
#ifdef PWM8_LED
	PERIPHERAL_ID_TIMER8,
#endif

	PERIPHERAL_ID_AUDIO_ADC0_RX,
	PERIPHERAL_ID_SDIO_RX,
};

static  LineInPlayContext*		sLineInPlayCt;

extern uint16_t SampleRateIndexGet(uint32_t SampleRate);

void LineInPlayResFree(void)
{
	AudioCoreSourceDisable(LINEIN_SOURCE_NUM);

	AudioAnaChannelSet(ANA_INPUT_CH_NONE);

	AudioADC_Disable(ADC0_MODULE);
	AudioADC_DeInit(ADC0_MODULE);

	if(sLineInPlayCt->ADCFIFO != NULL)
	{
		APP_DBG("ADCFIFO\n");
		osPortFree(sLineInPlayCt->ADCFIFO);
		sLineInPlayCt->ADCFIFO = NULL;
	}
	AudioCoreSourceDeinit(LINEIN_SOURCE_NUM);
	
	osPortFree(sLineInPlayCt);
	sLineInPlayCt = NULL;
#if (LINEIN_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN2)
	//aux 2 B0/B1 恢复SW口
	GPIO_PortBModeSet(GPIOB0,1);
	GPIO_PortBModeSet(GPIOB1,1);
#endif
	APP_DBG("Line:Kill Ct\n");
}

bool LineInPlayResMalloc(uint16_t SampleLen)
{
	sLineInPlayCt = (LineInPlayContext*)osPortMalloc(sizeof(LineInPlayContext));
	if(sLineInPlayCt == NULL)
	{
		return FALSE;
	}
	memset(sLineInPlayCt, 0, sizeof(LineInPlayContext));

	sLineInPlayCt->ADCFIFO_len = SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2;
	//LineIn5  digital (DMA)
	sLineInPlayCt->ADCFIFO = (uint32_t*)osPortMalloc(sLineInPlayCt->ADCFIFO_len);
	if(sLineInPlayCt->ADCFIFO == NULL)
	{
		return FALSE;
	}
	memset(sLineInPlayCt->ADCFIFO, 0, sLineInPlayCt->ADCFIFO_len);

	return TRUE;
}

void LineinADCDigitalInit(void)
{
	AUDIO_BitWidth BitWidth = ADC_WIDTH_16BITS;

#ifdef	CFG_AUDIO_WIDTH_24BIT
	if(AudioCoreSourceBitWidthGet(LINEIN_SOURCE_NUM) == PCM_DATA_24BIT_WIDTH)
		BitWidth = ADC_WIDTH_24BITS;
#endif

	AudioADC_DigitalInit(ADC0_MODULE, sLineInPlayCt->SampleRate,BitWidth, (void*)sLineInPlayCt->ADCFIFO, sLineInPlayCt->ADCFIFO_len);

#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
    Clock_AudioMclkSel(AUDIO_ADC0, gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source > 2 ? (gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source - 1):gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source);
#else
	gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC0);
#endif
}

void LineInPlayResInit(void)
{
	AudioCoreIO	AudioIOSet;
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

	sLineInPlayCt->SampleRate = AudioCoreMixSampleRateGet(DefaultNet);
	AudioIOSet.Adapt = STD;
	AudioIOSet.Sync = TRUE;
	AudioIOSet.Channels = 2;
	AudioIOSet.Net = DefaultNet;

	AudioIOSet.DataIOFunc = AudioADC0_DataGet;
	AudioIOSet.LenGetFunc = AudioADC0_DataLenGet;

#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//需要数据进行位宽扩展
#endif
	AudioIOSet.SampleRate = sLineInPlayCt->SampleRate;
	//AudioIOSet.
	if(!AudioCoreSourceInit(&AudioIOSet, LINEIN_SOURCE_NUM))
	{
		DBG("Line source error!\n");
		return;
	}

	AudioCoreSourceEnable(LINEIN_SOURCE_NUM);

}
void LineInPlayHardwareInit(void)
{
	AudioAnaChannelSet(LINEIN_INPUT_CHANNEL);

	LineinADCDigitalInit();

#if (LINEIN_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN1)
#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN1_LEFT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN1_RIGHT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#else
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN1_LEFT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN1_RIGHT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
#endif
#if (LINEIN_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN2)
	GPIO_PortBModeSet(GPIOB0,0);
	GPIO_PortBModeSet(GPIOB1,0);
	GPIO_RegBitsClear(GPIO_B_IE,GPIOB0);
	GPIO_RegBitsClear(GPIO_B_OE,GPIOB0);
	GPIO_RegBitsClear(GPIO_B_IE,GPIOB1);
	GPIO_RegBitsClear(GPIO_B_OE,GPIOB1);

#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN2_LEFT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN2_RIGHT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#else
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN2_LEFT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN2_RIGHT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
#endif
}

void LineInPlayRun(uint16_t msgId)
{
	switch(msgId)
	{
		case MSG_PLAY_PAUSE:
			HardWareMuteOrUnMute();
			break;
			
		default:
			CommonMsgProccess(msgId);
			break;
	}
}

bool LineInPlayInit(void)
{
	APP_DBG("LineIn Play Init\n");

	DMA_ChannelAllocTableSet((uint8_t *)sDmaChannelMap);//lineIn

	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	if(!LineInPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("LineInPlay Res Error!\n");
		return FALSE;
	}
	//Core Process
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioMusicProcess);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif

	LineInPlayResInit();

	LineInPlayHardwareInit();
	gCtrlVars.HwCt.ADC0DigitalCt.adc_sample_rate = SampleRateIndexGet(AudioADC_SampleRateGet(ADC0_MODULE));

#if 0
#if defined(CFG_FUNC_REMIND_SBC)
	DecoderServiceCreate(sLineInPlayCt->msgHandle, DECODER_BUF_SIZE_SBC, DECODER_FIFO_SIZE_FOR_SBC);//提示音格式决定解码器内存消耗
#elif defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceCreate(sLineInPlayCt->msgHandle, DECODER_BUF_SIZE_MP3, DECODER_FIFO_SIZE_FOR_MP3);
#endif
#endif

	AudioCodecGainUpdata();//update hardware config

	/*****************************************************************/
	//reset ADC0 to fix linein LR channel reverse
	AudioADC_Disable(ADC0_MODULE);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
	DMA_CircularFIFOClear(PERIPHERAL_ID_AUDIO_ADC0_RX);
	AudioADC_FuncReset(ADC0_MODULE);
	AudioADC_Enable(ADC0_MODULE);
	AudioADC_LREnable(ADC0_MODULE, 1, 1);
	DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC0_RX);
	/*****************************************************************/

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(RemindSoundServiceItemRequest(SOUND_REMIND_XIANLUMO, REMIND_ATTR_NEED_MUTE_APP_SOURCE) == FALSE)
	{
		if(IsAudioPlayerMute() == TRUE)
		{
			HardWareMuteOrUnMute();
		}
	}
#endif

#ifndef CFG_FUNC_REMIND_SOUND_EN
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
#endif
	AudioCoreSourceUnmute(LINEIN_SOURCE_NUM,TRUE,TRUE);
	return TRUE;
}


bool LineInPlayDeinit(void)
{
	APP_DBG("LineIn Play Deinit\n");
	if(sLineInPlayCt == NULL)
	{
		return TRUE;
	}

	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	
	PauseAuidoCore();
	
	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig((void*)AudioNoAppProcess);

	LineInPlayResFree();

	ModeCommonDeinit();//通路全部释放
	return TRUE;
}

#ifdef CFG_FUNC_LINEIN_MIX_MODE

bool LineInMixPlayInit(void)
{
	APP_DBG("LineIn Mix Play Init\n");
	if(!LineInPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("LineInPlay Res Error!\n");
		return FALSE;
	}

	LineInPlayResInit();

	LineInPlayHardwareInit();
	gCtrlVars.HwCt.ADC0DigitalCt.adc_sample_rate = SampleRateIndexGet(AudioADC_SampleRateGet(ADC0_MODULE));

//	AudioCodecGainUpdata();//update hardware config
	AudioCoreSourceUnmute(LINEIN_SOURCE_NUM,TRUE,TRUE);
	return TRUE;
}

bool LineInMixPlayDeinit(void)
{
	APP_DBG("LineIn Mix Play Deinit\n");
	if(sLineInPlayCt == NULL)
	{
		return TRUE;
	}
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}
	LineInPlayResFree();
	return TRUE;
}
#endif
#endif//#ifdef CFG_APP_LINEIN_MODE_EN
