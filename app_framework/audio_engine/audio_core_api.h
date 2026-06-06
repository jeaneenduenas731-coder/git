/**
 **************************************************************************************
 * @file    audio_core_api.h
 * @brief   audio core 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __AUDIO_CORE_API_H__
#define __AUDIO_CORE_API_H__

#include "audio_core_adapt.h"
#include "resampler.h"
#include "audio_core_effect.h"
#include "bt_config.h"
enum
{
	MIC_SOURCE_NUM,			//麦克风通路
	APP_SOURCE_NUM,			//app主要音源通道,配music音效
	REMIND_SOURCE_NUM,	 	//提示音使用固定混音通道 无音效
	PLAYBACK_SOURCE_NUM,	//flashfs 录音回放通道		无音效
	I2S_MIX_SOURCE_NUM,         //I2S MIX通道
	I2S_MIX2_SOURCE_NUM,        //I2S MIX2通道
	USB_SOURCE_NUM,             //USB MIX通道
	USB_HOST_SOURCE_NUM,		//USB HOST MIX通道
	LINEIN_MIX_SOURCE_NUM,      //Line in MIX通道
	I2S_TDM0_SOURCE_NUM,        //I2S TDM0通道
	I2S_TDM1_SOURCE_NUM,        //I2S TDM1通道
	AUDIO_CORE_SOURCE_MAX_NUM,
};

//不会并存的通路可以合并，特别是sink通路
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE  //USB MIX通道，使用 USB_SOURCE_NUM
	extern 	uint32_t				UsbAudioSourceNum;
	#define USB_AUDIO_SOURCE_NUM	UsbAudioSourceNum //USB_SOURCE_NUM
#else
	#define USB_AUDIO_SOURCE_NUM	APP_SOURCE_NUM
#endif
	#define USB_AUDIO_SOURCE1_NUM	USB_SOURCE_NUM
enum
{
	AUDIO_DAC0_SINK_NUM,		//主音频输出在audiocore Sink中的通道，必须配置，audiocore借用此通道buf处理数据	
#ifdef CFG_FUNC_RECORDER_EN
	AUDIO_RECORDER_SINK_NUM,	//录音专用通道		 不叠加提示音音源。
#endif
#if	(defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)) || defined(CFG_APP_USB_AUDIO_MODE_EN) || (defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD))
	AUDIO_APP_SINK_NUM,
#endif
#if defined(CFG_RES_AUDIO_I2SOUT_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	AUDIO_STEREO_SINK_NUM,      //模式无关Dac0之外的 立体声输出
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	AUDIO_I2S_MIX_OUT_SINK_NUM, //I2S MIX OUT通道
#endif
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	AUDIO_SPDIF_SINK_NUM,
#endif
#if BT_SOURCE_SUPPORT || (defined(CFG_WIRELESS_EN) && (ENCODE_CH != 0) && (DECODE_CH == 0))
	AUDIO_BT_SOURCE_SINK_NUM,
#endif
#ifdef CFG_FUNC_USB_HOST_AUDIO_MIX_MODE
	AUDIO_USB_HOST_SINK_NUM, //USB HOST MIX OUT通道
#endif
	AUDIO_CORE_SINK_MAX_NUM,
};
//随模式调用的共用sink
#define AUDIO_HF_SCO_SINK_NUM 	AUDIO_APP_SINK_NUM
#define USB_AUDIO_SINK_NUM		AUDIO_APP_SINK_NUM

#if defined(CFG_WIRELESS_EN) && (ENCODE_CH != 0) && (DECODE_CH == 0)
	//无共存场景
	#define WIRELESS_AUDIO_SINK_NUM		AUDIO_BT_SOURCE_SINK_NUM
#elif defined(CFG_WIRELESS_EN) && defined(PACKET_AUDIO_CH_BACKWARD)
	#define WIRELESS_AUDIO_SINK_NUM		AUDIO_APP_SINK_NUM
#endif

#if defined(CFG_RES_AUDIO_I2SOUT_EN)
#define AUDIO_I2SOUT_SINK_NUM	AUDIO_STEREO_SINK_NUM//i2s_out通道 常驻
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
#define AUDIO_I2S_MIX2_OUT_SINK_NUM AUDIO_STEREO_SINK_NUM //I2S MIX2 OUT通道
#endif

typedef uint16_t (*AudioCoreDataGetFunc)(void* Buf, uint16_t Samples);
typedef uint16_t (*AudioCoreDataLenFunc)(void);
typedef uint16_t (*AudioCoreDataSetFunc)(void* Buf, uint16_t Samples);
typedef uint16_t (*AudioCoreSpaceLenFunc)(void);

//每个模式混音通路组合及其音效处理的函数结构。
typedef void (*AudioCoreProcessFunc)(void*);//音效处理主函数

#ifdef CFG_AUDIO_WIDTH_24BIT
	#define PCM_DATA_TYPE	int32_t
	typedef enum
	{
		//输入音频位宽，0,16bit，1,24bit
		PCM_DATA_16BIT_WIDTH = 0,
		PCM_DATA_24BIT_WIDTH,
	}PCM_DATA_WIDTH;

#else
	#define PCM_DATA_TYPE	int16_t
#endif

typedef struct _AudioCoreSource
{
	bool						Enable;//通路启停
	bool						Sync;//TRUE:每帧同步，不足时混音通路阻塞。 FALSE:帧操作，Source数据不足时填零，Sink：空间不足时丢弃
	uint8_t						Channels;
	bool						LeftMuteFlag;//静音标志
	bool						RightMuteFlag;//静音标志
	bool						MuteFlagbk;//静音标志
	bool						FrameReady;//播放标志
	bool						Active;//混音组合的数据/空间帧完备，发起AudioProcessMain，此通路激活。
	MIX_NET						Net;//缺省DefaultNet/0,  使能条件下同步帧输入和输出
	AUDIO_ADAPT					Adapt;//缺省:STD,通路采样和微调功能设置
	AudioCoreDataGetFunc		DataGetFunc;//抽数据入口函数
	AudioCoreDataLenFunc		DataLenFunc;//数据sample数 函数
	PCM_DATA_TYPE				*PcmInBuf;	//通路数据帧处理buf
	PCM_DATA_TYPE				*AdaptBuf;	//SRC&SRA缓存buf
#ifdef	CFG_AUDIO_WIDTH_24BIT
	PCM_DATA_WIDTH				BitWidth;//输入音频位宽，0,16bit，1,24bit
	bool						BitWidthConvFlag;//输入音频位宽，是否需要扩充到24bit
#endif
	SRC_ADAPTER					*SrcAdapter; //转采样适配器
	void						*AdjAdapter;//微调适配器
	bool						InitFlag;	 //初始化标志
	bool						PcmBufFlag;	//为1的时候，PcmInBuf由audiocore申请，需要audiocore释放
	bool						SrcAdapterRomFlag;	//转采样用rom代码
}AudioCoreSource;


typedef struct _AudioCoreSink
{
	bool							Enable;//通路启停
	bool							Sync;//TRUE:每帧同步，不足时混音通路阻塞。 FALSE:帧操作，Source数据不足时填零，Sink：空间不足时丢弃
	uint8_t							Channels;
	bool							Active;//混音组合的数据/空间帧完备，发起AudioProcessMain，此通路激活。
	MIX_NET							Net; //缺省DefaultNet/0,  使能条件下同步帧输入和输出
	AUDIO_ADAPT						Adapt;//缺省:STD,通路采样和微调功能设置
	bool							LeftMuteFlag;//静音标志
	bool							RightMuteFlag;//静音标志
	bool							MuteFlagbk;//静音标志
	AudioCoreDataSetFunc			DataSetFunc;//推流入口 内buf->外缓冲搬移函数
	AudioCoreSpaceLenFunc			SpaceLenFunc;//数据填充空间sample数 函数
	uint32_t						Depth;//LenGetFunc()最大采样点深度，用于计算存留数据量
	PCM_DATA_TYPE					*PcmOutBuf;//通路数据帧处理buf
	PCM_DATA_TYPE					*AdaptBuf;//SRC&SRA缓存buf
#ifdef	CFG_AUDIO_WIDTH_24BIT
	PCM_DATA_WIDTH					BitWidth;//输入音频位宽，0,16bit，1,24bit
	bool							BitWidthConvFlag;//输入音频位宽，是否需要位宽转换 24bit <--> 16bit
#endif
	void							*AdjAdapter; //微调适配器
	SRC_ADAPTER						*SrcAdapter; //转采样适配器

	bool							InitFlag;	 //初始化标志
	bool							PcmBufFlag;	//为1的时候，PcmOutBuf由audiocore申请，需要audiocore释放
	bool							SrcAdapterRomFlag;	//转采样用rom代码
}AudioCoreSink;


typedef struct _AudioCoreContext
{
	uint32_t		AdaptIn[(MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE)) / 2];//转采样和软件微调输入buf 4字节对齐便于dmafifo衔接
	uint32_t		AdaptOut[(MAX_FRAME_SAMPLES * SRC_SCALE_MAX * sizeof(PCM_DATA_TYPE))/ 2];//转采样和软件微调输出buf
	MIX_NET			CurrentMix; //当前混音组合，旨在 多通路异步处理和收发。
	uint32_t		FrameReady; //使用位段登记 数据/空间帧可用
	uint32_t		SampleRate[MaxNet];//[DefaultNet]/[0]:主通路中心采样率。
	uint16_t		FrameSize[MaxNet];//[DefaultNet]/[0]:主通路 采样帧，支持独立通路组合SeparateNet及独立采样帧。
	AudioCoreSource AudioSource[AUDIO_CORE_SOURCE_MAX_NUM];
	AudioCoreProcessFunc AudioEffectProcess;			//****流处理入口
	AudioCoreSink   AudioSink[AUDIO_CORE_SINK_MAX_NUM];
}AudioCoreContext;

extern AudioCoreContext		AudioCore;

//typedef void (*AudioCoreProcessFunc)(AudioCoreContext *AudioCore);
/**
 * @func        AudioCoreSourceFreqAdjustEnable
 * @brief       使能系统音频分频微调，使信道之间同步(与异步音源)
 * @param       uint8_t AsyncIndex   异步音频源混音信道编号
 * @param       uint16_t LevelLow   异步音频源混音信道低水位采样点值
 * @param       uint16_t LevelHigh   异步音频源混音信道高水位采样点值
 * @Output      None
 * @return      None
 * @Others
 * Record
 * 1.Date        : 20180518
 *   Author      : pi.wang
 *   Modification: Created function
 */


