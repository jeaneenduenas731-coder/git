/**
 *******************************************************************************
 * @file    clk.h
 * @brief	Clock driver interface
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2015-11-05 10:46:11$
 *
 * @Copyright (C) 2015, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */
 
 
/**
 * @addtogroup CLOCK
 * @{
 * @defgroup clk clk.h
 * @{
 */
 
#ifndef __CLK_H__
#define __CLK_H__
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "spi_flash.h"
/**
 * CLK module switch macro define
 */
typedef enum __CLOCK_MODULE1_SWITCH
{
	MCLK_PLL_CLK0_EN = (1 << 0),		/**<MCLK PLL0 module clk switch */
	MCLK_PLL_CLK1_EN = (1 << 1),		/**<MCLK PLL1 module clk switch */
	FLASH_CONTROL_PLL_CLK_EN = (1 << 2),/**<Flash Control PLL module clk switch */
	USB_UART_PLL_CLK_EN = (1 << 3),		/**<USB/UART PLL module clk switch */
	AUDIO_DAC0_CLK_EN = (1 << 4),		/**<Audio DAC port0 module clk switch */
	AUDIO_ADC0_CLK_EN = (1 << 5),		/**<Audio ADC port0 module clk switch */
	I2S0_CLK_EN = (1 << 6),				/**<I2S port0 module clk switch */
	FLASH_CONTROL_CLK_EN = (1 << 7),	/**<Flash control module clk switch */
	USB_CLK_EN = (1 << 8),           	/**<USB module clk switch */
	UART0_CLK_EN = (1 << 9),           /**<UART0 module clk switch */
	DMA_CLK_EN = (1 << 10),   			/**<DMA control module clk switch */
	FFT_CLK_EN = (1 << 11),            	/**<FFT module clk switch */
	ADC_CLK_EN = (1 << 12),				/**<sarADC module clk switch */
	EFUSE_CLK_EN = (1 << 13),          	/**<EFUSE module clk switch */
	I2C_CLK_EN = (1 << 14),          	/**<I2C module clk switch */
	SPIM_CLK_EN = (1 << 15),          	/**<SPI master module clk switch */
	SDIO_CLK_EN = (1 << 16),          	/**<SDIO module clk switch */
	TIMER1_CLK_EN = (1 << 17),          /**<TIMER1 module clk switch */
	TIMER2_CLK_EN = (1 << 18),          /**<TIMER2 module clk switch */
	TIMER3_CLK_EN = (1 << 19),          /**<TIMER3 module clk switch */
	TIMER4_CLK_EN = (1 << 20),          /**<TIMER4 module clk switch */
	TIMER5_CLK_EN = (1 << 21),          /**<TIMER5 module clk switch */
	TIMER6_CLK_EN = (1 << 22),          /**<TIMER6 module clk switch */
	BTDM_HCLK_EN = (1 << 23),			/**< module clk switch */
	USB_REG_CLK_EN = (1 << 24),			/**< module clk switch */
	SDIO_REG_CLK_EN = (1 << 25),		/**< module clk switch */
	SPIM_REG_CLK_EN = (1 << 26),			/**< module clk switch */
	UART0_REG_CLK_EN = (1 << 27),		/**< module clk switch */
	FPU_CLK_EN = (1 << 28),				/**< module clk switch */
	BTDM_24M_CLK_EN = (1 << 29),		/**< module clk switch */
	CAN_24M_CLK_EN = (1 << 30),			/**< module clk switch */
	MDM_PLL_CLK_EN = (1 << 31),			/**< module clk switch */
	DEFAULT_MODULE1_CLK_SWITCH = (0x10002080), /**< default module clk switch */
	ALL_MODULE1_CLK_SWITCH = (0xFFFFFFFF),/**<all module clk SWITCH*/
} CLOCK_MODULE1_SWITCH;

