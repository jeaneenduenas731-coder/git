/*
 * wireless_audio_main_tx.c
 *@brief   Original name:audio_main.c @wireless_mic_tx_sdk
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
#include "mixer.h"
#include "rom.h"
#include "wireless2.h"
#include "sbcenc_api.h"
#include "math.h"
#include "log_info.h"

#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
void audio_PackRebuild(unsigned char* audio_encodeframe);
void wireless_AudioIgnorecntSet(unsigned int ignore_cnt);
unsigned char wireless_AudioCounterProcess();
void wireless_sbc_encoder_aplly(void *sbc_enc,int16_t *in_pcm,uint8_t *out_sbc,uint32_t *length);
uint16_t Wireless_TransSpaceLen(void);

#if defined(ENCODE_CH) && ENCODE_CH == 2
#include "sbcenc_api.h"
#endif

//care for Porting, must stereo 
int16_t EncBuf[ONE_FRAME * 2]; //Poring to stereo

#if defined(ENCODE_CH) && ENCODE_CH == 2//
	uint8_t sbc_buf_out[RFAUDIO_LEN_PER_FREME*2+8];
	SBCEncoderContext sbc_enc;
	void wireless_sbc_encoder_init(SBCEncoderContext *ct);
#else
	#ifdef WIRELESS_TURNKEY2_9
		uint8_t sbc_buf_out[RFAUDIO_TRANS_C + CRC_PACKSUB];
	#else
		uint8_t sbc_buf_out[RFAUDIO_LEN_PER_FREME*2+8];
	#endif
	SBC_ENC_PARAMS sbc_enc;
	void wireless_sbc_encoder_init(SBC_ENC_PARAMS *ct);
#endif

uint8_t sbcCnt = 0;

#ifdef CRC_MULTIPLE
	const uint8_t is_multi_crc = TRUE;
#else
	const uint8_t is_multi_crc = FALSE;
#endif

/****************Audio to Wireless Declare**************/
#define TRANS_LEN		(RFAUDIO_TRANS_LEN * (256 / (ONE_FRAME * RFPACK_NAUDIO)) * 5) //Porting change for audiosdk effect Frame
MCU_CIRCULAR_CONTEXT 	WirelessTransCircularBuf_t;
unsigned char WirelessTransCircularBuf[TRANS_LEN];

/***************Audio Sync Declare*********************/
//tx Packet trigger & monitor
//0--没有同步;0xff--双设备adc与rf对齐; Others：Wireless send request count + 1
static unsigned char audio_conuter = 0;
static unsigned int  audio_ignorecnt = 0;
//mic fifo delay Control
static unsigned char audio_paritycnt = 0xff;
static unsigned char audio_RFstartcnt = 0;//rf首包不稳多判断几次
unsigned char wireless_AudioCounterProcess(void);

