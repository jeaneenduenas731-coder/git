/*
 * log_info.h
 *
 *  Created on: Aug 20, 2025
 *      Author: piwang
 */

#ifndef AUDIO_LOG_INFO_H_
#define AUDIO_LOG_INFO_H_

#include "app_config.h"
/***********************************************
 * 应2.4G中断内事项Log分析需要,启用Log事务模块,宏管理特定测试项.
 * 本文件@ app_config.h管理表项
 ***************************************************/
#if DECODE_CH != 0
	//#define	LOG_LOST_TRH 		10			//连续丢多包(half) 包号跟踪数量,
	//#define	LOG_RECV_DELAY					//收包延时稳定性跟踪

	//#define 	DEBUG_PACKET_ID_EN				//音频收包 解包 丢包监测接口,禁止中断API打印
	#define LOG_RECV_SYNC_RST_EN				//打印Rx重新同步
#endif
//not Porting #define DEBUG_LOG_AUDIO_MCPS_EN				//打印Audio MCPS Max


/**************Log项 枚举*****************
 * 定义枚举符,API内Set,LogEvent映射Case
 * Lib事件--需应用转译
 ***************************/
typedef enum _EVENT_LOG_FLAG
{
#if defined(LOG_LOST_TRH) && DECODE_CH != 0
	ELOG_LOST_SEVERAL,
#endif
	ELOG_NUM,//Max = 32
} EVENT_LOG_FLAG;

extern uint32_t LogEventWord;
#define SET_EVENT_FLAG(FLAG)			LogEventWord |= BIT(FLAG)
#define CLR_EVENT_FLAG(FLAG)			LogEventWord &= ~BIT((FLAG)
#define IS_EVENT_FLAG(FLAG)				(LogEventWord & BIT(FLAG))

typedef enum _INFO_LOG_FLAG
{
#if ENCODE_CH != 0
	ILOG_SEND_EMPTY,
#endif
	ILOG_NUM,//Max = 32
} INFO_LOG_FLAG;

extern uint32_t LogInfoWord;
#define SET_INFO_FLAG(FLAG)				LogInfoWord |= BIT(FLAG)
#define CLR_INFO_FLAG(FLAG)				LogInfoWord &= ~BIT((FLAG)
#define IS_INFO_FLAG(FLAG)				(LogInfoWord & BIT(FLAG))

void LogTimerInit(void);

void LogEvent(void);

#endif /* AUDIO_LOG_INFO_H_ */
