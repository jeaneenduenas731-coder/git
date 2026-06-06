/**
 *******************************************************************************
 * @file    powercontroller.h
 * @brief	powercontroller module driver interface

 * @author  Sean
 * @version V1.0.0

 * $Created: 2017-11-13 16:51:05$
 * @Copyright (C) 2017, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *******************************************************************************
 */

/**
 * @addtogroup POWERCONTROLLER
 * @{
 * @defgroup powercontroller powercontroller.h
 * @{
 */
 
#ifndef __POWERCONTROLLER_H__
#define __POWERCONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

typedef enum __PWR_SYSWAKEUP_SOURCE_SEL
{
	SYSWAKEUP_SOURCE_NONE = 0,
	SYSWAKEUP_SOURCE0_GPIO = (1 << 0),     /**<system wakeup source0 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE1_GPIO = (1 << 1),     /**<system wakeup source1 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE2_GPIO = (1 << 2),     /**<system wakeup source2 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE3_GPIO = (1 << 3),     /**<system wakeup source3 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE4_GPIO = (1 << 4),     /**<system wakeup source4 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE5_GPIO = (1 << 5),     /**<system wakeup source5 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE6_POWERKEY = (1 << 6), /**<system wakeup source6 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE7_CHARGEON = (1 << 7), /**<system wakeup source7 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE8_LVD = (1 << 8),      /**<system wakeup source8 macro,一般不用,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE9_UARTRX = (1 << 9),   /**<system wakeup source9 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE10_RTC = (1 << 10),    /**<system wakeup source10 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE11_IR = (1 << 11),     /**<system wakeup source11 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE12_TIMER5 = (1 << 12), /**<system wakeup source12 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE13_UART1 = (1 << 13),  /**<system wakeup source13 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE14_CAN = (1 << 14),    /**<system wakeup source14 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE15_BT = (1 << 15),     /**<system wakeup source15 macro,should be waked up by rise edge*/

} PWR_SYSWAKEUP_SOURCE_SEL;

typedef enum __PWR_BTWAKEUP_SOURCE_SEL{
	BTWAKEUP_SOURCE0_GPIO = (1 << 0),				/**<system wakeup source0 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE1_GPIO = (1 << 1),				/**<system wakeup source1 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE2_GPIO = (1 << 2),				/**<system wakeup source2 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE3_GPIO = (1 << 3),				/**<system wakeup source3 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE4_GPIO = (1 << 4),				/**<system wakeup source4 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE5_GPIO = (1 << 5),				/**<system wakeup source5 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE6_POWERKEY = (1 << 6),				/**<system wakeup source6 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE7_CHARGE = (1 << 7),				/**<system wakeup source7 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE8_LVD = (1 << 8),				/**<system wakeup source8 macro,一般不用,should be waked up by rise edge*/
	BTWAKEUP_SOURCE9_UART0_RX = (1 << 9),				    /**<system wakeup source9 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE10_RTC_INT = (1 << 10),        /**<system wakeup source10 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE11_IR_CMD = (1 << 11),          /**<system wakeup source11 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE12_TIMER5_INT = (1 << 12),          /**<system wakeup source12 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE13_UART1_RX = (1 << 13),          /**<system wakeup source13 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE14_CAN_IRQ = (1 << 14),          /**<system wakeup source14 macro,should be waked up by rise edge*/

}PWR_BTWAKEUP_SOURCE_SEL;