typedef enum __CLOCK_MODULE2_SWITCH
{
	BT32K_CLK_EN = (1 << 0),			/**< module clk switch */
	BT_OR_PMU_32K_CLK_EN = (1 << 1),	/**< module clk switch */
	PMU_32K_CLK_EN = (1 << 2),			/**< module clk switch */
//	reserved_CLK2_EN = (1 << 3),		/**< module clk switch */
	MCLK0_APLL_CLK_EN = (1 << 4),		/**<MCLK0 APLL module clk switch */
	MCLK1_APLL_CLK_EN = (1 << 5),		/**<MCLK1 APLL module clk switch */
	SPDIF_APLL_CLK_EN = (1 << 6),  		/**<SPDIF APLL module clk switch */
	USB_APLL_CLK_EN = (1 << 7),         /**<USB module APL clk switch */
	MDM_APLL_CLK_EN = (1 << 8),			/**< module clk switch */
//	reserved_CLK2_EN = (1 << 9),		/**< module clk switch */
//	reserved_CLK2_EN = (1 << 10),		/**< module clk switch */
//	reserved_CLK2_EN = (1 << 11),		/**< module clk switch */
	MCLK0_DPLL_CLK_EN = (1 << 12),		/**<MCLK0 DPLL module clk switch */
	MCLK1_DPLL_CLK_EN = (1 << 13),		/**<MCLK1 DPLL module clk switch */
	SPDIF_DPLL_CLK_EN = (1 << 14),  	/**<SPDIF DPLL module clk switch */
	USB_DPLL_CLK_EN = (1 << 15),        /**<USB module DPLL clk switch */
	MDM_DPLL_CLK_EN = (1 << 16),		/**< module clk switch */
	DEFAULT_MODULE2_CLK_SWITCH = (0x1F1F7), /**< default module clk switch */
	ALL_MODULE2_CLK_SWITCH = (0x1F1F7),	/**<all module clk SWITCH*/
} CLOCK_MODULE2_SWITCH;

typedef enum __CLOCK_MODULE3_SWITCH
{
	SPDIF_PLL_CLK_EN = (1 << 0),		/**< module clk switch */
	SPDIF_CLK_EN = (1 << 1),			/**< module clk switch */
	AUDIO_ADC1_CLK_EN = (1 << 2),		/**<Audio ADC port1 module clk switch */
	I2S1_CLK_EN = (1 << 3),				/**<I2S port1 module clk switch */
	UART1_CLK_EN = (1 << 4),           /**<UART1 module clk switch */
	SPIS_REG_CLK_EN = (1 << 5),			/**< module clk switch */
	UART1_REG_CLK_EN = (1 << 6),		/**< module clk switch */
	SPDIF0_REG_CLK_EN = (1 << 7),		/**< module clk switch */
	MDM_12M_CLK_EN = (1 << 8),			/**< module clk switch */
	MDM_24M_CLK_EN = (1 << 9),			/**< module clk switch */
	MDM_48M_CLK_EN = (1 << 10),			/**< module clk switch */
	RTC32K_CLK_EN = (1 << 11),			/**< module clk switch */
	SPDIF1_CLK_EN = (1 << 12),			/**< module clk switch */
	SPDIF1_REG_CLK_EN = (1 << 13),		/**< module clk switch */
	BTDM_LP_CLK_EN = (1 << 14),			/**< module clk switch */
	I2S0_REG_CLK_EN = (1 << 15),		/**< module clk switch */
	I2S1_REG_CLK_EN = (1 << 16),		/**< module clk switch */
	MDAC_REG_CLK_EN = (1 << 17),		/**< module clk switch */
	TIMER7_CLK_EN = (1 << 18),          /**<TIMER7 module clk switch */
	TIMER8_CLK_EN = (1 << 19),          /**<TIMER8 module clk switch */
	DEFAULT_MODULE3_CLK_SWITCH = (0x3E080), /**< default module clk switch */
	ALL_MODULE3_CLK_SWITCH = (0xFFFFF),	/**<all module clk SWITCH*/
} CLOCK_MODULE3_SWITCH;


typedef enum __CLK_MODE{
	RC_CLK_MODE,	/*RC 12M source*/
	PLL_CLK_MODE,	/*DPLL source*/
	APLL_CLK_MODE,	/*APLL source*/
	SYSTEM_CLK_MODE /*System Clock*/
}CLK_MODE;

typedef enum _CLK_32K_MODE{
	HOSC_DIV_32K_CLK_MODE = 0,	/*HOSC DIV source,OSC24M_or_RC12M_DIV_32k_clk*/
	RC_CLK32_MODE,			/*RC32K source,bt_32k_clk */
	PMU_32K_MODE,			/*pmu_32k_clk*/
	LOSC_32K_MODE,			/*LOSC 32K source,RTC32K*/
}CLK_32K_MODE;


