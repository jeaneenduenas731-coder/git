/*
 * wireless_audio_main_rx.c
 *@brief   Original name:audio_main.c @wireless_mic_rx_sdk
 *
 *  Created on: Dec 6, 2024
 *      Author: piwang
 */
#include "app_config.h"
#include "audio_core_api.h"
#include "audio_core_adapt.h"
#include "audio_association.h"
#include "sbc_encoder.h"
#include "mvintrinsics.h"
#include "sbcenc_api.h"
#include "log_info.h"

#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
/***************Audio Sync*********************/
void AudioOutSyncChange(uint8_t Device);
#if defined(DECODE_CH) && DECODE_CH != 0
	uint8_t SilenceFrame[] = SBC_SIL_CH_FRAME_SAMPLERATE_QUALITY;
#endif

unsigned char    sbc_buf_f[SBC_DEC_LEN_PER_FREME * 2+6];
short	         MicLeftPcmFifo_f[SBC_DEC_LEN_PER_FREME * 2 * 20];
#if DECODE_CH == 2
	short			 MicRightPcmFifo_f[SBC_DEC_LEN_PER_FREME * 2 * 1];
#elif DECODE_CH == 1
	short            MicRightPcmFifo_f[SBC_DEC_LEN_PER_FREME * 2 * 20];
#endif
#if defined(DECODE_CH) && DECODE_CH!= 0
	Rx_AudioAssociation_param_t wirelessaudio_config={
	#if DECODE_CH == 2
		.FrameGroups       = STEREO_ONE_DEVICE,
	#elif DECODE_CH == 1
		.FrameGroups       = STEREO_TWO_DEVICE,
	#endif
		.SilencePack       = SilenceFrame,

		.wireless_frames   = WIRELESS_RECV_FIFO_THRHLD,
		.AddHeaderLen	   = CRC_PACKSUB,
		.pack_info_len     = PACKET_CNT_LEN,

		.frame_len		   = ONE_FRAME,

		.Encoder_out_len   = SBC_DEC_LEN_PER_FREME,			//sizeof(SilenceFrame),
		.sbc_buf_p         = sbc_buf_f,
		.MicLeftPcmFifo_p  = MicLeftPcmFifo_f,
		.MicRightPcmFifo_p = MicRightPcmFifo_f,
	#ifdef LOG_ASSOCIATION_EN
		.LogEnable = TRUE,
	#else
		.LogEnable = FALSE,
	#endif
	};
#endif

int16_t PcmBuf[ONE_FRAME * 2];//Porting to Stereo
int16_t PcmBuf2[ONE_FRAME];//Porting to mono
uint8_t WirelessinFrameRead = ONE_FRAME;//音频通路帧与Wireless帧可能不对齐，数据残留PcmBuf, 初始指向帧末尾ONE_FRAME

#ifdef CRC_MULTIPLE
	const uint8_t is_multi_crc = TRUE;
#else
	const uint8_t is_multi_crc = FALSE;
#endif

static uint16_t AudioOutDelete = 0;//删点缩短Dac延时

//0--without sync	1--start	2--sync
static unsigned char Device0_syncpackallright,Device1_syncpackallright;
static unsigned char device0_hiscnt = 0,device1_hiscnt = 0;
unsigned int Device0_2rdPackSample,Device1_2rdPackSample;

//源自audio_association.c
#define PACKET_STABLE_THRHLD_RF	20
static unsigned char rf0_packcnt = 0, rf1_packcnt = 0;//防止rf不稳定,先接收20包再给APP
#if (defined(WIRELESS_TURNKEY2_9))
	#define INTERVAL_NRF	(3)
#else
	#define INTERVAL_NRF	(2)
#endif

uint32_t TargetVal = 0;
void AudioClkRetarget(void)
{
	uint32_t temp1;
	float temp2;

#if defined(WIRELESS_TURNKEY1_4)||TURNKEY_2_X ||\
	TURNKEY_3_X ||\
	defined(WIRELESS_TURNKEY4_1)||\
	defined(WIRELESS_TURNKEY6_1)||\
	defined(WIRELESS_TURNKEY5_2)||defined(WIRELESS_TURNKEY5_1)
	temp1  = 21<<16;
	(void)temp2;
#else
	(void)temp2;
	return;
#endif
#if defined(WIRELESS_TURNKEY1_4)||\
	TURNKEY_2_X ||\
	TURNKEY_3_X ||\
	defined(WIRELESS_TURNKEY4_1)||\
	defined(WIRELESS_TURNKEY6_1)||\
	defined(WIRELESS_TURNKEY5_2)||defined(WIRELESS_TURNKEY5_1)
	temp1 += 16944;
#else
	temp1 += (temp2 * 65536);
#endif
	(*(volatile unsigned long *) 0x40021054) = temp1;
	TargetVal = temp1;
}

