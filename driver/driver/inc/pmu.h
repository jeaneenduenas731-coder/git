/**
 **************************************************************************************
 * @file    pmu.h
 * @brief	pmu module driver interface
 *
 * @author  tony
 * @version V1.0.7
 *
 * @Created: 201-4-16
 *
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 
 /**
 * @addtogroup PMU
 * @{
 * @defgroup PMU pmu.h
 * @{
 */

#ifndef __PMU_H__
#define __PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "type.h"

/* Typedef -------------------------------------------------------------------*/

typedef enum _POWERUP_EVENT_SEL
{
	CHARGE_IN_OUT_EVENT = (0),
	GPIO_C0_EVENT 		= (1),
	POWERKEY_EVENT      = (2)
}POWERUP_EVENT_SEL;

typedef enum _POWERKEY_PULLUP_SEL
{
	SEL_22K  = (0),
	SEL_650K = (1),
}POWERKEY_PULLUP_SEL;

typedef enum _POWERKEY_SWITCH_MODE
{
	HARD_MODE = 0,
	SOFT_MODE = 1
}POWERKEY_SWITCH_MODE;

typedef enum _POWER_LONG_RESET_MODE
{
	LONGR_RST_MODE_TIMEOUT_KEYUP = 0,
	LONGR_RST_MODE_TIMEOUT = 1
}POWER_LONG_RESET_MODE;

typedef enum _HARD_TRIGGER_MODE
{
	LEVEL_TRIGGER = 0,
	EDGE_TRIGGER = 1
}HARD_TRIGGER_MODE;

typedef enum _POWERKEY_ACTIVE_LEVEL
{
	LOW_INDICATE_POWERON = 0,
	HIGH_INDICATE_POWERON = 1
}POWERKEY_ACTIVE_LEVEL;

typedef enum _POWERKEY_LONGORSHORT_PRESS_SEL
{
	POWERKEY_LONG_PRESS_MODE = 0,
	POWERKEY_SHORT_PRESS_MODE = 1
}POWERKEY_LONGORSHORT_PRESS_SEL;

typedef enum _C0_POWERUP_MODE
{
	C0PU_MODE_LEVEL = 0,
	C0PU_MODE_EDGE_RISING = 1,
	C0PU_MODE_EDGE_FALLING = 2,
	C0PU_MODE_EDGE_BOTH = 3
}C0_POWERUP_MODE;

typedef enum _C0_GPIO_REG_OFF
{
    PMU_GPIO_C0_REG_O_OFF = 0x00,
    PMU_GPIO_C0_REG_O_SET_OFF = 0x04,
    PMU_GPIO_C0_REG_O_CLR_OFF = 0x08,
    PMU_GPIO_C0_REG_O_TGL_OFF = 0x0C
} C0_GPIO_REG_OFF;

/* Define --------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/


/* API -----------------------------------------------------------------------*/

/**
 * @brief   PMU register write enable
 * @param   None
 * @return  None
 * @note
 */
void PMU_WriteEnable(void);

/**
 * @brief   PMU register write disable
 * @param   None
 * @return  None
 * @note
 */
void PMU_WriteDisable(void);

/**
 * @brief   Trigger system entering power down
 * @param   None
 * @return  None
 * @note
 */
void PMU_SystemPowerDown(void);

/**
 * @brief   Enable automatic power up from passive power off state when power ready
 * @param   None
 * @return  None
 * @note
 */
void PMU_AutoPowerOnEnable(void);

/**
 * @brief   Disable automatic power up from passive power off state when power ready
 * @param   None
 * @return  None
 * @note
 */
void PMU_AutoPowerOnDisable(void);

/**
 * @brief   Check setting state of automatic power up mode
 * @param   None
 * @return  true:Enable; false:Disable;
 * @note
 */
bool PMU_IsAutoPowerOnEnabled(void);

/**
 * @brief   Clean NVM mem when first power on
 * @param   None
 * @return  None
 * @note    This function should be called before PMU_NvmRead and PMU_NvmWrite.
 */
void PMU_NVMInit(void);

/**
 * @brief   Read NVM data
 * @param   Nvmoffset: the offset range is 0 - 15
 * @param   Buf: the memory address to store NVM data
 * @param   Length: data length, Nvmoffset + Length <= 16
 * @return  true:read success; false: read failed because of parameter error or PMU_NVMInit not be called firstly;
 * @note    
 */
bool PMU_NvmRead(uint8_t Nvmoffset, uint8_t* Buf, uint8_t Length);