typedef enum __PLL_CLK_INDEX{
	PLL_CLK_1 = 0,	/**PLL分频时钟器1,软件配置生成为11.2896M，软件可以修改*/
	PLL_CLK_2		/**PLL分频时钟器2,软件配置生成为12.288M，软件可以修改*/
}PLL_CLK_INDEX;

typedef enum __AUDIO_MODULE{
	AUDIO_DAC0,
	AUDIO_ADC0,
	AUDIO_ADC1,
	AUDIO_I2S0,
	AUDIO_I2S1,
}AUDIO_MODULE;

typedef enum __MCLK_CLK_SEL{
	PLL_CLOCK1 = 0,	/**PLL分频1时钟，软件配置PLL分频时钟生产成频率为11.2896M，或者其他频率*/
	PLL_CLOCK2,		/**PLL分频2时钟，软件配置PLL分频时钟生产成频率为12.288M，或者其他频率*/
	GPIO_IN0,		/**GPIO MCLK in0输入*/
	GPIO_IN1,       /**GPIO MCLK in1输入*/
}MCLK_CLK_SEL;


typedef enum __CLOCK_GPIO_Port//输出时钟观测端口
{
	GPIO_CLK_OUT_A0,		//GPIO A0   clk1_out_0
	GPIO_CLK_OUT_A1,		//GPIO A1   clk0_out_0
	GPIO_CLK_OUT_A29,		//GPIO A29  clk0_out_1
	GPIO_CLK_OUT_B6,		//GPIO B6  	clk0_out_2
}CLK_GPIO_Port;

typedef enum __CLOCK_OUT_MODE
{
	CLK0_ETTING_NONE = 0,	//0:   none
	MCLK_PLL_CLK0_DIV,	//1:  clk0   mclk_pll_clk0_div
	FSHC_CLK,			//2:  clk0    fshc_clk
	USB_CLK,			//3:  clk0    usb_clk
	APB_CLK,			//4:  clk0    apb_clk
	SPIM_CLK,			//5:  clk0    spim_clk
	SDIO_CLK,			//6:  clk0    sdio_clk
	RC12M_CLK,			//7:  clk0    rc12M_clk
	BTDM_HCLK,			//8:  clk0    btdm_hclk
	BTDM_24M_CLK,		//9:  clk0    btdm_24m_clk
	BT_PMU_32K_CLK,		//10: clk0    bt_or_pmu_32k_clk
	DPLL_CLK_DIV,		//11: clk0    pll_clk_div
	SPDIF_CLK,			//12: clk0    spdif_clk
	CODEC_CLK_IN,		//13: clk0    codec_clk_in
	MDACK0_CLK_SW,		//14: clk0    mdac0_clk_sw
	ASDM0_CLK_SW,		//15: clk0    asdm0_clk_sw
	ASDM1_CLK_SW,		//16: clk0    asdm1_clk_sw
	APLL_CLK_SW,		//17: clk0    apll_clk_div
	LOSC_32K,			//18: clk0    LOSC32K
	MDM_24M_CLK,		//19: clk0    mdm_24m_clk
	OTG_1KHz_CLK= 0x21,	    //1:   clk1  otg1khz_clk
	SPDIF_BLOCK_FLAG ,	    //2:   clk1  spdif_block_flag
	SPDIF_FRAME_FLAG,		//3:   clk1  spdif_frame_flag
	CLK_12M_MDM_MUX,		//4:   clk1  clk_12m_mdm_mux
	CLK_48M_MDM_MUX,		//5:   clk1  clk_48m_mdm_mux
}CLOCK_OUT_MODE;

typedef enum __CLOCK_SYNC_CTRL_MODULE{
	I2S_BCLK_SEL,
	MDAC_MCLK_SEL,
	START_EN,
	UPDATE_DONE_CLR,
	UPDATE_EN,
	UPDATE_DONE,
}CLOCK_SYNC_CTRL_MODULE;

typedef enum __CLOCK_CLK_CNT_MODULE{
	I2S_BCLK_CNT0,
	I2S_BCLK_CNT1,
	MDAC_MCLK_CNT0,
	MDAC_MCLK_CNT1,
}CLOCK_CLK_CNT_MODULE;

typedef enum __CLOCK_RC32K_MODULE{
	BT_32K_CLK,
	PMU_32K_CLK,
	reserve,
	LOSC_32K_CLK,//RTC
}CLOCK_RC32K_MODULE;

