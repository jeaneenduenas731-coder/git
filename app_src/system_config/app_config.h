/******************************************************************************
 * @file    app_config.h
 * @author
 * @version V_NEW
 * @date    2021-06-25
 * @maintainer
 * @brief
 ******************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "type.h"
#include "chip_config.h"
#include "spi_flash.h"
#include "debug.h"
#include "bt_config.h"
#include "chip_info.h"
//************************************************************************************************************
//    本系统默认开启2个系统全局宏，在IDE工程配置(Build Settings-Compiler-Symbols)，此处用于提醒
//*CFG_APP_CONFIG 和 FUNC_OS_EN*/
//************************************************************************************************************

//************************************************************************************************************
//    功能开关说明：
// *CFG_APP_*  : 软件系统应用模式开启，比如USB U盘播放歌曲应用模式的选择
// *CFG_FUNC_* : 软件功能开关
// *CFG_PARA_* : 系统软件参数设置
// *CFG_RES_*  : 系统硬件资源配置
// ************************************************************************************************************

//24bit 音效开关宏控制
//24bit SDK RAM开销会大幅度增加
#define  CFG_AUDIO_WIDTH_24BIT

/**音频SDK版本号：V1.0.0**/
/*0xB5：芯片B15X，01：大版本号， 00：小版本号， 00：用户修订号（由用户设定，可结合补丁号）；
 *实际存储字节序：1A 01 00 00 ，略区别于sdk版本*/
/*开启flash_boot时，用flashboot升级usercode后，boot明码区和code明码(如0xB8和0xB8+0x10000)两个值会不同，
 * 前者是burner烧录时版本，后者是mva版本需关注*/
#if BP15_ROM_VERSION == BP15_ROM_V1
#define	 CFG_SDK_VER_CHIPID			(0xB5)
#elif BP15_ROM_VERSION == BP15_ROM_V2
#define	 CFG_SDK_VER_CHIPID			(0x35)
#endif

#define  CFG_SDK_MAJOR_VERSION		(1)
#define  CFG_SDK_MINOR_VERSION		(3)
#define  CFG_SDK_PATCH_VERSION	    (0)

//****************************************************************************************
// 系统App功能模式选择
//****************************************************************************************
#define CFG_APP_IDLE_MODE_EN
#define CFG_APP_BT_MODE_EN
#define	CFG_APP_USB_PLAY_MODE_EN
// #define	CFG_APP_CARD_PLAY_MODE_EN
#define	CFG_APP_LINEIN_MODE_EN
// #define CFG_APP_RADIOIN_MODE_EN
// #define CFG_APP_USB_AUDIO_MODE_EN
// #define CFG_APP_I2SIN_MODE_EN
// #define	CFG_APP_OPTICAL_MODE_EN	// SPDIF 光纤模式
// #define CFG_APP_COAXIAL_MODE_EN	// SPDIF 同轴模式
// #define CFG_APP_HDMIIN_MODE_EN
#if defined(CFG_WIRELESS_EN) && (DECODE_CH != 0)
	#define CFG_APP_WIRELESSIN_MODE_EN
#endif

#if defined(CHIP_BT_DISABLE)
	#undef CFG_APP_BT_MODE_EN
#endif

#define CFG_FUNC_OPEN_SLOW_DEVICE_TASK

/**USB/CARD模式相关设备未插入时，可进入/不退出模式**/
//#define CFG_FUNC_APP_USB_CARD_IDLE

#ifdef CFG_APP_RADIOIN_MODE_EN
    #define FUNC_RADIO_RDA5807_EN
    //#define FUNC_RADIO_QN8035_EN  //芯片io时钟只支持12M，qn8035必须外挂晶振
#if defined(FUNC_RADIO_RDA5807_EN) && defined(FUNC_RADIO_QN8035_EN)
   #error Conflict: radio type error //不能同时选择两种显示模式
#endif
#endif

/**USB声卡，读卡器，一线通功能 **/
#define USB_VID				0x1238
#define USB_PID_BASE		0x18B5//具体PID叠加下列功能值作为Offset

#define HID					0
#define AUDIO_ONLY			1
#define MIC_ONLY			2
#define AUDIO_MIC			3
#define AUDIO_MIC_AUDIO		4
#define READER				5

#ifdef  CFG_APP_USB_AUDIO_MODE_EN
//双声卡框图 demo 在默认框图中使用USB_SOURCE_NUM 和 APP_SOURCE_NUM 分别作为两个usb下行通道
//配置 CFG_PARA_USB_MODE == AUDIO_MIC_AUDIO
//使用DEMO的方式配置音效主要是因为使用率低，切占用内存较多，暂时不加入到默认框图中。
//音效demo主要是以展示效果用，各demo功能之间切勿同时打开。因为音效框图并不一致。
	#define CFG_PARA_USB_MODE	AUDIO_MIC

	#define CFG_PARA_AUDIO_USB_IN_SYNC	//时钟偏差引起的 采样点同步
	#define CFG_PARA_AUDIO_USB_IN_SRC	//转采样准备

	#define CFG_PARA_AUDIO_USB_OUT_SYNC	//时钟偏差引起的 采样点同步
	#define CFG_PARA_AUDIO_USB_OUT_SRC	//转采样准备
	#define CFG_RES_AUDIO_USB_VOL_SET_EN

	#if((CFG_PARA_USB_MODE == MIC_ONLY) || (CFG_PARA_USB_MODE == AUDIO_MIC) || (CFG_PARA_USB_MODE == AUDIO_MIC_AUDIO))
		#define	CFG_OTG_MODE_MIC_EN			//OTG声卡 MIC开启
	#endif

	#if((CFG_PARA_USB_MODE == AUDIO_ONLY) || (CFG_PARA_USB_MODE == AUDIO_MIC) || (CFG_PARA_USB_MODE == AUDIO_MIC_AUDIO))
		#define	CFG_OTG_MODE_AUDIO_EN		//OTG声卡 AUDIO开启
	#endif

	#if((CFG_PARA_USB_MODE == READER))
		#define	CFG_OTG_MODE_READER_EN		//OTG声卡 READER开启
	#endif

	#if(CFG_PARA_USB_MODE == AUDIO_MIC_AUDIO)
		#define	CFG_OTG_MODE_AUDIO1_EN		//OTG声卡 AUDIO1开启
	#endif

