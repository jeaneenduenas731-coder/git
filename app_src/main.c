#include <stdlib.h>
#include "main_task.h"
#include "clk.h"
#include "timer.h"
#include "i2s.h"
#include "watchdog.h"
#include "reset.h"
#include "rtc.h"
#include "spi_flash.h"
#include "gpio.h"
#include "chip_info.h"
#include "irqn.h"
#include "remap.h"
#include "otg_detect.h"
#include "remind_sound.h"
#include "sw_uart.h"
#ifdef CFG_APP_BT_MODE_EN
#include "bt_common_api.h"
#endif

#include "sadc_interface.h"
#include "powercontroller.h"
#include "audio_decoder_api.h"
#include "sys.h"
#ifdef CFG_FUNC_DISPLAY_EN
#include "display.h"
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_HFP_SUPPORT)
#include "bt_hf_mode.h"
#endif
#endif
#include "rtc_timer.h"
#include "rtc_ctrl.h"
#include "efuse.h"
#include "mode_task.h"
#include "device_detect.h"
#include "idle_mode.h"
#include "flash_table.h"
#include "sys_param.h"
#include "pmu.h"

#include "bt_em_config.h"
#include "audio_adc.h"
#include "adc.h"
//-----------------globle timer----------------------//
volatile uint32_t gInsertEventDelayActTimer = 2000; // ms
volatile uint32_t gChangeModeTimeOutTimer = CHANGE_MODE_TIMEOUT_COUNT;
volatile uint32_t gDeviceCheckTimer = DEVICE_DETECT_TIMER; //ms
#ifdef CFG_FUNC_CARD_DETECT	
volatile uint32_t gDeviceCardTimer = DEVICE_CARD_DETECT_TIMER ;//ms
#endif
#ifdef CFG_LINEIN_DET_EN
volatile uint32_t gDeviceLineTimer = DEVICE_LINEIN_DETECT_TIMER ;//ms
#endif

#ifdef HDMI_HPD_CHECK_DETECT_EN
volatile uint32_t gDevicehdmiTimer = DEVICE_HDMI_DETECT_TIMER ;//ms
#endif

#ifdef CFG_FUNC_BREAKPOINT_EN
volatile uint32_t gBreakPointTimer = 0 ;//ms
#endif
#if defined(CFG_APP_IDLE_MODE_EN)&&defined(CFG_FUNC_REMIND_SOUND_EN)
volatile uint32_t gIdleRemindSoundTimeOutTimer = 0 ;//ms
#endif
//-----------------globle timer----------------------//
extern void DBUS_Access_Area_Init(uint32_t start_addr);
extern const unsigned char *GetLibVersionFatfsACC(void);
extern void UsbAudioTimer1msProcess(void);
extern char *effect_lib_version_return(void);

uint32_t critical_sec_cnt;
//void _printf_float()
//{
//
//}
void OneMSTimer(void)
{
	if(gInsertEventDelayActTimer)gInsertEventDelayActTimer--;
	if(gChangeModeTimeOutTimer)gChangeModeTimeOutTimer--;
	if(gDeviceCheckTimer)gDeviceCheckTimer--;
#ifdef CFG_FUNC_CARD_DETECT	
	if(gDeviceCardTimer > 1)gDeviceCardTimer--;
#endif
#ifdef CFG_LINEIN_DET_EN
	if(gDeviceLineTimer > 1)gDeviceLineTimer--;
#endif
#ifdef HDMI_HPD_CHECK_DETECT_EN
	if(gDevicehdmiTimer > 1)gDevicehdmiTimer--;
#endif
#ifdef CFG_FUNC_BREAKPOINT_EN	
	if(gBreakPointTimer > 1)gBreakPointTimer--;
#endif
#if defined(CFG_APP_IDLE_MODE_EN)&&defined(CFG_FUNC_REMIND_SOUND_EN)
	gIdleRemindSoundTimeOutTimer++;
#endif
#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
	mainAppCt.Silence_Power_Off_Time++;
#endif
}

void Timer2Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
#if defined(CFG_FUNC_USB_DEVICE_DETECT) || defined(CFG_FUNC_UDISK_DETECT)
	OTG_PortLinkCheck();
#endif

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	UsbAudioTimer1msProcess(); //1ms中断监控
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_HFP_SUPPORT)
	BtHf_Timer1msProcess();
#endif
#endif
	OneMSTimer();
#ifdef VD51_REDMINE_13199
	RgbLed1MsInterrupt();
