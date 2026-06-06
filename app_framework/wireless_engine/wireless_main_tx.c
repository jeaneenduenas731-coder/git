/**
 **************************************************************************************
 * @file    wireless_main.c
 * @brief   
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
#include "spi_flash.h"
#include "chip_info.h"
#include "audio_association.h"
//#include "wireless_usr_api.h"
#include "wireless2.h"
#include "sbc_encoder.h"
//#include "audio_main.h"

#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
Tx_Flash_param_t flash_dev_param;

bool IsWirelessConnectFail = 0;
char switch_ok = 0;
uint8_t wireless_link_status = 0xff;
uint8_t device_role = 0xff;
extern uint8_t sbcCnt;

extern uint8_t SbcCnt_l_cur;

const uint32_t CompanyWord = (COMPANY_BYTE3<<24) | (COMPANY_BYTE2<<16) | (WIRELESS_LINK_KEY1<<8) | (WIRELESS_LINK_KEY0);
uint32_t WirelessDeviceId = (COMPANY_BYTE2) | (COMPANY_BYTE3<<8) | (WIRELESS_LINK_KEY1<<16) | (WIRELESS_LINK_KEY0<<24);

#ifdef CFG_LOCK_PAIRED_TXRX
	uint8_t PairInfoUpdate = 0;//0:None; 1:write; 2：Earse
#endif

uint8_t		freqTrim_init = 0x13;

unsigned char TX_FlashDevNotNull(void);//1 have 0 null
void wireless_AudioParityCntReset();
void wireless_AudioParityCntStart();
unsigned char wireless_AudioParityCntGet();
void wireless_AudioParityCntProc();
void MVWIRE2_2T1R_ClrRecvRSSI(void);
void MVWIRE2_2T1R_SetPairRSSIEn(uint8_t set);

//char  connect_Timer_en = 0;
//TIMER connect_Timer_t;
//void connect_TimerStart()
//{
//	connect_Timer_en = 1;
//	TimeOutSet(&connect_Timer_t,2000);
//}
//void connect_TimerEnd()
//{
//	connect_Timer_en = 0;
//}
//
//void connect_TimerProcess()
//{
//	if((connect_Timer_en)&&(IsTimeOut(&connect_Timer_t)))
//	{
//		connect_TimerEnd();
//		if(device1.connected)
//			lc_detach(device1.handle&0xf,0x22);
//		printf("hande sh err!!!\r\n");
//	}
//}

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
void wireless_app_set_bd_addr(void);

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

unsigned char audio_init_isready = 0;//射频初始化很快，中断会提前进来，确保音频准备好了。
void MVWIRE2_ConnectedCB(unsigned char id,unsigned char role)
{
	if((audio_init_isready)&&(id==0)&&(device1.ConStatus != CONNECT_AUDIO))
	{
		device1.ConStatus = CONNECT_AUDIO;
		device1.handle = 0x80;
		device1.RecvNum = 0;

		Wireless_TransBufInit();
#ifdef DEBUG_STATISTICS_PACKET
		device2.PlayFrame = 0;
		device2.PlcFrame = 0;
		device2.MuteFrame = 0;
		device2.RstNum = 0;
#endif
#if	defined(WIRELESS_TURNKEY5_2) || defined(WIRELESS_TURNKEY5_1)
		wireless_AudioParityCntReset();
#endif
	}
}

void MVWIRE2_DisconnectedCB(unsigned char id,unsigned char role)
{
	if((id==0)&&(device1.ConStatus != CONNECT_NONE))
	{
		device1.handle = HANDLE_NONE;
		device1.ConStatus = CONNECT_NONE;
#ifdef PACKET_AUDIO_CH_BACKWARD
		WirelessAudioDevice1RxSyncReset();
#endif
		wireless_AudioParityCntReset();
		Wireless_TransBufInit();
	}
}


char MVWIRE2_GetDevice1ConnState(void)
{
	return device1.ConStatus != CONNECT_NONE;
}


char MVWIRE2_GetDevice2ConnState(void)
{
	return FALSE;
}

void MVWIRE2_ConnStateDisplay(void)
{
	static char change_f1=0;
	if(change_f1!=MVWIRE2_GetDevice1ConnState())
	{
		if(MVWIRE2_GetDevice1ConnState())
		{
			printf("device1_conn\r\n");
#ifdef CFG_LOCK_PAIRED_TX_ONLY
			TX_PairedInfoSetFlash_TXOnly();
#endif
		}
		else
		{
			printf("device1_disconn\r\n");
		}
		change_f1 = MVWIRE2_GetDevice1ConnState();
	}
}

extern	uint8_t SilenceFrame[];
uint8_t rfsend_buffer[RFAUDIO_TRANS_LEN + 23];//tx:Max:6-1=rf_translen+23
Wireless2_param_t	wireless2_config = {
	.npack		 = RFPACK_NAUDIO,
	.rf_pbuffer  = rfsend_buffer,
	.rf_translen = RFAUDIO_TRANS_LEN,//RF_PACKET_LEN,
	.au_audiolen = SBC_ENC_LEN_PER_FREME - CRC_PACKSUB,

#if   (defined(WIRELESS_TURNKEY4_1))
	.sbc_len_backward = SBC_DEC_LEN_PER_FREME - CRC_PACKSUB,
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

#if	defined(WIRELESS_TURNKEY2_6) || defined(WIRELESS_TURNKEY2_8)
	#ifdef WIRELESS2WITHBTCHIP
	MVWIRE2_en_coex(2);
	#endif
	#ifdef 	WIRELESS_CONN_G1
	MVWIRE2_2T1R_Allowed_Conn_Chip(WIRELESS2_CHIP_TYPE_G1X);
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
#elif defined(WIRELESS_TURNKEY5_1)
	#ifdef CFG_SOC_CALIBRATION_EN
	if(((soc_cal_moderead() != soc_calstart) && (soc_cal_read() != soc_calstart)))
	#endif
	{
		set_OOB_band(1);
	}
//	uni_1t1r51_dbg_en_set(1);
//	MVWIRE2_en_coex(2);
#elif defined(WIRELESS_TURNKEY4_1)

#endif

	wireless_em_size_init();
	ConfigWirelessBbParams(&params);
	MVWIRE2_ParamInit(&wireless2_config);
#if	TURNKEY_2_X ||\
	defined(WIRELESS_TURNKEY5_2)||defined(WIRELESS_TURNKEY5_1)
	wireless_AudioParityCntProc_cb = wireless_AudioParityCntProc;
	wireless_AudioParityCntStart_cb = wireless_AudioParityCntStart;
#endif

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

#ifdef	WIRELESS_EXTERNAL_PALNA_EN
	void enable_pa_lna(bool tx_en, bool rx_en);
	void MVWirless2_PaLnaEn(unsigned char tx_en, unsigned char rx_en);
	//Porting + wireless_
	wireless_enable_pa_lna(1, 1);
	MVWirless2_PaLnaEn(1,1);
#endif
}


//5-1需要配对芯片型号
//为了只写flash一次，底层先调用RX_PairedChipTypeSet，再调用RX_PairedInfoSet
uint8_t TX_PairChipTypeGet(unsigned char Device)
{
	return flash_dev_param.PairedDev1Type;
}
__attribute__((section(".tcm_section")))
bool TX_PairedChipTypeSet(uint8_t chipType, unsigned char Device)
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

//1: paired; 0: unpair
unsigned char TX_PairedDeviceFlagGet(void)
{
	return flash_dev_param.PairedDevInfo != NOPAIR_WORD;
}

uint32_t TX_PairInfoGet(unsigned char Device)
{
	return flash_dev_param.PairedDevInfo;
}

bool TX_PairedInfoSet(uint32_t Info, unsigned char Device)
{
	if(flash_dev_param.PairedDevInfo != Info)
	{
#ifdef CFG_LOCK_PAIRED_TXRX
		if(flash_dev_param.PairedDevInfo != NOPAIR_WORD)
		{
			PairInfoUpdate = 2;
		}
		else
			PairInfoUpdate = 1;
#endif
#if defined(CFG_LOCK_PAIRED_TXRX)||\
	TURNKEY_2_X || TURNKEY_3_X
		flash_dev_param.PairedDevInfo = Info;
#endif
	}
#ifdef CFG_LOCK_PAIRED_TXRX
	return TRUE;
#else
	return FALSE;
#endif
}

#ifdef CFG_LOCK_PAIRED_TX_ONLY
extern uint16_t get_RemoteID;
extern uint16_t slave_2t1r_flash_get_m_chip_id(void);
void TX_PairedDeviceInit_TXOnly(void)
{
	get_RemoteID = 0xFFFF;
	SpiFlashRead(CONINF_FLASH_ADDR, (uint8_t*)&get_RemoteID, 2, 100);

#ifdef	CFG_GET_RX_RSSI
	{
		if(get_RemoteID == 0xffff)
			get_RemoteID = 0xfff1;
		MVWIRE2_2T1R_SetPairRSSIEn(1);
		MVWIRE2_2T1R_ClrRecvRSSI();
	}
#endif

	DBG("get_RemoteID = %x\n", get_RemoteID);
}
void TX_PairedInfoSetFlash_TXOnly(void)
{
	uint16_t Temp;
	SpiFlashRead(CONINF_FLASH_ADDR, (uint8_t*)&get_RemoteID, 2, 100);
	Temp = slave_2t1r_flash_get_m_chip_id();
	DBG("chip_id = %x, RemoteID = %x\n", Temp, Temp);

	if(get_RemoteID == 0xFFFF)
	{
		DBG("save Paired info\n");
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&Temp, 2, 1);
		get_RemoteID = Temp;
	}
}
#endif
void TX_PairedDeviceInit(void)
{
	PairedFlagGetFunc = TX_PairedDeviceFlagGet;
	PairInfoGetFunc = TX_PairInfoGet;
	PairedInfoSetFunc = TX_PairedInfoSet;
#ifdef WIRELESS_TURNKEY5_1
	PairChipTpyeGetFunc = TX_PairChipTypeGet;
	PairedChipTpyeSetFunc = TX_PairedChipTypeSet;
#endif
#ifdef CFG_LOCK_PAIRED_TXRX
	SpiFlashRead(CONINF_FLASH_ADDR,(uint8_t*)&flash_dev_param,sizeof(flash_dev_param),100);
#else
	flash_dev_param.PairedDevInfo = NOPAIR_WORD;
#endif

	if(TX_PairedDeviceFlagGet())
	{
		printf("Paired 0x%08x\n", flash_dev_param.PairedDevInfo);
		printf("chip type 0x%08x\n", flash_dev_param.PairedDev1Type);
	}
	else
	{
		printf("Null device paired!\n");
	}
}

void TX_PairedClearRematch(void)
{
	if(flash_dev_param.PairedDevInfo != NOPAIR_WORD)
	{
		flash_dev_param.PairedDevInfo = NOPAIR_WORD;
	}
#if defined(CFG_LOCK_PAIRED_TX_ONLY) || defined(CFG_LOCK_PAIRED_TX_ONLY)
	SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
	SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
#endif
}

void TX_PairedClear(void)
{
#ifdef CFG_LOCK_PAIRED_TX_ONLY
	if(get_RemoteID != 0xFFFF)
	{
		get_RemoteID = 0xFFFF;
		flash_dev_param.PairedDevInfo = NOPAIR_WORD;
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
	}
#elif defined(CFG_LOCK_PAIRED_TXRX)
	if(flash_dev_param.PairedDevInfo != NOPAIR_WORD)
	{
		flash_dev_param.PairedDevInfo = NOPAIR_WORD;
		PairInfoUpdate = 2;
	}
#else
	flash_dev_param.PairedDevInfo = NOPAIR_WORD;
#endif
}

#ifdef CFG_LOCK_PAIRED_TXRX
void TX_PairedInfoSetFlash(void)
{
#ifdef CFG_FUNC_PAIRED_IR_HIGH_PRIORITY
	SpiFlashRead(CONINF_FLASH_ADDR,(uint8_t*)&flash_dev_param,sizeof(flash_dev_param),100);
	if(flash_dev_param.PairedDevInfo != NOPAIR_WORD)
	{
		return;//防止多次写，对于3-1红外配对需求，不需要写flash，只要flash中有值即可
	}
#endif
	if(PairInfoUpdate == 2)
	{
		PairInfoUpdate--;
		SpiFlashErase(SECTOR_ERASE, CONINF_FLASH_ADDR/4096, 1);
	}
	else if(PairInfoUpdate == 1)
	{
		SpiFlashWrite(CONINF_FLASH_ADDR, (uint8_t*)&flash_dev_param, sizeof(flash_dev_param),1);
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

void wireless2_ModeSleep(void)
{
	wireless2_Sleep();
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

#ifdef	CFG_GET_RX_RSSI
void get_RemoteRxRssiTbl(void)
{
	static TIMER dis_cnt;
	unsigned char test_RSSI;
	unsigned short ID;

	extern uint16_t get_RemoteID;
	extern void MVWIRE2_2T1R_GetRecvRSSI(uint8_t cnt,uint8_t *rssi,uint16_t *id);

	if(IsTimeOut(&dis_cnt)&&((device1.ConStatus == CONNECT_NONE)))
	{
		unsigned char runloop_cnt = 0;

		TimeOutSet(&dis_cnt,2000);

		for(runloop_cnt=0;runloop_cnt<5;runloop_cnt++)
		{
			MVWIRE2_2T1R_GetRecvRSSI(runloop_cnt,&test_RSSI,&ID);
			printf("%d [0x%04x]  %d\r\n",runloop_cnt,ID,test_RSSI);
			if((test_RSSI<50)&&(get_RemoteID != ID))
			{
				if(wireless2_GetRfStopMode()==0)
				{
					wireless2_SetRfStopMode();
					MVWIRE2_2T1R_ClrRecvRSSI();
					get_RemoteID = ID;
					wireless2_ClrRfStopMode();
				}
				TimeOutSet(&dis_cnt,5000);
				break;
			}
		}
	}
}
#endif

void MVWIRE2_slave_schedule(void);
void WirelessInfo(void)
{
#if defined(WIRELESS_TURNKEY1_4)
	MVWIRE2_slave_schedule();
#endif
#ifdef DEBUG_WIRELESS_EN
	MVWIRE2_DBG_FUNC();
#endif
	MVWIRE2_ConnStateDisplay();
#ifdef	CFG_GET_RX_RSSI
	get_RemoteRxRssiTbl();
#endif
	return;
}
#endif //defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
