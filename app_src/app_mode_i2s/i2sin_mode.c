/**
 **************************************************************************************
 * @file    i2sin_mode.c
 * @brief   
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2019-1-4 17:29:47$
 *
 * @Copyright (C) 2019, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "irqn.h"
#include "gpio.h"
#include "dma.h"
#include "rtos_api.h"
#include "app_message.h"
#include "app_config.h"
#include "debug.h"
#include "delay.h"
#include "audio_adc.h"
#include "dac.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "decoder.h"
#include "remind_sound.h"
#include "main_task.h"
#include "audio_effect.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "breakpoint.h"
#include "audio_vol.h"
#include "i2s.h"
#include "i2s_interface.h"
#include "ctrlvars.h"
#include "mcu_circular_buf.h"
#include "mode_task_api.h"
#include "clk.h"

#ifdef CFG_APP_I2SIN_MODE_EN

//#define CFG_I2SIN_TDM_MODE_EN

#define I2SIN_SOURCE_NUM				APP_SOURCE_NUM
#ifdef CFG_I2SIN_TDM_MODE_EN
#define I2STDM0_SOURCE_NUM				I2S_TDM0_SOURCE_NUM
#define I2STDM1_SOURCE_NUM				I2S_TDM1_SOURCE_NUM
#endif

#if (CFG_RES_I2S_MODE == 1)
volatile uint32_t CurrentSampleRate = 0;
#endif

typedef struct _I2SInPlayContext
{
//	xTaskHandle 		taskHandle;
//	MessageHandle		msgHandle;

	uint32_t			*I2SFIFO1;			//I2S的DMA循环fifo
#ifdef CFG_I2SIN_TDM_MODE_EN
	uint32_t            *I2STdmCarry;
	uint32_t			*I2STdm0PcmFifo;
	MCU_CIRCULAR_CONTEXT I2STdm0PcmCircularBuf;
	uint32_t			*I2STdm1PcmFifo;
	MCU_CIRCULAR_CONTEXT I2STdm1PcmCircularBuf;
#endif

#ifdef CFG_I2SIN_TDM_MODE_EN
	uint8_t 			SystemEffectMode;//用于标记system当前的音效模式
#endif

	//play
	uint32_t 			SampleRate; //带提示音时，如果不重采样，要避免采样率配置冲突

}I2SInPlayContext;

static const uint8_t DmaChannelMap[6] =
{
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

#ifdef CFG_FUNC_LINEIN_MIX_MODE
	PERIPHERAL_ID_AUDIO_ADC0_RX,
#endif

#if ((I2S_ALL_DMA_CH_CFG & I2SIN_MODE_I2S_DMA_CH) == 0)
	PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE,
#endif

#ifdef CFG_DUMP_DEBUG_EN
	CFG_DUMP_UART_TX_DMA_CHANNEL,
#endif

	PERIPHERAL_ID_SDIO_RX,
};

static  I2SInPlayContext*		sI2SInPlayCt;
uint8_t I2SInDecoderSourceNum(void);


#ifdef CFG_I2SIN_TDM_MODE_EN
//sample为单位
uint16_t AudioI2S1TDM0_DataLenGet(void)
{
	return MCUCircular_GetDataLen(&sI2SInPlayCt->I2STdm0PcmCircularBuf) / (sizeof(PCM_DATA_TYPE) * 2);
}

//sample为单位，buf大小：8 * MaxSize
uint16_t AudioI2S1TDM0_DataGet(void *pcm_out, uint16_t MaxPoint)
{
	return MCUCircular_GetData(&sI2SInPlayCt->I2STdm0PcmCircularBuf, pcm_out, MaxPoint * (sizeof(PCM_DATA_TYPE) * 2)) / (sizeof(PCM_DATA_TYPE) * 2);
}

//sample为单位
uint16_t AudioI2S1TDM1_DataLenGet(void)
{
	return MCUCircular_GetDataLen(&sI2SInPlayCt->I2STdm1PcmCircularBuf) / (sizeof(PCM_DATA_TYPE) * 2);
}

//sample为单位，buf大小：8 * MaxSize
uint16_t AudioI2S1TDM1_DataGet(void *pcm_out, uint16_t MaxPoint)
{
	return MCUCircular_GetData(&sI2SInPlayCt->I2STdm1PcmCircularBuf, pcm_out, MaxPoint * (sizeof(PCM_DATA_TYPE) * 2)) / (sizeof(PCM_DATA_TYPE) * 2);
}

void AudioI2S1TDMDataCarry(void)
{
    int32_t tdm0_space, tdm1_space;
    int32_t tdm_len, pcm_len;
    int32_t samples_to_process;
	int16_t *pcmBuf  = (int16_t *)sI2SInPlayCt->I2STdmCarry;

    // 获取可用DMA数据长度
    tdm_len = DMA_CircularDataLenGet(PERIPHERAL_ID_I2S1_RX);
    if(tdm_len == 0) return;

    // 计算两个buf的可用空间（以样本数计）
    tdm0_space = MCUCircular_GetSpaceLen(&sI2SInPlayCt->I2STdm0PcmCircularBuf);
    tdm1_space = MCUCircular_GetSpaceLen(&sI2SInPlayCt->I2STdm1PcmCircularBuf);

    // 确定可处理的样本数（取三个限制中的最小值）
    samples_to_process = MIN(tdm_len / (sizeof(PCM_DATA_TYPE) * 4), MIN(tdm0_space / (sizeof(PCM_DATA_TYPE) * 2), tdm1_space / (sizeof(PCM_DATA_TYPE) * 2)));

    if(samples_to_process <= 0)
    {
        DBG("Buf full or no data\n");
        return;
    }
    else
    {
        samples_to_process = (samples_to_process / 4) * 4;
    }
//    printf("len:%d ", samples_to_process);
    // 从DMA获取原始4声道TDM数据
    pcm_len = DMA_CircularDataGet(PERIPHERAL_ID_I2S1_RX, pcmBuf, samples_to_process * sizeof(PCM_DATA_TYPE) * 4);
    samples_to_process = pcm_len / (sizeof(PCM_DATA_TYPE) * 4);

//    printf("I2S DMA:%d, tdm0:%d, tdm1:%d, sample:%d\n", tdm_len, tdm0_space, tdm1_space, samples_to_process);
    // 分离声道处理
    PCM_DATA_TYPE *pInput = (PCM_DATA_TYPE *)pcmBuf;
    PCM_DATA_TYPE ch0_ch1[2 * samples_to_process]; // 前两个声道
    PCM_DATA_TYPE ch2_ch3[2 * samples_to_process]; // 后两个声道

    for(uint16_t i = 0; i < samples_to_process; i++)
    {
        // 分离前两个声道(CH0, CH1)
        ch0_ch1[2*i]   = pInput[4*i] >> 8;     // CH0
        ch0_ch1[2*i+1] = pInput[4*i+1] >> 8;   // CH1

        // 分离后两个声道(CH2, CH3)
        ch2_ch3[2*i]   = pInput[4*i+2] >> 8;   // CH2
        ch2_ch3[2*i+1] = pInput[4*i+3] >> 8;   // CH3
    }

    // 将分离后的数据放入各自的环形缓冲区
    MCUCircular_PutData(&sI2SInPlayCt->I2STdm0PcmCircularBuf, ch0_ch1, samples_to_process * sizeof(PCM_DATA_TYPE) * 2);
    MCUCircular_PutData(&sI2SInPlayCt->I2STdm1PcmCircularBuf, ch2_ch3, samples_to_process * sizeof(PCM_DATA_TYPE) * 2);
}

#endif

void I2SInPlayResFree(void)
{
	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig((void*)AudioNoAppProcess);
#ifdef CFG_I2SIN_TDM_MODE_EN
	AudioCoreSourceDisable(I2STDM0_SOURCE_NUM);
	AudioCoreSourceDeinit(I2STDM0_SOURCE_NUM);
	AudioCoreSourceDisable(I2STDM1_SOURCE_NUM);
	AudioCoreSourceDeinit(I2STDM1_SOURCE_NUM);
#else
	AudioCoreSourceDisable(I2SIN_SOURCE_NUM);
	AudioCoreSourceDeinit(I2SIN_SOURCE_NUM);
#endif

	I2S_ModuleRxDisable(CFG_RES_I2S_MODULE);
	I2S_ModuleDisable(CFG_RES_I2S_MODULE);
#ifndef CFG_RES_AUDIO_I2SOUT_EN
	DMA_ChannelDisable(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE);
#endif

	//PortFree

#if	0//defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDeinit(REMIND_SOURCE_NUM);
#endif

	if(sI2SInPlayCt->I2SFIFO1 != NULL)
	{
		osPortFree(sI2SInPlayCt->I2SFIFO1);
		sI2SInPlayCt->I2SFIFO1 = NULL;
	}

#ifdef CFG_I2SIN_TDM_MODE_EN
	if(sI2SInPlayCt->I2STdmCarry != NULL)
	{
		osPortFree(sI2SInPlayCt->I2STdmCarry);
		sI2SInPlayCt->I2STdmCarry = NULL;
	}
	if(sI2SInPlayCt->I2STdm0PcmFifo != NULL)
	{
		osPortFree(sI2SInPlayCt->I2STdm0PcmFifo);
		sI2SInPlayCt->I2STdm0PcmFifo = NULL;
	}
	if(sI2SInPlayCt->I2STdm1PcmFifo != NULL)
	{
		osPortFree(sI2SInPlayCt->I2STdm1PcmFifo);
		sI2SInPlayCt->I2STdm1PcmFifo = NULL;
	}
#endif

	APP_DBG("I2s:Kill Ct\n");
}

bool I2SInPlayResMalloc(uint16_t SampleLen)
{
	//I2SIn  digital (DMA)
//	sI2SInPlayCt->I2SFIFO1 = (uint32_t*)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	sI2SInPlayCt->I2SFIFO1 = (uint32_t*)osPortMalloc(SampleLen * 2 * sizeof(PCM_DATA_TYPE) *
												((CFG_PARA_I2S_SAMPLERATE <= 48000) ? 2 :
												 (CFG_PARA_I2S_SAMPLERATE <= 96000) ? 3 :
												 (CFG_PARA_I2S_SAMPLERATE <= 192000) ? 4 :
												 (CFG_PARA_I2S_SAMPLERATE <= 384000) ? 8 : 16));
	if(sI2SInPlayCt->I2SFIFO1 == NULL)
	{
		return FALSE;
	}
	memset(sI2SInPlayCt->I2SFIFO1, 0, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);

#ifdef CFG_I2SIN_TDM_MODE_EN
	//MCU buf(DMA 4ch -> MCU 2ch + MCU 2ch)
	sI2SInPlayCt->I2STdmCarry = (uint32_t*)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	if(sI2SInPlayCt->I2STdmCarry == NULL)
	{
		return FALSE;
	}
	memset(sI2SInPlayCt->I2STdmCarry, 0, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	//TDM0
	sI2SInPlayCt->I2STdm0PcmFifo = (uint32_t *)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	if(sI2SInPlayCt->I2STdm0PcmFifo == NULL)
	{
		return FALSE;
	}
	memset(sI2SInPlayCt->I2STdm0PcmFifo, 0, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	MCUCircular_Config(&sI2SInPlayCt->I2STdm0PcmCircularBuf, sI2SInPlayCt->I2STdm0PcmFifo, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	//TDM1
	sI2SInPlayCt->I2STdm1PcmFifo = (uint32_t *)osPortMalloc(SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	if(sI2SInPlayCt->I2STdm1PcmFifo == NULL)
	{
		return FALSE;
	}
	memset(sI2SInPlayCt->I2STdm1PcmFifo, 0, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
	MCUCircular_Config(&sI2SInPlayCt->I2STdm1PcmCircularBuf, sI2SInPlayCt->I2STdm1PcmFifo, SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2);
#endif

	return TRUE;
}

bool I2SInPlayResInit(void)
{
	I2SParamCt i2s_set;
	uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);

	sI2SInPlayCt->SampleRate = CFG_PARA_I2S_SAMPLERATE;//根据实际外设选择
	//Core Source1 para
	i2s_set.IsMasterMode=CFG_RES_I2S_MODE;// 0:master 1:slave
	i2s_set.SampleRate=sI2SInPlayCt->SampleRate;
#ifdef CFG_I2SIN_TDM_MODE_EN
	i2s_set.I2sFormat=I2S_FORMAT_TDM;//I2S_FORMAT_TDM in just support I2S1
#else
	i2s_set.I2sFormat=I2S_FORMAT_I2S;
#endif

#ifdef	CFG_AUDIO_WIDTH_24BIT
#ifdef CFG_I2SIN_TDM_MODE_EN
	i2s_set.I2sBits = I2S_LENGTH_32BITS;
#else
	i2s_set.I2sBits = I2S_LENGTH_24BITS;
#endif
#else
	i2s_set.I2sBits = I2S_LENGTH_16BITS;
#endif
	i2s_set.I2sTxRxEnable = 2;

	i2s_set.RxPeripheralID = PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE;

	i2s_set.RxBuf=sI2SInPlayCt->I2SFIFO1;
//	i2s_set.RxLen=AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2 ;//I2SIN_FIFO_LEN;
	i2s_set.RxLen=AudioCoreFrameSizeGet(DefaultNet) * 2  * sizeof(PCM_DATA_TYPE) *
										((CFG_PARA_I2S_SAMPLERATE <= 48000) ? 2 :
										 (CFG_PARA_I2S_SAMPLERATE <= 96000) ? 3 :
										 (CFG_PARA_I2S_SAMPLERATE <= 192000) ? 4 :
										 (CFG_PARA_I2S_SAMPLERATE <= 384000) ? 8 : 16);

#ifdef CFG_RES_AUDIO_I2SOUT_EN

	i2s_set.TxPeripheralID = PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE;

	i2s_set.TxBuf = (void*)mainAppCt.I2SFIFO;

	i2s_set.TxLen = mainAppCt.I2SFIFO_LEN;

	i2s_set.I2sTxRxEnable = 3;
#endif

	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MCLK_GPIO));//mclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_LRCLK_GPIO));//lrclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_BCLK_GPIO));//bclk
#ifdef I2S_DOUT_GPIO
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_DOUT_GPIO));//do
#endif
#ifdef I2S_DIN_GPIO
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_DIN_GPIO), GET_I2S_GPIO_MODE(I2S_DIN_GPIO));//di
#endif

	I2S_ModuleDisable(CFG_RES_I2S_MODULE);
	AudioI2S_Init(CFG_RES_I2S_MODULE,&i2s_set);

#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
	if(CFG_RES_I2S_MODULE == I2S0_MODULE)
		Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
	else
		Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
#else
	if(CFG_RES_I2S_MODULE == I2S0_MODULE)
		gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
	else
		gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
#endif

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
#if CFG_RES_I2S_MODE == 1
	I2S_SampleRateCheckInterruptClr(CFG_RES_I2S_MODULE);
	I2S_SampleRateCheckInterruptEnable(CFG_RES_I2S_MODULE);
#endif
#endif
//	//note Soure0.和sink0已经在main app中配置，不要随意配置
	//Core Soure1.Para
	AudioCoreIO	AudioIOSet;
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

#ifdef CFG_I2SIN_TDM_MODE_EN
	if(i2s_set.I2sFormat == I2S_FORMAT_TDM)
	{
		AudioIOSet.Sync = TRUE;//FALSE;//
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.Depth = AudioCoreFrameSizeGet(DefaultNet) * 2;//sI2SInPlayCt->I2SFIFO1 采样点深度
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_I2S_SAMPLERATE;//根据实际外设选择
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//需要数据进行位宽扩展
	#endif
		AudioIOSet.Adapt = SRC_ADJUST;
		AudioIOSet.DataIOFunc = AudioI2S1TDM0_DataGet ;
		AudioIOSet.LenGetFunc = AudioI2S1TDM0_DataLenGet;
		if(!AudioCoreSourceInit(&AudioIOSet, I2STDM0_SOURCE_NUM))
		{
			DBG("I2S TDM0 play source error!\n");
			return FALSE;
		}

		AudioIOSet.Adapt = SRC_ONLY;
		AudioIOSet.DataIOFunc = AudioI2S1TDM1_DataGet ;
		AudioIOSet.LenGetFunc = AudioI2S1TDM1_DataLenGet;
		if(!AudioCoreSourceInit(&AudioIOSet, I2STDM1_SOURCE_NUM))
		{
			DBG("I2S TDM1 play source error!\n");
			return FALSE;
		}
	}
	else
#endif
	{
#if ((CFG_RES_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_I2S_IN_SYNC_EN))
	{//master 或者关微调
		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = STD;
		else
			AudioIOSet.Adapt = SRC_ONLY;
	}
#else
	{//slave
//		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
//			AudioIOSet.Adapt = STD;//SRA_ONLY;//CLK_ADJUST_ONLY;//
//		else
//			AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;//
	#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		AudioIOSet.Adapt = STD;
	#elif defined(CFG_WIRELESS_EN) && defined(CFG_WIRELESS_OUT_ON)
		AudioIOSet.Adapt = SRC_SRA;
	#else
		AudioIOSet.Adapt = SRC_ADJUST;//SRC_ADJUST for slave in samplerate change
	#endif
	}
#endif
		AudioIOSet.Sync = TRUE;//FALSE;//
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.Depth = AudioCoreFrameSizeGet(DefaultNet) * 2;//sI2SInPlayCt->I2SFIFO1 采样点深度
	//	DBG("Depth:%d", AudioIOSet.Depth);
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_I2S_SAMPLERATE;//根据实际外设选择
	//	AudioIOSet.CoreSampleRate = CFG_PARA_SAMPLE_RATE;
		if(CFG_RES_I2S_MODULE == 0)
		{
			AudioIOSet.DataIOFunc = AudioI2S0_DataGet ;
			AudioIOSet.LenGetFunc = AudioI2S0_DataLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S1_DataGet ;
			AudioIOSet.LenGetFunc = AudioI2S1_DataLenGet;
		}

	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//需要数据进行位宽扩展
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, I2SIN_SOURCE_NUM))
		{
			DBG("I2Splay source error!\n");
			return FALSE;
		}
	}

	//Core Process
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioMusicProcess);
#else
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioBypassProcess);
#endif

	return TRUE;
}

/**
 * @func        I2SInPlay_Init
 * @brief       I2SIn模式参数配置，资源初始化
 * @param       MessageHandle 
 * @Output      None
 * @return      bool
 * @Others      任务块、I2S、Dac、AudioCore配置
 * @Others      数据流从I2S到audiocore配有函数指针，audioCore到Dac同理，由audiocoreService任务按需驱动
 * Record
 */