#endif
#ifdef CFG_FUNC_DEBUG_USE_TIMER
	{
		extern void uart_log_out(void);
		uart_log_out();
	}
#endif
}

#ifdef CFG_FUNC_LED_REFRESH
__attribute__((section(".tcm_section")))
void Timer6Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER6, UPDATE_INTERRUPT_SRC);

	//示例代码，需要添加段“.tcm_section”
	//关键字    __attribute__((section(".tcm_section")))
	//客户需要将自己的实现的API代码添加关键字
	//GPIO_RegOneBitSet(GPIO_A_TGL, GPIO_INDEX2);
	extern void LedFlushDisp(void);
	LedFlushDisp();
}
#endif

void SystemClockInit(bool FristPoweron)
{
#if (SYS_CORE_APLL_FREQ >= (288*1000)) || (SYS_CORE_DPLL_FREQ >= (288*1000))
#if (SYS_CORE_APLL_FREQ >= (360*1000)) || (SYS_CORE_DPLL_FREQ >= (360*1000))
	//超频需要配置LDO11
	Power_LDO11DConfig(PWD_LDO11_LVL_1V20);		//需要驱动0.2.8以后版本支持
#else
	//超频需要配置LDO11
	Power_LDO11DConfig(PWD_LDO11_LVL_1V15);
#endif
#endif

#ifndef USB_CRYSTA_FREE_EN
	if(FristPoweron)
		Clock_HOSCCurrentSet(15);
#endif

#ifdef USB_CRYSTA_FREE_EN
	Clock_USBCrystaFreeSet(SYS_CORE_DPLL_FREQ);
#else
	Clock_Config(1, SYS_CRYSTAL_FREQ);
	Clock_PllLock(SYS_CORE_DPLL_FREQ);
	Clock_APllLock(SYS_CORE_APLL_FREQ);//蓝牙使用APLL 240M
#endif

	Clock_SysClkSelect(SYS_CORE_CLK_SELECT);

	//Note: USB和UART时钟配置DPLL和APLL必须是同一个时钟,但是UART可以单独选择RC
	Clock_USBClkSelect(SYS_USB_CLK_SELECT);
	Clock_UARTClkSelect(SYS_UART_CLK_SELECT);

	Clock_SpdifClkSelect(SYS_SPDIF_CLK_SELECT);
	#ifndef PWM5_LED
	Clock_Timer5ClkSelect(RC_CLK_MODE);//for cec rc clk
	#endif
	//模块时钟使能配置
	Clock_Module1Enable(ALL_MODULE1_CLK_SWITCH);
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	Clock_Module3Enable(ALL_MODULE3_CLK_SWITCH);
#ifdef USB_CRYSTA_FREE_EN
	Clock_USBClkDivSet((SYS_CORE_DPLL_FREQ/1000)/48);
#endif
#ifndef USB_CRYSTA_FREE_EN
	if(FristPoweron)
		Clock_HOSCCurrentSet(5);
#endif

	SpiFlashInit(SYS_FLASH_FREQ_SELECT, MODE_4BIT, 0, SYS_FLASH_CLK_SELECT);

#if (SYS_CORE_SET_MODE == CORE_ONLY_APLL_MODE)
	Clock_PllClose();
#endif

#ifdef CHIP_USE_DCDC
	ldo_switch_to_dcdc(PWD_DCDC_LVL_1V6); // 3-1.6V Default:1.6V
#else
	Power_LDO16Config(1);
#endif
}

void LogUartConfig(bool InitBandRate)
{
#ifdef CFG_FUNC_DEBUG_EN
#ifdef CFG_USE_SW_UART
	#if (SW_UART_IO_PORT == GPIO_A_IN)	
		GPIO_PortAModeSet(1<<SW_UART_IO_PORT_PIN_INDEX, 0);
	#else
		GPIO_PortBModeSet(1<<SW_UART_IO_PORT_PIN_INDEX, 0);
	#endif
	if(InitBandRate)
	{
		EnableSwUartAsFuart(1);
	}
	SwUartTxInit(SW_UART_IO_PORT, SW_UART_IO_PORT_PIN_INDEX, CFG_SW_UART_BANDRATE);
#else
	if(GET_DEBUG_GPIO_PORT(CFG_UART_TX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_TX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_UART_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_UART_TX_PORT));
#ifdef CFG_FUNC_SHELL_EN
	if(GET_DEBUG_GPIO_PORT(CFG_FUNC_SHELL_RX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_SHELL_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_SHELL_RX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_SHELL_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_SHELL_RX_PORT));
#endif
	if(InitBandRate)
	{
		DbgUartInit(GET_DEBUG_GPIO_UARTPORT(CFG_UART_TX_PORT), CFG_UART_BANDRATE, 8, 0, 1);
	}
#endif
#endif
#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	UartFuncInit();
#endif
}

