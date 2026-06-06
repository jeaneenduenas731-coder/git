/**
  ******************************************************************************
  * @file    remap.h
  * @author  Peter Zhai
  * @version V1.0
  * @date    2017-10-27
  * @brief
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/**
 * @addtogroup SYSTEM
 * @{
 * @defgroup remap remap.h
 * @{
 */

#ifndef __REMAP_H__
#define __REMAP_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

#include"debug.h"

#define REMAP_DBG(format, ...)		//DBG(format, ##__VA_ARGS__)

#define FLASH_ADDR 			0x0
#define TCM_SIZE   			12//8//4
#define SRAM_END_ADDR		0x2003C000
#define TCM_SRAM_START_ADDR	0x20002000

extern uint32_t gSramEndAddr;
typedef enum
{
	ADDR_REMAP0=0,
	ADDR_REMAP1=1,
	ADDR_REMAP2=2,
	ADDR_REMAP3=3
} ADDR_REMAP_ID;

typedef enum
{
	REMAP_UNALIGNED_ADDRESS = -1,
	REMAP_OK = 0
}REMAP_ERROR;

/**
 * @brief	使能remap功能
 * @param	num	 ADDR_REMAP_ID
 * @param	src	 源地址，必须1KB对齐
 * @param	dst	 目的地址，必须1KB对齐
 * @param	size 单位是KB，必须1KB对齐
 * @return	错误码，详情见REMAP_ERROR
 * @note
 */
REMAP_ERROR Remap_AddrRemapSet(ADDR_REMAP_ID num, uint32_t src, uint32_t dst, uint32_t size);

/**
 * @brief	关闭remap功能
 * @param	num	 ADDR_REMAP_ID
 * @return	None
 * @note
 */
void Remap_AddrRemapDisable(ADDR_REMAP_ID num);

/**
 * @brief	使能ADDR_REMAP0 TCM功能
 * @param	StartAddr	源地址，必须1KB对齐
 * @param	TCMStartAddr TCM RAM目标地址，必须1KB对齐
 * @param	size 		单位是KB，必须1KB对齐
 * @return	None
 * @note
 */
#define Remap_InitTcm(StartAddr, TCMStartAddr, size) 	do{ memcpy((void*)TCMStartAddr, (void*)StartAddr, size*1024);\
															Remap_AddrRemapSet(ADDR_REMAP0, StartAddr, TCMStartAddr, size);\
														}while(0)

/**
 * @brief	禁能ADDR_REMAP0 TCM功能
 * @param	None
 * @return	None
 * @note
 */
#define Remap_DisableTcm()								Remap_AddrRemapDisable(ADDR_REMAP0)

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__REMAP_H__
/**
 * @}
 * @}
 */
