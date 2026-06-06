#include "main_task.h"
#include "debug.h"
#include "timer.h"
#include "pwm.h"
#include "dma.h"
#include "add_status.h"
#include "breakpoint.h"
#include <string.h>
#include "led_effect.h"
#include "remind_sound.h"
#include "bt_manager.h"
#include "ctrlvars.h"

#ifdef BT_TWS_SUPPORT
#include "bt_tws_api.h"
#endif


#ifdef CFG_PWM_LED_EN

/**********LED参数设置********************************/

#if (SYS_CORE_DPLL_FREQ == 240*1000)

#define LED_T		150		//200		//LED灯珠单个bit位周期（额定为1.25us）T=LED_T/120000000,系统时钟为120M
#define RE			0		//RES信号占空比为0
#define HT			100		//128  		//逻辑bit为1的占空比
#define LT			50		//64	  	//逻辑bit为0的占空比

#elif (SYS_CORE_DPLL_FREQ == 288*1000)

#define LED_T		180		//200		//LED灯珠单个bit位周期（额定为1.25us）T=LED_T/120000000,系统时钟为120M
#define RE			0		//RES信号占空比为0
#define HT			120		//128  		//逻辑bit为1的占空比
#define LT			60		//64	  	//逻辑bit为0的占空比

#elif (SYS_CORE_DPLL_FREQ == 360*1000)

#define LED_T		225		//200		//LED灯珠单个bit位周期（额定为1.25us）T=LED_T/120000000,系统时钟为120M
#define RE			0		//RES信号占空比为0
#define HT			150		//128  		//逻辑bit为1的占空比
#define LT			75		//64	  	//逻辑bit为0的占空比

#endif


#ifdef PWM7_LED
#define BUF_LEN		LED_NUM*3*8+50	//刷灯条数据的长度，加的50为前48个以及最后2个数据为0，用于模拟RES信号
static uint8_t LED_DATA_1[LED_NUM * 3]	= {0x00};
static uint8_t LedBufA[BUF_LEN] 		= {0x0};
static uint8_t LedBufB[BUF_LEN] 		= {0x0};
#else
static uint8_t LED_DATA_1[LED_NUM * 3]		= {0x00};
#endif

#ifdef PWM6_LED
#define BUF_LEN1	LED_NUM1*3*8+50	//刷灯条数据的长度，加的50为前48个以及最后2个数据为0，用于模拟RES信号
static uint8_t LED_DATA_2[LED_NUM1 * 3]	= {0x00};
static uint8_t LedBufC[BUF_LEN1] 		= {0x0};
static uint8_t LedBufD[BUF_LEN1] 		= {0x0};
#else
static uint8_t LED_DATA_2[LED_NUM1 * 3]		= {0x00};
#endif

#ifdef PWM5_LED
#define BUF_LEN2	LED_NUM2*3*8+50	//刷灯条数据的长度，加的50为前48个以及最后2个数据为0，用于模拟RES信号
static uint8_t LED_DATA_3[LED_NUM2 * 3]	= {0x00};
static uint8_t LedBufE[BUF_LEN2] 		= {0x00};
static uint8_t LedBufF[BUF_LEN2] 		= {0x00};
#else
static uint8_t LED_DATA_3[LED_NUM2 * 3]		= {0x00};
#endif

#ifdef PWM8_LED
#define BUF_LEN3	LED_NUM3*3*8+320	//刷灯条数据的长度，加的50为前48个以及最后2个数据为0，用于模拟RES信号
static uint8_t LED_DATA_4[LED_NUM3 * 3]	= {0x00};
static uint8_t LedBufG[BUF_LEN3] 		= {0x00};
static uint8_t LedBufH[BUF_LEN3] 		= {0x00};
#else
static uint8_t LED_DATA_4[LED_NUM3 * 3]		= {0x00};
#endif

static void pwm7_led_ctrl(uint8_t *data_led);	
static void pwm6_led_ctrl(uint8_t *data_led);	
static void pwm5_led_ctrl(uint8_t *data_led);
static void pwm8_led_ctrl(uint8_t *data_led);	

//修改
// 新增两个静态变量（需要在文件开头或函数内定义为static）
//static int direction = 1;      // 流水方向：1=正向，-1=反向
//static int flow_count = 0;     // 流水次数计数

// 或者在函数外定义全局变量
// int direction = 1;
// int flow_count = 0;


TIMER 	delay_led_on_timer;
TIMER 	delay_led_on_timer1;

//在 led_param_init 有初始化
TIMER 	led_switch_timer;
TIMER 	led_switch_timer1;
TIMER 	led_switch_timer2;
TIMER 	led_switch_timer3;
TIMER 	led_switch_timer4;

uint8_t if_refresh_led_data = FALSE;

bool led_exchange_flag = FALSE;
bool led_exchange_flag1 = FALSE;
bool led_exchange_flag2 = FALSE;
bool led_exchange_flag3 = FALSE;

bool 	if_add = FALSE;   //灯管亮度控制
bool 	if_add1 = FALSE;   //灯管亮度控制

bool 	if_sub = FALSE;

uint8_t if_accelerate = FALSE;

uint8_t light_persent = 1;  //灯光亮度

uint8_t light_persent_base = 30;  //灯光亮度

uint16_t color_start_index = 0;
uint16_t color_start_index1 = 0;
uint16_t color_start_index2 = 0;

#define LED_AUDIO_LEVEL_MAX			48
#define LED_AUDIO_RAW_GATE			500
#define LED_AUDIO_RAW_LOW			1800
#define LED_AUDIO_RAW_MID			5000
#define LED_AUDIO_RAW_HIGH			12000
#define LED_AUDIO_RAW_FULL			22000
#define LED_AUDIO_FILTER_SHIFT		2
#define LED_AUDIO_RISE_STEP			3
#define LED_AUDIO_FALL_STEP			2

static uint32_t led_audio_filtered_music = 0;
static uint32_t led_audio_filtered_mic = 0;
static uint8_t led_audio_level_music = 0;
static uint8_t led_audio_level_mic = 0;

static uint8_t LedAudioLevelLimit(uint8_t current, uint8_t target)
{
	if(target > current)
	{
		uint8_t delta = target - current;
		return current + ((delta > LED_AUDIO_RISE_STEP) ? LED_AUDIO_RISE_STEP : delta);
	}
	else if(current > target)
	{
		uint8_t delta = current - target;
		return current - ((delta > LED_AUDIO_FALL_STEP) ? LED_AUDIO_FALL_STEP : delta);
	}

	return current;
}

static uint8_t LedAudioRawToLevel(uint32_t raw)
{
	if(raw <= LED_AUDIO_RAW_GATE)
	{
		return 0;
	}
	else if(raw <= LED_AUDIO_RAW_LOW)
	{
		return (uint8_t)(1 + ((raw - LED_AUDIO_RAW_GATE) * 11 / (LED_AUDIO_RAW_LOW - LED_AUDIO_RAW_GATE)));
	}
	else if(raw <= LED_AUDIO_RAW_MID)
	{
		return (uint8_t)(12 + ((raw - LED_AUDIO_RAW_LOW) * 16 / (LED_AUDIO_RAW_MID - LED_AUDIO_RAW_LOW)));
	}
	else if(raw <= LED_AUDIO_RAW_HIGH)
	{
		return (uint8_t)(28 + ((raw - LED_AUDIO_RAW_MID) * 14 / (LED_AUDIO_RAW_HIGH - LED_AUDIO_RAW_MID)));
	}
	else if(raw <= LED_AUDIO_RAW_FULL)
	{
		return (uint8_t)(42 + ((raw - LED_AUDIO_RAW_HIGH) * 5 / (LED_AUDIO_RAW_FULL - LED_AUDIO_RAW_HIGH)));
	}

	return LED_AUDIO_LEVEL_MAX;
}

static uint8_t LedAudioLevelGet(LED_TYPE channel)
{
	uint32_t raw = GetAudioSdct(channel);
	uint32_t *filtered = &led_audio_filtered_music;
	uint8_t *level = &led_audio_level_music;

	if(channel == MIC_VOL_TYPE)
	{
		filtered = &led_audio_filtered_mic;
		level = &led_audio_level_mic;
	}

	if(raw <= LED_AUDIO_RAW_GATE)
	{
		*filtered = 0;
		*level = 0;
		return 0;
	}

	*filtered = ((*filtered * 3) + raw) >> LED_AUDIO_FILTER_SHIFT;
	uint8_t target = LedAudioRawToLevel(*filtered);
	*level = LedAudioLevelLimit(*level, target);

	if(*level > LED_AUDIO_LEVEL_MAX)
	{
		*level = LED_AUDIO_LEVEL_MAX;
	}

	return *level;
}
short color_contral_param = 0;
short color_contral_param1 = 0;
short color_contral_param2 = 0;
short color_contral_param3 = 0;
short color_contral_param4 = 0;
short color_contral_param5 = 0;
short color_contral_param6 = 0;
short color_contral_param7 = 0;

short color_contral_tmp = 0;
short color_contral_tmp1 = 0;
short color_contral_tmp2 = 0;
short color_contral_tmp3 = 0;
short color_contral_tmp4 = 0;
short color_contral_tmp5 = 0;

bool led_exchange_flag_other = FALSE;
short color_contral_param_other1 = 0;
short color_contral_param_other2 = 0;
short color_contral_param_other3 = 0;
short color_contral_param_other4 = 0;

short color_contral_tmp_other1 = 0;
short color_contral_tmp_other2 = 0;
short color_contral_tmp_other3 = 0;
short color_contral_tmp_other4 = 0;

#define ClearLedDataPwm7() memset(LED_DATA_1, 0, sizeof(LED_DATA_1))
#define ClearLedDataPwm6() memset(LED_DATA_2, 0, sizeof(LED_DATA_2))
#define ClearLedDataPwm5() memset(LED_DATA_3, 0, sizeof(LED_DATA_3))
#define ClearLedDataPwm8() memset(LED_DATA_4, 0, sizeof(LED_DATA_4))

void ClearLedDataAll(void)
{
	#ifdef PWM7_LED
	ClearLedDataPwm7();
	#endif
	
	#ifdef PWM6_LED
	ClearLedDataPwm6();
	#endif
	
	#ifdef PWM5_LED
	ClearLedDataPwm5();
	#endif
	
	#ifdef PWM8_LED
	ClearLedDataPwm8();
	#endif
}

void led_param_init(void)
{
	led_exchange_flag = TRUE;
	led_exchange_flag1 = TRUE;
	led_exchange_flag2 = TRUE;
	
	if_add = FALSE;   //灯管亮度控制
	if_add1 = FALSE;
	
	if_sub = FALSE;
	if_accelerate = FALSE;
	
	light_persent = 1;	//灯光亮度
	
	light_persent_base = 30;  //灯光亮度
	
	color_start_index = 0;
	color_start_index1 = 0;
	color_start_index2 = 0;
	
	color_contral_param = 0;
	color_contral_param1 = 0;
	color_contral_param2 = 0;
	color_contral_param3 = 0;
	color_contral_param4 = 0;
	color_contral_param5 = 0;
	color_contral_param6 = 0;
	color_contral_param7 = 0;
	
	color_contral_tmp = 0;
	color_contral_tmp1 = 0;
	color_contral_tmp2 = 0;
	color_contral_tmp3 = 0;
	color_contral_tmp4 = 0;	
	color_contral_tmp5 = 0;

	TimeOutSet(&led_switch_timer, 0);
	TimeOutSet(&led_switch_timer1, 0);
	TimeOutSet(&led_switch_timer2, 0);
	TimeOutSet(&led_switch_timer3, 0);
}

