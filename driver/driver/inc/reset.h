/**
  ******************************************************************************
  * @file    reset.h
  * @author  Peter Zhai
  * @version V1.0
  * @date    2017-10-27
  * @brief
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/**
 * @addtogroup SYSTEM
 * @{
 * @defgroup reset reset.h
 * @{
 */

#ifndef __RESET_H__
#define __RESET_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

/**
 * reset module(Register) macro define
 */
typedef enum __RESET_REGISTER_MODULE
{
	PWR_REG_SEPA = (1 << 0),				/**<Power register separate reset*/
	EFUSE_REG_SEPA = (1 << 1),				/**<Efuse register separate reset*/
	FLASH_CTRL_REG_SEPA = (1 << 2),			/**<Flash control register separate reset*/
	TIMER1_TIMER2_REG_SEPA = (1 << 3),      /**<Timer1 and Timer2 register separate reset */
	TIMER5_TIMER6_REG_SEPA = (1 << 4),		/**<Timer5 and Timer6 register separate reset */
	TIMER3_TIMER4_REG_SEPA = (1 << 5),      /**<Timer3 and Timer4 register separate reset */
	SDIO_REG_SEPA = (1 << 6),               /**<SDIO register separate reset */
	SPIM_REG_SEPA = (1 << 7),				/**<SPIM register separate reset */
	UART0_REG_SEPA = (1 << 8),       		/**<UART0 register separate reset */
	I2S0_REG_SEPA = (1 << 9),				/**<I2S0 register separate reset */
	MDAC_REG_SEPA = (1 << 10),				/**<MDAC register separate reset */
	ASDM_REG_SEPA = (1 << 11),				/**<ASDM register separate reset */
	SADC_REG_SEPA = (1 << 12),				/**<SADC register separate reset */
	FFT_REG_SEPA = (1 << 13),				/**<FFT register separate reset */
	DMAC_REG_SEPA = (1 << 14),          	/**<MDAC register separate reset */
	SPIS_REG_SEPA = (1 << 15),           	/**<SPIS register separate reset */
	UART1_REG_SEPA = (1 << 16),           	/**<UART1 register separate reset */
	I2S1_REG_SEPA = (1 << 17),   			/**<I2S1 register separate reset */
	IR_REG_SEPA = (1 << 18),           		/**<IR register separate reset */
	CAN_REG_SEPA = (1 << 19),              	/**<CAN register separate reset */
	RTC_REG_SEPA = (1 << 20),               /**<RTC register separate reset */
	TIMER7_REG_SEPA = (1 << 21),          	/**<TIMER7 register separate reset */
}RESET_REGISTER_MODULE;

/**
 * reset module(Function) macro define
 */
typedef enum __RESET_FUNC_MODULE
{
	I2S0_FUNC_SEPA = (1 << 0),				/**<I2S0 function separate reset*/
	ASDM0_FUNC_SEPA = (1 << 1),				/**<ASDM0 function separate reset*/
	MDAC_FUNC_SEPA = (1 << 2),		        /**<MDAC function separate reset*/
	SADC_FUNC_SEPA = (1 << 3),		        /**<SADC function separate reset*/
	USB_FUNC_SEPA = (1 << 4),             	/**<USB function separate reset */
	UART0_FUNC_SEPA = (1 << 5),		        /**<UART0 function separate reset */
	FSHC_FUNC_SEPA = (1 << 6),       		/**<FSHC function separate reset */
	TIMER1_FUNC_SEPA = (1 << 7),			/**<TIMER1 function separate reset */
	TIMER2_FUNC_SEPA = (1 << 8),      	    /**<TIMER2 function separate reset */
	TIMER3_FUNC_SEPA = (1 << 9),			/**<TIMER3 function separate reset */
	TIMER4_FUNC_SEPA = (1 << 10),	        /**<TIMER4 function separate reset */
	TIMER5_FUNC_SEPA = (1 << 11),		    /**<TIMER5 control function separate reset */
	TIMER6_FUNC_SEPA = (1 << 12),			/**<TIMER6 function separate reset */
	SPIM_FUNC_SEPA = (1 << 13),				/**<SPIM function separate reset */
	SDIO_FUNC_SEPA = (1 << 14),           	/**<SDIO function separate reset */
	DMAC_FUNC_SEPA = (1 << 15),           	/**<DMAC function separate reset */
	I2C_FUNC_SEPA = (1 << 16),           	/**<I2C function separate reset */
	FFT_FUNC_SEPA = (1 << 17),   			/**<FFT function separate reset */
	BTDM_FUNC_SEPA = (1 << 18),           	/**<BTDM function separate reset */
	I2S1_FUNC_SEPA = (1 << 19),             /**<I2S1 function separate reset */
	ASDM1_FUNC_SEPA = (1 << 20),           	/**<ASDM1 function separate reset */
	UART1_FUNC_SEPA = (1 << 21),          	/**<UART1 function separate reset */
	SPDIF_FUNC_SEPA = (1 << 22),          	/**<SPDIF function separate reset */
	SPIS_FUNC_SEPA = (1 << 23),          	/**<SPIS function separate reset */
	IR_FUNC_SEPA = (1 << 24),          		/**<IR function separate reset */
	CAN_FUNC_SEPA = (1 << 25),          	/**<CAN function separate reset */
	MDM_FUNC_SEPA = (1 << 26),          	/**<MDM function separate reset */
	RTC_FUNC_SEPA = (1 << 27),          	/**<RTC function separate reset */
	MCLKCNT_FUNC_SEPA = (1 << 28),          /**<MCLKCNT function separate reset */
	SPDIF1_FUNC_SEPA = (1 << 29),          	/**<SPDIF1 function separate reset */
	TIMER7_FUNC_SEPA = (1 << 30),          	/**<TIMER5 function separate reset */
	TIMER8_FUNC_SEPA = (1 << 31),          	/**<TIMER8 function separate reset */

}RESET_FUNC_MODULE;

/**
 * @brief	获取复位源
 * @param	None
 * @return	返回复位源
 * 			bit0=1	Power On Reset
 * 			bit1=1	Pin Reset
 * 			bit2=1	Powerkey long_press Reset
 * 			bit3=1	Charge Reset
 * 			bit4=1	WatchDog Reset
 * 			bit5=1	LVD(Low Voltage Detected On Vdd33) Reset
 * 			bit6=1	CPU SW Debug Reset
 * 			bit7=1	System Reset
 * @note
 */
uint16_t Reset_FlagGet(void);

/**
 * @brief	Clear复位标志
 * @param	None
 * @return	None
 * @note
 */
void Reset_FlagClear(void);

/**
 * @brief	系统复位
 * @param	None
 * @return	None
 * @note
 */
void Reset_McuSystem(void);

/**
 * @brief	模块寄存器复位
 * @param	Module：模块
 * @return	None
 * @note
 */
void Reset_RegisterReset(RESET_REGISTER_MODULE Module);

/**
 * @brief	模块功能复位
 * @param	Module：模块
 * @return	None
 * @note
 */
void Reset_FunctionReset(RESET_FUNC_MODULE Module);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__RESET_H__

/**
 * @}
 * @}
 */
