#include <string.h>
#include "type.h"
#include "dma.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "app_config.h"
#include "mode_task_api.h"
#include "main_task.h"
#include "debug.h"
#include "i2s_interface.h"
//service
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "mode_task_api.h"
#include "reset.h"
#include "ctrlvars.h"

#ifdef CFG_FUNC_I2S_MIX_MODE

#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
static void AudioI2sOutParamsSet(void)
{
	I2SParamCt i2s_set;
	i2s_set.IsMasterMode = CFG_RES_MIX_I2S_MODE;// 0:master 1:slave
	i2s_set.SampleRate = CFG_PARA_MIX_I2S_SAMPLERATE; //外设采样率
	i2s_set.I2sFormat = I2S_FORMAT_I2S;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	i2s_set.I2sBits = I2S_LENGTH_24BITS;
#else
	i2s_set.I2sBits = I2S_LENGTH_16BITS;
#endif

#if defined (CFG_RES_AUDIO_I2S_MIX_IN_EN) && defined (CFG_RES_AUDIO_I2S_MIX_OUT_EN)
	i2s_set.I2sTxRxEnable = 3;
#elif defined (CFG_RES_AUDIO_I2S_MIX_IN_EN)
	i2s_set.I2sTxRxEnable = 2;
#elif defined (CFG_RES_AUDIO_I2S_MIX_OUT_EN)
	i2s_set.I2sTxRxEnable = 1;
#else
	i2s_set.I2sTxRxEnable = 0;
#endif

	i2s_set.TxPeripheralID = PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE;

	i2s_set.TxBuf = (void*)mainAppCt.I2S_MIX_TX_FIFO;

	i2s_set.TxLen = mainAppCt.I2S_MIX_TX_FIFO_LEN;

	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_MCLK_GPIO));//mclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_MIX_LRCLK_GPIO));//lrclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_BCLK_GPIO));//bclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_DOUT_GPIO));//do

	AudioI2S_Init(CFG_RES_MIX_I2S_MODULE, &i2s_set);//

#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
	if(CFG_RES_MIX_I2S_MODULE == I2S0_MODULE)
		Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
	else
		Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
#else
	if(CFG_RES_MIX_I2S_MODULE == I2S0_MODULE)
		gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
	else
		gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
#endif
}
#endif