//建议MCLK0 配置为11.2896M，MCLK1配置为12.288M
#define		AUDIO_PLL_CLK1_FREQ		11289600//PLL1,11.2896MHz
#define		AUDIO_PLL_CLK2_FREQ		12288000//PLL2,12.288MHz
#define 	IsSelectMclkClk1(freq)  ((AUDIO_PLL_CLK1_FREQ%freq) == 0)


/**
 * @brief	系统参考时钟源配置选择
 * @param	IsOsc TURE：晶体时钟；FALSE：XIN端口外灌方波时钟
 * @param   Freq  系统参考时钟的工作频率：32.768K,1M,2M,...40M,单位HZ
 * @return	无
 * @note	如果使用PLL时钟则必须要先调用该函数
 */
void Clock_Config(bool IsOsc, uint32_t Freq);

/**
 * @brief	获取Core工作的时钟频率
 * @param	无
 * @return	Core工作频率
 * @note    Core时钟和系统时钟可以同频，此时最高运行120MHz
 */
uint32_t Clock_CoreClockFreqGet(void);

/**
 * @brief  设置Core时钟分频系数
 * @param  DivVal [1-256]，Freq = Fpll/Div
 * @return 无
 * @note    Core时钟和系统时钟可以同频，此时最高运行144MHz
 */
void Clock_CoreClkDivSet(uint32_t DivVal);

/**
 * @brief   获取Core分频系数
 * @param   无
 * @return  系统分频系数[0-255]
 * @note    Core时钟和系统时钟可以同频，此时最高运行144MHz
 */
uint32_t Clock_CoreClkDivGet(void);

/**
 * @brief	获取系统工作的时钟频率
 * @param	无
 * @return	系统工作频率
 * @note    Core时钟和系统时钟可以同频，此时最高运行144MHz
 */
uint32_t Clock_SysClockFreqGet(void);

/**
 * @brief   设置系统时钟分频系数
 * @param   DivVal [0-8]，0关闭分频功能, 1不分频
 * @return  无
 * @note    Core时钟和系统时钟可以同频，此时最高运行144MHz
 */
void Clock_SysClkDivSet(uint32_t DivVal);
    
/**
 * @brief   获取系统时钟分频系数
 * @param   NONE
 * @return  系统分频系数[1-8]
 */
uint32_t Clock_SysClkDivGet(void);

/**
 * @brief   设置APB总线分频系数，基于系统总线分频
 * @param   DivVal [2-14]
 * @return  无
 */
void Clock_ApbClkDivSet(uint32_t DivVal);

/**
 * @brief   获取APB总线分频系数，基于系统总线分频
 * @param   无
 * @return  APB总线分频系数[2-14]
 * @note	APB时钟不能大于37.5MHz
 */
uint32_t Clock_ApbClkDivGet(void);

/**
 * @brief   设置USB时钟分频系数，基于PLL时钟分频
 * @param   DivVal [2-16]
 * @return  无
 * #note	
 */
void Clock_USBClkDivSet(uint32_t DivVal);

/**
 * @brief   获取USB时钟分频系数，基于PLL时钟分频
 * @param   无
 * @return  USB时钟分频系数[2-16]
 */
uint32_t Clock_USBClkDivGet(void);

/**
 * @brief  	设置SPI master工作时钟分频系数，从系统时钟分频
 * @param  	DivVal 分频系数[2-15]
 * @return  无
 * @note	SPIM时钟不高于120M
 */
void Clock_SPIMClkDivSet(uint32_t DivVal);

/**
 * @brief  获取SPI master工作时钟分频系数，从系统时钟分频
 * @param  无
 * @return  分频系数[2-15]
 */
uint32_t Clock_SPIMClkDivGet(void);

/**
 * @brief   设置SDIO0工作时钟分频系数，从系统时钟分频
 * @param   DivVal 分频系数[2-15]
 * @return  无
 * @note	SDIO时钟不能大于60MHz
 */
void Clock_SDIOClkDivSet(uint32_t DivVal);

/**
 * @brief  获取SDIO0工作时钟分频系数，从系统时钟分频
 * @param  无
 * @return  分频系数[2-15]
 */
uint32_t Clock_SDIOClkDivGet(void);

