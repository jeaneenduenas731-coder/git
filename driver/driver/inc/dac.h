/**
 *******************************************************************************
 * @file    dac.h
 * @author  Sean
 * @version V1.0.0
 * @date    2017/04/21
 * @brief
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2014 MVSilicon </center></h2>
 */

/**
 * @addtogroup DAC
 * @{
 * @defgroup dac dac.h
 * @{
 */

#ifndef __DAC_H__
#define __DAC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * DAC Module Select
 */
typedef enum _DAC_MODULE
{
	DAC0 = 0
}DAC_MODULE;

typedef enum _SCRAMBLE_MODULE
{
	DWA = 0,
	RANDOM_DWA = 1,
	POS_NEG = 2
}SCRAMBLE_MODULE;

typedef enum _DOUT_MODE
{
    MODE0 = 0,// L_o=L_i;R_o=R_i;
    MODE1 = 1,// L_o=R_i;R_o=L_i; L R swap;
    MODE2 = 2,// L_o=(L_i+R_i)/2;R_o=(L_i+R_i)/2;
    MODE3 = 3,// L_o=(L_i+R_i)/2;R_o=-(L_i+R_i)/2;
}DOUT_MODE;

typedef enum _DMIX_MODE
{
    DMIXMODE0 = 0,//direct path data sel asdm data
    DMIXMODE1 = 1,//direct path data sel i2s0 data
    DMIXMODE2 = 2,//direct path data sel i2s1 data
	DMIXMODE3 = 3,//direct path data sel asdm data
}DMIX_MODE;

//B5 dac The L/R channel definition and schematic are inverted, so it is recommended to use MODE1 by default
typedef enum _EXT_MODE
{
    EXTMODE0 = 0,//tx_dat mix direct path data
    EXTMODE1 = 1,//L=L_int+L_ext,R=R_int+R_ext
    EXTMODE2 = 2,//L=L_int+L_ext,R=R_int+L_ext
    EXTMODE3 = 3,//L=L_int+L_ext,R=R_int+0
}EXT_MODE;

typedef enum _DOUT_WIDTH
{
	WIDTH_16_BIT = 0,
	WIDTH_24_BIT_1 = 2,
	WIDTH_24_BIT_2 = 3,
}DOUT_WIDTH;

typedef enum _EDGE
{
	POSEDGE = 0,
	NEGEDGE = 1
}EDGE;

typedef enum DAC_Model
{
	DAC_Single,   //单端
	DAC_Diff      //差分
} DAC_Model;

typedef enum DAC_LoadStatus
{
	DAC_NOLoad = 0, //不带负载
	DAC_Load     	//带负载
} DAC_LoadStatus;

typedef enum PVDD_Model
{
	PVDD16 = 0, //使用1.6V电源
	PVDD33     	//使用3.3V电源
} PVDD_Model;

typedef enum DAC_EnergyModel
{
	DACLowEnergy = 0, //低功耗模式
	DACCommonEnergy
} DAC_EnergyModel;

typedef enum DAC_VcomModel
{
	Disable = 0,
	Direct     //vcom直驱耳机
} DAC_VcomModel;

/**
 * @brief  Audio DAC module enable
 * @param  DACModule which AudioDac you will use
 * @return NONE
 */
