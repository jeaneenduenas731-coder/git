/*
 * wireless_config.h
 *
 *  Created on: Dec 6, 2024
 *      Author: piwang
 */

#ifndef __WIRELESS_CONFIG_H__
#define __WIRELESS_CONFIG_H__

#include "chip_config.h"
#include "flash_table.h"
#include "bt_em_config.h"
#include "wireless2.h"
#include "spi_flash.h"

//sync threshold @script_tcmcheck.bat
#define WIRELESS_TCM_SIZE   			(20 + DRV_REMAP_SIZE/1024)// 12
#define TCM_SRAM_START_ADDR_1			(BB_MPU_START_ADDR - WIRELESS_TCM_SIZE * 1024)
/*ADC、Dac、flash等资源不受wireless控制,部分功能宏暂不支持，不能开启,API要适配,保留代码段便于更新*/
#define	CFG_RESOURCE_DIS

/******************************************************************************
 * AudioSDK缺省配置启用2.4G应用步骤:
 * 1、开WIRELESS_SUPPORT@bt_config.h
 * 2、如需配对记忆,开WIRELESS_BP_EN@flash_table.h
 * 3、连接稳定,关BP_SAVE_TO_FLASH@app_config.h
 * 4、Tx/Rx与相应2.4G SDK turnkey设备混合使用
 * 2.4G T/Rx SDK角色宏控制,关 CFG_SYNC_TO_TX_SDK 等效于 CFG_SYNC_TO_RX_SDK
 * 设备操作说明, 参见本文件底部
 *****************************************************************************/
#define	CFG_SYNC_TO_TX_SDK

/****************************************************************************************
 *                 wireless Scheme 选择一项
 ****************************************************************************************/
//#define		WIRELESS_TURNKEY1_4			//1对1 双向，立体声, 44.1K采样率
//#define		WIRELESS_TURNKEY2_5			//2T1R 单向，单声道，44.1K采样率，音质优先
//#define		WIRELESS_TURNKEY2_6			//2T1R 单向，单声道，44.1K采样率,听感增强 可配连G1
//#define		WIRELESS_TURNKEY2_8			//2T1R 单向，单声道，44.1K采样率,音频高品质 可配连G1
//#define		WIRELESS_TURNKEY2_9			//2T1R 单向，单声道，44.1K采样率，射频增强，带内通讯
#define		WIRELESS_TURNKEY3_1			//1T多R 单向,双声道，44.1K采样率,2MCPS
//#define		WIRELESS_TURNKEY3_5			//1T多R 单向,双声道，44.1K采样率,带内通讯
//#define		WIRELESS_TURNKEY4_1			//1对2 双向，立体声, 44.1K采样率
//#define		WIRELESS_TURNKEY5_1			//1对1 单向，立体声, 44.1K采样率，

/**************TURNKEY CONFIG 禁止修改（除非标注可配），请咨询MV FAE*************************/
#if defined(WIRELESS_TURNKEY1_4)
	#define		TURNKEY_NAME					"1_4"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					2
	#define		RFPACK_REQUEST					3
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				41
	#else
		#define 	AUDIO_QUALITY				21
	#endif
	#define		PACKET_AUDIO_CH_BACKWARD		2
	#define 	CRC_PACKSUB						3
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define		MV_WIRELESS2_PARAM1				1
	#define 	WIRELESS_FUNCTION				wireless2_1_X_initfuncset
	#define		TURNKEY_TAG						1_x
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY2_5)
	#define		TURNKEY_NAME					"2_5"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					2
	#define 	AUDIO_QUALITY					20
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				7
	#define 	WIRELESS_FUNCTION				wireless2_2_X_initfuncset
	#define		TURNKEY_TAG						2_x
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY2_6)
	#define		TURNKEY_NAME					"2_6"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					2
	#define 	AUDIO_QUALITY					20 // 18
	#define 	CRC_PACKSUB						0
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				0
	#define		MV_WIRELESS2_PARAM2				8
	#define 	WIRELESS_FUNCTION				wireless2_2_X_initfuncset
	#define		TURNKEY_TAG						2_x
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY2_8)
	#define		TURNKEY_NAME					"2_8"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					2
	#define 	AUDIO_QUALITY					24
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				13
	#define 	WIRELESS_FUNCTION				wireless2_2_X_initfuncset
	#define		TURNKEY_TAG						2_x
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY2_9)
	#define		TURNKEY_NAME					"2_9"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					3
	#define 	AUDIO_QUALITY					20 // 18
	#define 	CRC_PACKSUB						4
	#define		BPLC_EN
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_C
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				14
	#define 	WIRELESS_FUNCTION				wireless2_2_X_initfuncset
	#define		TURNKEY_TAG						2_x