/**
 * @brief	设置OSC分频系数，分频之后的时钟提供给RTC使用
 * @param	DivVal 分频系数[1-2048]。
 * @return  无
 */
void Clock_OSCClkDivSet(uint32_t DivVal);

/**
 * @brief	获取OSC分频系数，分频之后的时钟提供给RTC使用
 * @param	无
 * @return  OSC分频系数[1-2048]
 */
uint32_t Clock_OSCClkDivGet(void);

/**
 * @brief   设置ADC0工作时钟分频系数，基于Mclk时钟分频
 * @param   DivVal 分频系数[0-3]
 * @return  无
 * @note	//mclk_frequency/(N+1)
 */
void Clock_ADC0ClkDivSet(uint32_t DivVal);

/**
 * @brief  获取ADC1工作时钟分频系数，基于Mclk时钟分频
 * @param  无
 * @return  分频系数[0-3]
 */
uint32_t Clock_ADC1ClkDivGet(void);

/**
 * @brief   设置ADC1工作时钟分频系数，基于Mclk时钟分频
 * @param   DivVal 分频系数[0-3]
 * @return  无
 * @note	//mclk_frequency/(N+1)
 */
void Clock_ADC1ClkDivSet(uint32_t DivVal);

/**
 * @brief  获取ADC0工作时钟分频系数，基于Mclk时钟分频
 * @param  无
 * @return  分频系数[0-3]
 */
uint32_t Clock_ADC0ClkDivGet(void);

/**
 * @brief	设置pll工作频率,等待pll lock
 * @param	PllFreq pll频率,单位KHz[120000K-240000K]
 * @return  PLL锁定情况  TRUE:按设定目标频率锁定
 */
bool Clock_PllLock(uint32_t PllFreq);

/**
 * @brief	设置APLL工作频率,等待APLL lock,输出频率为设置频率，没有分频
 * @param	APllFreq pll频率,单位KHz[120000K-240000K]
 * @return  APLL锁定情况  TRUE:按设定目标频率锁定
 */
bool Clock_APllLock(uint32_t PllFreq);


bool Clock_APllLock_Pro(uint32_t ApllLockFreq,uint8_t DIVMODE,uint8_t VCO_ISEL,uint8_t CP_ISEL);

/**
 * @brief	获取RC12M频率
 * @param	IsReCount 是否再次获取硬件计数器值。TRUE：再次启动硬件计数器。FALSE：获取上次记录值。
 * @return  rc频率，单位Hz
 */	
uint32_t Clock_RcFreqGet(bool IsReCount);

/**
 * @brief	获取RC132k频率
 * @param	IsReCount 是否再次获取硬件计数器值。TRUE：再次启动硬件计数器。FALSE：获取上次记录值。
 * @return  rc频率，单位Hz
 */
uint32_t Clock_RC32KFreqGet(bool IsReCount, CLOCK_RC32K_MODULE clk);

/**
 * @brief	获取pll频率
 * @param	无
 * @return  pll频率
 * @note    该函数调用需要应用确保pll已经lock，否则读数错误
 */
uint32_t Clock_PllFreqGet(void);

/**
 * @brief	获取pll频率
 * @param	Win 计数器窗口【3:2048】
 * @return  无
 */
void Clock_HOSCCntWindowSet(uint32_t Win);

/**
 * @brief	关闭rc频率硬件自动更新功能
 * @param	无
 * @return  无
 */
void Clock_HOSCFreqAutoCntDisable(void);

/**
 * @brief	启动rc频率硬件自动更新功能
 * @param	无
 * @return  无
 */
void Clock_HOSCFreqAutoCntStart(void);

/**
 * @brief	获取rc频率（硬件自动更新）
 * @param	无
 * @return  无
 */
uint32_t Clock_HOSCFreqAutoCntGet(void);

/**
 * @brief	设置pll工作频率,快速锁定模式，等待pll lock
 * @param	PllFreq pll频率,单位KHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @param	FC [0-2]
 * @param	Slope [0-16777216]
 * @return  PLL锁定情况  TRUE:按设定目标频率锁定
 */
bool Clock_PllQuicklock(uint32_t PllFreq, uint8_t K1, uint8_t OS, uint32_t NDAC, uint32_t FC, uint32_t Slope);

/**
 * @brief	设置pll工作频率,开环锁定模式(不需要晶体，无自校准)
 * @param	PllFreq pll频率,单位KHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @param	FC [0-2]
 * @return  无
 */
