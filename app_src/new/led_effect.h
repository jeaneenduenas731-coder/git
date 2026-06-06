#ifndef __LED_EFFECT_H__
#define __LED_EFFECT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "add_status.h"


#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))

// TIM7
#define LED_NUM			(2)//

// TIM6
#define LED_NUM1		(48)//

// TIM5
#define LED_NUM2		(48)//

// TIM8
#define LED_NUM3		(2)//

typedef enum 
{
	LED_TIMER_7 = 0,		//开机灯效
	LED_TIMER_6,
	LED_TIMER_5,
	LED_TIMER_8,
	
	MAX_LED_TIMER,
}LED_TIMER;

typedef enum 
{
	LED_MODE_POWER_ON = 0,		//开机灯效
	
	LED_MODE_CHANGE1,	
	LED_MODE_CHANGE2,
	LED_MODE_CHANGE3,
	LED_MODE_CHANGE4,

	LED_MODE_CHANGE5,
	LED_MODE_CHANGE6,
	LED_MODE_CHANGE7,
	LED_MODE_CHANGE8,

	LED_MODE_CHANGE9,

	LED_MODE_CLEAR_LED,	//灭灯

	LED_MODE_CHANGE10,
	LED_MODE_POWER_OFF,	//关机灯效
	
	MAX_LED_MODE,
}LED_MODE;


enum LED_CONTRAL_TIME
{
    LED_CONTRAL_TIME1 	= 2,
	LED_CONTRAL_TIME2 	= 3,
	LED_CONTRAL_TIME3 	= 5,
	LED_CONTRAL_TIME4	= 7,
	LED_CONTRAL_TIME5 	= 10,
	LED_CONTRAL_TIME6	= 20,
	LED_CONTRAL_TIME7	= 50,//
	LED_CONTRAL_TIME8	= 100, 
	LED_CONTRAL_TIME9 	= 200,
	LED_CONTRAL_TIME10	= 500,
	LED_CONTRAL_TIME11 	= 1000,
	LED_CONTRAL_TIME12 	= 2000,
	LED_CONTRAL_TIME13 	= 5000,
};

extern const uint8_t grb3[];
extern const uint8_t color_base[];
extern const uint8_t grb[];
extern const uint8_t r_1[];
extern const uint8_t g_1[];
extern const uint8_t b_1[];

extern void pwm_led_Init(void);
extern void CheckRgbLedEffect(void);
extern void led_param_init(void);
extern void LedEffectOff(uint8_t tws);
extern void LedEffectIODeInit(void);
extern void LedEffectNext(void);
extern void LedEffectSwitch(LED_MODE led_mode, uint8_t display, uint8_t tws);
extern void LedEffectSwitchOther(LED_TYPE led_mode);

#endif

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
