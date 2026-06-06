#include <string.h>
#include "type.h"
#include "audio_adc.h"
#include "dma.h"
#include "clk.h"
#ifdef CFG_APP_CONFIG
#include "app_config.h"
#else
#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
#endif
#include "adc_interface.h"

static AUDIO_BitWidth ADC0_BitWidth = ADC_WIDTH_24BITS;
static AUDIO_BitWidth ADC1_BitWidth = ADC_WIDTH_24BITS;
extern uint8_t mclkFreqNum;

//AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
void AudioADC_DigitalInit(ADC_MODULE Module, uint32_t SampleRate, AUDIO_BitWidth BitWidth, void* Buf, uint16_t Len)
{
	//音频时钟使能，其他模块可能也会开启
	if(SampleRate > 48000)
	{
		mclkFreqNum = 4;
	}
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_1, AUDIO_PLL_CLK1_FREQ * mclkFreqNum);
	Clock_AudioPllClockSet(SYS_AUDIO_CLK_SELECT, PLL_CLK_2, AUDIO_PLL_CLK2_FREQ * mclkFreqNum);

	if(SampleRate > 48000)
	{
		AudioADC_MclkFreqSet(Module, 2);
	}
	else
	{
		AudioADC_MclkFreqSet(Module, mclkFreqNum);
	}

	if(Module == ADC0_MODULE)
    {
        ADC0_BitWidth = BitWidth;
    	AudioADC_Disable(ADC0_MODULE);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_DONE_INT);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_THRESHOLD_INT);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_ERROR_INT);
		DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
        DMA_CircularConfig(PERIPHERAL_ID_AUDIO_ADC0_RX, Len/2, Buf, Len);
        if(AudioADC_IsOverflow(ADC0_MODULE))
		{
        	AudioADC_OverflowClear(ADC0_MODULE);
		}

    	if(IsSelectMclkClk1(SampleRate))
    	{
    		Clock_AudioMclkSel(AUDIO_ADC0, PLL_CLOCK1);
    	}
    	else
    	{
    		Clock_AudioMclkSel(AUDIO_ADC0, PLL_CLOCK2);
    	}
    	AudioADC_SampleRateSet(ADC0_MODULE, SampleRate);

    	AudioADC_HighPassFilterConfig(ADC0_MODULE, 0xFFE);
    	AudioADC_HighPassFilterSet(ADC0_MODULE, TRUE);
        AudioADC_LREnable(ADC0_MODULE, 1, 1);
        AudioADC_FadeTimeSet(ADC0_MODULE, 10);
        AudioADC_FadeEnable(ADC0_MODULE);
		//AudioADC_FadeDisable(ADC0_MODULE);
        AudioADC_VolSet(ADC0_MODULE, 0x1000, 0x1000);
        AudioADC_SoftMute(ADC0_MODULE, FALSE, FALSE);
        AudioADC_Clear(ADC0_MODULE);

        AudioADC_WidthSet(ADC0_MODULE,BitWidth);
        AudioADC_Enable(ADC0_MODULE);

        DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC0_RX);
    }
    else if(Module == ADC1_MODULE)
    {
    	ADC1_BitWidth = BitWidth;
    	AudioADC_Disable(ADC1_MODULE);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_DONE_INT);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_THRESHOLD_INT);
    	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_ERROR_INT);
		DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC1_RX);
        DMA_CircularConfig(PERIPHERAL_ID_AUDIO_ADC1_RX, Len/2, Buf, Len);
    	if(AudioADC_IsOverflow(ADC1_MODULE))
    	{
    		AudioADC_OverflowClear(ADC1_MODULE);
    	}

    	if(IsSelectMclkClk1(SampleRate))
    	{
    		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK1);
    	}
    	else
    	{
    		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK2);
    	}
    	AudioADC_SampleRateSet(ADC1_MODULE, SampleRate);

    	AudioADC_HighPassFilterConfig(ADC1_MODULE, 0xFFE);
        AudioADC_HighPassFilterSet(ADC1_MODULE, TRUE);
        AudioADC_LREnable(ADC1_MODULE, 1, 1);
        AudioADC_FadeTimeSet(ADC1_MODULE, 10);
        AudioADC_FadeEnable(ADC1_MODULE);
        //AudioADC_FadeDisable(ADC1_MODULE);
        AudioADC_VolSet(ADC1_MODULE, 0x1000, 0x1000);
        AudioADC_SoftMute(ADC1_MODULE, FALSE, FALSE);
        AudioADC_Clear(ADC1_MODULE);
        AudioADC_WidthSet(ADC1_MODULE, BitWidth);
        AudioADC_Enable(ADC1_MODULE);

        DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC1_RX);
    }
}