//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY3_1)
	#define		TURNKEY_NAME					"3_1"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					2
	#define		RFPACK_REQUEST					3
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				39
	#else
		#define 	AUDIO_QUALITY				20
	#endif
	#define 	CRC_PACKSUB						3
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				0
	#define		MV_WIRELESS2_PARAM2				5
	#define 	WIRELESS_FUNCTION				wireless2_3_X_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						3_x
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY3_3) //2.4G不退
	#define		TURNKEY_NAME					"3_3"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					1
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				47
	#else
		#define 	AUDIO_QUALITY				24
	#endif
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				11
	#define 	WIRELESS_FUNCTION				wireless2_3_3_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						3_3
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY3_5)// CFG_SYNC_TO_RX_SDK 连接异常
	#define		TURNKEY_NAME					"3_5"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					3
	#define		RFPACK_REQUEST					1//5
	#define		AUDIO_LOWFRAME
	#define		BPLC_EN
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				43
	#else
		#define 	AUDIO_QUALITY				22
	#endif
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_E
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				15
	#define 	WIRELESS_FUNCTION				wireless2_3_5_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						3_5
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY3_6)// CFG_SYNC_TO_RX_SDK 连接异常
	#define		TURNKEY_NAME					"3_6"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					3
	#define		RFPACK_REQUEST					1//8
	#define 	AUDIO_QUALITY					22
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_E
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				16
	#define 	WIRELESS_FUNCTION				wireless2_3_6_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						3_6
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY4_1)
	#define		TURNKEY_NAME					"4_1"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					2
	#ifdef CFG_SYNC_TO_TX_SDK
		#define		RFPACK_REQUEST				3//T3、R1
	#else
		#define		RFPACK_REQUEST				1
	#endif	
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				18
		#define 	AUDIO_QUALITY_BACKWARD		35
	#else
		#define 	AUDIO_QUALITY				18
	#endif
	#define		PACKET_AUDIO_CH_BACKWARD		2
	#define 	CRC_PACKSUB						3
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define		MV_WIRELESS2_PARAM1				1
	#define 	WIRELESS_FUNCTION				wireless2_4_X_initfuncset
	#define		TURNKEY_TAG						4_1
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY5_1)
	#define		TURNKEY_NAME					"5_1"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64//128
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					1//4
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				39
	#else
		#define 	AUDIO_QUALITY				20
	#endif
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define		MV_WIRELESS2_PARAM1				1
	#define 	WIRELESS_FUNCTION				wireless2_5_1_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						5_1
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY5_2) //2.4G不退
	#define		TURNKEY_NAME					"5_2"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						128
	#define		PACKET_AUDIO_CH					2
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					1//5
	#define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				39
	#else
		#define 	AUDIO_QUALITY				20
	#endif
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define		MV_WIRELESS2_PARAM1				1
	#define 	WIRELESS_FUNCTION				wireless2_5_2_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						5_2
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 1)//+ 0 for no effect
#elif defined(WIRELESS_TURNKEY6_1)
	#define		TURNKEY_NAME					"6_1"
	#define 	SAMPLE_RATE						44100
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					1
	#define		RFPACK_REQUEST					2
	#define 	AUDIO_QUALITY					19
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_A
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				12
	#define 	WIRELESS_FUNCTION				wireless2_6_1_initfuncset
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						6_1
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#elif defined(WIRELESS_TURNKEY8_1)
	#define		TURNKEY_NAME					"8_1"
	#define 	SAMPLE_RATE						16000
	#define		ONE_FRAME						64
	#define		PACKET_AUDIO_CH					1
	#define 	RFPACK_NAUDIO					3
	#define		RFPACK_REQUEST					1//5
	// #define		AUDIO_LOWFRAME
	#ifdef AUDIO_LOWFRAME
		#define 	AUDIO_QUALITY				43
	#else
		#define 	AUDIO_QUALITY				32
	#endif
	#define		PACKET_AUDIO_CH_BACKWARD		1
	#define 	CRC_PACKSUB						4
	#define		RFAUDIO_TRANS_LEN				RFAUDIO_TRANS_F
	#define 	MV_WIRELESS2_MODE2
	#define		MV_WIRELESS2_PARAM1				1
	#define		MV_WIRELESS2_PARAM2				15
	#define 	WIRELESS_FUNCTION				Wireless8x1InitFunSet
	#define		WIRELESS_TX_MASTER
	#define		TURNKEY_TAG						8_1
	//可调
	#define 	WIRELESS_RECV_FIFO_THRHLD		((RFPACK_NAUDIO + 1) + 4)//+0 for no effect
