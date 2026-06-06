/*
 * main_tx.c
 *
 *  Created on: Mar 5, 2025
 *      Author: piwang
 */

#include "irqn.h"
#include "app_config.h"
#include "app_message.h"
#include "timeout.h"
#include "wireless_hmi.h"
#include "log_info.h"

#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
extern unsigned char audio_init_isready;

#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX)
TIMER TimerPairITNR;
#define TIMEOUT_PAIR		(30*1000)//默认设置为30s
static uint32_t wireless_1tnr_pair_flag = 0;
#endif
//SystemEventProcess
//Porting:No effect、Vol、Mute
void WirelessMsgProcess(uint32_t Msg)
{
	switch(Msg)
	{
#ifdef CFG_FUNC_BLE_OTA_EN
	case MSG_OTA_UPGRADE:
		DBG("MSG_OTA_UPGRADE\n");
		start_up_grate(SysResourceBtOTA);
		break;
#endif
#ifdef CFG_FUNC_2G4_OTA_TX_MASTER
	case MSG_OTA_FLASH_ANALYZE:
		DBG("MSG_OTA_FLASH_ANALYZE\n");
		{
			extern char EILM_SIZE;
			uint32_t offset = &EILM_SIZE;
			DBG("offset = %d\n", offset);
			OTAFlashAnalyze(offset);
		}
		break;
	case MSG_2G4_OTA_UPGRADE:
		OTA2g4UpgradeFlag = 1;
		{
			extern char EILM_SIZE;
			uint32_t offset = &EILM_SIZE;
			DBG("offset = %d\n", offset);
			if(OTAFlashAnalyze(offset) == TRUE)
			{
				OTAFlashAnalyzeFlag = 1;
			}
		}
#endif
		break;
#ifdef DEEP_SLEEP_MODE_EN
	case MSG_DEEPSLEEP:
		DBG("deepsleep\n");
		wireless2_SetRfStopMode();
		break;
	case MSG_WAKEUP:
		DBG("wakeup\n");
		wireless2_ClrRfStopMode();
		break;
#endif
//Porting 改宏
#ifdef KEY_REMOTE
	case MSG_EFFECT_USR1:
		DBG("DENOISE\n");
		InfoBitsSet(INFO_BIT_BLUENS);
//		LedStateSet();
//		FlowParaModeChange(1);
		break;
	case MSG_EFFECT_DEFAULT:
		DBG("NORMAL\n");
		InfoBitsClear(INFO_BIT_BLUENS);
//		LedStateSet();
//		FlowParaModeChange(0);
		break;
#endif
	case MSG_WIRELESS_REMATCH:
		if(wireless2_GetRfStopMode()==0)
		{
			wireless2_SetRfStopMode();
			DBG("Rematch!\n");
			TX_PairedClearRematch();
			wireless2_ClrRfStopMode();
		}
		break;

#ifdef DEEP_SLEEP_MODE_SEND_TO_RX
	case MSG_RX_WAKEUP:
		DBG("MSG_RX_WAKEUP\n");
		wireless_1tnr_master_send_cmd(0x99);
		DBG("normal \n");
		break;
	case MSG_RX_SLEEP:
		DBG("MSG_RX_SLEEP\n");
		wireless_1tnr_master_send_cmd(0x66);
		DBG("silent \n");
		break;
#endif

#ifdef CFG_SOC_CALIBRATION_MODE1_EN
	case MSG_SOC_CAP_CAL_MODE:
		DBG("MSG_SOC_CAP_CAL_MODE\n");
		soc_cal_mode1set();
		break;
#endif

	case MSG_WIRELESS_DUTMODE:
#ifdef 	CFG_OPENDUT_MODE
		DBG("MSG_WIRELESS_DUTMODE\n");
		{
			unsigned char Nvm_readdata ;

			PMU_NvmRead(0,&Nvm_readdata,1);
			Nvm_readdata |= DUT_MODE_CMD;
			PMU_NvmWrite(0,&Nvm_readdata,1);
			NVIC_SystemReset();
		}
#endif
		break;
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX)
	case MSG_1TNR_PAIR_START:
		DBG("MSG_1TNR_PAIR_START\n");
		TimeOutSet(&TimerPairITNR, TIMEOUT_PAIR);
		if(TX_PairedDeviceFlagGet())//有记忆值保留，无记忆值清理
		{
			MvWireless2AdvModeForceUseSavedId(1);
		}
		else
		{
			MvWireless2AdvModeForceUseSavedId(0);
		}
		MvWireless2AdvModeMasterStartPair();
		wireless_1tnr_pair_flag = 1;
		break;
	case MSG_1TNR_PAIR_END:
		DBG("MSG_1TNR_PAIR_END\n");
		MvWireless2AdvModeMasterStopPair();
        wireless2_RfStop();
        while(wireless2_CheckStopped() ==0)
        {
            WDG_Feed();
        }
        printf("restart\n");
        wireless2_Restart();
        wireless_1tnr_pair_flag = 0;
		break;
	case MSG_1TNR_PAIR_CLEAN://清理配对记忆,然后再发起配对流程
		DBG("MSG_1TNR_PAIR_CLEAN\n");
		//SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
		TX_PairedClear();
		break;
	case MSG_1TNR_PAIR_KEY://生成一个新的Key
		DBG("MSG_1TNR_PAIR_KEY\n");
		{
			uint16_t chipsycn = MvWireless2AdvModeGenNewTokenId();
			DBG("chipsycn == %x\n", chipsycn);
		#ifdef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
			chipsycn |= 0x8000;//最高位置1
			DBG("chipsycn == %x\n", chipsycn);
		#endif
			MvWireless2AdvModeFlashSetTokenId(chipsycn);
		}
		break;
#endif
#ifdef CFG_FUNC_IRSEND_EN		
	case MSG_IR_SEND_LONG:
		DBG("IR SEND START\n");
	#ifdef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
		uint16_t chipsycn = MvWireless2AdvModeGenNewTokenId();
		DBG("chipsycn == %x\n", chipsycn);
		chipsycn |= 0x8000;//最高位置1
		DBG("chipsycn == %x\n", chipsycn);
		MvWireless2AdvModeFlashSetTokenId(chipsycn);
		Ir_SendDataUpdata(chipsycn);//刷新IR数据表

	#ifdef CFG_FUNC_AUTO_PAIRING_EN
		extern Tx_Flash_param_t flash_dev_param;
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
		flash_dev_param.PairedDevInfo = chipsycn;
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
	#endif
		wireless2_RfStop();
        while(wireless2_CheckStopped() ==0)
        {
            WDG_Feed();
        }
        printf("restart\n");
        wireless2_Restart();
	#endif

		Ir_SendInit(nec_buf, sizeof(nec_buf)/sizeof(uint32_t));
		ir_cnt++;
		break;		
	case MSG_IR_SEND_HOLD:
		break;
	case MSG_IR_SEND_LONG_PRESS_RELEASE:
		DBG("IR SEND END\n");
		ir_cnt = 0;	
		break;
#endif
#if	defined(WIRELESS_TURNKEY6_1)
	case MSG_2TNR_PAIR:
		APP_DBG("MSG_2TNR_PAIR\n");
		uni_2tnr_m_pair_tx_config();
		break;
#endif

#ifdef CFG_FCC_MODE
	case MSG_WIRELESS_FCC:
	fcc_mode_start();
	break;
#endif
#ifdef WIRELESS_TURNKEY8_1
	case MSG_WL_RS_FORCE_RELEASE:
		DBG("RS SCH FORCE RELEASE\n");
		MvWireless2AdvModeResourceRelease();
		break;

	case MSG_WL_RS_SCH_PERMIT_SWITCH:
		{
			uint8_t permit = 0;
			permit = MvWireless2AdvModeResourceSchSwitch();
			if(permit == 1)
			{
				DBG("RS SCH SWITCH 1\n");
			}
			else if(permit == 0)
			{
				DBG("RS SCH SWITCH 0\n");
			}
		}
		break;

	case MSG_WL_LOCAL_MUTE:
		break;

	case MSG_WL_REMOTE_CLOSE:
		DBG("REMOTE CLOSE RECEIVER\n");
		MvWireless2AdvModeCloseReceiver();
		break;
#endif

	default:
		break;
	}


