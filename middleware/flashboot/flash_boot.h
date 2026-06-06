#ifndef __FLASH_BOOT_H__
#define __FLASH_BOOT_H__
/*
版本说明：当前为V5.3.0版本
日期：2025年8月13日
*/
#ifdef CFG_APP_CONFIG
	#include "upgrade.h"
	#include "app_config.h"
	#include "flash_table.h"
	#include "chip_info.h"
#endif

#define FLASH_BOOT_NONE			0 //无flashboot
#define FLASH_BOOT_BASIC		1 //基础版flashboot
#define FLASH_BOOT_OTA_PRO		2 //Pro版 flashboot

#ifdef CFG_FUNC_BT_OTA_EN
	#define FLASH_BOOT_EN     	FLASH_BOOT_OTA_PRO		//带OTA功能
#else
	#define FLASH_BOOT_EN    	FLASH_BOOT_BASIC
#endif

//需要和debug.h中定义的GPIO一一对应
typedef enum __UART_TX
{
	BOOT_DEBUG_TX_A0 = 0,
	BOOT_DEBUG_TX_A1,
	BOOT_DEBUG_TX_A6,
	BOOT_DEBUG_TX_A10,
	BOOT_DEBUG_TX_A18,
	BOOT_DEBUG_TX_A19,
	BOOT_DEBUG_TX_A20,
	BOOT_DEBUG_TX_A21,
	BOOT_DEBUG_TX_A22,
	BOOT_DEBUG_TX_A23,
	BOOT_DEBUG_TX_A24,
	BOOT_DEBUG_TX_A28,
	BOOT_DEBUG_TX_A29,
	BOOT_DEBUG_TX_A30,
	BOOT_DEBUG_TX_A31,
	BOOT_DEBUG_TX_B5
}UART_TX;

//需要和debug.h中定义的波特率一一对应
typedef enum __UART_BAUDRATE
{
	BOOT_DEBUG_BAUDRATE_9600 = 0,
	BOOT_DEBUG_BAUDRATE_115200,
	BOOT_DEBUG_BAUDRATE_256000,
	BOOT_DEBUG_BAUDRATE_1000000,
	BOOT_DEBUG_BAUDRATE_2000000,
	BOOT_DEBUG_BAUDRATE_3000000,
}UART_BAUDRATE;


//波特率配置
#if CFG_FLASHBOOT_DEBUG_EN

	#define  DEBUG_CONNECT(x, y)			x ## y
	#define  DEBUG_STRING_CONNECT(x, y)		DEBUG_CONNECT(x, y)

	#define CFG_BOOT_UART_BANDRATE   		DEBUG_STRING_CONNECT(BOOT_,CFG_UART_BANDRATE)
	#define CFG_BOOT_UART_TX_PORT    		DEBUG_STRING_CONNECT(BOOT_,CFG_UART_TX_PORT)
	#define BOOT_UART_CONFIG				((CFG_FLASHBOOT_DEBUG_EN<<7) + (CFG_BOOT_UART_BANDRATE<<4) + CFG_BOOT_UART_TX_PORT)

#else
	#define BOOT_UART_CONFIG				0
#endif


/*  JUDGEMENT_STANDARD说明
 * 分高4bit与低4bit：
 *   高4bit：
 *      为F则code按版本号升级
 *      为5则按code的CRC进行升级
 *   低4bit:
 *     为F则在升级code需要用到多大空间即擦除多大空间
 *     为5时则标识升级code前全部擦除芯片数据，即擦除“全片”（即除开flash的0地址开始flashboot占用空间不擦除以及最后8K不擦除）
 * 例如：0x5F 则为比较CODE CRC判断是否需要升级；升级时仅部分擦除，不进行全擦除
 */
#define JUDGEMENT_STANDARD		0x55

#if CFG_RES_CARD_GPIO == SDIO_A15_A16_A17
#define SD_PORT				CHN_MASK_SDCARD
#else
#define SD_PORT				CHN_MASK_SDCARD1
#endif