#endif

#define TURNKEY_2_X		(defined(WIRELESS_TURNKEY2_5)||defined(WIRELESS_TURNKEY2_6)||defined(WIRELESS_TURNKEY2_8)||\
						 defined(WIRELESS_TURNKEY2_9))

#define TURNKEY_3_X		(defined(WIRELESS_TURNKEY3_1)||defined(WIRELESS_TURNKEY3_3)||defined(WIRELESS_TURNKEY3_5)||\
						 defined(WIRELESS_TURNKEY3_6))

//#define WIRELESS2_CONNWITH_B5_VER2_34_1	//兼容(BP15 v2.34.1)及之前版本.
/****************************************************************************************
 *                	 功能参数配置
 ****************************************************************************************/
/*****无线配对码 初值，TX和RX初始值要一致才能正确配对，用户可修改****/
#define			WIRELESS_LINK_KEY0					(0x20)//配对的Key
#define			WIRELESS_LINK_KEY1					(0x56)//配对的Key
#if WIRELESS_BP_EN//LOCK Paired
	#if	defined(CFG_SYNC_TO_TX_SDK)
		#if (!defined(WIRELESS_TURNKEY3_3))
			#define   		CFG_LOCK_PAIRED_TXRX						//配对码记忆，保持于内部flash，如果使能该宏，初次配对之后会记忆UUID，否则一直使用初值
			#if	defined(CFG_LOCK_PAIRED_TXRX) && (defined(WIRELESS_TURNKEY1_4))
			/*0:1个TX可以在多个已经配对过的RX中切换连接，一次只能有一个TX和RX连接
			* 1:1个RX可以在多个已经配对过的TX中切换连接，一次只能有一个TX和RX连接
			* 2:1个TX只能和1个已经配对过的RX连接，TX和RX进入重新配对，记忆信息清除*/
				#define CFG_LOCK_PAIRED_MODE		0//0//1//2
			#endif
			#if defined(CFG_LOCK_PAIRED_TXRX) && (defined(WIRELESS_TURNKEY8_1))
				 #define CFG_PAIRING_SUPPORTMDOE	defined(WIRELESS_TURNKEY8_1)
			#endif
		#endif
		#if ( defined(CFG_LOCK_PAIRED_TXRX) && \
			  ( TURNKEY_3_X	))
			#define CFG_TX_AUTO_SEND_PAIRING_KEY//TX配对记忆模式下，上电自动广播配对码3s，3s之后结束广播，默认关闭
			#define CFG_PAIRING_SUPPORTMDOE		(defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_5) ||\
												 defined(WIRELESS_TURNKEY3_6) )
		#endif
		#if !defined(CFG_LOCK_PAIRED_TXRX) &&  \
			TURNKEY_2_X
//			#define   		CFG_LOCK_PAIRED_TX_ONLY				//配对仅仅记忆TX，注意目前该功能仅仅针对2T1R
//			#define			CFG_GET_RX_RSSI						//获取周围RX设备的RSSI值

			/*TX端需要打开宏CFG_LOCK_PAIRED_TX_ONLY，RX需要关闭宏CFG_LOCK_PAIRED_TX_ONLY*/
