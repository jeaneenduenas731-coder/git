/**
 **************************************************************************************
 * @file    wireless_rx_main.c
 * @brief   Original name:wireless_main.c @wireless_mic_rx_sdk
 *
 * @author  KK
 * @version V1.0.0
 *
 * $Created: 2018-2-9 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


#include <string.h>
#include "type.h"
#include "gpio.h" //for BOARD
#include "debug.h"
#include "uarts.h"
#include "dma.h"
#include "timeout.h"
#include "irqn.h"
#include "clk.h"
#include "reset.h"
#include "wireless_bb_api.h"
#include "irqn.h"
#include "app_config.h"
#include "mcu_circular_buf.h"
#include "spi_flash.h"
#include "chip_info.h"
#include "audio_association.h"
#include "wireless2.h"
#include <sbc_encoder.h>

#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)

uint8_t wireless_link_status = 0xff;
uint8_t device_role = 0xff;
Rx_Flash_param_t flash_dev_param;

extern uint8_t SbcCnt_l_cur;
extern uint8_t SbcCnt_r_cur;

const uint32_t CompanyWord = (COMPANY_BYTE3<<24) | (COMPANY_BYTE2<<16) | (WIRELESS_LINK_KEY1<<8) | (WIRELESS_LINK_KEY0);
uint32_t WirelessDeviceId = ((~COMPANY_BYTE3)&0xff) | (((~COMPANY_BYTE2)&0xff)<<8) | (((~WIRELESS_LINK_KEY1)&0xff)<<16) | (((~WIRELESS_LINK_KEY0)&0xff)<<24);

#if defined(CFG_LOCK_PAIRED_TXRX) || defined(CFG_LOCK_PAIRED_RX_EN)
	uint8_t PairInfoUpdate = 0;//0:None; 1:write; 2：Earse
#endif

uint8_t		freqTrim_init = 0x13;

//0null  bit0 dev1 bit1 dev2
unsigned char Rx_PairedDeviceFlagGet(void);

void wireless_app_set_bd_addr(void);
void Audio_Check1stFrameAllRightCounterReset(unsigned char id);
void Audio_Check1stFrameAllRightCounterStart(unsigned char id);
unsigned char Audio_Check1stFrameAllRightStateGet(unsigned char id);
unsigned char Audio_Check1stFrameAllRight(unsigned char id,unsigned char cnt);

//void WirelessHoscInit(void)
//{
//	// extern uint8_t sync_state;
//	unsigned long get_reg = (*(volatile unsigned long *) 0x40021070);
//
//	// sync_state = 0;
//	get_reg &= ~(0x1f|(0x1f<<5));
//	get_reg |= (freqTrim_init|(freqTrim_init<<5));
//	(*(volatile unsigned long *) 0x40021070) = get_reg;
//}

void wireless_em_size_init(void)
{
	uint32_t wireless_em_mem;

	wireless_em_mem = wireless_em_size();
	APP_DBG("BB_EM_SIZE=%d,EM_WIRELESS_END=%lu\n", BB_EM_SIZE, wireless_em_mem);
	if(wireless_em_mem%4096)
	{
		wireless_em_mem = ((wireless_em_mem/4096)+1)*4096;
	}
	if(wireless_em_mem > BB_EM_SIZE)
	{
		APP_DBG("wireless em config error!\nyou must check BB_EM_SIZE\n%s%u \n",__FILE__,__LINE__);
		while(1);
	}
	else
	{
		APP_DBG("wireless em size:%uKB\n", (unsigned int)wireless_em_mem/1024);
	}
}

/***********************************************************************************
 * 配置BB的参数
 **********************************************************************************/
void ConfigWirelessBbParams(WirelessBbParams *params)
{
	if(params == NULL)
		return;

	memset(params, 0 ,sizeof(WirelessBbParams));

	//em config
	params->em_start_addr = BB_EM_START_PARAMS;

	//agc config
	params->pAgcDisable = 0; //0=auto agc;	1=close agc
	params->pAgcLevel = 1;

	//sniff config
	params->pSniffNego = 0;//1=open;  0=close
	params->pSniffDelay = 0;
	params->pSniffInterval = 0x320;//500ms
	params->pSniffAttempt = 0x01;
	params->pSniffTimeout = 0x01;

}