void Clock_PllFreeRun(uint32_t PllFreq, uint32_t K1, uint32_t OS, uint32_t NDAC, uint32_t FC);

/**
 * @brief	pll free run 一般只需要调此函数即可
 * @param	无
 * @return  TRUE, 配置成功
 */
bool Clock_PllFreeRunEfuse(void);

/**
 * @brief	DPLL USB免晶体时钟功能配置
 * @param	目标 DPLL 时钟频率
 * @return  无
 */
void Clock_USBCrystaFreeSet(uint32_t target_freq);

/**
 * @brief	USB免晶体时钟校准处理
 * @param	无
 * @return  校准状态
 */
uint8_t Clock_USBCrystaFreeAdjustProcess(void);

/**
 * @brief	pll模块关闭
 * @param	无
 * @return  无
 */
void Clock_PllClose(void);

/**
 * @brief	AudioPll模块关闭
 * @param	无
 * @return  无
 */
void Clock_APllClose(void);

/**
 * @brief	选择系统工作时钟
 * @param	ClkMode, RC_CLK_MODE: RC12M时钟; PLL_CLK_MODE:pll时钟;
 * @return  是否成功切换系统时钟，TRUE：工程切换；FALSE：切换时钟失败。
 * @note    系统时钟工作于pll时钟时，为pll时钟的2分频
 */
bool Clock_SysClkSelect(CLK_MODE ClkMode);

/**
 * @brief	Uart模块时钟选择,pll时钟还是RC时钟
 * @param	ClkMode 时钟源选择 
 *   @arg	RC_CLK_MODE: RC时钟;
 *   @arg	PLL_CLK_MODE:DPLL时钟（PLL分频之后）
 *   @arg	APLL_CLK_MODE:APLL时钟（APLL分频之后）
 * @return  无
 * @note	注意该时钟源如果选择PLL或者APLL时钟，是和USB时钟源同源
 */
void Clock_UARTClkSelect(CLK_MODE ClkMode);

/**
 * @brief	Timer5模块时钟选择,系统时钟时钟还是RC时钟
 * @param	ClkMode SYSTEM_CLK_MODE: 系统时钟; RC_CLK_MODE:RC 12M时钟
 * @return  无
 */
void Clock_Timer5ClkSelect(CLK_MODE ClkMode);

/**
 * @brief	BTDM模块时钟选择,系统时钟时钟还是RC时钟
 * @param	ClkMode BTDM模块
 *   @arg	HOSC_DIV_32K_CLK_MODE: 24M晶体或者RC12M时钟分频到32K;
 *   @arg	RC_CLK32_MODE: RC32K source,bt_32k_clk
 *   @arg	PMU_32K_MODE: pmu_32k_clk
 *   @arg	LOSC_32K_MODE: LOSC 32K source,RTC32K
 * @return  无
 */
void Clock_BTDMClkSelect(CLK_32K_MODE ClkMode);

/**
 * @brief	Spdif模块时钟选择,DPLL时钟还是APLL时钟
 * @param	ClkMode	时钟源选择
 *   @arg	PLL_CLK_MODE:DPLL时钟（PLL分频之后）
 *   @arg	APLL_CLK_MODE:APLL时钟（APLL分频之后）
 * @return  无
 */
void Clock_SpdifClkSelect(CLK_MODE ClkMode);


/**
 * @brief	USB模块时钟选择,DPLL时钟还是APLL时钟
 * @param	ClkMode	时钟源选择
 *   @arg	PLL_CLK_MODE:DPLL时钟（PLL分频之后）
 *   @arg	APLL_CLK_MODE:APLL时钟（APLL分频之后）
 * @return  无
 * @note	时钟频率不要超过60M，一般设置为60M
 */
void Clock_USBClkSelect(CLK_MODE ClkMode);

/**
 * @brief	模块时钟使能
 * @param	ClkSel 模块源，根据CLOCK_MODULE1_SWITCH选择
 * @return  无
 */
void Clock_Module1Enable(CLOCK_MODULE1_SWITCH ClkSel);

/**
 * @brief	模块时钟禁能
 * @param	ClkSel 模块源，根据CLOCK_MODULE1_SWITCH选择
 * @return  无
 */
void Clock_Module1Disable(CLOCK_MODULE1_SWITCH ClkSel);