void LedEffectMusicVolum(uint8_t volum)
{
	if(led_exchange_flag_other)
	{
		ClearLedDataAll();
		led_exchange_flag_other = FALSE;
		color_contral_param_other1 = 0;
		color_contral_param_other2 = 0;
		color_contral_param_other3 = 0;
		color_contral_param_other4 = 5;

		color_contral_tmp_other1 = 0;
		color_contral_tmp_other2 = 0;
		color_contral_tmp_other3 = 0;
		color_contral_tmp_other4 = 0;

		TimeOutSet(&led_switch_timer4, 0);
		DBG("--------LedEffectMusicVolum--------%d\n", volum);
	}
	if(IsTimeOut(&led_switch_timer4))
	{
		ClearLedDataAll();
		if_refresh_led_data = 0xFF;
		switch (volum)
		{
		case 0:
			color_contral_param_other1 = 0;
			break;

		case 1 ... 3:
			color_contral_param_other1 = 1;
			break;

		case 4 ... 6:
			color_contral_param_other1 = 2;
			break;

		case 7 ... 9:
			color_contral_param_other1 = 3;
			break;

		case 10 ... 12:
			color_contral_param_other1 = 4;
			break;
			
		case 13 ... 15:
			color_contral_param_other1 = 5;
			break;
			
		case 16:
			color_contral_param_other1 = 6;
			break;
		}

		if(volum == 16)
		{
			for(uint8_t i = 0; i < color_contral_param_other1; i++)
			{
				LED_DATA_3[i * 3 + 0] = 0;
				LED_DATA_3[i * 3 + 1] = 0;
				LED_DATA_3[i * 3 + 2] = 255 * userVar.brightness / 100;
			}
		}
		else if(volum == 0)
		{
			ClearLedDataAll();
		}
		else
		{
			for(uint8_t i = 0; i < (color_contral_param_other1 - 1); i++)
			{
				LED_DATA_3[i * 3 + 0] = 0;
				LED_DATA_3[i * 3 + 1] = 0;
				LED_DATA_3[i * 3 + 2] = 255 * userVar.brightness / 100;
			}
			if(volum % 3 == 0)
			{
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 0] = 0;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 1] = 0;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 2] = 255 * userVar.brightness / 100;
			}
			else if(volum % 3 == 1)
			{
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 0] = 0;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 1] = 255 * userVar.brightness / 100;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 2] = 0;
			}
			else if(volum % 3 == 2)
			{
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 0] = 255 * userVar.brightness / 100;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 1] = 0;
				LED_DATA_3[(color_contral_param_other1 - 1) * 3 + 2] = 0;
			}
		}
		
		TimeOutSet(&led_switch_timer4, LED_CONTRAL_TIME8);
	}
}

void LedEffectBtConnect(uint8_t flag)
{
	if(led_exchange_flag_other)
	{
		ClearLedDataAll();
		led_exchange_flag_other = FALSE;
		color_contral_param_other1 = 0;
		color_contral_param_other2 = 0;
		color_contral_param_other3 = 0;
		color_contral_param_other4 = 5;

		color_contral_tmp_other1 = 0;
		color_contral_tmp_other2 = 0;
		color_contral_tmp_other3 = 0;
		color_contral_tmp_other4 = 0;

		TimeOutSet(&led_switch_timer4, 0);
		DBG("--------LedEffectBtConnect--------\n");
	}
	if(IsTimeOut(&led_switch_timer4))
	{
		if(!color_contral_tmp_other3) ClearLedDataAll();
		if_refresh_led_data = 0xFF;
		
		if(color_contral_param_other1 < color_contral_param_other4)
		{
			color_contral_param_other1++;
		}
		else
		{
			color_contral_param_other4--;
			color_contral_param_other1 = -1;
		}

		for(uint8_t i = color_contral_param_other4 + 1; i < 6; i++)
		{
			color_contral_tmp_other1 = (5 - i) * 255;
			LED_DATA_3[i * 3 + 0] = grb[color_contral_tmp_other1 * 3 + 0] * userVar.brightness / 100;
			LED_DATA_3[i * 3 + 1] = grb[color_contral_tmp_other1 * 3 + 1] * userVar.brightness / 100;
			LED_DATA_3[i * 3 + 2] = grb[color_contral_tmp_other1 * 3 + 2] * userVar.brightness / 100;
		}

		if(color_contral_param_other4 == -1)
		{
			if(!color_contral_tmp_other3) 
			{
				TimeOutSet(&userVar.OtherLedTimer, LED_CONTRAL_TIME10);
				color_contral_tmp_other3 = 1;
			}
			TimeOutSet(&led_switch_timer4, LED_CONTRAL_TIME8);
			return;
		}

		color_contral_tmp_other1 = (5 - color_contral_param_other1) * 255;
		LED_DATA_3[color_contral_param_other1 * 3 + 0] = grb[color_contral_tmp_other1 * 3 + 0] * userVar.brightness / 100;
		LED_DATA_3[color_contral_param_other1 * 3 + 1] = grb[color_contral_tmp_other1 * 3 + 1] * userVar.brightness / 100;
		LED_DATA_3[color_contral_param_other1 * 3 + 2] = grb[color_contral_tmp_other1 * 3 + 2] * userVar.brightness / 100;
		
		TimeOutSet(&led_switch_timer4, LED_CONTRAL_TIME8);
	}
}

void LedEffectLedBrightness(uint8_t brightness)
{
	
	if(led_exchange_flag_other)
	{
		ClearLedDataAll();
		led_exchange_flag_other = FALSE;
		color_contral_param_other1 = 0;
		color_contral_param_other2 = 0;
		color_contral_param_other3 = 0;
		color_contral_param_other4 = 5;

		color_contral_tmp_other1 = 0;
		color_contral_tmp_other2 = 0;
		color_contral_tmp_other3 = 0;
		color_contral_tmp_other4 = 0;

		TimeOutSet(&led_switch_timer4, 0);
		DBG("--------LedEffectLedBrightness--------%d\n", brightness);
	}
	if(IsTimeOut(&led_switch_timer4))
	{
		ClearLedDataAll();
		if_refresh_led_data = 0xFF;

		// 根据亮度值计算LED_DATA_3要点亮的LED数量
		switch (brightness)
		{
		case 0 ... 20:     // 最低亮度
			color_contral_param_other1 = 10;
			break;

		case 21 ... 40:
			color_contral_param_other1 = 19;
			break;

		case 41 ... 60:
			color_contral_param_other1 = 29;
			break;

		case 61 ... 80:
			color_contral_param_other1 = 38;
			break;

		case 81 ... 100:   // 最高亮度
			color_contral_param_other1 = 48;
			break;
		}

		// LED_DATA_3 全亮（左侧48个灯珠）
		for(uint8_t i = 0; i < 48; i++)
		{
		    uint8_t r = 0, g = 0, b = 0;

		    if (i < 16)
		    {
		        // 红 → 绿 (0-15)
		        r = 255 - (i * 16);
		        g = i * 16;
		        b = 0;
		    }
		    else if (i < 32)
		    {
		        // 绿 → 蓝 (16-31)
		        r = 0;
		        g = 255 - ((i - 16) * 16);
		        b = (i - 16) * 16;
		    }
		    else
		    {
		        // 蓝 → 红 (32-47)
		        r = (i - 32) * 16;
		        g = 0;
		        b = 255 - ((i - 32) * 16);
		    }

		    LED_DATA_3[i * 3 + 0] = r * userVar.brightness / 100;
		    LED_DATA_3[i * 3 + 1] = g * userVar.brightness / 100;
		    LED_DATA_3[i * 3 + 2] = b * userVar.brightness / 100;
		}


		// ========== LED_DATA_2 根据亮度档位点亮对应数量的灯珠 ===========
		if(brightness == 100)
		{
		    // 直接复制 LED_DATA_3 的颜色到 LED_DATA_2
		    for(uint8_t i = 0; i < color_contral_param_other1; i++)
		    {
		        LED_DATA_2[i*3 + 0] = LED_DATA_3[i*3 + 0];
		        LED_DATA_2[i*3 + 1] = LED_DATA_3[i*3 + 1];
		        LED_DATA_2[i*3 + 2] = LED_DATA_3[i*3 + 2];
		    }
		}

		else if(brightness == 0)
		{
			// 亮度0%时：LED_DATA_2全部熄灭（但LED_DATA_3保持全亮）
			// 不需要额外操作，因为ClearLedDataAll已经清空
		}
		else
		{
		    for(uint8_t i = 0; i < color_contral_param_other1; i++)
		    {
		        uint8_t r = 0, g = 0, b = 0;
		        uint8_t pos = i % 48;

		        if (pos < 16)
		        {
		            r = 255 - (pos * 16);
		            g = pos * 16;
		            b = 0;
		        }
		        else if (pos < 32)
		        {
		            r = 0;
		            g = 255 - ((pos - 16) * 16);
		            b = (pos - 16) * 16;
		        }
		        else
		        {
		            r = (pos - 32) * 16;
		            g = 0;
		            b = 255 - ((pos - 32) * 16);
		        }

		        LED_DATA_2[i * 3 + 0] = r * userVar.brightness / 100;
		        LED_DATA_2[i * 3 + 1] = g * userVar.brightness / 100;
		        LED_DATA_2[i * 3 + 2] = b * userVar.brightness / 100;
		    }
		}
		TimeOutSet(&led_switch_timer4, LED_CONTRAL_TIME8);
	}
}
void LedEffectPowerOn()
{
    static uint8_t flow_phase = 0;      // 0:点亮阶段, 1:熄灭阶段
    static uint8_t led_index = 0;       // 当前操作的LED索引

    if (led_exchange_flag)
    {
        ClearLedDataAll();
        led_exchange_flag = FALSE;

        color_contral_param   = -1;
        color_contral_tmp     = 0;
        light_persent         = 0;
        flow_phase = 0;              // 点亮阶段
        led_index = 0;               // 从第0颗开始
        TimeOutSet(&led_switch_timer, 0);

        DBG("--------LedEffectPowerOn--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        if(flow_phase == 0)  // 点亮阶段
        {
            if(led_index < LED_NUM2)
            {
                // 点亮当前LED
                color_contral_tmp = led_index * 1530 / LED_NUM2;

                LED_DATA_3[led_index * 3 + 0] = grb[color_contral_tmp * 3 + 0] * userVar.brightness / 100;
                LED_DATA_3[led_index * 3 + 1] = grb[color_contral_tmp * 3 + 1] * userVar.brightness / 100;
                LED_DATA_3[led_index * 3 + 2] = grb[color_contral_tmp * 3 + 2] * userVar.brightness / 100;
                LED_DATA_2[led_index * 3 + 0] = grb[color_contral_tmp * 3 + 0] * userVar.brightness / 100;
                LED_DATA_2[led_index * 3 + 1] = grb[color_contral_tmp * 3 + 1] * userVar.brightness / 100;
                LED_DATA_2[led_index * 3 + 2] = grb[color_contral_tmp * 3 + 2] * userVar.brightness / 100;

                led_index++;
            }
            else
            {
                // 所有LED都已点亮，切换到熄灭阶段
                flow_phase = 1;
                led_index = 0;
                TimeOutSet(&led_switch_timer, 15);
                return;
            }
        }
        else  // 熄灭阶段
        {
            if(led_index < LED_NUM2)
            {
                // 熄灭当前LED
                LED_DATA_3[led_index * 3 + 0] = 0;
                LED_DATA_3[led_index * 3 + 1] = 0;
                LED_DATA_3[led_index * 3 + 2] = 0;
                LED_DATA_2[led_index * 3 + 0] = 0;
                LED_DATA_2[led_index * 3 + 1] = 0;
                LED_DATA_2[led_index * 3 + 2] = 0;

                led_index++;
            }
            else
            {
                // 所有LED都已熄灭，结束特效
                LedEffectSwitch(userVar.led_mode_bak, TRUE, TRUE);
                TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME10);
                return;
            }
        }

        TimeOutSet(&led_switch_timer, 15);
        if_refresh_led_data = 0xFF;
    }
}
void LedEffectPowerOff()
{
	if (led_exchange_flag)
	{
		ClearLedDataAll();
		led_exchange_flag = FALSE;
		
		color_contral_param 	= LED_NUM2;
		color_contral_tmp 		= 0;
		light_persent		 	= 0;
		TimeOutSet(&led_switch_timer, 0);

		for(uint8_t i = 0; i < LED_NUM2; i++)
		{
			color_contral_tmp = (11 - i) * 127;
			LED_DATA_3[i * 3 + 0] = grb[color_contral_tmp * 3 + 0]  * userVar.brightness / 100;
			LED_DATA_3[i * 3 + 1] = grb[color_contral_tmp * 3 + 0] * userVar.brightness / 100;
			LED_DATA_3[i * 3 + 2] = grb[color_contral_tmp * 3 + 0]  * userVar.brightness / 100;
		}

		RemindSoundServiceItemRequest(SOUND_REMIND_GUANJI, REMIND_PRIO_NORMAL);
		
		DBG("--------LedEffectPowerOff--------\n");
	}
	if (IsTimeOut(&led_switch_timer))
	{
		if(color_contral_param > 0)
		{
			color_contral_param--;
		}
		else
		{
			MainTaskMsgSend(MSG_POWERDOWN);//MSG_DEEPSLEEP
			TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME8);
			return;
		}

	
		if_refresh_led_data = 0xFF;

		TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME10 - 200);
	}
}

/**
 * LED电平表效果 - 模式一（带音量控制和音乐加速）
 * 效果：绿色(低音量) -> 黄色(中音量) -> 红色(高音量) 渐变
 * 音量越大，点亮的灯珠数量越多
 * 音乐播放时，动画速度会随音量变化而加快
 */

// 峰值保持和音乐加速相关静态变量
static uint8_t peak_hold_value_led1 = 0;      // 当前峰值保持的电平值
static uint8_t peak_decay_timer_led1 = 0;     // 峰值衰减定时器
static uint8_t if_accelerate_led1 = FALSE;    // 音乐加速标志
static uint8_t accelerate_counter_led1 = 0;   // 加速计数器
static uint8_t volume_level_led1 = 0;         // 当前音量电平值

