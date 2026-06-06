#ifndef	__ADD_STATUS_H__
#define	__ADD_STATUS_H__

#include "app_config.h"
#include "timeout.h"
#include "gpio.h"
#include "adc.h"
#include "sys_gpio.h"
#include <stdlib.h>
#include "user_enum.h"


//软件版本，发给客户的就+1，会在 main.c 的 main 函数里打印
#define ProjectName				"炫音- DB96"
#define Software_version		(2)

#define ADC_KEY(x, y) 		(((x) * 4095 / 330 + (y) * 4095 / 330)/2)	//电压方式
#define AVERAGE(x, y) 		(((x) + (y))/2)

#define GPIO_PORT 	GPIO_Port 
#define MODESET 	ModeSet 

// 将第 pos 位置 1
#define SetBit(num, pos) ((num) |= BIT((pos)))

// 多位置 1
#define SetBits(num, mask) ((num) |= (mask))

// 将第 pos 位清 0
#define CleanBit(num, pos) ((num) &= ~BIT((pos)))

// 多位清 0
#define CleanBits(num, mask) ((num) &= ~(mask))

// 获取pos位置的数据
#define GetBit(num, pos) (((num) & BIT((pos))) != 0)

// 翻转num中指定位置的位（0变1，1变0）
#define ToggleBit(num, pos) ((num) ^= BIT((pos)))

// 翻转num中，掩码mask里所有为1的bit位（mask由多个BIT(n)组合而成）
// 翻转第0位和第2位（掩码：BIT(0)|BIT(2) = 0b101）
// 实例 ： ToggleBits(val, BIT(0) | BIT(2));
#define ToggleBits(num, mask) ((num) ^= (mask))


#define UserSoftPower
#ifdef UserSoftPower

//****************************** 宏开关 *************************************************//
//自定义开关机
//#define DelayPowerON  (100)		//延时开关--50*20
#ifdef DelayPowerON
	#define CFG_POWERKEY_EN		//是否需要长按开机
#endif
#define CFG_LOCK_EN		//自锁开关

#ifdef CFG_FUNC_RECORDER_EN
	#define CFG_REC_NO_ENOUGH_EN	//录音没内存，关音效
#endif

#define SILENCE_THRESHOLD1 (5)

//检测是否有信号
#define CFG_CHECK_SIGNAL_EN

//假关机
#define CFG_FAKE_DEEPSLEEP			//假关机时，保持芯片有电流的脚，要拉底

// #define CFG_FUNC_WAKEUP_MCU_RESET

//开机放手后按键才起作用
//#define CFG_POWERON_LET_GO

//AUX模式静音音乐
//#define CFG_AUX_MUTE_EN

//OPT模式静音音乐
//#define CFG_OPT_MUTE_EN

//COA模式静音音乐
//#define CFG_COA_MUTE_EN

//HDMI模式静音音乐
//#define CFG_HDMI_MUTE_EN

//Radio模式静音音乐
//#define CFG_RADIOIN_MUTE_EN

//自定义屏幕
//#define UserDisplay_EN

//IIC
//#define CFG_USER_IIC_EN
#ifdef CFG_USER_IIC_EN
	#define CFG_IIC_AMP_MUTE_EN	//没声音静音
#endif

#ifdef CFG_IIC_AMP_MUTE_EN
	void IIC_AMP_MUTE(void);
#endif

//功放宏
//#define CFG_AMP_MUTE_EN
#ifdef CFG_AMP_MUTE_EN
	#define CFG_AMP_MUTE_STATE_EN	//没声音静音
	//#define CFG_HARD_POWEROFF		//硬件关机防po声
	//#define CFG_AMP_SHDN_EN
#endif
#define CFG_AMP_MUTE_STATE_EN	//没声音静音

//4917耳返
//#define CFG_MUTE_4917_EN

//从机供电
// #define CFG_SLAVE_EN

//电池充满检测，默认正常
//#define CFG_INCHARGE_FULL_EN	

//灯
//#define CFG_BT_LED
#define CFG_LED1_G
#define CFG_LED1_R
#define CFG_LED2_G
#define CFG_LED2_R