/**
 * @brief   Write NVM data
 * @param   Nvmoffset: the offset range is 0 - 15
 * @param   Buf: the memory address reading data from
 * @param   Length: data length, Nvmoffset + Length <= 16
 * @return  true:write success; false: write failed because of parameter error or PMU_NVMInit not be called firstly;
 * @note    
 */
bool PMU_NvmWrite(uint8_t Nvmoffset, uint8_t* Buf, uint8_t Length);

/**
 * @brief   Get powerup event flag
 * @param   None
 * @return  CHARGE_IN_OUT_EVENT: charge insert or extract
 *          GPIO_C0_EVENT:       GPIO_C0
 *          POWERKEY_EVENT:      valid POWERKEY event
 * @note    
 */
POWERUP_EVENT_SEL PMU_PowerupEventGet(void);

/**
 * @brief   Clear powerup event flag
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerupEventClr(void);

/**
 * @brief   Enable Powerkey to SarADC path, Powerkey can be used as ADCKey
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerkeySarADCEn(void);

/**
 * @brief   Disable Powerkey to SarADC path
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerkeySarADCDis(void);

/**
 * @brief   Check if Powerkey to SarADC path is enabled
 * @param   None
 * @return  true:enabled; false:disabled;
 * @note
 */
uint8_t PMU_PowerkeySarADCEnGet(void);

/**
 * @brief   Set Powerkey pullup through 650K resistance
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerkeyPullup650K(void);

/**
 * @brief   Set Powerkey pullup through 22K resistance
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerkeyPullup22K(void);

/**
 * @brief   Get Powerkey pullup type
 * @param   None
 * @return  SEL_650K: through 650K resistance; SEL_22K: through 22K resistance;
 * @note
 */
POWERKEY_PULLUP_SEL PMU_PowerkeyPullupOhmGet(void);

/**
 * @brief   Clear Powerkey press trigger status flag
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerKeyStateClear(void);

/**
 * @brief   Clear Powerkey short press trigger status flag
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerKeyShortPressStateClear(void);

/**
 * @brief   Clear Powerkey long press trigger status flag
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerKeyLongPressStateClear(void);

/**
 * @brief   Set Powerkey switch mode
 * @param   PowerKeyMode: HARD_MODE or SOFT_MODE
 * @return  None
 * @note
 */
void PMU_PowerKeyModeSet(POWERKEY_SWITCH_MODE PowerKeyMode);

/**
 * @brief   Get Powerkey switch mode
 * @param   None
 * @return  HARD_MODE or SOFT_MODE
 * @note
 */
POWERKEY_SWITCH_MODE PMU_PowerKeyModeGet(void);

/**
 * @brief   Set Powerkey trigger type of hard switch mode
 * @param   HardTriggerMode: 0 - LEVEL_TRIGGER; 1 - EDGE_TRIGGER;
 * @return  None
 * @note
 */
void PMU_PowerKeyHardModeSet(HARD_TRIGGER_MODE HardTriggerMode);

/**
 * @brief   Get Powerkey trigger type settings of hard switch mode
 * @param   None
 * @return  LEVEL_TRIGGER or EDGE_TRIGGER
 * @note
 */
HARD_TRIGGER_MODE PMU_PowerKeyHardModeGet(void);

/**
 * @brief   Set Powerkey active level 
 * @param   PowerkeyActiveLevel: LOW_INDICATE_POWERON or HIGH_INDICATE_POWERON;
 * @return  None
 * @note
 */
void PMU_PowerKeyActiveLevelSet(POWERKEY_ACTIVE_LEVEL PowerkeyActiveLevel);

/**
 * @brief   Get settings of Powerkey active level 
 * @param   None
 * @return  LOW_INDICATE_POWERON or HIGH_INDICATE_POWERON
 * @note
 */
POWERKEY_ACTIVE_LEVEL PMU_PowerKeyActiveLevelGet(void);

/**
 * @brief   Set the startup trigger type of Powerkey when in powerdown status
 * @param   mode_set: POWERKEY_LONG_PRESS_MODE  - long press trigger startup
 *                    POWERKEY_SHORT_PRESS_MODE - short press trigger startup
 * @return  None
 * @note
 */
void PMU_PowerKeyLongOrShortPressSet(POWERKEY_LONGORSHORT_PRESS_SEL mode_set);