typedef enum __PWR_WAKEUP_GPIO_SEL
{
	WAKEUP_GPIOA0 = 0, /**<wakeup by GPIOA0 macro*/
	WAKEUP_GPIOA1,     /**<wakeup by GPIOA1 macro*/
	WAKEUP_GPIOA2,     /**<wakeup by GPIOA2 macro*/
	WAKEUP_GPIOA3,     /**<wakeup by GPIOA3 macro*/
	WAKEUP_GPIOA4,     /**<wakeup by GPIOA4 macro*/
	WAKEUP_GPIOA5,     /**<wakeup by GPIOA5 macro*/
	WAKEUP_GPIOA6,     /**<wakeup by GPIOA6 macro*/
	WAKEUP_GPIOA7,     /**<wakeup by GPIOA7 macro*/
	WAKEUP_GPIOA8,     /**<wakeup by GPIOA8 macro*/
	WAKEUP_GPIOA9,     /**<wakeup by GPIOA9 macro*/
	WAKEUP_GPIOA10,    /**<wakeup by GPIOA10 macro*/
	WAKEUP_GPIOA11,    /**<wakeup by GPIOA11 macro*/
	WAKEUP_GPIOA12,    /**<wakeup by GPIOA12 macro*/
	WAKEUP_GPIOA13,    /**<wakeup by GPIOA13 macro*/
	WAKEUP_GPIOA14,    /**<wakeup by GPIOA14 macro*/
	WAKEUP_GPIOA15,    /**<wakeup by GPIOA15 macro*/
	WAKEUP_GPIOA16,    /**<wakeup by GPIOA16 macro*/
	WAKEUP_GPIOA17,    /**<wakeup by GPIOA17 macro*/
	WAKEUP_GPIOA18,    /**<wakeup by GPIOA18 macro*/
	WAKEUP_GPIOA19,    /**<wakeup by GPIOA19 macro*/
	WAKEUP_GPIOA20,    /**<wakeup by GPIOA20 macro*/
	WAKEUP_GPIOA21,    /**<wakeup by GPIOA21 macro*/
	WAKEUP_GPIOA22,    /**<wakeup by GPIOA22 macro*/
	WAKEUP_GPIOA23,    /**<wakeup by GPIOA23 macro*/
	WAKEUP_GPIOA24,    /**<wakeup by GPIOA24 macro*/
	WAKEUP_GPIOA25,    /**<wakeup by GPIOA25 macro*/
	WAKEUP_GPIOA26,    /**<wakeup by GPIOA26 macro*/
	WAKEUP_GPIOA27,    /**<wakeup by GPIOA27 macro*/
	WAKEUP_GPIOA28,    /**<wakeup by GPIOA28 macro*/
	WAKEUP_GPIOA29,    /**<wakeup by GPIOA29 macro*/
	WAKEUP_GPIOA30,    /**<wakeup by GPIOA30 macro*/
	WAKEUP_GPIOA31,    /**<wakeup by GPIOA31 macro*/
	WAKEUP_GPIOB0,     /**<wakeup by GPIOB0 macro*/
	WAKEUP_GPIOB1,     /**<wakeup by GPIOB1 macro*/
	WAKEUP_GPIOB2,     /**<wakeup by GPIOB2 macro*/
	WAKEUP_GPIOB3,     /**<wakeup by GPIOB3 macro*/
	WAKEUP_GPIOB4,     /**<wakeup by GPIOB4 macro*/
	WAKEUP_GPIOB5,     /**<wakeup by GPIOB5 macro*/
	WAKEUP_GPIOB6,     /**<wakeup by GPIOB6 macro*/
	WAKEUP_GPIOB7,     /**<wakeup by GPIOB7 macro*/
	WAKEUP_GPIOB8,     /**<wakeup by GPIOB8 macro*/
	WAKEUP_GPIOB9,     /**<wakeup by GPIOB9 macro*/
	WAKEUP_GPIOC0      /**<wakeup by GPIOC0 macro*/

} PWR_WAKEUP_GPIO_SEL;

typedef enum __PWR_SYSWAKEUP_SOURCE_EDGE_SEL
{
	SYSWAKEUP_SOURCE_NEGE_TRIG = 0,  /**negedge trigger*/
	SYSWAKEUP_SOURCE_POSE_TRIG,      /**posedge trigger*/
	SYSWAKEUP_SOURCE_BOTH_EDGES_TRIG /**both edges trigger*/
									 /**只在SourSel为[0~6]才有上升沿或者下降沿触发之分，其他SourSel都是默认上升沿触发*/

} PWR_SYSWAKEUP_SOURCE_EDGE_SEL;