/**避免切模式重配Mclk造成频偏丢包*/
void TargetLock(void)
{
	if(TargetVal != 0)
		(*(volatile unsigned long *) 0x40021054) = TargetVal;
}

void wireless_audio_init_rx(void)
{
#ifdef	BPLC_EN
	AudioPlc_2_Init(SAMPLE_RATE,ONE_FRAME);
#elif (ONE_FRAME==128)
	AudioPlc128Init();
#elif (ONE_FRAME==96)
	AudioPlc96Init();
#elif (ONE_FRAME==64)
	AudioPlc64Init();
#endif
#if  !defined(CFG_RESOURCE_DIS)
	#if	TURNKEY_2_X||TURNKEY_3_X
		wirelessaudio_config.Wireless2_2T1RDelaySyncResetFunc_t = Audio_Check1stFrameAllRightCounterResetForAudioAssociation,
	#elif (!defined(WIRELESS_TURNKEY5_1))
		wirelessaudio_config.RemainFunc = AUDIOOUT_STEP_LEN_FUNC;
		wirelessaudio_config.AudioOutSyncFunc = AudioOutSyncChange;
	#endif
#endif // CFG_RESOURCE_DIS
	wirelessaudio_config.MicLeftPcmFifo_len_set  = sizeof(MicLeftPcmFifo_f);
	wirelessaudio_config.MicRightPcmFifo_len_set = sizeof(MicRightPcmFifo_f);

#ifdef PACKET_AUDIO_CH_BACKWARD
	audio_wireless_init();
#endif
	if(SilenceFrame==NULL)
		DBG("Decoder cant find!!!\r\n");
	extern uint8_t syncdevice2_thold;
#if	TURNKEY_2_X
	{
		syncdevice2_thold = 1;
	}
#elif defined(WIRELESS_RECV_FIFO_THRHLD)
	if(wirelessaudio_config.FrameGroups == STEREO_TWO_DEVICE)
	{
		syncdevice2_thold = RFPACK_NAUDIO + 1;
	}
#endif
	AudioAssociationInit(&wirelessaudio_config);
#if	SAMPLE_RATE == 44100
	AudioClkRetarget();
#endif
}