bool  I2SInPlayInit(void)
{
	bool ret;

	APP_DBG("I2SIn init\n");
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);//

	sI2SInPlayCt = (I2SInPlayContext*)osPortMalloc(sizeof(I2SInPlayContext));
	if(sI2SInPlayCt == NULL)
	{
		return FALSE;
	}
	memset(sI2SInPlayCt, 0, sizeof(I2SInPlayContext));

#ifdef CFG_I2SIN_TDM_MODE_EN
	sI2SInPlayCt->SystemEffectMode = mainAppCt.EffectMode;
	mainAppCt.EffectMode = EFFECT_MODE_TDM;
#endif
	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	if(!I2SInPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("I2SInPlay Res Error!\n");
		return FALSE;
	}

	ret = I2SInPlayResInit();
#ifdef CFG_I2SIN_TDM_MODE_EN
	AudioCoreSourceEnable(I2STDM0_SOURCE_NUM);
	AudioCoreSourceAdjust(I2STDM0_SOURCE_NUM, TRUE);
	AudioCoreSourceEnable(I2STDM1_SOURCE_NUM);
	AudioCoreSourceAdjust(I2STDM1_SOURCE_NUM, TRUE);
#else
	AudioCoreSourceEnable(I2SIN_SOURCE_NUM);
	AudioCoreSourceAdjust(I2SIN_SOURCE_NUM, TRUE);