#endif

//IDLE模式(假待机),powerkey/deepsleep可以同时选中也可以单独配置
//通过消息进入不同的模式
//MSG_DEEPSLEEP/MSG_POWER/MSG_POWERDOWN --> 进入IDLE模式(假待机)
//MSG_DEEPSLEEP --> 进入IDLE模式以后如果CFG_IDLE_MODE_DEEP_SLEEP打开进入deepsleep
//MSG_POWERDOWN --> 进入IDLE模式以后如果CFG_IDLE_MODE_POWER_KEY打开进入powerdown
#ifdef  CFG_APP_IDLE_MODE_EN
	// #define CFG_IDLE_MODE_POWER_KEY	//power key
	#define CFG_IDLE_MODE_DEEP_SLEEP //deepsleep
	#ifdef CFG_IDLE_MODE_POWER_KEY
		#define POWERKEY_MODE		POWERKEY_MODE_PUSH_BUTTON
		#if(POWERKEY_MODE==POWERKEY_MODE_PUSH_BUTTON)
			//#define POWERKEY_FIRST_ENTER_POWERDOWN		//第一次上电需要按下PowerKey
		#endif
	#endif
	#ifdef CFG_IDLE_MODE_DEEP_SLEEP
		/*红外按键唤醒,注意CFG_PARA_WAKEUP_GPIO_IR和 唤醒键IR_KEY_POWER设置*/
		#define CFG_PARA_WAKEUP_SOURCE_IR		SYSWAKEUP_SOURCE11_IR//固定source11
		/*ADCKey唤醒 配合CFG_PARA_WAKEUP_GPIO_ADCKEY 唤醒键ADCKEYWAKEUP设置及其电平*/
		// #define CFG_PARA_WAKEUP_SOURCE_ADCKEY	SYSWAKEUP_SOURCE1_GPIO//如使用ADC唤醒，必须打开CFG_RES_ADC_KEY_SCAN 和CFG_RES_ADC_KEY_USE
		#define CFG_PARA_WAKEUP_SOURCE_POWERKEY SYSWAKEUP_SOURCE6_POWERKEY
		// #define CFG_PARA_WAKEUP_SOURCE_IOKEY1	SYSWAKEUP_SOURCE3_GPIO
		// #define CFG_PARA_WAKEUP_SOURCE_IOKEY2	SYSWAKEUP_SOURCE4_GPIO
	#endif
#endif

#define CFG_RES_MIC_SELECT      (0)//0=NO MIC, 1= MIC


//****************************************************************************************
//                 音频输出通道相关配置
//说明:
//    如下输出源可同时输出；
//****************************************************************************************
/**DAC通道配置选择**/
#define CFG_RES_AUDIO_DAC0_EN

/**I2S音频输出通道配置选择**/
#include "i2s_interface.h"
//#define CFG_RES_AUDIO_I2SOUT_EN

/**光纤同轴音频输出通道配置选择**/
//#define CFG_RES_AUDIO_SPDIFOUT_EN
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	#define SPDIF_OUT_NUM			SPDIF1
	#define SPDIF_OUT_DMA_ID		PERIPHERAL_ID_SPDIF1_TX
	#define	SPDIF_OUT_GPIO_SET()	GPIO_PortAModeSet(GPIOA29, 0x0b)
//	#define CFG_I2S_SLAVE_TO_SPDIFOUT_EN	//Just support I2S1 and AUDIO_CLK must select DPLL
	#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		#define CFG_FUNC_EFFECT_BYPASS_EN
		#undef 	CFG_RES_AUDIO_DAC0_EN
		#undef 	CFG_RES_MIC_SELECT
		#define CFG_RES_MIC_SELECT      (0)
	#endif
#endif

#if defined(CFG_WIRELESS_EN) && (ENCODE_CH != 0) && (DECODE_CH == 0)
	#define CFG_WIRELESS_OUT_EN //配置转译，勿关闭/注销

	#define CFG_WIRELESS_OUT_ON //单向Tx 支持上电常驻
	#if defined(CFG_WIRELESS_OUT_ON) && defined(CFG_APP_BT_MODE_EN)
		#define CFG_RF_TO_BT //切回蓝牙用于射频测试
	#endif
	#if defined(CFG_APP_BT_MODE_EN) && !defined(CFG_RF_TO_BT)//单向Tx端,不带测试配置时，不支持BT
		#undef CFG_APP_BT_MODE_EN
	#endif
#endif