extern	uint8_t SilenceFrame[];
uint8_t rfsend_buffer[RFAUDIO_TRANS_LEN + 2];//Rx:Max:4-1=rf_translen + 1
Wireless2_param_t	wireless2_config = {
	.npack		 = RFPACK_NAUDIO,
	.rf_pbuffer  = rfsend_buffer,
	.rf_translen = RFAUDIO_TRANS_LEN,//RF_PACKET_LEN,
	.au_audiolen = SBC_DEC_LEN_PER_FREME - CRC_PACKSUB,

#if   (defined(WIRELESS_TURNKEY4_1))
	.sbc_len_backward = SBC_ENC_LEN_PER_FREME - CRC_PACKSUB,
#endif
#if   (defined(WIRELESS_TURNKEY1_4))
	.rf_interval = (11610),//(967.5*SWBB_FREQUENCY)
#endif
};

uint32_t	em_start_addr = BB_EM_START_PARAMS;
void WirelessInit(void)
{
	WirelessBbParams params;
#ifdef WIRELESS2_CONNWITH_B5_VER2_34_1
	extern unsigned char crc8_ex;
	crc8_ex = 0;
#endif
//	MVWIRE2_2T1R_Set_Chn_Select_Mode(1);//2T1R设置为全频段跳频
	//目前只有2-6支持AFH功能，带内开启AFH功能，带外关闭AFH
#if	!defined(WIRELESS_TURNKEY2_6)
	wireless2_set_afh(0);
#endif
#if	(defined(WIRELESS_TURNKEY2_6)||defined(WIRELESS_TURNKEY2_8))
	#ifdef 	WIRELESS_CONN_G1
	MVWIRE2_2T1R_Allowed_Conn_Chip(WIRELESS2_CHIP_TYPE_G1X);
	#endif
	#ifdef WIRELESS2WITHBTCHIP
	MVWIRE2_en_coex(2);
	#endif
	#ifdef CFG_LOCK_PAIRED_RX_ALLOWED
	MVWIRE2_2T1R_Disable_Auto_Conn_Mode();
	#endif
	if(get_OOB_band() == 1)//带外，默认关闭AFH
	{
		wireless2_set_afh(0);
	}
	#if !defined(WIRELESS_CONN_G1) && defined(WIRELESS_TURNKEY2_6)
	else
	{
		wireless2_set_afh(1);
	}
	#endif
#elif defined(WIRELESS_TURNKEY2_9)||defined(WIRELESS_TURNKEY3_5)||defined(WIRELESS_TURNKEY3_6)
	set_OOB_band(0);
#elif defined(WIRELESS_TURNKEY8_1)
	set_OOB_band(1);
#elif defined(WIRELESS_TURNKEY5_1)
	#ifdef CFG_SOC_CALIBRATION_EN
	if(((soc_cal_moderead() != soc_calstart) && (soc_cal_read() != soc_calstart))) 
	#endif
	{
		set_OOB_band(1);
	}
//	MVWIRE2_en_coex(2);//打开即2.2G
#elif defined(WIRELESS_TURNKEY4_1)

#endif
	wireless_em_size_init();
	ConfigWirelessBbParams(&params);
	MVWIRE2_ParamInit(&wireless2_config);
#ifndef CFG_RESOURCE_DIS
	#if	TURNKEY_2_X
		Audio_Check1stFrameAllRight_cb		       = Audio_Check1stFrameAllRight;
		Audio_Check1stFrameAllRightStateGet_cb	   = Audio_Check1stFrameAllRightStateGet;
		Audio_Check1stFrameAllRightCounterStart_cb = Audio_Check1stFrameAllRightCounterStart;
	#elif TURNKEY_3_X||\
		  defined(WIRELESS_TURNKEY5_1)||defined(WIRELESS_TURNKEY8_1)
		Audio_Check1stFrameAllRightCounterStart_cb = Audio_Check1stFrameAllRightCounterStart;
	#endif
#else
	Audio_Check1stFrameAllRight_cb = NULL;
	Audio_Check1stFrameAllRightStateGet_cb = NULL;
	Audio_Check1stFrameAllRightCounterStart_cb = NULL;
#endif //CFG_RESOURCE_DIS

#ifdef	CFG_LOCK_PAIRED_TXRX
	#if	defined(WIRELESS_TURNKEY1_4)
	extern void bi_1t1r_set_syncword_source(uint8_t source);
//0:1个TX可以在多个已经配对过的RX中切换连接，一次只能有一个TX和RX连接
//1:1个RX可以在多个已经配对过的TX中切换连接，一次只能有一个TX和RX连接
//2:1个TX只能和1个已经配对过的RX连接，TX和RX进入重新配对，记忆信息清除
	bi_1t1r_set_syncword_source(CFG_LOCK_PAIRED_MODE);
	#endif
#endif
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX)
	MvWireless2AdvModePairModeEn(1);