//			#define   		CFG_LOCK_PAIRED_RX_ALLOWED				//配对需要通过RX来手动操作，注意目前该功能仅仅针对2-6。
		#endif
	#else //SYNC_TO_RX_SDK
		#if (!defined(WIRELESS_TURNKEY3_3))
			#define   	CFG_LOCK_PAIRED_TXRX						//配对码记忆，保持于内部flash，如果使能该宏，初次配对之后会记忆UUID，否则一直使用初值

			#if	TURNKEY_3_X
				#define CFG_PAIRING_SUPPORTMDOE		(defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_5) ||\
													 defined(WIRELESS_TURNKEY3_6))
			#endif
			#if defined(CFG_LOCK_PAIRED_TXRX) && (defined(WIRELESS_TURNKEY8_1))
				 #define CFG_PAIRING_SUPPORTMDOE	defined(WIRELESS_TURNKEY8_1)
			#endif
			#if	defined(CFG_LOCK_PAIRED_TXRX) && (defined(WIRELESS_TURNKEY1_4))
			/*0:1个TX可以在多个已经配对过的RX中切换连接，一次只能有一个TX和RX连接
			* 1:1个RX可以在多个已经配对过的TX中切换连接，一次只能有一个TX和RX连接
			* 2:1个TX只能和1个已经配对过的RX连接，TX和RX进入重新配对，记忆信息清除*/
				#define CFG_LOCK_PAIRED_MODE		0//0//1//2
			#endif
		#endif
		#if !defined(CFG_LOCK_PAIRED_TXRX) && \
			TURNKEY_2_X
//			#define   		CFG_LOCK_PAIRED_TX_ONLY				//配对仅仅记忆TX，注意目前该功能仅仅针对2T1R
			/**TX端需要打开宏CFG_LOCK_PAIRED_TX_ONLY，RX需要关闭宏CFG_LOCK_PAIRED_TX_ONLY**/
			#define   		CFG_LOCK_PAIRED_RX_ALLOWED				//配对需要通过RX来手动操作，注意目前该功能仅仅针对2-6。
			#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
//				#define   	CFG_LOCK_PAIRED_RX_EN	//RX有记忆功能，自动回连配对过的设备，该功能和CFG_LOCK_PAIRED_TXRX 不同,记忆的是TX chipID
//				#define   	CFG_LOCK_PAIRED_RX_SAVE_NVM	//保存flash中会关中断，已经连接的设备会断了再重新连，介意这个需要保存到NVM，通过命令再写flash
			#endif
		#endif
	#endif //#if	defined(CFG_SYNC_TO_TX_SDK)
#endif //LOCK Paired
//#define     KEY_REMOTE						    //tx端控制 blue ns等状态  aplly位于rx，通过info传输。
#define		TCM_EN
#define     PACKET_CRC_LEN				    (2)
#define     PACKET_CNT_LEN				    (2)
#if RFPACK_NAUDIO > 1
	#define CRC_MULTIPLE //RF多音频包 CRC重组
#endif
/***
 * CRC_PACKSUB:Wireless tx ~ AudioPacket CRC Buf Offset
 */
/****以上配置,可调参数除外，TX/RX SDK需完全镜像, TX/RX收发功能映射由下文关联***/

/********此宏无效,AudioSDK由BT HCI管理
//#define		CFG_OPENDUT_MODE
****************************************/

/**FCC测试模式，默认关闭 通过MSG_WIRELESS_FCC进入模式，进入前会复位重启 **/
//#define CFG_FCC_MODE

/*************此宏无效AudioSDK：CHIP_USE_DCDC接管
#ifdef 	CHIP_USE_DCDC
	#define 	CFG_DCDC_EN //芯片内置DCDC使能,关宏提射频性能
#endif
******************************************/

