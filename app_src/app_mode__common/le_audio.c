#include <type.h>
#include "app_config.h"
#include "gpio.h"
#include "irqn.h"
#include "dma.h"
#include "main_task.h"
#include "dac_interface.h"
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "mode_task_api.h"
#include "reset.h"
#include "ctrlvars.h"

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN

static uint16_t	i2s_in_drop_len = 0;
static bool dac_pcm_invalid_flag = 0;
static bool     dac_drop_flag = 0;
static int32_t i2s_in_recv_data_len = 0;
static uint32_t i2s_in_recv_flag = 0;

enum{
	LeAudio_INT_NONE,
	LeAudio_DAC_I2S_OFF_INT,
	LeAudio_DAC_ON_INT,
	LeAudio_I2S_ON_INT,
};

void LeAudioIntEnable(void)
{
#ifndef CFG_MIC_KARAOKE_DUAL_CHIP_MODE_EN
	//enable pull up resister.
    GPIO_RegOneBitSet(GPIO_A_IE, GPIOA4);
	GPIO_RegOneBitClear(GPIO_A_OE, GPIOA4);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIOA4);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIOA4);

	//enable int
	GPIO_INTEnable(GPIO_A_INT, GPIOA4, GPIO_EDGE_TRIGGER);

	//enable gpio irqc
	NVIC_EnableIRQ(Gpio_IRQn);
#endif
}

void LeAudioIntDisable(void)
{
	NVIC_DisableIRQ(Gpio_IRQn);
}

void AudioDAC0_DataDrop(uint16_t Len)
{
	int32_t write = DMA_CircularWritePtrGet(PERIPHERAL_ID_AUDIO_DAC0_TX);
	int32_t read = DMA_CircularReadPtrGet(PERIPHERAL_ID_AUDIO_DAC0_TX);
	uint16_t dac_len = 	AudioDAC0_DataLenGet();

	if(write < 0 || read < 0)
		return;

	if(dac_len < Len)
		return;

//	Len = dac_len-Len;
//	printf("DMA_CircularWritePtrGet:%x,%x\n",write,read);

#ifdef	CFG_AUDIO_WIDTH_24BIT
	memcpy(mainAppCt.DACFIFO,mainAppCt.DACFIFO+Len*2,(dac_len - Len)*8);
	DMA_CircularWritePtrSet(PERIPHERAL_ID_AUDIO_DAC0_TX,(dac_len - Len)*8);
#else
	memcpy(mainAppCt.DACFIFO,mainAppCt.DACFIFO+Len,(dac_len - Len)*4);
	DMA_CircularWritePtrSet(PERIPHERAL_ID_AUDIO_DAC0_TX,(dac_len - Len)*4);
#endif
}

void GpioInterrupt(void)
{
	if(GPIO_INTFlagGet(GPIO_A_INT) & GPIOA4)
	{
		GPIO_INTFlagClear(GPIO_A_INT, GPIOA4);

		if(GPIO_RegGet(GPIO_A_IN) & GPIOA4)
		{
			i2s_in_recv_data_len += AudioI2S_DataLenGet(CFG_RES_I2S_MODULE);
			i2s_in_recv_flag = 0;
//			printf("-----------DAC On:%d-%d %d\n",	AudioDAC0_DataLenGet(), i2s_in_drop_len,AudioI2S_DataLenGet(CFG_RES_I2S_MODULE));
			AudioDAC0_DataDrop(i2s_in_drop_len);
//			printf("DacValidBuffer:%d\n",AudioDAC0_DataLenGet());
			DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_DAC0_TX);
			AudioDAC_Enable(DAC0);
			dac_pcm_invalid_flag = 0;
//			printf("-----------DAC On:%d\n",i2s_in_recv_data_len);
//			i2s_in_recv_data_len = 0;
		}
		else
		{
			i2s_in_drop_len = AudioI2S_DataLenGet(CFG_RES_I2S_MODULE) + 512;
			i2s_in_recv_data_len = -(i2s_in_drop_len - 512);
//			printf("------------DAC Off %d\n", i2s_in_drop_len);
			dac_pcm_invalid_flag = 1;
			i2s_in_recv_flag = 1;
			AudioDAC_Disable(DAC0);
			AudioDAC_FuncReset(DAC0);
			DMA_CircularFIFOClear(PERIPHERAL_ID_AUDIO_DAC0_TX);
			DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_DAC0_TX);
		}

	}
}

uint16_t AudioI2S_DataGet_LeAudio(void* Buf, uint16_t Len)
{
	dac_pcm_invalid_flag = 0;
	if(i2s_in_recv_flag)
		i2s_in_recv_data_len += Len;
	return AudioI2S_DataGet(CFG_RES_I2S_MODULE, Buf, Len);
}