static uint8_t peak_hold_value_led1_3 = 0;   // LED_DATA_3的峰值
static uint8_t peak_hold_value_led1_2 = 0;   // LED_DATA_2的峰值
static uint8_t peak_decay_timer_led1_3 = 0;  // LED_DATA_3的衰减定时器
static uint8_t peak_decay_timer_led1_2 = 0;  // LED_DATA_2的衰减定时器

void LedEffect1()
{
    if (led_exchange_flag)
    {
        ClearLedDataAll();

        led_exchange_flag = FALSE;

        color_contral_param   = 0;
        color_contral_tmp     = 0;
        light_persent         = 0;
        color_start_index     = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect1--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // LED3 = 音乐音量
        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led3 = volume;
        }
        else
        {
            volume_level_led3 = 0;
        }

        // LED2 = 麦克风音量
        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led2 = volume;
        }
        else
        {
            volume_level_led2 = 0;
        }

        // 保存音量供加速使用
        volume_level_led1 = volume_level_led3;

        // ====================== LED_DATA_3 固定颜色亮灯 ======================
        for(uint8_t i = 0; i < volume_level_led3; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            b_val = 255 * userVar.brightness / 100;

            LED_DATA_3[i * 3 + 0] = r_val;
            LED_DATA_3[i * 3 + 1] = g_val;
            LED_DATA_3[i * 3 + 2] = b_val;
        }

        // ====================== LED_DATA_2 固定颜色亮灯 ======================
        for(uint8_t i = 0; i < volume_level_led2; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            b_val = 255 * userVar.brightness / 100;

            LED_DATA_2[i * 3 + 0] = r_val;
            LED_DATA_2[i * 3 + 1] = g_val;
            LED_DATA_2[i * 3 + 2] = b_val;
        }

        // ========== 音乐加速 ==========
        if(userVar.if_music_play)
        {
            if(!if_accelerate_led1 && IsTimeOut(&led_switch_timer1))
            {
                uint16_t volume_raw = GetAudioSdct(MUSIC_VOL_TYPE);
                uint8_t volume_strength = volume_raw / 1300;
                if(volume_strength > 4) volume_strength = 4;

                if(volume_strength == 0)
                    accelerate_counter_led1 = 7;
                else if(volume_strength == 1)
                    accelerate_counter_led1 = 9;
                else if(volume_strength == 2)
                    accelerate_counter_led1 = 12;
                else if(volume_strength == 3)
                    accelerate_counter_led1 = 17;
                else if(volume_strength == 4)
                    accelerate_counter_led1 = 35;

                if_accelerate_led1 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength * 20) * accelerate_counter_led1 + 500));
            }

            if(if_accelerate_led1)
            {
                if(accelerate_counter_led1 > 0)
                    accelerate_counter_led1--;
                else
                    if_accelerate_led1 = FALSE;

                TimeOutSet(&led_switch_timer, 70 - volume_level_led1 * 2);
            }
            else
            {
                TimeOutSet(&led_switch_timer, 80);
            }
        }
        else
        {
            if_accelerate_led1 = FALSE;
            accelerate_counter_led1 = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME8);
        }

        if_refresh_led_data = 0xFF;
    }
}
// 峰值保持相关静态变量
static uint8_t peak_hold_value_led2 = 0;   // LED_DATA_2当前峰值保持的电平值
static uint8_t peak_hold_value_led3 = 0;   // LED_DATA_3当前峰值保持的电平值
static uint8_t peak_decay_timer = 0;       // 峰值衰减定时器
static uint8_t if_accelerate_led2 = FALSE;    // 音乐加速标志
static uint8_t accelerate_counter_led2 = 0;   // 加速计数器
static uint8_t volume_strength_value = 0;     // 音量强度值



void LedEffect2()
{
    if (led_exchange_flag)
    {
        ClearLedDataAll();

        led_exchange_flag = FALSE;

        color_contral_param   = 0;
        color_contral_tmp     = 0;
        light_persent         = 0;
        color_start_index     = 0;


        peak_hold_value_led1_3 = 0;
        peak_decay_timer_led1_3 = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect1--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // LED3 = 音乐音量
        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led3 = volume;
        }
        else
        {
            volume_level_led3 = 0;
        }

        // LED2 = 麦克风音量
        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led2 = volume;
        }
        else
        {
            volume_level_led2 = 0;
        }

        // 保存音量供加速使用
        volume_level_led1 = volume_level_led3;

        // ====================== LED_DATA_3 固定颜色亮灯 ======================
        for(uint8_t i = 0; i < volume_level_led3; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(i <= 34)        // 1~35 → 绿色
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(i <= 44)   // 36~45 → 黄色
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else               // 46~48 → 红色
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_3[i * 3 + 0] = r_val;
            LED_DATA_3[i * 3 + 1] = g_val;
            LED_DATA_3[i * 3 + 2] = b_val;
        }

        // ====================== LED_DATA_2 固定颜色亮灯 ======================
        for(uint8_t i = 0; i < volume_level_led2; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(i <= 34)
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(i <= 44)
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_2[i * 3 + 0] = r_val;
            LED_DATA_2[i * 3 + 1] = g_val;
            LED_DATA_2[i * 3 + 2] = b_val;
        }

        //LED3峰值
        if(volume_level_led3 > peak_hold_value_led1_3)
        {
            peak_hold_value_led1_3 = volume_level_led3;
            peak_decay_timer_led1_3 = 2;
        }

        if(peak_decay_timer_led1_3 > 0)
        {
            peak_decay_timer_led1_3--;
            if(peak_decay_timer_led1_3 == 0 && peak_hold_value_led1_3 > 0)
            {
                peak_hold_value_led1_3--;
                peak_decay_timer_led1_3 = 2;
            }
        }

        // ====================== LED2 峰值 ======================
               if(volume_level_led2 > peak_hold_value_led1_2)
               {
                   peak_hold_value_led1_2 = volume_level_led2;
                   peak_decay_timer_led1_2 = 2;
               }

               if(peak_decay_timer_led1_2 > 0)
               {
                   peak_decay_timer_led1_2--;
                   if(peak_decay_timer_led1_2 == 0 && peak_hold_value_led1_2 > 0)
                   {
                       peak_hold_value_led1_2--;
                       peak_decay_timer_led1_2 = 2;
                   }
               }

        // ====================== 峰值颜色随位置变化 ======================
        if(peak_hold_value_led1_3 > 0 && peak_hold_value_led1_3 <= 48)
        {
            uint8_t peak_idx = peak_hold_value_led1_3 - 1;
            uint8_t r_val = 0, g_val = 0, b_val = 0;


            if(peak_idx <= 34)       // 绿色段
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(peak_idx <= 44)  // 黄色段
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else                     // 红色段
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_3[peak_idx * 3 + 0] = r_val;
            LED_DATA_3[peak_idx * 3 + 1] = g_val;
            LED_DATA_3[peak_idx * 3 + 2] = b_val;
        }

        if(peak_hold_value_led1_2 > 0 && peak_hold_value_led1_2 <= 48)
                {
                    uint8_t peak_idx = peak_hold_value_led1_2 - 1;
                    uint8_t r_val = 0, g_val = 0, b_val = 0;


                    if(peak_idx <= 34)       // 绿色段
                    {
                        g_val = 255 * userVar.brightness / 100;
                    }
                    else if(peak_idx <= 44)  // 黄色段
                    {
                        r_val = 255 * userVar.brightness / 100;
                        g_val = 255 * userVar.brightness / 100;
                    }
                    else                     // 红色段
                    {
                        r_val = 255 * userVar.brightness / 100;
                    }

                    LED_DATA_2[peak_idx * 3 + 0] = r_val;
                    LED_DATA_2[peak_idx * 3 + 1] = g_val;
                    LED_DATA_2[peak_idx * 3 + 2] = b_val;
                }


        // ========== 音乐加速 ==========
        if(userVar.if_music_play)
        {
            if(!if_accelerate_led1 && IsTimeOut(&led_switch_timer1))
            {
                uint16_t volume_raw = GetAudioSdct(MUSIC_VOL_TYPE);
                uint8_t volume_strength = volume_raw / 1300;
                if(volume_strength > 4) volume_strength = 4;

                if(volume_strength == 0)
                    accelerate_counter_led1 = 7;
                else if(volume_strength == 1)
                    accelerate_counter_led1 = 9;
                else if(volume_strength == 2)
                    accelerate_counter_led1 = 12;
                else if(volume_strength == 3)
                    accelerate_counter_led1 = 17;
                else if(volume_strength == 4)
                    accelerate_counter_led1 = 35;

                if_accelerate_led1 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength * 20) * accelerate_counter_led1 + 500));
            }

            if(if_accelerate_led1)
            {
                if(accelerate_counter_led1 > 0)
                    accelerate_counter_led1--;
                else
                    if_accelerate_led1 = FALSE;

                TimeOutSet(&led_switch_timer, 70 - volume_level_led1 * 2);
            }
            else
            {
                TimeOutSet(&led_switch_timer, 80);
            }
        }
        else
        {
            if_accelerate_led1 = FALSE;
            accelerate_counter_led1 = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME8);

            // ====================== 没音乐时峰值慢慢归零 ======================
            if(peak_hold_value_led1_3 > 0)
            {
                peak_decay_timer_led1_3--;
                if(peak_decay_timer_led1_3 == 0)
                {
                    peak_hold_value_led1_3--;
                    peak_decay_timer_led1_3 = 2;
                }
            }
        }

        if_refresh_led_data = 0xFF;
    }
}
void LedEffect3()
{
    if (led_exchange_flag)
    {
        led_exchange_flag = FALSE;
        ClearLedDataAll();

        color_start_index    = 0;
        color_contral_tmp    = 0;
        color_contral_tmp1   = 0;
        color_contral_param  = 0;
        color_contral_param1 = 0;
        color_contral_param2 = 0;

        if_accelerate        = FALSE;
        color_contral_param6 = 0;
        color_contral_param7 = 0;

        // 音乐加速变量初始化
        if_accelerate_led2 = FALSE;
        accelerate_counter_led2 = 0;
        volume_strength_value = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect8--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // ========== 1. LED3 使用：音乐音量 ======================
        uint8_t volume_level_music = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_music = volume;
        }
        else
        {
            volume_level_music = 0;
        }

        // ========== 2. LED2 使用：麦克风音量（和音乐不一样）======================
        uint8_t volume_level_mic = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE); // 这里改成 MIC
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_mic = volume;
        }
        else
        {
            volume_level_mic = 0;
        }

        // ========== 点亮 LED_DATA_3（音乐音量）==========
        for(uint8_t i = 0; i < volume_level_music; i++)
        {
            uint8_t red_val = 255 * userVar.brightness / 100;
            LED_DATA_3[i * 3 + 0] = 0;
            LED_DATA_3[i * 3 + 1] = red_val;
            LED_DATA_3[i * 3 + 2] = 0;
        }

        // ========== 点亮 LED_DATA_2（麦克风音量，不同步）==========
        for(uint8_t i = 0; i < volume_level_mic; i++)
        {
            uint8_t red_val = 255 * userVar.brightness / 100;
            LED_DATA_2[i * 3 + 0] = 0;
            LED_DATA_2[i * 3 + 1] = red_val;
            LED_DATA_2[i * 3 + 2] = 0;
        }

        // ========== 音乐加速逻辑
        if(userVar.if_music_play)
        {
            // 加速触发逻辑
            if(!if_accelerate_led2 && IsTimeOut(&led_switch_timer1))
            {
                // 根据音量强度计算加速参数
                volume_strength_value = GetLedSwitchTime1(2) / 2;

                if(volume_strength_value == 0)
                    accelerate_counter_led2 = 7;
                else if(volume_strength_value == 1)
                    accelerate_counter_led2 = 9;
                else if(volume_strength_value == 2)
                    accelerate_counter_led2 = 12;
                else if(volume_strength_value == 3)
                    accelerate_counter_led2 = 17;
                else if(volume_strength_value == 4)
                    accelerate_counter_led2 = 35;
                else
                    accelerate_counter_led2 = 35;

                if_accelerate_led2 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength_value * 20) * accelerate_counter_led2 + 500));
            }

            // 加速执行逻辑
            if(if_accelerate_led2)
            {
                if(accelerate_counter_led2 > 0)
                {
                    accelerate_counter_led2--;
                }
                else
                {
                    if_accelerate_led2 = FALSE;
                }

                // 音量越大，刷新间隔越短（动画更流畅）
                uint8_t refresh_speed = 70 - volume_strength_value * 10;
                if(refresh_speed < LED_CONTRAL_TIME4)
                {
                    refresh_speed = LED_CONTRAL_TIME4;
                }
                TimeOutSet(&led_switch_timer, refresh_speed);
            }
            else
            {
                // 正常刷新模式
                TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
            }
        }
        else
        {
            // 无音乐时，重置加速标志
            if_accelerate_led2 = FALSE;
            accelerate_counter_led2 = 0;
            volume_strength_value = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
        }

        if_refresh_led_data = 0xFF;
    }
}

