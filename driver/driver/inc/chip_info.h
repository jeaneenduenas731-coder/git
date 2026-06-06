/**
 **************************************************************************************
 * @file    chip_info.h
 * @brief	define all chip info
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2018-1-15 19:46:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

/**
 * @addtogroup SYSTEM
 * @{
 * @defgroup chip_info chip_info.h
 * @{
 */
 
#ifndef __CHIP_INFO_H__
#define __CHIP_INFO_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define BP15_ROM_V1			0x01
#define BP15_ROM_V2			0x02
#define BP15_ROM_VERSION BP15_ROM_V1

//芯片ECO编号
uint16_t Chip_Version(void);

//LDOVol: 0: 低压(3.6V)； 1：高压(大于3.6V)
void Chip_Init(uint32_t LDOVol);

const unsigned char *GetLibVersionDriver(void);

const unsigned char *GetLibVersionLrc(void);

const unsigned char *GetLibVersionRTC(void);

const unsigned char *GetLibVersionFatfsACC(void);


/**
 * @brief	获取芯片的唯一ID号
 * @param	64bit ID号
 * @return	无
 */
void Chip_IDGet(uint64_t* IDNum);

typedef enum
{
	XMEM0_ARB_FIXED_PRI = 1<<0,
	XMEM1_ARB_FIXED_PRI = 1<<1,
	XMEM2_ARB_FIXED_PRI = 1<<2,
	XMEM3_ARB_FIXED_PRI = 1<<3,
	XMEM4_ARB_FIXED_PRI = 1<<4,
}XMEM_ARB_SET;

/**
	0:burst by burst round-robin priority
	1:cycle by cycle fixed priority(bb_bus > dma0 > dma1 > ibus > dbus > fft)
	when xmem used as Exchange memory,the recommend arbitration strategy is cycle by cycle fixed priority
	xmem4 only exist when dcache_down_size or icache_down_size and configure the downsizing mem used as xmem

	Chip_MemInit();
	Chip_MemARBSet(XMEM0_ARB_FIXED_PRI|XMEM3_ARB_FIXED_PRI);
 */
void Chip_MemInit(void);

void Chip_MemARBSet(uint32_t arb);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif /*__CHIP_INFO_H__*/

/**
 * @}
 * @}
 */