/**
 * @brief   Get settings of Powerkey startup trigger type
 * @param   None
 * @return  POWERKEY_LONG_PRESS_MODE or POWERKEY_SHORT_PRESS_MODE
 * @note
 */
POWERKEY_LONGORSHORT_PRESS_SEL PMU_PowerKeyLongOrShortPressGet(void);

/**
 * @brief   Get the flag of Powerkey trigger state
 * @param   None
 * @return  true: Powerkey trigger flag is set; false: Powerkey trigger flag is not set;
 * @note    This flag indicates that the time Powerkey pressed has lasted for short press time
 */
bool PMU_PowerKeyTrigStateGet(void);

/**
 * @brief   Get the flag of Powerkey short press state
 * @param   None
 * @return  true: Powerkey short press flag is set; false: Powerkey short press flag is not set;
 * @note    This flag indicates that the time Powerkey pressed is more than short press time and less than long press time
 */
bool PMU_PowerKeyShortPressTrigStateGet(void);

/**
 * @brief   Get the flag of Powerkey long press state
 * @param   None
 * @return  true: Powerkey long press flag is set; false: Powerkey long press flag is not set;
 * @note    This flag indicates that the time Powerkey pressed has lasted for long press time in soft switch mode
 */
bool PMU_PowerKeyLongPressTrigStateGet(void);

/**
 * @brief   Get current Powerkey pin state
 * @param   None
 * @return  1: Powerkey pin is high level; 0: Powerkey pin is low level;
 * @note
 */
uint8_t PMU_PowerKeyPinStateGet(void);

/**
 * @brief   Enable Powerkey module
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerKeyEnable(void);

/**
 * @brief   Disable Powerkey module
 * @param   None
 * @return  None
 * @note
 */
void PMU_PowerKeyDisable(void);

/**
 * @brief   Get current Powerkey enable state
 * @param   None
 * @return  ture: enabled; false: disabled;
 * @note
 */
bool PMU_IsPowerKeyEnabled(void);

/**
 * @brief   Set Powerkey short press time
 * @param   cnt: 1~63, unit:8ms; 
 * @return  None
 * @note    1) Powerkey short press time is: cnt*8ms + NoiseFilter*4ms;
 *          2) cnt should not be zero
 */
void PMU_PowerKeyShortPressTrigMaxCntSet(uint8_t cnt);

/**
 * @brief   Get settings of Powerkey short press time
 * @param   None
 * @return  setting value
 * @note
 */
uint8_t PMU_PowerKeyShortPressTrigMaxCntGet(void);

/**
 * @brief   Set Powerkey long press time
 * @param   cnt: 1~63, unit:64ms; 
 * @return  None
 * @note    Powerkey long press time is: cnt*64ms + NoiseFilter*4ms;
 */
void PMU_PowerKeyLongPressTrigMaxCntSet(uint8_t cnt);

/**
 * @brief   Get settings of Powerkey long press time
 * @param   None
 * @return  setting value
 * @note    shuld confirm: long press time > short press time
 */
uint8_t PMU_PowerKeyLongPressTrigMaxCntGet(void);

/**
 * @brief   Set Powerkey long press reset time 
 * @param   cnt: 0~15, unit:1.024s; 
 * @return  None
 * @note    Powerkey long press time is: cnt*1.024s + NoiseFilter*4ms;
 */
void PMU_PowerKeyResetTrigMaxCntSet(uint8_t cnt);

/**
 * @brief   Get settings of Powerkey long press reset time 
 * @param   None
 * @return  setting value
 * @note
 */
uint8_t PMU_PowerKeyResetTrigMaxCntGet(void);

/**
 * @brief   Set Powerkey noise filter time
 * @param   cnt: 0~63, unit:4ms; 
 * @return  None
 * @note
 */
void PMU_PowerKeyNoiseFilterMaxCntSet(uint8_t cnt);

/**
 * @brief   Get settings of Powerkey noise filter time
 * @param   None
 * @return  setting value
 * @note
 */
uint8_t PMU_PowerKeyNoiseFilterMaxCntGet(void);

/**
 * @brief   Set Powerkey long press reset mode
 * @param   PowerLongResetMode: 
 *          RST_MODE_TIMEOUT_KEYUP - reset release until press release if press time is longer than setting time
 *          LONGR_RST_MODE_TIMEOUT - reset release when press time reached setting time no matter when press release
 * @return  None
 * @note
 */
void PMU_PowerLongResetModeSet(POWER_LONG_RESET_MODE PowerLongResetMode);