void LedEffect4()
{
    if (led_exchange_flag)
    {
        led_exchange_flag = FALSE;
        ClearLedDataAll();

        color_start_index    = 0;
        color_contral_tmp    = 0;
        color_contral_tmp1   = 0;
        color_contral_param  = 0;
        color_contral_param1 = 0;
        color_contral_param2 = 0;

        if_accelerate        = FALSE;
        color_contral_param6 = 0;
        color_contral_param7 = 0;

        // 峰值保持变量初始化
        peak_hold_value_led2 = 0;
        peak_hold_value_led3 = 0;
        peak_decay_timer = 0;

        // 音乐加速变量初始化
        if_accelerate_led2 = FALSE;
        accelerate_counter_led2 = 0;
        volume_strength_value = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect7--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // LED3 使用：音乐音量 MUSIC
        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led3 = volume;
        }
        else
        {
            volume_level_led3 = 0;
        }

        // LED2 使用：麦克风音量 MIC（和音乐不一样）
        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led2 = volume;
        }
        else
        {
            volume_level_led2 = 0;
        }

        // ====================== 颜色渐变流动变量 ======================
        static uint8_t color_flow = 0;
        color_flow += 2;  // 渐变速度

        // ========== 2. 点亮 LED_DATA_3（使用独立音量）==========
        for(uint8_t i = 0; i < volume_level_led3; i++)
        {
            LED_DATA_3[i * 3 + 0] =255 * userVar.brightness / 100;
            LED_DATA_3[i * 3 + 1] = 0;
            LED_DATA_3[i * 3 + 2] = 0;
        }

        // ========== 3. 点亮 LED_DATA_2（使用独立音量）==========
        for(uint8_t i = 0; i < volume_level_led2; i++)
        {
            LED_DATA_2[i * 3 + 0] = 255 * userVar.brightness / 100;
            LED_DATA_2[i * 3 + 1] = 0;
            LED_DATA_2[i * 3 + 2] = 0;
        }

        // ====================== LED3 峰值逻辑（使用独立音量）======================
        if(volume_level_led3 > peak_hold_value_led1_3)
        {
            peak_hold_value_led1_3 = volume_level_led3;
            peak_decay_timer_led1_3 = 2;
        }

        if(peak_decay_timer_led1_3 > 0)
        {
            peak_decay_timer_led1_3--;
            if(peak_decay_timer_led1_3 == 0 && peak_hold_value_led1_3 > 0)
            {
                peak_hold_value_led1_3--;
                peak_decay_timer_led1_3 = 2;
            }
        }

        // ====================== LED2 峰值逻辑（使用独立音量）======================
        if(volume_level_led2 > peak_hold_value_led1_2)
        {
            peak_hold_value_led1_2 = volume_level_led2;
            peak_decay_timer_led1_2 = 3;
        }

        if(peak_decay_timer_led1_2 > 0)
        {
            peak_decay_timer_led1_2--;
            if(peak_decay_timer_led1_2 == 0 && peak_hold_value_led1_2 > 0)
            {
                peak_hold_value_led1_2--;
                peak_decay_timer_led1_2 = 3;
            }
        }

        // ====================== 峰值 → 渐变色 ======================
        uint8_t r=0,g=0,b=0;
        uint8_t color = color_flow;
        if(color < 85)
        {
            r = 255 - color*3;
            g = color*3;
            b = 0;
        }
        else if(color < 170)
        {
            color -= 85;
            r = 0;
            g = 255 - color*3;
            b = color*3;
        }
        else
        {
            color -= 170;
            r = color*3;
            g = 0;
            b = 255 - color*3;
        }
        r = r * userVar.brightness / 100;
        g = g * userVar.brightness / 100;
        b = b * userVar.brightness / 100;

        // ========== 峰值点显示（渐变色） ==========
        if(peak_hold_value_led1_3 > 0 && peak_hold_value_led1_3 <= 48)
        {
            uint8_t peak_idx = peak_hold_value_led1_3 - 1;
            LED_DATA_3[peak_idx * 3 + 0] = g;
            LED_DATA_3[peak_idx * 3 + 1] = r;
            LED_DATA_3[peak_idx * 3 + 2] = b;
        }

        if(peak_hold_value_led1_2 > 0 && peak_hold_value_led1_2 <= 48)
        {
            uint8_t peak_idx = peak_hold_value_led1_2 - 1;
            LED_DATA_2[peak_idx * 3 + 0] = g;
            LED_DATA_2[peak_idx * 3 + 1] = r;
            LED_DATA_2[peak_idx * 3 + 2] = b;
        }

        // ========== 音乐加速逻辑 ==========
        if(userVar.if_music_play)
        {
            // 加速触发逻辑
            if(!if_accelerate_led2 && IsTimeOut(&led_switch_timer1))
            {
                // 根据音量强度计算加速参数
                volume_strength_value = GetLedSwitchTime1(2) / 2;

                if(volume_strength_value == 0)
                    accelerate_counter_led2 = 7;
                else if(volume_strength_value == 1)
                    accelerate_counter_led2 = 9;
                else if(volume_strength_value == 2)
                    accelerate_counter_led2 = 12;
                else if(volume_strength_value == 3)
                    accelerate_counter_led2 = 17;
                else if(volume_strength_value == 4)
                    accelerate_counter_led2 = 35;
                else
                    accelerate_counter_led2 = 35;

                if_accelerate_led2 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength_value * 20) * accelerate_counter_led2 + 500));
            }

            // 加速执行逻辑
            if(if_accelerate_led2)
            {
                if(accelerate_counter_led2 > 0)
                    accelerate_counter_led2--;
                else
                    if_accelerate_led2 = FALSE;
            }
            else
            {
                TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
            }
        }
        else
        {
            if_accelerate_led2 = FALSE;
            accelerate_counter_led2 = 0;
            volume_strength_value = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
        }

        if_refresh_led_data = 0xFF;
    }
}

void LedEffect5()
{
    if (led_exchange_flag)
    {
        led_exchange_flag = FALSE;
        ClearLedDataAll();

        color_start_index    = 0;
        color_contral_tmp    = 0;
        color_contral_tmp1   = 0;
        color_contral_param  = 0;
        color_contral_param1 = 0;
        color_contral_param2 = 0;

        if_accelerate        = FALSE;
        color_contral_param6 = 0;
        color_contral_param7 = 0;

        // 音乐加速变量初始化
        if_accelerate_led2 = FALSE;
        accelerate_counter_led2 = 0;
        volume_strength_value = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect5--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // LED3 使用：音乐音量 MUSIC
        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led3 = volume;
        }
        else
        {
            volume_level_led3 = 0;
        }

        // LED2 使用：麦克风音量 MIC
        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led2 = volume;
        }
        else
        {
            volume_level_led2 = 0;
        }

        // 固定点亮 2 格灯珠
        int16_t light_len = 2;
        int16_t end_pos = volume_level_led3;
        int16_t start_pos = end_pos - light_len;
        if(start_pos < 0) start_pos = 0;

        // LED2 使用自己的音量（独立不同步）
        int16_t end_pos_led2 = volume_level_led2 + 1;
        int16_t start_pos_led2 = (volume_level_led2 - light_len) + 1;
        if(end_pos_led2 > 48) end_pos_led2 = 48;
        if(start_pos_led2 < 0) start_pos_led2 = 0;

        // ========== 点亮 LED_DATA_3（原始位置） ==========
        for(uint8_t i = start_pos; i < end_pos; i++)
        {
            uint8_t green_val = 255 * userVar.brightness / 100;

            // 纯绿色：R=0, G=255, B=0
            LED_DATA_3[i * 3 + 0] = 0;
            LED_DATA_3[i * 3 + 1] = green_val;
            LED_DATA_3[i * 3 + 2] = 0;
        }

        // ========== 3. 点亮 LED_DATA_2（偏移+1，更高一位） ==========
        for(uint8_t i = start_pos_led2; i < end_pos_led2; i++)
        {
            uint8_t green_val = 255 * userVar.brightness / 100;

            // 纯绿色：R=0, G=255, B=0
            LED_DATA_2[i * 3 + 0] = 0;
            LED_DATA_2[i * 3 + 1] = green_val;
            LED_DATA_2[i * 3 + 2] = 0;
        }

        // ========== 音乐加速逻辑 ==========
        if(userVar.if_music_play)
        {
            if(!if_accelerate_led2 && IsTimeOut(&led_switch_timer1))
            {
                volume_strength_value = GetLedSwitchTime1(2) / 2;

                if(volume_strength_value == 0)      accelerate_counter_led2 = 7;
                else if(volume_strength_value == 1) accelerate_counter_led2 = 9;
                else if(volume_strength_value == 2) accelerate_counter_led2 = 12;
                else if(volume_strength_value == 3) accelerate_counter_led2 = 17;
                else                                accelerate_counter_led2 = 35;

                if_accelerate_led2 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength_value * 20) * accelerate_counter_led2 + 500));
            }

            if(if_accelerate_led2)
            {
                if(accelerate_counter_led2 > 0)
                    accelerate_counter_led2--;
                else
                    if_accelerate_led2 = FALSE;

                uint8_t refresh_speed = 70 - volume_strength_value * 10;
                if(refresh_speed < LED_CONTRAL_TIME4)
                    refresh_speed = LED_CONTRAL_TIME4;

                TimeOutSet(&led_switch_timer, refresh_speed);
            }
            else
            {
                TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
            }
        }
        else
        {
            if_accelerate_led2 = FALSE;
            accelerate_counter_led2 = 0;
            volume_strength_value = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
        }

        if_refresh_led_data = 0xFF;
    }
}

