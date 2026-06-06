/**
 *****************************************************************************
 * @file     otg_detect.h
 * @author   Owen
 * @version  V1.0.0
 * @date     27-03-2017
 * @brief    otg port detect module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2015 MVSilicon </center></h2>
 */

/**
 * @addtogroup OTG
 * @{
 * @defgroup otg_detect otg_detect.h
 * @{
 */
#ifndef __OTG_DETECT_H__
#define	__OTG_DETECT_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 


#include "type.h"

/**
 * @brief  Port连接检测函数
 * @param  NONE
 * @return NONE
 * 重要提示：
 * 目标设备必须固定角色
 * 两方都检测Host/Device时，无法识别
 */
void OTG_PortLinkCheck(void);

/**
 * @brief  检查Port端口上是否有一个U盘设备连接
 * @param  NONE
 * @return 1-有U盘设备连接，0-无U盘设备连接
 */
bool OTG_PortHostIsLink(void);

/**
 * @brief  检查Port端口上是否有一个PC连接
 * @param  NONE
 * @return 1-有PC连接，0-无PC连接
 */
bool OTG_PortDeviceIsLink(void);


/**
 * @brief  使能port端口检查U盘连接
 * @param  HostEnable: 是否检测U盘连接
 * @param  DeviceEnable: 是否检测PC连接
 * @return NONE
 */
void OTG_PortSetDetectMode(bool HostEnable, bool DeviceEnable);

/**
 * @brief  使能port端口检查暂停
 * @param  PauseEnable: 是否暂停检测
 * @return NONE
 */
void OTG_PortSetDetectPause(bool PauseEnable);

/**
 * @brief  usb快速检测设备插入
 * @param  DetectQuickEnable: 是否使能快速检测(默认使能)
 * @return NONE
 */
void OTG_PortSetDetectQuick(bool DetectQuickEnable);
#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__OTG_DETECT_H__

/**
 * @}
 * @}
 */