bool I2S_MixInit(bool hf_mode_flag)
{
	AudioCoreIO	AudioIOSet;
	uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);

	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	if(!AudioCoreSourceIsInit(I2S_MIX_SOURCE_NUM))
	{
		I2SParamCt i2s_set;
		i2s_set.IsMasterMode=CFG_RES_MIX_I2S_MODE;// 0:master 1:slave
		i2s_set.SampleRate=CFG_PARA_MIX_I2S_SAMPLERATE;
		i2s_set.I2sFormat=I2S_FORMAT_I2S;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		i2s_set.I2sBits = I2S_LENGTH_24BITS;
	#else
		i2s_set.I2sBits = I2S_LENGTH_16BITS;
	#endif
		i2s_set.I2sTxRxEnable = 2;

		i2s_set.RxPeripheralID = PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX_I2S_MODULE;

//		mainAppCt.I2S_MIX_RX_FIFO_LEN = AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2;
		mainAppCt.I2S_MIX_RX_FIFO_LEN = AudioCoreFrameSizeGet(DefaultNet) * 2 * sizeof(PCM_DATA_TYPE)*
															((CFG_PARA_MIX_I2S_SAMPLERATE <= 48000) ? 2 :
															 (CFG_PARA_MIX_I2S_SAMPLERATE <= 96000) ? 3 :
															 (CFG_PARA_MIX_I2S_SAMPLERATE <= 192000) ? 4 :
															 (CFG_PARA_MIX_I2S_SAMPLERATE <= 384000) ? 8 : 16);
		if (mainAppCt.I2S_MIX_RX_FIFO == NULL)
		{
			mainAppCt.I2S_MIX_RX_FIFO = (uint32_t*)osPortMalloc(mainAppCt.I2S_MIX_RX_FIFO_LEN);//I2S mix rx fifo
		}
		i2s_set.RxBuf=(void*)mainAppCt.I2S_MIX_RX_FIFO;
		i2s_set.RxLen=mainAppCt.I2S_MIX_RX_FIFO_LEN;

		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_MCLK_GPIO));//mclk
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_MIX_LRCLK_GPIO));//lrclk
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_BCLK_GPIO));//bclk
	#ifdef I2S_MIX_DOUT_GPIO
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_DOUT_GPIO));//do
	#endif
	#ifdef I2S_MIX_DIN_GPIO
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX_DIN_GPIO), GET_I2S_GPIO_MODE(I2S_MIX_DIN_GPIO));//di
	#endif

		I2S_ModuleDisable(CFG_RES_MIX_I2S_MODULE);
		AudioI2S_Init(CFG_RES_MIX_I2S_MODULE,&i2s_set);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		if(CFG_RES_MIX_I2S_MODULE == I2S0_MODULE)
			Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
		else
			Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
	#else
		if(CFG_RES_MIX_I2S_MODULE == I2S0_MODULE)
			gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
		else
			gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
	#endif

	#if ((CFG_RES_MIX_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_MIX_I2S_IN_SYNC_EN))
		{//master 或者关微调
			if(CFG_PARA_MIX_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;
			else
				AudioIOSet.Adapt = SRC_ONLY;
		}
	#else
		{//slave
			if(CFG_PARA_MIX_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;//
			else
				AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;//
		}
	#endif

		AudioIOSet.Sync = FALSE;//
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.Depth = AudioCoreFrameSizeGet(DefaultNet) * 2;//sI2SInPlayCt->I2SFIFO1 采样点深度
	//	DBG("Depth:%d", AudioIOSet.Depth);
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_MIX_I2S_SAMPLERATE;//根据实际外设选择
	//	AudioIOSet.CoreSampleRate = CFG_PARA_SAMPLE_RATE;
		if(CFG_RES_MIX_I2S_MODULE == 0)
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
		if(hf_mode_flag)
		{
			AudioIOSet.IOBitWidthConvFlag = 1;//需要数据进行位宽扩展
		}
		else
		{
			AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
		}
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, I2S_MIX_SOURCE_NUM))
		{
			DBG("I2S_MIX play source error!\n");
			return FALSE;
		}
	}
	AudioCoreSourceEnable(I2S_MIX_SOURCE_NUM);
	AudioCoreSourceAdjust(I2S_MIX_SOURCE_NUM, TRUE);
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[DefaultNet] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	if(hf_mode_flag)
	{
		AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 1;//需要做位宽转换处理
	}
	else
	{
		AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//不需要做位宽转换处理
	}
#endif
	if(!AudioCoreSinkIsInit(AUDIO_I2S_MIX_OUT_SINK_NUM))
	{
//		mainAppCt.I2S_MIX_TX_FIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.I2S_MIX_TX_FIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) *
											((CFG_PARA_MIX_I2S_SAMPLERATE <= 48000) ? 2 :
											 (CFG_PARA_MIX_I2S_SAMPLERATE <= 96000) ? 3 :
											 (CFG_PARA_MIX_I2S_SAMPLERATE <= 192000) ? 4 :
											 (CFG_PARA_MIX_I2S_SAMPLERATE <= 384000) ? 8 : 16);
		mainAppCt.I2S_MIX_TX_FIFO = (uint32_t*)osPortMalloc(mainAppCt.I2S_MIX_TX_FIFO_LEN);//I2S fifo
		if(mainAppCt.I2S_MIX_TX_FIFO != NULL)
		{
			memset(mainAppCt.I2S_MIX_TX_FIFO, 0, mainAppCt.I2S_MIX_TX_FIFO_LEN);
		}
		else
		{
			APP_DBG("malloc I2SFIFO error\n");
			return FALSE;
		}

#if((CFG_RES_MIX_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_MIX_I2S_OUT_SYNC_EN))
		{// Master 或不开微调
			if(CFG_PARA_MIX_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;//SRC_ONLY
			else
				AudioIOSet.Adapt = SRC_ONLY;
		}
#else//slave
		{
			if(CFG_PARA_MIX_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;//SRA_ONLY;//CLK_ADJUST_ONLY;
			else
				AudioIOSet.Adapt = SRC_ONLY;//SRC_SRA;//SRC_ADJUST;
		}
#endif
		AudioIOSet.Sync = TRUE;//I2S slave 时候如果master没有接，有可能会导致DAC也不出声音。
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_MIX_I2S_SAMPLERATE;//根据实际外设选择
		if(CFG_RES_MIX_I2S_MODULE == 0)
		{
			AudioIOSet.DataIOFunc = AudioI2S0_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S0_DataSpaceLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S1_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S1_DataSpaceLenGet;
		}


		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_I2S_MIX_OUT_SINK_NUM))
		{
			DBG("I2S Mix out init error");
			return FALSE;
		}

		AudioI2sOutParamsSet();
		AudioCoreSinkEnable(AUDIO_I2S_MIX_OUT_SINK_NUM);
		AudioCoreSinkAdjust(AUDIO_I2S_MIX_OUT_SINK_NUM, TRUE);
	}
	else//sam add,20230221
	{
		I2S_SampleRateSet(CFG_RES_MIX_I2S_MODULE, sampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_I2S_MIX_OUT_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_I2S_MIX_OUT_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
		AudioCore.AudioSink[AUDIO_I2S_MIX_OUT_SINK_NUM].Sync = TRUE;
	#endif
	}
#endif
	return TRUE;
}