uint32_t TargetVal = 0;
void AudioClkRetarget(void)
{
	uint32_t temp1;
	float temp2;

#if defined(WIRELESS_TURNKEY1_4)|| \
	TURNKEY_2_X ||  TURNKEY_3_X ||\
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
	TURNKEY_2_X || TURNKEY_3_X  ||\
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

void wireless_audio_init_tx(void)
{
	wireless_sbc_encoder_init(&sbc_enc);
	Wireless_TransBufInit();

#ifdef PACKET_AUDIO_CH_BACKWARD
	wireless_audio_init();
#endif

#if	SAMPLE_RATE == 44100
	AudioClkRetarget();
#endif
}

#ifdef AUDIO_SINE_TEST_EN
	#ifdef WIRELESS_TX2
		#define TEST_PERIOD	128
		int16_t SineMono[TEST_PERIOD] = SINE_128_MONO;
	#else
		#define TEST_PERIOD	32
		int16_t SineMono[TEST_PERIOD] = SINE_32_0DB_MONO;
	#endif
uint32_t FrameOffset = 0;
//SineMono[FrameOffset++%TEST_PERIOD];
#endif//AUDIO_SINE_TEST_EN

/****************************************
 *
 *
 ***************************************/

/********************tx audio 处理 ****************************/
void wireless_out_process(void)
{
	static unsigned char send_audio_cnt = 1;

	while(WirelessOutDataLenGet() >= ONE_FRAME)
	{
		uint32_t sbc_len = 0;

		WirelessOutDataGet(EncBuf, ONE_FRAME);
#ifdef AUDIO_SINE_TEST_EN
		int i;
		for(i=0;i<ONE_FRAME;i++)
		{
			EncBuf[2 * i] = SineMono[FrameOffset % TEST_PERIOD] / 2;
			EncBuf[2 * i + 1] = SineMono[FrameOffset++ % TEST_PERIOD] / 2;
		}
#endif


#if ENCODE_CH == 1
		downmix_2to1_apply(EncBuf,  EncBuf, ONE_FRAME);
#endif

		// printf("device1.ConStatus:%d\r\n",device1.ConStatus);
#if (!defined (WIRELESS_TURNKEY6_1)) && (!defined (WIRELESS_TURNKEY8_1))
		if(device1.ConStatus != CONNECT_NONE)
#else
		if(1)
#endif
		{
			wireless_sbc_encoder_aplly(&sbc_enc, EncBuf, sbc_buf_out+(SBC_ENC_LEN_PER_FREME*(send_audio_cnt-1)),&sbc_len);
			if(((send_audio_cnt%RFPACK_NAUDIO)==0)&&(Wireless_TransSpaceLen() > RFAUDIO_TRANS_LEN))
			{
				send_audio_cnt = 1;
#ifdef KEY_REMOTE
				audio_PackBuild(sbc_buf_out, sbcCnt, InfoGet());
#else
				audio_PackBuild(sbc_buf_out, sbcCnt, 0xff);
#endif
				Wireless_TransBufWrite(&sbc_buf_out[CRC_PACKSUB], RFAUDIO_TRANS_LEN);
			}
			else if(((send_audio_cnt%RFPACK_NAUDIO)!=0)&&(Wireless_TransSpaceLen() > RFAUDIO_TRANS_LEN))
			{
				send_audio_cnt++;
			}
			else
				printf(".");//Audio帧<->WirelessSlot失锁--前者快

			sbcCnt++;
		}
		else if((device1.ConStatus == CONNECT_NONE))
		{
			send_audio_cnt = 1;
		}
#if defined(CFG_FUNC_DEBUG_EN)//defined(DEBUG_LOG_EN) || defined(CFG_FUNC_USBDEBUG_EN)
		//RF取不到数据的报错
		static unsigned char err_display = 0;

		if( (wireless_AudioIgnorecntGet()==0)&&(err_display==1) )
		{
			err_display = 0;
		}
		if( (wireless_AudioIgnorecntGet()!=0)&&(err_display==0) )
		{
			err_display = 1;
			DBG("AdcFifo Err!!\n");
		}
#endif
	}
}

/************************************Audio to Wireless API******************************************************/
void audio_PackBuild(unsigned char* audio_encodeframe, uint8_t Cnt, uint8_t Cmd)
{
	unsigned short sbc_crc16;

#if !(RFPACK_NAUDIO > 1)
	#if defined(WIRELESS_TURNKEY2_9)
		//Half1+Cmd+Crc1+Half2+Crc2+Half2
		unsigned long half_pack_len = (RFAUDIO_LEN_PER_FREME - CRC_PACKSUB)/2;
		uint8_t *PacketBuf =  audio_encodeframe + CRC_PACKSUB;
		memcpy(&PacketBuf[half_pack_len * 2 + PACKET_CRC_LEN * 2 + PACKET_CNT_LEN], &PacketBuf[half_pack_len], half_pack_len);//backup
		memcpy(&PacketBuf[half_pack_len + PACKET_CRC_LEN + PACKET_CNT_LEN], &PacketBuf[half_pack_len * 2 + PACKET_CRC_LEN * 2 + PACKET_CNT_LEN], half_pack_len);

		PacketBuf[half_pack_len  ] = Cnt;
		PacketBuf[half_pack_len + 1] = Cmd;
		sbc_crc16=ROM_CRC16((const char*)PacketBuf, half_pack_len + PACKET_CNT_LEN, 0);
		sbc_crc16+=7;
		PacketBuf[half_pack_len + PACKET_CNT_LEN] = sbc_crc16&0xff;
		PacketBuf[half_pack_len + PACKET_CNT_LEN + 1] = sbc_crc16>>8;

		sbc_crc16=ROM_CRC16((const char*)&PacketBuf[half_pack_len + PACKET_CNT_LEN + PACKET_CRC_LEN], half_pack_len, 0);
		sbc_crc16+=7;
		PacketBuf[half_pack_len * 2 + PACKET_CNT_LEN + PACKET_CRC_LEN] = sbc_crc16&0xff;
		PacketBuf[half_pack_len * 2 + PACKET_CNT_LEN + PACKET_CRC_LEN + 1] = sbc_crc16>>8;
	#else //defined(WIRELESS_TURNKEY2_9)
		audio_encodeframe[RFAUDIO_FRAME_LEN + 0] = Cnt;//40
		audio_encodeframe[RFAUDIO_FRAME_LEN + 1] = Cmd;
		#if defined(WIRELESS_TURNKEY2_5) || defined(WIRELESS_TURNKEY2_8) || defined(WIRELESS_TURNKEY3_3) || defined(WIRELESS_CONN_G1)
			sbc_crc16 = wireless_CRC16_2(audio_encodeframe + CRC_PACKSUB, RFAUDIO_FRAME_LEN - CRC_PACKSUB + PACKET_CNT_LEN);
		#else //2-2 2-6 5-1 5-2 6-1
			sbc_crc16 = ROM_CRC16((char*)audio_encodeframe + CRC_PACKSUB, RFAUDIO_FRAME_LEN - CRC_PACKSUB + PACKET_CNT_LEN, 0);
		#endif
		audio_encodeframe[RFAUDIO_FRAME_LEN + PACKET_CNT_LEN + 0] = sbc_crc16 & 0xff;//40
		audio_encodeframe[RFAUDIO_FRAME_LEN + PACKET_CNT_LEN + 1] = sbc_crc16>>8;
	#endif//defined(WIRELESS_TURNKEY2_9)
#elif (RFAUDIO_TRANS_LEN==RFAUDIO_TRANS_D)
		uint8_t Index;
		uint8_t SubPacketLen;
		uint8_t FrameData[SBC_ENC_LEN_PER_FREME - CRC_PACKSUB];

		//数据P1+CRC+P2+CRC...Pn+CMD+CRC(待补充)
		SubPacketLen = SBC_ENC_LEN_PER_FREME - CRC_PACKSUB;
		for(Index = 0; Index < RFPACK_NAUDIO; Index++)
		{
			if(Index != 0)
			{
				memcpy(FrameData, &audio_encodeframe[SBC_ENC_LEN_PER_FREME * Index + CRC_PACKSUB], SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
				memcpy(&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], FrameData, SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
			}
		}
		audio_encodeframe[CRC_PACKSUB+(RFAUDIO_FRAME_LEN - CRC_PACKSUB) * (RFPACK_NAUDIO) + 0] = Cnt;//40
		audio_encodeframe[CRC_PACKSUB+(RFAUDIO_FRAME_LEN - CRC_PACKSUB) * (RFPACK_NAUDIO) + 1] = Cmd;
		sbc_crc16=ROM_CRC16((const char*)&audio_encodeframe[CRC_PACKSUB], SubPacketLen * RFPACK_NAUDIO + PACKET_CNT_LEN, 0);
		audio_encodeframe[CRC_PACKSUB+(RFAUDIO_FRAME_LEN - CRC_PACKSUB) * (RFPACK_NAUDIO) + PACKET_CNT_LEN + 0] = sbc_crc16 & 0xff;
		audio_encodeframe[CRC_PACKSUB+(RFAUDIO_FRAME_LEN - CRC_PACKSUB) * (RFPACK_NAUDIO) + PACKET_CNT_LEN + 1] = sbc_crc16>>8;
#elif (RFAUDIO_TRANS_LEN==RFAUDIO_TRANS_E)
		uint8_t Index;
		uint8_t SubPacketLen;
		uint8_t FrameData[SBC_ENC_LEN_PER_FREME - CRC_PACKSUB];

		//数据P1+CRC+P2+CRC...Pn+CMD+CRC(待补充)
		SubPacketLen = SBC_ENC_LEN_PER_FREME + PACKET_CRC_LEN - CRC_PACKSUB;
		for(Index = 0; Index < RFPACK_NAUDIO; Index++)
		{
			if(Index != 0)
			{
				memcpy(FrameData, &audio_encodeframe[SBC_ENC_LEN_PER_FREME * Index + CRC_PACKSUB], SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
				memcpy(&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], FrameData, SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
			}

			if((Index+1)==RFPACK_NAUDIO)
			{
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN] = Cnt - (RFPACK_NAUDIO - 1) + Index;//40
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + 1] = Cmd;
				sbc_crc16 = ROM_CRC16((const char*)&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], SubPacketLen + PACKET_CNT_LEN - PACKET_CRC_LEN, 0);
				sbc_crc16 +=7 ;
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + PACKET_CNT_LEN + 0] = sbc_crc16 & 0xff;
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + PACKET_CNT_LEN + 1] = sbc_crc16>>8;
			}
			else
			{
				sbc_crc16 = ROM_CRC16((const char*)&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], SubPacketLen - PACKET_CRC_LEN, 0);
				sbc_crc16 +=7 ;
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + 0] = sbc_crc16 & 0xff;
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + 1] = sbc_crc16>>8;
			}
		}
