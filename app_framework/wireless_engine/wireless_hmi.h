/*
 * wireless_hmi.h
 *
 *  Created on: Jun 9, 2025
 *      Author: piwang
 */

#ifndef _WIRELESS_HMI_H_
#define _WIRELESS_HMI_H_

/**Porting from key.h@2.4G SDK**/
//TX端控制RX端的命令，通过一个字节的cmd来实现
#include "app_config.h"
#include "type.h"
#define	INFO_BIT_BLUENS		1
//#define INFO_BIT_BATLOW		2
#define INFO_BIT_MASK		0x0f
#define INFO_BIT_SYNC		0xa0
extern uint8_t InfoBits;

void InfoBitsSet(uint8_t Flag);
void InfoBitsClear(uint8_t Flag);
uint8_t InfoBitsGet(uint8_t Flag);
uint8_t InfoGet(void);
/**********End of key.h@2.4G SDK***********/

#endif /* _WIRELESS_HMI_H_ */