//#define CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
//****************************************************************************************
//     I2S相关配置选择
//说明:
//    1.I2S输出也使能时端口选择和模式需要注意保持一致;
//	  2、缺省配置为Master，I2S外设音频使用Master的MCLK,否则对方不微调时钟就会有pop音。
//    3.谨慎开启I2S slave，I2S外设必须提供稳定时钟，如果外设连线工作不稳定，请设置AudioIOSet.Sync = FALSE；自行增加DetectLock调整Sync
//****************************************************************************************
#if defined(CFG_APP_I2SIN_MODE_EN) || defined(CFG_RES_AUDIO_I2SOUT_EN)
//i2s gpio配置，必须都配置成i2s1或者i2s0
#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
	#define I2S_MCLK_GPIO					I2S1_MCLK_IN_A6 //选择MCLK_OUT/MCLK_IN脚，I2S自动配置成master/slave
	#define I2S_LRCLK_GPIO					I2S1_LRCLK_A7
	#define I2S_BCLK_GPIO					I2S1_BCLK_A9
	#ifdef CFG_RES_AUDIO_I2SOUT_EN
		#define I2S_DOUT_GPIO				I2S1_DOUT_A30
	#endif
	#ifdef CFG_APP_I2SIN_MODE_EN
		#define I2S_DIN_GPIO				I2S1_DIN_A10
	#endif
#else
	#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
		#define I2S_MCLK_GPIO					I2S0_MCLK_IN_A24 //选择MCLK_OUT/MCLK_IN脚，I2S自动配置成master/slave
	#else
		#define I2S_MCLK_GPIO					I2S0_MCLK_OUT_A24 //选择MCLK_OUT/MCLK_IN脚，I2S自动配置成master/slave
	#endif
	#define I2S_LRCLK_GPIO					I2S0_LRCLK_A20
	#define I2S_BCLK_GPIO					I2S0_BCLK_A21
	#ifdef CFG_RES_AUDIO_I2SOUT_EN
		#define I2S_DOUT_GPIO				I2S0_DOUT_A22
	#endif
	#if defined(CFG_APP_I2SIN_MODE_EN) || defined(CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN)
		#define I2S_DIN_GPIO				I2S0_DIN_A23
	#endif
#endif
#define CFG_RES_I2S_MODE				GET_I2S_MODE(I2S_MCLK_GPIO)		//根据I2S_MCLK_GPIO自动配置master/slave
																		//也可以手动配置成1或者0  0:master mode ;1:slave mode
#define	CFG_RES_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MCLK_GPIO)	//根据I2S_MCLK_GPIO自动配置i2s1/i2s0
																		//也可以手动配置成1或者0  0 ---> i2s0  1 ---> i2s1

#ifdef CFG_APP_BIS_BT_DUAL_CHIP_MODE_EN
	#define CFG_PARA_I2S_SAMPLERATE			48000
#else
	#define CFG_PARA_I2S_SAMPLERATE			44100
#endif
#define CFG_FUNC_I2S_IN_SYNC_EN			//缺省为SRA
#define CFG_FUNC_I2S_OUT_SYNC_EN
#endif

//****************************************************************************************
//                 解码器类型选择
//说明:
//    如下解码器类型选择会影响code size;
//****************************************************************************************
//打开后支持高采样率解码，资源消耗较大请自行评估（仅支持ape/flac/wav）
//建议同步开启CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000 ，减少转采样带来的消耗
//#define LOSSLESS_DECODER_HIGH_RESOLUTION

#define USE_MP3_DECODER
#define USE_WMA_DECODER
#define USE_SBC_DECODER
#define USE_WAV_DECODER
//#define USE_DTS_DECODER
// #define USE_FLAC_DECODER	//24bit 1.5Mbps高码率时，需要扩大DECODER_FIFO_SIZE_FOR_PLAYER 输出fifo，或扩大输入：FLAC_INPUT_BUFFER_CAPACITY
#define USE_AAC_DECODER
//#define USE_AIF_DECODER
//#define USE_AMR_DECODER
// #define USE_APE_DECODER

//****************************************************************************************
//                 总音效功能配置
//****************************************************************************************
//高低音调节功能配置说明:
//    1.此功能是基于MIC OUT EQ进行手动设置的，需要在调音参数中使能此EQ；
//    2.默认f5对应bass gain,f6对应treb gain,若调音界面上修改此EQ filter数目，需要对应修改BassTrebAjust()中对应序号；
//EQ模式功能配置说明:
//    1.此功能是基于MUSIC EQ进行手动设置的，需要在调音参数中使能此EQ；
//    2.可在flat/classic/pop/jazz/pop/vocal boost之间通过KEY来切换   
#if CFG_RES_MIC_SELECT
//	#define	CFG_FUNC_MIC_KARAOKE_EN      //MIC karaoke功能选择
#endif

//usb host uac 框图demo 在默认框图中增加了 USB_HOST_SOURCE_NUM 和 AUDIO_USB_HOST_SINK_NUM
//音效DEMO配置说明：
//	1.使用DEMO的方式配置音效主要是因为使用率低，切占用内存较多，暂时不加入到默认框图中。
//	2.音效demo主要是以展示效果用，各demo功能之间切勿同时打开。因为音效框图并不一致。
//#define CFG_FUNC_USB_HOST_AUDIO_MIX_MODE	//usb host uac 后台