#elif (RFAUDIO_TRANS_LEN==RFAUDIO_TRANS_F)
		uint8_t Index;
		uint8_t SubPacketLen;
		uint8_t FrameData[SBC_ENC_LEN_PER_FREME - CRC_PACKSUB];

		//P1+P2...Pn+CNT+CMD
		SubPacketLen = SBC_ENC_LEN_PER_FREME - CRC_PACKSUB;
		for(Index = 0; Index < RFPACK_NAUDIO; Index++)
		{
			memcpy(FrameData, &audio_encodeframe[SBC_ENC_LEN_PER_FREME * Index + CRC_PACKSUB], SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
			memcpy(&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], FrameData, SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);

			if((Index+1)==RFPACK_NAUDIO)
			{
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1)] = Cnt - (RFPACK_NAUDIO - 1) + Index;//40
				audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) + 1] = Cmd;
			}
		}
#else //RFPACK_NAUDIO > 1
	uint8_t Index;
	uint8_t SubPacketLen;
	uint8_t FrameData[SBC_ENC_LEN_PER_FREME - CRC_PACKSUB];

	//数据P1+CRC+P2+CRC...Pn+CMD+CRC(待补充)
	SubPacketLen = SBC_ENC_LEN_PER_FREME + PACKET_CNT_LEN + PACKET_CRC_LEN - CRC_PACKSUB;
	for(Index = 0; Index < RFPACK_NAUDIO; Index++)
	{
		if(Index != 0)
		{
			memcpy(FrameData, &audio_encodeframe[SBC_ENC_LEN_PER_FREME * Index + CRC_PACKSUB], SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
			memcpy(&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], FrameData, SBC_ENC_LEN_PER_FREME - CRC_PACKSUB);
		}
		audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CNT_LEN - PACKET_CRC_LEN] = Cnt - (RFPACK_NAUDIO - 1) + Index;//40
		audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CNT_LEN - PACKET_CRC_LEN + 1] = Cmd;
	#if defined(WIRELESS_TURNKEY3_1)
		sbc_crc16=wireless_CRC16_2(&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], SubPacketLen - PACKET_CRC_LEN);
	#else
		sbc_crc16=ROM_CRC16((const char*)&audio_encodeframe[CRC_PACKSUB + SubPacketLen * Index], SubPacketLen - PACKET_CRC_LEN, 0);
	#endif
		audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + 0] = sbc_crc16 & 0xff;
		audio_encodeframe[CRC_PACKSUB + SubPacketLen * (Index + 1) - PACKET_CRC_LEN + 1] = sbc_crc16>>8;
	}
