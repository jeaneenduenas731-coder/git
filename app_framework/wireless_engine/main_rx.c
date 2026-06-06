/*
 * main_rx.c
 *
 *  Created on: Mar 5, 2025
 *      Author: piwang
 */
#include "irqn.h"
#include "app_config.h"
#include "app_message.h"
#include "audio_association.h"
#include "wireless_hmi.h"
#include "log_info.h"

#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
extern unsigned char audio_init_isready;

//0: Left; 1: Right
//return data，0xff表示无消息
//RecvData 当前接受到 实际数据，注意如果有丢包可能是上一次的值
uint8_t KeyRemote(uint8_t device, uint8_t* RecvData);

#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
TIMER TimerPaired;
TIMER TimerConnet;
uint16_t ChipID = 0;
uint16_t ChipIDBack;
uint16_t ChipConnetFlag = 0;
#endif

void cmd_process(void)
{
#ifdef KEY_REMOTE
	uint8_t RemoteMsgL, RemoteMsgR;
	uint8_t RecMsgL, RecMsgR;

	RemoteMsgL = KeyRemote(0, &RecMsgL);
	RemoteMsgR = KeyRemote(1, &RecMsgR);

	if((RemoteMsgL & (~INFO_BIT_MASK)) == INFO_BIT_SYNC)
	{
		DBG("RemoteMsgL = %x\n", RemoteMsgL);
		if(RemoteMsgL & (INFO_BIT_BLUENS))
		{
			//Key1Press = 1;
			DBG("L Key1Press = 1;\n");
			InfoBitsSet(INFO_BIT_BLUENS);
		}
		else
		{
			//Key1Press = 0;
			DBG("L Key1Press = 0;\n");
			InfoBitsClear(INFO_BIT_BLUENS);
		}
	}

	if((RemoteMsgR & (~INFO_BIT_MASK)) == INFO_BIT_SYNC)
	{
		DBG("RemoteMsgR = %x\n", RemoteMsgR);
		if(RemoteMsgR & (INFO_BIT_BLUENS))
		{
			//Key1Press = 1;
			DBG("R Key1Press = 1;\n");
			InfoBitsSet(INFO_BIT_BLUENS);
		}
		else
		{
			//Key1Press = 0;
			DBG("R Key1Press = 0;\n");
			InfoBitsClear(INFO_BIT_BLUENS);
		}
	}
#endif
}
#ifdef CFG_FUNC_IR_REC_PAIRING_KEY
//0xA573 为 SDK中标定值，如果需要修改需要TX的发射码需要同步修改
//注意特征码需要区别普通红外键码
void IrRec2G4PairingKey(uint16_t KeyData)
{
	DBG("link key = %x, Key =%x\n", flash_dev_param.PairedDev1Info, KeyData);
	if(flash_dev_param.PairedDev1Info == KeyData)
	{
		return;//配对码已经一致，不需要反复重置
	}
	SpiFlashRead(CONINF_FLASH_ADDR,(uint8_t*)&flash_dev_param,sizeof(flash_dev_param),100);
	if(KeyData != flash_dev_param.PairedDev1Info)
	{
		DBG("save key\n");
	#ifndef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
		flash_dev_param.PairedDev1Info = KeyData;
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
	#else
		flash_dev_param.PairedDev1Info = KeyData;//不写flash，防止把flash写挂了
	#endif
		wireless2_RfStop();
		while(wireless2_CheckStopped() ==0)
		{
			WDG_Feed();
		}
		DBG("restart\n");
		wireless2_Restart();
	}
	else
	{
		wireless2_RfStop();
		while(wireless2_CheckStopped() ==0)
		{
			WDG_Feed();
		}
		DBG("allow link\n");
		wireless2_Restart();
	}
}
#endif

//void SystemEventProcess(void)
//porting no effect、
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
#ifdef DEEP_SLEEP_MODE_EN
	case MSG_DEEPSLEEP:
		DBG("deepsleep\n");
		wireless2_ModeSleep();
		break;
	case MSG_WAKEUP:
		DBG("wakeup\n");
		wireless2_ModeActive();
		break;
#endif

#ifdef CFG_2T1R_SEND_MULTIBYTE_CMD_EN
	case MSG_2T1R_RX_SEND_MULTI_CMD:
		DBG("2T1R send CMD\n");
		//cmdBuf 用户代码中更新，默认一直使用默认值
		extern uint8_t user_CmdBuf[8];
		wireless_user_cmd_data_updata(user_CmdBuf, 8);
		wireless_user_cmd_send();
		break;
