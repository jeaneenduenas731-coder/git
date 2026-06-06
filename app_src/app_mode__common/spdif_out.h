#ifndef _SPDIF_OUT_H_
#define _SPDIF_OUT_H_

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN

typedef enum _SPDIF_METADATA_SAMPLEFREQ
{
	//bit24,25,26,27
	FREQ_22K  = 0b0010,	//0010
	FREQ_44K  = 0b0000,	//0000
	FREQ_88K  = 0b0001,	//0001
	FREQ_176K = 0b0011,	//0011
	FREQ_24K  = 0b0110,	//0110
	FREQ_48K  = 0b0100,	//0100
	FREQ_96K  = 0b0101,	//0101
	FREQ_192K = 0b0111,	//0111
	FREQ_32K  = 0b1100,	//1100
	FREQ_768K = 0b1001,	//1001
} SPDIF_METADATA_SAMPLEFREQ;

typedef enum _SPDIF_METADATA_WORDLEN
{
	//bit32,33,34,35
	LEN_16BIT = 0b0100,	//0100
	LEN_24BIT = 0b1101,	//1101
} SPDIF_METADATA_WORDLEN;

typedef struct _SPDIF_ChannelStatus_Context
{
//	uint8_t 		channel_status_block		 	:1;
//	uint8_t 		pcm_identify			 		:1;
//	uint8_t 		copyright						:1;
//	uint8_t 		format_info					 	:3;
//	uint8_t 		channel_status_mode				:2;
//	uint8_t 		category_code					:8;
//	uint8_t 		source_num					 	:4;
//	uint8_t 		channel_num					 	:4;
	uint8_t 		sample_freq					 	:4;
	uint8_t 		clock_accuracy					:2;
	uint8_t 							 			:2;
	uint8_t 		word_len					 	:4;
//	uint8_t 		orign_sample_freq				:4;
//	uint8_t 		CGMS_A					 		:2;
} SPDIF_ChannelStatus_Context;

SPDIF_ChannelStatus_Context spdif_channelstatus_ct;

void AudioSpdifOutParamsSet(void);
uint16_t AudioSpdifTXDataSet(void* Buf, uint16_t Len);
uint16_t AudioSpdifTXDataSpaceLenGet(void);
void SpdifSampleRateandBiteProcess(int32_t *spdifBuf, uint32_t len);
#endif
#endif