void AudioADC_AnaInit(ADC_MODULE ADCMODULE, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel, AUDIO_Mode AUDIOMode, ADC_EnergyModel ADCEnergyModel, uint16_t Gain)
{
    if(ADCMODULE == ADC0_MODULE)
    {
        AudioADC_BIASPowerOn();
        if(ADCEnergyModel == ADCCommonEnergy)
        {
            AudioADC_ComparatorIBiasSet(ADC0_MODULE, 4, 4);
            AudioADC_OTA1IBiasSet(ADC0_MODULE, 4, 4);
            AudioADC_OTA2IBiasSet(ADC0_MODULE, 4, 4);
            AudioADC_LatchDelayIBiasSet(ADC0_MODULE, 4, 4);
            AudioADC_PGAIBiasSet(ADC0_MODULE, 4, 4);
        }
        else
        {
            AudioADC_BufferIBiasSet(ADC0_MODULE, 2, 2);
            AudioADC_ComparatorIBiasSet(ADC0_MODULE, 2, 2);
            AudioADC_OTA1IBiasSet(ADC0_MODULE, 1, 1);
            AudioADC_OTA2IBiasSet(ADC0_MODULE, 3, 3);
            AudioADC_PGAIBiasSet(ADC0_MODULE, 4, 4);
        }

        // 选择linein1通路
        if(ChannelSel == CHANNEL_LEFT)
        {
            AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN_NONE);
            if(InputSel == LINEIN1_LEFT)
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN1_LEFT);
                AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_LEFT, LINEIN1_LEFT, Gain);
            }
            else if(InputSel == LINEIN2_LEFT)
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN2_LEFT);
                AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_LEFT, LINEIN2_LEFT, Gain);
            }
            else
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN_NONE);
            }
        }
        else if(ChannelSel == CHANNEL_RIGHT)
        {
            AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
            if(InputSel == LINEIN1_RIGHT)
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN1_RIGHT);
                AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_RIGHT, LINEIN1_RIGHT, Gain);
            }
            else if(InputSel == LINEIN2_RIGHT)
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN2_RIGHT);
                AudioADC_PGAGainSet(ADC0_MODULE, CHANNEL_RIGHT, LINEIN2_RIGHT, Gain);
            }
            else
            {
                AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
            }
        }

        AudioADC_PGAMute(ADC0_MODULE, 0, 0);
        AudioADC_LREnable(ADC0_MODULE, 1, 1);
    }
    else if(ADCMODULE == ADC1_MODULE)
    {
        if(ChannelSel == CHANNEL_LEFT)
        {
            if(InputSel == MIC_LEFT)
            {
                AudioADC_BIASPowerOn();
                if(ADCEnergyModel == ADCCommonEnergy)
                {
                    AudioADC_ComparatorIBiasSet(ADC1_MODULE, 4, 4);
                    AudioADC_OTA1IBiasSet(ADC1_MODULE, 4, 4);
                    AudioADC_OTA2IBiasSet(ADC1_MODULE, 4, 4);
                    AudioADC_LatchDelayIBiasSet(ADC1_MODULE, 4, 4);
                    AudioADC_PGAIBiasSet(ADC1_MODULE, 4, 4);
                }
                else
                {
                    AudioADC_BufferIBiasSet(ADC1_MODULE, 1, 1);
                    AudioADC_ComparatorIBiasSet(ADC1_MODULE, 2, 2);
                    AudioADC_OTA1IBiasSet(ADC1_MODULE, 1, 1);
                    AudioADC_OTA2IBiasSet(ADC1_MODULE, 2, 2);
                    AudioADC_PGAIBiasSet(ADC1_MODULE, 2, 2);
					AudioADC_LatchDelayIBiasSet(ADC1_MODULE, 4, 4);
                }

                AudioADC_PGAPowerUp(ADC1_MODULE, 1, 1);
                AudioADC_PGAAbsMute(ADC1_MODULE, 0, 0);
                AudioADC_PGAMute(ADC1_MODULE, 0, 0);
                AudioADC_PowerUp(ADC1_MODULE, 1, 1);

                AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, MIC_LEFT, Gain); // PGA校准 0db

                AudioADC_PGAZeroCrossEnable(ADC1_MODULE, 1, 1);
                if(Diff == AUDIOMode) // 差分模式
                {
                    AudioADC_PGAMode(ADC1_MODULE, Diff);
                }
                else if(Single == AUDIOMode)
                {
                    AudioADC_PGAMode(ADC1_MODULE, Single);
                }
            }
            else
            {
                AudioADC_PGAPowerUp(ADC1_MODULE, 0, 0);
            }
        }
        else
        {
            AudioADC_PGAPowerUp(ADC1_MODULE, 0, 0);
        }
    }
}

void AudioADC_AnaDeInit(ADC_MODULE ADCMODULE)
{
    if(ADCMODULE == ADC0_MODULE)
    {
        AudioADC_PGAPowerUp(ADC0_MODULE, 0, 0);
        AudioADC_PowerUp(ADC0_MODULE, 0, 0);
        AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN_NONE);
        AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN_NONE);
    }
    else
    {
        AudioADC_PGAPowerUp(ADC1_MODULE, 0, 0);
        AudioADC_PowerUp(ADC1_MODULE, 0, 0);
    }
}