//5050灯 PWM + DMA
#define CFG_PWM_LED_EN

//5050灯 DMA
//#define CFG_DMA_LED_EN

#if (defined(CFG_PWM_LED_EN) && defined(CFG_DMA_LED_EN))
#error	"CFG_PWM_LED_EN && CFG_DMA_LED_EN Not at the same time!!!!!"
#endif

#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	#define OTHER_LED_EFFECT	//其他灯效，如调音量，蓝牙连接成功

	//#define PWM7_LED	A
	#ifdef PWM7_LED
	#define	PWM7_LED_PIN			GPIO_INDEX6		//TIMER7_PWM_A3_A5_A20_B4
	#define	PWM7_PIN_SEL			6				//上面第几个(使用 CFG_PWM_LED_EN 宏才需要选择，CFG_DMA_LED_EN 则无所谓)
	#endif
	
	#define PWM6_LED	A
	#ifdef PWM6_LED
	#define	PWM6_LED_PIN			GPIO_INDEX9		//TIMER6_PWM_A1_A9_A10_A23_A24_A28
	#define	PWM6_PIN_SEL			1
	#endif
	
	#define PWM5_LED	A
	#ifdef PWM5_LED
	#define	PWM5_LED_PIN			GPIO_INDEX7		//TIMER5_PWM_A0_A7_A10_A22_A24
	#define	PWM5_PIN_SEL			1
	#endif
	
	//#define PWM8_LED	A
	#ifdef PWM8_LED
	#define	PWM8_LED_PIN			GPIO_INDEX6		//TIMER8_PWM_A4_A6_A21_B5
	#define	PWM8_PIN_SEL			1
	#endif
#endif

//自定义Key
//#define CFG_USER_KEY
#ifdef CFG_USER_KEY
	#define USER_CODE_KEY_EN	//自定义CodeKey
#endif

//串口通信
//#define CFG_UART_EN
#ifdef CFG_UART_EN
	#define CFG_UART_SEND_EN	//发送串口
	#define CFG_UART_RECV_EN	//接收串口
	//#define CFG_CHECK_SUM_EN	//是否需要校验码
#endif

//#define CFG_SIM_COMM_EN
#ifdef CFG_SIM_COMM_EN
#define CFG_SIM_COMM_SEND_EN	//发送模拟通信
#define CFG_SIM_COMM_RECV_EN	//接收模拟通信
#endif

#ifdef CFG_LOCK_EN
#define	LOCK_PROT				A
#define	LOCK_PIN				GPIO_INDEX20
#define LOCK_ON					GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LOCK_PROT, GPOUT), LOCK_PIN)
#define LOCK_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LOCK_PROT, GPOUT), LOCK_PIN)
#define LOCK_INIT()	  			do{\
								STRING_CONNECT(GPIO_PORT, LOCK_PROT, MODESET)(LOCK_PIN, 0x0);\		
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LOCK_PROT, GPIE), LOCK_PIN);\				
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LOCK_PROT, GPOE), LOCK_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LOCK_PROT, GPPU), LOCK_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LOCK_PROT, GPPD), LOCK_PIN);\
						 		}while(0)
#endif


#ifdef CFG_AMP_MUTE_EN
#ifndef CFG_IIC_AMP_MUTE_EN
#define	AMP_MUTE_PROT			A
#define	AMP_MUTE_PIN			GPIO_INDEX21
#define AMP_MUTE_ON				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPOUT), AMP_MUTE_PIN)
#define AMP_MUTE_OFF			GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPOUT), AMP_MUTE_PIN)
#define AMP_MUTE_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, AMP_MUTE_PROT, MODESET)(AMP_MUTE_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPIE), AMP_MUTE_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPOE), AMP_MUTE_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPPU), AMP_MUTE_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_MUTE_PROT, GPPD), AMP_MUTE_PIN);\
						 		}while(0)
#else
#define AMP_MUTE_ON				IIC_AMP_MUTE(TRUE)
#define AMP_MUTE_OFF			IIC_AMP_MUTE(FALSE)
#define AMP_MUTE_INIT()			IIC_AMP_MUTE(TRUE)
#endif
#endif

