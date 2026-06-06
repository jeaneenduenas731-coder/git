/**
 **************************************************************************************
 * @file    adc_key.c
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-01-11 17:30:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "app_config.h"
//driver
#include "adc.h"
#include "clk.h"
#include "gpio.h"
#include "timeout.h"
#include "adc_key.h"
#include "debug.h"
#include "sadc_interface.h"
#include "rtos_api.h"
#include "add_status.h"


#define     NORMAL_ADKEY                (0)    ///1 = 标准的ADKEY值处理，0 = 用户自定义ADKEY值处理


#define 	ADC_KEY_SCAN_TIME			10
#define 	ADC_KEY_JITTER_TIME			30
#define 	ADC_KEY_CP_TIME				1000 	//for hold or long press
#define 	ADC_KEY_CPH_TIME			200		//for long long press
#define		ADC_KEY_FULL_VAL			4096
#define 	ADC_KEY_OFFEST				11
#define	    ADC_KEY_COUNT				3

#define		ADC_VAL(val)				(val*4096/330)


#define 	PWR_ADCKEY_COUNT			1

#if NORMAL_ADKEY
const uint16_t	PWR_ADCKEY_VAL_TAB[PWR_ADCKEY_COUNT] =
{
	(ADC_VAL(28) + ADC_VAL(53))/2,
	(ADC_VAL(53) + ADC_VAL(77))/2,
	(ADC_VAL(77) + ADC_VAL(105))/2,
	(ADC_VAL(105) + ADC_VAL(129))/2,
	(ADC_VAL(129) + ADC_VAL(158))/2,
	(ADC_VAL(158) + ADC_VAL(189))/2,
	(ADC_VAL(189) + ADC_VAL(215))/2,
	(ADC_VAL(215) + ADC_VAL(242))/2,
	(ADC_VAL(242) + ADC_VAL(272))/2,
	(ADC_VAL(272) + ADC_VAL(300))/2,
	(ADC_VAL(300) + ADC_VAL(330))/2,
};
#else
/*
power
380
*/
const uint16_t	PWR_ADCKEY_VAL_TAB[PWR_ADCKEY_COUNT] =
{
	1000,
};
#endif

#if NORMAL_ADKEY
	#define USER_ADCKEY_VAL_TAB	NULL
#else
	#define ADKEY_0    (ADC_VAL(0)+ADC_VAL(30))/2
	#define ADKEY_1    (ADC_VAL(30)+ADC_VAL(60))/2
	#define ADKEY_2    (ADC_VAL(60)+ADC_VAL(85))/2
	#define ADKEY_3    (ADC_VAL(85)+ADC_VAL(110))/2
	#define ADKEY_4    (ADC_VAL(110)+ADC_VAL(140))/2
	#define ADKEY_5    (ADC_VAL(140)+ADC_VAL(165))/2
	#define ADKEY_6    (ADC_VAL(165)+ADC_VAL(180))/2
	#define ADKEY_7    (ADC_VAL(180)+ADC_VAL(220))/2
	#define ADKEY_8    (ADC_VAL(220)+ADC_VAL(230))/2
	#define ADKEY_9    (ADC_VAL(230)+ADC_VAL(260))/2
	#define ADKEY_10   (ADC_VAL(260)+ADC_VAL(290))/2
/*
mode  brightness	led
2195	1780		1490
*/
	const uint16_t UserADKey_Tab[ADC_KEY_COUNT]=
	{
		AVERAGE(1490, 1780),
		AVERAGE(1780, 2195),
		AVERAGE(2195, 2600),
	};

	#define USER_ADCKEY_VAL_TAB	UserADKey_Tab
#endif

typedef enum _ADC_KEY_STATE
{
	ADC_KEY_STATE_IDLE,
	ADC_KEY_STATE_JITTER,
	ADC_KEY_STATE_PRESS_DOWN,
	ADC_KEY_STATE_CP

} ADC_KEY_STATE;



#ifdef CFG_RES_ADC_KEY_SCAN
TIMER			AdcKeyWaitTimer;
//TIMER			AdcKeyScanTimer;
ADC_KEY_STATE	AdcKeyState;
static 	uint8_t 	PreKeyIndex = KEY_MSG_INDEX_EMPTY;

const struct
{
	uint8_t 			channel;
	uint8_t 			key_count;
	uint8_t 			key_index_offset;
	const uint16_t * 	key_adc_val_tab;
} AdcKeyScanMap[] =
{
#ifdef CFG_RES_POWERKEY_ADC_EN
	{ADC_CHANNEL_BK_ADKEY, 		PWR_ADCKEY_COUNT, 	0, 				PWR_ADCKEY_VAL_TAB},
#endif
#ifdef CFG_RES_ADC_KEY_PORT_CH1
	{CFG_RES_ADC_KEY_PORT_CH1,	ADC_KEY_COUNT,		ADC_KEY_OFFEST,	USER_ADCKEY_VAL_TAB},
#endif
#ifdef CFG_RES_ADC_KEY_PORT_CH2
	{CFG_RES_ADC_KEY_PORT_CH2,	ADC_KEY_COUNT,		ADC_KEY_COUNT*2,USER_ADCKEY_VAL_TAB},
#endif
};


void AdcKeyInit(void)
{
	AdcKeyState = ADC_KEY_STATE_IDLE;
	PreKeyIndex = KEY_MSG_INDEX_EMPTY;
	//TimeOutSet(&AdcKeyScanTimer, 0);

#ifdef CFG_RES_ADC_KEY_PORT_CH1
	GPIO_RegOneBitSet(CFG_RES_ADC_KEY_CH1_ANA_EN, CFG_RES_ADC_KEY_CH1_ANA_MASK);
#endif 

#ifdef CFG_RES_ADC_KEY_PORT_CH2
	GPIO_RegOneBitSet(CFG_RES_ADC_KEY_CH2_ANA_EN, CFG_RES_ADC_KEY_CH2_ANA_MASK);
#endif 

	return;
}