typedef enum __PWR_LVD_Threshold_SEL
{
	PWR_LVD_Threshold_2V3 = 0, /**LVD threshold select:2.3V */
	PWR_LVD_Threshold_2V4,     /**LVD threshold select:2.4V*/
	PWR_LVD_Threshold_2V5,     /**LVD threshold select:2.5V*/
	PWR_LVD_Threshold_2V6,     /**LVD threshold select:2.6V*/
	PWR_LVD_Threshold_2V7,     /**LVD threshold select:2.7V*/
	PWR_LVD_Threshold_2V8,     /**LVD threshold select:2.8V*/
	PWR_LVD_Threshold_2V9,     /**LVD threshold select:2.9V*/
	PWR_LVD_Threshold_3V0,     /**LVD threshold select:3.0V*/

} PWR_LVD_Threshold_SEL;

typedef enum _PWR_LDO11_LVL_SEL
{
    PWD_LDO11_LVL_0V95 = 0, // 0.95V
    PWD_LDO11_LVL_1V0,      // 1.0V
    PWD_LDO11_LVL_1V05,     // 1.05V
    PWD_LDO11_LVL_1V10,     // 1.10V
    PWD_LDO11_LVL_1V15,     // 1.15V
    PWD_LDO11_LVL_1V18,     // 1.18V
    PWD_LDO11_LVL_1V20,     // 1.20V
    PWD_LDO11_LVL_1V22,     // 1.22V
    PWD_LDO11_LVL_1V25,     // 1.25V
    PWD_LDO11_LVL_MAX       // 请不要用这个值配置！！！仅用来做有效性判断，实际配置请用PWD_LDO11_LVL_0V95~PWD_LDO11_LVL_1V25
} PWR_LDO11_LVL_SEL;

/**
 * @brief  Config LDO11D voltage
 * @param  uint8_t value: 0-0.95V 1-1.00V 2-1.05V 3-1.10V 4-1.15V 5-1.18V 6-1.20V, 7-1.22V, 8-1.25V >8:1.25V
 * @return void
 * @note   none
 */
void Power_LDO11DConfig(PWR_LDO11_LVL_SEL level);

/**
 * @brief  系统进入sleep模式
 * @param  无
 * @return 无
 */
void Power_GotoSleep(void);

/**
 * @brief  系统进入deepsleep模式
 * @param  无
 * @return 无
 */
void Power_GotoDeepSleep(void);

/**
 * @brief  配置DeepSleep唤醒源
 * @param  SourSel 配置唤醒源,分别为：GPIO，PowerKey,RTC,LVD(一般不用),IR,UART,CAN,BT为蓝牙事件唤醒
 * @param  Gpio   配置gpio唤醒引脚，[0~42],分别对应GPIOA[31:0]、GPIOB[9:0]、GPIOC[0]
 * @param  Edge   gpio的边沿触发方式选择：1上升沿0下降沿
 * @note   参数gpio，    只在SourSel为GPIO才有效
 *         参数edge，    只在SourSel为GPIO和PowerKey才有上升沿或者下降沿触发之分，其他SourSel都是默认上升沿触发
 * @return 无
 */
void Power_WakeupSourceSet(PWR_SYSWAKEUP_SOURCE_SEL SourSel, PWR_WAKEUP_GPIO_SEL Gpio, PWR_SYSWAKEUP_SOURCE_EDGE_SEL Edge);

/**
 * @brief  设置某个通道唤醒使能
 * @param  SourSel 唤醒源:GPIO，PowerKey,RTC,LVD(一般不用),IR,UART,CAN,BT为蓝牙事件唤醒
 * @return 无
 */