#endif
#ifdef 	CFG_OPENDUT_MODE
	case MSG_WIRELESS_DUTMODE:
		DBG("MSG_WIRELESS_DUTMODE\n");
		{
			unsigned char Nvm_readdata ;

			PMU_NvmRead(0,&Nvm_readdata,1);
			Nvm_readdata |= DUT_MODE_CMD;
			PMU_NvmWrite(0,&Nvm_readdata,1);
			NVIC_SystemReset();
		}
		break;
#endif
	case MSG_WIRELESS_REMATCH:
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX) && !defined(CFG_AUTO_PAIRING_EN)
		MvWireless2AdvModePairingScanEn(1);
#endif
		if(wireless2_GetRfStopMode()==0)
		{
			wireless2_SetRfStopMode();
			DBG("Rematch1!\n");
			RX_PairedInfoSetRematch(NOPAIR_WORD, DEVICE1_MASK);//擦俩是：DEVICE1_MASK | DEVICE2_MASK
			wireless2_ClrRfStopMode();
		}
		break;

	case MSG_WIRELESS_REMATCH2:
		if(wireless2_GetRfStopMode()==0)
		{
			wireless2_SetRfStopMode();
			DBG("Rematch2!\n");
			RX_PairedInfoSetRematch(NOPAIR_WORD, DEVICE2_MASK);
			wireless2_ClrRfStopMode();
		}
		break;

#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
	case MSG_WIRELESS_PAIRING_ALLOWED:
		DBG("MSG_WIRELESS_PAIRING_ALLOWED\n");
		if((ChipID) && IsTimeOut(&TimerConnet))
		{
			ChipIDBack = ChipID;
			MVWIRE2_2T1R_Cancel_Pairing_Device();
			MVWIRE2_2T1R_Set_Allowed_Pairing_Device(ChipID);
			DBG("ChipID  = %x, ChipIDBack = %x\n", ChipID, ChipIDBack);
			ChipID = 0;
			TimeOutSet(&TimerConnet, 2000);
			ChipConnetFlag = 1;
	#ifndef CFG_LOCK_PAIRED_RX_SAVE_NVM
			if(RX_PairedDeviceFlagGet() & DEVICE1_MASK == ~DEVICE1_MASK)
			{
				RX_PairedInfoSet(ChipIDBack, DEVICE1_MASK);
				DBG("save device1\n");
			}
			else if(RX_PairedDeviceFlagGet() & DEVICE2_MASK == ~DEVICE2_MASK)
			{
				RX_PairedInfoSet(ChipIDBack, DEVICE2_MASK);
				DBG("save device2\n");
			}
	#endif
		}
		break;

	case MSG_WIRELESS_PAIRING_CANCEL:
		DBG("MSG_WIRELESS_PAIRING_CANCEL\n");
		MVWIRE2_2T1R_Cancel_Pairing_Device();
		break;
#endif
#ifdef CFG_LOCK_PAIRED_RX_SAVE_NVM
	case MSG_2T1R_RX_SAVE_FLASH:
		DBG("MSG_WIRELESS_PAIRING_CANCEL\n");
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
		memset(&flash_dev_param.PairedDev1Info, 0, sizeof(Rx_Flash_param_t));
		PMU_NvmRead(5, &flash_dev_param.PairedDev1Info, 2);
		PMU_NvmRead(7, &flash_dev_param.PairedDev2Info, 2);
		DBG("inf0 = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
		break;
#endif

#ifdef CFG_SOC_CALIBRATION_MODE1_EN
	case MSG_SOC_CAP_CAL_MODE:
		DBG("MSG_SOC_CAP_CAL_MODE\n");
		soc_cal_mode1set();
		break;
#endif
#ifdef CFG_FUNC_IRSEND_EN		
	case MSG_IR_SEND_LONG:
		DBG("IR SEND START\n");
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

	case MSG_FLASH_BANKB_UPDATA:
		DBG("双bank升级");
		if(ROM_BankBUpgradeApply(0,0) == 0)
		{
			DBG("发起请求成功");
			NVIC_SystemReset();
		}
		break;
#ifdef CFG_FUNC_2G4_OTA_RX_MASTER
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
		break;
#endif
#ifdef CFG_FCC_MODE
	case MSG_WIRELESS_FCC:
	fcc_mode_start();
	break;
#endif

#ifdef WIRELESS_TURNKEY8_1
	case MSG_WL_RS_SCH:
		DBG("RS SCH REQ\n");
		sch_status = MvWireless2AdvModeResourceSch();
		DBG("RS_SCH_STATUS:%d\n",sch_status);	//sch_status 0-- 抢麦失败  1--抢麦线路忙  2-- 抢麦进行中  3 -- 抢麦已成功
		break;

	case MSG_WL_RS_RELEASE:
		DBG("RS SCH RELEASE\n");
		MvWireless2AdvModeResourceRelease();
		break;
#endif

	default:
		break;
	}