/**
 * @brief	模块时钟使能
 * @param	ClkSel 模块源，根据CLOCK_MODULE2_SWITCH选择
 * @return  无
 */
void Clock_Module2Enable(CLOCK_MODULE2_SWITCH ClkSel);

/**
 * @brief	模块时钟禁能
 * @param	ClkSel 模块源，根据CLOCK_MODULE2_SWITCH选择
 * @return  无
 */
void Clock_Module2Disable(CLOCK_MODULE2_SWITCH ClkSel);

/**
 * @brief	模块时钟使能
 * @param	ClkSel 模块源，根据CLOCK_MODULE3_SWITCH选择
 * @return  无
 */
void Clock_Module3Enable(CLOCK_MODULE3_SWITCH ClkSel);

/**
 * @brief	模块时钟禁能
 * @param	ClkSel 模块源，根据CLOCK_MODULE3_SWITCH选择
 * @return  无
 */
void Clock_Module3Disable(CLOCK_MODULE3_SWITCH ClkSel);

/**
 * @brief	音频PLL时钟生成
 * @param	CLK_MODE PLL源: PLL_CLK 还是 APLL_CLK
 * @param	Index PLL源，PLL_CLK_1:11.2896M;PLL_CLK_2:12.288M;
 * @param	TargetFreq 锁定的音频MCLK频率，推荐配置PLL1： 11 2896M；PLL2: 12.288M。Unit:Hz
 * @return  无
 */
void Clock_AudioPllClockSet(CLK_MODE ClkMode, PLL_CLK_INDEX Index, uint32_t TargetFreq);

/**
 * @brief	音频模块主时钟源选择
 * @param	Module 音频模块
 * @param	ClkSel 时钟来源类型选择
 * @return  无
 */
void Clock_AudioMclkSel(AUDIO_MODULE Module, MCLK_CLK_SEL ClkSel);

/**
 * @brief	获取音频模块主时钟源
 * @param	Module 音频模块
 * @return  时钟来源类型
 */
MCLK_CLK_SEL Clock_AudioMclkGet(AUDIO_MODULE Module);

/**
 * @brief	音频时钟源采样率微调，PLL1和PLL2
 * @param	Index PLL源，PLL_CLK_1:11.2896M;PLL_CLK_2:12.288M;
 * @param	Sign  0：调慢；1：调快
 * @param	Ppm 时钟微调参数，为0时硬件时钟无微调功能。
 * @return  无
 */
void Clock_AudioPllClockAdjust(PLL_CLK_INDEX Index,uint8_t Sign, uint16_t Ppm);

/**
 * @brief	使用DeepSleep功能时的系统时钟选择和flash时钟选择
 * @param	ClockSelect
 * @param	FlashClockSel
 * @param	IsEnterDeepSeep  TRUE:系统进入DeepSleep  FALSE:系统不进DeepSleep
 * @return  bool  TRUE:时钟切换成功     FALSE:时钟切换失败
 */
bool Clock_DeepSleepSysClkSelect(CLK_MODE SysClockSelect, FSHC_CLK_MODE FlashClockSel, bool IsEnterDeepSeep);

/**
 * @brief	配置高频晶体的内置起振电容
 * @param	XICap 晶体输入端起振电容
 * @param	XOCap 晶体输出端起振电容
 * @return  无
 */
void Clock_HOSCCapSet(uint32_t XICap, uint32_t XOCap);

/**
 * @brief	配置高频晶体的内置粗档电容
 * @param	XICap 晶体粗档输入端起振电容【0-3】
 * @param	XOCap 晶体粗档输出端起振电容【0-3】
 * @return  无
 */
void Clock_HOSCMosCapSet(uint32_t XICap, uint32_t XOCap);

/**
 * @brief	配置高频晶体的内置起振电容偏置电流
 * @param	Current 电流值，【0x0-0x1F】，0x1F最大
 * @return  无
 * @note	注意，电流值的调整会影响24M晶体的震荡频率
 */
void Clock_HOSCCurrentSet(uint32_t Current);

/**
 * @brief	HOSC晶振关闭
 * @param	无
 * @return  无
 */
void Clock_HOSCDisable(void);

/**
 * @brief	LOSC晶振关闭
 * @param	无
 * @return  无
 */
void Clock_LOSCDisable(void);