void AudioADC_DeInit(ADC_MODULE Module)
{
	if(Module == ADC0_MODULE)
    {
	    AudioADC_Disable(ADC0_MODULE);
        DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
        //DMA_ChannelClose(PERIPHERAL_ID_AUDIO_ADC0_RX);
        AudioADC_AnaDeInit(ADC0_MODULE);
    }
    else if(Module == ADC1_MODULE)
    {
	    AudioADC_Disable(ADC1_MODULE);
    	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC1_RX);
    	//DMA_ChannelClose(PERIPHERAL_ID_AUDIO_ADC1_RX);
    	AudioADC_AnaDeInit(ADC1_MODULE);
    }
}

//返回 uint：sample
uint16_t AudioADC0_DataLenGet(void)
{
	uint16_t NumSamples = 0;

	NumSamples = DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_ADC0_RX);
	if(ADC0_BitWidth == ADC_WIDTH_16BITS)
		return NumSamples / 4;
	else
		return NumSamples / 8;
}

//ADC1 单声道
uint16_t AudioADC1_DataLenGet(void)
{
	uint16_t NumSamples = 0;

    NumSamples = DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_ADC1_RX);

    return NumSamples / 4;
}

//返回Length uint：sample
uint16_t AudioADC0_DataGet(void* Buf, uint16_t Len)
{
	uint16_t Length = 0;//Samples

    Length = DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_ADC0_RX);

    if(ADC0_BitWidth == ADC_WIDTH_16BITS)
    {
		if(Length > Len * 4)
		{
			Length = Len * 4;
		}
		DMA_CircularDataGet(PERIPHERAL_ID_AUDIO_ADC0_RX, Buf, Length & 0xFFFFFFFC);
		return Length / 4;
    }
    else
    {
		if(Length > Len * 8)
		{
			Length = Len * 8;
		}
		DMA_CircularDataGet(PERIPHERAL_ID_AUDIO_ADC0_RX, Buf, Length & 0xFFFFFFF8);

		//高8位无符号位，需要移位产生
		int32_t *pcm = Buf;
		uint32_t i;
		for(i=0;i<Length / 4;i++)
		{
			pcm[i]<<=8;
			pcm[i]>>=8;
		}

		return Length / 8;
    }
}

//此处后续可能要做处理，区分单声道和立体声,数据格式转换在外边处理
//和DMA的配置也会有关系
//返回Length uint：sample
uint16_t AudioADC1_DataGet(void* Buf, uint16_t Len)
{
	uint16_t Length = 0;//Samples

    Length = DMA_CircularDataLenGet(PERIPHERAL_ID_AUDIO_ADC1_RX);
	if(Length > Len * 4)
	{
		Length = Len * 4;
	}
	DMA_CircularDataGet(PERIPHERAL_ID_AUDIO_ADC1_RX, Buf, Length & 0xFFFFFFFC);

    if(ADC1_BitWidth == ADC_WIDTH_16BITS)
    {
    	//高16位为0，转成16位数据
		int16_t *pcm = Buf;
		int32_t *pcm32 = Buf;
		uint32_t i;
		for(i=0;i<Length / 4;i++)
		{
			pcm[i] = pcm32[i];
		}
    }
    else
    {
		//高8位为0，符号位需要移位产生
		int32_t *pcm = Buf;
		uint32_t i;
		for(i=0;i<Length / 4;i++)
		{
			pcm[i]<<=8;
			pcm[i]>>=8;
		}
    }

    return Length / 4;
}

void AudioADC_SampleRateChange(ADC_MODULE Module,uint32_t SampleRate)
{
	AudioADC_MclkFreqSet(Module, mclkFreqNum);

	if(Module == ADC0_MODULE)
    {
    	if(IsSelectMclkClk1(SampleRate))
    	{
    		Clock_AudioMclkSel(AUDIO_ADC0, PLL_CLOCK1);
    	}
    	else
    	{
    		Clock_AudioMclkSel(AUDIO_ADC0, PLL_CLOCK2);
    	}
    	AudioADC_SampleRateSet(ADC0_MODULE, SampleRate);
    }
    else if(Module == ADC1_MODULE)
    {
    	if(IsSelectMclkClk1(SampleRate))
    	{
    		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK1);
    	}
    	else
    	{
    		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK2);
    	}
    	AudioADC_SampleRateSet(ADC1_MODULE, SampleRate);
    }
}

void AudioADC_MclkFreqSet(ADC_MODULE Module,uint32_t Div)
{
	if(Module == ADC0_MODULE)
    {
		Clock_ADC0ClkDivSet(Div - 1);
    }
	else if(Module == ADC1_MODULE)
	{
		Clock_ADC1ClkDivSet(Div - 1);
	}
}
