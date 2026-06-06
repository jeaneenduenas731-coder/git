/**
 **************************************************************************************
 * @file    adc_interface.h
 * @brief   audio adc interface
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2015-09-23 14:01:05$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__ADC_INTERFACE_H__
#define		__ADC_INTERFACE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 
#include "audio_adc.h"

typedef enum ADC_EnergyModel
{
	ADCLowEnergy = 0, //低功耗模式
	ADCCommonEnergy
} ADC_EnergyModel;

void AudioADC_DMARestart(ADC_MODULE Module, void* Buf, uint16_t Len);

/**
 * @brief  AudioADC数字部分配置函数
 * @return 无
 * @Note   因为涉及时钟配置, AudioDAC_Init需要在AudioADC和I2S配置之前调用
 * @Note   AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
 */
void AudioADC_DigitalInit(ADC_MODULE Module, uint32_t SampleRate, AUDIO_BitWidth BitWidth, void* Buf, uint16_t Len);
void AudioADC_DeInit(ADC_MODULE Module);
void AudioADC_AnaInit(ADC_MODULE ADCMODULE, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel, AUDIO_Mode AUDIOMode, ADC_EnergyModel ADCEnergyModel, uint16_t Gain);
void AudioADC_AnaDeInit(ADC_MODULE ADCMODULE);

uint16_t AudioADC0_DataLenGet(void);
uint16_t AudioADC1_DataLenGet(void);

uint16_t AudioADC0_DataGet(void* Buf, uint16_t Len);
uint16_t AudioADC1_DataGet(void* Buf, uint16_t Len);

/**
 * @brief  AudioADC采样率切换配置
 * @return 无
 * @Note   如果采样率在<=48K、>48K之间切换的，需要在切换完dac的采样率后重新配置AudioADC、I2S的时钟
 * @Note   AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
 */
void AudioADC_SampleRateChange(ADC_MODULE Module,uint32_t SampleRate);

void AudioADC_MclkFreqSet(ADC_MODULE Module,uint32_t Div);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__ADC_INTERFACE_H__