void LeAduio_BisDacPcmDropLen(uint16_t  len)
{
	if(i2s_in_recv_data_len == 0)
		return;
	printf("DacPcmDropLen: %d,%d\n",i2s_in_recv_data_len,len);
	i2s_in_recv_data_len -= len;
	printf("delay: %dus,%d %d\n",i2s_in_recv_data_len*20,AudioDAC0_DataLenGet(),i2s_in_recv_data_len);
	if(i2s_in_recv_data_len > 1 || i2s_in_recv_data_len < -1)
		dac_drop_flag = 1;
	else
		dac_drop_flag = 0;
}

uint16_t AudioDAC0_DataSet_LeAudio(void* Buf, uint16_t Len)
{
	if(dac_pcm_invalid_flag)
	{
		return 0;
	}

	if(dac_drop_flag)
	{
		if(i2s_in_recv_data_len < 0)
		{
			if(Len > -i2s_in_recv_data_len)
				AudioDAC0_DataSet(Buf,-i2s_in_recv_data_len);
			dac_drop_flag = 0;
			i2s_in_recv_data_len = 0;
		}
		else
		{
			if(Len > i2s_in_recv_data_len)
			{
				Len -= i2s_in_recv_data_len;
				dac_drop_flag = 0;
				i2s_in_recv_data_len = 0;
			}
			else if(Len < i2s_in_recv_data_len)
			{
				i2s_in_recv_data_len -= Len;
				Len = 0;
			}
			else
			{
				dac_drop_flag = 0;
				i2s_in_recv_data_len = 0;
			}
		}
	}
	return AudioDAC0_DataSet(Buf,Len);
}

bool LeAudio_I2sInInit(bool hf_mode_flag)
{
	AudioCoreIO	AudioIOSet;
	uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);

	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

	if(!AudioCoreSourceIsInit(I2S_MIX_SOURCE_NUM))
	{
		I2SParamCt i2s_set;
		uint32_t sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);

		i2s_set.IsMasterMode = CFG_RES_I2S_MODE;// 0:master 1:slave
		i2s_set.SampleRate = CFG_PARA_I2S_SAMPLERATE;//根据实际外设选择
		i2s_set.I2sFormat = I2S_FORMAT_I2S;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		i2s_set.I2sBits = I2S_LENGTH_24BITS;
	#else
		i2s_set.I2sBits = I2S_LENGTH_16BITS;
	#endif
		i2s_set.I2sTxRxEnable = 2;

		i2s_set.RxPeripheralID = PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE;

		mainAppCt.LE_AUDIO_I2S_RX_FIFO_LEN = AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 6;
		if (mainAppCt.LE_AUDIO_I2S_RX_FIFO == NULL)
		{
			mainAppCt.LE_AUDIO_I2S_RX_FIFO = (uint32_t*)osPortMalloc(mainAppCt.LE_AUDIO_I2S_RX_FIFO_LEN);//I2S mix rx fifo
		}

		i2s_set.RxBuf=(void*)mainAppCt.LE_AUDIO_I2S_RX_FIFO;

		i2s_set.RxLen = mainAppCt.LE_AUDIO_I2S_RX_FIFO_LEN;//I2SIN_FIFO_LEN;

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
			Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
		else
			Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
	#else
		if(CFG_RES_I2S_MODULE == I2S0_MODULE)
			gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
		else
			gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
	#endif

		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = STD;
		else
			AudioIOSet.Adapt = SRC_ONLY;

		AudioIOSet.Sync = TRUE;//
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
			AudioIOSet.DataIOFunc = AudioI2S_DataGet_LeAudio;
			AudioIOSet.LenGetFunc = AudioI2S0_DataLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S_DataGet_LeAudio;
			AudioIOSet.LenGetFunc = AudioI2S1_DataLenGet;
		}

#ifdef	CFG_AUDIO_WIDTH_24BIT
		if(hf_mode_flag)
			AudioIOSet.IOBitWidth = 0;//0,16bit,1:24bit
		else
			AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//需要数据进行位宽扩展
#endif

		if(!AudioCoreSourceInit(&AudioIOSet, I2S_MIX_SOURCE_NUM))
		{
			DBG("I2S_MIX play source error!\n");
			return FALSE;
		}
	}

	I2S_FadeDisable(CFG_RES_I2S_MODULE);
	I2S_MuteSet(CFG_RES_I2S_MODULE,0);

	AudioCoreSourceEnable(I2S_MIX_SOURCE_NUM);
//	AudioCoreSourceDisable(I2S_MIX_SOURCE_NUM);
	AudioCoreSourceAdjust(I2S_MIX_SOURCE_NUM, TRUE);

	return TRUE;
}

void LeAudio_I2sInDeinit(void)
{
	if(AudioCoreSourceIsInit(I2S_MIX_SOURCE_NUM))
	{
		AudioCoreSourceDisable(I2S_MIX_SOURCE_NUM);
		AudioCoreSourceDeinit(I2S_MIX_SOURCE_NUM);
	}

	if(mainAppCt.LE_AUDIO_I2S_RX_FIFO != NULL)
	{
		osPortFree(mainAppCt.LE_AUDIO_I2S_RX_FIFO);
		mainAppCt.LE_AUDIO_I2S_RX_FIFO = NULL;
	}
}
#endif