void I2S_MixDeinit(void)
{
#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	if(AudioCoreSourceIsInit(I2S_MIX_SOURCE_NUM))
	{
		AudioCoreSourceDisable(I2S_MIX_SOURCE_NUM);
		AudioCoreSourceDeinit(I2S_MIX_SOURCE_NUM);
	}

	if(mainAppCt.I2S_MIX_RX_FIFO != NULL)
	{
		osPortFree(mainAppCt.I2S_MIX_RX_FIFO);
		mainAppCt.I2S_MIX_RX_FIFO = NULL;
	}
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	I2S_ModuleDisable(CFG_RES_MIX_I2S_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE);

	if(mainAppCt.I2S_MIX_TX_FIFO != NULL)
	{
		APP_DBG("I2S MIX FIFO\n");
		osPortFree(mainAppCt.I2S_MIX_TX_FIFO);
		mainAppCt.I2S_MIX_TX_FIFO = NULL;
	}
	AudioCoreSinkDeinit(AUDIO_I2S_MIX_OUT_SINK_NUM);
#endif
}
#endif


#ifdef CFG_FUNC_I2S_MIX2_MODE

#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
static void AudioI2sMix2OutParamsSet(void)
{
	I2SParamCt i2s_set;
	i2s_set.IsMasterMode = CFG_RES_MIX2_I2S_MODE;// 0:master 1:slave
	i2s_set.SampleRate = CFG_PARA_MIX2_I2S_SAMPLERATE; //外设采样率
	i2s_set.I2sFormat = I2S_FORMAT_I2S;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	i2s_set.I2sBits = I2S_LENGTH_24BITS;
#else
	i2s_set.I2sBits = I2S_LENGTH_16BITS;
#endif

#if defined (CFG_RES_AUDIO_I2S_MIX2_IN_EN) && defined (CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	i2s_set.I2sTxRxEnable = 3;
#elif defined (CFG_RES_AUDIO_I2S_MIX2_IN_EN)
	i2s_set.I2sTxRxEnable = 2;
#elif defined (CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	i2s_set.I2sTxRxEnable = 1;
#else
	i2s_set.I2sTxRxEnable = 0;
#endif

	i2s_set.TxPeripheralID = PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX2_I2S_MODULE;

	i2s_set.TxBuf = (void*)mainAppCt.I2S_MIX2_TX_FIFO;

	i2s_set.TxLen = mainAppCt.I2S_MIX2_TX_FIFO_LEN;

	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_MCLK_GPIO));//mclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_MIX2_LRCLK_GPIO));//lrclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_BCLK_GPIO));//bclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_DOUT_GPIO));//do

	AudioI2S_Init(CFG_RES_MIX2_I2S_MODULE, &i2s_set);//

#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
	if(CFG_RES_MIX_I2S_MODULE == I2S0_MODULE)
		Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
	else
		Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
#else
	if(CFG_RES_MIX2_I2S_MODULE == I2S0_MODULE)
		gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
	else
		gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
#endif
}
#endif

