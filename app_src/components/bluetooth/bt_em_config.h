
///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_em_config.h
//  maintainer: kk
///////////////////////////////////////////////////////////////////////////////

#ifndef BT_EM_CONFIG_H_
#define BT_EM_CONFIG_H_

#include "bt_config.h"

#define BB_EM_MAP_ADDR				0x80000000

#if( ( BLE_SUPPORT) && ( BT_SUPPORT))
#define BB_EM_SIZE					(28*1024)
#define BB_EM_START_PARAMS			((CFG_CHIP_RAM_SIZE-BB_EM_SIZE)/1024)
#elif( (!BLE_SUPPORT) && ( BT_SUPPORT))
#define BB_EM_SIZE					(24*1024)
#define BB_EM_START_PARAMS			((CFG_CHIP_RAM_SIZE-BB_EM_SIZE)/1024)
#else
#define BB_EM_SIZE					0
#define BB_EM_START_PARAMS			0
#endif

#define BB_MPU_START_ADDR			(0x20040000 + CFG_D16KMEM16K_RAM_SIZE - BB_EM_SIZE)

#define TCM_SRAM_START_ADDR_2		(BB_MPU_START_ADDR - DRV_REMAP_SIZE)
/*
 * RF SPI part
 ****************************************************************************************
 */

//void app_bt_em_params_config(void);

#endif /*__BT_EM_CFG_H__*/

