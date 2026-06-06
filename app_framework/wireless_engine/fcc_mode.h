/**
 **************************************************************************************
 * @file    fcc_mode.h
 * @brief   
 *
 * @author  yangsen
 * @version V0.0.1
 *
 * $Created: 2025-06-27 
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef FCC_MODE_H_
#define FCC_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define FCC_FREQ    (0)
#define FCC_POWER   (7)

void fcc_mode_read(void);
void fcc_mode_init(void);

void fcc_mode_start(void);
#ifdef __cplusplus
}
#endif//__cplusplus

#endif /* HMI_FCC_MODE_H_ */
