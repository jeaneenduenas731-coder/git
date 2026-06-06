/**
 **************************************************************************************
 * @file    bt_source.c
 * @brief   
 *
 * @author  BKD
 * @version V1.0.0
 *
 * $Created: 21-04-09
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "rtos_api.h"
#include "app_config.h"
#include "debug.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "audio_core_service.h"
#include "bt_config.h"
#include "sbcenc_api.h"
#include "bt_manager.h"
#include "mcu_circular_buf.h"
#include "sbcenc_api.h"
//#include <nds32_intrinsic.h>
#if BT_SOURCE_SUPPORT
//#define SBC_NUMBER_PER_FRAME 83//119
#define SBC_SEND_FRAME_NUMBER 5//4//
#define SBC_NUMBER_PER_FRAME_QUALITY		(13+AppA2dpSourceSbcBitpool()*2)
#define SBC_FRAME_BUFF_SIZE					(13+53*2)

static uint8_t sbc_encode_buffer[120];
static uint8_t sbc_encode_buffer_send[120*SBC_SEND_FRAME_NUMBER];
static volatile uint32_t s_operate_only=0;
static SBCEncoderContext *sbc_source_encode_ct;
static MCU_CIRCULAR_CONTEXT sAudioSbcCircularBuf;
static int16_t	*sAudioCircularSbcFifo;

//uint8_t sbc_quality_type = 0;//0=high quality; 1=middle quality
uint32_t gSbcEncoderReinit = 0;//初始化标志
static uint32_t gSbcEncoderRegistered = 0;//注册标志

extern void A2DP_StreamSendSbcData(unsigned char *data,uint32_t DataLength, uint8_t frame_size);
void SbcSourceSendData(void);

extern uint8_t AppA2dpSourceSbcBitpool(void);
extern uint8_t AppA2dpSourceSbcChannelMode(void);
extern uint8_t AppA2dpSourceSbcAllocation(void);
void BtSourceInit(void)
{
//	int32_t SamplePerframe=128;

	if(gSbcEncoderReinit == 0)
		return;

	APP_DBG("ch mode:%d, bitpool:%d, allocation:%d d:%d \n", AppA2dpSourceSbcChannelMode(),AppA2dpSourceSbcBitpool(),AppA2dpSourceSbcAllocation(),SBC_NUMBER_PER_FRAME_QUALITY);

	if(sbc_source_encode_ct == NULL)
		sbc_source_encode_ct = (SBCEncoderContext*)osPortMalloc(sizeof(SBCEncoderContext));
	
	if(sbc_source_encode_ct == NULL)
	{
		DBG("sbc_source_encode_ct init malloc error\n");
	}
	memset(sbc_source_encode_ct, 0, sizeof(SBCEncoderContext));

//	sbc_encoder_initialize_advanced(sbc_source_encode_ct, SBC_ENC_MODE_JOINT_STEREO, CFG_PARA_SAMPLE_RATE, 16, 8, 35, SBC_ENC_AM_LOUDNESS);

	sbc_encoder_initialize_advanced(sbc_source_encode_ct, AppA2dpSourceSbcChannelMode(), AudioCoreMixSampleRateGet(DefaultNet), 16, 8, AppA2dpSourceSbcBitpool(), AppA2dpSourceSbcAllocation());

	//数据缓存,按照高等质量的buf进行申请
	if(sAudioCircularSbcFifo == NULL)
		sAudioCircularSbcFifo = (int16_t *)osPortMalloc(SBC_FRAME_BUFF_SIZE * 18);//18*3
		
	if(sAudioCircularSbcFifo == NULL)
	{
		DBG("sAudioCircularSbcFifo malloc error \n");
	}
	memset(sAudioCircularSbcFifo, 0, SBC_FRAME_BUFF_SIZE * 18);
	MCUCircular_Config(&sAudioSbcCircularBuf, sAudioCircularSbcFifo, SBC_FRAME_BUFF_SIZE * 18);

	gSbcEncoderReinit = 0;
	gSbcEncoderRegistered = 1;

	APP_DBG("bt source init success ok!!!\n");
}
uint16_t SbcSourceEncode(void* InBuf, uint16_t InLen)
{
	uint32_t sbc_encoded_data_length;
	uint32_t sample_count;
	//uint64_t cycle_value;

	if(gSbcEncoderReinit)
	{
		BtSourceInit();
	}

	if((sAudioCircularSbcFifo == NULL)||(sbc_source_encode_ct == NULL))
		return InLen;

	if((gSbcEncoderRegistered != 1)||(gSbcEncoderReinit != 0))
		return InLen;
	
	if(GetSourceA2dpState()<BT_A2DP_STATE_CONNECTED)
	{
		return InLen;
	}
	
	if(s_operate_only)
	{
		return InLen;
	}
	
	s_operate_only=1;

	if(mainAppCt.sbc_encode_pcm_buf)
	{
		MCUCircular_PutData(&mainAppCt.sbc_encode_pcm_context,InBuf,InLen*4);
		InLen = MCUCircular_GetDataLen(&mainAppCt.sbc_encode_pcm_context)/4;
		for(sample_count=0;sample_count<InLen/128;sample_count++)
		{
			//__nds32__mtsr(0,NDS32_SR_PFMC0);
			//__nds32__mtsr(1,NDS32_SR_PFM_CTL);
			if(mainAppCt.sbc_encode_pcm_buf)
			{
				MCUCircular_GetData(&mainAppCt.sbc_encode_pcm_context,InBuf,128*2*2);
				if(sbc_encoder_encode(sbc_source_encode_ct,((int16_t *)InBuf), (uint8_t *)&sbc_encode_buffer, &sbc_encoded_data_length)==SBC_ENC_ERROR_OK)
				{
					//__nds32__mtsr(0,NDS32_SR_PFM_CTL);
					//cycle_value=__nds32__mfsr(NDS32_SR_PFMC0);
					 //APP_DBG("%d\n",sbc_encoded_data_length);//*1000/288000000
					 if(MCUCircular_GetSpaceLen(&sAudioSbcCircularBuf) > sbc_encoded_data_length)
					 {
						 MCUCircular_PutData(&sAudioSbcCircularBuf,sbc_encode_buffer,sbc_encoded_data_length);//SBC_NUMBER_PER_FRAME BYTE
					 }
					 else
						 break;
				}
			}
		}
	}
	else
	{
		for(sample_count=0;sample_count<InLen/128;sample_count++)
		{
			//__nds32__mtsr(0,NDS32_SR_PFMC0);
			//__nds32__mtsr(1,NDS32_SR_PFM_CTL);
			if(sbc_encoder_encode(sbc_source_encode_ct,((int16_t *)InBuf)+sample_count*256, (uint8_t *)&sbc_encode_buffer, &sbc_encoded_data_length)==SBC_ENC_ERROR_OK)
			{
				//__nds32__mtsr(0,NDS32_SR_PFM_CTL);
				//cycle_value=__nds32__mfsr(NDS32_SR_PFMC0);
				 //APP_DBG("%d\n",sbc_encoded_data_length);//*1000/288000000
				 if(MCUCircular_GetSpaceLen(&sAudioSbcCircularBuf) > sbc_encoded_data_length)
				 {
					 MCUCircular_PutData(&sAudioSbcCircularBuf,sbc_encode_buffer,sbc_encoded_data_length);//SBC_NUMBER_PER_FRAME BYTE
				 }
			}
		}
	}
	s_operate_only=0;
	SbcSourceSendData();
	return InLen;
	
}
extern uint32_t send_ok_bkd;
void SbcSourceSendData(void)
{
	uint32_t sbc_data_length=0;
	
	//GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA2);
	if(GetSourceA2dpState()<BT_A2DP_STATE_CONNECTED
	  || GetAvrcpState(0) < BT_AVRCP_STATE_CONNECTED)
	{	
		return;
	}
	
	if((sAudioCircularSbcFifo == NULL)||(sbc_source_encode_ct == NULL))
		return ;
	if(s_operate_only)
	{
		return;
	}

	s_operate_only=1;
	sbc_data_length=MCUCircular_GetDataLen(&sAudioSbcCircularBuf);

	if(sbc_data_length>=SBC_NUMBER_PER_FRAME_QUALITY*SBC_SEND_FRAME_NUMBER)
	{
		//GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA2);
		MCUCircular_GetData(&sAudioSbcCircularBuf,sbc_encode_buffer_send,SBC_NUMBER_PER_FRAME_QUALITY*SBC_SEND_FRAME_NUMBER);
		A2DP_StreamSendSbcData(sbc_encode_buffer_send, SBC_NUMBER_PER_FRAME_QUALITY*SBC_SEND_FRAME_NUMBER, SBC_NUMBER_PER_FRAME_QUALITY);

//			GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA2);
	}
	s_operate_only=0;
}

uint16_t GetBtSourceFifoSpaceLength(void)
{
	if((sAudioCircularSbcFifo == NULL)||(sbc_source_encode_ct == NULL))
		return 0;
	if(MCUCircular_GetSpaceLen(&sAudioSbcCircularBuf)>=SBC_NUMBER_PER_FRAME_QUALITY)
	{
		return AudioCoreFrameSizeGet(DefaultNet);
	}
	else
	{
		return 0;
	}
}

extern void AppA2dpSuspendStream(void);
extern void AppA2dpStartStream(void);

void BtSourcePlay(void)
{
	AudioCoreSinkEnable(AUDIO_BT_SOURCE_SINK_NUM);
	AppA2dpStartStream();
}

void BtSourceStop(void)
{
	AudioCoreSinkDisable(AUDIO_BT_SOURCE_SINK_NUM);
	AppA2dpSuspendStream();
}

#endif




