#include <encoder/include/sbc_encoder.h>
//#include "oi_codec_sbc.h"
#include "app_config.h"
#include "debug.h"
#if defined(ENCODE_CH) && ENCODE_CH == 2
#include "sbcenc_api.h"
#endif
#include "sbc_frame_decoder.h"
//#include "oi_codec_sbc_private.h"
#ifdef CFG_WIRELESS_EN
uint32_t sbc_encoder_mem_size(void)
{

	return sizeof(SBC_ENC_PARAMS);
}

#if defined(ENCODE_CH) && ENCODE_CH==2
	void wireless_sbc_encoder_init(SBCEncoderContext *ct)
	{
	#ifdef AUDIO_LOWFRAME
		sbc_encoder_initialize_advanced(ct,SBC_ENC_MODE_JOINT_STEREO,SAMPLE_RATE,SBC_BLOCK,SBC_SUBBAND,ENC_BITPOOL,SBC_SNR);
	#else
		sbc_encoder_initialize_advanced(ct,SBC_ENC_MODE_DUAL_CHANNEL,SAMPLE_RATE,SBC_BLOCK,SBC_SUBBAND,ENC_BITPOOL,SBC_SNR);
	#endif
	}

	void wireless_sbc_encoder_aplly(void *sbc_enc,int16_t *in_pcm,uint8_t *out_sbc,uint32_t *length)
	{
		sbc_encoder_encode(sbc_enc,in_pcm,out_sbc,length);
	}
#elif defined(ENCODE_CH) && ENCODE_CH==1
	void wireless_sbc_encoder_init(SBC_ENC_PARAMS *ct)
	{
		memset(ct,0,sizeof(SBC_ENC_PARAMS));
		ct->s16NumOfBlocks = SBC_BLOCK;
		ct->s16NumOfSubBands = SBC_SUBBAND;
		ct->s16AllocationMethod = SBC_SNR;
		//ct->s16BitPool = 25;
		ct->s16BitPool = ENC_BITPOOL;
		ct->mSBCEnabled = 0;
		ct->s16ChannelMode = SBC_MONO;
		ct->s16NumOfChannels = 1;
	#if(SAMPLE_RATE==44100)
		ct->s16SamplingFreq = SBC_sf44100;
	#elif(SAMPLE_RATE==32000)
		ct->s16SamplingFreq = SBC_sf32000;
	#elif(SAMPLE_RATE==16000)
		ct->s16SamplingFreq = SBC_sf16000;
	#else
		ct->s16SamplingFreq = SBC_sf48000;
	#endif

	//    printf("s32SbBuffer size=%u %u %u %u %u %u\n",sizeof(ct->s32SbBuffer),sizeof(ct->as16Bits),sizeof(ct->as16Join),sizeof(ct->as16ScaleFactor),sizeof(ct->s16ScartchMemForBitAlloc),sizeof(ct));

		SBC_Encoder_Init(ct);
	}


	void wireless_sbc_encoder_aplly(void *sbc_enc,int16_t *in_pcm,uint8_t *out_sbc,uint32_t *length)
	{
		SBC_ENC_PARAMS *ct = (SBC_ENC_PARAMS*)sbc_enc;
		ct->ps16PcmBuffer = in_pcm;
		ct->pu8Packet = out_sbc;
		SBC_Encoder(ct);
		*length = ct->u16PacketLength;
	}
#endif

void wireless_sbc_decoder_init(SBCFrameDecoderContext *p,uint8_t ch)
{
	sbc_frame_decoder_initialize(p);
}

int32_t wireless_sbc_decoder_apply(SBCFrameDecoderContext *sbc_dec,uint8_t *sbc_buf,uint8_t sbc_size,int16_t *pcm_buf)
{
	int32_t status = sbc_frame_decoder_decode(sbc_dec,sbc_buf,sbc_size);
	if(status == 0)
	{
		memcpy(pcm_buf, sbc_dec->pcm, sbc_dec->pcm_length * DECODE_CH * 2);
	}
	else
	{
		DBG("SBC Deocder Err!%d\n", (int)status);
	}
    return status;
}
#endif //#ifdef CFG_WIRELESS_EN

//uint8_t mv_SBC_CalculateChecksum(uint8_t *data)
//{
//	OI_CODEC_SBC_COMMON_CONTEXT common;
//	OI_SBC_ReadHeader(&common, data);
//	return OI_SBC_CalculateChecksum(&common.frameInfo, data);
//}



//////////////////////////code size = 8208   RODATA = 1172