#ifdef CFG_FUNC_MIC_KARAOKE_EN
// I2S mix mode for Karaoke
//#define CFG_RES_AUDIO_I2S_MIX_OUT_EN
//#define CFG_RES_AUDIO_I2S_MIX_IN_EN
//#define CFG_RES_AUDIO_I2S_MIX2_OUT_EN
//#define CFG_RES_AUDIO_I2S_MIX2_IN_EN
//USB Audio mix for Karaoke
#ifdef CFG_APP_USB_AUDIO_MODE_EN
//	#define CFG_FUNC_USB_AUDIO_MIX_MODE //USB Audio mix需要开启USB_AUDIO_MODE
#endif
#if defined(CFG_RES_AUDIO_I2S_MIX_IN_EN) || defined(CFG_RES_AUDIO_I2S_MIX_OUT_EN)
	#define CFG_FUNC_I2S_MIX_MODE
	#define I2S_MIX_MCLK_GPIO					I2S1_MCLK_OUT_A6 //选择MCLK_OUT/MCLK_IN脚，I2S自动配置成master/slave
	#define I2S_MIX_LRCLK_GPIO					I2S1_LRCLK_A7
	#define I2S_MIX_BCLK_GPIO					I2S1_BCLK_A9

#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	#define I2S_MIX_DIN_GPIO				I2S1_DIN_A10
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	#define I2S_MIX_DOUT_GPIO				I2S1_DOUT_A10
#endif

	#define CFG_RES_MIX_I2S_MODE				GET_I2S_MODE(I2S_MIX_MCLK_GPIO)		//根据I2S_MCLK_GPIO自动配置master/slave
	#define	CFG_RES_MIX_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MIX_MCLK_GPIO)	//根据I2S_MCLK_GPIO自动配置i2s1/i2s0
	#define CFG_PARA_MIX_I2S_SAMPLERATE		44100
	#define CFG_FUNC_MIX_I2S_IN_SYNC_EN		//缺省为SRA
	#define CFG_FUNC_MIX_I2S_OUT_SYNC_EN
#endif

#if defined(CFG_RES_AUDIO_I2S_MIX2_IN_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	#undef 	CFG_RES_AUDIO_I2SOUT_EN
	#define CFG_FUNC_I2S_MIX2_MODE
	#define I2S_MIX2_MCLK_GPIO					I2S0_MCLK_OUT_A24 //选择MCLK_OUT/MCLK_IN脚，I2S自动配置成master/slave
	#define I2S_MIX2_LRCLK_GPIO					I2S0_LRCLK_A20
	#define I2S_MIX2_BCLK_GPIO					I2S0_BCLK_A21

#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	#define I2S_MIX2_DIN_GPIO				I2S0_DIN_A22
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	#define I2S_MIX2_DOUT_GPIO				I2S0_DOUT_A23
#endif

	#define CFG_RES_MIX2_I2S_MODE				GET_I2S_MODE(I2S_MIX2_MCLK_GPIO)		//根据I2S_MCLK_GPIO自动配置master/slave
	#define	CFG_RES_MIX2_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MIX2_MCLK_GPIO)	//根据I2S_MCLK_GPIO自动配置i2s1/i2s0
	#define CFG_PARA_MIX2_I2S_SAMPLERATE		44100
	#define CFG_FUNC_MIX2_I2S_IN_SYNC_EN		//缺省为SRA
	#define CFG_FUNC_MIX2_I2S_OUT_SYNC_EN
#endif
#endif
#if defined(CFG_APP_LINEIN_MODE_EN)|| defined(CFG_FUNC_LINEIN_MIX_MODE)
	#define LINEIN_INPUT_CHANNEL				(CHIP_LINEIN_CHANNEL)
#endif

#define CFG_FUNC_AUDIO_EFFECT_EN //总音效使能开关
#ifdef CFG_FUNC_AUDIO_EFFECT_EN

	#define CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC		//自动生成音效控制代码
	//#define CFG_FUNC_EFFECT_BYPASS_EN		//开启后默认运行bypass音效框图，用于音频指标测试
	#ifdef CFG_FUNC_EFFECT_BYPASS_EN
		#undef CFG_FUNC_MIC_KARAOKE_EN
	#endif

    //#define CFG_FUNC_ECHO_DENOISE          //消除快速调节delay时的杂音，
 	//#define CFG_FUNC_MUSIC_EQ_MODE_EN     //Music EQ模式功能配置

	#ifdef CFG_FUNC_MIC_KARAOKE_EN
		#define CFG_FUNC_MIC_TREB_BASS_EN    	//Mic高低音调节功能配置
		#define CFG_FUNC_MUSIC_TREB_BASS_EN    //Music高低音调节功能配置
		//闪避功能选择设置
		//注:若需要完善移频开启后的啾啾干扰声问题，需要开启此功能(利用MIC信号检测接口处理)
	//	#define  CFG_FUNC_SHUNNING_EN
			#define SHNNIN_VALID_DATA                          	 500  ////MIC音量阈值
			#define SHNNIN_STEP                                  1  /////单次调节的步，对应VolArr中的一级
			#define SHNNIN_THRESHOLD                             SHNNIN_STEP*10  ////threshold
			#define SHNNIN_VOL_RECOVER_TIME                      50////伴奏音量恢复时长：50*20ms = 1s
			#define SHNNIN_UP_DLY                                3/////音量上升时间
			#define SHNNIN_DOWN_DLY                              1/////音量下降时间

			#define CFG_FLOWCHART_KARAOKE_ENABLE
	#endif
    //#define CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN     //无信号自动关机功能，
    #ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN      
		#define  SILENCE_THRESHOLD                 120        //设置信号检测门限，小于这个值认为无信号
		#define  SILENCE_POWER_OFF_DELAY_TIME      10*60*1000 //无信号关机延时时间:10Min，单位：ms
    #endif

	#define CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN//在线调音
	#ifdef CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN
		#define  CFG_COMMUNICATION_BY_USB			//在线调音硬件接口USB HID