#endif
}

/**********Wireless Send FIfo******************/
void Wireless_TransBufInit(void)
{
	wireless_AudioCounterReset();
	GIE_DISABLE();
	MCUCircular_Config(&WirelessTransCircularBuf_t, WirelessTransCircularBuf, sizeof(WirelessTransCircularBuf));
	GIE_ENABLE();
}

uint16_t Wireless_TransSpaceLen(void)
{
	return MCUCircular_GetSpaceLen(&WirelessTransCircularBuf_t);
}

//write data in manloop, need DISABLE Interrupt
void Wireless_TransBufWrite(unsigned char* data,unsigned int len)
{
	GIE_DISABLE();
	MCUCircular_PutData(&WirelessTransCircularBuf_t, data,len);
#ifdef AUDIO_WIRELESS_SYNC_EN
	if(AudioInSyncFrames <= AUDIOIN_SYNC_COUNT)
		AudioInStepOffset = AUDIOIN_STEP_LEN_FUNC();
#endif
	GIE_ENABLE();
}

__attribute__((section(".tcm_section")))
unsigned int Wireless_TransBufRead(unsigned char* data,unsigned int len)
{
	return MCUCircular_GetData(&WirelessTransCircularBuf_t, data,len);
}

/*******************************************Audio Sync API***************************************/
/******************************
 * mic fifo delay Control By tx wireless timing interrupt
 ****************************/
