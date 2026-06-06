/*
 * audio_main_backward.c
 *
 *  Created on: Mar 7, 2025
 *      Author: piwang
 */
#include "irqn.h"
#include "rom.h"
#include "app_config.h"
#include "adc_interface.h"
#include "audio_core_api.h"
#include "debug.h"
#include "mcu_circular_buf.h"
#include "audio_association.h"
#include "mvintrinsics.h"
#include "sbcenc_api.h"
#include "sbc_encoder.h"
#include "wireless2.h"
#include "log_info.h"

#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)

extern uint16_t gSysVolArr[];

#ifdef AUDIO_SINE_TEST_EN
	#define TEST_PERIOD	256
	int16_t SineMono[TEST_PERIOD] = SINE_256_MONO;
	uint32_t FrameOffset = 0;
	//SineMono[FrameOffset++%TEST_PERIOD];
#endif

#if defined(PACKET_AUDIO_CH_BACKWARD)
void audio_PackBuild(unsigned char* audio_encodeframe, uint8_t Cnt, uint8_t Cmd);
void wireless_AudioIgnorecntSet(unsigned int ignore_cnt);
uint16_t Wireless_TransSpaceLen(void);

//care for Porting, must stereo
int16_t EncBuf[ONE_FRAME * 2];
uint8_t sbc_buf_out[RFAUDIO_LEN_PER_FREME+10];
SBCEncoderContext sbc_enc;
void wireless_sbc_encoder_aplly(void *sbc_enc,int16_t *in_pcm,uint8_t *out_sbc,uint32_t *length);
void wireless_sbc_encoder_init(SBCEncoderContext *ct);

uint8_t sbcCnt = 0;


#define TRANS_LEN		(RFAUDIO_TRANS_LEN * (256 / (ONE_FRAME * RFPACK_NAUDIO)) * 5)//Porting change for audiosdk effect Frame
MCU_CIRCULAR_CONTEXT 	WirelessTransCircularBuf_t;
unsigned char WirelessTransCircularBuf[TRANS_LEN];

//tx Packet trigger & monitor
//0--没有同步;0xff--双设备adc与rf对齐; Others：Wireless send request count + 1
unsigned char audio_conuter = 0;
unsigned int  audio_ignorecnt = 0;

void audio_wireless_init(void)
{
	wireless_sbc_encoder_init(&sbc_enc);
	Wireless_TransBufInit();
}

/********************双向传输send audio 处理 ****************************/
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

		if(device1.ConStatus != CONNECT_NONE
#ifdef WIRELESS_TURNKEY8_1
			&& MvWireless2AdvModeGetSchState()
#endif
				)
		{
			wireless_sbc_encoder_aplly(&sbc_enc, EncBuf, &sbc_buf_out[SBC_ENC_LEN_PER_FREME*(send_audio_cnt-1)], &sbc_len);

			if(((send_audio_cnt%RFPACK_NAUDIO)==0)&&(Wireless_TransSpaceLen() > RFAUDIO_TRANS_LEN))
			{
				send_audio_cnt = 1;
				audio_PackBuild(sbc_buf_out, sbcCnt, 0xFF);
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
	}
}

/************************************Audio to Wireless API******************************************************/

void audio_PackBuild(unsigned char* audio_encodeframe, uint8_t Cnt, uint8_t Cmd)
{
	unsigned short sbc_crc16;

#if	!(RFPACK_NAUDIO > 1)
	audio_encodeframe[RFAUDIO_FRAME_LEN + 0] = Cnt;//40
	audio_encodeframe[RFAUDIO_FRAME_LEN + 1] = Cmd;
	sbc_crc16 = ROM_CRC16((const char* )audio_encodeframe + CRC_PACKSUB, RFAUDIO_FRAME_LEN - CRC_PACKSUB + PACKET_CNT_LEN, 0);
	audio_encodeframe[RFAUDIO_FRAME_LEN + PACKET_CNT_LEN + 0] = sbc_crc16 & 0xff;//40
	audio_encodeframe[RFAUDIO_FRAME_LEN + PACKET_CNT_LEN + 1] = sbc_crc16>>8;
#el#if (RFAUDIO_TRANS_LEN==RFAUDIO_TRANS_F)//RFPACK_NAUDIO > 1

	uint8_t Index;
	uint8_t SubPacketLen;
	uint8_t FrameData[SBC_ENC_LEN_PER_FREME - CRC_PACKSUB];

	//数据P1+P2...Pn+CNT+CMD
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
#else
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

/***********************Wireless Tx FIfo************************/
void Wireless_TransBufInit(void)
{
	wireless_AudioCounterReset();
	GIE_DISABLE();
	MCUCircular_Config(&WirelessTransCircularBuf_t, WirelessTransCircularBuf, sizeof(WirelessTransCircularBuf));
	GIE_ENABLE();
	sbcCnt = 0;
}

uint16_t Wireless_TransSpaceLen(void)
{
	return MCUCircular_GetSpaceLen(&WirelessTransCircularBuf_t);
}

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

void wireless_AudioIgnorecntSet(unsigned int ignore_cnt)
{
	audio_ignorecnt = ignore_cnt;
}
//1-x Master isr
unsigned int wireless_AudioIgnorecntGet(void)
{
	return audio_ignorecnt;
}

void wireless_AudioCounterReset(void)
{
	audio_conuter = 0;
}

/****************************
 * Wireless send Packet request interrupt
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
		if(audio_ignorecnt==0)
			WirelessAudioRxSyncReset();
		return 0;
	}
	if(audio_conuter==0)
	{
		if(MCUCircular_GetDataLen(&WirelessTransCircularBuf_t) >= WIRELESS_TRANS_THRHLD * RFAUDIO_TRANS_LEN && StepNum % RFPACK_REQUEST == 0)
			audio_conuter = 1;
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
			wireless_AudioIgnorecntSet(240 * RFPACK_REQUEST / RFPACK_NAUDIO);
			Wireless_TransBufInit();
		}
	}
	return 0;
}

#endif //End #ifdef	PACKET_AUDIO_CH_BACKWARD
#endif //#if defined(CFG_WIRELESS_EN) && !defined(CFG_SYNC_TO_TX_SDK)
