/*
 * wireless_service.h
 *
 *  Created on: Dec 6, 2024
 *      Author: piwang
 */

#ifndef __WIRELESS_SERVICE_H__
#define __WIRELESS_SERVICE_H__

#include "FreeRTOS.h"

#define WIRELESS_OUT_FIFO_SAMPLES		(512 + 256)//1.5*(Max512)
#define WIRELESS_OUT_SAMPLES_BYTE		4
#define WIRELESS_OUT_FIFO_LEN			(WIRELESS_OUT_FIFO_SAMPLES * WIRELESS_OUT_SAMPLES_BYTE)


/**
 * @brief	Get message receive handle of Wireless manager
 * @param	NONE
 * @return	MessageHandle
 */
xTaskHandle GetWirelessServiceTaskHandle(void);

/************************************************************************************
 * @brief	Start Wireless service.
 * @param	NONE
 * @return
 ***********************************************************************************/
bool WirelessServiceStart(void);

/************************************************************************************
 * @brief	Kill wireless service.
 * @param	NONE
 * @return
 ***********************************************************************************/
bool WirelessServiceKill(void);

uint16_t WirelessOutSpaceLenGet(void);

uint16_t WirelessOutDataSet(int16_t *Buf, uint32_t Samples);

#endif /* __WIRELESS_SERVICE_H__ */
