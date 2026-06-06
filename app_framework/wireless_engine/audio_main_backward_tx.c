/*
 * audio_main_backward.c
 *
 *  Created on: Mar 7, 2025
 *      Author: piwang
 */
#include "app_config.h"
#include "audio_core_api.h"
#include "audio_core_adapt.h"
#include "sbc_encoder.h"
#include "audio_association.h"
#include "mvintrinsics.h"
#include "mixer.h"
#include "mcu_circular_buf.h"
#include "debug.h"
#include "wireless2.h"

#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)

#ifdef PACKET_AUDIO_CH_BACKWARD
//care for Porting, must stereo
int16_t PcmBuf[ONE_FRAME * 2];
//uint8_t WirelessinFrameRead = ONE_FRAME;//音效帧与Wirelessin帧可能不对齐，数据残留PcmBuf, 初始指向帧末尾ONE_FRAME

static uint16_t AudioOutDelete = 0;//删点缩短Dac延时

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
Rx_AudioAssociation_param_t wirelessaudio_config={
	#if DECODE_CH == 2
	.FrameGroups       = STEREO_ONE_DEVICE,
	#elif DECODE_CH == 1
	.FrameGroups       = MONO_ONE_DEVICE,
	#endif
	.SilencePack       = SilenceFrame,
	.AddHeaderLen	   = CRC_PACKSUB,
	.wireless_frames   = WIRELESS_RECV_FIFO_THRHLD,
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

void wireless_audio_init(void)
{
#if (ONE_FRAME==128)
	AudioPlc128Init();
#elif (ONE_FRAME==96)
	AudioPlc96Init();
#elif (ONE_FRAME==64)
	AudioPlc64Init();
#endif
#if  !defined(CFG_RESOURCE_DIS)
	wirelessaudio_config.RemainFunc = AUDIOOUT_STEP_LEN_FUNC;
	wirelessaudio_config.AudioOutSyncFunc = AudioOutSyncChange;
#endif
	wirelessaudio_config.MicLeftPcmFifo_len_set  = sizeof(MicLeftPcmFifo_f);
	wirelessaudio_config.MicRightPcmFifo_len_set = sizeof(MicRightPcmFifo_f);
	if(SilenceFrame==NULL)
		DBG("Decoder cant find!!!\r\n");
	AudioAssociationInit(&wirelessaudio_config);
}

/*******************************wireless in to audio Dac**************************************/
void wireless_audio_in_process(void)
{
	uint16_t i;

#ifdef WIRELESS_TURNKEY8_1
	// if((device1.ConStatus == CONNECT_NONE) || (!MvWireless2AdvModeGetSchState()))
	if((device1.ConStatus == CONNECT_NONE))
	{
		return;
	}
#endif


	while(WirelessInDataLenGet() < SOURCEFRAME(APP_SOURCE_NUM))
	{
		if(device1.ConStatus != CONNECT_NONE && AudioAssociationProcess(PcmBuf, NULL)
#ifdef WIRELESS_TURNKEY8_1
			&& (MvWireless2AdvModeGetSchState())
#endif
		)
		{
#if DECODE_CH == 1
			for(i = ONE_FRAME; i>0; i--)
			{
				PcmBuf[2 * i - 1] = PcmBuf[i - 1];
				PcmBuf[2 * i - 2] = PcmBuf[i - 1];
			}
#endif
		}
		else
		{
			memset(PcmBuf, 0, ONE_FRAME * 4);
		}
		WirelessInDataSet(PcmBuf, ONE_FRAME);

#if !defined(CFG_RESOURCE_DIS)
		if(AudioDAC0_DataLenGet() <= 1 )
		{
			DBG("DacFifo Err!\n");
			WirelessAudioRxSyncReset();
			Wireless_TransBufInit();
		}

		AudioDAC0_DataSet(&sink_dac[AudioOutDelete * 2], frame_size2 - AudioOutDelete);
		AudioOutDelete = 0;
#endif//!defined(CFG_RESOURCE_DIS)
	}
}

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
#ifndef CFG_RESOURCE_DIS
void AudioOutSyncChange(uint8_t Device)
{
	uint64_t SampleZero = 0;
	uint16_t Offset = 0;

//#ifdef WIRELESS_TURNKEY1_4
//	return;
//#endif
	if(Device == DEVICE1_MASK)
	{
		Offset = ((uint16_t)((Device1Info.RecvTimingOffset - AUDIOOUT_STEP_LEN_FUNC()))) % ONE_FRAME;
	}
	else
		Offset = ((uint16_t)((Device2Info.RecvTimingOffset - AUDIOOUT_STEP_LEN_FUNC()))) % ONE_FRAME;

	if(Offset < AUDIOOUT_SYNC_JITTER)
	{
		for(;Offset <AUDIOOUT_SYNC_JITTER; Offset+=2)
		{
			AudioDAC0_DataSet(&SampleZero, 2);
		}
	}
	else
	{
		AudioOutDelete = Offset - AUDIOOUT_SYNC_JITTER;
	}
}
#endif// ifndef CFG_RESOURCE_DIS
#else//无rx,,免link代码段和变量
	__attribute__((section(".tcm_section")))
	void AudioAssociationRec(uint16_t id, uint8_t *p,uint16_t len)
	{
		//for wireless2.a
	}
#endif //#ifdef PACKET_AUDIO_CH_BACKWARD


#ifdef	WIRELESS_AUDIOEFFECTTEST_MODE
#include "sbc_frame_decoder.h"
	void wireless_sbc_decoder_init(SBCFrameDecoderContext *p,uint8_t ch);
	int32_t wireless_sbc_decoder_apply(SBCFrameDecoderContext *sbc_dec,uint8_t *sbc_buf,uint8_t sbc_size,int16_t *pcm_buf);
	extern SBCFrameDecoderContext 	sbc_dec;
	void EffectTestinit(void)
	{
		wireless_sbc_decoder_init(&sbc_dec,1);
	}


	void EffectTestDecode(uint8_t *Buf, uint16_t Len)
	{
		if(sink_dac != NULL)
		{
			wireless_sbc_decoder_apply(&sbc_dec, Buf, Len, sink_dac);
	#if(PACKET_AUDIO_CH == 1)
				upmix_1to2_apply(sink_dac,  sink_dac, g_frame_size);
	#endif
			AudioDAC0_DataSet(sink_dac, g_frame_size);
		}
	}
#endif

#endif //#if defined(CFG_WIRELESS_EN) && defined(CFG_SYNC_TO_TX_SDK)
