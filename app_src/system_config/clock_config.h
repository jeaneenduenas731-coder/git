#ifndef _POWER_CONFIG_H__
#define _POWER_CONFIG_H__

#include "app_config.h"

#define CORE_HIGH_MODE				0	//超频/高速模式，主频 功耗较高
#define CORE_USB_CRYSTA_FREE_MODE	1	//免晶体模式，需要开启USB声卡功能并且声卡在线
#define CORE_ONLY_APLL_MODE			2	//单APLL模式，关闭DPLL，需要关闭BT功能以后使用
#define CORE_USER_MODE				3	//
#define CORE_SPDIF_OUT_MODE			4	//spdif使用DPLL,其他时钟使用APLL

//配置系统模式
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	#define SYS_CORE_SET_MODE			CORE_SPDIF_OUT_MODE
#else
	#define SYS_CORE_SET_MODE			CORE_USER_MODE//CORE_ONLY_APLL_MODE  //george
#endif

#define SYS_CRYSTAL_FREQ			24*1000*1000	//晶体频率 Hz
#define SYS_CORE_CLK_DIV                 1
#define SYS_CLK_DIV                      2
#define SYS_CLK_DIV_SNIFF                1
#define SYS_CORE_CLK_DIV_SNIFF           4

#if (SYS_CORE_SET_MODE == CORE_USB_CRYSTA_FREE_MODE)
	#ifndef  CFG_APP_USB_AUDIO_MODE_EN
		#error	USB_AUDIO_MODE is not turned on!!!
	#else
		//usb声卡免晶体模式/需要开启USB声卡功能并且声卡在线
		#define USB_CRYSTA_FREE_EN
		//如果需要超频，需要配置为48M的整数倍（USB时钟频率为48M）
		//USB默认5分频，Clock_USBClkDivSet(5);
		//配置DPLL_FREQ为288M，需要对应配置USB频率：Clock_USBClkDivSet(6);
		#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ

		#define SYS_CORE_CLK_SELECT			PLL_CLK_MODE
		//Note: USB和UART时钟配置DPLL和APLL必须是同一个时钟,但是UART可以单独选择RC
		#define SYS_UART_CLK_SELECT			PLL_CLK_MODE
		#define SYS_USB_CLK_SELECT			PLL_CLK_MODE
		#define SYS_SPDIF_CLK_SELECT		PLL_CLK_MODE

		//flash 时钟配置，频率必须为APLL_FREQ/DPLL_FREQ整数分频（不大于96M）
		#define SYS_FLASH_CLK_SELECT		FSHC_PLL_CLK_MODE
		#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_DPLL_FREQ/3)*1000) 	//Hz

		//音频时钟
		#define	SYS_AUDIO_CLK_SELECT		PLL_CLK_MODE
	#endif
#elif (SYS_CORE_SET_MODE == CORE_ONLY_APLL_MODE)
	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ
	#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ

	#define SYS_CORE_CLK_SELECT			APLL_CLK_MODE
	//Note: USB和UART时钟配置DPLL和APLL必须是同一个时钟,但是UART可以单独选择RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE

	//flash 时钟配置，频率必须为APLL_FREQ/DPLL_FREQ整数分频（不大于96M）
	#define SYS_FLASH_CLK_SELECT		FSHC_APLL_CLK_MODE
	#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_APLL_FREQ/3)*1000) 	//Hz

	//音频时钟
	#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
#elif (SYS_CORE_SET_MODE == CORE_SPDIF_OUT_MODE)

	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ
	#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ only allow 240M、264M and 288M

	#define SYS_CORE_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	//Note: USB和UART时钟配置DPLL和APLL必须是同一个时钟,但是UART可以单独选择RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE//APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE //APLL_CLK_MODE
	//flash 时钟配置，频率必须为APLL_FREQ/DPLL_FREQ整数分频（不大于96M）
	#define SYS_FLASH_CLK_SELECT		FSHC_APLL_CLK_MODE //FSHC_APLL_CLK_MODE
	#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_APLL_FREQ/3)*1000) 	//Hz

	//音频时钟
	#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		#define	SYS_AUDIO_CLK_SELECT		PLL_CLK_MODE
	#else
		#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
	#endif

#else
	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ

	#if defined(CFG_WIRELESS_EN)
		#define	SYS_CORE_DPLL_FREQ			360*1000
	#elif (SYS_CORE_SET_MODE == CORE_HIGH_MODE) || defined(CFG_AI_DENOISE_EN)
		#define	SYS_CORE_DPLL_FREQ			288*1000		//kHZ only allow 240M、264M and 288M
	#else
		#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ only allow 240M、264M and 288M
	#endif

	#define SYS_CORE_CLK_SELECT			PLL_CLK_MODE //APLL_CLK_MODE
	//Note: USB和UART时钟配置DPLL和APLL必须是同一个时钟,但是UART可以单独选择RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE//APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE //APLL_CLK_MODE
	//flash 时钟配置，频率必须为APLL_FREQ/DPLL_FREQ整数分频（不大于96M）
	#define SYS_FLASH_CLK_SELECT		FSHC_PLL_CLK_MODE //FSHC_APLL_CLK_MODE
	#if defined(CFG_WIRELESS_EN)
		#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_DPLL_FREQ/4)*1000) 	//Hz
	#else
		#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_DPLL_FREQ/3)*1000) 	//Hz
	#endif

	//音频时钟
	#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
#endif

#if (SYS_FLASH_FREQ_SELECT > 96*1000*1000)
	//flash时钟频率不能大于96M
	#error	flash frequency error!!!
#endif

#endif



