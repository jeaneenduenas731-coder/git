#include <stdlib.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "app_config.h"
#include "debug.h"
#include "clk.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "chip_info.h"
#include "remap.h"
#include "irqn.h"
#include "wireless2.h"
#include "reset.h"
#include "soc_calibration.h"
#include "pmu.h"
#include "timeout.h"
#include "bb_api.h"
#if defined(CFG_WIRELESS_EN)

#define BT_ADDR_SIZE				6
#define BT_NAME_SIZE				40
#define BLE_NAME_SIZE				40

typedef struct Flash_soc_cap_param
{
	uint8_t mos_xi;
	uint8_t mos_xo;
}Flash_soc_cap_param_t;

typedef struct _BT_CONFIGURATION_PARAMS
{
	//BT参量表中排序
	uint8_t			bt_LocalDeviceAddr[BT_ADDR_SIZE];//flash mac顺序: NAP-UAP-LAP (此变量用于保存flash的参数,不能随意修改)
	uint8_t			ble_LocalDeviceAddr[BT_ADDR_SIZE];
	uint8_t			bt_LocalDeviceName[BT_NAME_SIZE];
	uint8_t			ble_LocalDeviceName[BLE_NAME_SIZE];	//预留:现BLE name是通过BLE工具生成的广播包中包含

	uint8_t			bt_ConfigHeader[4];
	
	uint8_t			bt_trimValue;
	uint8_t			bt_TxPowerValue;
	
	uint32_t		bt_SupportProfile;
	uint8_t			bt_simplePairingFunc;
	uint8_t			bt_pinCode[17];//max len: 16
	uint8_t			bt_pinCodeLen;
	
	uint8_t			bt_reconFunc;
	uint8_t			bt_reconDevNum;
	uint8_t			bt_reconCount;
	uint8_t			bt_reconIntervalTime;
	uint8_t			bt_reconTimeout;
	uint8_t			bt_accessMode;
	uint8_t			soc_HoscCapXi;
	uint8_t			soc_HoscCapXo;
	#if BT_SOURCE_SUPPORT
	uint32_t		bt_source_SupportProfile;
	#endif
}BT_CONFIGURATION_PARAMS;

static Flash_soc_cap_param_t soc_cap;
static BT_CONFIGURATION_PARAMS	Params;


void soc_cal_set(void)
{
	SpiFlashRead(CONINF_CAP_FLASH_ADDR, (uint8_t *)&Params, sizeof(Params), 1);

	if((Params.bt_trimValue != 0xFF))
	{
		Clock_HOSCCapSet(Params.bt_trimValue, Params.bt_trimValue);
		return;
	}
#ifndef CFG_RESOURCE_DIS
	//for old firmware
	SpiFlashRead(CONINF_CAP_FLASH_ADDR1, (uint8_t *)&soc_cap, sizeof(soc_cap), 1);

	if((soc_cap.mos_xi != 0xFF) && (soc_cap.mos_xo != 0xFF))
	{
		Clock_HOSCCapSet(soc_cap.mos_xi, soc_cap.mos_xo);
		return;
	}
#endif
}
#endif//#if defined(CFG_WIRELESS_EN)

#ifdef CFG_SOC_CALIBRATION_EN
#define GPIO_RESET_MISC_ADR                   (*(volatile unsigned long *) 0x40020000)

 
static bool soc_log_display  = 1;
static uint8_t soc_cal_flag = FALSE;
static bool _g_soc_mode;
static TIMER dis_cnt;
extern unsigned char osc_cap_min_1,osc_cap_max_1;
extern int8_t sync_time;
extern uint32_t last_avg_rssi;

void soc_cal_valueset(uint8_t TrimValueXi,uint8_t TrimValueXo)
{
	SpiFlashRead(CONINF_CAP_FLASH_ADDR, (uint8_t *)&Params, sizeof(Params), 1);
	Params.bt_ConfigHeader[0]='M';
	Params.bt_ConfigHeader[1]='V';
	Params.bt_ConfigHeader[2]='B';
	Params.bt_ConfigHeader[3]='T';
	Params.bt_trimValue = TrimValueXo;
	Params.bt_trimValue = TrimValueXi;	
	SpiFlashErase(SECTOR_ERASE, CONINF_CAP_FLASH_ADDR/4096, 1);
	SpiFlashWrite((CONINF_CAP_FLASH_ADDR), (uint8_t *)&Params, sizeof(Params),1);	
}