#ifdef CFG_SYNC_TO_TX_SDK
	/****************************************************************************************
	 *                	 TX SDK Turnkey映射
	 ****************************************************************************************/
	#define	ENCODE_CH			PACKET_AUDIO_CH			//foreward:wireless_tx->wireless_rx
	#define ENCODE_QUALITY		AUDIO_QUALITY
	//发包启动,系统/采集/编码节拍瞬时波动过大时，可加此水位->加Tx延时。
	#define		WIRELESS_TRANS_THRHLD			2
	#ifdef PACKET_AUDIO_CH_BACKWARD
		#define DECODE_CH			PACKET_AUDIO_CH_BACKWARD//backware:wireless_rx->wireless_tx
		#ifdef AUDIO_QUALITY_BACKWARD
			#define DECODE_QUALITY		AUDIO_QUALITY_BACKWARD
		#else
			#define DECODE_QUALITY		AUDIO_QUALITY
		#endif
	#else
		#define DECODE_CH		0
		#undef	WIRELESS_RECV_FIFO_THRHLD
	//	#define DECODE_QUALITY		AUDIO_QUALITY
	#endif
	#ifdef WIRELESS_TX_MASTER
		#define	WIRELESS_SDK_ROLE		MVWIRE2_MASTER_ROLE
		#define ROLE_TAG				master
	#else
		#define	WIRELESS_SDK_ROLE		MVWIRE2_SLAVER_ROLE
		#define ROLE_TAG				slaver
	#endif
	#if	(defined(WIRELESS_TURNKEY2_6) || defined(WIRELESS_TURNKEY2_8))
//		#define		WIRELESS_CONN_G1    //连G1或兼容G1的B5，注意 配对码一致
//		#define 	WIRELESS2WITHBTCHIP	 //WIRELESS增强模式，抗干扰能力加强，G1需要同步打开WIRELESS2WITHBTCHIP宏

	#endif
#else
	/****************************************************************************************
	 *                	 RX SDK映射
	 ****************************************************************************************/
	#define	DECODE_CH			PACKET_AUDIO_CH			//foreward:wireless_tx->wireless_rx
	#define DECODE_QUALITY		AUDIO_QUALITY
	#ifdef PACKET_AUDIO_CH_BACKWARD
		#define ENCODE_CH			PACKET_AUDIO_CH_BACKWARD//backware:wireless_rx->wireless_tx
		#ifdef AUDIO_QUALITY_BACKWARD
			#define ENCODE_QUALITY		AUDIO_QUALITY_BACKWARD
		#else
			#define ENCODE_QUALITY		AUDIO_QUALITY
		#endif
		//发包启动,系统/采集/编码节拍瞬时波动过大时，可加此水位->加Tx延时。
		#define		WIRELESS_TRANS_THRHLD			2//1
	#else
		#define ENCODE_CH		0
	//	#define ENCODE_QUALITY		AUDIO_QUALITY
	#endif

	#ifdef WIRELESS_TX_MASTER
		#define	WIRELESS_SDK_ROLE		MVWIRE2_SLAVER_ROLE
		#define ROLE_TAG				slaver
	#else
		#define	WIRELESS_SDK_ROLE		MVWIRE2_MASTER_ROLE
		#define ROLE_TAG				master
	#endif

	#if defined(WIRELESS_TURNKEY2_6) || defined(WIRELESS_TURNKEY2_8)
//		#define		WIRELESS_CONN_G1    //连接Device芯片改为G1，注意B5 和G1 配对码需要一致
//		#define 	WIRELESS2WITHBTCHIP	 //WIRELESS增强模式，抗干扰能力加强，G1需要同步打开WIRELESS2WITHBTCHIP宏
	#endif
#endif

/****************************************************************************************
 *                 测试配置
 ****************************************************************************************/
//#define		AUDIO_SINE_TEST_EN //使用正弦波数据替换 Wireless发送 适用于传输连贯性 录音测试

#if DECODE_CH != 0
	#define LOG_ASSOCIATION_EN
#endif
// #define DEBUG_WIRELESS_EN				//2.4G状态统计Log

/****************************************************************************************
 *                 晶体频率校准功能
 * 				   晶体频率使用Nvm  3 4位，使用Nvm 时请注意
 *				   重启生效
 ****************************************************************************************/