#ifdef CFG_AMP_SHDN_EN
#define	AMP_SHDN_PROT			B
#define	AMP_SHDN_PIN			GPIO_INDEX5
#define AMP_SHDN_ON				GPIO_RegOneBitSet(	STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPOUT), AMP_SHDN_PIN)
#define AMP_SHDN_OFF			GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPOUT), AMP_SHDN_PIN)
#define AMP_SHDN_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, AMP_SHDN_PROT, MODESET)(AMP_SHDN_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPIE), AMP_SHDN_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPOE), AMP_SHDN_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPPU), AMP_SHDN_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, AMP_SHDN_PROT, GPPD), AMP_SHDN_PIN);\
						 		}while(0)
#endif

#ifdef CFG_MUTE_4917_EN
#define	MUTE_4917_PROT			A
#define	MUTE_4917_PIN			GPIO_INDEX7
#define MUTE_4917_ON			GPIO_RegOneBitClear(STRING_CONNECT(GPIO, MUTE_4917_PROT, GPOUT), MUTE_4917_PIN)
#define MUTE_4917_OFF			GPIO_RegOneBitSet(	STRING_CONNECT(GPIO, MUTE_4917_PROT, GPOUT), MUTE_4917_PIN)
#define MUTE_4917_INIT()	  	do{\
								STRING_CONNECT(GPIO_PORT, MUTE_4917_PROT, MODESET)(MUTE_4917_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, MUTE_4917_PROT, GPIE), MUTE_4917_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, MUTE_4917_PROT, GPOE), MUTE_4917_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, MUTE_4917_PROT, GPPU), MUTE_4917_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, MUTE_4917_PROT, GPPD), MUTE_4917_PIN);\
						 		}while(0)
#endif


#ifdef CFG_SLAVE_EN
#define	SLAVE_PROT				B
#define	SLAVE_PIN				GPIO_INDEX1
#define SLAVE_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, SLAVE_PROT, GPOUT), SLAVE_PIN)
#define SLAVE_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, SLAVE_PROT, GPOUT), SLAVE_PIN)
#define SLAVE_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, SLAVE_PROT, MODESET)(SLAVE_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, SLAVE_PROT, GPIE), SLAVE_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, SLAVE_PROT, GPOE), SLAVE_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, SLAVE_PROT, GPPU), SLAVE_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, SLAVE_PROT, GPPD), SLAVE_PIN);\
						 		}while(0)
#endif


#ifdef CFG_INCHARGE_FULL_EN
#define	INCHARGE_FULL_PROT		B
#define	INCHARGE_FULL_PIN		GPIO_INDEX1
#define INCHARGE_FULL_DET		GPIO_RegOneBitGet(  STRING_CONNECT(GPIO, INCHARGE_FULL_PROT, GPIN), INCHARGE_FULL_PIN)
#define INCHARGE_FULL_INIT()	do{\
								STRING_CONNECT(GPIO_PORT, INCHARGE_FULL_PROT, MODESET)(INCHARGE_FULL_PIN, 0x0);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, INCHARGE_FULL_PROT, GPIE), INCHARGE_FULL_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, INCHARGE_FULL_PROT, GPOE), INCHARGE_FULL_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, INCHARGE_FULL_PROT, GPPU), INCHARGE_FULL_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, INCHARGE_FULL_PROT, GPPD), INCHARGE_FULL_PIN);\
							 	}while(0)
#endif


#ifdef CFG_BT_LED
#define BT_LED_PORT				A
#define BT_LED_PIN				GPIO_INDEX10
#define BT_LED_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, BT_LED_PORT, GPOUT), BT_LED_PIN)
#define BT_LED_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, BT_LED_PORT, GPOUT), BT_LED_PIN)
#define BT_LED_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, BT_LED_PORT, MODESET)(BT_LED_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, BT_LED_PORT, GPIE), BT_LED_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, BT_LED_PORT, GPOE), BT_LED_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, BT_LED_PORT, GPPU), BT_LED_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, BT_LED_PORT, GPPD), BT_LED_PIN);\
						 		}while(0)
#endif