//		#define  CFG_COMMUNICATION_BY_UART   		//UART 在线调音，注意DMA资源是否足够
		#ifdef CFG_COMMUNICATION_BY_UART
			#define CFG_FUNC_COMMUNICATION_RX_PORT 			DEBUG_RX_A0		//需要和打印配置成不同的UART
			#define CFG_FUNC_COMMUNICATION_TX_PORT 			DEBUG_TX_A1
			#define CFG_FUNC_COMMUNICATION_UART_BAUDRATE 	DEBUG_BAUDRATE_115200
			#define	CFG_FUNC_COMMUNICATION_RX_DMA_PORT		PERIPHERAL_ID_UART0_RX
			#define	CFG_FUNC_COMMUNICATION_TX_DMA_PORT		PERIPHERAL_ID_UART0_TX
		#endif
		#define  CFG_COMMUNICATION_CRYPTO						(0)////调音通讯加密=1 调音通讯不加密=0
		#define  CFG_COMMUNICATION_PASSWORD                     0x11223344//////四字节的长度密码
	#endif

	//使用flash存好的调音参数
	//音效参数存储于flash固定区域中
//	#define CFG_EFFECT_PARAM_IN_FLASH_EN
	#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
		#define CFG_EFFECT_PARAM_IN_FLASH_SIZE			(16)//KB，分配给音效参数在线下载的flash空间
		#ifdef CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN
			#define CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
		#endif
	#endif

#endif

//****************************************************************************************
//     转采样功能选择
//说明:
//    1.使能该宏表示系统会将会以统一的采样率输出，默认使用44.1KHz;
//    2.此版本默认打开，请勿关闭!!!!!!!!!!
//****************************************************************************************	
#define	CFG_FUNC_MIXER_SRC_EN
//#define	CFG_FUNC_SRC_HIGHER_EN
//#define	CFG_FUNC_USB_ADJUST_UNION_EN			//usb上下行硬件微调共用机制
#define	CFG_FUNC_AUDIO_SILENCE_AVOIDANCE_EN		//DAC防播空机制

//     采样率硬件微调功能选择
//说明:
//	     硬件微调同一时刻只可使能开启一个微调。使系统AUPLL时钟跟随音源
//****************************************************************************************	
//#define	CFG_FUNC_FREQ_ADJUST
#ifdef CFG_FUNC_FREQ_ADJUST
	#define CFG_PARA_BT_FREQ_ADJUST		//Btplay 模式续存期间 硬件微调，与CFG_PARA_BT_SYNC宏配合
 	#define CFG_PARA_HFP_FREQ_ADJUST	//通话模式 续存期间 硬件微调  使用上行微调，下行跟随。 与CFG_PARA_HFP_SYNC配合
#endif

//****************************************************************************************
//                 ADC/DAC/I2S mclk功能配置
//说明:
//    1.如下宏开启则基于调音工具在线导出ADC,DAC,I2S时钟配置初始化mclk
//    2.该功能与CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000冲突
//****************************************************************************************
#ifndef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
//	#define	CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
#endif
//****************************************************************************************
//                 录音功能配置
//说明:CFG_FUNC_RECORD_EXTERN_FLASH_EN	//录制多段提示音到外置flash/内部flash  用于特效音录制和播放
//****************************************************************************************
//#define CFG_FUNC_RECORDER_EN
#ifdef CFG_FUNC_RECORDER_EN
	#define CFG_FUNC_RECORD_SD_UDISK	//录音到SD卡或者U盘
//	#define	CFG_FUNC_RECORD_EXTERN_FLASH_EN
	//录制多段提示音到外置flash/内部flash用于特效音录制和播放，和CFG_FUNC_RECORD_SD_UDISK只能2选1
	//关闭USE_EXTERN_FLASH_SPACE宏，录制特效音到内部flash，还需要到 \BT_Audio_APP\tools\merge_script\merge.ini配置地址
	//merge.ini中开启如下配置，	maxlen长度需要配置为 CFG_PARA_RECORDS_MAX_SIZE * CFG_PARA_RECORDS_INDEX
	//	[rec_data]
	//	virtual = 1
	//	fullpath=..\merge_script\rec_data.bin
	//	maxlen = 0x30000
	//	enable = 1
	#define WAV_BIT_PER_SAMPLE					16	//WAV格式采样位数16，24
	#define RECORD_FORMAT_WAV					1	//录音格式WAV
	#define RECORD_FORMAT_MP2					2	//录音为mp2格式
	#define RECORD_FORMAT_MP3					3	//录音为mp3格式

	#ifdef CFG_FUNC_RECORD_SD_UDISK
		#define CFG_FUNC_RECORD_UDISK_FIRST				//U盘和卡同时存在时，录音设备优先选择U盘，否则优先选择录音到SD卡。
		#define CFG_PARA_RECORDS_FOLDER 		"REC"	//录卡录U盘时根目录文件夹。注意ffpresearch_init 使用回调函数过滤字符串。
		#define CFG_FUNC_RECORDS_MIN_TIME		1000	//单位ms，开启此宏后，小于这个长度的自动删除。
		#define CFG_PARA_REC_MAX_FILE_NUM       256     //录音文件最大数目

		#define MEDIAPLAYER_SUPPORT_REC_FILE            // U盘或TF卡模式下，打开此功能，则支持播放录音文件；否则，只能在录音回放模式下播放录音文件
        //#define AUTO_DEL_REC_FILE_FUNCTION            //录音文件达到最大数后，自动删除全部录音文件的功能选项
	#endif
	#define DEL_REC_FILE_EN

	#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
