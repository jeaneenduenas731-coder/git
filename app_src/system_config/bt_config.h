///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_config.h
//  maintainer: KK
///////////////////////////////////////////////////////////////////////////////
#ifndef __BT_DEVICE_CFG_H__
#define __BT_DEVICE_CFG_H__

#define ENABLE						(1)
#define DISABLE						(0)

/* "sys_param.h" 蓝牙基础参数*/
/*****************************************************************
 * 蓝牙频偏参数
 *****************************************************************/
#define BT_DEFAULT_TRIM				0x06 //trim范围:0x00~0x0f

#define BT_MIN_TRIM					0x0
#define BT_MAX_TRIM					0xf

/*****************************************************************
 * 蓝牙功能开关(经典蓝牙BT和低功耗蓝牙BLE模块)
 *****************************************************************/
#define BLE_SUPPORT					DISABLE
#define BT_SUPPORT			        ENABLE

/*****************************************************************
 * BLE Central And Peripheral (主从一体)
 * 扫描+广播 分时复用
 *****************************************************************/
#if (BLE_SUPPORT == ENABLE)
#define BLE_DUAL_ROLE 				DISABLE //BLE主从一体开关
#endif

/*****************************************************************
 * 蓝牙发射功能配置
 *****************************************************************/
#define BT_SOURCE_SUPPORT			DISABLE

#if BT_SOURCE_SUPPORT
#define A2DP_SET_SINK 				1
#define A2DP_SET_SOURCE 			2
#define BT_SINK_NAME				"BP10"		//扫描设备名称过滤条件,  可增加多个蓝牙名称匹配连接,在"BtSourceNameMatch"中增加即可

#define SUPPORT_SOURCE_SINK_AUTO_SWITCH_FUNC 
//#define SOURCE_ACCESS_MODE_ENABLE		// 在蓝牙发射模式下,允许被搜索被连接
//#define SOURCE_AUTO_INQUIRY_AND_CONNECT	// source模式下自动INQUIRY,然后连接设备

#ifdef SOURCE_AUTO_INQUIRY_AND_CONNECT
#define SOURCE_AUTO_INQUIRY_AND_CONNECT_RSSI	//自动INQUIRY，连接信号最好的设备
#endif

#define A2DP_SCAN_NUMBER			10 //扫描设备最大保存个数
#define A2DP_INQUIRY_TIMEOUT		10 //unit:s   default:10s

#endif /* BT_SOURCE_SUPPORT */


/*****************************************************************
 * 2.4G功能配置
 * 关联script_copy.ini,无BLE、无BT_SOURCE_SUPPORT
 *****************************************************************/
#define WIRELESS_SUPPORT			DISABLE//ENABLE//

/*****************************************************************
 * 蓝牙通用功能配置
 *****************************************************************/
//#define BT_SNIFF_ENABLE			//sniff功能开关
//#define BT_SNIFF_DEEPSLEEP_ENABLE    //在sniff模式允许进入deepsleep

//#define BT_PROFILE_BQB_ENABLE		//BQB认证开关

/*****************************************************************
 * 蓝牙名称注意事项:
 * 1.蓝牙名称支持中文,需要使用URL编码,开启bt_name.h,屏蔽此文件下的BT_NAME定义
 * 2.BLE的名称修改在ble广播数据中体现(ble_app_func.c)
 *****************************************************************/
//#include "bt_name.h"
#define BT_NAME						"DB-96BT"
#define BLE_NAME					"BP15_BLE"

//蓝牙发射功率配置
/* Rf Tx Power Range, max:7dbm, step:1dbm/step
{   level  dbm
	[23] = 7,
	[22] = 6,
	[21] = 5,
	[20] = 4,
	[19] = 3,
	[18] = 2,
	[17] = 1,
	[16] = 0,
}
*/
#define BT_TX_POWER_LEVEL			23		//蓝牙正常工作时发射功率
#define BT_PAGE_TX_POWER_LEVEL		16		//蓝牙回连发射功率(减小上电时蓝牙回连的干扰声)

//BT 后台设置
//0 -> BT后台不能连接手机
//1 -> BT后台可以连接手机
//2 -> 无后台
enum
{
	BT_BACKGROUND_FAST_POWER_ON_OFF = 0,
	BT_BACKGROUND_POWER_ON = 1,
	BT_BACKGROUND_DISABLE = 2,
};
#if BT_SOURCE_SUPPORT
#define SYS_BT_BACKGROUND_TYPE		BT_BACKGROUND_POWER_ON
#else
#define SYS_BT_BACKGROUND_TYPE		BT_BACKGROUND_FAST_POWER_ON_OFF		//0 -> BT后台不能连接手机
#endif