#ifdef CFG_LED1_G
#define LED1_G_PORT				A
#define LED1_G_PIN				GPIO_INDEX15
#define LED1_G_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_G_PORT, GPOUT), LED1_G_PIN)
#define LED1_G_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_G_PORT, GPOUT), LED1_G_PIN)
#define LED1_G_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, LED1_G_PORT, MODESET)(LED1_G_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_G_PORT, GPIE), LED1_G_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_G_PORT, GPOE), LED1_G_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_G_PORT, GPPU), LED1_G_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_G_PORT, GPPD), LED1_G_PIN);\
						 		}while(0)
#endif

#ifdef CFG_LED1_R
#define LED1_R_PORT				A
#define LED1_R_PIN				GPIO_INDEX16
#define LED1_R_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_R_PORT, GPOUT), LED1_R_PIN)
#define LED1_R_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_R_PORT, GPOUT), LED1_R_PIN)
#define LED1_R_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, LED1_R_PORT, MODESET)(LED1_R_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_R_PORT, GPIE), LED1_R_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_R_PORT, GPOE), LED1_R_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED1_R_PORT, GPPU), LED1_R_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED1_R_PORT, GPPD), LED1_R_PIN);\
						 		}while(0)
#endif

#ifdef CFG_LED2_G
#define LED2_G_PORT				A
#define LED2_G_PIN				GPIO_INDEX6
#define LED2_G_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_G_PORT, GPOUT), LED2_G_PIN)
#define LED2_G_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_G_PORT, GPOUT), LED2_G_PIN)
#define LED2_G_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, LED2_G_PORT, MODESET)(LED2_G_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_G_PORT, GPIE), LED2_G_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_G_PORT, GPOE), LED2_G_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_G_PORT, GPPU), LED2_G_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_G_PORT, GPPD), LED2_G_PIN);\
						 		}while(0)
#endif

#ifdef CFG_LED2_R
#define LED2_R_PORT				A
#define LED2_R_PIN				GPIO_INDEX10
#define LED2_R_ON				GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_R_PORT, GPOUT), LED2_R_PIN)
#define LED2_R_OFF				GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_R_PORT, GPOUT), LED2_R_PIN)
#define LED2_R_INIT()	  		do{\
								STRING_CONNECT(GPIO_PORT, LED2_R_PORT, MODESET)(LED2_R_PIN, 0x0);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_R_PORT, GPIE), LED2_R_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_R_PORT, GPOE), LED2_R_PIN);\
								GPIO_RegOneBitSet(  STRING_CONNECT(GPIO, LED2_R_PORT, GPPU), LED2_R_PIN);\
								GPIO_RegOneBitClear(STRING_CONNECT(GPIO, LED2_R_PORT, GPPD), LED2_R_PIN);\
						 		}while(0)
#endif

#ifdef CFG_USER_IIC_EN
	#define SDA_PORT	B
	#define	SDA_PIN		1
	#define SCL_PORT	B
	#define	SCL_PIN		0
#endif