#ifdef CFG_FUNC_BT_OTA_EN
	#define UP_PORT				(CHN_MASK_USBCDC + CHN_MASK_UDISK + SD_PORT + CHN_MASK_BLE) //根据应用情况决定打开那些升级接口
#else
	#define UP_PORT				(CHN_MASK_USBCDC + CHN_MASK_UDISK + SD_PORT) //根据应用情况决定打开那些升级接口
#endif

typedef enum __UART_UP
{
	UART0_UP_TXRX_A0A1 = 0,
	UART0_UP_TXRX_A1A0,
	UART0_UP_TXRX_A6A5,
	UART1_UP_TXRX_A10A9,
	UART1_UP_TXRX_A18A19,
	UART1_UP_TXRX_A19A18,
}UART_UP_PORT;
typedef enum __UART_UP_BAUDRATE
{
	UART_UP_BAUDRATE_115200 = 0,
	UART_UP_BAUDRATE_256000,
	UART_UP_BAUDRATE_1000000,
	UART_UP_BAUDRATE_2000000,
}UART_UP_BAUDRATE;
//打印串口和升级串口不能使用同一组。
/* UART_UP_CONFIG 说明
	bit[3:0]:端口配置
		参考 UART_UP_PORT 枚举参数
	bit[7:4]: 波特率配置
		参考 UART_BAUDRATE 枚举参数
*/
#define CFG_UART_UP_PORT		UART0_UP_TXRX_A6A5
#define CFG_UART_UP_BAUD_RATE	UART_UP_BAUDRATE_115200
#define UART_UP_CONFIG			((CFG_UART_UP_BAUD_RATE<<4)+CFG_UART_UP_PORT)

/* SPIMFLASH_UP_CONFIG 说明
	bit[0]: 端口配置
		2’b00:SPIM_PORT0_A5_A6_A7
		2’b01:SPIM_PORT1_A20_A21_A22
	bit[1]: 保留
	bit[2]: CS片选IO组配置
		2’b00:GPIO_A_START
		2’b01:GPIO_B_START
	bit[7:3]: CS片选IO索引 GPIO_INDEX
		0~31: GPIO_INDEX0~GPIO_INDEX31
*/
#define SPIMFLASH_UP_SPIM_PORT	0	//SPIM_PORT0_A5_A6_A7
#define SPIMFLASH_UP_CS_PORT	0	//GPIO_A_START
#define SPIMFLASH_UP_CS_PIN		9	//GPIO_INDEX9
#define SPIMFLASH_UP_CONFIG ((SPIMFLASH_UP_CS_PIN<<3) + (SPIMFLASH_UP_CS_PORT<<2) + SPIMFLASH_UP_SPIM_PORT)

/*  BOOT_SYSTEM_CONFIGURATIOM 说明
	bit[1:0]:系统时钟配置
		2’b00: HSE OSC 24M
		2’b01: RC_32K
	bit[3:2]:代码校验模式配置
		2’b00: SYMBOL_CHECK,			//中断向量表+特征校验
		2’b01: CRC_PART_CODE_CHECK,		//CRC部分  user code 校验
		2’b10: CRC_ALL_CODE_CHECK,		//CRC全部  user code 校验
	bit[7:4]:保留
 */
typedef enum {
	HSE_OSC_24M,						//外部24m晶体
	RC_32K,								//内部RC32K
}BOOT_CLK_MODE;

typedef enum {
	SYMBOL_CHECK,						//中断向量表+特征校验
	CRC_PART_CODE_CHECK,				//CRC部分  user code 校验
	CRC_ALL_CODE_CHECK,					//CRC全部  user code 校验
}UPGRAD_CHECK_MODE;


#ifdef USB_CRYSTA_FREE_EN
#define BOOT_CLK 	RC_32K
#else
#define BOOT_CLK 	HSE_OSC_24M			//外部24m晶体
#endif

#define CODE_CHECK 	CRC_ALL_CODE_CHECK	//CRC全部  user code 校验

#define BOOT_SYSTEM_CONFIGURATIOM		((CODE_CHECK<<2) + BOOT_CLK)

#define  CFG_SDK_MAGIC_NUMBER                (0xB0BEBDC9)

#endif