#endif

	AudioCodecGainUpdata();//update hardware config

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(RemindSoundServiceItemRequest(SOUND_REMIND_I2SMODE, REMIND_PRIO_NORMAL) == FALSE)
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
#ifdef CFG_I2SIN_TDM_MODE_EN
	AudioCoreSourceUnmute(I2STDM0_SOURCE_NUM, TRUE, TRUE);
	AudioCoreSourceUnmute(I2STDM1_SOURCE_NUM, TRUE, TRUE);
#else
	AudioCoreSourceUnmute(I2SIN_SOURCE_NUM, TRUE, TRUE);
#endif
	return ret;
}
/**
 * @func        I2sInPlayEntrance
 * @brief       模式执行主体
 * @param       void * param  
 * @Output      None
 * @return      None
 * @Others      模式建立和结束过程
 * Record
 */
void I2SInPlayRun(uint16_t msgId)
{
#if (CFG_RES_I2S_MODE == 1)
	extern void AudioSpdifOut_SampleRateChange(uint32_t SampleRate);
	if (I2S_SampleRateCheckInterruptGet(CFG_RES_I2S_MODULE) && (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source < 2))
	{
#if defined (CFG_RES_AUDIO_SPDIFOUT_EN) && defined (CFG_I2S_SLAVE_TO_SPDIFOUT_EN)
		{
			Clock_PllLock(225792);
		}//Add the above actions to make I2S_SampleRateGet right
#endif
		CurrentSampleRate = I2S_SampleRateGet(CFG_RES_I2S_MODULE);
//		AudioI2S_SampleRateChange(CFG_RES_I2S_MODULE, CurrentSampleRate);
		APP_DBG("I2SIn samplerate change to:%ld\n", CurrentSampleRate);
#if defined (CFG_RES_AUDIO_SPDIFOUT_EN) && defined (CFG_I2S_SLAVE_TO_SPDIFOUT_EN)
		AudioSpdifOut_SampleRateChange(CurrentSampleRate);
		SyncModule_Reset();

		extern bool AudioEffectModeSel(EFFECT_MODE effectMode, uint8_t sel);
		AudioEffectModeSel(mainAppCt.EffectMode, 1);
#elif defined (CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000)
		AudioOutSampleRateSet(CurrentSampleRate);
#endif
		AudioI2S_SampleRateChange(CFG_RES_I2S_MODULE, CurrentSampleRate);
		DelayMs(1);

#ifdef CFG_I2SIN_TDM_MODE_EN
		AudioCoreSourceChange(I2STDM0_SOURCE_NUM, 0, CurrentSampleRate);
		AudioCoreSourceChange(I2STDM1_SOURCE_NUM, 0, CurrentSampleRate);
#else
		AudioCoreSourceChange(I2SIN_SOURCE_NUM, 0, CurrentSampleRate);
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
		AudioCoreSinkChange(AUDIO_I2SOUT_SINK_NUM, 0, CurrentSampleRate);
#endif

		I2S_SampleRateCheckInterruptClr(CFG_RES_I2S_MODULE);
	}
#endif

#ifdef CFG_I2SIN_TDM_MODE_EN
	AudioI2S1TDMDataCarry();
#endif

	switch(msgId)//警告：在此段代码，禁止新增提示音插播位置。
	{
/*	case MSG_REMIND_SOUND_PLAY_START:
			break;

		case MSG_REMIND_SOUND_PLAY_DONE://提示音播放结束
		case MSG_REMIND_SOUND_PLAY_REQUEST_FAIL:
			//AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
			break;
*/
		default:
			CommonMsgProccess(msgId);
			break;
	}
}

