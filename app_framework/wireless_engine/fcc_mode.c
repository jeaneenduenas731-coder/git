/**
 **************************************************************************************
 * @file    fcc_mode.c
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
#include "app_config.h"
#include "fcc_mode.h"
#include "reset.h"
#include "bb_api.h"
#include "pmu.h"
#include "debug.h"
#ifdef CFG_FCC_MODE

static bool fcc_mode_state;
void fcc_mode_read(void)
{
	uint8_t fcc_mode = 0;
	PMU_NvmRead(4, &fcc_mode, 1);
    if((fcc_mode >> 1) & 0x1 && fcc_mode_state == 0) {
        DBG("fcc mode freq %d power %d\n",(2402 +FCC_FREQ),FCC_POWER);
        Wireless_fcc_mode(TRUE);
        fcc_mode ^= 2;
        fcc_mode_state = TRUE;
        PMU_NvmWrite(4, &fcc_mode, 1);
    }
}
void fcc_mode_init(void){

    if(fcc_mode_state == TRUE) {
        DBG("fcc mode freq %d power %d\n",(2402 +FCC_FREQ),FCC_POWER);
        DelayMs(10);

        extern void rf_tx_dynam_test(uint16_t freq_val,uint16_t tx_pwr);
        rf_tx_dynam_test(FCC_FREQ, FCC_POWER);
    }
}

void fcc_mode_start(void){
    if(fcc_mode_state == 0){
        DBG("fcc mode start\n");
        uint8_t fcc_mode = 0;
        PMU_NvmRead(4, &fcc_mode, 1);
        fcc_mode |= 0x2;
        PMU_NvmWrite(4, &fcc_mode, 1);
        Reset_McuSystem();
    }

}
#endif