void Power_WakeupEnable(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  设置某个通道唤醒禁止
 * @param  SourSel 唤醒源:GPIO，PowerKey,RTC,LVD(一般不用),IR,UART,CAN,BT为蓝牙事件唤醒
 * @return 无
 */
void Power_WakeupDisable(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  获取唤醒通道标志
 * @param  无
 * @return 获取唤醒通道标志
 */
uint32_t Power_WakeupSourceGet(void);

/**
 * @brief  清除唤醒通道标志
 * @param  无
 * @return 无
 */
void Power_WakeupSourceClear(void);

/**
 * @brief  通过唤醒源标志查询gpio唤醒引脚：[0~42],分别对应GPIOA[31:0]、GPIOB[8:0]、GPIOC[0]
 * @param  SourSel为所获取的唤醒源，为：SYSWAKEUP_SOURCE0_GPIO-SYSWAKEUP_SOURCE5_GPIO
 * @return gpio唤醒引脚
 * @note   必须先获取唤醒源
 *         只在SourSel为GPIO才有效
 */
uint32_t Power_WakeupGpioGet(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  通过唤醒源标志查询gpio唤醒的触发边沿
 * @param  SourSel:
 * @return 唤醒源的触发边沿      0：下降沿； 1：上升沿
 * @note   必须先获取唤醒源
 *         只在SourSel为GPIO和PowerKey才有上升沿或者下降沿触发之分，其他SourSel都是默认上升沿触发
 */
uint8_t Power_WakeupEdgeGet(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  使能LVD低电压检测（VIN电压）
 * @param  无
 * @return 无
 * @note   无
 */
void Power_LVDEnable(void);

/**
 * @brief  禁能LVD低电压检测（VIN电压）
 * @param  无
 * @return 无
 * @note   无
 */
void Power_LVDDisable(void);

/**
 * @brief  LVD低电压检测的水位选择（VIN电压）
 * @param  Lvd_Threshold_Sel  水位选择档位[2.3~3.0]（默认水位为2.3V）
 * @return 无
 * @note   无
 */
void Power_LVDThresholdSel(PWR_LVD_Threshold_SEL Lvd_Threshold_Sel);

/**
 * @brief  使用DeepSleep功能时配置LVD低电压检测的水位（VIN电压）
 * @param  Lvd_Threshold_Sel  LVD低电压检测的水位选择（默认水位为2.3V）
 * @return 无
 * @note   无
 */
void Power_LVDWakeupConfig(PWR_LVD_Threshold_SEL Lvd_Threshold_Sel);

/**
 * @brief   Is HRC run during deepsleep?
 * @param   IsOpen  TRUE: Run HRC during deepsleep
 *                  FALSE: Not run HRC during deepsleep
 * @return TRUE:配置成功
 *         FALSE:配置失败
 * @note   若返回值是FALSE则说明前面已经配置的唤醒源中有需要用到HRC的Source,所以禁止关闭
 */
bool Power_HRCCtrlByHwDuringDeepSleep(bool IsOpen);


/**
 * @brief  Close RF3V3D for DeepSleep
 * @param  无
 * @return 无
 * @note   无
 */
void RF_PowerDown(void);

/**
* @brief  获取LDO11D配置的电压值
* @param  void
* @return uint16_t 电压值，单位：mV
* @note   none
*/
uint16_t Power_GetLDO11DConfigVoltage();

/**
* @brief  获取LDO16配置的电压值
* @param  void
* @return uint16_t 电压值，单位：mV
* @note   none
*/
uint16_t Power_GetLDO16ConfigVoltage();

/**
* @brief  获取LDO33D配置的电压值
* @param  void
* @return uint16_t 电压值，单位：mV
* @note   none
*/
uint16_t Power_GetLDO33DConfigVoltage();

/**
* @brief  获取LDO33A配置的电压值
* @param  void
* @return uint16_t 电压值，单位：mV
* @note   none
*/
uint16_t Power_GetLDO33AConfigVoltage();

/**
 * @brief  config power LDO33D voltage
 * @param  level: 0：2.7V  
 *                1：2.8V
 *                2：2.9V
 *                3：3.0V
 *                4：3.1V
 *                5：3.2V
 *                6：3.3V
 * @return none
 * @note   芯片个体之间会有稍许差异和偏差，偏差范围在0mV~50mV之间；其中2.9V和3.0V挡位偏差较大在80-180mV
 */
void Power_LDO33DConfig(uint8_t level);

/**
* @brief  config power LDO33A voltage
* @param  value: 0:3.1V
                 1:3.2V 
                 2:3.3V 
                 3:3.4V  
                 4:3.5V
* @return void
* @note  芯片个体之间会有稍许差异和偏差，偏差范围在0mV~30mV之间；
*/
void Power_LDO33AConfig(uint8_t level);

/**
 * @brief  config power LDO33D high current limit
 * @param  OCSel_val: Set high current limit for LDO33  0:68mA; 1:300mA; 2:350mA
 * @note   none
 */
void Power_LDO33DConfigHighCurrentLimit(uint8_t OCSel_val);

/**
 * @brief  config power LDO16D high current limit
 * @param  OCSel_val: Set high current limit for LDO16  0:105mA; 1:208mA; 2:252mA
 * @note   none
 */
void Power_LDO16DConfigHighCurrentLimit(uint8_t OCSel_val);

/**
 * @brief  config power LDO33A high current limit
 * @param  OCSel_val: Set high current limit for LDO33A  0:24mA; 1:104mA; 2:128mA
 * @note   none
 */
void Power_LDO33AConfigHighCurrentLimit(uint8_t OCSel_val);

typedef enum _PWR_DCDC_LVL_SEL
{
    PWD_DCDC_LVL_1V6 = 3, // 1.6V
    PWD_DCDC_LVL_1V5 = 4, // 1.5V
    PWD_DCDC_LVL_1V4 = 5, // 1.4V
    PWD_DCDC_LVL_1V3 = 6, // 1.3V
} PWR_DCDC_LVL_SEL;
/**
 * @brief  ldo_switch_to_dcdc, 切换到DCDC模式，配置电压
 * @param  trim_cfg: 3-1.6V;4-1.5V;5-1.4V;6-1.3V
 * @return void
 * @note
 *          DCDC电感选型： 负载最大电流150mA选择一类电感，负载最大电流100mA选择一类或二类电感
 *          一类电感： 4.7uH，Isat>360mA，DCR<0.3Ω，Irms>220mA，S.R.F>10MHz
 *          二类电感： 4.7uH,360mA>Isat>200mA，L0>2uH @ 400mA ,DCR<0.3Ω，Irms>220mA，S.R.F>15MHz
 *          注意：
 *           1.不支持DCDC的芯片型号请勿调用该接口，否则会导致芯片因无法正常供电而无法工作！
 *           2.切换到DCDC需要每次开机都调用该接口，芯片将从LDO切换到DCDC工作模式，若后续需要调整DCDC电压，请调用Power_DCDCConfig()接口。
 */
void ldo_switch_to_dcdc(PWR_DCDC_LVL_SEL trim_cfg);

/**
 * @brief  Config LDO16D voltage
 * @param  uint8_t value: 3-1.6V;4-1.5V;5-1.4V;6-1.3V
 * @return void
 * @note   需要在DCDC模式下调用，否则无效。
 */
void Power_DCDCConfig(PWR_DCDC_LVL_SEL value);

/**
 * @brief  Power config LDO16 voltage
 * @param  value: 0-1.6V
 * @return void
 * @note   none
 */
void Power_LDO16Config(uint8_t value);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__POWERCONTROLLER_H__

/**
 * @}
 * @}
 */
