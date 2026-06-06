#ifndef _CHIP_CONFIG_H_
#define _CHIP_CONFIG_H_

//****************************************************************************************
// 芯片封装定义,请勿随意修改
//****************************************************************************************
#define CFG_CHIP_B5X128			1		//128pin 开发板
#define CFG_CHIP_BP1524A1		2
#define CFG_CHIP_BP1524A2		3
#define CFG_CHIP_BP1532A1		4
#define CFG_CHIP_BP1532A2		5
#define CFG_CHIP_BP1532B1		6
#define CFG_CHIP_BP1532B2		7
#define CFG_CHIP_BP1548C1		8
#define CFG_CHIP_BP1548C2		9
#define CFG_CHIP_BP1564A1		10
#define CFG_CHIP_BP1564A2		11
#define CFG_CHIP_AP1524A1		12		//无蓝牙
#define CFG_CHIP_AP1532B1		13		//无蓝牙
#define CFG_CHIP_BP1532E2		14
#define CFG_CHIP_BP1532E1		15
#define CFG_CHIP_BP1552A2		16
#define CFG_CHIP_BP1552D2		17
#define CFG_CHIP_BP1532F1		18
#define CFG_CHIP_BP1532F2		19
#define CFG_CHIP_AP1532B0		20		//无蓝牙
#define CFG_CHIP_BP1540A2		21
#define CFG_CHIP_AP1524A0		22		//无蓝牙
//****************************************************************************************
//       芯片型号选择配置
// 不同的封装的电源配置/蓝牙模拟参数等可能会不一样
// 请在这里选择相应的封装芯片进行开发
//****************************************************************************************
#define CFG_CHIP_SEL           	CFG_CHIP_BP1532B1


//****************************************************************************************
// 芯片相关参数宏定义，请勿随意修改
//****************************************************************************************
#if (CFG_CHIP_SEL == CFG_CHIP_B5X128)
    #define CHIP_FLASH_CAPACTITY			16						//flash容量
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1	//linein通道
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1524A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1524A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DAC使用差分输出
	#define	CHIP_DAC_USE_PVDD16										//DAC使用PVDD16
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DAC使用差分输出
	#define	CHIP_DAC_USE_PVDD16										//DAC使用PVDD16
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532B1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532B2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532E2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532E1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1548C1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define	CHIP_DAC_USE_DIFF
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1548C2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define	CHIP_DAC_USE_DIFF
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1564A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define	CHIP_DAC_USE_DIFF
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1564A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define	CHIP_DAC_USE_DIFF
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1552A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1552D2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1524A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_BT_DISABLE											//无蓝牙功能
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1532B1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC
	#define	CHIP_BT_DISABLE											//无蓝牙功能
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1532B0)
    #define CHIP_FLASH_CAPACTITY			4
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC
	#define	CHIP_BT_DISABLE											//无蓝牙功能
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1524A0)
    #define CHIP_FLASH_CAPACTITY			4
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_BT_DISABLE											//无蓝牙功能
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532F1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DAC使用差分输出
	//PVDD板子上连接了DVDD16需要打开CHIP_DAC_USE_PVDD16,如果连接了DVDD33则关闭该宏
	#define	CHIP_DAC_USE_PVDD16										//DAC使用PVDD16
	#define	CHIP_USE_DCDC											//使用DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532F2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DAC使用差分输出
	//PVDD板子上连接了DVDD16需要打开CHIP_DAC_USE_PVDD16,如果连接了DVDD33则关闭该宏
	#define	CHIP_DAC_USE_PVDD16										//DAC使用PVDD16
	#define	CHIP_USE_DCDC
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1540A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//使用DCDC
	#define	CHIP_DAC_USE_DIFF
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
#else
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define CHIP_AUDIODAC_DOUT_MODE			MODE1
    #error "Undefined Chip type!!!!"
#endif

//****************************************************************************************
// VCOM免电容直接驱动耳机 (部分芯片支持)
//****************************************************************************************
#if (CFG_CHIP_SEL == CFG_CHIP_BP1532E1) || (CFG_CHIP_SEL == CFG_CHIP_BP1532E2) || (CFG_CHIP_SEL == CFG_CHIP_BP1564A1) || (CFG_CHIP_SEL == CFG_CHIP_BP1564A2)
//	#define CFG_VCOM_DRIVE_EN
#endif

//****************************************************************************************
// 芯片RAM宏定义，请勿随意修改
//****************************************************************************************
#define CFG_D16K_MEM16K_EN		0		//1 ---> D-Cache拿出16K给RAM用,D-Cache 16KB
										//0 ---> D-Cache 32KB
#define CFG_D16KMEM16K_RAM_SIZE	(CFG_D16K_MEM16K_EN*16*1024)

#define CFG_CHIP_RAM_SIZE		(256*1024 + CFG_D16KMEM16K_RAM_SIZE)


#endif