/*For deepsleep power save*/
void closeADC()
{
    // disable HPF
    AudioADC_HighPassFilterSet(ADC0_MODULE, FALSE); // SREG_ASDM0_CTRL.ASDM0_HPF_EN = 0;
    AudioADC_HighPassFilterSet(ADC1_MODULE, FALSE); // SREG_ASDM1_CTRL.ASDM1_HPF_EN = 0;

    // disable aux channel
    AudioADC_LREnable(ADC0_MODULE, FALSE, FALSE);
    AudioADC_PGASel(ADC0_MODULE, CHANNEL_LEFT, LINEIN_NONE);
    AudioADC_PGASel(ADC0_MODULE, CHANNEL_RIGHT, LINEIN_NONE);

    // disbale mic channel
    AudioADC_LREnable(ADC1_MODULE, FALSE, FALSE);

    // disable sarADC
    ADC_Disable();
}

/**
 * @brief  DeepSleep时IO临时配置成普通IO功能，并做下拉，尽可能的减少IO漏电
 * @param  void
 * @return void
 * @note   具体根据实际IO外设使用情况来决定是否配置下拉IO来避免漏电
 */
void DeepSleepIOConfig()
{
    GPIO_PortAModeSet(GPIOA0, 0x0000);
    GPIO_PortAModeSet(GPIOA1, 0x0000);
    GPIO_PortAModeSet(GPIOA2, 0x0000);
    GPIO_PortAModeSet(GPIOA3, 0x0000);
    GPIO_PortAModeSet(GPIOA4, 0x0000);
    GPIO_PortAModeSet(GPIOA5, 0x0000);
    GPIO_PortAModeSet(GPIOA6, 0x0000);
    GPIO_PortAModeSet(GPIOA7, 0x0000);
    GPIO_PortAModeSet(GPIOA9, 0x0000);
    GPIO_PortAModeSet(GPIOA10, 0x0000);
    GPIO_PortAModeSet(GPIOA15, 0x0000);
    GPIO_PortAModeSet(GPIOA16, 0x0000);
    GPIO_PortAModeSet(GPIOA17, 0x0000);
    GPIO_PortAModeSet(GPIOA18, 0x0000);
    GPIO_PortAModeSet(GPIOA19, 0x0000);
    GPIO_PortAModeSet(GPIOA20, 0x0000);
    GPIO_PortAModeSet(GPIOA21, 0x0000);
    GPIO_PortAModeSet(GPIOA22, 0x0000);
    GPIO_PortAModeSet(GPIOA23, 0x0000);
    GPIO_PortAModeSet(GPIOA24, 0x0000);
    //    GPIO_PortAModeSet(GPIOA25, 0x0000); //default 1:fshc_wp(io) for flash ctrl
    //    GPIO_PortAModeSet(GPIOA26, 0x0000); //default 1:fshc_hold(io) for flash ctrl
    GPIO_PortAModeSet(GPIOA28, 0x0000);
    GPIO_PortAModeSet(GPIOA29, 0x0000);
    GPIO_PortAModeSet(GPIOA30, 0x0000);
    GPIO_PortAModeSet(GPIOA31, 0x0000);

    //    GPIO_PortBModeSet(GPIOB0, 0x000); // B0、B1一般复用为SW下载调试口
    //    GPIO_PortBModeSet(GPIOB1, 0x000);
    GPIO_PortBModeSet(GPIOB2, 0x000);
    GPIO_PortBModeSet(GPIOB3, 0x000);
    GPIO_PortBModeSet(GPIOB4, 0x000);
    GPIO_PortBModeSet(GPIOB5, 0x000);
    GPIO_PortBModeSet(GPIOB6, 0x000);
    GPIO_PortBModeSet(GPIOB7, 0x000);
    GPIO_PortBModeSet(GPIOB8, 0x000);
    GPIO_PortBModeSet(GPIOB9, 0x000);

    GPIO_RegSet(GPIO_A_IE, 0x00000000);
    GPIO_RegSet(GPIO_A_OE, 0x00000000);
    GPIO_RegSet(GPIO_A_OUTDS0, 0x00000000);
    GPIO_RegSet(GPIO_A_OUTDS1, 0x00000000);
    GPIO_RegSet(GPIO_A_PD, 0x3fffffff); //开发板上A30,A31硬件上做ADC-key,不配置下拉,否则会漏电
    GPIO_RegSet(GPIO_A_PU, 0x00000000); 
    GPIO_RegSet(GPIO_A_ANA_EN, 0x00000000);
    GPIO_RegSet(GPIO_A_PULLDOWN0, 0x00000000);
    GPIO_RegSet(GPIO_A_PULLDOWN1, 0x00000000);

    GPIO_RegSet(GPIO_B_IE, 0x000);
    GPIO_RegSet(GPIO_B_OE, 0x000);
    GPIO_RegSet(GPIO_B_OUTDS, 0x000);
    GPIO_RegSet(GPIO_B_PD, 0x1DF); // B5,B9可能作为外部32K晶振IO，不配置下拉，保持默认值
    GPIO_RegSet(GPIO_B_PU, 0x00); 
    GPIO_RegSet(GPIO_B_ANA_EN, 0x000);
    GPIO_RegSet(GPIO_B_PULLDOWN0, 0x000);//GPIO_B_PULLDOWN0
}