//#define	CFG_SOC_CALIBRATION_EN
#ifdef CFG_SOC_CALIBRATION_EN
	#include "soc_calibration.h"
	#define CFG_SOC_CALIBRATION_MODE			CFG_SOC_CALIBRATION_AUTO //模式选择 默认自动触发校准
	#define CFG_SOC_CALIBRATION_ACCURATE		CFG_SOC_ACCURATE_NORMAL//校准精度选择 默认普通校准精度
	#define CFG_SOC_CALIBRATION_LENGTH			CFG_SOC_LENGTH_HIGH//校准范围选择 默认有距离限制为2m左右

	#if CFG_SOC_CALIBRATION_MODE == CFG_SOC_CALIBRATION_ADC_KEY
	    #define CFG_SOC_CALIBRATION_MODE1_EN
	#elif CFG_SOC_CALIBRATION_MODE == CFG_SOC_CALIBRATION_AUTO
	    #define CFG_SOC_CALIBRATION_MODE2_EN
	#endif
	/*配置后校准后可以继续校准或者用读频偏，如果校准后值为0xf或者趋近0xf则电容太小会存着风险*/
//	#define	CFG_SOC_CALIBRATION_READ_EN
	#if defined CFG_SOC_CALIBRATION_READ_EN && (defined(CFG_SOC_CALIBRATION_MODE2_EN)||defined(CFG_SOC_CALIBRATION_MODE1_EN))
		#define CFG_SOC_CALIBRATION_READ
	#endif
#endif

/****************************************************************************************
 *                 反向多字节命令字
 *                 2-x RxDev -> TxDev：1~8Bytes
 *                 20~300mS @ Turnkey、Link、Len、Signal
 ****************************************************************************************/
#if	defined(WIRELESS_TURNKEY2_6)||defined(WIRELESS_TURNKEY2_5)
//	#define		CFG_2T1R_SEND_MULTIBYTE_CMD_EN
#endif

/****************************************************************************************
 *                 外置PALNA提升距离
 *                 PA复用于A0，LNA复用于A1
 *                 谨慎使用,天线/增益需匹配,IO不可浮空
 *                 BT射频PA要bypass，以免增益超标
 ****************************************************************************************/
//#define		WIRELESS_EXTERNAL_PALNA_EN


/****************************************************************************************
 *                 PAIR RSSI HOLD
 *                 配置Rx限制Tx设备配对距离，只有RSSI在规定范围内才会进行配对
 ****************************************************************************************/
 #if !defined(CFG_SYNC_TO_TX_SDK)
//	#define		CFG_WIRELESS_PAIR_RSSi_HOLD
	#ifdef  CFG_WIRELESS_PAIR_RSSi_HOLD
		#define WIRELESS_PAIR_RSSi_VALUE			(60)
	#endif
#endif

#ifndef CFG_RESOURCE_DIS
/****************************************************************************************
 *                 支持BLE OTA升级功能
 *                 注意：需要解决flashboot目录下的flash_boot_ota.rar以替换原flash_boot.c
 ****************************************************************************************/
//#define		CFG_FUNC_BLE_OTA_EN
/****************************************************************************************
 *                 支持2.4G OTA升级功能(双bank升级，注意flash容量大小)
 *                 注意：
 *                 		1）所有B5芯片可以支持该功能
 *                 		2）2-6模式下支持TX给RX无线升级code
 *                 		3）可以双向升级，注意宏的方向控制
 *                 		4）通过USB端口下载MVA包文件到master设备上，再触发ota升级流程
 ****************************************************************************************/
//#define		CFG_FUNC_2G4_OTA_EN
#ifdef	CFG_FUNC_2G4_OTA_EN
	#define CFG_FUNC_2G4_OTA_TX_MASTER//TX给RX升级代码
//	#define CFG_FUNC_2G4_OTA_RX_MASTER//RX给TX升级代码
	#define	CFG_FUNC_MVA_UPGRADE_BY_USBHID//通过USB接口将MVA升级包存放到flash中
#endif