#ifdef CFG_2T1R_SEND_MULTIBYTE_CMD_EN
	if(wireless_user_cmd_tx_result_get())
	{
		DBG("cmd send OK\n");
		wireless_user_cmd_tx_result_clean();
	}
#endif

#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX)
	{
		static uint32_t connect_state_back = 0;
		uint32_t connect_state = 0;
		connect_state = MvWireless2AdvModeGetConnState();
		if(connect_state != connect_state_back)
		{
			DBG("connet_state  %d\n", connect_state);
		
			if(connect_state == 1)
			{
		#ifndef CFG_AUTO_PAIRING_EN
				MvWireless2AdvModePairingScanEn(0);
		#endif
				DBG("Advmode RX pair scan ok\n");
			}

			connect_state_back = connect_state;
		}
	}
#endif

#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
	if(IsTimeOut(&TimerPaired))
	{
		int8_t rssi;
		TimeOutSet(&TimerPaired, 200);
		if(MVWIRE2_2T1R_Check_Requesting_Device(&ChipID, &rssi))
		{
			DBG("chip= %x, rssi= %d\n", ChipID,rssi);
			{
				if((ChipConnetFlag == 1) && (IsTimeOut(&TimerConnet)) && (ChipIDBack == ChipID))
				{
					ChipConnetFlag = 0;
					MVWIRE2_2T1R_Cancel_Pairing_Device();
					MVWIRE2_2T1R_Set_Allowed_Pairing_Device(ChipID);
					ChipID = 0;
					TimeOutSet(&TimerConnet, 2000);
				}
			}
		}
		else
		{
			ChipID = 0;
		}
	#ifdef CFG_LOCK_PAIRED_RX_EN
		if(IsTimeOut(&TimerConnet))
		{
			if(ChipID == flash_dev_param.PairedDev1Info)
			{
				ChipIDBack = ChipID;
				MVWIRE2_2T1R_Cancel_Pairing_Device();
				DBG("device1 ChipID  = %x, ChipIDBack = %x\n", ChipID, ChipIDBack);
				MVWIRE2_2T1R_Set_Allowed_Pairing_Device(ChipID);
				ChipID = 0;
				TimeOutSet(&TimerConnet, 2000);
				ChipConnetFlag = 1;
			}
			else if(ChipID == flash_dev_param.PairedDev2Info)
			{
				ChipIDBack = ChipID;
				MVWIRE2_2T1R_Cancel_Pairing_Device();
				MVWIRE2_2T1R_Set_Allowed_Pairing_Device(ChipID);
				ChipID = 0;
				TimeOutSet(&TimerConnet, 2000);
				ChipConnetFlag = 1;
			}
		}
	#endif
	}
#endif
}

//Porting from main()
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
//	PMU_NVMInit();//注意audio系统配置
	WirelessDeviceIdInit();
	RX_PairedDeviceInit();
#ifdef 	CFG_OPENDUT_MODE
	{
		unsigned char Nvm_readdata;
		extern void Wireless_DutInit(void);
		extern void Wireless_DutProcess(void);

		PMU_NvmRead(0,&Nvm_readdata,1);
		if( (Nvm_readdata&DUT_MODE_CMD)==DUT_MODE_CMD )
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

#ifdef CFG_FUNC_IRSEND_EN
	Ir_SendConfig(3,38000);
#endif
#if (defined(CFG_SOC_CALIBRATION_MODE2_EN))
	if(soc_cal_moderead() == soc_cal){
		soc_cal_mode2set();
	}
#endif

#if defined(CFG_RESOURCE_DIS) && (defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_3))
	audio_init_isready = 1;//Porting change
#elif (!defined(WIRELESS_TURNKEY3_1) && !defined(WIRELESS_TURNKEY3_3))
	audio_init_isready = 1;
#endif
#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
	TimeOutSet(&TimerPaired, 200);
	TimeOutSet(&TimerConnet, 2000);
#endif
#ifdef CFG_FUNC_2G4_OTA_EN
	TimeOutSet(&TimeOTA2G4, 100);
#endif
#if defined(CFG_FUNC_DEBUG_EN)//Porting care of
	LogTimerInit();
#endif
}

#endif //#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