//***********自定义变量结构体 定义在main_task.h方便多处可见??*******************************//
typedef struct _UserVar
{

	uint8_t  	Version;
	uint8_t  	Slave_Version;			//双芯片等时使用

	uint8_t		ProgramSate;			//程序状态
	TIMER		ProgramDelay;			//程序延时大概为开机提示音前
	
	TIMER   	FlushTimer[FLUSH_TIME_MAX];				//闪烁
	uint32_t	FlushFlag;				//按bit位，每个代表几看枚举 FLUSH_TIME

	#ifdef CFG_FUNC_RECORDER_EN
	uint32_t 	RecodingTime;			//录音时间
	uint8_t     IsRecoding;
	TIMER   	IsRecodingTimer;		//闪烁
	#ifdef CFG_REC_NO_ENOUGH_EN
	TIMER 		RecTimer;
	uint8_t 	if_rec;					//是否在录音
    #endif
	#endif
	
	#ifdef CFG_APP_BT_MODE_EN
	bool		BtPlay;
	#endif

	#ifdef CFG_CHECK_SIGNAL_EN
	bool 	if_music_play;			//是否在播放音乐
	uint16_t 	music_play_count;
	#endif

	#ifdef CFG_AUX_MUTE_EN
	bool  	AuxPlayFlag;			//AUX清空音乐输出
	#endif
	
	#ifdef CFG_OPT_MUTE_EN
	bool  	OpticalPlayFlag;		//Optical清空音乐输出
	#endif
	
	#ifdef CFG_COA_MUTE_EN
	bool  	CoaxialPlayFlag;		//Optical清空音乐输出
	#endif
	
	#ifdef CFG_HDMI_MUTE_EN
	bool  	HdmiPlayFlag;		//Hdmi清空音乐输出
	#endif

	#ifdef CFG_RADIOIN_MUTE_EN
	bool  	RadioPlayFlag;		//Radio清空音乐输出
	#endif
	
	#ifdef CFG_HARD_POWEROFF
	bool		IsPowerFlag;			//是否为硬件关机
	TIMER   	IsPowerTimer;			//是否为硬件关机定时
	#endif
	
	#ifdef CFG_POWERON_LET_GO
	bool		IsLetGoFlag;			
	TIMER   	IsLetGoTimer;			//开机延时一会才使能 adckey
	#endif
	
	#ifdef CFG_AMP_MUTE_STATE_EN
	uint8_t 	AmpMuteFlag;			//功放是否开关，有耳机3为都静音
	TIMER   	AmpMuteTimer;			//检测是否有声音定时器
	TIMER   	AmpMuteDelayTimer;		//检测是否有声音定时器
	uint32_t 	SilenceAmpMuteTime;
	uint32_t 	SilenceAmpUnmuteTime;
	#endif
	
	TIMER   	Device_DET_Timer;		// led 检测定时器
	uint8_t 	BT_Flag;				// 0关，1开

	#if (defined(CFG_PWM_LED_EN) || defined(CFG_DMA_LED_EN))
	uint8_t 	LedInit;				// CFG_DMA_LED_EN 用的
	uint8_t		brightness;				//亮度
	uint8_t		led_mode;				//当前灯效
	uint8_t		led_mode_bak;			//保存的有效灯效
	uint8_t		OtherLedFlag;			//其它需插入的灯效标志
	TIMER		OtherLedTimer;			//音量等灯效显示时间
	TIMER		FlushLedTimer;			//刷新灯时间
	uint8_t		led_PowerOff;			
	#endif
}UserVar;
extern UserVar userVar;

extern void GPIO_Init(void);				//GPIO初始化
extern void VarInit(void);					//自定义结构体初始化
extern void HeartBeat(void);				//心跳函数
extern void PowerOn(void);					//自定义添加开机时用的函数
extern void PowerOff(void);					//自定义添加关机时用的函数
extern bool PowerOn_Delay(uint8_t flag);	//自定义添加开机延时的函数

//***********基础函数声明*******************************//
extern void DisplayTaskSend(uint32_t msgId);
extern void MainTaskMsgSend(uint32_t msgId);	//发送消息

extern uint16_t GetAdcArvgeVal(uint32_t adc_ch);	//计算一小段时间的平均AD值
extern uint8_t GetRemindStateIsPlay(void);		//是否有提示音在播放

extern void AmpMuteState(void);				//根据情况是否静音函数
extern void AmpMuteSet(bool flag);			//静音

extern void LED_Swtich(void);				//灯相关
extern void BT_LED_Switch(void);			//蓝牙灯

extern uint16_t UserCheckSum(uint8_t *checkbuff, uint16_t buflen, uint8_t checktype);

extern uint8_t VersionSet(void);

#ifdef CFG_FUNC_POWER_MONITOR_EN
extern uint8_t GetPowerLevel(void);
extern bool IsInCharge(void);
#endif

#ifdef CFG_CHECK_SIGNAL_EN
extern void CheckSignal(void);						//检测是否声音
extern uint32_t GetAudioSdct(LED_TYPE channel);		//获取声音强度
extern uint32_t GetLedSwitchTime(uint8_t flag);		//音乐音频信号
extern uint32_t GetLedSwitchTime1(uint8_t flag);
extern uint32_t GetLedSwitchTime2(uint8_t flag);
#endif

#endif

#endif	//__ALL_CONFIG_H__