//#define FLASH_SAVE_REMOTE_BT_NAME		//是否需要记忆已连设备的蓝牙名称

//蓝牙简易配对功能
#define BT_SIMPLEPAIRING_FLAG		TRUE 	//0:use pin code; 1:simple pairing
#define BT_PINCODE					"0000"	//简易配对设置为0,则通过PINCODE进行连接

#define BT_ADDR_SIZE				6
#define BT_LSTO_DFT					8000    //连接超时时间 		//timeout:8000*0.625=5s
#define BT_PAGE_TIMEOUT				8000	//page timeout	//timeout:8000*0.625=5s  

//蓝牙异步时钟采样率微调配置
#define	CFG_PARA_BT_SYNC					//BtPlay 异步时钟 采样点同步
#define CFG_PARA_HFP_SYNC					//通话 异步时钟 采样点同步
	
/*****************************************************************
 * 蓝牙协议宏定义开关
 *****************************************************************/
#define BT_A2DP_SUPPORT				(1)		//音乐播放和控制(A2DP和AVRCP关联)
#define BT_HFP_SUPPORT				(0)		//通话功能
#define BT_SPP_SUPPORT				(0)		//数据传输
#define BT_OBEX_SUPPORT				(0)		//文件传输(适用于安卓手机通过文件推送进行OTA升级)
#define BT_PBAP_SUPPORT				(0)		//电话本
#define BT_HID_SUPPORT				(0)		

#if BT_SOURCE_SUPPORT && BT_HFP_SUPPORT
#define BT_HFG_SUPPORT				(1)		//语音发射(暂只支持CVSD格式--8KHz)
#else
#define BT_HFG_SUPPORT				(0)
#endif

/*****************************************************************
 *****************************************************************/
//#define BT_AUTO_ENTER_PLAY_MODE	//在非蓝牙模式下,后台蓝牙连上手机,播放音乐自动切换到播放模式

//经典蓝牙上电开机的可见性配置
//0 -> 不可见不可连接
//1 -> 可见不可连接
//2 -> 不可见可连接
//3 -> 可见可连接
#define		BT_ACCESSBLE_NONE					0
#define		BT_ACCESSBLE_DISCOVERBLEONLY		1
#define		BT_ACCESSBLE_CONNECTABLEONLY		2
#define		BT_ACCESSBLE_GENERAL				3

#define POWER_ON_BT_ACCESS_MODE_SET  BT_ACCESSBLE_GENERAL

#if (POWER_ON_BT_ACCESS_MODE_SET == BT_ACCESSBLE_GENERAL)
// #define BT_ACCESS_MODE_SET_BY_POWER_ON_TIMEOUT		//开机蓝牙可见性设置延时,按键MSG_BT_OPEN_ACCESS消息打开可见性
// #define BT_VISIBILITY_DELAY_TIME		10000	//单位:ms  开机蓝牙可见性设置延时时间
#endif

/*****************************************************************
 * 连接链路参数配置
 * 包含作为蓝牙SINK,支持双手机连接配置
 *****************************************************************/
//#define BT_MULTI_LINK_SUPPORT		//双手机连接
#ifdef BT_MULTI_LINK_SUPPORT
#define LAST_PLAY_PRIORITY				//后播放优先
//#define BT_LINK_2DEV_ACCESS_DIS_CON		//开启后,第一个手机连上后,第二个手机需要能搜索到; 关闭后,第二个手机搜索不到,能回连上
#endif

/*****************************************************************
 * 蓝牙协议连接链路参数配置
 * 不要随意修改
 *****************************************************************/
#ifdef BT_MULTI_LINK_SUPPORT
#define BT_LINK_DEV_NUM				2 	//蓝牙连接手机个数 (1 or 2)
#define BT_DEVICE_NUMBER			2	//蓝牙ACL连接个数 (1 or 2)
#define BT_SCO_NUMBER				2	//蓝牙通话链路个数 (1 or 2),BT_SCO_NUMBER必须小于BT_DEVICE_NUMBER
#else
#define BT_LINK_DEV_NUM				1
#define BT_DEVICE_NUMBER			1
#define BT_SCO_NUMBER				1
#endif

/*****************************************************************
 * A2DP profile
 *****************************************************************/
#if BT_A2DP_SUPPORT

#define BT_AUDIO_AAC_ENABLE		//AAC解码开关
//Note1:开启AAC,需要同步开启解码器类型USE_AAC_DECODER(app_config.h)
//Note2:目前双手机配置跟AAC解码不能同时打开
#if defined(BT_PROFILE_BQB_ENABLE) && !defined(BT_AUDIO_AAC_ENABLE)
	#define BT_AUDIO_AAC_ENABLE
#endif