void LedEffect6()
{
    if (led_exchange_flag)
    {
        led_exchange_flag = FALSE;
        ClearLedDataAll();

        color_start_index    = 0;
        color_contral_tmp    = 0;
        color_contral_tmp1   = 0;
        color_contral_param  = 0;
        color_contral_param1 = 0;
        color_contral_param2 = 0;

        if_accelerate        = FALSE;
        color_contral_param6 = 0;
        color_contral_param7 = 0;

        // 峰值保持变量初始化
        peak_hold_value_led2 = 0;
        peak_hold_value_led3 = 0;
        peak_decay_timer = 0;

        // 音乐加速变量初始化
        if_accelerate_led2 = FALSE;
        accelerate_counter_led2 = 0;
        volume_strength_value = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect4--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        // LED3 使用：音乐音量
        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led3 = volume;
        }
        else
        {
            volume_level_led3 = 0;
        }

        // LED2 使用：麦克风音量（和音乐不一样）
        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            if(volume < 0) volume = 0;
            volume_level_led2 = volume;
        }
        else
        {
            volume_level_led2 = 0;
        }

        // 固定点亮 2 格灯珠 —— LED3 使用自己音量
        int16_t light_len = 2;
        int16_t end_pos = volume_level_led3;
        int16_t start_pos = end_pos - light_len;
        if(start_pos < 0) start_pos = 0;

        // LED2 使用自己音量 + 偏移
        int16_t end_pos_led2 = volume_level_led2 + 1;
        int16_t start_pos_led2 = (volume_level_led2 - light_len) + 1;
        if(end_pos_led2 > 48) end_pos_led2 = 48;
        if(start_pos_led2 < 0) start_pos_led2 = 0;

        // ========== 点亮 LED_DATA_3 ==========
        for(uint8_t i = start_pos; i < end_pos; i++)
        {
            uint8_t green_val = 255 * userVar.brightness / 100;
            LED_DATA_3[i * 3 + 0] = 0;
            LED_DATA_3[i * 3 + 1] = green_val;
            LED_DATA_3[i * 3 + 2] = 0;
        }

        // ========== 点亮 LED_DATA_2 ==========
        for(uint8_t i = start_pos_led2; i < end_pos_led2; i++)
        {
            uint8_t green_val = 255 * userVar.brightness / 100;
            LED_DATA_2[i * 3 + 0] = 0;
            LED_DATA_2[i * 3 + 1] = green_val;
            LED_DATA_2[i * 3 + 2] = 0;
        }

        // ====================== LED3 峰值
        if(volume_level_led3 > peak_hold_value_led1_3)
        {
            peak_hold_value_led1_3 = volume_level_led3;
            peak_decay_timer_led1_3 = 2;
        }

        if(peak_decay_timer_led1_3 > 0)
        {
            peak_decay_timer_led1_3--;
            if(peak_decay_timer_led1_3 == 0 && peak_hold_value_led1_3 > 0)
            {
                peak_hold_value_led1_3--;
                peak_decay_timer_led1_3 = 2;
            }
        }

        // ======================LED2 峰值 ======================
        if(volume_level_led2 > peak_hold_value_led1_2)
        {
            peak_hold_value_led1_2 = volume_level_led2;
            peak_decay_timer_led1_2 = 3;
        }

        if(peak_decay_timer_led1_2 > 0)
        {
            peak_decay_timer_led1_2--;
            if(peak_decay_timer_led1_2 == 0 && peak_hold_value_led1_2 > 0)
            {
                peak_hold_value_led1_2--;
                peak_decay_timer_led1_2 = 3;
            }
        }

        // ========== 峰值小黄点显示 ==========
        if(peak_hold_value_led1_3 > 0 && peak_hold_value_led1_3 <= 48)
        {
            uint8_t peak_idx = peak_hold_value_led1_3 - 1;
            uint8_t yellow_val = 255 * userVar.brightness / 100;

            LED_DATA_3[peak_idx * 3 + 0] = yellow_val;
            LED_DATA_3[peak_idx * 3 + 1] = yellow_val;
            LED_DATA_3[peak_idx * 3 + 2] = 0;
        }

        if(peak_hold_value_led1_2 > 0 && peak_hold_value_led1_2 <= 48)
        {
            uint8_t peak_idx = peak_hold_value_led1_2 - 1;
            uint8_t yellow_val = 255 * userVar.brightness / 100;

            LED_DATA_2[peak_idx * 3 + 0] = yellow_val;
            LED_DATA_2[peak_idx * 3 + 1] = yellow_val;
            LED_DATA_2[peak_idx * 3 + 2] = 0;
        }

        // ========== 音乐加速逻辑 ==========
        if(userVar.if_music_play)
        {
            if(!if_accelerate_led2 && IsTimeOut(&led_switch_timer1))
            {
                volume_strength_value = GetLedSwitchTime1(2) / 2;

                if(volume_strength_value == 0)      accelerate_counter_led2 = 7;
                else if(volume_strength_value == 1) accelerate_counter_led2 = 9;
                else if(volume_strength_value == 2) accelerate_counter_led2 = 12;
                else if(volume_strength_value == 3) accelerate_counter_led2 = 17;
                else                                accelerate_counter_led2 = 35;

                if_accelerate_led2 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength_value * 20) * accelerate_counter_led2 + 500));
            }

            if(if_accelerate_led2)
            {
                if(accelerate_counter_led2 > 0)
                    accelerate_counter_led2--;
                else
                    if_accelerate_led2 = FALSE;

                uint8_t refresh_speed = 70 - volume_strength_value * 10;
                if(refresh_speed < LED_CONTRAL_TIME4)
                    refresh_speed = LED_CONTRAL_TIME4;

                TimeOutSet(&led_switch_timer, refresh_speed);
            }
            else
            {
                TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
            }
        }
        else
        {
            if_accelerate_led2 = FALSE;
            accelerate_counter_led2 = 0;
            volume_strength_value = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME4);
        }

        if_refresh_led_data = 0xFF;
    }
}
static const color_index_2[] =
{
	0,		255, 	0,
	165,	255,	0,
	255, 	255, 	0,
	255, 	0, 		0,
	255, 	0, 		255,
	0, 		0, 		255,
	128, 	0, 		255,
	255,	255,	255,
};

void LedEffect7()
{
	#define LedEffect_Color	40
	if (led_exchange_flag)
	{
		led_exchange_flag = FALSE;
		ClearLedDataAll();

		color_start_index	= 0;
		color_contral_tmp 	= 0;
		color_contral_tmp1 	= 0;
		color_contral_param = 0;
		color_contral_param1 = 0;
		color_contral_param2 = 0;
		if_accelerate 		= FALSE;
		color_contral_param6 = 0;
		color_contral_param7 = 0;

		TimeOutSet(&led_switch_timer, 0);
		TimeOutSet(&led_switch_timer1, 0);

		DBG("--------LedEffect3--------\n");
	}

	if (IsTimeOut(&led_switch_timer))
	{
		if_refresh_led_data = 0xFF;
		ClearLedDataAll();

		// ====================== LED3 使用 音乐音量 MUSIC ======================
		uint8_t volume_level_led3 = 0;
		if(userVar.if_music_play)
		{
			uint16_t vol = LedAudioLevelGet(MUSIC_VOL_TYPE);
			if(vol > 48) vol = 48;
			volume_level_led3 = vol;
		}

		// ====================== LED2 使用 麦克风音量 MIC（不同步）======================
		uint8_t volume_level_led2 = 0;
		if(userVar.if_music_play)
		{
			uint16_t vol = LedAudioLevelGet(MIC_VOL_TYPE);
			if(vol > 48) vol = 48;
			volume_level_led2 = vol;
		}

		// 中心 24、25 号灯
		uint8_t center_left  = 23;
		uint8_t center_right = 24;

		// LED3 扩散范围（音乐）
		int16_t expand_led3 = (volume_level_led3 * 23) / 48;
		int16_t start_led3 = center_left - expand_led3;
		int16_t end_led3   = center_right + expand_led3;
		if(start_led3 < 0)  start_led3 = 0;
		if(end_led3 >= 48)  end_led3 = 47;

		// LED2 扩散范围（麦克风，独立）
		int16_t expand_led2 = (volume_level_led2 * 23) / 48;
		int16_t start_led2 = center_left - expand_led2;
		int16_t end_led2   = center_right + expand_led2;
		if(start_led2 < 0)  start_led2 = 0;
		if(end_led2 >= 48)  end_led2 = 47;

		// 颜色渐变（自动流动）
		static uint8_t color_flow = 0;
		color_flow += 3;

		// ====================== 点亮灯带 ======================
		for (int16_t i = 0; i < 48; i++)
		{
			uint8_t r = 0, g = 0, b = 0;

			// ====================== LED3 使用自己的范围 ======================
			if(i >= start_led3 && i <= end_led3)
			{
				uint8_t color = color_flow + i*5;
				if(color < 85)
				{
					r = 255 - color*3;
					g = color*3;
					b = 0;
				}
				else if(color < 170)
				{
					color -= 85;
					r = 0;
					g = 255 - color*3;
					b = color*3;
				}
				else
				{
					color -= 170;
					r = color*3;
					g = 0;
					b = 255 - color*3;
				}
				r = r * userVar.brightness / 100;
				g = g * userVar.brightness / 100;
				b = b * userVar.brightness / 100;
			}
			// 写入 LED3
			LED_DATA_3[i*3+0] = g;
			LED_DATA_3[i*3+1] = r;
			LED_DATA_3[i*3+2] = b;

			// ====================== LED2 使用自己的范围（独立不同步）======================
			r = 0; g = 0; b = 0;
			if(i >= start_led2 && i <= end_led2)
			{
				uint8_t color = color_flow + i*5;
				if(color < 85)
				{
					r = 255 - color*3;
					g = color*3;
					b = 0;
				}
				else if(color < 170)
				{
					color -= 85;
					r = 0;
					g = 255 - color*3;
					b = color*3;
				}
				else
				{
					color -= 170;
					r = color*3;
					g = 0;
					b = 255 - color*3;
				}
				r = r * userVar.brightness / 100;
				g = g * userVar.brightness / 100;
				b = b * userVar.brightness / 100;
			}
			// 写入 LED2
			LED_DATA_2[i*3+0] = g;
			LED_DATA_2[i*3+1] = r;
			LED_DATA_2[i*3+2] = b;
		}

		TimeOutSet(&led_switch_timer, 12);
	}
}

// 峰值独立变量（LED2、LED3 分开，不同步）
static uint8_t peak_left_led2  = 23;
static uint8_t peak_right_led2 = 24;
static uint8_t peak_left_led3  = 23;
static uint8_t peak_right_led3 = 24;
static uint8_t peak_decay = 0;

static uint8_t peak_run_value_led9_3 = 0;
static uint8_t peak_run_value_led9_2 = 0;
static uint8_t peak_run_timer_led9_3 = 0;
static uint8_t peak_run_timer_led9_2 = 0;
void LedEffect8()
{
	#define LedEffect_Color	40
	if (led_exchange_flag)
	{
		led_exchange_flag = FALSE;
		ClearLedDataAll();

		color_start_index	= 0;
		color_contral_tmp 	= 0;
		color_contral_tmp1 	= 0;
		color_contral_param = 0;
		color_contral_param1 = 0;
		color_contral_param2 = 0;
		if_accelerate 		= FALSE;
		color_contral_param6 = 0;
		color_contral_param7 = 0;

		// 峰值初始化
		peak_left_led2  = 23;
		peak_right_led2 = 24;
		peak_left_led3  = 23;
		peak_right_led3 = 24;
		peak_decay = 0;

		TimeOutSet(&led_switch_timer, 0);
		TimeOutSet(&led_switch_timer1, 0);

		DBG("--------LedEffect6--------\n");
	}

	if (IsTimeOut(&led_switch_timer))
	{
		if_refresh_led_data = 0xFF;
		ClearLedDataAll();

		// ====================== LED2 使用：音乐音量 ======================
		uint8_t volume_level = 0;
		if(userVar.if_music_play)
		{
			uint16_t vol = LedAudioLevelGet(MUSIC_VOL_TYPE);
			if(vol > 48) vol = 48;
			volume_level = vol;
		}

		// ====================== LED3 使用：麦克风音量（独立不同步）======================
		uint8_t mic_volume_level = 0;
		if(userVar.if_music_play)
		{
			uint16_t mic_vol = LedAudioLevelGet(MIC_VOL_TYPE);
			if(mic_vol > 48) mic_vol = 48;
			mic_volume_level = mic_vol;
		}

		// 中心 24、25 号灯
		uint8_t center_left  = 23;
		uint8_t center_right = 24;

		// LED2 扩散范围（音乐）
		int16_t expand = (volume_level * 23) / 48;
		int16_t start = center_left - expand;
		int16_t end   = center_right + expand;
		if(start < 0)  start = 0;
		if(end >= 48)  end = 47;

		// LED3 扩散范围（麦克风）
		int16_t mic_expand = (mic_volume_level * 23) / 48;
		int16_t mic_start = center_left - mic_expand;
		int16_t mic_end   = center_right + mic_expand;
		if(mic_start < 0)  mic_start = 0;
		if(mic_end >= 48)  mic_end = 47;

		// LED2 峰值更新
		if(start < peak_left_led2)  peak_left_led2 = start;
		if(end   > peak_right_led2) peak_right_led2 = end;

		// LED3 峰值更新
		if(mic_start < peak_left_led3)  peak_left_led3 = mic_start;
		if(mic_end   > peak_right_led3) peak_right_led3 = mic_end;

		// 峰值衰减
		peak_decay++;
		if(peak_decay >= 8)
		{
			peak_decay = 0;
			if(peak_left_led2  < 23) peak_left_led2++;
			if(peak_right_led2 > 24) peak_right_led2--;
			if(peak_left_led3  < 23) peak_left_led3++;
			if(peak_right_led3 > 24) peak_right_led3--;
		}

		// 颜色渐变
		static uint8_t color_flow = 0;
		color_flow += 3;

		// ====================== LED2 和 LED3 独立亮灯 ======================
		for (int16_t i = 0; i < 48; i++)
		{
			// ---------------- LED2 用音乐范围 ----------------
			uint8_t r = 0, g = 0, b = 0;
			if(i >= start && i <= end)
			{
				uint8_t color = color_flow + i*5;
				if(color < 85)      { r = 255-color*3; g = color*3; b = 0; }
				else if(color <170){ color-=85; r=0; g=255-color*3; b=color*3; }
				else               { color-=170; r=color*3; g=0; b=255-color*3; }
				r = r*userVar.brightness/100;
				g = g*userVar.brightness/100;
				b = b*userVar.brightness/100;
			}
			LED_DATA_2[i*3+0] = g;
			LED_DATA_2[i*3+1] = r;
			LED_DATA_2[i*3+2] = b;

			// ---------------- LED3 用麦克风范围（完全不一样）----------------
			r = 0; g = 0; b = 0;
			if(i >= mic_start && i <= mic_end)
			{
				uint8_t color = color_flow + i*5;
				if(color < 85)      { r = 255-color*3; g = color*3; b = 0; }
				else if(color <170){ color-=85; r=0; g=255-color*3; b=color*3; }
				else               { color-=170; r=color*3; g=0; b=255-color*3; }
				r = r*userVar.brightness/100;
				g = g*userVar.brightness/100;
				b = b*userVar.brightness/100;
			}
			LED_DATA_3[i*3+0] = g;
			LED_DATA_3[i*3+1] = r;
			LED_DATA_3[i*3+2] = b;
		}

		// 峰值白光
		if(peak_left_led2 >=0 && peak_left_led2 <48)
		{
			LED_DATA_2[peak_left_led2*3 +0] = 0;
			LED_DATA_2[peak_left_led2*3 +1] = 255;
			LED_DATA_2[peak_left_led2*3 +2] = 0;
		}
		if(peak_right_led2 >=0 && peak_right_led2 <48)
		{
			LED_DATA_2[peak_right_led2*3 +0] = 0;
			LED_DATA_2[peak_right_led2*3 +1] = 255;
			LED_DATA_2[peak_right_led2*3 +2] = 0;
		}
		if(peak_left_led3 >=0 && peak_left_led3 <48)
		{
			LED_DATA_3[peak_left_led3*3 +0] = 0;
			LED_DATA_3[peak_left_led3*3 +1] = 255;
			LED_DATA_3[peak_left_led3*3 +2] = 0;
		}
		if(peak_right_led3 >=0 && peak_right_led3 <48)
		{
			LED_DATA_3[peak_right_led3*3 +0] = 0;
			LED_DATA_3[peak_right_led3*3 +1] = 255;
			LED_DATA_3[peak_right_led3*3 +2] = 0;
		}

		TimeOutSet(&led_switch_timer, 12);
	}
}