uint8_t GetAdcKeyIndex(void)
{
	uint8_t 	KeyIndex = KEY_MSG_INDEX_EMPTY;
	uint8_t 	i;
	uint16_t	Val;

	for(i=0;i<sizeof(AdcKeyScanMap)/sizeof(AdcKeyScanMap[0]);i++)
	{
#if 0
	   //若是用到复用的ADC口，例如A20和A23口做ADC，参考此代码
		if(  AdcKeyScanMap[i].channel == ADC_CHANNEL_GPIOA20_A23
		  && AdcKeyScanMap[i].key_index_offset == ADC_KEY_COUNT) //PORT_CH1 按键
		{
			GPIO_RegOneBitSet(GPIO_A_ANA_EN, GPIO_INDEX23);
			Val = ADC_SingleModeDataGet(AdcKeyScanMap[i].channel);
			GPIO_RegOneBitClear(GPIO_A_ANA_EN, GPIO_INDEX23);
			GPIO_RegOneBitSet(GPIO_A_ANA_EN, GPIO_INDEX20);
		}
		else
#else
		Val = ADC_SingleModeDataGet(AdcKeyScanMap[i].channel);
		// if(Val < 3800)
		// {
		// 	DBG("adc[%d] = %d\n", i, Val);
		// }
#endif
		if(Val < 3800)//(ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2))
		{
			if(AdcKeyScanMap[i].key_adc_val_tab)
			{
				uint8_t j;
				for(j = 0; j < AdcKeyScanMap[i].key_count; j++)
				{
					if(Val < AdcKeyScanMap[i].key_adc_val_tab[j])
					{
						KeyIndex = j;
						break;
					}
				}
			}
			else
			{
				KeyIndex = (Val + (ADC_KEY_FULL_VAL / AdcKeyScanMap[i].key_count) / 2) * AdcKeyScanMap[i].key_count / ADC_KEY_FULL_VAL;
			}

			if(KeyIndex != KEY_MSG_INDEX_EMPTY)
			{
				KeyIndex += AdcKeyScanMap[i].key_index_offset;
				break;
			}
		}
	}

	return KeyIndex;
}


/**
 * @func        AdcKeyScan
 * @brief       AdcKeyScan 按下有消抖，弹起没有消抖处理。
 * @param       void  
 * @Output      None
 * @return      AdcKeyMsg
 * @Others      
 * Record
 * 1.Date        : 20180123
 *   Author      : pi.wang
 *   Modification: Created function
*/
KeyScanMsg AdcKeyScan(void)
{
	uint8_t				KeyIndex=KEY_MSG_INDEX_EMPTY;
	KeyScanMsg Msg = {KEY_MSG_INDEX_EMPTY, KEY_UNKOWN_TYPE,ADC_KEY_SOURCE};

//	if(!IsTimeOut(&AdcKeyScanTimer))
//	{
//		return Msg;
//	}
//	TimeOutSet(&AdcKeyScanTimer, ADC_KEY_SCAN_TIME);
	
	KeyIndex = GetAdcKeyIndex();
	
	switch(AdcKeyState)
	{
		case ADC_KEY_STATE_IDLE:
			if(KeyIndex == KEY_MSG_INDEX_EMPTY)
			{
				return Msg;
			}
			PreKeyIndex = KeyIndex;
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_JITTER_TIME);
			//APP_DBG("GOTO JITTER!\n");
			AdcKeyState = ADC_KEY_STATE_JITTER;

		case ADC_KEY_STATE_JITTER:
			if(PreKeyIndex != KeyIndex)
			{
				//APP_DBG("GOTO IDLE Because jitter!\n");
				PreKeyIndex = KEY_MSG_INDEX_EMPTY;
				AdcKeyState = ADC_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//APP_DBG("GOTO PRESS_DOWN!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CP_TIME);
				AdcKeyState = ADC_KEY_STATE_PRESS_DOWN;
				Msg.index = PreKeyIndex;
				Msg.type = KEY_PRESSED;
			}
			break;

		case ADC_KEY_STATE_PRESS_DOWN:
			if(PreKeyIndex != KeyIndex)
			{
				//APP_DBG("key release %d\n", PreKeyIndex);
				AdcKeyState = ADC_KEY_STATE_IDLE;
				Msg.index = PreKeyIndex;
				Msg.type = KEY_RELEASED;
				PreKeyIndex = KEY_MSG_INDEX_EMPTY;
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cp value
				//APP_DBG("ADC KEY CPS!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				AdcKeyState = ADC_KEY_STATE_CP;
				Msg.index = PreKeyIndex;
				Msg.type = KEY_LONG_PRESSED;
			}
			break;

		case ADC_KEY_STATE_CP:
			if(PreKeyIndex != KeyIndex)
			{
				//return key cp value
				//APP_DBG("ADC KEY CPR!\n");
				AdcKeyState = ADC_KEY_STATE_IDLE;
				Msg.index = PreKeyIndex;
				Msg.type  = KEY_LONG_RELEASED;
				PreKeyIndex = KEY_MSG_INDEX_EMPTY;
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cph value
				//APP_DBG("ADC KEY CPH!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				Msg.index = PreKeyIndex;
				Msg.type  = KEY_LONG_PRESS_HOLD;
			}
			break;

		default:
			AdcKeyState = ADC_KEY_STATE_IDLE;
			PreKeyIndex = KEY_MSG_INDEX_EMPTY;
			break;
	}

	return Msg;
}

#endif //CFG_RES_ADC_KEY_SCAN