/**
 * @brief	时钟输出到GPIO端口上
 * @param	CLK_GPIO_Port GPIO复用输出时钟端口，GPIOA29，GPIOB6，GPIOB7
 * @param	CLOCK_OUT_MODE 时钟输出的源
 * @return  无
 */
void Clock_GPIOOutSel(CLK_GPIO_Port Port, CLOCK_OUT_MODE mode);

/**
 * @brief	关闭GPIO端口上的时钟输出功能
 * @param	CLK_GPIO_Port GPIO复用输出时钟端口，GPIOA29，GPIOB6，GPIOB7
 * @return  无
 */
void Clock_GPIOOutDisable(CLK_GPIO_Port Port);


/**
 * @brief	单次获取RC时钟开始
 * @param	无
 * @return  无
 */
void Clock_RcFreqCntOneTimeStart(void);

/**
 * @brief	单次获取RC时钟可读取状态
 * @param	无
 * @return  1 可读
 * 			0 不可读
 */
bool Clock_RcFreqCntOneTimeReady(void);

/**
 * @brief	单次获取rc频率
 * @param	IsReCount 是否再次获取硬件计数器值。TRUE：再次启动硬件计数器。FALSE：获取上次记录值。
 * @return  rc频率，单位Hz
 */
uint32_t Clock_RcFreqCntOneTimeGet(bool IsReCount);

/**
 * @brief	开启BB控制HOSC的功能，默认为1
 * @param	0 : 关闭BB控制HOSC
 * @param	1 : 开启BB控制HOSC
 * @return  无
 */
void Clock_BBCtrlHOSCInDeepsleep(uint8_t set);
uint8_t Clock_BBCtrlHOSCInDeepsleepGet(void);

/**
 * @brief	HOSC对32K的分频系数
 * @param	clk_div : 分频系数值(除法)
 * @return  无
 */
void Clock_32KClkDivSet(uint32_t clk_div);
uint32_t Clock_32KClkDivGet(void);

/**
 * @brief  Close BT&PMU RC32K clock for deepsleep
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DeepSleepCloseBTPMURC32K();

/**
 * @brief  Restore BT&PMU RC32K clock for wakeup
 * @param  void
 * @return void
 * @note   none
 */
void Clock_WakeupRetoreBTPMURC32K();

/**
 * @brief  Enable RTC LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_EnableRTCLOSC32K();

/**
 * @brief  Disable RTC LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DisableRTCLOSC32K();

/**
 * @brief  Enable LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_EnableLOSC32K();

/**
 * @brief  Disable LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DisableLOSC32K();

/**
 * @brief  IR Clock select HRC32K in for deepsleep wakeup test
 * @param  void
 * @return void
 * @note   none
 */
void Clock_IRSelHRC32K();

/**
 * @brief  Restore IR Clock selected
 * @param  void
 * @return void
 * @note   none
 */
void Clock_IRRestoreDefaultClk();

/**
 * @brief  RTC_ANA_CTRL
 * @param  RTC_MODE_SEL : 	1: differential Amplifier
 * 							0: single-port  Amplifier
 * @param  RTC_CS : #RTC Osc current control:
 * 					##000~111:240nA+120nA*RTC_CS
 * @return void
 * @note   none
 */
void Clock_RTCLOSC32K_ANA_CTRL(uint8_t RTC_MODE_SEL,uint8_t RTC_CS);

/**
 * @brief	CLOCK_SYNC_CTRL模块状态设置
 * @param	CLOCK_SYNC_CTRL源选择
 *   @arg	Module:CLOCK_SYNC_CTRL源
 *   @arg	Value:设置值
 * @return  none
 */
void Clock_SyncCtrl_Set(CLOCK_SYNC_CTRL_MODULE Module, uint32_t Value);

/**
 * @brief	CLOCK_SYNC_CTRL模块状态获取
 * @param	CLOCK_SYNC_CTRL源选择
 *   @arg	Module:CLOCK_SYNC_CTRL源
 * @return  模块状态
 */
uint32_t Clock_SyncCtrl_Get(CLOCK_SYNC_CTRL_MODULE Module);

/**
 * @brief	CLK_CNT模块计数值获取
 * @param	CLOCK_CLK_CNT源选择
 *   @arg	CLOCK_CLK_CNT源
 * @return  计数值
 */
uint32_t Clock_ClkCnt_Get(CLOCK_CLK_CNT_MODULE Module);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
 
