#ifndef __LEDC_H__
#define __LEDC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief      LEDC 数据线配置
 * GPIOA3-GPIOA7 LEDC_OUT[0-4]  GPIOA9-GPIO10 LEDC_OUT[5-6]   GPIOA9 LEDC_OUT[7]
 */
void LEDC_DataIOConfig(void);

/**
 * @brief      LEDC CLK线配置
 * GPIOA24->LEDC_CLK
 */
void LEDC_CLKIOConfig(void);

/**
 * @brief      LEDC 定时器控制参数 使能定时器dma请求
 *
 * @param[in]  TimerIndex    定时器索引
 * @param[in]  latency:      ledc_clk lanuch edge is generated after N+2 cycle after timer_dma_ack negedge
 * @param[in]  polarity:     ledc_clk polarity 0:launch with posedge  1:launch with negedge
 * @param[in]  enable:       timer_generated lec_clk launch ledc data 0:disable 1;enable
 * @return     0: 配置成功   -1:错误的定时器索引
 */
int32_t  LEDC_CtrlInit(TIMER_INDEX TimerIndex,int8_t latency,int8_t polarity,int8_t enable);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__TIMER_H__
