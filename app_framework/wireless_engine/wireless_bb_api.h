
#ifndef _BB_API_H_
#define _BB_API_H_

#include "type.h"
#include "bt_em_config.h"

//#define BB_EM_SIZE					(16*1024) // (16*1024)
//#define BB_EM_START_PARAMS			((0x40000-BB_EM_SIZE)/1024)

//#define BB_EM_MAP_ADDR			0x80000000

//#define BB_MPU_START_ADDR		(0x20040000 - BB_EM_SIZE)

typedef unsigned char (*BBSniffNotifyCallback)(void);

typedef struct _WirelessBbParams
{
	uint8_t		*localDevName;
	uint8_t		localDevAddr[6];
	uint8_t		freqTrim;
	uint32_t	em_start_addr;

	//bb agc config
	uint8_t		pAgcDisable;
	uint8_t		pAgcLevel;

	//sniff config
	uint8_t		pSniffNego;
	uint16_t	pSniffDelay;
	uint16_t	pSniffInterval;
	uint16_t	pSniffAttempt;
	uint16_t	pSniffTimeout;

	BBSniffNotifyCallback	bbSniffNotify;
}WirelessBbParams;

void Wireless_common_init(void* params);

/***************************************************
 *
 **************************************************/
void wireless_init(void* params);

/***************************************************
 *
 **************************************************/
void rw_main(void);

uint32_t wireless_em_size(void);

void Wireless_fcc_mode(bool state);

void GPIOA_Set2(char iox,char set);

void GPIOB_Set2(char iox,char set);

#endif //_BB_API_H_
