/**
 **************************************************************************************
 * @file    soc_calibration.h
 * @brief   soc_calibration
 *
 * @author Yangsen
 * @version V0.0.1
 *
 * $Created: 2025-02-07
 *
 * @Copyright (C) 2024, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


#ifndef __SOC_CALIBRATION_H__
#define __SOC_CALIBRATION_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include "type.h"

#define SOC_CALIBRATION_NVM             (3) //校准标志位
#define SOC_CALIBRATION_GPIO_STATE      (4) //GPIO状态标志位、

#define CFG_SOC_CALIBRATION_ADC_KEY     (1) /**ADC KEY的 key8触发校准模式和读频偏模式*/
#define CFG_SOC_CALIBRATION_AUTO        (2) /**上电自动进入校准模式或读频偏模式*/

#define CFG_SOC_ACCURATE_NORMAL         (1)//校准速度快，适用于批量测试
#define CFG_SOC_ACCURATE_HIGH	        (2)//校准精度更高但是速度慢，适用于做FCC认证和特别高需求的情况

#define CFG_SOC_LENGTH_NORMAL           (0)//校准范围不做判断
#define CFG_SOC_LENGTH_HIGH	            (1)//校准范围在2m左右,也跟当前板子天线和摆放位置有关
typedef enum _Soc_calstate{
    soc_calstart,
    soc_cal,        
}Soc_calstate;

void soc_cal_mode2set(void);
//读取校准状态
Soc_calstate soc_cal_read(void);
//读取校准状态 
Soc_calstate soc_cal_moderead(void);
//校准模式初始化
void soc_cal_wirelessinit(void);
//读取flash校准值后进行校准
void soc_cal_set(void);
//结束频偏校准，先进行重启然后进入正常模式
void soc_cal_end_restart(void);
//开始进行频偏校准，先进行重启然后进入校准模式
void soc_cal_mode1set(void);
//手动配置频偏参数
void soc_cal_valueset(uint8_t TrimValueXi,uint8_t TrimValueXo);
#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__SOC_CALIBRATION_H__
