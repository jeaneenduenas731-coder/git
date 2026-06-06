/**
 **************************************************************************************
 * @file    dac_interface.h
 * @brief   audio dac interface
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2015-09-23 14:01:05$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__DAC_INTERFACE_H__
#define		__DAC_INTERFACE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 
#include "dac.h"

typedef struct __DACParamCt
{
	DAC_Model DACModel;
	DAC_LoadStatus DACLoadStatus;
	PVDD_Model PVDDModel;
	DAC_EnergyModel DACEnergyModel;
	DAC_VcomModel DACVcomModel;
} DACParamCt;

/**
 * @brief  AudioDAC配置函数
 * @return 无
 * @Note   因为涉及时钟配置, AudioDAC_Init需要在AudioADC和I2S配置之前调用
 * @Note   AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
 */
void AudioDAC_Init(DACParamCt *ct, uint32_t SampleRate, uint16_t BitWidth, void *Buf, uint16_t Len, void *BufEXT, uint16_t LenEXT);

/**
 * @brief  AudioDAC采样率切换配置
 * @return 无
 * @Note   如果采样率在<=48K、>48K之间切换的，需要在切换完dac的采样率后重新配置AudioADC、I2S的时钟
 * @Note   AudioADC的采样率大于48K时，AudioDAC采样率不能小于等于48K
 */
void AudioDAC0_SampleRateChange(uint32_t SampleRate);

uint16_t AudioDAC0_DataSpaceLenGet(void);
uint16_t AudioDAC0_DataSet(void* Buf, uint16_t Len);
uint16_t AudioDAC0_DataLenGet(void);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__DAC_INTERFACE_H__

