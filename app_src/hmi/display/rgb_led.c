#include "app_config.h"
#include "pwm.h"
#include <string.h>

#define		RGB_G_PWM_CH			TIMER6_PWM_A1_A9_A10_A23_A24_A28
#define		RGB_G_PWM_GPIO			0 		//A1
#define		RGB_G_PWM_TIMER			TIMER6

#define		RGB_R_PWM_CH			TIMER5_PWM_A0_A7_A10_A22_A24
#define		RGB_R_PWM_GPIO			0 		//A0
#define		RGB_R_PWM_TIMER			TIMER5

#define		RGB_B_PWM_CH			TIMER7_PWM_A3_A5_A20_B4
#define		RGB_B_PWM_GPIO			1 		//A5
#define		RGB_B_PWM_TIMER			TIMER7


#define 	SetLedPwmDuty(rgb,duty)	PWM_Disable(rgb),\
									RgbLed.PWMParam.Duty = duty,\
									PWM_Config(rgb,&RgbLed.PWMParam),\
									PWM_Enable(rgb)

#define 	RGB_PWM_MAX_DUTY		80 //呼吸灯最大亮度
#define 	RGB_PWM_MIN_DUTY		5  //呼吸灯最小亮度
#define 	RGB_PWM_OPEN_DUTY		65 //灯亮度
#define 	RGB_PWM_CLOSE_DUTY		0  //关闭

#define 	RGB_MODE_SET_MENU_EXIT_TIMER	10000

typedef enum _RGB_LIGHT_MODE
{
	RGB_LED_WHITE_BREATHING = 0,//白色呼吸灯
	RGB_LED_RED_BREATHING,		//红色呼吸灯
	RGB_LED_GREEN_BREATHING,	//绿色呼吸灯
	RGB_LED_BLUE_BREATHING,		//蓝色呼吸灯
	RGB_LED_FLOW_LIGHT,			//跑马灯
	RGB_LED_MONO_LOOP,			//单色循环灯
	RGB_LED_CLOSE,				//关闭
	RGB_LED_MAX_MODE
}RGB_LIGHT_MODE;

static struct
{
	PWM_StructInit  PWMParam;
	RGB_LIGHT_MODE	mode;

	uint8_t			r_duty;	//占空比 RGB_PWM_MIN_DUTY/RGB_PWM_MAX_DUTY
	uint8_t			g_duty;
	uint8_t			b_duty;

	uint8_t			breathing_dir;
	uint16_t		delay_cnt;
	uint16_t		loop;

	uint16_t		MenuTimer;
}RgbLed;


