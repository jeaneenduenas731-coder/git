/*
 * wireless_hmi.c
 *
 *  Created on: Jun 9, 2025
 *      Author: piwang
 */

/**Porting from key.c@2.4G SDK**/
#include "wireless_hmi.h"
uint8_t InfoBits = INFO_BIT_SYNC;
void InfoBitsSet(uint8_t Flag)
{
	InfoBits |= (Flag) & INFO_BIT_MASK;
}

void InfoBitsClear(uint8_t Flag)
{
	InfoBits &= ~((Flag) & INFO_BIT_MASK);
}

uint8_t InfoBitsGet(uint8_t Flag)
{
	return InfoBits & Flag;
}

uint8_t InfoGet(void)
{
	return InfoBits;
}
/***********End of Key.c@2.4G SDK************/
