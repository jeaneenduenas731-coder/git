/**
 **************************************************************************************
 * @file    bluetooth_sniff.c
 * @brief   bluetooth sniff相关函数功能接口
 *
 * @author  KK
 * @version V1.0.0
 *
 * $Created: 2021-4-18 18:00:00$
 *
 * @Copyright (C) Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>
#include "type.h"
#include "app_config.h"
#include "bt_config.h"
//driver
#include "chip_info.h"
#include "debug.h"
//middleware
#include "main_task.h"
#include "bt_manager.h"
//application
#include "bt_app_sniff.h"
#include "bt_app_connect.h"
#include "dac_interface.h"
#include "dma.h"
#include "powercontroller.h"
#include "clock_config.h"
#include "clk.h"
#include "audio_core_service.h"
#include "audio_adc.h"

/**
 * @brief  sniff休眠应用处理
 * @param  sleepTime
 * @return void
 * @note   sniff休眠的时候应用会调用该函数，注意调用函数时，中断已经关闭，
 */
void SendDeepSleepMsg(void)
{
#ifdef BT_SNIFF_ENABLE
	extern void BtDeepSleepForUsr(void);

	BtDeepSleepForUsr();
#endif
}
/**
 * @brief  sniff的低功耗模式
 * @param  void
 * @return void
 * @note   手机在没有任务的时候，会设置设备进入低功耗可以睡眠的模式，可以在这个函数中关闭不需要的外设,CORE和SYS时钟用48MHZ
 */
void BtSniffLowPower(void)
{
	PauseAuidoCore();
#ifdef CHIP_USE_DCDC
	ldo_switch_to_dcdc(5); // 电压高RF性能好，最求RF功率性能，建议设置为1.7V；低功耗配置建议设置为1.4V
#else
	Power_LDO16Config(0);
#endif
#ifdef CFG_RES_ADC_KEY_SCAN
	Power_LDO11DConfig(PWD_LDO11_LVL_1V05); // 降低到0.95V  VDD使用SarADC功能，最低电压只能到1.05V
#else
	Power_LDO11DConfig(PWD_LDO11_LVL_0V95); // 降低到0.95V  VDD使用SarADC功能，最低电压只能到1.05V
#endif
	Power_LDO33DConfig(3);
#if CFG_RES_MIC_SELECT
	AudioADC_DeInit(ADC1_MODULE);
	AudioCoreSourceDisable(MIC_SOURCE_NUM);
	AudioADC_HighPassFilterSet(ADC1_MODULE, FALSE); // SREG_ASDM1_CTRL.ASDM1_HPF_EN = 0;
	AudioADC_LREnable(ADC1_MODULE, FALSE, FALSE);// disbale mic channel
#endif

#ifdef CFG_RES_AUDIO_DAC0_EN
	AudioCoreSinkDisable(AUDIO_DAC0_SINK_NUM);
	AudioDAC_Disable(DAC0);
	AudioDAC_FuncReset(DAC0);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_DAC0_TX);
	AudioDAC_AllPowerDown();
#endif
	Clock_SysClkDivSet(SYS_CLK_DIV_SNIFF);
	Clock_CoreClkDivSet(SYS_CORE_CLK_DIV_SNIFF);
	SystemTimerInit();
	//APP_DBG("enter sniff low power\n");//
}

/**
 * @brief  正常功耗模式
 * @param  void
 * @return void
 * @note   手机在有播放音乐或者打电话的时候，会设置设备进入正常功耗的模式
 */

void BtSniffNormalPower(void)
{
	Clock_SysClkDivSet(SYS_CLK_DIV);
	Clock_CoreClkDivSet(SYS_CORE_CLK_DIV);
#ifdef CHIP_USE_DCDC
	ldo_switch_to_dcdc(3); // 3-1.6V Default:1.6V
#else
	Power_LDO16Config(1);
#endif
	Power_LDO33DConfig(6);
	Power_LDO11DConfig(PWD_LDO11_LVL_1V10); // 升回1.1V
	AudioCoreServiceResume();
#ifdef CFG_RES_AUDIO_DAC0_EN
	uint16_t BitWidth;
#ifdef	CFG_AUDIO_WIDTH_24BIT
		BitWidth = 24;
#else
		BitWidth = 16;
#endif
	extern const DACParamCt DACDefaultParamCt;
	extern MainAppContext	mainAppCt;
	AudioDAC_Init((DACParamCt *)&DACDefaultParamCt,AudioCoreMixSampleRateGet(DefaultNet),BitWidth, (void*)mainAppCt.DACFIFO, mainAppCt.DACFIFO_LEN, NULL, 0);
	AudioCoreSinkEnable(DAC0);
#endif
#if CFG_RES_MIC_SELECT
	AudioADC_DigitalInit(ADC1_MODULE, AudioCoreMixSampleRateGet(DefaultNet),ADC_WIDTH_24BITS,(void*)mainAppCt.ADCFIFO, AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2);
	AudioCoreSourceEnable(MIC_SOURCE_NUM);
#endif
	SystemTimerInit();
}

void BtChangePowerMode(uint8_t mode)
{
	if(mode == 2)
	{
		 BtSniffLowPower();
	}
	else
	{
		BtSniffNormalPower();
	}

}