void RgbLedInit(void)
{
	memset((void *)(&RgbLed),0,sizeof(RgbLed));

	RgbLed.PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	RgbLed.PWMParam.OutputType    			= PWM_OUTPUT_SINGLE_1;
	RgbLed.PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	RgbLed.PWMParam.FreqDiv					= 400;
	RgbLed.PWMParam.Duty			        = RGB_PWM_CLOSE_DUTY;

	PWM_GpioConfig(RGB_R_PWM_CH,RGB_R_PWM_GPIO,PWM_IO_MODE_OUT);
	PWM_GpioConfig(RGB_G_PWM_CH,RGB_G_PWM_GPIO,PWM_IO_MODE_OUT);
	PWM_GpioConfig(RGB_B_PWM_CH,RGB_B_PWM_GPIO,PWM_IO_MODE_OUT);

	SetLedPwmDuty(RGB_R_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
	SetLedPwmDuty(RGB_G_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
	SetLedPwmDuty(RGB_B_PWM_TIMER,RGB_PWM_CLOSE_DUTY);

	RgbLed.MenuTimer = 0;
}

bool IsRgbLedModeMenuExit(void)
{
	return (RgbLed.MenuTimer == 0);
}

void RgbLedModeMenuEnter(void)
{
	RgbLed.MenuTimer = RGB_MODE_SET_MENU_EXIT_TIMER;
}

void RgbLedModeMenuExit(void)
{
	RgbLed.MenuTimer = 0;
}

static void RgbLedSet(void)
{
	switch(RgbLed.mode)
	{
	default:
		break;
	case RGB_LED_WHITE_BREATHING:	//白色呼吸灯
	case RGB_LED_RED_BREATHING:		//红色呼吸灯
	case RGB_LED_GREEN_BREATHING:	//绿色呼吸灯
	case RGB_LED_BLUE_BREATHING:	//蓝色呼吸灯
		RgbLed.r_duty = RGB_PWM_MIN_DUTY;
		RgbLed.g_duty = RGB_PWM_MIN_DUTY;
		RgbLed.b_duty = RGB_PWM_MIN_DUTY;
		RgbLed.breathing_dir = 1;
		SetLedPwmDuty(RGB_R_PWM_TIMER,RgbLed.r_duty);
		SetLedPwmDuty(RGB_G_PWM_TIMER,RgbLed.g_duty);
		SetLedPwmDuty(RGB_B_PWM_TIMER,RgbLed.b_duty);
		break;
	case RGB_LED_FLOW_LIGHT:		//跑马灯
		RgbLed.loop = 0;
		RgbLed.breathing_dir = 1;
		RgbLed.r_duty = RGB_PWM_OPEN_DUTY;
		RgbLed.g_duty = RGB_PWM_OPEN_DUTY;
		RgbLed.b_duty = RGB_PWM_OPEN_DUTY;
		SetLedPwmDuty(RGB_R_PWM_TIMER,RGB_PWM_OPEN_DUTY);
		SetLedPwmDuty(RGB_G_PWM_TIMER,RGB_PWM_OPEN_DUTY);
		SetLedPwmDuty(RGB_B_PWM_TIMER,RGB_PWM_OPEN_DUTY);
		break;
	case RGB_LED_MONO_LOOP:			//单色循环灯
		RgbLed.loop = 0;
		SetLedPwmDuty(RGB_R_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		SetLedPwmDuty(RGB_G_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		SetLedPwmDuty(RGB_B_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		break;
	case RGB_LED_CLOSE:				//关闭
		SetLedPwmDuty(RGB_R_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		SetLedPwmDuty(RGB_G_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		SetLedPwmDuty(RGB_B_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		break;
	}
}

uint8_t GetRgbLedMode(void)
{
	return RgbLed.mode;
}

void SetRgbLedMode(uint8_t mode)
{
	if(RgbLed.mode < RGB_LED_MAX_MODE)
		RgbLed.mode = mode;
	RgbLedSet();
	DBG("SetRgbLedMode --- RgbLed.mode = %d\n",RgbLed.mode);
}

void RgbLedModeSet(void)
{
	RgbLed.mode = (RgbLed.mode + 1) % RGB_LED_MAX_MODE;

	DBG("RgbLed.mode = %d\n",RgbLed.mode);

	RgbLed.MenuTimer = RGB_MODE_SET_MENU_EXIT_TIMER;
	RgbLed.delay_cnt = 0;

	RgbLedSet();
}


void RgbLed1MsInterrupt(void)
{
	if(RgbLed.MenuTimer)
		RgbLed.MenuTimer--;

	switch(RgbLed.mode)
	{
	default:
		break;
	case RGB_LED_WHITE_BREATHING:	//白色呼吸灯
	case RGB_LED_RED_BREATHING:		//红色呼吸灯
	case RGB_LED_GREEN_BREATHING:	//绿色呼吸灯
	case RGB_LED_BLUE_BREATHING:	//蓝色呼吸灯
		if(++RgbLed.delay_cnt > 20)
		{
			RgbLed.delay_cnt = 0;
			if(RgbLed.breathing_dir)
			{
				if(++RgbLed.r_duty > RGB_PWM_MAX_DUTY)
				{
					RgbLed.breathing_dir = 0;
					RgbLed.r_duty = RGB_PWM_MAX_DUTY;
				}
			}
			else
			{
				if(--RgbLed.r_duty < RGB_PWM_MIN_DUTY)
				{
					RgbLed.breathing_dir = 1;
					RgbLed.r_duty = RGB_PWM_MIN_DUTY;
				}
			}
			if(RgbLed.mode == RGB_LED_WHITE_BREATHING || RgbLed.mode == RGB_LED_RED_BREATHING)
				SetLedPwmDuty(RGB_R_PWM_TIMER,RgbLed.r_duty);
			else
				SetLedPwmDuty(RGB_R_PWM_TIMER,RGB_PWM_CLOSE_DUTY);

			if(RgbLed.mode == RGB_LED_WHITE_BREATHING || RgbLed.mode == RGB_LED_GREEN_BREATHING)
				SetLedPwmDuty(RGB_G_PWM_TIMER,RgbLed.r_duty);//复用r_duty数据
			else
				SetLedPwmDuty(RGB_G_PWM_TIMER,RGB_PWM_CLOSE_DUTY);

			if(RgbLed.mode == RGB_LED_WHITE_BREATHING || RgbLed.mode == RGB_LED_BLUE_BREATHING)
				SetLedPwmDuty(RGB_B_PWM_TIMER,RgbLed.r_duty);//复用r_duty数据
			else
				SetLedPwmDuty(RGB_B_PWM_TIMER,RGB_PWM_CLOSE_DUTY);
		}
		break;
	case RGB_LED_FLOW_LIGHT:		//跑马灯
		if(++RgbLed.delay_cnt > 20)
		{
			RgbLed.delay_cnt = 0;
			switch(RgbLed.loop)
			{
			default:
				RgbLed.loop = 0;
			case 0:
				RgbLed.r_duty = RGB_PWM_OPEN_DUTY;
				if(RgbLed.g_duty > RGB_PWM_CLOSE_DUTY)
					RgbLed.g_duty--;
				else if(RgbLed.b_duty > RGB_PWM_CLOSE_DUTY)
					RgbLed.b_duty--;
				else
				{
					RgbLed.loop = 1;
				}
				break;
			case 1:
				if(RgbLed.g_duty < RGB_PWM_OPEN_DUTY)
					RgbLed.g_duty++;
				else if(RgbLed.r_duty > RGB_PWM_CLOSE_DUTY)
					RgbLed.r_duty--;
				else
				{
					RgbLed.loop = 2;
				}
				break;
			case 2:
				if(RgbLed.b_duty < RGB_PWM_OPEN_DUTY)
					RgbLed.b_duty++;
				else if(RgbLed.g_duty > RGB_PWM_CLOSE_DUTY)
					RgbLed.g_duty--;
				else
				{
					RgbLed.loop = 3;
				}
				break;
			case 3:
				if(RgbLed.g_duty < RGB_PWM_OPEN_DUTY)
					RgbLed.g_duty++;
				else if(RgbLed.r_duty < RGB_PWM_OPEN_DUTY)
					RgbLed.r_duty++;
				else
				{
					RgbLed.loop = 0;
				}
				break;
			}
			SetLedPwmDuty(RGB_R_PWM_TIMER,RgbLed.r_duty);
			SetLedPwmDuty(RGB_G_PWM_TIMER,RgbLed.g_duty);
			SetLedPwmDuty(RGB_B_PWM_TIMER,RgbLed.b_duty);
		}
		break;
	case RGB_LED_MONO_LOOP:			//单色循环灯
		if(++RgbLed.delay_cnt > 500)
		{
			RgbLed.delay_cnt = 0;

			if(RgbLed.loop == 0 || RgbLed.loop == 3 || RgbLed.loop == 4 || RgbLed.loop == 6)
				RgbLed.r_duty = RGB_PWM_OPEN_DUTY;
			else
				RgbLed.r_duty = RGB_PWM_CLOSE_DUTY;

			if(RgbLed.loop == 1 || RgbLed.loop == 3 || RgbLed.loop == 5 || RgbLed.loop == 6)
				RgbLed.g_duty = RGB_PWM_OPEN_DUTY;
			else
				RgbLed.g_duty = RGB_PWM_CLOSE_DUTY;

			if(RgbLed.loop == 2 || RgbLed.loop == 4 || RgbLed.loop == 5 || RgbLed.loop == 6)
				RgbLed.b_duty = RGB_PWM_OPEN_DUTY;
			else
				RgbLed.b_duty = RGB_PWM_CLOSE_DUTY;

			SetLedPwmDuty(RGB_R_PWM_TIMER,RgbLed.r_duty);
			SetLedPwmDuty(RGB_G_PWM_TIMER,RgbLed.g_duty);
			SetLedPwmDuty(RGB_B_PWM_TIMER,RgbLed.b_duty);
			RgbLed.loop = (RgbLed.loop + 1) % 7;
		}
		break;
	case RGB_LED_CLOSE:				//关闭
		break;
	}
}