#if defined(BT_AUDIO_AAC_ENABLE) && !defined(USE_AAC_DECODER)
	#define USE_AAC_DECODER
#endif
#if (defined(BT_AUDIO_AAC_ENABLE) && defined(BT_MULTI_LINK_SUPPORT))
#error Conflict: BT_AUDIO_AAC_ENABLE and BT_MULTI_LINK_SUPPORT setting error
#endif

/*****************************************************************
 * AVRCP profile
 *****************************************************************/
#define BT_AVRCP_VOLUME_SYNC			(0)		//音量同步功能开关

//(eg:EQ/repeat mode/shuffle/scan configuration)
#define BT_AVRCP_PLAYER_SETTING			(0)		//歌曲的循环模式(只支持苹果手机自带播放器)

#define BT_AVRCP_SONG_PLAY_STATE		(0)		//歌曲播放状态和时间获取

#define BT_AVRCP_SONG_TRACK_INFOR		(0)		//歌曲和歌词信息获取(尽量和歌曲播放状态同时使用)

#define BT_AUTO_PLAY_MUSIC				(0)		//AVRCP连接成功后，自动播放歌曲

/*****************************************************************
 *
 * get remote device time
 * android: map
 * ios:hfp
 *
 *****************************************************************/
//#define BT_GETTIME_SUPPORT


#endif /* BT_A2DP_SUPPORT */

/*****************************************************************
 * HFP profile
 *****************************************************************/
//蓝牙通话来电铃声配置
//0 -> 不支持来电铃声
//1 -> 来电报号和铃声
//2 -> 使用手机铃声，若没有则播本地铃声
//3 -> 强制使用本地铃声
enum
{
	USE_NULL_RING = 0,
	USE_NUMBER_REMIND_RING = 1,
	USE_LOCAL_AND_PHONE_RING = 2,
	USE_ONLY_LOCAL_RING = 3,
};
#define SYS_DEFAULT_RING_TYPE		USE_LOCAL_AND_PHONE_RING			

#if BT_HFP_SUPPORT
//0: only cvsd
//1: cvsd + msbc
#define BT_HFP_SUPPORT_WBS				(1)

//电池电量同步(开启需要和 CFG_FUNC_POWER_MONITOR_EN 关联)
//#define BT_HFP_BATTERY_SYNC			//电池电量同步(本地电量同步到手机端显示)

#define BT_REMOTE_AEC_DISABLE			//关闭手机端AEC

#define BT_HFP_MIC_PGA_GAIN				15  //ADC PGA GAIN +18db(0~31, 0:max, 31:min)

#define BT_HFP_CALL_DURATION_DISP		//来电通话时长配置选项

//开启HFP连接，不使能进入HFP相关模式(推送电池电量显示)
//该应用适用于某些需要连接HFP协议，但是不需要HFP相关功能的场合
//#define BT_HFP_MODE_DISABLE

#endif /* BT_HFP_SUPPORT */

/*****************************************************************
 * 蓝牙回连参数配置
 *****************************************************************/
#define BT_RECONNECTION_SUPPORT			(1)		// 开启自动重连(开机或者切换模式)
#define BT_POR_TRY_COUNTS				(5)			// 开机重连尝试次数
#define BT_POR_INTERNAL_TIME			(3)			// 开机重连每两次间隔时间(in seconds)

#define BT_BLR_RECONNECTION_SUPPORT		(1)		// 开启BB Lost(连接丢失)重连
#define BT_BLR_TRY_COUNTS				(90)		// BB Lost 尝试重连次数
#define BT_BLR_INTERNAL_TIME			(5)			// BB Lost 重连每两次间隔时间(in seconds)


/*****************************************************************
 *
 * OBEX config (mva通过obex进行升级,升级方式:双bank)
 * 使用说明：
 * 1. chip_config.h中,需要注意flash容量,如芯片使用16Mb,要通过OBEX实现双BANK升级,则需要将CHIP_FLASH_CAPACTITY配置为8Mb
 * 2. OBEX双bank升级,数据缓存ram需要63KB,需要注意下
 * 
 *****************************************************************/
#if BT_OBEX_SUPPORT
//#define MVA_BT_OBEX_UPDATE_FUNC_SUPPORT			//Obex协议进行蓝牙双BANK升级功能
#endif /*BT_OBEX_SUPPORT*/

#if BT_SOURCE_SUPPORT
	#include "bt_source.h"
#endif

#if WIRELESS_SUPPORT  && !(BLE_SUPPORT || BT_SOURCE_SUPPORT)
	#include "wireless_config.h" //仅在此引用
	#define  CFG_WIRELESS_EN	//for App config
#endif
#endif /*__BT_DEVICE_CFG_H__*/