//		#define USE_EXTERN_FLASH_SPACE		//使用外置flash空间(屏蔽这行使用芯片内置flash)
		#define CFG_PARA_RECORDS_MAX_SIZE			(0x30000) 	// 定义为一个录音文件的大小192K
		#define CFG_PARA_RECORDS_INFO_SIZE			256				// 定义256BYTE空间，用来放置一些想保存的录音信息，例如录音时长、录音大小等等等
		#define	EXTERN_FLASH_RECORDER_FILE_SECOND	30				//单个文件录音时间
		#ifdef USE_EXTERN_FLASH_SPACE
			#define CFG_PARA_RECORDS_INDEX				4			// 定义最大的允许录音数量
			#define	CFG_PARA_RECORDS_FLASH_BEGIN_ADDR	0			// 录音文件的起始地址
			#define	SpiWrite(a,b,c)						SPI_Flash_Write(a,b,c)
			#define	SpiRead(a,b,c)						SPI_Flash_Read(a,b,c)
			#define SpiErase(a)							SPI_Flash_Erase_4K(a)
		#else
			#define CFG_PARA_RECORDS_INDEX				1			// 定义最大的允许录音数量
			#define CFG_FLASH_MUTEX_USE
			#define	SpiWrite(a,b,c)						SpiFlashWriteWpr(a,b,c,1)
			#define	SpiRead(a,b,c)						SpiFlashReadWpr(a,b,c,1)
			#define SpiErase(a)							SpiFlashEraseWpr(SECTOR_ERASE, a, 1)
		#endif
	#endif

	//N >= 2 ；考虑128系统帧以及加音效MIPS较高，优先级为3的编码进程处理数据较慢，推荐值为 6。提高系统帧，mips低时可以调小N,节约ram。
	#define MEDIA_RECORDER_FIFO_N				6
	#define MEDIA_RECORDER_FIFO_LEN				(CFG_PARA_MAX_SAMPLES_PER_FRAME * MEDIA_RECORDER_CHANNEL * MEDIA_RECORDER_FIFO_N)
	//调整下列参数后，录音介质可能需要重做兼容性测试 适配FILE_WRITE_FIFO_LEN。
	#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
		#define MEDIA_RECORDER_CHANNEL				1
		#define MEDIA_RECORDER_BITRATE				48 //Kbps
		#define MEDIA_RECORDER_FORMAT				RECORD_FORMAT_MP2
	#else
	#define MEDIA_RECORDER_CHANNEL				2
	#define MEDIA_RECORDER_BITRATE				96 //Kbps
	#define MEDIA_RECORDER_FORMAT				RECORD_FORMAT_MP3	// RECORD_FORMAT_WAV //RECORD_FORMAT_MP3 //RECORD_FORMAT_MP2
	#endif


	//FIFO_Len=(码率(96) / 8 * 缓冲时间ms(1000) （码率单位Kbps,等效毫秒）
	//根据SDIO协议，写卡阻塞存在250*2ms阻塞 可能，实测部分U盘存在785ms周期性写入阻塞，要求编码数据fifo空间 确保超过这个长度的两倍(含同步)。
	#if MEDIA_RECORDER_FORMAT == RECORD_FORMAT_WAV	//录音格式WAV
		#define FILE_WRITE_FIFO_LEN					((((48*MEDIA_RECORDER_CHANNEL*(WAV_BIT_PER_SAMPLE/8)) * 200 ) / 512) * 512) //200ms
	#else
		#define MEDIA_RECODER_IO_BLOCK_TIME			1000//ms
		#define FILE_WRITE_FIFO_LEN					((((MEDIA_RECORDER_BITRATE / 8) * MEDIA_RECODER_IO_BLOCK_TIME ) / 512) * 512)//(依据U盘/Card兼容性需求和RAM资源可选400~1500ms。按扇区512对齐
	#endif
#endif //CFG_FUNC_RECORDER_EN
//****************************************************************************************
//                 U盘或SD卡模式相关功能配置
//    
//****************************************************************************************
#if(defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN) || BT_AVRCP_SONG_TRACK_INFOR)
/**LRC歌词功能 **/
//#define CFG_FUNC_LRC_EN			 	// LRC歌词文件解析

/*------browser function------*/
//#define FUNC_BROWSER_PARALLEL_EN  		//browser Parallel
//#define FUNC_BROWSER_TREE_EN  			//browser tree
#if	defined(FUNC_BROWSER_PARALLEL_EN)||defined(FUNC_BROWSER_TREE_EN)
#define FUNCTION_FILE_SYSTEM_REENTRY
#if defined(FUNC_BROWSER_TREE_EN)||defined(FUNC_BROWSER_PARALLEL_EN)
#define GUI_ROW_CNT_MAX		5		//最多显示多少行
#else
#define GUI_ROW_CNT_MAX		1		//最多显示多少行
#endif
#endif
/*------browser function------*/

/**字符编码类型转换 **/
/**目前支持Unicode     ==> Simplified Chinese (DBCS)**/
/**字符转换库由fatfs提供，故需要包含文件系统**/
/**如果支持转换其他语言，需要修改fatfs配置表**/
#define CFG_FUNC_STRING_CONVERT_EN	// 支持字符编码转换

//#define CFG_FUNC_DECRYPT_EN     //支持加密文件播放功能(加密文件由MVAssistant上位机工具生成)
/**取消AA55判断**/
/*fatfs内磁盘系统MBR和DBR扇区结尾有此标记检测，为提高非标类型盘兼容性，可开启此项, 为有效鉴定无效盘，此项默认关闭*/
//#define	CANCEL_COMMON_SIGNATURE_JUDGMENT
//#define FUNC_UPDATE_CONTROL   //升级交互过程控制(通过按键确认升级)
#endif