#ifdef CFG_IDLE_MODE_DEEP_SLEEP
extern void FshcClkSwitch(FSHC_CLK_MODE ClkSrc, uint32_t flash_clk);
void SleepMain(void)
{
	WDG_Feed();
    closeADC(); // 关闭ADC，降低功耗
//    DeepSleepIOConfig();
    Clock_RcFreqGet(1);		//先获取RC时钟，避免在唤醒源检测时启动硬件计数器
#ifdef CHIP_USE_DCDC
    ldo_switch_to_dcdc(PWD_DCDC_LVL_1V3); // DCDC 1.3V,降低功耗
#else
	Power_LDO16Config(0);
#endif

	Clock_UARTClkSelect(RC_CLK_MODE);//先切换log clk。避免后续慢速处理
	LogUartConfig(TRUE); //scan不打印时 可屏蔽

	SpiFlashInit(80000000, MODE_1BIT, 0, FSHC_PLL_CLK_MODE);//rc时钟 不支持flash 4bit，系统恢复时重配。
	FshcClkSwitch(FSHC_RC_CLK_MODE, 80000000);//再切RC
	Clock_DeepSleepSysClkSelect(RC_CLK_MODE, FSHC_RC_CLK_MODE, 1);
	Clock_PllClose();
	Clock_APllClose();//APLL会增加约980uA的功耗
#if !defined(CFG_RES_RTC_EN)
	Clock_HOSCDisable();//若有RTC应用并且RTC所用时钟是HOSC，则不关闭HOSC，即24M晶振
#endif
//	Clock_LOSCDisable(); //若有RTC应用并且RTC所用时钟是LOSC，则不关闭LOSC，即32K晶振
    Power_LDO11DConfig(PWD_LDO11_LVL_0V95); // 降低到0.95V
}

void WakeupMain(void)
{
	WDG_Feed();
    Power_LDO11DConfig(PWD_LDO11_LVL_1V10); // 升回1.1V
    
    Chip_Init(1);
	SystemClockInit(TRUE);
	LogUartConfig(TRUE);//调整时钟后，重配串口前不要打印。

	SysTickInit();
	ADC_Enable();
}
#endif


/*****************************************************************
 * BT EM Size
 *****************************************************************/
#ifdef CFG_APP_BT_MODE_EN
extern uint32_t bt_em_size(void);
void bt_em_size_init(void)
{
	uint32_t bt_em_mem;

	bt_em_mem = bt_em_size();
	APP_DBG("BB_EM_SIZE=%d,EM_BT_END=%d\n", (int)BB_EM_SIZE, (int)bt_em_mem);
	if(bt_em_mem%4096)
	{
		bt_em_mem = ((bt_em_mem/4096)+1)*4096;
	}
	if(bt_em_mem > BB_EM_SIZE)
	{
		APP_DBG("bt em config error!\nyou must check BB_EM_SIZE\n%s%u \n",__FILE__,__LINE__);
		while(1);
	}
	else
	{
		APP_DBG("bt em size:%uKB\n", (unsigned int)bt_em_mem/1024);
	}

}
#endif


#if 0//(BP15_ROM_VERSION == BP15_ROM_V2)