/*******************************wireless in to audio Dac**************************************/
void wireless_audio_in_process(void)
{
//	uint32_t i;
	uint8_t	 frame_size_cnt = 1;
#ifndef CFG_RESOURCE_DIS
#if defined(WIRELESS_TURNKEY3_1) || defined(WIRELESS_TURNKEY3_3)
	if((audio_init_isready==0) && (AudioDAC0_DataLenGet() >= (AUDIOOUT_DELAY_PRESET)))
	{
		extern unsigned char audio_init_isready;
		audio_init_isready = 1;
	}
#endif
#endif

	if(WirelessInDataLenGet() < SOURCEFRAME(APP_SOURCE_NUM))
	{
#if !defined(CFG_RESOURCE_DIS)//#if	TURNKEY_2_X
		{
			static unsigned char audio_1st_data = 0;
			//争取第一时间开始计数器
			if((Audio_Check1stFrameAllRightStateGet(0) != 2) &&
			   (Audio_Check1stFrameAllRightStateGet(1) != 2))
			{
				audio_1st_data = 0;
			}

			if((audio_1st_data==0) && ((Audio_Check1stFrameAllRightStateGet(0) == 2) ||
									  (Audio_Check1stFrameAllRightStateGet(1)==2) )
			   )
			{
				//DAC这个位置正好是rf第二包完成
				unsigned char start_sampleset;// = ((ONE_FRAME*RFPACK_NAUDIO)*3/2)-ONE_FRAME-10;

				audio_1st_data = 1;
				if((Audio_Check1stFrameAllRightStateGet(0) == 2))
				{
					AudioOutDelete = (Device0_2rdPackSample%ONE_FRAME);
#if	defined(WIRELESS_TURNKEY2_6)
					AudioDAC0_DataSet(sink_dac, (ONE_FRAME/8));//DAC这个位置正好是rf第二包完成
					return;
#else
					AudioDAC0_DataSet(sink_dac, (ONE_FRAME/4));
#endif
				}
				else
				{
#if	  defined(WIRELESS_TURNKEY2_6)
					AudioDAC0_DataSet(sink_dac, (ONE_FRAME/3));
#else
					AudioDAC0_DataSet(sink_dac, (ONE_FRAME/4));
#endif
					AudioOutDelete = (Device1_2rdPackSample%ONE_FRAME);
				}
			}
		}
#endif //!defined(CFG_RESOURCE_DIS)
		{
			uint16_t sync_cnt = 0;
			while(WirelessInDataLenGet() < SOURCEFRAME(APP_SOURCE_NUM))
			{
				if(WirelessinFrameRead == ONE_FRAME)
				{
					sync_cnt = AudioAssociationProcess(PcmBuf, PcmBuf2);//按照帧长128samples来处理
					WirelessinFrameRead = 0;
					int16_t *pLeft = NULL;
					int16_t *pRight = NULL;
					if((Device1Info.AudioSycn >= SYNC_PACKET_START) && (Device2Info.AudioSycn >= SYNC_PACKET_START) && sync_cnt)
					{
						pLeft = PcmBuf;
						pRight = PcmBuf2;
					}
					else if(Device2Info.AudioSycn >= SYNC_PACKET_START && sync_cnt)
					{
						pLeft = PcmBuf2;
						pRight = PcmBuf2;
					}
					else if(Device1Info.AudioSycn >= SYNC_PACKET_START && sync_cnt)
					{
						pLeft = PcmBuf;
						pRight = PcmBuf;
					}
					if(pLeft == NULL || pRight == NULL)
					{
						memset(PcmBuf, 0, sizeof(PcmBuf));
					}
					else
					{
#if DECODE_CH == 1
						for(int i = ONE_FRAME; i > 0; i--)
						{
							PcmBuf[2 * i - 1] = pRight[i - 1];
							PcmBuf[2 * i - 2] = pLeft[i - 1];
						}
#endif
					}
				}

				uint16_t SampleLen = MIN(ONE_FRAME - WirelessinFrameRead, SOURCEFRAME(APP_SOURCE_NUM) - WirelessInDataLenGet());
				WirelessInDataSet(PcmBuf + 2 * WirelessinFrameRead, SampleLen);
				WirelessinFrameRead += SampleLen;
			}
		}
#if !defined(CFG_RESOURCE_DIS)
	#if !defined(PACKET_AUDIO_CH_BACKWARD)//双向与单向方案 rx dac同步方式有区别
			{
				if(AUDIOOUT_STEP_LEN_FUNC() <= 1 && ((Device1Info.AudioSycn >= SYNC_PACKET_START) || (Device2Info.AudioSycn >= SYNC_PACKET_START)))
				{
					DBG("DacFifo Err!\n");
					WirelessAudioRxSyncReset();
				}
				if(AudioOutDelete)
				{
					AudioDAC0_DataSet(&sink_dac[AudioOutDelete * 2], g_frame_size - AudioOutDelete);
					AudioOutDelete = 0;
				}
				else
					AudioDAC0_DataSet(sink_dac, g_frame_size);
			}
	#else //end !defined(PACKET_AUDIO_CH_BACKWARD) 双向
			if(AUDIOOUT_STEP_LEN_FUNC() <= 1 && ((Device1Info.AudioSycn >= SYNC_PACKET_START) || (Device2Info.AudioSycn >= SYNC_PACKET_START)))
			{
				DBG("DacFifo Err!\n");
				WirelessAudioRxSyncReset();
				Wireless_TransBufInit();
			}

			AudioDAC0_DataSet(&sink_dac[AudioOutDelete * 2], g_frame_size - AudioOutDelete);
			AudioOutDelete = 0;
	#endif //双向rx

#endif//!defined(CFG_RESOURCE_DIS)
	}
}

/**************************************Packet*******************************************************/
//恢复信息，中断调用禁打印
#if CRC_PACKSUB == 0
	__attribute__((section(".tcm_section")))
	void audio_PackAddheader(unsigned char* audio_encodeframe)
	{
		memcpy(audio_encodeframe, SilenceFrame, 3);
	}
#endif//仅2-2/2-6保留方案

__attribute__((section(".tcm_section")))
void audio_Pack2Frames(unsigned char* audio_encodeframe)
{
#if 0//RFPACK_NAUDIO > 1
	uint8_t Index;
	uint8_t SubPacketLen;
	uint8_t FrameData[SBC_DEC_LEN_PER_FREME - CRC_PACKSUB];

	SubPacketLen = SBC_DEC_LEN_PER_FREME + PACKET_CNT_LEN + PACKET_CRC_LEN - CRC_PACKSUB;
	memcpy(&audio_encodeframe[SBC_DEC_LEN_PER_FREME * RFPACK_NAUDIO], &audio_encodeframe[CRC_PACKSUB + SubPacketLen * RFPACK_NAUDIO - PACKET_CRC_LEN], PACKET_CRC_LEN + PACKET_CNT_LEN);
	for(Index = 0; Index < RFPACK_NAUDIO; Index++)
	{
		if(Index != 0)
		{
			memcpy(FrameData, &audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], SBC_DEC_LEN_PER_FREME - CRC_PACKSUB);
			memcpy(&audio_encodeframe[SBC_DEC_LEN_PER_FREME * Index + CRC_PACKSUB], FrameData, SBC_DEC_LEN_PER_FREME - CRC_PACKSUB);
		}
		memcpy(&audio_encodeframe[(SBC_DEC_LEN_PER_FREME) * Index], SilenceFrame, CRC_PACKSUB);
	}