#endif

	memset((uint8_t*)BB_EM_MAP_ADDR, 0, BB_EM_SIZE);//clear em erea
	//sub_band_config(2);//配置频率范围段，对1-x有效
	//OOB_2G2_enable();
	Wireless_common_init(&params);

#if defined DEEP_SLEEP_MODE_EN && (defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_3))
	wireless2_Enable_Remote_Sleep_Cmd(1);
#else
	wireless2_Enable_Remote_Sleep_Cmd(0);
#endif
#ifdef	WIRELESS_EXTERNAL_PALNA_EN
	void enable_pa_lna(bool tx_en, bool rx_en);
	void MVWirless2_PaLnaEn(unsigned char tx_en, unsigned char rx_en);
	//Porting + wireless_
	wireless_enable_pa_lna(1, 1);
	MVWirless2_PaLnaEn(1,1);
#endif
}



unsigned char audio_init_isready = 0;//射频初始化很快，中断会提前进来，确保音频准备好了。

void MVWIRE2_ConnectedCB(unsigned char id,unsigned char role)
{

#ifdef PACKET_AUDIO_CH_BACKWARD
	if((device1.ConStatus == CONNECT_NONE)&&(device2.ConStatus == CONNECT_NONE))
	{
		Wireless_TransBufInit();
	}
#endif
	if((audio_init_isready)&&(id==0)&&(device1.ConStatus != CONNECT_AUDIO))
	{
		device1.ConStatus = CONNECT_AUDIO;
		device1.handle = 0x80;
#ifdef 	WIRELESS_TURNKEY2_6
		//音频库中抗抖动,这里2-6保证稳定
		device1.RecvNum = 30;
#else
		device1.RecvNum = 0;
#endif
#ifdef DEBUG_STATISTICS_PACKET
		device1.PlayFrame = 0;
		device1.PlcFrame = 0;
		device1.MuteFrame = 0;
		device1.RstNum = 0;
#endif
	}
	else if((audio_init_isready)&&(id==1)&&(device2.ConStatus != CONNECT_AUDIO))
	{
		device2.handle = 0x81;
		device2.ConStatus = CONNECT_AUDIO;
#ifdef 	WIRELESS_TURNKEY2_6
		//音频库中抗抖动,这里2-6保证稳定
		device2.RecvNum = 30;
#else
		device2.RecvNum = 0;
#endif
#ifdef 	DEBUG_STATISTICS_PACKET
		device2.PlayFrame = 0;
		device2.PlcFrame = 0;
		device2.MuteFrame = 0;
		device2.RstNum = 0;
#endif
	}
}

void MVWIRE2_DisconnectedCB(unsigned char id,unsigned char role)
{
	if((id==0)&&(device1.ConStatus != CONNECT_NONE))
	{
		device1.handle = 0xff;
		device1.ConStatus = CONNECT_NONE;
#if !defined(CFG_RESOURCE_DIS)
		Audio_Check1stFrameAllRightCounterReset(0);
#endif
		WirelessAudioDevice1RxSyncReset();
	}
	else if((id==1)&&(device2.ConStatus != CONNECT_NONE))
	{
		device2.handle = 0xff;
		device2.ConStatus = CONNECT_NONE;
#if !defined(CFG_RESOURCE_DIS)
		Audio_Check1stFrameAllRightCounterReset(1);
#endif
		WirelessAudioDevice2RxSyncReset();
	}
#ifdef PACKET_AUDIO_CH_BACKWARD
	if((device1.ConStatus == CONNECT_NONE)&&(device2.ConStatus == CONNECT_NONE))
	{
		Wireless_TransBufInit();
	}
#endif
}

char MVWIRE2_GetDevice1ConnState(void)
{
	return device1.ConStatus != CONNECT_NONE;
}

char MVWIRE2_GetDevice2ConnState(void)
{
	return device2.ConStatus != CONNECT_NONE;
}