bool I2SInPlayDeinit(void)
{
	if(sI2SInPlayCt == NULL)
	{
		return TRUE;
	}
	APP_DBG("I2SIn Play deinit\n");
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	//Kill used services
#if	0//defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	AudioCoreSourceDisable(PLAYBACK_SOURCE_NUM);
	DecoderServiceDeinit(DECODER_REMIND_CHANNEL);
	while(GetDecoderServiceState()!=TaskStateStopped)
	{
		APP_DBG("I2S IN:%d\n",GetDecoderServiceState());
		osTaskDelay(1);
	}
#endif

	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MCLK_GPIO), 0);//mclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_LRCLK_GPIO),0);//lrclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_BCLK_GPIO), 0);//bclk
#ifdef I2S_DIN_GPIO
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_DIN_GPIO), 0);//di
#endif

#ifdef CFG_I2SIN_TDM_MODE_EN
	AudioCoreSourceDisable(I2STDM0_SOURCE_NUM);
	AudioCoreSourceDisable(I2STDM1_SOURCE_NUM);
#else
	AudioCoreSourceDisable(I2SIN_SOURCE_NUM);
#endif
	PauseAuidoCore();
	
	I2SInPlayResFree();
	ModeCommonDeinit();//通路全部释放
#ifdef CFG_I2SIN_TDM_MODE_EN
	mainAppCt.EffectMode = sI2SInPlayCt->SystemEffectMode;
#endif

	osPortFree(sI2SInPlayCt);
	sI2SInPlayCt = NULL;

	return TRUE;
}

#endif//#ifdef CFG_APP_I2SIN_MODE_EN
