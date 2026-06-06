/*
 * efuse.h
 *
 *  Created on: May 15, 2019
 *      Author: jerry_rao
 */
/**
 * @addtogroup EFUSE
 * @{
 * @defgroup efuse efuse.h
 * @{
 */

#ifndef __EFUSE_H__
#define __EFUSE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

/**
 * @addtogroup EFUSE
 * @{
 * @defgroup efuse efuse.h
 * @{
 */
 
#include "type.h"


typedef enum
{
	BT_SWITCH 					= 0x00,
	SWBB_SWITCH					= 0x01,
	LE_AUDIO_MESH_SWITCH		= 0x02,
	CAN_SWITCH					= 0x03,
	ADC_SWITCH					= 0x04,
	DAC_SWITCH					= 0x05,
	ALGO1_SWITCH				= 0x06,
//	ALGO2_SWITCH				= 0x07,
//	ALGO3_SWITCH				= 0x08
}FUSE_MODULE_SWITCH;



/**
 * @brief  读取Efuse中指定地址的数据
 * @param  Addr为Efuse中指定的地址，十六进制表示
 *         可读的地址为  0~31
 * @return 读取到指定地址的数据
 */
uint8_t Efuse_ReadData(uint8_t Addr);


/**
 * @brief read module switch status from fuse
 * @param module name
 * return enable (l) or not (0)
 */
bool Read_ModuleSwitch_Status(FUSE_MODULE_SWITCH Module_Switch);

/**
 * @brief  读取Chip ECO版本
 * @param
 * @return 0/1/2/3
 */
uint8_t Read_ChipECO_Version(void);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif // __EFUSE_H__ 
/**
 * @}
 * @}
 */