void MVWIRE2_ConnStateDisplay(void)
{
	static char change_f1=0,change_f2=0;

	if(change_f1!=MVWIRE2_GetDevice1ConnState())
	{
		if(MVWIRE2_GetDevice1ConnState())
		{
			printf("device1_conn\r\n");
#ifdef	CFG_LOCK_PAIRED_RX_SAVE_NVM
			if(flash_dev_param.PairedDev2Info&0xFFFF == 0xFFFF)
			{
				uint16_t temp;
				PMU_NvmRead(7, &temp, 2);
				if(temp != 0xFFFF)
				{
					flash_dev_param.PairedDev2Info = temp;
					DBG("info = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
				}
			}

			if(flash_dev_param.PairedDev1Info == flash_dev_param.PairedDev2Info)
			{
				PMU_NvmRead(5, &flash_dev_param.PairedDev2Info, 2);
				DBG("LR change, device1= %x, device2=%x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
				PMU_NvmWrite(7, &flash_dev_param.PairedDev2Info, 2);
			}

			PMU_NvmWrite(5, &flash_dev_param.PairedDev1Info, 2);
#endif
			DBG("PairedInfo = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
		}
		else
		{
			printf("device1_disconn\r\n");
#ifdef	CFG_LOCK_PAIRED_RX_SAVE_NVM
			if(flash_dev_param.PairedDev1Info&0xFFFF == 0xFFFF)
			{
				flash_dev_param.PairedDev1Info = 0;
				PMU_NvmRead(5, &flash_dev_param.PairedDev1Info, 2);
			}
#endif
			DBG("info = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
		}
		change_f1 = MVWIRE2_GetDevice1ConnState();
	}

	if(change_f2!=MVWIRE2_GetDevice2ConnState())
	{
		if(MVWIRE2_GetDevice2ConnState())
		{
			printf("device2_conn\r\n");
#ifdef	CFG_LOCK_PAIRED_RX_SAVE_NVM
			if(flash_dev_param.PairedDev1Info&0xFFFF == 0xFFFF)
			{
				uint16_t temp;
				PMU_NvmRead(5, &temp, 2);
				if(temp != 0xFFFF)
				{
					flash_dev_param.PairedDev1Info = temp;
				}
			}
			if(flash_dev_param.PairedDev1Info == flash_dev_param.PairedDev2Info)
			{
				PMU_NvmRead(7, &flash_dev_param.PairedDev1Info, 2);
				DBG("LR change22, device1= %x, device2=%x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
				PMU_NvmWrite(5, &flash_dev_param.PairedDev1Info, 2);
			}

			PMU_NvmWrite(7, &flash_dev_param.PairedDev2Info, 2);
#endif
			DBG("info = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);

		}
		else
		{
			printf("device2_disconn\r\n");
#ifdef	CFG_LOCK_PAIRED_RX_SAVE_NVM
			if(flash_dev_param.PairedDev2Info&0xFFFF == 0xFFFF)
			{
				flash_dev_param.PairedDev2Info = 0;
				PMU_NvmRead(7, &flash_dev_param.PairedDev2Info, 2);
			}
#endif
			DBG("info = %x, %x\n", flash_dev_param.PairedDev1Info, flash_dev_param.PairedDev2Info);
		}
		change_f2 = MVWIRE2_GetDevice2ConnState();
	}
}

//5-1需要配对芯片型号
//为了只写flash一次，底层先调用RX_PairedChipTypeSet，再调用RX_PairedInfoSet
uint8_t RX_PairChipTypeGet(unsigned char Device)
{
	return flash_dev_param.PairedDev1Type;
}
__attribute__((section(".tcm_section")))
bool RX_PairedChipTypeSet(uint8_t chipType, unsigned char Device)
{
	if(flash_dev_param.PairedDev1Type != chipType)
	{
//		if(flash_dev_param.PairedDev1Info != 0xFF)
//		{
//			NeedErase = TRUE;
//		}
		flash_dev_param.PairedDev1Type = chipType;
	}
}

//bit0：dev1 paired; bit1: dev2 paired @ DEVICE1_MASK DEVICE2_MASK
unsigned char RX_PairedDeviceFlagGet(void)
{
	unsigned char DeviceMask = 0;

	if(flash_dev_param.PairedDev1Info != NOPAIR_WORD)
		DeviceMask |= DEVICE1_MASK;
#if	TURNKEY_2_X
	if(flash_dev_param.PairedDev2Info != NOPAIR_WORD)
		DeviceMask |= DEVICE2_MASK;
#endif
	 return DeviceMask;
}

uint32_t RX_PairInfoGet(unsigned char Device)
{
	if(Device & DEVICE1_MASK && RX_PairedDeviceFlagGet() & DEVICE1_MASK)
	{
		return flash_dev_param.PairedDev1Info;
	}
#if	TURNKEY_2_X
	else if(Device & DEVICE2_MASK)
	{
		return flash_dev_param.PairedDev2Info;
	}
#endif
	else
	{
		return NOPAIR_WORD;
	}
}

bool RX_PairedInfoSetRematch(uint32_t Info, unsigned char Device)
{
	if(Device & DEVICE1_MASK)
	{
		if(flash_dev_param.PairedDev1Info != Info)
		{
			flash_dev_param.PairedDev1Info = Info;
		}
	}
	if(Device & DEVICE2_MASK)
	{
		if(flash_dev_param.PairedDev2Info != Info)
		{
			flash_dev_param.PairedDev2Info = Info;
		}
	}
#if (defined(CFG_LOCK_PAIRED_TXRX) || defined(CFG_LOCK_PAIRED_RX_EN)) && !defined(CFG_LOCK_PAIRED_RX_SAVE_NVM)
	SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
	SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
#endif
	return TRUE;
}

bool RX_PairedInfoSet(uint32_t Info, unsigned char Device)
{
	bool NeedErase = FALSE;
#if defined(CFG_LOCK_PAIRED_TXRX)\
	||TURNKEY_2_X \
	||defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_3)
	if(Device & DEVICE1_MASK)
	{
		if(flash_dev_param.PairedDev1Info != Info)
		{
			if(flash_dev_param.PairedDev1Info != NOPAIR_WORD)
			{
				NeedErase = TRUE;
#if defined(CFG_LOCK_PAIRED_RX_EN) && !defined(CFG_LOCK_PAIRED_RX_SAVE_NVM)
				if(flash_dev_param.PairedDev2Info == Info)
				{
					flash_dev_param.PairedDev2Info = flash_dev_param.PairedDev1Info;
					DBG("LR  change\n");
				}
#endif
			}
			flash_dev_param.PairedDev1Info = Info;
		}
	}
#endif
#if	TURNKEY_2_X
	if(Device & DEVICE2_MASK)
	{
		if(flash_dev_param.PairedDev2Info != Info)
		{
			if(flash_dev_param.PairedDev2Info != NOPAIR_WORD)
			{
				NeedErase = TRUE;
			}
			flash_dev_param.PairedDev2Info = Info;
		}
	}
#endif

#if defined(CFG_LOCK_PAIRED_TXRX) || defined(CFG_LOCK_PAIRED_RX_EN)
	if(NeedErase)
	{
		PairInfoUpdate = 2;
	}
	else
		PairInfoUpdate = 1;

#ifdef CFG_LOCK_PAIRED_RX_SAVE_NVM
	return FALSE;
#else
	return TRUE;
#endif
#else
	return FALSE;
#endif
}

void RX_PairedDeviceInit(void)
{
	PairedFlagGetFunc = RX_PairedDeviceFlagGet;
	PairInfoGetFunc = RX_PairInfoGet;
	
#ifdef WIRELESS_TURNKEY8_1
	PairedInfoSetFunc = RX_PairedInfoSetRematch;
#else
	PairedInfoSetFunc = RX_PairedInfoSet;
#endif

#ifdef WIRELESS_TURNKEY5_1
	PairChipTpyeGetFunc = RX_PairChipTypeGet;
	PairedChipTpyeSetFunc = RX_PairedChipTypeSet;
#endif
#if defined(CFG_LOCK_PAIRED_TXRX) || defined(CFG_LOCK_PAIRED_RX_EN)
	SpiFlashRead(CONINF_FLASH_ADDR,(uint8_t*)&flash_dev_param,sizeof(flash_dev_param),100);
#ifdef CFG_LOCK_PAIRED_RX_SAVE_NVM
	PMU_NvmWrite(5, &flash_dev_param.PairedDev1Info, 2);
	PMU_NvmWrite(7, &flash_dev_param.PairedDev2Info, 2);
#endif
#else
	flash_dev_param.PairedDev1Info = NOPAIR_WORD;
	#if	defined(WIRELESS_TURNKEY2_5) || defined(WIRELESS_TURNKEY2_6) ||\
		defined(WIRELESS_TURNKEY2_8) || defined(WIRELESS_TURNKEY2_9)
		flash_dev_param.PairedDev2Info = NOPAIR_WORD;
	#endif
#endif
	if(RX_PairedDeviceFlagGet() & DEVICE1_MASK)
	{
#ifdef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
		printf("Paired 0x%08x\n", flash_dev_param.PairedDev1Info);
		flash_dev_param.PairedDev1Info &= ~0x8000;
#endif
		printf("Paired 0x%08x\n", flash_dev_param.PairedDev1Info);
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX) && !defined(CFG_AUTO_PAIRING_EN)
		MvWireless2AdvModePairingScanEn(0);
#endif
	}
	else
	{
#if	CFG_PAIRING_SUPPORTMDOE && defined(CFG_LOCK_PAIRED_TXRX) && !defined(CFG_AUTO_PAIRING_EN)
		MvWireless2AdvModePairingScanEn(1);
#endif
		printf("Null device1 paired!\n");
	}
#if	TURNKEY_2_X
	if(RX_PairedDeviceFlagGet() & DEVICE2_MASK)
	{
		printf("Paired device2 0x%08x\n", flash_dev_param.PairedDev2Info);
	}
	else
	{
		printf("Null device2 paired!\n");
	}
#endif
}


#if defined(CFG_LOCK_PAIRED_TXRX) || defined(CFG_LOCK_PAIRED_RX_EN)
void RX_PairedInfoSetFlash(void)
{
	if(PairInfoUpdate == 2)
	{
		PairInfoUpdate--;
	#ifndef CFG_LOCK_PAIRED_RX_SAVE_NVM
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
	#endif
	}
	else if(PairInfoUpdate == 1)
	{
	#ifndef CFG_LOCK_PAIRED_RX_SAVE_NVM
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
	#endif
		PairInfoUpdate--;
		#if defined(WIRELESS_TURNKEY5_1)
		extern void uni_1t1r51_set_paired_and_flashStored();
		uni_1t1r51_set_paired_and_flashStored();
		#endif
	}
}
#endif

void WirelessDeviceIdInit(void)
{
	unsigned long long ChipID;

	Chip_IDGet(&ChipID);
	ChipID = (ChipID>>32)&0xffffffff;

	DBG("ChipID:0x%08llx\n", ChipID);
	if(ChipID != 0 && ChipID != 0xffffffff)
	{
		WirelessDeviceId = ChipID;
	}
	//根据ID随机延时，防止多设备同时启动撞时序
	DelayMs(WirelessDeviceId&0xff);
}

unsigned int wireless2_GetSleepMode(void)
{
	return wireless2_GetSleep();
}

void wireless2_ModeSleep()
{
	wireless2_ModeSleep();
}

void wireless2_ModeActive(void)
{
	wireless2_Active();
}
unsigned int wireless2_GetRfStopMode(void)
{
	return wireless2_CheckStopped();
}

void wireless2_SetRfStopMode(void)
{
	wireless2_RfStop();
	while(!wireless2_CheckStopped())
	{
		WDG_Feed();
	}
}
void wireless2_ClrRfStopMode(void)
{
	MVWIRE2_DeviceRoleSet(WIRELESS_SDK_ROLE);
	wireless2_Restart();
}

void WirelessInfo(void)
{
#ifdef DEBUG_WIRELESS_EN
	MVWIRE2_DBG_FUNC();
#endif

	MVWIRE2_ConnStateDisplay();
#ifdef WIRELESS_TURNKEY8_1
	uint8_t role;
	uint8_t state;
	static uint8_t last_state = 0xFF;
	static uint32_t last_recv_cnt = 0;
	if(MvWireless2AdvModeGetRemoteCloseCmdState())	//debug by xiaofeng
	{
		printf("Remote Close\n");
	}

	if(MvWireless2AdvModeGetWorkState(&role,&state))
	{
		if(last_state != state)
		{
			last_state = state;
			printf("Role:%d,State:%d\n",role,state);
		}
	}

	extern uint32_t slave1_total_recv_pkt;
	extern uint32_t slave1_syncerr_cnt;
	extern uint32_t slave1_pkt_num_err;
	if((slave1_total_recv_pkt !=0) && ((slave1_total_recv_pkt % 4000) == 0) && (last_recv_cnt != slave1_total_recv_pkt))
	{
		last_recv_cnt = slave1_total_recv_pkt;
		printf("recv_total:%d,sync_err:%d,crc_err:%d\n",slave1_total_recv_pkt,slave1_syncerr_cnt,slave1_pkt_num_err);
	}

#endif
	return;
}
#endif //defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