bool I2S_Mix2Init(bool hf_mode_flag)
{
	AudioCoreIO	AudioIOSet;
	uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);

	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	if(!AudioCoreSourceIsInit(I2S_MIX2_SOURCE_NUM))
	{
		I2SParamCt i2s_set;
		i2s_set.IsMasterMode=CFG_RES_MIX2_I2S_MODE;// 0:master 1:slave
		i2s_set.SampleRate=CFG_PARA_MIX2_I2S_SAMPLERATE;
		i2s_set.I2sFormat=I2S_FORMAT_I2S;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		i2s_set.I2sBits = I2S_LENGTH_24BITS;
	#else
		i2s_set.I2sBits = I2S_LENGTH_16BITS;
	#endif
		i2s_set.I2sTxRxEnable = 2;

		i2s_set.RxPeripheralID = PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX2_I2S_MODULE;

//		mainAppCt.I2S_MIX2_RX_FIFO_LEN = AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2;
		mainAppCt.I2S_MIX2_RX_FIFO_LEN = AudioCoreFrameSizeGet(DefaultNet) * 2 * sizeof(PCM_DATA_TYPE) *
															((CFG_PARA_MIX2_I2S_SAMPLERATE <= 48000) ? 2 :
															 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 96000) ? 3 :
															 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 192000) ? 4 :
															 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 384000) ? 8 : 16);
		if (mainAppCt.I2S_MIX2_RX_FIFO == NULL)
		{
			mainAppCt.I2S_MIX2_RX_FIFO = (uint32_t*)osPortMalloc(mainAppCt.I2S_MIX2_RX_FIFO_LEN);//I2S mix rx fifo
		}
		i2s_set.RxBuf=(void*)mainAppCt.I2S_MIX2_RX_FIFO;
		i2s_set.RxLen=mainAppCt.I2S_MIX2_RX_FIFO_LEN;

		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_MCLK_GPIO));//mclk
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_MIX2_LRCLK_GPIO));//lrclk
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_BCLK_GPIO));//bclk
	#ifdef I2S_MIX2_DOUT_GPIO
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_DOUT_GPIO));//do
	#endif
	#ifdef I2S_MIX2_DIN_GPIO
		GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MIX2_DIN_GPIO), GET_I2S_GPIO_MODE(I2S_MIX2_DIN_GPIO));//di
	#endif

		I2S_ModuleDisable(CFG_RES_MIX2_I2S_MODULE);
		AudioI2S_Init(CFG_RES_MIX2_I2S_MODULE,&i2s_set);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		if(CFG_RES_MIX2_I2S_MODULE == I2S0_MODULE)
			Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
		else
			Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source > 2 ? (gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source - 1):gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
	#else
		if(CFG_RES_MIX2_I2S_MODULE == I2S0_MODULE)
			gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
		else
			gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
	#endif

	#if ((CFG_RES_MIX2_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_MIX2_I2S_IN_SYNC_EN))
		{//master 或者关微调
			if(CFG_PARA_MIX2_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;
			else
				AudioIOSet.Adapt = SRC_ONLY;
		}
	#else
		{//slave
			if(CFG_PARA_MIX2_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;//
			else
				AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;//
		}
	#endif

		AudioIOSet.Sync = FALSE;//
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.Depth = AudioCoreFrameSizeGet(DefaultNet) * 2;//sI2SInPlayCt->I2SFIFO1 采样点深度
	//	DBG("Depth:%d", AudioIOSet.Depth);
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_MIX2_I2S_SAMPLERATE;//根据实际外设选择
	//	AudioIOSet.CoreSampleRate = CFG_PARA_SAMPLE_RATE;
		if(CFG_RES_MIX2_I2S_MODULE == 0)
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
		if(hf_mode_flag)
		{
			AudioIOSet.IOBitWidthConvFlag = 1;//需要数据进行位宽扩展
		}
		else
		{
			AudioIOSet.IOBitWidthConvFlag = 0;//不需要数据进行位宽扩展
		}
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, I2S_MIX2_SOURCE_NUM))
		{
			DBG("I2S_MIX play source error!\n");
			return FALSE;
		}
	}
	AudioCoreSourceEnable(I2S_MIX2_SOURCE_NUM);
	AudioCoreSourceAdjust(I2S_MIX2_SOURCE_NUM, TRUE);
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[DefaultNet] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	if(hf_mode_flag)
	{
		AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 1;//需要做位宽转换处理
	}
	else
	{
		AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//不需要做位宽转换处理
	}