#ifdef CFG_2T1R_SEND_MULTIBYTE_CMD_EN
	if(wireless_user_cmd_tx_result_get())
	{
		wireless_user_cmd_rx_data_get();
		wireless_user_cmd_tx_result_clean();
	}
#endif
	//2.4G连接失败，一直重连
//	if(IsWirelessConnectFail)
//	{
//		printf("BB_Write_Inq_Mode = 0\n");
//		void BB_Write_Inq_Mode(uint8_t inq_mode);
//		BB_Write_Inq_Mode(0);
//		//BB_inq(0x9E8B34 & 0xFF , 8, 0);
//		IsWirelessConnectFail = 0;
//	}
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX)
	if((IsTimeOut(&TimerPairITNR)) && (wireless_1tnr_pair_flag == 1))
	{
		DBG("MSG_1TNR_PAIR_END\n");
		MvWireless2AdvModeMasterStopPair();
        wireless2_RfStop();
        while(wireless2_CheckStopped() ==0)
        {
            WDG_Feed();
        }
        printf("restart\n");
        wireless2_Restart();
        wireless_1tnr_pair_flag = 0;
	}
#endif
}

//
void SchemeInit(void)
{
#ifndef CFG_RESOURCE_DIS
	#if (defined(CFG_DCDC_EN)&&\
		( (!(defined(WIRELESS_TURNKEY2_9))) && (!(defined(WIRELESS_TURNKEY3_5))) && (!(defined(WIRELESS_TURNKEY3_6))) ))
		//ldo_switch_to_dcdc(3);//0-1.9V;1-1.8;2-1.7V;3-1.6V;4-1.5V;5-1.4V;6-1.3V
		ldo_switch_to_dcdc(5);//0-1.9V;1-1.8;2-1.7V;3-1.6V;4-1.5V;5-1.4V;6-1.3V
	#else
		Power_LDO16Config(0);
	#endif