#endif 
}

/*******************************************Audio Sync API***************************************/
void AudioOutSyncChange(uint8_t Device)
{
	uint64_t SampleZero = 0;
	uint16_t Offset = 0;
#if	!defined(CFG_RESOURCE_DIS)
	if(Device == DEVICE1_MASK)
	{
		Offset = ((uint16_t)((Device1Info.RecvTimingOffset - AUDIOOUT_STEP_LEN_FUNC()))) % ONE_FRAME;//g_frame_size
	}
	else
		Offset = ((uint16_t)((Device2Info.RecvTimingOffset - AUDIOOUT_STEP_LEN_FUNC()))) % ONE_FRAME;//g_frame_size

	if(Offset < AUDIOOUT_SYNC_JITTER)
	{
		for(; Offset <AUDIOOUT_SYNC_JITTER; Offset+=2)
		{
			AudioDAC0_DataSet(&SampleZero, 2);
		}
	}
	else
	{
		AudioOutDelete = Offset - AUDIOOUT_SYNC_JITTER;
	}
#endif //!defined(CFG_RESOURCE_DIS)
}

#if	!defined(CFG_RESOURCE_DIS)
void Audio_Check1stFrameAllRightCounterReset(unsigned char id)
{
	if(id==0)
	{
		rf0_packcnt = 0;
		Device0_syncpackallright = 0;
		device0_hiscnt = 0;
	}
	if(id==1)
	{
		rf1_packcnt = 0;
		Device1_syncpackallright = 0;
		device1_hiscnt = 0;
	}
}

void Audio_Check1stFrameAllRightCounterResetForAudioAssociation(unsigned char id)
{
	if(id==0)
	{
		Device0_syncpackallright = 0;
		device0_hiscnt = 0;
	}
	if(id==1)
	{
		Device1_syncpackallright = 0;
		device1_hiscnt = 0;
	}
}

void Audio_Check1stFrameAllRightCounterStart(unsigned char id)
{
#if	TURNKEY_3_X ||\
	defined(WIRELESS_TURNKEY5_1) || defined(WIRELESS_TURNKEY8_1)
	if(Device0_syncpackallright == 0)
	{
		Device0_syncpackallright = 1;
	#ifdef	BPLC_EN
		AudioDAC0_DataSet(source_wireless_in, ONE_FRAME);
	#else
		AudioDAC0_DataSet(source_wireless_in, 15);
	#endif
		AudioOutDelete = (AudioDAC0_DataLenGet()%ONE_FRAME);
	}
#else
	if(id==0)
	{
		if((rf0_packcnt++)<PACKET_STABLE_THRHLD_RF)
			return;
		Device0_syncpackallright = 1;
		device0_hiscnt = 0;
	}
	if(id==1)
	{
		if((rf1_packcnt++)<PACKET_STABLE_THRHLD_RF)
			return;
		Device1_syncpackallright = 1;
		device1_hiscnt = 0;
	}
#endif
}
unsigned char Audio_Check1stFrameAllRightStateGet(unsigned char id)
{
	if(id==0)
	{
		return Device0_syncpackallright;
	}
	if(id==1)
	{
		return Device1_syncpackallright;
	}
	return 0;
}


unsigned char Audio_Check1stFrameAllRight(unsigned char id,unsigned char cnt)
{
	static unsigned char device0_checkcnt = 0, device1_checkcnt = 0;
	if((id==0)&&(Device0_syncpackallright==1))
	{
		if((device0_hiscnt==0)||(device0_hiscnt != cnt))
			device0_checkcnt = 0;
		if((cnt!=0)&&(device0_hiscnt == cnt))
		{
			device0_checkcnt++;
			if(device0_checkcnt>=(INTERVAL_NRF-1))
			{
				Device0_2rdPackSample = AudioDAC0_DataLenGet();
				Device0_syncpackallright = 2;
			}
		}
		device0_hiscnt = cnt;
		return 0;
	}
	if((id==1)&&(Device1_syncpackallright==1))
	{
		if((device1_hiscnt==0)||(device1_hiscnt != cnt))
			device1_checkcnt = 0;
		if((cnt!=0)&&(device1_hiscnt == cnt))
		{
			device1_checkcnt++;
			if(device1_checkcnt>=(INTERVAL_NRF-1))
			{
				Device1_2rdPackSample = AudioDAC0_DataLenGet();
				Device1_syncpackallright = 2;
			}
		}
		device1_hiscnt = cnt;
		return 0;
	}
	if((id==0)&&(Device0_syncpackallright==2))
		return 1;
	if((id==1)&&(Device1_syncpackallright==2))
		return 1;

	return 0;
}
#endif //!defined(CFG_RESOURCE_DIS)
#endif //defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