//随通路数据调整声道配置，通路init时需要配置最大声道。
void AudioCoreSourcePcmFormatConfig(uint8_t Index, uint16_t Format);

void AudioCoreSourceEnable(uint8_t Index);

void AudioCoreSourceDisable(uint8_t Index);

bool AudioCoreSourceIsEnable(uint8_t Index);

void AudioCoreSourceMute(uint8_t Index, bool IsLeftMute, bool IsRightMute);

void AudioCoreSourceUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute);

bool AudioCoreSourceMuteStatusGet(uint8_t Index);

void AudioCoreSourceConfig(uint8_t Index, AudioCoreSource* Source);

void AudioCoreSinkEnable(uint8_t Index);

void AudioCoreSinkDisable(uint8_t Index);

bool AudioCoreSinkIsEnable(uint8_t Index);

void AudioCoreSinkConfig(uint8_t Index, AudioCoreSink* Sink);

void AudioCoreProcessConfig(AudioCoreProcessFunc AudioEffectProcess);


bool AudioCoreInit(void);

void AudioCoreDeinit(void);

void AudioCoreRun(void);

void AudioCoreSourceMuteApply(void);
void AudioCoreSinkMuteApply(void);

#ifdef	CFG_AUDIO_WIDTH_24BIT
PCM_DATA_WIDTH AudioCoreSourceBitWidthGet(uint8_t Index);
#endif

#endif //__AUDIO_CORE_API_H__