#endif //#ifndef CFG_RESOURCE_DIS

#ifdef CFG_SOC_CALIBRATION_EN
	soc_cal_set();
#endif
	WirelessDeviceIdInit();
	TX_PairedDeviceInit();
#ifdef CFG_LOCK_PAIRED_TX_ONLY
	TX_PairedDeviceInit_TXOnly();
#endif

#ifdef 	CFG_OPENDUT_MODE
	PMU_NVMInit();
	{
		unsigned char Nvm_readdata;
		extern void Wireless_DutInit(void);
		extern void Wireless_DutProcess(void);

		PMU_NvmRead(0,&Nvm_readdata,1);
		if( (Nvm_readdata&DUT_MODE_CMD) == DUT_MODE_CMD )
		{
			DBG("\n wireless DUT mode running. .\n\n");
			Nvm_readdata &= ~DUT_MODE_CMD;
			PMU_NvmWrite(0,&Nvm_readdata,1);
			Wireless_DutInit();
			while(1)
			{
				WDG_Feed();
				Wireless_DutProcess();
			}
		}
	}
#endif

#if (defined(CFG_SOC_CALIBRATION_MODE1_EN))||(defined(CFG_SOC_CALIBRATION_MODE2_EN)) || (defined(CFG_SOC_CALIBRATION_READ))
	if(soc_cal_moderead() == soc_calstart || soc_cal_read() == soc_calstart)
	{
		DBG("soc cal start\n");
		soc_cal_wirelessinit();
	}
	else
#endif
	{
		WIRELESS_FUNCTION(BB_MPU_START_ADDR);
	}

#if defined(CFG_2T1R_SEND_MULTIBYTE_CMD_EN)
	wireless_user_cmd_rx_int();
#endif

#if defined(CFG_LOCK_PAIRED_TXRX) && defined(WIRELESS_TURNKEY3_1)
	//if((!TX_PairedDeviceFlagGet()) && ( 非自动配对条件满足))//芯片启动之后线读取配对值，如果没有生成一个配对码
	if(!TX_PairedDeviceFlagGet())
	{
		MVWIRE2_1tnr_master_force_use_saved_id(1);
		DBG("MSG_1TNR_PAIR Debug\n");
		extern uint16_t SWBB_1TnR_M_Gen_New_Token_id(void);
		uint16_t chipsycn = SWBB_1TnR_M_Gen_New_Token_id();
		DBG("chipsycn == %x\n", chipsycn);
		#ifdef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
		chipsycn |= 0x8000;//最高位置1
		DBG("chipsycn == %x\n", chipsycn);
		#endif
		m_s_1tnr_flash_set_token_id(chipsycn);
	#ifdef CFG_FUNC_IR_SEND_PAIRING_KEY
		Ir_SendDataUpdata(chipsycn);//刷新IR数据表
	#endif
	}
	#ifdef CFG_FUNC_IR_SEND_PAIRING_KEY
	else
	{
		DBG("key = %x\n",TX_PairInfoGet(1));
		Ir_SendDataUpdata(TX_PairInfoGet(1));//刷新IR数据表
	}
	#endif
#endif

	MVWIRE2_DeviceRoleSet(WIRELESS_SDK_ROLE);

	#ifndef MV_WIRELESS2_MODE2
		set_swbb_speed(MV_WIRELESS2_PARAM1);//speed(0:1M速率;1:2M速率)
	#else
		MVWIRE2_2T1R_Set_TxMode(MV_WIRELESS2_PARAM1);
		MVWIRE2_SetWorkMode(MV_WIRELESS2_PARAM2);
	#endif//MV_WIRELESS2_MODE2

	WirelessInit();
	NVIC_SetPriority(BT_IRQn,0);
	NVIC_EnableIRQ(BT_IRQn);

#if (defined(CFG_SOC_CALIBRATION_MODE2_EN))
	if(soc_cal_moderead() == soc_cal){
		soc_cal_mode2set();
	}
#endif

	audio_init_isready = 1;

#ifdef CFG_FUNC_2G4_OTA_EN
	TimeOutSet(&TimeOTA2G4, 100);
#endif

	//仅仅方便调试，建议使用ADC来触发进行广播配对
#ifdef CFG_TX_AUTO_SEND_PAIRING_KEY
	TimeOutSet(&TimerPairITNR, 3000);
	MvWireless2AdvModeForceUseSavedId(1);
	MvWireless2AdvModeMasterStartPair();
	wireless_1tnr_pair_flag = 1;
#endif

#if defined(CFG_FUNC_DEBUG_EN)//Porting care of
	LogTimerInit();
#endif
}
#endif //#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
