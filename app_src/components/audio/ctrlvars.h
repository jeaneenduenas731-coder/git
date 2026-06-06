/**
 **************************************************************************************
 * @file    ctrlvars.h
 * @brief   Control Variables Definition
 * 
 * @author  Aissen Li
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 
#ifndef __CTRLVARS_H__
#define __CTRLVARS_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdint.h>
#include "audio_effect_library.h"
#include "app_config.h"
#include "bt_config.h"
#include "audio_core_api.h"
#include "sys.h"
#include "clk.h"
#include "timer.h"
#include "irqn.h"
#include "i2s_interface.h"
#include "rtos_api.h"

#define  MAX_MIC_DIG_STEP               (32)
#define  MAX_MUSIC_DIG_STEP             (32)
#define  MAX_MIC_EFFECT_DELAY_STEP      (32)

extern int16_t EqMode;
extern int16_t MicBassStep;
extern int16_t MicTrebStep;
extern int16_t MicVolume;
extern int16_t MusicBassStep;
extern int16_t MusicTrebStep;
extern int16_t MusicVolume;
extern int16_t ReverbStep;

typedef enum _EFFECT_MODE
{
	EFFECT_MODE_DEFAULT = 0,
	/**********uac mode****************/
	EFFECT_MODE_UAC,
	/**********two audio mode****************/
	EFFECT_MODE_TWO_AUDIO,
	/**********mic mode****************/
	EFFECT_MODE_MIC,
	/**********music mode**************/
	EFFECT_MODE_MUSIC,
	/**********micusbAI mode***********/
	EFFECT_MODE_MICUSBAI,
	/**********bypass mode*************/
	EFFECT_MODE_BYPASS,
	/**********hfp mode****************/
	EFFECT_MODE_HFP_AEC,
	/**********karaoke mode************/
    EFFECT_MODE_HunXiang,
    EFFECT_MODE_DianYin,
    EFFECT_MODE_MoYin,
    EFFECT_MODE_HanMai,
    EFFECT_MODE_NanBianNv,
    EFFECT_MODE_NvBianNan,
    EFFECT_MODE_WaWaYin,

	EFFECT_MODE_BIS,
	/**********tdm mode**************/
	EFFECT_MODE_TDM,

	EFFECT_MODE_COUNT,
    //User can add other effect mode
} EFFECT_MODE;

//实际有效的音效轮转模式数量
#if (BT_HFP_SUPPORT)
#define	EFFECT_MODE_NUM_ACTIVCE 	2
#else
#define	EFFECT_MODE_NUM_ACTIVCE 	1
#endif

typedef enum _REMIND_TYPE
{
    REMIND_TYPE_KEY,
	REMIND_TYPE_BACKGROUND, 

} REMIND_TYPE;

typedef enum _EQ_MODE
{
    EQ_MODE_FLAT,
	EQ_MODE_CLASSIC,	
	EQ_MODE_POP,
	EQ_MODE_ROCK,
	EQ_MODE_JAZZ,
	EQ_MODE_VOCAL_BOOST,
} EQ_MODE;


#define ANA_INPUT_CH_NONE    0
#define ANA_INPUT_CH_LINEIN1 1
#define ANA_INPUT_CH_LINEIN2 2
#define ANA_INPUT_CH_LINEIN3 3
#define ANA_INPUT_CH_LINEIN4 4
#define ANA_INPUT_CH_LINEIN5 5

//for ADC0 PGA      0x03
typedef struct _ADC0PGAContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
    uint16_t            pga_aux_l_show;
    uint16_t            pga_aux_r_show;
    uint16_t            pga_aux_l_en;
    uint16_t            pga_aux_r_en;
    uint16_t            pga_aux_l_gain;
    uint16_t            pga_aux_r_gain;
}ADC0PGAContext;

//for ADC0 DIGITAL  0x04
typedef struct _ADCDigitalContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
    uint16_t         	adc_channel_en;
	uint16_t  			adc_mute;
    uint16_t  			adc_dig_l_vol;
    uint16_t  			adc_dig_r_vol;
    uint16_t  			adc_sample_rate;
	uint16_t  			adc_lr_swap;
	uint16_t  			adc_hpc;
	uint16_t            adc_fade_time;
	uint16_t            adc_mclk_source;
	uint16_t            adc_dc_blocker_en;
}ADCDigitalContext;

typedef struct _ADCAGCContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
	uint16_t            adc_agc_mode;
	uint16_t            adc_agc_max_level;
	uint16_t            adc_agc_target_level;
	uint16_t            adc_agc_max_gain;
	uint16_t            adc_agc_min_gain;
	uint16_t            adc_agc_gainoffset;
	uint16_t            adc_agc_fram_time;
	uint16_t            adc_agc_hold_time;
	uint16_t            adc_agc_attack_time;
	uint16_t            adc_agc_decay_time;
	uint16_t            adc_agc_noise_gate_en;
	uint16_t            adc_agc_noise_threshold;
	uint16_t            adc_agc_noise_gate_mode;
	uint16_t            adc_agc_noise_time;
}ADCAGCContext;

typedef struct _ADC1PGAContext
{
	uint8_t	  			 sync_head;
	uint8_t	  			 len;
	uint16_t             pga_mic_show;
	uint16_t             pga_mic_mode;
    uint16_t             pga_mic_enable;
	uint16_t             pga_mic_gain;
}ADC1PGAContext;