Soc_calstate soc_cal_moderead(void)
{
	_g_soc_mode = soc_cal;
#if defined CFG_SOC_CALIBRATION_MODE1_EN || defined CFG_SOC_CALIBRATION_MODE2_EN
	if((Params.bt_trimValue != 0xFF))
	{
		_g_soc_mode = soc_cal;
	}
	else if((soc_cap.mos_xi != 0xFF) && (soc_cap.mos_xo != 0xFF)) 
	{
		_g_soc_mode = soc_cal;
	}
	else {
		PMU_NvmRead(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
		soc_log_display = _g_soc_mode;
	}
#endif
	if(_g_soc_mode == soc_cal){
		uint8_t GPIO_RESET = 0;
		PMU_NvmRead(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);
		if(GPIO_RESET == 1){
		GPIO_RESET_MISC_ADR = 0;
		}
		GPIO_RESET = 0;
		PMU_NvmWrite(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);		
	}
	return _g_soc_mode;
}


Soc_calstate soc_cal_read(void)
{
	_g_soc_mode = soc_cal;
#ifdef CFG_SOC_CALIBRATION_READ
	PMU_NvmRead(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
	soc_log_display = _g_soc_mode;
#endif

	return _g_soc_mode;
}

void soc_cal_mode1set(void)
{
#if defined(CFG_SOC_CALIBRATION_MODE1_EN)		
	_g_soc_mode ^= 1;
	PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
	Reset_McuSystem();
#endif	
}

void soc_cal_mode2set(void) {
#if !defined (CFG_SOC_CALIBRATION_READ) && defined(CFG_SOC_CALIBRATION_MODE2_EN)	
	if((Params.bt_trimValue == 0xFF) && (soc_cap.mos_xi == 0xFF) && (soc_cap.mos_xo == 0xFF))
	{
		_g_soc_mode = soc_calstart;
		PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
	}
#elif defined (CFG_SOC_CALIBRATION_READ) && defined(CFG_SOC_CALIBRATION_MODE2_EN)
	_g_soc_mode = soc_calstart;
	PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
#endif
}

void soc_cal_end_restart(void)
{

	if(soc_cal_flag == 1)
	{
		while(!wireless2_Check_Test_mode_Stopped())
		{
			WDG_Feed();
		}
		DBG("\nsoc_cap.xi = %d, soc_cap.mos_xo = %d\n", Params.bt_trimValue, Params.bt_trimValue);
		Params.bt_ConfigHeader[0]='M';
		Params.bt_ConfigHeader[1]='V';
		Params.bt_ConfigHeader[2]='B';
		Params.bt_ConfigHeader[3]='T';
		SpiFlashErase(SECTOR_ERASE, CONINF_CAP_FLASH_ADDR/4096, 1);
		SpiFlashWrite((CONINF_CAP_FLASH_ADDR), (uint8_t *)&Params, sizeof(Params),1);

		//重新配置模式
		soc_cal_flag = 0;
		_g_soc_mode = soc_cal;		
		PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);		
		if(GPIO_RESET_MISC_ADR == 1){
			uint8_t GPIO_RESET = 0;
			PMU_NvmWrite(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);
		}
		else{
			uint8_t GPIO_RESET = 1;
		GPIO_RESET_MISC_ADR = 1;
			PMU_NvmWrite(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);
		}
		Reset_McuSystem();
	}
	else if(soc_cal_flag == 2)
	{
		soc_cal_flag = 0;
		DBG("soc_cap_cal_fail\r\n");

		_g_soc_mode = soc_cal;		
		PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
		if(GPIO_RESET_MISC_ADR == 1){
			uint8_t GPIO_RESET = 0;
			PMU_NvmWrite(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);
		}
		else{
			uint8_t GPIO_RESET = 1;
		GPIO_RESET_MISC_ADR = 1;		
			PMU_NvmWrite(SOC_CALIBRATION_GPIO_STATE, &GPIO_RESET, 1);
		}
		Reset_McuSystem();
	}
#if defined(CFG_SOC_CALIBRATION_MODE2_EN)	
	else if(wireless2_test_calibrationget() == 1 && _g_soc_mode == soc_calstart)
	{
			// DBG("111...[%d,%d] %d %d %d\r\n",osc_cap_min_1,osc_cap_max_1,_g_soc_mode , osc_cap_min_1, osc_cap_max_1);

		_g_soc_mode = soc_cal;		
		PMU_NvmWrite(SOC_CALIBRATION_NVM, &_g_soc_mode, 1);
	}
#endif

	if(((soc_log_display == soc_calstart) && osc_cap_min_1 != 255 && osc_cap_max_1 != 0))
	{
		if(IsTimeOut(&dis_cnt))
		{
			TimeOutSet(&dis_cnt,500);

			DBG("OSC Cal...[%d,%d] rssi %d\r\n",osc_cap_min_1,osc_cap_max_1, last_avg_rssi);
			osc_cap_min_1 = 0xff;
			osc_cap_max_1 = 0;
		}
	}
	else if(wireless2_test_calibrationget() == 1)
	{
		if(IsTimeOut(&dis_cnt))
		{
			TimeOutSet(&dis_cnt,500);
			DBG("OSC Read...[%d,%d] rssi %d freq %d\r\n",Params.soc_HoscCapXi,Params.soc_HoscCapXo,last_avg_rssi,sync_time);
			osc_cap_min_1 = 0xff;
			osc_cap_max_1 = 0;
		}
	}
}


void soc_cal_result_cb(unsigned char result, unsigned char mos_xo, unsigned char mos_xi)
{
	if(result == 0)
	{
		Params.bt_trimValue = mos_xo;
		soc_cal_flag = 1;
	}
	else if(result == 1){
		soc_cal_flag = 2;
	}
}

void soc_cal_wirelessinit(void){
	DBG("soc_cap_calstart \r\n");
	wireless2_test_mode_slave_start(soc_cal_result_cb,BB_MPU_START_ADDR);

	MVWIRE2_DeviceRoleSet(WIRELESS_SDK_ROLE);	

	if((Params.bt_trimValue != 0xFF)) {
		wireless2_test_calibrationread(Params.bt_trimValue, Params.bt_trimValue, CFG_SOC_CALIBRATION_ACCURATE, CFG_SOC_CALIBRATION_LENGTH);
	}
	else if((soc_cap.mos_xi != 0xFF) && (soc_cap.mos_xo != 0xFF)) {
		wireless2_test_calibrationread(soc_cap.mos_xi, soc_cap.mos_xo, CFG_SOC_CALIBRATION_ACCURATE, CFG_SOC_CALIBRATION_LENGTH);
	}
	else {
		wireless2_test_calibrationread(0xff, 0xff, CFG_SOC_CALIBRATION_ACCURATE, CFG_SOC_CALIBRATION_LENGTH);
	}
}
#endif