/****************************************************************************************
 *                 3-1模式，配对记忆方式，配对码通过红外通讯收发
 *                 默认关闭，注意TX和RX都要打开对应的宏
 ****************************************************************************************/
#if defined(CFG_SYNC_TO_TX_SDK) &&  CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX) 
	#ifdef CFG_FUNC_IRSEND_EN
		//#define CFG_FUNC_IR_SEND_PAIRING_KEY
		#ifdef CFG_FUNC_IR_SEND_PAIRING_KEY
			#define CFG_FUNC_PAIRED_IR_HIGH_PRIORITY//红外配对优先
		#endif
		//#define CFG_FUNC_AUTO_PAIRING_EN
	#endif
#elif defined(WIRELESS_TURNKEY3_1) && defined(CFG_LOCK_PAIRED_TXRX)
	#ifdef CFG_RES_IR_KEY_EN
		//#define CFG_FUNC_IR_REC_PAIRING_KEY
		#ifdef CFG_FUNC_IR_REC_PAIRING_KEY
			#define CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
		#endif
	#endif
#endif
#endif //CFG_RESOURCE_DIS
/****************************************************************************************
 *                 配对记忆保存flash地址
 ****************************************************************************************/
#define CONINF_FLASH_ADDR					(WIRELEDD_CONFIG_ADDR)
#define CONINF_CAP_FLASH_ADDR				(WIRELESS_CAP_ADDR)//0x1FC000//待调整(SOC_CAP_CAL_ADDR)

#ifndef CFG_SYNC_TO_TX_SDK
	typedef struct Rx_Flash_param
	{
	unsigned int PairedDev1Info;
	unsigned int PairedDev2Info;
	unsigned char PairedDev1Type;//5-1需要有一个字节用于表示当前连接的芯片类型
//	unsigned char PairedDev2Type;//未使用
	}Rx_Flash_param_t;
#else
	typedef struct Tx_Flash_param
	{
		unsigned int PairedDevInfo;
		unsigned char PairedDev1Type;//5-1需要有一个字节用于表示当前连接的芯片类型
	//	unsigned char PairedDev2Type;//未使用
	}Tx_Flash_param_t;
#endif

/****************************************************************************************
 *                 组合宏编译警告
 ****************************************************************************************/

#if (RFPACK_NAUDIO==1) && defined(CRC_MULTIPLE)
	#error	"(CRC Config err)"
#endif

#if (defined(WIRELESS_TURNKEY3_3)) && defined(CFG_LOCK_PAIRED_TXRX)
	#warning	"CFG_LOCK_PAIRED_TXRX not support!!!"
	#undef		CFG_LOCK_PAIRED_TXRX
#endif

#if defined(CFG_LOCK_PAIRED_TX_ONLY) && (defined(wireless2_1_X_initfuncset) || defined(wireless2_3_X_initfuncset))
	#error	"CFG_LOCK_PAIRED_TX_ONLY not support!!!"
#endif
#if BLE_SUPPORT || BT_SOURCE_SUPPORT
	#error	"WIRELESS_SUPPORT Error!!!"
#endif
#endif /* __WIRELESS_CONFIG_H__ */