void RomFix_LC3(void)
{
#define     REG_ROM_FIX_ADDR0              (*(volatile unsigned long *) 0x400400D4)
#define     REG_ROM_FIX_DATA0              (*(volatile unsigned long *) 0x400400E0)
#define     REG_ROM_FIX_ENABLE             (*(volatile unsigned long *) 0x400400D0)

	//lc3_ltpf_analyse() __attribute__((section(".lc3_section")))
	//flash addr： 0x108
	//Rom addr 0x00807ea8
	extern void lc3_ltpf_analyse(void);
	void (*funcPtr)(void) = lc3_ltpf_analyse;
	uint32_t offset = (((int)funcPtr - (int)0x00807ea8) >> 1) & 0x00FFFFFF;
	uint32_t data = 0x48 | (offset & 0xFF) << 24 | (offset& 0xFF00) << 8 | (offset & 0xFF0000) >> 8;

    REG_ROM_FIX_ADDR0 = 0x00807ea8 / 4;
//    REG_ROM_FIX_DATA0 = 0x30C1BF48;
    REG_ROM_FIX_DATA0 = data;

    REG_ROM_FIX_ENABLE |= 0x01;
    __nds32__isb();
}

#endif

/*****************************************************************
 * main function
 *****************************************************************/
int main(void)
{
	uint16_t RstFlag = 0;
	extern char __sdk_code_start;

#if 0//(BP15_ROM_VERSION == BP15_ROM_V2)
	RomFix_LC3();
#endif
	Chip_Init(1);
	Chip_MemInit();
	WDG_Enable(WDG_STEP_4S);
//	WDG_Disable();

	RstFlag = Reset_FlagGet();
	Reset_FlagClear();

	//如果需要使用NVM内存时，需要调用该API，第一次系统上电需要清除对NVM内存清零操作，在breakpoint 内实施。
	PMU_NVMInit();

	SystemClockInit(TRUE);

	LogUartConfig(TRUE);

	
#ifdef UserSoftPower
	memset(&userVar, 0, sizeof(UserVar));
#endif

#ifdef CFG_FUNC_USBDEBUG_EN
	extern void usb_hid_printf_init(void);
	usb_hid_printf_init();
#endif
	
	DBUS_Access_Area_Init(0);//设置Databus访问区间为codesize
	
#if defined(TCM_EN) && defined(CFG_WIRELESS_EN)
	DBG("TCM_SRAM_START_ADDR_1 = %x, TCM_SIZE = %d KB\n", TCM_SRAM_START_ADDR_1, WIRELESS_TCM_SIZE);
	Remap_InitTcm((uint32_t)(&__sdk_code_start),TCM_SRAM_START_ADDR_1, WIRELESS_TCM_SIZE);
#else
	Remap_InitTcm((uint32_t)(&__sdk_code_start), TCM_SRAM_START_ADDR_2, DRV_REMAP_SIZE/1024);
#endif

	//考虑到大容量的8M flash，写之前需要Unlock，SDK默认不做加锁保护
	//用户为了增加flash 安全性，请根据自己flash大小和实际情况酌情做flash加锁保护机制
//	SpiFlashIOCtrl(IOCTL_FLASH_PROTECT, FLASH_LOCK_RANGE_HALF);//加锁保护code区域
	prvInitialiseHeap();

	osSemaphoreMutexCreate();//硬件串口OS启用了软件锁，必须在创建锁之后输出log，否则死机，锁要初始化堆栈后创建。软件模拟串口不影响。

#ifdef CFG_SOFT_POWER_KEY_EN
	SoftPowerInit();
	WaitSoftKey();
#endif

	NVIC_EnableIRQ(SWI_IRQn);
	GIE_ENABLE();	//开启总中断
#ifdef CFG_FUNC_LED_REFRESH
	//默认优先级为0，旨在提高刷新速率，特别是断点记忆等写flash操作有影响刷屏，必须严格遵守所有timer6中断调用都是TCM代码，含调用的driver库代码
	//已确认GPIO_RegOneBitSet、GPIO_RegOneBitClear在TCM区，其他api请先确认。
	NVIC_SetPriority(Timer6_IRQn, 0);
 	Timer_Config(TIMER6,1000,0);
 	Timer_Start(TIMER6);
 	NVIC_EnableIRQ(Timer6_IRQn);

 	//此行代码仅仅用于延时，配合Timer中断处理函数，客户一定要做修改调整
 	//GPIO_RegOneBitSet(GPIO_A_OE, GPIO_INDEX2);//only test，user must modify
#endif

#ifdef CFG_FUNC_DISPLAY_EN
 	DispInit(0);
#endif

	APP_DBG("\n");
	APP_DBG("****************************************************************\n");
	APP_DBG("|                    MVsB5_BT_Audio_SDK                        |\n");
	APP_DBG("|            Mountain View Silicon Technology Co.,Ltd.         |\n");
	APP_DBG("|            SDK Version: %d.%d.%d                                |\n", CFG_SDK_MAJOR_VERSION, CFG_SDK_MINOR_VERSION, CFG_SDK_PATCH_VERSION);
	APP_DBG("****************************************************************\n");
	APP_DBG("sys clk =%ld\n",Clock_SysClockFreqGet());

	APP_DBG("CFG_CHIP_SEL:%d\n",CFG_CHIP_SEL);
#ifdef	CHIP_USE_DCDC
	APP_DBG("CHIP_USE_DCDC\n");
#endif
#ifdef CHIP_DAC_USE_DIFF
	APP_DBG("DAC_Diff\n");
#else
	APP_DBG("DAC_Single\n");
#endif
#ifdef CHIP_DAC_USE_PVDD16
	APP_DBG("PVDDModel: PVDD16\n");
#endif

#if defined(CFG_DOUBLE_KEY_EN) && !defined(CFG_AI_DENOISE_EN)
	{
		extern void AlgUserDemo(void);
		AlgUserDemo();
	}
#endif

#ifdef CFG_APP_IDLE_MODE_EN
 	IdleModeConfig();
#endif
	flash_table_init();
	sys_parameter_init();
#ifdef CFG_APP_BT_MODE_EN
	bt_em_size_init();
#endif

	APP_DBG("RstFlag = %x\n", RstFlag);
	APP_DBG("Audio Decoder Version: %s\n", (unsigned char *)audio_decoder_get_lib_version());
	APP_DBG("Audio Effect  Lib Version: %s\n", (char *)effect_lib_version_return());
	APP_DBG("Roboeffect  Lib Version: %s\n", ROBOEFFECT_LIB_VER);
	APP_DBG("Driver Version: %s\n", GetLibVersionDriver());
#ifdef CFG_FUNC_LRC_EN
    APP_DBG("Lrc Version: %s\n", GetLibVersionLrc()); 
#endif
#if defined(CFG_APP_BT_MODE_EN) || defined(CFG_WIRELESS_EN)
    APP_DBG("BtLib Version: %s\n", (unsigned char *)GetLibVersionBt());
#endif
#ifdef CFG_WIRELESS_EN
    APP_DBG("wireless2 Version: %s\n", (unsigned char *)GetLibVersionWireless2());
#endif
#ifdef CFG_RES_FLASHFS_EN
	APP_DBG("FlashFSLib Version: %s\n", (unsigned char *)GetLibVersionFlashFS());
#endif
	APP_DBG("Fatfs presearch acc Lib Version: %s\n", (unsigned char *)GetLibVersionFatfsACC());
#ifdef CFG_FUNC_ALARM_EN
	APP_DBG("RTC Version: %s\n", GetLibVersionRTC());//bkd 
#endif
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
	extern const unsigned char *GetLibVersionOtgHost(void);
	APP_DBG("OtgHost Version: %s\n",(unsigned char *) GetLibVersionOtgHost());
#endif
	APP_DBG("ECO Flag: %x\n",Read_ChipECO_Version());
	APP_DBG("\n");

#ifdef USE_EXTERN_FLASH_SPACE
	SPIM_Init(0, 0);
	SPI_Flash_Init();
	APP_DBG("SPI_Flash_ReadMID=%x\n",SPI_Flash_ReadMID());
#endif

#ifdef DBG_IO_TGL
	DbgPortInit(); //deepsleep唤醒后要重新初始化
#endif
#ifdef CFG_FUNC_RTC_EN
	RTC_ServiceInit(RstFlag&0x01);
	RTC_IntEnable();
	NVIC_EnableIRQ(RTC_IRQn);
#endif

	__nds32__mtsr(0,NDS32_SR_PFMC0);
	__nds32__mtsr(1,NDS32_SR_PFM_CTL);
#ifdef VD51_REDMINE_13199
	RgbLedInit();
#endif
#ifdef CFG_FUNC_DEBUG_USE_TIMER
	{
		extern uint8_t uart_switch ;
		uart_switch = 1;
	}
#endif
	MainAppTaskStart();
	vTaskStartScheduler();

	while(1);

}