/**USB Host检测功能**/
#if(defined(CFG_APP_USB_PLAY_MODE_EN)||defined(CFG_FUNC_USB_HOST_AUDIO_MIX_MODE))
#define CFG_RES_UDISK_USE
#define CFG_FUNC_UDISK_DETECT
#endif

/**USB Device检测功能**/
#if (defined (CFG_APP_USB_AUDIO_MODE_EN)) || (defined(CFG_COMMUNICATION_BY_USB))
	#define CFG_FUNC_USB_DEVICE_DETECT
#endif


//****************************************************************************************
//****************************************************************************************
/**OS操作系统进入IDLE时经core进入休眠状态，以达到降低功耗目的**/
/*注意，这是OS调度的IDLE，并非应用层APPMODE，应用层无需关心*/
#ifndef CFG_FUNC_MIC_KARAOKE_EN //KARAOKE配置下，默认关闭
#define CFG_FUNC_IDLE_TASK_LOW_POWER
#ifdef	CFG_FUNC_IDLE_TASK_LOW_POWER
	#define	CFG_GOTO_SLEEP_USE
#endif
#endif
//************************************************************************************************************
//* 低功耗优化,根据各个功能模块的优化,达到降低功耗的目的
//* 注意: 各个模块的低功耗优化方式和模块性能有关系
//************************************************************************************************************
#define CFG_FUNC_SYSTEM_LOW_POWER
#ifdef CFG_FUNC_SYSTEM_LOW_POWER
#define CFG_ADCDAC_SEL_LOWPOWERMODE  //ADC/DAC使用低功耗模式
#endif

//****************************************************************************************
//                 UART DEBUG功能配置
//注意： DEBUG打开后，会增大mic通路的delay，不需要DEBUG调试代码时，建议关闭掉！
//		SHELL功能需要开启 UART DEBUG功能
//****************************************************************************************
#define CFG_FUNC_DEBUG_EN
//#define CFG_FUNC_USBDEBUG_EN
#ifdef CFG_FUNC_DEBUG_EN
	#define CFG_USE_SW_UART
	#ifdef CFG_USE_SW_UART
		#define SW_UART_IO_PORT					SWUART_GPIO_PORT_A//SWUART_GPIO_PORT_B
		#define SW_UART_IO_PORT_PIN_INDEX		17//bit num
		#define CFG_SW_UART_BANDRATE   			512000//software uart baud rate select:115200 512000 ,default 512000
	#else
		#define CFG_UART_TX_PORT 				DEBUG_TX_A10
		#define CFG_UART_BANDRATE   			DEBUG_BAUDRATE_2000000//DEBUG_BAUDRATE_115200
		#define CFG_FLASHBOOT_DEBUG_EN          (1)
	#endif
//	#define CFG_FUNC_DEBUG_USE_TIMER		//使用定时器进行打印
//	#define CFG_FUNC_SHELL_EN				//SHELL功能配置
	#ifdef	CFG_FUNC_SHELL_EN
		//UART RX配置,需要和串口日志打印为同一个UART组
		#define CFG_FUNC_SHELL_RX_PORT 		DEBUG_RX_A9
	#endif
#else
	#define	APP_DBG(format, ...)
	#define	DBG(format, ...)
	#define	OTG_DBG(format, ...)
	#define	BT_DBG(format, ...)
#endif

#ifndef CFG_COMMUNICATION_BY_UART
//#define CFG_UPGRADE_BY_UART
#endif
#ifdef CFG_UPGRADE_BY_UART
		#define CFG_FUNC_UPGRADE_RX_PORT 			DEBUG_RX_A5		//需要和打印配置成不同的UART
		#define CFG_FUNC_UPGRADE_TX_PORT 			DEBUG_TX_A6
		#define CFG_FUNC_UPGRADE_UART_BAUDRATE 		DEBUG_BAUDRATE_2000000
		#define	CFG_FUNC_UPGRADE_RX_DMA_PORT		PERIPHERAL_ID_UART0_RX
		#define	CFG_FUNC_UPGRADE_TX_DMA_PORT		PERIPHERAL_ID_UART0_TX
#endif

//****************************************************************************************
//                 提示音功能配置
//说明:
//    1.烧录工具参见MVs26_SDK\tools\script；
//    2.提示音功能开启，注意flash中const data提示音数据需要预先烧录，否则不会播放;
//    3.const data数据开机检查，影响开机速度，主要用于验证。
//****************************************************************************************
#define CFG_FUNC_REMIND_SOUND_EN
#ifdef CFG_FUNC_REMIND_SOUND_EN
	#define CFG_PARAM_FIXED_REMIND_VOL   	CFG_PARA_SYS_VOLUME_DEFAULT		//固定提示音音量值,0表示受music vol同步控制
#endif

//****************************************************************************************
//                 断点记忆功能配置        
//****************************************************************************************
#define CFG_FUNC_BREAKPOINT_EN
#ifdef CFG_FUNC_BREAKPOINT_EN
	#define BP_PART_SAVE_TO_NVM			// 断点信息保存到NVM
	#define BP_SAVE_TO_FLASH			// 断电信息保存到Flash
	#define FUNC_MATCH_PLAYER_BP		// 获取FS扫描后与播放模式断点信息匹配的文件。文件夹和ID号
#endif