void LedEffect9()
{
    if (led_exchange_flag)
    {
        ClearLedDataAll();

        led_exchange_flag = FALSE;

        color_contral_param   = 0;
        color_contral_tmp     = 0;
        light_persent         = 0;
        color_start_index     = 0;

        peak_run_value_led9_3 = 0;
        peak_run_value_led9_2 = 0;
        peak_run_timer_led9_3 = 0;
        peak_run_timer_led9_2 = 0;

        TimeOutSet(&led_switch_timer, 0);
        TimeOutSet(&led_switch_timer1, 0);

        DBG("--------LedEffect9--------\n");
    }

    if (IsTimeOut(&led_switch_timer))
    {
        ClearLedDataAll();

        uint8_t volume_level_led3 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MUSIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            volume_level_led3 = volume;
        }

        uint8_t volume_level_led2 = 0;
        if(userVar.if_music_play)
        {
            uint16_t volume = LedAudioLevelGet(MIC_VOL_TYPE);
            if(volume > 48) volume = 48;
            volume_level_led2 = volume;
        }

        volume_level_led1 = volume_level_led3;

        for(uint8_t i = 0; i < volume_level_led3; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(i <= 34)
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(i <= 44)
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_3[i * 3 + 0] = r_val;
            LED_DATA_3[i * 3 + 1] = g_val;
            LED_DATA_3[i * 3 + 2] = b_val;
        }

        for(uint8_t i = 0; i < volume_level_led2; i++)
        {
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(i <= 34)
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(i <= 44)
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_2[i * 3 + 0] = r_val;
            LED_DATA_2[i * 3 + 1] = g_val;
            LED_DATA_2[i * 3 + 2] = b_val;
        }

        if(volume_level_led3 > peak_run_value_led9_3)
        {
            peak_run_value_led9_3 = volume_level_led3;
            peak_run_timer_led9_3 = 2;
        }

        if(peak_run_timer_led9_3 > 0)
        {
            peak_run_timer_led9_3--;
            if(peak_run_timer_led9_3 == 0 && peak_run_value_led9_3 > 0)
            {
                if(peak_run_value_led9_3 < 48)
                {
                    peak_run_value_led9_3++;
                    peak_run_timer_led9_3 = 2;
                }
                else
                {
                    peak_run_value_led9_3 = 0;
                }
            }
        }

        if(volume_level_led2 > peak_run_value_led9_2)
        {
            peak_run_value_led9_2 = volume_level_led2;
            peak_run_timer_led9_2 = 2;
        }

        if(peak_run_timer_led9_2 > 0)
        {
            peak_run_timer_led9_2--;
            if(peak_run_timer_led9_2 == 0 && peak_run_value_led9_2 > 0)
            {
                if(peak_run_value_led9_2 < 48)
                {
                    peak_run_value_led9_2++;
                    peak_run_timer_led9_2 = 2;
                }
                else
                {
                    peak_run_value_led9_2 = 0;
                }
            }
        }

        if(peak_run_value_led9_3 > 0 && peak_run_value_led9_3 <= 48)
        {
            uint8_t peak_idx = peak_run_value_led9_3 - 1;
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(peak_idx <= 34)
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(peak_idx <= 44)
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_3[peak_idx * 3 + 0] = r_val;
            LED_DATA_3[peak_idx * 3 + 1] = g_val;
            LED_DATA_3[peak_idx * 3 + 2] = b_val;
        }

        if(peak_run_value_led9_2 > 0 && peak_run_value_led9_2 <= 48)
        {
            uint8_t peak_idx = peak_run_value_led9_2 - 1;
            uint8_t r_val = 0, g_val = 0, b_val = 0;

            if(peak_idx <= 34)
            {
                g_val = 255 * userVar.brightness / 100;
            }
            else if(peak_idx <= 44)
            {
                r_val = 255 * userVar.brightness / 100;
                g_val = 255 * userVar.brightness / 100;
            }
            else
            {
                r_val = 255 * userVar.brightness / 100;
            }

            LED_DATA_2[peak_idx * 3 + 0] = r_val;
            LED_DATA_2[peak_idx * 3 + 1] = g_val;
            LED_DATA_2[peak_idx * 3 + 2] = b_val;
        }

        if(userVar.if_music_play)
        {
            if(!if_accelerate_led1 && IsTimeOut(&led_switch_timer1))
            {
                uint16_t volume_raw = GetAudioSdct(MUSIC_VOL_TYPE);
                uint8_t volume_strength = volume_raw / 1300;
                if(volume_strength > 4) volume_strength = 4;

                if(volume_strength == 0)
                    accelerate_counter_led1 = 7;
                else if(volume_strength == 1)
                    accelerate_counter_led1 = 9;
                else if(volume_strength == 2)
                    accelerate_counter_led1 = 12;
                else if(volume_strength == 3)
                    accelerate_counter_led1 = 17;
                else if(volume_strength == 4)
                    accelerate_counter_led1 = 35;

                if_accelerate_led1 = TRUE;
                TimeOutSet(&led_switch_timer1, ((70 - volume_strength * 20) * accelerate_counter_led1 + 500));
            }

            if(if_accelerate_led1)
            {
                if(accelerate_counter_led1 > 0)
                    accelerate_counter_led1--;
                else
                    if_accelerate_led1 = FALSE;

                TimeOutSet(&led_switch_timer, 70 - volume_level_led1 * 2);
            }
            else
            {
                TimeOutSet(&led_switch_timer, 80);
            }
        }
        else
        {
            if_accelerate_led1 = FALSE;
            accelerate_counter_led1 = 0;
            TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME8);
        }

        if_refresh_led_data = 0xFF;
    }
}
/*void LedEffect9()
{
	if (led_exchange_flag)
	{
		ClearLedDataAll();

		led_exchange_flag = FALSE;
		TimeOutSet(&led_switch_timer, 0);

		DBG("--------LedEffect9--------\n");
	}

	if (IsTimeOut(&led_switch_timer))
	{
		for (uint8_t i = 0; i < LED_NUM; i++)
		{
			LED_DATA_1[i * 3 + 0] = color_index_2[6 * 3 + 0];
			LED_DATA_1[i * 3 + 1] = color_index_2[6 * 3 + 1];
			LED_DATA_1[i * 3 + 2] = color_index_2[6 * 3 + 2];
		}

		if_refresh_led_data = 0xFF;

		TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME10);
	}
}*/

/*void LedEffect10()
{
	if (led_exchange_flag)
	{
		ClearLedDataAll();

		led_exchange_flag = FALSE;
		TimeOutSet(&led_switch_timer, 0);

		DBG("--------LedEffect10--------\n");
	}

	if (IsTimeOut(&led_switch_timer))
	{
		for (uint8_t i = 0; i < LED_NUM; i++)
		{
			LED_DATA_1[i * 3 + 0] = color_index_2[7 * 3 + 0];
			LED_DATA_1[i * 3 + 1] = color_index_2[7 * 3 + 1];
			LED_DATA_1[i * 3 + 2] = color_index_2[7 * 3 + 2];
		}

		if_refresh_led_data = 0xFF;

		TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME10);
	}
}*/

void LedEffectClearLed(void)
{
	if (led_exchange_flag)
	{
		ClearLedDataAll();
		led_exchange_flag = FALSE;
		
		TimeOutSet(&led_switch_timer, 0);
		
		DBG("--------LED_MODE_CLOSE--------\n");
	}
	if (IsTimeOut(&led_switch_timer))
	{
		ClearLedDataAll();
		
		if_refresh_led_data = 0xFF;

		TimeOutSet(&led_switch_timer, LED_CONTRAL_TIME9);
	}
}

void CheckRgbLedEffect(void)
{
    if(!userVar.LedInit || !IsTimeOut(&delay_led_on_timer) || userVar.ProgramSate == PROGRAM_POWERON_SLEEPING)
	{
		return;
	}

	#if defined(OTHER_LED_EFFECT)
	if (!IsTimeOut(&userVar.OtherLedTimer) && (userVar.OtherLedFlag) \
		&& (userVar.led_mode > LED_MODE_POWER_ON \
		&& userVar.led_mode < LED_MODE_CLEAR_LED))
	{
		switch(userVar.OtherLedFlag)
		{
		case MUSIC_VOL_TYPE:
			LedEffectMusicVolum(MusicVolume);
			break;
		
		case MIC_VOL_TYPE:
			LedEffectMusicVolum(MicVolume);
			break;

		case BT_CONNECTED_TYPE:
			LedEffectBtConnect(btManager.btLinkState);
			break;

		case LED_BRIGHTNESS_TYPE:
			LedEffectLedBrightness(userVar.brightness);
			break;
		}
			
		#ifdef PWM7_LED
		if(GetBit(if_refresh_led_data, LED_TIMER_7))
		{
			pwm7_led_ctrl(LED_DATA_1);
			CleanBit(if_refresh_led_data, LED_TIMER_7);
		}
		#endif

		#ifdef PWM6_LED
		if(GetBit(if_refresh_led_data, LED_TIMER_6))
		{
			pwm6_led_ctrl(LED_DATA_2);
			CleanBit(if_refresh_led_data, LED_TIMER_6);
		}
		#endif

		#ifdef PWM5_LED
		if(GetBit(if_refresh_led_data, LED_TIMER_5))
		{
			pwm5_led_ctrl(LED_DATA_3);
			CleanBit(if_refresh_led_data, LED_TIMER_5);
		}
		#endif

		#ifdef PWM8_LED
		if(GetBit(if_refresh_led_data, LED_TIMER_8))
		{
			pwm8_led_ctrl(LED_DATA_4);
			CleanBit(if_refresh_led_data, LED_TIMER_8);
		}
		#endif
		return;
	}
	#endif

	switch(userVar.led_mode)
	{
	//开机
	case LED_MODE_POWER_ON:
		{
			LedEffectPowerOn();
		}	
		break;
	
	case LED_MODE_CHANGE1:
		{
			LedEffect1();
		}	
		break;
	
	case LED_MODE_CHANGE2:
		{
			LedEffect2();
		}	
		break;
	
	case LED_MODE_CHANGE3:
		{
			LedEffect3();
		}	
		break;

	case LED_MODE_CHANGE4:
		{
			LedEffect4();
		}	
		break;
	
	case LED_MODE_CHANGE5:
		{
			LedEffect5();
		}	
		break;
	
	case LED_MODE_CHANGE6:
		{
			LedEffect6();
		}	
		break;
	
	case LED_MODE_CHANGE7:
		{
			LedEffect7();
		}	
		break;
	
	case LED_MODE_CHANGE8:
		{
			LedEffect8();
		}	
		break;
	
	case LED_MODE_CHANGE9:
		{
			LedEffect9();
		}
		break;
	
	//case LED_MODE_CHANGE10:
	//	{
		//	LedEffect10();
		//}
		//break;
	
	case LED_MODE_CLEAR_LED:
		{
			LedEffectClearLed();
		}
		break;
	
	case LED_MODE_POWER_OFF:
		{
			LedEffectPowerOff();
		}
		break;
	}

#ifdef PWM7_LED
	if(GetBit(if_refresh_led_data, LED_TIMER_7))
	{
		pwm7_led_ctrl(LED_DATA_1);
		CleanBit(if_refresh_led_data, LED_TIMER_7);
	}
#endif

#ifdef PWM6_LED
	if(GetBit(if_refresh_led_data, LED_TIMER_6))
	{
		pwm6_led_ctrl(LED_DATA_2);
		CleanBit(if_refresh_led_data, LED_TIMER_6);
	}
#endif

#ifdef PWM5_LED
	if(GetBit(if_refresh_led_data, LED_TIMER_5))
	{
		pwm5_led_ctrl(LED_DATA_3);
		CleanBit(if_refresh_led_data, LED_TIMER_5);
	}
#endif

#ifdef PWM8_LED
	if(GetBit(if_refresh_led_data, LED_TIMER_8))
	{
		pwm8_led_ctrl(LED_DATA_4);
		CleanBit(if_refresh_led_data, LED_TIMER_8);
	}
#endif
}


