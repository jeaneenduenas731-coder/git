#include <string.h>
#include "type.h"
#include "gpio.h"
#include "dma.h"
#include "app_message.h"
#include "app_config.h"
#include "debug.h"
#include "delay.h"
#include "spdif.h"
#include "clk.h"
#include "mcu_circular_buf.h"
#include "spdif_mode.h"
#include "spdif_out.h"
#include "ctrlvars.h"

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN

#define SPDIF_FIFO_LEN				(10 * 1024)
uint32_t SpdifAddr[SPDIF_FIFO_LEN/4];
int32_t SpdifBuf[1024];
bool isFirstFlag = 1;
uint16_t processFlag = 0;


bool Clock_APllLock(uint32_t PllFreq);
#include "clk.h"

void AudioSpdifOut_SampleRateChange(uint32_t SampleRate)
{
	if((SampleRate == 11025) || (SampleRate == 22050) || (SampleRate == 44100)
			|| (SampleRate == 88200) || (SampleRate == 176400))
	{
		Clock_PllLock(225792);
	}
	else
	{
		Clock_PllLock(245760);
	}
	SPDIF_SampleRateSet(SPDIF_OUT_NUM,SampleRate);

	switch (SampleRate)
	{
		case 32000: spdif_channelstatus_ct.sample_freq = FREQ_32K; break;
		case 44100: spdif_channelstatus_ct.sample_freq = FREQ_44K; break;
		case 48000: spdif_channelstatus_ct.sample_freq = FREQ_48K; break;
		case 88200: spdif_channelstatus_ct.sample_freq = FREQ_88K; break;
		case 96000: spdif_channelstatus_ct.sample_freq = FREQ_96K; break;
		case 176400: spdif_channelstatus_ct.sample_freq = FREQ_176K; break;
		case 192000: spdif_channelstatus_ct.sample_freq = FREQ_192K; break;
	}
	isFirstFlag = 1;
}

void AudioSpdifOutParamsSet(void)
{
	uint32_t sampleRate = AudioCoreMixSampleRateGet(DefaultNet);;

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
	SyncModule_Init();
#endif
	SPDIF_OUT_GPIO_SET();
	Clock_SpdifClkSelect(PLL_CLK_MODE);//DPLL
	SPDIF_TXInit(SPDIF_OUT_NUM,1, 1, 0, 0);
	DMA_CircularConfig(SPDIF_OUT_DMA_ID, sizeof(SpdifAddr)/4, SpdifAddr, sizeof(SpdifAddr));
	AudioSpdifOut_SampleRateChange(sampleRate);
	SPDIF_ModuleEnable(SPDIF_OUT_NUM);
	DMA_ChannelEnable(SPDIF_OUT_DMA_ID);

	switch (sampleRate)
	{
		case 32000: spdif_channelstatus_ct.sample_freq = FREQ_32K; break;
		case 44100: spdif_channelstatus_ct.sample_freq = FREQ_44K; break;
		case 48000: spdif_channelstatus_ct.sample_freq = FREQ_48K; break;
		case 88200: spdif_channelstatus_ct.sample_freq = FREQ_88K; break;
		case 96000: spdif_channelstatus_ct.sample_freq = FREQ_96K; break;
		case 176400: spdif_channelstatus_ct.sample_freq = FREQ_176K; break;
		case 192000: spdif_channelstatus_ct.sample_freq = FREQ_192K; break;
	}

#ifdef CFG_AUDIO_WIDTH_24BIT
	spdif_channelstatus_ct.word_len = LEN_24BIT;
#else
	spdif_channelstatus_ct.word_len = LEN_16BIT;
#endif
}

uint16_t AudioSpdifTXDataSet(void* Buf, uint16_t Len)
{
	uint16_t Length;
	int m;
	if(Buf == NULL) return 0;

	memset(SpdifBuf, 0, sizeof(SpdifBuf));

#ifdef CFG_AUDIO_WIDTH_24BIT
	Length = Len * 8;
	m = SPDIF_PCMDataToSPDIFData(SPDIF_OUT_NUM,(int32_t *)Buf, Length, (int32_t *)SpdifBuf, SPDIF_WORDLTH_24BIT);
#else
	Length = Len * 4;
	m = SPDIF_PCMDataToSPDIFData(SPDIF_OUT_NUM,(int32_t *)Buf, Length, (int32_t *)SpdifBuf, SPDIF_WORDLTH_16BIT);
#endif

	if(isFirstFlag)
	{
		processFlag = 0;
	}
	SpdifSampleRateandBiteProcess((int32_t *)SpdifBuf, m / 8);
	DMA_CircularDataPut(SPDIF_OUT_DMA_ID, (void *)SpdifBuf, m & 0xFFFFFFFC);
	if(isFirstFlag)
	{
		Clock_Module3Disable(SPDIF1_CLK_EN);
		SPDIF_ModuleEnable(SPDIF_OUT_NUM);
		DelayUs(1);
		Clock_Module3Enable(SPDIF1_CLK_EN);
		isFirstFlag = 0;
	}

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
	SyncModule_Process();
#endif
	return 0;
}


uint16_t AudioSpdifTXDataSpaceLenGet(void)
{
#ifdef CFG_AUDIO_WIDTH_24BIT
	return DMA_CircularSpaceLenGet(SPDIF_OUT_DMA_ID) / 16;
#else
	return DMA_CircularSpaceLenGet(SPDIF_OUT_DMA_ID) / 8;
#endif
}

void SpdifSampleRateandBiteProcess(int32_t *spdifBuf, uint32_t len)
{
	uint16_t i = 0;
	for(; i < len; i++)
	{
		if(processFlag == 192)
		{
			processFlag = 0;
		}

		if(processFlag >=23 && processFlag <= 26)
		{
			spdifBuf[i*2+0] |= (((spdif_channelstatus_ct.sample_freq >> (26 - processFlag)) & 0x1) << 26);
			spdifBuf[i*2+1] |= (((spdif_channelstatus_ct.sample_freq >> (26 - processFlag)) & 0x1) << 26);
//			printf("frame:%d,bit%d = %d\n", i, processFlag+1, ((spdif_channelstatus_ct.sample_freq >> (26 - processFlag)) & 0x1));
		}
		else if(processFlag >=31 && processFlag <= 34)
		{
			spdifBuf[i*2+0] |= (((spdif_channelstatus_ct.word_len >> (34 - processFlag)) & 0x1) << 26);
			spdifBuf[i*2+1] |= (((spdif_channelstatus_ct.word_len >> (34 - processFlag)) & 0x1) << 26);
//			printf("frame:%d,bit%d = %d\n", i, processFlag+1, ((spdif_channelstatus_ct.word_len >> (34 - processFlag)) & 0x1));
		}

		processFlag++;
	}
}
#endif