/**
 * @brief  Get settings of Powerkey long press reset mode
 * @param  None
 * @return RST_MODE_TIMEOUT_KEYUP or LONGR_RST_MODE_TIMEOUT
 * @note
 */
POWER_LONG_RESET_MODE PMU_PowerLongResetModeGet(void);

/**
 * @brief  Configure C0 pin function
 * @param  set_sel: 4'd0:gpio default register value
 *                  4'd1:wkup_in(i)
 *                  4'd2:board_pmu_dis(o)
 *                  4'd3:long_press_rst_n(o)
 *                  4'd4:pmu_32k_clk(o)
 *                  4'd5:CORE_POR_N(o)
 *                  4'd6:POWERKEY(o)
 *                  ..
 * @return None
 * @note
 */
void PMU_C0PinConfig(uint8_t set_sel);

/**
 * @brief  Set C0 Powerup mode when C0 used as wakeup pin
 * @param  powerup_mode: C0PU_MODE_LEVEL        - high level trigger powerup
 *                       C0PU_MODE_EDGE_RISING  - rising edge trigger powerup
 *                       C0PU_MODE_EDGE_FALLING - falling edge trigger powerup
 *                       C0PU_MODE_EDGE_BOTH    - both edge trigger powerup
 * @return None
 * @note
 */
void PMU_C0PowerupModeSet(C0_POWERUP_MODE powerup_mode);

/**
 * @brief  Get C0 Powerup mode
 * @param  None
 * @return C0PU_MODE_LEVEL        - high level trigger powerup
 *         C0PU_MODE_EDGE_RISING  - rising edge trigger powerup
 *         C0PU_MODE_EDGE_FALLING - falling edge trigger powerup
 *         C0PU_MODE_EDGE_BOTH    - both edge trigger powerup
 * @note
 */
C0_POWERUP_MODE PMU_C0PowerupModeGet(void);

/**
 * @brief  Set C0 GPIO control 
 * @param  ie: 0 - input disable; other - input enable;
 * @param  oe: 0 - onput disable; other - output enable;
 * @param  ds: driver strength, 0-weak pull(20uA), 1-strong pull(70uA)
 * @param  pd: pull down enable, 0-Pull-down Disable, 1-20uA Pull-down Enable
 * @return none
 * @note
 */
void PMU_C0GPIOCtrlSet(uint8_t ie, uint8_t oe, uint8_t ds, uint8_t pd);

/**
 * @brief  Get settings of C0 GPIO control 
 * @param  pIE: addr ptr to save ie settings
 * @param  pOE: addr ptr to save oe settings
 * @param  pDS: addr ptr to save ds settings
 * @param  pPD: addr ptr to save pd settings
 * @return none
 * @note
 */
void PMU_C0GPIOCtrlGet(uint8_t * pIE, uint8_t * pOE, uint8_t * pDS, uint8_t * pPD);


/**
 * @brief  GPIO_C0 Register Set.
 * @param  RegOffset: such as PMU_GPIO_C0_REG_O_OFF
 * @param  SetVal: such as TURE
 * @return None
 * @note
 */
void PMU_C0GPIORegSet(C0_GPIO_REG_OFF RegOffset,uint8_t SetVal);

/**
 * @brief  PMU restore config for wakeup
 * @param  void
 * @return void
 * @note
 */
void PMU_WakeupRestoreConfig();

/**
 * @brief  PMU config for deepsleep
 * @param  void
 * @return void
 * @note
 */
void PMU_DeepSleepConfig();

/**
 * @brief  get bool to determine if the PMU was first powered on.
 * @param  None
 * @return true:first time power on.
 *         false:not the first time.
 * @note   This function should be called before PMU_PowerKeyEnable()
 */
bool PMU_FristPowerOnFlagGet(void);

/**
 * @brief   Set reserve register one bit to 1
 * @param   BitMask register bit mask[bit0--bit7]
 * @return  None
 * @note    bit0,bit1 is used by system
 */
void PMU_ResRegOneBitSet(uint8_t BitMask);

/**
 * @brief   Clear reserve register one bit to 0
 * @param   BitMask register bit mask[bit0--bit7]
 * @return  None
 * @note    bit0,bit1 is used by system
 */
void PMU_ResRegOneBitClear(uint8_t BitMask);

/**
 * @brief   Get reserve register value
 * @param   None
 * @return  Register value
 * @note
 */
uint8_t PMU_ResRegGet(void);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 * @}
 */