void LedEffectInit(void)
{		
	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);

	led_param_init();
	
	userVar.LedInit = 1;
	TimeOutSet(&delay_led_on_timer, 10);
	TimeOutSet(&led_switch_timer, 5);
	TimeOutSet(&led_switch_timer1, 5);
	TimeOutSet(&led_switch_timer2, 5);
	TimeOutSet(&led_switch_timer3, 5);

	userVar.OtherLedFlag = NORMAL_TYPE;
	TimeOutSet(&userVar.OtherLedTimer, 0);

	led_exchange_flag 	= TRUE;
	led_exchange_flag1 	= TRUE;
	led_exchange_flag2 	= TRUE;

	userVar.brightness = 80;

	#if 1
	userVar.led_mode = LED_MODE_POWER_ON; 	
	#else
	userVar.led_mode = userVar.led_mode_bak; 	
	#endif
	
	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);

	vTaskDelay(10);
	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);
	
	DBG("---LedEffectInit--- %d - %d\n", userVar.led_mode, userVar.led_mode_bak);
}


void LedEffectIODeInit(void)	//GPIO输出低电平
{		
	DBG("--------LedEffectIODeInit--------\n");
	
#ifdef PWM7_LED
	PWM_GpioConfig(TIMER7_PWM_A3_A5_A20_B4, PWM7_PIN_SEL, PWM_IO_MODE_NONE);
	STRING_CONNECT(GPIO_PORT, PWM7_LED, MODESET)(PWM7_LED_PIN, 0x0);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM7_LED, GPIE), 	PWM7_LED_PIN);				
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM7_LED, GPOE), 	PWM7_LED_PIN);
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM7_LED, GPPU), 	PWM7_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM7_LED, GPPD), 	PWM7_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM7_LED, GPOUT), 	PWM7_LED_PIN);
#endif	

#ifdef PWM6_LED
	PWM_GpioConfig(TIMER6_PWM_A1_A9_A10_A23_A24_A28, PWM6_PIN_SEL, PWM_IO_MODE_NONE);
	STRING_CONNECT(GPIO_PORT, PWM6_LED, MODESET)(PWM6_LED_PIN, 0x0);		
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM6_LED, GPIE), 	PWM6_LED_PIN);				
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM6_LED, GPOE), 	PWM6_LED_PIN);
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM6_LED, GPPU), 	PWM6_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM6_LED, GPPD), 	PWM6_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM6_LED, GPOUT), 	PWM6_LED_PIN);
#endif	

#ifdef PWM5_LED
	PWM_GpioConfig(TIMER5_PWM_A0_A7_A10_A22_A24 , PWM5_PIN_SEL, PWM_IO_MODE_NONE);
	STRING_CONNECT(GPIO_PORT, PWM5_LED, MODESET)(PWM5_LED_PIN, 0x0);		
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM5_LED, GPIE), 	PWM5_LED_PIN);				
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM5_LED, GPOE), 	PWM5_LED_PIN);
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM5_LED, GPPU), 	PWM5_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM5_LED, GPPD), 	PWM5_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM5_LED, GPOUT), 	PWM5_LED_PIN);
#endif

#ifdef PWM8_LED
	PWM_GpioConfig(TIMER8_PWM_A4_A6_A21_B5 , PWM8_PIN_SEL, PWM_IO_MODE_NONE);
	STRING_CONNECT(GPIO_PORT, PWM8_LED, MODESET)(PWM8_LED_PIN, 0x0);		
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM8_LED, GPIE), 	PWM8_LED_PIN);				
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM8_LED, GPOE), 	PWM8_LED_PIN);
	GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, PWM8_LED, GPPU), 	PWM8_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM8_LED, GPPD), 	PWM8_LED_PIN);
	GPIO_RegOneBitClear(STRING_CONNECT(GPIO, PWM8_LED, GPOUT), 	PWM8_LED_PIN);
#endif
}

void LedEffectOff(uint8_t tws)	//切换到关灯
{		
	if(userVar.led_mode == LED_MODE_POWER_OFF)
	{
		return;
	}
	
	//关机灯效调用，不保存，非关机灯效保存
	if(userVar.led_mode > LED_MODE_POWER_ON && userVar.led_mode < LED_MODE_CLEAR_LED)
	{
		userVar.led_mode_bak = userVar.led_mode;
	}
	else if(userVar.led_mode_bak > LED_MODE_POWER_ON && userVar.led_mode_bak < LED_MODE_CLEAR_LED)
	{
		userVar.led_mode_bak = userVar.led_mode_bak;
	}
	else
	{
		userVar.led_mode_bak = 1;//LED_MODE_CLEAR_LED
	}
	
	userVar.led_mode = LED_MODE_CLEAR_LED;
	
	led_exchange_flag = TRUE;
	led_exchange_flag1 = TRUE;
	led_exchange_flag2 = TRUE;
	led_exchange_flag3 = TRUE;
	
	DBG("--------LedEffectOff--------\n");
	
	#ifdef BT_TWS_SUPPORT
	if(tws)
	{
		tws_rgb_led_send(userVar.led_mode, FALSE);
	}
	#endif

	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);

	vTaskDelay(20);
	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);

	vTaskDelay(20);
	ClearLedDataAll();
	pwm7_led_ctrl(LED_DATA_1);
	pwm6_led_ctrl(LED_DATA_2);
	pwm5_led_ctrl(LED_DATA_3);
	pwm8_led_ctrl(LED_DATA_4);

}

void LedEffectNext(void)	//灯效切换
{
	//开关机灯效中（关机中），不能切换
	if(userVar.led_mode == LED_MODE_POWER_OFF || userVar.led_mode == LED_MODE_POWER_ON || !IsTimeOut(&userVar.OtherLedTimer))
	{
		return;
	}

	//上一个灯效是否是有效灯效
	if((userVar.led_mode > LED_MODE_POWER_ON) && (userVar.led_mode < LED_MODE_CLEAR_LED))
	{
		userVar.led_mode_bak = userVar.led_mode;
	}
	else if(userVar.led_mode_bak > LED_MODE_POWER_ON && userVar.led_mode_bak < LED_MODE_CLEAR_LED)
	{
		userVar.led_mode_bak = userVar.led_mode_bak;
	}
	else
	{	//上一个灯效是无效灯效，默认保存第一个灯效
		userVar.led_mode_bak = 1;//userVar.led_mode;
	}

	//默认切下一个灯效
	++userVar.led_mode;

	//切换后是否为有效灯效
	if((userVar.led_mode <= LED_MODE_POWER_ON) ||  (userVar.led_mode > LED_MODE_CLEAR_LED))
	{
		userVar.led_mode = LED_MODE_CHANGE1;
	}
	
	APP_DBG("LedEffectNext %d -- %d\n", userVar.led_mode, userVar.led_mode_bak);
	
	#ifdef BT_TWS_SUPPORT
	tws_rgb_led_send(userVar.led_mode, TRUE);
	#endif
	
	#ifdef CFG_FUNC_DISPLAY_EN
	DisplayTaskMsgSend(MSG_DISPLAY_SERVICE_LED_MODE);
	#endif

	//led_param_init();

	//灯效初始化标志
	led_exchange_flag 	= TRUE;
	led_exchange_flag1 	= TRUE;
	led_exchange_flag2 	= TRUE;
	led_exchange_flag3 	= TRUE;

	#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
	#endif
}

void LedEffectSwitch(LED_MODE led_mode, uint8_t display, uint8_t tws)//选择切换
{
	//开关机灯效中（关机中），不能切换
	if(userVar.led_mode == LED_MODE_POWER_OFF)
	{
		return;
	}
	else if((led_mode == LED_MODE_CLEAR_LED) && (userVar.led_mode == LED_MODE_CLEAR_LED))
	{
		led_mode = userVar.led_mode_bak;
	}

	//上一个灯效是否是有效灯效
	if((userVar.led_mode > LED_MODE_POWER_ON) && (userVar.led_mode < LED_MODE_CLEAR_LED))
	{
		userVar.led_mode_bak = userVar.led_mode;
	}
	else if(userVar.led_mode_bak > LED_MODE_POWER_ON && userVar.led_mode_bak < LED_MODE_CLEAR_LED)
	{
		userVar.led_mode_bak = userVar.led_mode_bak;
	}
	else
	{	//上一个灯效是无效灯效，默认保存第一个灯效
		userVar.led_mode_bak = 1;//userVar.led_mode;
	}
	
	//切换的灯效是否为有效灯效
	if((led_mode > LED_MODE_POWER_ON) && (led_mode <= LED_MODE_CLEAR_LED))
	{
		userVar.led_mode = led_mode;
	}
	else if(led_mode == LED_MODE_POWER_OFF)
	{	//切换到关机灯效
		userVar.led_mode = LED_MODE_POWER_OFF;
	}
	else if(led_mode == LED_MODE_POWER_ON && GetSystemMode() == ModeIdle)
	{	//切换到关机灯效
		userVar.led_mode = LED_MODE_POWER_ON;
	}
	else
	{	//切换到无效灯效，默认切换到第一个灯效
		userVar.led_mode = 1;//LED_MODE_CHANGE1
	}
	
	APP_DBG("LedEffectSwitch %d - %d - %d\n", led_mode, userVar.led_mode, userVar.led_mode_bak);
	
	#ifdef CFG_FUNC_DISPLAY_EN
	if(display)
	{
		DisplayTaskMsgSend(MSG_DISPLAY_SERVICE_LED_MODE);
	}
	#endif
	
	#ifdef BT_TWS_SUPPORT
	if(tws)
	{
		tws_rgb_led_send(userVar.led_mode, display);
	}
	#endif

	led_param_init();
	
	//灯效初始化标志
	led_exchange_flag 	= TRUE;
	led_exchange_flag1 	= TRUE;
	led_exchange_flag2 	= TRUE;
	led_exchange_flag3	= TRUE;

	#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
	#endif
}

void LedEffectSwitchOther(LED_TYPE led_mode)	//切换到某些指示灯
{
	if(led_mode <= 0) return;

	userVar.OtherLedFlag = led_mode;
	if(userVar.OtherLedFlag == MUSIC_VOL_TYPE)
		TimeOutSet(&userVar.OtherLedTimer, 2000);		//时间为灯效时间
	else if(userVar.OtherLedFlag == BT_CONNECTED_TYPE)
		TimeOutSet(&userVar.OtherLedTimer, 6000);
	else if(userVar.OtherLedFlag == LED_BRIGHTNESS_TYPE)
		TimeOutSet(&userVar.OtherLedTimer, 2000);

	led_exchange_flag_other = TRUE;
	DBG("LedEffectSwitchOther: %d\n", userVar.OtherLedFlag);
}

/**
 * @brief     做数据转换
 *
 * @param[in] *src  源数据存放区
 * @param[in] *dst  转换完成数据存放区
 * @param[in] num   灯珠个数
 *
 * @return    无
 */
static void Data_Conversion(uint8_t *src,uint8_t *dst,uint16_t num)
{
	uint16_t i=0;
	uint8_t j=0;
	uint16_t len;
	uint8_t *temp;
	temp = dst+48;
	len=3*num;
	for(i=0;i<len;i++)
	{
		for(j=0;j<8;j++)
		{
			if((*(src+i)<<j)&0x80)
			{
				*(temp + 8*i+j) =HT;
			}
			else
			{
				*(temp + 8*i+j) =LT;
			}
		}
	}
}

/**
 * @brief     主要是TIMER输出PWM，每一个PWM周期完成后会触发DMA请求，
 * 			     通过DMA更新PWM的占空比
 */
static void pwm7_led_Init(void)
{
#ifdef PWM7_LED
	//TIMER7_PWM_A3_A5_A20_B4
	#if (PWM7_PIN_SEL == 0)
	printf("[pwm_led_Init]	TIM7_A3\n");
	#elif  (PWM7_PIN_SEL == 1)
	printf("[pwm_led_Init]	TIM7_A5\n");
	#elif (PWM7_PIN_SEL == 2)
	printf("[pwm_led_Init]	TIM7_A20\n");
	#elif (PWM7_PIN_SEL == 3)
	printf("[pwm_led_Init]	TIM7_B4\n");
	#endif
	
	/**********************DMA配置**************************/
	DMA_CONFIG	  DMAParam;
	DMAParam.Dir   				= DMA_CHANNEL_DIR_MEM2PERI;
	DMAParam.Mode  				= DMA_BLOCK_MODE;//DMA_BLOCK_MODE;
	DMAParam.ThresholdLen 		= 0 ;
	DMAParam.SrcAddress 		= (uint32_t)LedBufA;
	DMAParam.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;

	DMAParam.DstAddress 		= 0x4002E024;// 占空比寄存器 0x4002E024
	DMAParam.DataWidth 			= DMA_DWIDTH_BYTE;
	DMAParam.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAParam.BufferLen			= BUF_LEN;
	
	DMA_TimerConfig(PERIPHERAL_ID_TIMER7, &DMAParam);
	DMA_BlockBufSet(PERIPHERAL_ID_TIMER7, LedBufA, BUF_LEN);	
	DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER7, DMA_DONE_INT);//清除DMA传输完成中断
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER7);
	