typedef struct _DACContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
	uint16_t            dac_enable;
	uint16_t            dac_sample_rate;
	uint16_t            dac_dig_mute;
	uint16_t            dac_dig_l_vol;
	uint16_t            dac_dig_r_vol;
	uint16_t            dac_dither;
	uint16_t            dac_scramble;
	uint16_t            dac_out_mode;
	uint16_t            dac_pause;
	uint16_t            dac_sample_edge_mode;
	uint16_t            dac_SCF_mute;
	uint16_t            dac_fade_time;
	uint16_t            dac_zero_num;
	uint16_t            dac_mclk_source;
}DACContext;

typedef struct _I2SContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
	uint16_t            i2s_tx_enable;
	uint16_t            i2s_rx_enable;
	uint16_t            i2s_sample_rate;
	uint16_t            i2s_mclk_source;
	uint16_t            i2s_master_slave;
	uint16_t            i2s_word_length;
	uint16_t            i2s_stereo_mono;
	uint16_t            i2s_fade_time;
	uint16_t            i2s_data_format;
	uint16_t            i2s_bclk_invert;
	uint16_t            i2s_lrclk_invert;
}I2SContext;

typedef struct _SPDIFContext
{
	uint8_t	  			sync_head;
	uint8_t	  			len;
	uint16_t            spdif0_enable;
	uint16_t            spdif0_sample_rate;
	uint16_t            spdif0_channel_mode;
	uint16_t            spdif0_io_mode;
	uint16_t            spdif0_lock_status;
	uint16_t            spdif1_enable;
	uint16_t            spdif1_sample_rate;
	uint16_t            spdif1_channel_mode;
	uint16_t            spdif1_io_mode;
	uint16_t            spdif1_lock_status;
}SPDIFContext;

typedef struct _HardwareConfigContext
{
	//for ADC0 PGA      0x03
	ADC0PGAContext		ADC0PGACt;
    //for ADC0 DIGITAL  0x04
	ADCDigitalContext	ADC0DigitalCt;

	//for AGC0 ADC0     0x05(just for sync but not used)
	uint16_t			ADC0AGC_Reserved;

	//for ADC1 PGA      0x06
	ADC1PGAContext		ADC1PGACt;

	//for ADC1 DIGITAL  0x07
	ADCDigitalContext	ADC1DigitalCt;

	//for AGC1  ADC1    0x08
	ADCAGCContext		ADC1AGCCt;

	//for DAC0          0x09
	DACContext			DAC0Ct;

	//for DAC1          0x0a
	uint16_t            DAC1_Reserved;

	//not used
	I2SContext			I2S0Ct;
	I2SContext			I2S1Ct;
	SPDIFContext		SPDIFCt;
}HardwareConfigContext;

typedef enum _AutoRefreshType
{
	AutoRefresh_NONE = 0,			//不需要刷新
	AutoRefresh_ALL_PARA = 1,		//刷新0x00开始的所有参数
	AutoRefresh_ALL_EFFECTS_PARA = 2,//刷新0x81开始所有音效参数

	//3-255 刷新指定地址的参数
	AutoRefresh_0x03_PARA,
	AutoRefresh_0x04_PARA,
}AutoRefreshType;

//-----system var--------------------------//
typedef struct _ControlVariablesContext
{
	//for system control 0x01
	//for System status 0x02
	AutoRefreshType 			AutoRefresh;
#ifdef CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN
	MessageHandle				RefreshMsgHandle;
#endif
	uint8_t 					EffectRefresh;

	HardwareConfigContext HwCt;

    //for system define

	uint16_t            sample_rate_index;
	uint32_t            sample_rate;

	#ifdef CFG_FUNC_DETECT_MIC_SEG_EN
	uint8_t             MicSegment;
	#endif
	#ifdef CFG_FUNC_DETECT_PHONE_EN
	uint8_t             EarPhoneOnlin;
	#endif
}ControlVariablesContext;

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
typedef struct _SyncModuleContext
{
	uint64_t gI2sBclkCnt;
	uint64_t gDacMclkCnt;
	uint8_t  gRefreshFlag;
	uint8_t  gClearDoneFlag;
	uint64_t gMClkFromBCLK;
	uint64_t gMClkFromDPLL;
	uint32_t gSyncTimerCnt;
	uint16_t defVal_bak;
	uint16_t dat;
	uint16_t clkRatio;
}SyncModuleContext;

#define SYNC_TIMER_INDEX        TIMER4  //定时器
#define SYNC_TIMER_OUT_VALUE    100000 //定时器的间隔值，单位us
#define BCLK_MCLK_RATIO         4       //MCLK和BCLK的比值， 48K与12.288M是4， 44.1与11.2896M是4.。。
#define MCLK_DIV_VALUE          20

extern SyncModuleContext gSyncModule;
#endif
extern ControlVariablesContext gCtrlVars;

extern const uint16_t HPCList[3];

void CtrlVarsInit(void);
void DefaultParamgsInit(void);
void Line3MicPinSet(void);
void UsbLoadAudioMode(uint16_t len,uint8_t *buff);
void AudioLineSelSet(int8_t ana_input_ch);
void AudioAnaChannelSet(int8_t ana_input_ch);
void AudioLine3MicSelect(void);

//音效参数更新之后同步更新模拟Gain和数字Vol
//只更新增益相关参数，其他参数比如通道选择不会同步更新，必须由SDK代码来实现
void AudioCodecGainUpdata(void);

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
void SyncModule_Init(void);
void SyncModule_Reset(void);
void SyncModule_Get(void);
void SyncModule_Process(void);
#endif

extern const int16_t DigVolTab_32[32];
extern const int16_t DigVolTab_16[16];


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__CTRLVARS_H__