//apply:2-5 2-6 5-1 5-2;wireless_AudioParityCntStart_cb
void wireless_AudioParityCntStart()
{
	extern unsigned char audio_init_isready;
	if(audio_RFstartcnt >= 5)
	{
		if((audio_paritycnt == 0xff))
		{
#ifndef CFG_RESOURCE_DIS
			unsigned int add_adcsample[ONE_FRAME];
			unsigned int audio_PreSample = 10;
			unsigned char current_ADCLen = (AudioADC1_DataLenGet());
#endif// ifndef CFG_RESOURCE_DIS
			audio_paritycnt =  0;
#ifndef CFG_RESOURCE_DIS
			//如果音频清空，t1在master同步包时无法准备好数据，所以ad不清空而是只留10samples数据
			if(current_ADCLen<audio_PreSample)
			{
				ADC_DeleteSample = audio_PreSample-current_ADCLen;
			}
			else
			{
				while((current_ADCLen-audio_PreSample)>ONE_FRAME)
				{
					AudioADC1_DataGet(add_adcsample, ONE_FRAME);
					current_ADCLen = current_ADCLen-ONE_FRAME;
				}
				if(current_ADCLen>audio_PreSample)
					AudioADC1_DataGet(add_adcsample, current_ADCLen-audio_PreSample);
			}
#endif// ifndef CFG_RESOURCE_DIS
			Wireless_TransBufInit();
		}
	}
	else if(audio_init_isready==1)
		audio_RFstartcnt++;
}

//5-1 5-2
void wireless_AudioParityCntReset()
{
	audio_paritycnt = 0xff;
	audio_RFstartcnt = 0;
}

//no use
unsigned char wireless_AudioParityCntGet()
{
	return audio_paritycnt;
}
//apply：2-5 2-6 5-1 5-2;wireless_AudioParityCntProc_cb
void wireless_AudioParityCntProc()
{
	if(audio_paritycnt != 0xff)
	{
		if(audio_paritycnt==1)
			audio_paritycnt = 0;
		else
			audio_paritycnt = 1;
	}
}

void wireless_AudioIgnorecntSet(unsigned int ignore_cnt)
{
	audio_ignorecnt = ignore_cnt;
}
//
unsigned int wireless_AudioIgnorecntGet(void)
{
	return audio_ignorecnt;
}
void wireless_AudioCounterReset(void)
{
	audio_conuter = 0;
}

/****************************
 * Wireless send Packet request in Interrupt
 * trigger No.1
 * Monitor audio Packet stability
 *****************************/
__attribute__((section(".tcm_section")))
bool Wireless_TransPacketIsReady(uint32_t StepNum)
{
	if(audio_ignorecnt!=0)
	{
		Wireless_TransBufInit();
		audio_ignorecnt--;
#if	defined(WIRELESS_TURNKEY1_4)
		if(audio_ignorecnt==0)
			WirelessAudioRxSyncReset();
#endif
		return 0;
	}

	if((audio_conuter==0)
#if	TURNKEY_2_X
		&&(wireless_AudioParityCntGet()!=0xff)
#endif
	)
	{
		if(MCUCircular_GetDataLen(&WirelessTransCircularBuf_t) >= WIRELESS_TRANS_THRHLD * RFAUDIO_TRANS_LEN && StepNum % RFPACK_REQUEST == 0)
		{
			audio_conuter = 1;
			return 1;
		}
	}
	else if(audio_conuter==0xff)//双设备同步才进入
	{
		if(MCUCircular_GetDataLen(&WirelessTransCircularBuf_t) >= RFAUDIO_TRANS_LEN)
		{
			audio_conuter = RFPACK_REQUEST;
		}
	}
	else if(audio_conuter!=0)
	{
		audio_conuter++;
		if(audio_conuter > RFPACK_REQUEST)
		{
			audio_conuter = 1;
			if(MCUCircular_GetDataLen(&WirelessTransCircularBuf_t) >= RFAUDIO_TRANS_LEN)
			{
				return 1;
			}
			//Audio帧<->WirelessSlot失锁--前者慢
//			DBG("AdcFifo Err!!\n");
			SET_INFO_FLAG(ILOG_SEND_EMPTY);
			//本地时钟异常，忽略一批音频音频,对端200包重新同步，本地更稳定。
			wireless_AudioIgnorecntSet(240 * RFPACK_REQUEST / RFPACK_NAUDIO);
			Wireless_TransBufInit();
		}
	}
	return 0;
}
#endif//defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