void AudioDAC_Enable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC module disable
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_Disable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC module dither enable
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_DitherEnable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC module dither disable
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_DitherDisable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC module scramble enable
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_ScrambleEnable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC module scramble disable
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_ScrambleDisable(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC pause,stop ask data from DMA
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_Pause(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC disable pause, go on asking data from DMA
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_Run(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC output mode set
 * @param  DoutOut 	MODE0 = 0,// L_o=L_i;R_o=R_i;
					MODE1 = 1,// L_o=R_i;R_o=L_i;
					MODE2 = 2,// L_o=(L_i+R_i)/2;R_o=(L_i+R_i)/2;
					MODE3 = 3// L_o=(L_i+R_i)/2;R_o=-(L_i+R_i)/2;
 * @param DoutOut
 * @param DoutWidth
 * @return NONE
 */
void AudioDAC_DoutModeSet(DAC_MODULE DACModule, DOUT_MODE DoutOut, DOUT_WIDTH DoutWidth);

/**
 * @brief  Audio DAC sample edge mode config
 * @param  DACModule which AudioDAC you will use
 * @param  Edge posedge or negedge to sample data
 * @return NONE
 */
void AudioDAC_EdgeSet(DAC_MODULE DACModule,EDGE Edge);

/**
 * @brief  Audio DAC scramble mode config
 * @param  DACModule which AudioDAC you will use
 * @param  ScrambleMode DWA
 *                      RANDOM_DWA
 *                      BUTTERFLY_DWA
 * @return NONE
 */
void AudioDAC_ScrambleModeSet(DAC_MODULE DACModule,SCRAMBLE_MODULE ScrambleMode);

/**
 * @brief  Audio DAC dsm outdis mode config
 * @param  DACModule which AudioDAC you will use
 * @param  mode 0 the DSM out drive the scramble module as normal
 *              1 auto mute the scramble input data  when the continuous N points of the PCM * VOL data values are 0, or the VOL is 0,the N is set by mdac0_zeros_num_sel
 * @return NONE
 */
void AudioDAC_DsmOutdisModeSet(DAC_MODULE DACModule,uint8_t mode);

/**
 * @brief  Audio DAC zero num set
 * @param  DACModule which AudioDAC you will use
 * @param  sel the continuous PCM data as 0, and the 0's number limit
 *             	0: zeros number value: 512
				1: zeros number value: 1024
				2: zeros number value: 2048
				3: zeros number value: 4096
				4: zeros number value: 8192
				5: zeros number value: 16384
				6: zeros number value: 32768
				7: zeros number value: 65535
 * @return NONE
 */
void AudioDAC_ZeroNumSet(DAC_MODULE DACModule,uint8_t sel);

/**
 * @brief  Use optimized DSM
 * @return NONE
 */
void AudioDAC_DsmOptimizeSet(void);

/**
 * @brief  Use original DSM
 * @return NONE
 */
void AudioDAC_DsmOriginSet(void);

/**
 * @brief  Audio DAC dither pow set
 * @param  DACModule which AudioDAC you will use
 * @param  DitherPow you will set this value to dither pow
 * @return NONE
 */
void AudioDAC_DitherPowSet(DAC_MODULE DACModule,uint32_t DitherPow);

/**
 * @brief  Audio DAC get dither pow
 * @param  DACModule which AudioDAC you will use
 * @return Dither Pow you used
 */
uint32_t AudioDAC_DitherPowGet(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC get sampler rate
 * @param  DACModule which AudioDAC you will use
 * @return Sample Rate you used
 */
uint32_t AudioDAC_SampleRateGet(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC sampler rate set
 * @param  DACModule which AudioDAC you will use
 * @param  SampleRate 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 
 * 					  88200, 96000, 176400, 192000 are supported.
 * @return NONE
 */
void AudioDAC_SampleRateSet(DAC_MODULE DACModule,uint32_t SampleRate);

/**
 * @brief  Audio DAC reset the reg data and mem data
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_Reset(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC clear mdac_load_error
 * @param  DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_ErrorFlagClear(DAC_MODULE DACModule);

/**
 * @brief  Audio DAC  mdac load error, when load next data ,but data is not ready
 * @param  DACModule which AudioDAC you will use
 * @return  ErrorFlag
 */
bool AudioDAC_ErrorFlagGet(DAC_MODULE DACModule);

/**
 * @brief Audio DAC fade time set
 * @param DACModule which AudioDAC you will use
 * @param FadeTime unit is ms
 * @return NONE
 */
void AudioDAC_FadeTimeSet(DAC_MODULE DACModule,uint8_t FadeTime);

/**
 * @brief Audio DAC fadein fadeout enable
 * @param DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_FadeEnable(DAC_MODULE DACModule);

/**
 * @brief Audio DAC fadein fadeout disable
 * @param DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_FadeDisable(DAC_MODULE DACModule);

/**
 * @brief Audio DAC Digital mute,DAC0 has left channel and right channel
 * @param DACModule which AudioDAC you will use
 * @param LeftMuteEn left channel for DAC0
 * @param RightMuteEn right channel for DAC0
 * @return NONE
 */
void AudioDAC_DigitalMute(DAC_MODULE DACModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief Audio DAC soft mute,DAC0 has left channel and right channel
 * @param DACModule which AudioDAC you will use
 * @param LeftMuteEn left channel for DAC0
 * @param RightMuteEn right channel for DAC0
 * @return NONE
 * @note You must have already used SysTickInit before using this API
 */
void AudioDAC_SoftMute(DAC_MODULE DACModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief Audio DAC volume set,DAC0 has left channel and right channel
 * @param DACModule which AudioDAC you will use
 * @param LeftVol left channel for DAC0,its scope is form 0 to 0x3fff
 * @param RightVol right channel for DAC0,its scope is form 0 to 0x3fff
 * @return NONE
 */
void AudioDAC_VolSet(DAC_MODULE DACModule, uint16_t LeftVol, uint16_t RightVol);

/**
 * @brief Audio DAC volume Get,DAC0 has left channel and right channel
 * @param DACModule which AudioDAC you will use
 * @param *LeftVol left channel for DAC0,its scope is form 0 to 0x3fff
 * @param *RightVol right channel for DAC0,its scope is form 0 to 0x3fff
 * @return NONE
 */
void AudioDAC_VolGet(DAC_MODULE DACModule,uint16_t* LeftVol, uint16_t* RightVol);

/**
 * @brief enable DAC clock
 * @param DACModule which AudioDAC you will use
 * @param Enable TRUE for enable, FALSE for disable
 * @return NONE
 */
void AudioDAC_ClkEnable(DAC_MODULE DACModule, bool Enable);

/**
 * @brief function reset for DAC
 * @param DACModule which AudioDAC you will use
 * @return NONE
 */
void AudioDAC_FuncReset(DAC_MODULE DACModule);

/**
 * @brief register reset for DAC, this will reset ALL REGISTER including DAC0
 * @param NONE
 * @return NONE
 */
void AudioDAC_RegReset(void);

/**
 * @brief VMID starts charging with programmed current, and power down BIAS
 * @param NONE
 * @return NONE
 */
void AudioDAC_VIMDControl(void);

/**
 * @brief BIAS power on/off control
 * @param OnOff TRUE for BIAS Power on, FALSE for BIAS Power off
 * @return NONE
 */
void AudioDAC_BIASPowerCtrl(bool OnOff);

/**
 * @brief control SCF_VREFP current for left/right channel
 * @param DACModule which AudioDAC you will use
 * @param VrefpL: form 0 to 0x3
 * @param VrefpR: form 0 to 0x3
 * @return NONE
 */
void AudioDAC_IBSelect(DAC_MODULE DACModule, uint8_t VrefpL, uint8_t VrefpR);

/**
 * @brief All Analog power on DAC
 * @param DACModel: DAC_Single 单端，DAC_Diff 差分
 * @param DAC_LoadStatus: DAC_NOLoad 不带负载，DAC_Load 带负载；DAC单端时该参数无效，DAC差分时且一般驱动差分耳机建议配置带负载模式
 * @param PVDDModel：PVDD16 使用1.6V电源 ; PVDD33 使用3.3V电源
 * @param DACEnergyModel：LowEnergy 低功耗模式; CommonEnergy 普通功耗模式
 * @param DACVcomModel：Direct Vcom直驱耳机模式; Disable：不使用Vcom
 * @note  使用PVDD16时不要使用单端模式
 * @return TRUE:sucess,FALSE:failure
 */
bool AudioDAC_AllPowerOn(DAC_Model DACModel, DAC_LoadStatus DACLoadStatus, PVDD_Model PVDDModel, DAC_EnergyModel DACEnergyModel, DAC_VcomModel DACVcomModel);

/**
 * @brief All Analog power on DAC in Fast mode
 * @param DACModel: DAC_Single 单端，DAC_Diff 差分
 * @param DAC_LoadStatus: DAC_NOLoad 不带负载，DAC_Load 带负载
 * @param PVDDModel：PVDD16 使用1.6V电源 ; PVDD33 使用3.3V电源
 * @param DACEnergyModel：LowEnergy 低功耗模式; CommonEnergy 普通功耗模式
 * @param DACVcomModel：Direct Vcom直驱耳机模式; Disable：不使用Vcom
 * @note  使用PVDD16时不要使用单端模式
 * @return TRUE:sucess,FALSE:failure
 */
bool AudioDAC_AllPowerOn_Fast(DAC_Model DACModel, DAC_LoadStatus DACLoadStatus, PVDD_Model PVDDModel, DAC_EnergyModel DACEnergyModel, DAC_VcomModel DACVcomModel);

/**
 * @brief All Analog power Down DAC
 * @note The digital DIG_EN cannot be turned off before PowerDown
 * @note You must have already used SysTickInit before using this API
 * @return TRUE:sucess,FALSE:failure
 */
bool AudioDAC_AllPowerDown(void);

/**
 * @brief All Analog power Down DAC in Fast mode
 * @note The digital DIG_EN cannot be turned off before PowerDown
 * @note You must have already used SysTickInit before using this API
 * @return TRUE:sucess,FALSE:failure
 */
bool AudioDAC_AllPowerDown_Fast(void);

/**
 * @brief Mute or Unmute SCF
 * @param DACModule which AudioDAC you will use
 * @param MuteL, channel to mute
 * @param MuteR, channel to mute
 * @return NONE
 */
void AudioDAC_SCFMute(DAC_MODULE DACModule, bool MuteL, bool MuteR);

/*
 * @brief  Audio DAC mdac EXT path enable
 * @param  DMIXMODE:    DMIXMODE0 = 0,//direct path data sel asdm data
 *						DMIXMODE3 = 3,//direct path data sel asdm data
 *  					DMIXMODE1 = 1,//direct path data sel i2s0 data
 *  					DMIXMODE2 = 2,//direct path data sel i2s1 data
 *
 * @param  EXTMODE:     EXTMODE0 = 0,//tx_dat mix direct path data
 *  					EXTMODE1 = 1,//L=L_int+L_ext,R=R_int+R_ext
 *  					EXTMODE2 = 2,//L=L_int+L_ext,R=R_int+L_ext
 *  					EXTMODE3 = 3,//L=L_int+L_ext,R=R_int+0
 * @return NONE
 */
void AudioDAC_ExternalEnable(DMIX_MODE DMIXMODE, EXT_MODE EXTMODE);

/**
 * @brief  Audio DAC mdac EXT path disable
 * @return NONE
 */
void AudioDAC_ExternalDisable(void);

void AudioDAC_DirectEnable(DMIX_MODE DMIXMODE);

void AudioDAC_DirectVolSet(uint16_t vol);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__DAC_H__

/**
 * @}
 * @}
 */