//****************************************************************************************
//                            Key 按键相关配置
//****************************************************************************************
/**按键beep音功能**/
//#define  CFG_FUNC_BEEP_EN
#define CFG_PARA_BEEP_DEFAULT_VOLUME    15//注意:若蓝牙音量同步功能开启后，此值最大为16

/**按键双击功能**/
#define  CFG_FUNC_DBCLICK_MSG_EN

/**ADC按键**/
#define CFG_RES_ADC_KEY_SCAN

/**IR按键**/
// #define CFG_RES_IR_KEY_SCAN				//启用device service Key扫描IRKey

/**编码旋钮按键**/
//#define	CFG_RES_CODE_KEY_USE

/**GPIO按键**/
//#define CFG_RES_IO_KEY_SCAN

/**电位器功能选择**/
#define CFG_ADC_LEVEL_KEY_EN

//***************************************************************************************
//					RTC/闹钟功能配置
//    OSC_32K: RTC时钟选择晶体晶振32.768kHZ,芯片引脚：GPIOB5/GPIOB9，仅部分型号支持
//    OSC_24M: RTC时钟选择晶体晶振24MHZ
//***************************************************************************************
//#define CFG_FUNC_RTC_EN
#ifdef CFG_FUNC_RTC_EN
	#define CFG_FUNC_RTC_OSC_FREQ		OSC_32K
	#define CFG_FUNC_ALARM_EN  			//闹钟功能,必须开时钟
	#define CFG_FUNC_LUNAR_EN  			//万年历,必须开时钟
	#ifdef CFG_FUNC_ALARM_EN
		#define CFG_FUNC_SNOOZE_EN 		//闹钟贪睡功能
	#endif
#endif

//****************************************************************************************
//                            Display 显示配置
//****************************************************************************************
//#define  CFG_FUNC_DISPLAY_EN
#ifdef CFG_FUNC_DISPLAY_EN
//  #define  DISP_DEV_SLED
  #define  DISP_DEV_7_LED
/**8段LED显示操作**/
/*LED显存刷新需要在Timer1ms中断进行，读写flash操作时会关闭中断*/
/*所以需要做特殊处理，请关注该宏包含的代码段*/
/*注意timer中断服务函数和调用到的API必须进入TCM，含调用的所有api，库函数请咨询支持*/
/*开启此宏，要关注所有使用NVIC_SetPriority 设置为0的代码，避免对应中断调用非TCM代码引起死机复位*/
#ifdef DISP_DEV_7_LED
  #define	CFG_FUNC_LED_REFRESH
#endif

#if defined(DISP_DEV_SLED) && defined(DISP_DEV_7_LED) && defined(LED_IO_TOGGLE)
   #error Conflict: display setting error //不能同时选择两种显示模式
#endif
#endif

//****************************************************************************************
//				   耳机插拔检测功能选择设置
//****************************************************************************************
//#define  CFG_FUNC_DETECT_PHONE_EN                            

//****************************************************************************************
//				   3线，4线耳机类型检测功能选择设置
//****************************************************************************************
//#define  CFG_FUNC_DETECT_MIC_SEG_EN  

//flash系统参数在线调整
#define CFG_FUNC_FLASH_PARAM_ONLINE_TUNING_EN

//CAN功能demo
//#define CFG_FUNC_CAN_DEMO_EN

//AI_DENOISE demo,IDE需要使用V323或者以后的版本,需要专用芯片
//开启AI_DENOISE，资源有限建议关掉BT模式，提示音等功能，需要同时开启CFG_DOUBLE_KEY_EN宏
//下面宏定义采用脚本控制，必须单独一行，不要在该行后面添加注释
//#define CFG_AI_DENOISE_EN

//双key方案 demo，可以单独开启。
//需要注意：双key方案 工具链需要配套 MVAssistant_V3.3.0版本或者以后版本
//开启CFG_AI_DENOISE_EN，需要同时开启 CFG_DOUBLE_KEY_EN
//下面宏定义采用脚本控制，必须单独一行，不要在该行后面添加注释
//#define CFG_DOUBLE_KEY_EN

#if defined(CFG_AI_DENOISE_EN) &&(!defined(CFG_DOUBLE_KEY_EN))
	#error 开启CFG_AI_DENOISE_EN 需要同时开启 CFG_DOUBLE_KEY_EN //开启CFG_AI_DENOISE_EN，需要同时开启 CFG_DOUBLE_KEY_EN
#endif

//开启flashboot，BT_OTA功能
//#define CFG_FUNC_BT_OTA_EN

#include "sys_gpio.h"
#include "clock_config.h"

//************************************************************************************************************
//dump工具,可以将数据发送到dump工具,用于分析
//注意1:每个模式下DmaChannelMap的配置,需要占用一路DMA配置用于定义CFG_DUMP_UART_TX_DMA_CHANNEL; 
//      参考main_task.c的DmaChannelMap
//注意2:默认DUMP_UART是A31-UART1,需要和DEBUG_UART进行区分
//************************************************************************************************************
//#define CFG_DUMP_DEBUG_EN
#ifdef CFG_DUMP_DEBUG_EN
	//UART配置,需要和串口日志打印为不同的UART组
	#define CFG_DUMP_UART_TX_PORT 				DEBUG_TX_A31
	#define CFG_DUMP_UART_BANDRATE 				(2000000)
	#define CFG_DUMP_UART_TX_DMA_CHANNEL		(PERIPHERAL_ID_UART0_TX + 2*GET_DEBUG_GPIO_UARTPORT(CFG_DUMP_UART_TX_PORT))
#endif

#include "i2s_dma_cfg.h"

#endif /* APP_CONFIG_H_ */