/**********************PWM配置**************************/
	
	PWM_StructInit	PWMParam;
	PWMParam.CounterMode			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType 			= PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable			= PWM_REQ_DMA_MODE;
	PWMParam.FreqDiv				= LED_T; //120M系统频率下 12000 = 100us
	PWMParam.Duty					= 0;
	//GPIO Config
	PWM_GpioConfig(TIMER7_PWM_A3_A5_A20_B4, PWM7_PIN_SEL, PWM_IO_MODE_OUT);
	//PWM Config
	PWM_Config(TIMER7, &PWMParam);
	//PWM Start
	PWM_Enable(TIMER7);
#endif
}

static void pwm6_led_Init(void)
{
#ifdef PWM6_LED
	//TIMER6_PWM_A1_A9_A10_A23_A24_A28
	#if (PWM6_PIN_SEL == 0)
	printf("[pwm_led_Init]	TIM6_A1\n");
	#elif  (PWM6_PIN_SEL == 1)
	printf("[pwm_led_Init]	TIM6_A9\n");
	#elif (PWM6_PIN_SEL == 2)
	printf("[pwm_led_Init]	TIM6_A10\n");
	#elif (PWM6_PIN_SEL == 3)
	printf("[pwm_led_Init]	TIM6_A23\n");
	#elif  (PWM6_PIN_SEL == 4)
	printf("[pwm_led_Init]	TIM6_A24\n");
	#elif (PWM6_PIN_SEL == 5)
	printf("[pwm_led_Init]	TIM6_A28\n");
	#endif

	/**********************DMA配置**************************/
	DMA_CONFIG	  DMAParam;

	DMAParam.Dir   				= DMA_CHANNEL_DIR_MEM2PERI;
	DMAParam.Mode  				= DMA_BLOCK_MODE;//DMA_BLOCK_MODE;
	DMAParam.ThresholdLen 		= 0;
	DMAParam.SrcAddress 		= (uint32_t)LedBufC;
	DMAParam.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;

	DMAParam.DstAddress 		= 0x4002C824;// 占空比寄存器
	DMAParam.DataWidth 			= DMA_DWIDTH_BYTE;
	DMAParam.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAParam.BufferLen			= BUF_LEN1;

	DMA_TimerConfig(PERIPHERAL_ID_TIMER6, &DMAParam);
	DMA_BlockBufSet(PERIPHERAL_ID_TIMER6, LedBufC, BUF_LEN1); 
	DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER6, DMA_DONE_INT);//清除DMA传输完成中断
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER6);

/**********************PWM配置**************************/
	PWM_StructInit	PWMParam;

	PWMParam.CounterMode			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType 			= PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable			= PWM_REQ_DMA_MODE;
	PWMParam.FreqDiv				= LED_T; //120M系统频率下 12000 = 1.25us
	PWMParam.Duty					= 0;
	//GPIO Config
	PWM_GpioConfig(TIMER6_PWM_A1_A9_A10_A23_A24_A28, PWM6_PIN_SEL, PWM_IO_MODE_OUT);
	//PWM Config
	PWM_Config(TIMER6, &PWMParam);
	//PWM Start
	PWM_Enable(TIMER6);
#endif
}

static void pwm5_led_Init(void)
{
#ifdef PWM5_LED
	//TIMER5_PWM_A0_A7_A10_A22_A24
	#if (PWM5_PIN_SEL == 0)
	printf("[pwm_led_Init]	TIM5_A0\n");
	#elif  (PWM5_PIN_SEL == 1)
	printf("[pwm_led_Init]	TIM5_A7\n");
	#elif (PWM5_PIN_SEL == 2)
	printf("[pwm_led_Init]	TIM5_A10\n");
	#elif (PWM5_PIN_SEL == 3)
	printf("[pwm_led_Init]	TIM5_A22\n");
	#elif (PWM5_PIN_SEL == 4)
	printf("[pwm_led_Init]	TIM5_A24\n");
	#endif
	
	/**********************DMA配置**************************/
	DMA_CONFIG	  DMAParam;
	DMAParam.Dir   				= DMA_CHANNEL_DIR_MEM2PERI;
	DMAParam.Mode  				= DMA_BLOCK_MODE;//DMA_BLOCK_MODE;
	DMAParam.ThresholdLen 		= 0 ;
	DMAParam.SrcAddress 		= (uint32_t)LedBufE;
	DMAParam.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;

	DMAParam.DstAddress 		= 0x4002C024;// 占空比寄存器
	DMAParam.DataWidth 			= DMA_DWIDTH_BYTE;
	DMAParam.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAParam.BufferLen			= BUF_LEN2;

	DMA_TimerConfig(PERIPHERAL_ID_TIMER5, &DMAParam);
	DMA_BlockBufSet(PERIPHERAL_ID_TIMER5, LedBufE, BUF_LEN2); 
	DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER5, DMA_DONE_INT);//清除DMA传输完成中断
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER5);

/**********************PWM配置**************************/
	PWM_StructInit	PWMParam;

	PWMParam.CounterMode			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType 			= PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable			= PWM_REQ_DMA_MODE;
	PWMParam.FreqDiv				= LED_T; //120M系统频率下 12000 = 100us
	PWMParam.Duty					= 0;
	//GPIO Config
	PWM_GpioConfig(TIMER5_PWM_A0_A7_A10_A22_A24, PWM5_PIN_SEL, PWM_IO_MODE_OUT);
	//PWM Config
	PWM_Config(TIMER5, &PWMParam);
	//PWM Start
	PWM_Enable(TIMER5);
#endif
}

static void pwm8_led_Init(void)
{
#ifdef PWM8_LED
	//TIMER8_PWM_A4_A6_A21_B5
	#if (PWM8_PIN_SEL == 0)
	printf("[pwm_led_Init]	TIM8_A4\n");
	#elif  (PWM8_PIN_SEL == 1)
	printf("[pwm_led_Init]	TIM8_A6\n");
	#elif (PWM8_PIN_SEL == 2)
	printf("[pwm_led_Init]	TIM8_A21\n");
	#elif (PWM8_PIN_SEL == 3)
	printf("[pwm_led_Init]	TIM8_B5\n");
	#endif
	
	/**********************DMA配置**************************/
	DMA_CONFIG	  DMAParam;
	DMAParam.Dir   				= DMA_CHANNEL_DIR_MEM2PERI;
	DMAParam.Mode  				= DMA_BLOCK_MODE;//DMA_BLOCK_MODE;
	DMAParam.ThresholdLen 		= 0 ;
	DMAParam.SrcAddress 		= (uint32_t)LedBufG;
	DMAParam.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;

	DMAParam.DstAddress 		= 0x4002E824;// 占空比寄存器
	DMAParam.DataWidth 			= DMA_DWIDTH_BYTE;
	DMAParam.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAParam.BufferLen			= BUF_LEN3;

	DMA_TimerConfig(PERIPHERAL_ID_TIMER8, &DMAParam);
	DMA_BlockBufSet(PERIPHERAL_ID_TIMER8, LedBufG, BUF_LEN3); 
	DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER8, DMA_DONE_INT);//清除DMA传输完成中断
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER8);

/**********************PWM配置**************************/
	PWM_StructInit	PWMParam;

	PWMParam.CounterMode			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType 			= PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable			= PWM_REQ_DMA_MODE;
	PWMParam.FreqDiv				= LED_T; //120M系统频率下 12000 = 100us
	PWMParam.Duty					= 0;
	//GPIO Config
	PWM_GpioConfig(TIMER8_PWM_A4_A6_A21_B5, PWM8_PIN_SEL, PWM_IO_MODE_OUT);
	//PWM Config
	PWM_Config(TIMER8, &PWMParam);
	//PWM Start
	PWM_Enable(TIMER8);
#endif
}

void pwm_led_Init(void)
{
	pwm7_led_Init();
	pwm6_led_Init();
	pwm5_led_Init();
	pwm8_led_Init();

	LedEffectInit();
}

static void pwm7_led_ctrl(uint8_t *data_led)
{
#ifdef PWM7_LED
	static uint8_t send_flag=0; //数据刷屏发送状态
	static uint8_t state_flag=0;//数据准备状态
	uint16_t i=0,j=0;

	if(state_flag==0)//准备下一次显示数据
	{
				
		if(send_flag)
		{
			Data_Conversion(data_led,LedBufA,LED_NUM);
		}
		else
		{
			Data_Conversion(data_led,LedBufB,LED_NUM);
		}
		state_flag=1;
	}

	if(DMA_InterruptFlagGet(PERIPHERAL_ID_TIMER7, DMA_DONE_INT))
	{
		if(state_flag)
		{
			DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER7, DMA_DONE_INT);

			if(send_flag)
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER7,LedBufA,BUF_LEN);
				send_flag = 0;
			}
			else
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER7,LedBufB,BUF_LEN);
				send_flag = 1;
			}
			DMA_ChannelEnable(PERIPHERAL_ID_TIMER7);
			state_flag=0;
		}
	}
#endif
}

static void pwm6_led_ctrl(uint8_t *data_led)
{
#ifdef PWM6_LED
	static uint8_t send_flag=0; //数据刷屏发送状态
	static uint8_t state_flag=0;//数据准备状态
	uint16_t i=0,j=0;

	if(state_flag==0)//准备下一次显示数据
	{
				
		if(send_flag)
		{
			Data_Conversion(data_led,LedBufC,LED_NUM1);
		}
		else
		{
			Data_Conversion(data_led,LedBufD,LED_NUM1);
		}
		state_flag=1;
	}

	if(DMA_InterruptFlagGet(PERIPHERAL_ID_TIMER6, DMA_DONE_INT))
	{
		if(state_flag)
		{
			DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER6, DMA_DONE_INT);

			if(send_flag)
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER6,LedBufC,BUF_LEN1);
				send_flag = 0;
			}
			else
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER6,LedBufD,BUF_LEN1);
				send_flag = 1;
			}
			DMA_ChannelEnable(PERIPHERAL_ID_TIMER6);
			state_flag=0;
		}
	}
#endif
}

static void pwm5_led_ctrl(uint8_t *data_led)
{
#ifdef PWM5_LED
	static uint8_t send_flag=0; //数据刷屏发送状态
	static uint8_t state_flag=0;//数据准备状态
	uint16_t i=0,j=0;

	if(state_flag==0)//准备下一次显示数据
	{
				
		if(send_flag)
		{
			Data_Conversion(data_led,LedBufE,LED_NUM2);
		}
		else
		{
			Data_Conversion(data_led,LedBufF,LED_NUM2);
		}
		state_flag=1;
	}

	if(DMA_InterruptFlagGet(PERIPHERAL_ID_TIMER5, DMA_DONE_INT))
	{
		if(state_flag)
		{
			DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER5, DMA_DONE_INT);

			if(send_flag)
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER5,LedBufE,BUF_LEN2);
				send_flag = 0;
			}
			else
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER5,LedBufF,BUF_LEN2);
				send_flag = 1;
			}
			DMA_ChannelEnable(PERIPHERAL_ID_TIMER5);
			state_flag=0;
		}
	}
#endif
}

static void pwm8_led_ctrl(uint8_t *data_led)
{
#ifdef PWM8_LED
	static uint8_t send_flag=0; //数据刷屏发送状态
	static uint8_t state_flag=0;//数据准备状态
	uint16_t i=0,j=0;

	if(state_flag==0)//准备下一次显示数据
	{
				
		if(send_flag)
		{
			Data_Conversion(data_led,LedBufG,LED_NUM3);
		}
		else
		{
			Data_Conversion(data_led,LedBufH,LED_NUM3);
		}
		state_flag=1;
	}

	if(DMA_InterruptFlagGet(PERIPHERAL_ID_TIMER8, DMA_DONE_INT))
	{
		if(state_flag)
		{
			DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER8, DMA_DONE_INT);

			if(send_flag)
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER8,LedBufG,BUF_LEN3);
				send_flag = 0;
			}
			else
			{
				DMA_BlockBufSet(PERIPHERAL_ID_TIMER8,LedBufH,BUF_LEN3);
				send_flag = 1;
			}
			DMA_ChannelEnable(PERIPHERAL_ID_TIMER8);
			state_flag=0;
		}
	}
#endif
}

#endif