/******************************************************************************
 * 源码构造与版本：
 * 1、 AudioSDK是主体软件框架,版本说明参见BT_Audio_APP_history.txt
 * 2、预制版为临时分支，修缮时注意小版本号映射
 * 3、wireless功能作为被移植模块，版本说明见2.4GSDK发布release_notes.txt
 * 4、修缮时，源码参见下文;
 * 5、增量发布需列举修缮点
 *
 * 2.4G + AudioSDK操作:
 * 2.4G SDK同步版本(下文标注)
 * 操作说明：
 * 硬件：DEVELOPMENT开发板，K34键mode循环,可进退wirelessinplay，(单向Tx-上电常驻，带CFG_RF_TO_BT时K5键短按退出wireless，进测试模式)。
 * 软件：Turnkey、配对KEY和记忆宏保持一致.
 * Tx：MV_wireless_mic_SDK\wireless_mic_tx_sdk 或Audio SDK 配置CFG_SYNC_TO_TX_SDK
 * Rx：MV_wireless_mic_SDK\wireless_mic_rx_sdk 或Audio SDK 关闭宏CFG_SYNC_TO_TX_SDK

 * 简要参考：
 * 2-6关音效CFG_FUNC_AUDIO_EFFECT_EN为例， TxLinein-> RxDac--ADDA缺省配置延时测定：
 * Audio SDK + 2.4G Tx    ->    Audio SDK + 2.4G Rx      延时29mS
 * wireless_mic_tx_sdk    ->     Audio SDK + 2.4G Rx     延时28mS
 * Audio SDK + 2.4G Tx    ->     wireless_mic_rx_sdk     延时18mS
 * wireless_mic_tx_sdk    ->     wireless_mic_rx_sdk     延时16mS
 * 表明：Audio SDK的 Dac播放水位是延时扩大的主要原因。
 *
 * Turnkey说明：
 * 1、turnkey3-1 配对需Tx按键操作(K10/K9)或关闭记忆宏(WIRELESS_BP_EN)。
 * 2、重启：部分turnkey配对后需手动重启生效、DUT模式/频偏校准等需重启
 * 3、相比缺省2.4G SDK，Adc和Dac不受控制，暂无相位对齐措施。
 *
 * AudioSDK说明：
 * 1、AudioSDK缺省关闭 2.4G应用(带Wireless为预配置版：已开WIRELESS_SUPPORT)。
 * 2、单向Turnkey tx端仅工程测试状态支持蓝牙，因蓝牙模式退出潜在异常; 上电可直接开启2.4G Tx;为支持BT下的射频测试，启用CFG_RF_TO_BT时,支持Msg退出2.4G,切模式进Btplay,切回2.4G时系统重置。
 * 3、2.4G中断密集，时序要求高，写flash会触发断联回连，通常AudioSDK宏BP_SAVE_TO_FLASH先关闭，相关功能需求建议：断连、退出2.4G或主动关机前写入flash，依场景实现。
 * 4、1-4或4-1是双向传输,缺省发送Mic音频;单向方案tx发送dac相同音频。
 * 5、 wireless slaver跟随master，锁定传输CLK，MCLK使用指定分频值,锁定音频与传输，2.4G通信禁止启动硬件微调;
 * 6、关CFG_AUDIO_WIDTH_24BIT&&CFG_FUNC_AUDIO_EFFECT_EN/MIC_ALT2_EN/SPEAKER_ALT2_EN, 保障MCPS，再逐步按需开启。
 * 7、BtPlay进退时Bt Stack必需运行(依赖),消息MSG_MODE,驱动进wirelessinPlay，再发MSG_WIRELESS关BtStack开WirlessStack。
 *    反之MSG_WIRELESS_EXIT(MSG_MODE令wirelessinPlay退出/单向tx按键触发)驱动关WirelessStack开BtStack，此时Btplay可用(MSG_MODE)。
 *
 * FAQ:
 * 1、AudioSDk 2.4G反复连接、断开：BP_SAVE_TO_FLASH宏未关，(切音效、播卡播U、radio换台等事件间歇写flash记忆)
 * 2、AudioSDk 无法启动蓝牙模式：配置了单向turnkey，CFG_SYNC_TO_TX_SDK，未配置CFG_RF_TO_BT。
 * 3、wirelessRx反复LD/LE:开音效宏后，系统帧长会变，需确保启用的方案 WIRELESS_RECV_FIFO_THRHLD > AudioEffect.audioeffect_frame_size / ONE_FRAME。
 * 4、关闭CFG_AUDIO_WIDTH_24BIT后无声，音效图接口是BITS_24,改BITS_16导出或关音效宏。
 *
 *
 * 251222同步代码：
 * 1、RF库libBtStack_wireless_noble.a更新至V15_1.6.0 / 6154
 * 2、audio_association.a同步至V1.3.2 / 5803
 * 3、wireless2.a :V1.3.89 / 6092
 * 4、2.4G应用层:V2.35.5(不含)之后 / 5803

 * MVsB5_BT_Audio_MVWire6092_SDK_V1.3.0发布
 * 1、2G4距离变近问题。
 * ***************************************************************************/