#endif
	if(!AudioCoreSinkIsInit(AUDIO_I2S_MIX2_OUT_SINK_NUM))
	{
//		mainAppCt.I2S_MIX2_TX_FIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.I2S_MIX2_TX_FIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) *
									((CFG_PARA_MIX2_I2S_SAMPLERATE <= 48000) ? 2 :
									 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 96000) ? 3 :
									 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 192000) ? 4 :
									 (CFG_PARA_MIX2_I2S_SAMPLERATE <= 384000) ? 8 : 16);
		mainAppCt.I2S_MIX2_TX_FIFO = (uint32_t*)osPortMalloc(mainAppCt.I2S_MIX2_TX_FIFO_LEN);//I2S fifo
		if(mainAppCt.I2S_MIX2_TX_FIFO != NULL)
		{
			memset(mainAppCt.I2S_MIX2_TX_FIFO, 0, mainAppCt.I2S_MIX2_TX_FIFO_LEN);
		}
		else
		{
			APP_DBG("malloc I2SFIFO error\n");
			return FALSE;
		}

#if((CFG_RES_MIX2_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_MIX2_I2S_OUT_SYNC_EN))
		{// Master 或不开微调
			if(CFG_PARA_MIX2_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;//SRC_ONLY
			else
				AudioIOSet.Adapt = SRC_ONLY;
		}
#else//slave
		{
			if(CFG_PARA_MIX2_I2S_SAMPLERATE == sampleRate)
				AudioIOSet.Adapt = STD;//SRA_ONLY;//CLK_ADJUST_ONLY;
			else
				AudioIOSet.Adapt = SRC_ONLY;//SRC_SRA;//SRC_ADJUST;
		}
#endif
		AudioIOSet.Sync = TRUE;//I2S slave 时候如果master没有接，有可能会导致DAC也不出声音。
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_MIX2_I2S_SAMPLERATE;//根据实际外设选择
		if(CFG_RES_MIX2_I2S_MODULE == 0)
		{
			AudioIOSet.DataIOFunc = AudioI2S0_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S0_DataSpaceLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S1_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S1_DataSpaceLenGet;
		}


		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_I2S_MIX2_OUT_SINK_NUM))
		{
			DBG("I2S Mix out init error");
			return FALSE;
		}

		AudioI2sMix2OutParamsSet();
		AudioCoreSinkEnable(AUDIO_I2S_MIX2_OUT_SINK_NUM);
		AudioCoreSinkAdjust(AUDIO_I2S_MIX2_OUT_SINK_NUM, TRUE);
	}
	else//sam add,20230221
	{
		I2S_SampleRateSet(CFG_RES_MIX2_I2S_MODULE, sampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_I2S_MIX2_OUT_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_I2S_MIX2_OUT_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
		AudioCore.AudioSink[AUDIO_I2S_MIX2_OUT_SINK_NUM].Sync = TRUE;
	#endif
	}
#endif
	return TRUE;
}

void I2S_Mix2Deinit(void)
{
#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	if(AudioCoreSourceIsInit(I2S_MIX2_SOURCE_NUM))
	{
		AudioCoreSourceDisable(I2S_MIX2_SOURCE_NUM);
		AudioCoreSourceDeinit(I2S_MIX2_SOURCE_NUM);
	}

	if(mainAppCt.I2S_MIX2_RX_FIFO != NULL)
	{
		osPortFree(mainAppCt.I2S_MIX2_RX_FIFO);
		mainAppCt.I2S_MIX2_RX_FIFO = NULL;
	}
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	I2S_ModuleDisable(CFG_RES_MIX2_I2S_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX2_I2S_MODULE, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX2_I2S_MODULE, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX2_I2S_MODULE, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX2_I2S_MODULE);

	if(mainAppCt.I2S_MIX2_TX_FIFO != NULL)
	{
		APP_DBG("I2S MIX2 FIFO\n");
		osPortFree(mainAppCt.I2S_MIX2_TX_FIFO);
		mainAppCt.I2S_MIX2_TX_FIFO = NULL;
	}
	AudioCoreSinkDeinit(AUDIO_I2S_MIX2_OUT_SINK_NUM);
#endif
}
#endif
