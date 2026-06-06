#include "type.h"
#include "dac.h"
#include "dma.h"
#include "clk.h"
#include "debug.h"
#include "dac_interface.h"
#if defined(CFG_CHIP_CONFIG)
	#include "chip_config.h"
	#define DAC_DOUT_MODE				CHIP_AUDIODAC_DOUT_MODE
#elif defined(CFG_APP_CONFIG)
	#include "app_config.h"
	#include "ctrlvars.h"
	#define DAC_DOUT_MODE				gCtrlVars.HwCt.DAC0Ct.dac_out_mode
#else
	#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
	#define DAC_DOUT_MODE				MODE1
#endif

static uint8_t DAC_BitWidth = 24;
uint8_t mclkFreqNum = 1;

//AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
void AudioDAC_Init(DACParamCt *ct, uint32_t SampleRate, uint16_t BitWidth, void *Buf, uint16_t Len,  void *BufEXT, uint16_t LenEXT)
{
	mclkFreqNum = SampleRate > 48000 ? 4:1;
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_1, AUDIO_PLL_CLK1_FREQ * mclkFreqNum);
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_2, AUDIO_PLL_CLK2_FREQ * mclkFreqNum);

	if(IsSelectMclkClk1(SampleRate))
	{
		Clock_AudioMclkSel(AUDIO_DAC0, PLL_CLOCK1);
	}
	else
	{
		Clock_AudioMclkSel(AUDIO_DAC0, PLL_CLOCK2);
	}

    AudioDAC_SampleRateSet(DAC0, SampleRate);
    AudioDAC_EdgeSet(DAC0, POSEDGE);
    AudioDAC_VolSet(DAC0, 0x1000, 0x1000);
    AudioDAC_ScrambleEnable(DAC0);
    AudioDAC_ScrambleModeSet(DAC0, POS_NEG);
    AudioDAC_DsmOutdisModeSet(DAC0, 1);
	if(SampleRate<= 48000)
	{
		AudioDAC_DsmOriginSet();
	}
	else
	{
		AudioDAC_DsmOptimizeSet();
	}
	AudioDAC_ZeroNumSet(DAC0, 5);
	AudioDAC_FadeTimeSet(DAC0, 10);
	AudioDAC_FadeEnable(DAC0);
    if(BitWidth == 16)
    {
        AudioDAC_DoutModeSet(DAC0, DAC_DOUT_MODE, WIDTH_16_BIT); // WIDTH_24_BIT_1, WIDTH_24_BIT_2, WIDTH_16_BIT
        DAC_BitWidth = 16;
    }
    else if(BitWidth == 24)
    {
        AudioDAC_DoutModeSet(DAC0, DAC_DOUT_MODE, WIDTH_24_BIT_2);
        DAC_BitWidth = 24;
    }
    if(BufEXT != NULL)
    {
    	AudioDAC_ExternalEnable(DMIXMODE1, EXTMODE1);
    }

    AudioDAC_FuncReset(DAC0);

//#if	defined(CFG_CHIP_BP1532A2)
//	AudioDAC_AllPowerOn(DAC_Diff,DAC_NOLoad);
//#else
//	AudioDAC_AllPowerOn(DAC_Single,DAC_NOLoad);
//#endif

	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_DAC0_TX);
	DMA_CircularConfig(PERIPHERAL_ID_AUDIO_DAC0_TX, Len/2, Buf, Len);
	DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_DAC0_TX);

    if(BufEXT != NULL)
    {
    	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_DAC_EXT_TX);
        DMA_CircularConfig(PERIPHERAL_ID_AUDIO_DAC_EXT_TX, LenEXT / 2, BufEXT, LenEXT); // 这个给的ext通路
        DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_DAC_EXT_TX);
    }

	AudioDAC_ClkEnable(DAC0, TRUE);

//	AudioDAC_Enable(DAC0);//使能放在模拟上电部分 避免上电pop声
	AudioDAC_Reset(DAC0);
	AudioDAC_SoftMute(DAC0,FALSE, FALSE);
	if(AudioDAC_AllPowerOn(ct->DACModel, ct->DACLoadStatus, ct->PVDDModel, ct->DACEnergyModel, ct->DACVcomModel))
	{
		DBG("dac poweron ok!\n");
	}
	else
	{
		DBG("dac poweron abnormal or repeatedly!\n");
	}
}


void AudioDAC0_SampleRateChange(uint32_t SampleRate)
{
	mclkFreqNum = SampleRate > 48000 ? 4:1;
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_1, AUDIO_PLL_CLK1_FREQ * mclkFreqNum);
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_2, AUDIO_PLL_CLK2_FREQ * mclkFreqNum);

	if(IsSelectMclkClk1(SampleRate))
	{
		Clock_AudioMclkSel(DAC0, PLL_CLOCK1);
	}
	else
	{
		Clock_AudioMclkSel(DAC0, PLL_CLOCK2);
	}
	AudioDAC_SampleRateSet(DAC0, SampleRate);
	if(SampleRate<= 48000)
	{
		AudioDAC_DsmOriginSet();
	}
	else
	{
		AudioDAC_DsmOptimizeSet();
	}
	AudioDAC_FuncReset(DAC0);
}

uint16_t AudioDAC0_DataSpaceLenGet(void)
{
	if(DAC_BitWidth == 16)
		return DMA_CircularSpaceLenGet(PERIPHERAL_ID_AUDIO_DAC0_TX) / 4;
	else
		return DMA_CircularSpaceLenGet(PERIPHERAL_ID_AUDIO_DAC0_TX) / 8;
}

uint16_t AudioDAC0_DataLenGet(void)
{
	if(DAC_BitWidth == 16)
		return DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_DAC0_TX) / 4;
	else
		return DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_DAC0_TX) / 8;
}

uint16_t AudioDAC0_DataSet(void* Buf, uint16_t Len)
{
	uint16_t Length;

	if(Buf == NULL) return 0;
	if(DAC_BitWidth == 16)
	{
		Length = Len * 4;
		DMA_CircularDataPut(PERIPHERAL_ID_AUDIO_DAC0_TX, Buf, Length & 0xFFFFFFFC);
	}
	else
	{
		Length = Len * 8;
		DMA_CircularDataPut(PERIPHERAL_ID_AUDIO_DAC0_TX, Buf, Length & 0xFFFFFFF8);
	}
	return 0;
}

