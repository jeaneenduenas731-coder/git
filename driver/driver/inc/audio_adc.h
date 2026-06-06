/**
 *******************************************************************************
 * @file    audio_adc.h
 * @brief	模拟∑Δ调制A/D转换器（ASDM）驱动程序接口
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-04-26 13:27:11$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *******************************************************************************
 */

/**
 * @addtogroup AUDIO_ADC
 * @{
 * @defgroup audio_adc audio_adc.h
 * @{
 */

#ifndef __AUDIO_ADC_H__
#define __AUDIO_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

////////////////////////////////////////////
//     Gain Volume Table for PGA(dB)      //
////////////////////////////////////////////
//	Vol // Gain of Mic 单端(600ohm)// Gain of Mic 差分(600ohm)  // Gain of LineIn //
//  00	//     26.5  		       //     28.7		         	//    18.3        //
//  01	//     25.3  		       //     27.3		         	//    17.4        //
//  02	//     24.2  		       //     25.9		         	//    16.4        //
//  03	//     23  		           //     24.5	             	//    15.4        //
//  04	//     21.8  		       //     23.2		         	//    14.4        //
//  05	//     20.7	  		       //     21.8  		        //    13.3        //
//  06	//     19.5	  		       //     20.5  		        //    12.3        //
//  07	//     18.3	  		       //     19.2  		        //    11.1        //
//  08	//     17.1      		   //     17.8    		     	//    10.1        //
//  09	//     15.9	  		       //     16.5  		        //    8.9         //
//  10	//     14.6	  		       //     15.3  		        //    7.9         //
//  11	//     13.4	  		       //     13.9  		        //    6.6         //
//  12	//     12.2	  		       //     12.7  		        //    5.5         //
//  13	//     10.9	  		       //     11.4  		        //    4.2         //
//  14	//     9.7	  		       //     10.1  		        //    3.1         //
//  15	//     8.5	  		       //     8.8  		         	//    1.8         //
//  16	//     7.3      		   //     7.6   		     	//    0.7         //
//  17	//     6      		       //     6.3 		         	//    -0.6        //
//  18	//     4.7      		   //     5   		     		//    -1.78       //
//  19	//     3.5      		   //    3.7    				//    -3          //
//  20	//     2.3      			//    2.5    				//    -4.2        //
//  21	//     1.1      			//    1.2    				//    -5.53       //
//  22	//     0.2      			//    0    					//    -6.6        //
//  23	//     -1.4      			//    -1.3     				//    -8          //
//  24	//     -2.7      			//    -2.6     				//    -9.1        //
//  25	//     -4     				//    -3.8  				//    -10.4       //
//  26	//     -5.2      			//    -5.1     				//    -11.6       //
//  27	//     -6.5      			//    -6.4     				//    -12.9       //
//  28	//     -7.7      			//    -7.6     				//    -14         //
//  29	//     -9      				//    -8.9   				//    -15.4       //
//  30	//     -10.2      			//    -10.1      			//    -16.4       //
//  31	//     -11.3      			//    -11.4      			//    -17.9       //
////////////////////////////////////////////

/**
 * ADC 模块
 */
typedef enum _ADC_MODULE
{
    ADC0_MODULE,
    ADC1_MODULE

} ADC_MODULE;

typedef enum _ADC_CHANNEL
{
    CHANNEL_LEFT,
    CHANNEL_RIGHT

} ADC_CHANNEL;

typedef enum _ADC_DMIC_DOWN_SAMPLLE_RATE
{
    DOWN_SR_64,
	DOWN_SR_128

} ADC_DMIC_DSR;

typedef enum AUDIO_ADC_INPUT
{
	LINEIN_NONE,				//none，用于关闭当前PGA下的channel选择

	LINEIN1_LEFT,
	LINEIN1_RIGHT,

	LINEIN2_LEFT,
	LINEIN2_RIGHT,

	MIC_LEFT
} AUDIO_ADC_INPUT;

/**
 * AGC 模式使能选择
 */
typedef enum _AGC_CHANNEL
{
    AGC_DISABLE	        = 0x00,       /**<禁止AGC功能 */
    AGC_RIGHT_ONLY      = 0x01,       /**<仅右通道使能AGC功能*/
    AGC_LEFT_ONLY       = 0x02,       /**<仅左通道使能AGC功能*/
    AGC_STEREO_OPEN     = 0x03        /**<左右双通道使能AGC功能*/

} AGC_CHANNEL;

/**
 * ADC 位宽选择
 */
typedef enum AUDIO_BitWidth
{
	ADC_WIDTH_16BITS,
	ADC_WIDTH_24BITS
} AUDIO_BitWidth;

/**
 * MIC 模式选择
 */
typedef enum AUDIO_Mode
{
	Single,   //单端
	Diff     //差分
} AUDIO_Mode;

/**
 * AUX 模块选择
 */
typedef enum LINEIN_MODULE
{
	LINEIN1_MODULE,
	LINEIN2_MODULE
} LINEIN_MODULE;

/**
 * @brief  ADC 模块左右通道使能选择
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn     TRUE,左通道能使; FALSE,左通道关闭
 * @param  IsRightEn    TRUE,右通道能使; FALSE,右通道关闭
 * @return 无
 * @Note 如果完全使能ADC还需要调用AudioADC_Enable API
 */
void AudioADC_LREnable(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC 模块使能（总开关）
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 * @Note 如果完全使能ADC还需要调用AudioADC_LREnable API
 */
void AudioADC_Enable(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块关闭
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_Disable(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块是否交换左右通道输入
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsSwap       TRUE,左右通道交换输入; FALSE,左右通道正常输入
 * @return 无
 */
void AudioADC_ChannelSwap(ADC_MODULE ADCModule, bool IsSwap);

/**
 * @brief  ADC 模块高通滤波器截止频率参数配置
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  Coefficient  滤波器系数，12bit位宽，默认值0xFFE。
 *   @arg  Coefficient = 0xFFE  48k采样率下在20Hz处衰减-1.5db。
 *   @arg  Coefficient = 0xFFC  48k采样率下在40Hz处衰减-1.5db。
 *   @arg  Coefficient = 0xFFD  32k采样率下在40Hz处衰减-1.5db。
 * @return 无
 * @Note 该函数必须在AudioADC_Enable()函数调用之后
 */
void AudioADC_HighPassFilterConfig(ADC_MODULE ADCModule, uint16_t Coefficient);

/**
 * @brief  ADC 模块高通滤波是否使能（去除直流偏量）
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsHpfEn      TRUE,能使高通滤波器，FALSE,关闭高通滤波器
 * @return 无
 */
void AudioADC_HighPassFilterSet(ADC_MODULE ADCModule, bool IsHpfEn);

/**
 * @brief  ADC 模块清除寄存器和内存中的数值
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_Clear(ADC_MODULE ADCModule);

/**
 * @brief  adc 位宽设置
 * @param  ADCModule: ADC0_MODULE,  ADC1_MODULE
 * @param  BitWidth  ADC_LENGTH_24BITS：24位; ADC_LENGTH_16BITS：16位
 * @return 无
 */
void AudioADC_WidthSet(ADC_MODULE ADCModule, AUDIO_BitWidth BitWidth);

/**
 * @brief  ADC 模块采样率配置
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  SampleRate   ADC采样率值，10种采样率
 * @return 无
 */
void AudioADC_SampleRateSet(ADC_MODULE ADCModule, uint32_t SampleRate);

/**
 * @brief  获取ADC 模块采样率配置值
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 当前采样率配置值
 */
uint32_t AudioADC_SampleRateGet(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块淡入淡出时间设置
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  FadeTime     淡入淡出时间, 单位:Ms
 * @return 无
 * @Note   建议时间为10Ms，时间不能设置为0，如果想关闭淡入淡出功能请调用函数AudioADC_FadeDisable();
 */
void AudioADC_FadeTimeSet(ADC_MODULE ADCModule, uint8_t FadeTime);

/**
 * @brief  ADC 模块淡入淡出功能使能
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_FadeEnable(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块淡入淡出功能禁用
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_FadeDisable(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块数字静音控制，左右通道分别独立控制
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  LeftMuteEn   TRUE,左通道静音使能; FALSE,左通道取消静音
 * @param  RightMuteEn  TRUE,右通道静音使能; FALSE,右通道取消静音
 * @return 无
 * @Note   该函数内部不带延时，配置硬件寄存器之后立即退出。如果需要等待静音动作完成，则要在函数外部做延时
 */
void AudioADC_DigitalMute(ADC_MODULE ADCModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief  ADC 模块软件静音控制，左右通道分别独立控制
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  LeftMuteEn   TRUE,左通道静音使能; FALSE,左通道取消静音
 * @param  RightMuteEn  TRUE,右通道静音使能; FALSE,右通道取消静音
 * @note   调用此API之前必须已经成功调用SysTickInit
 * @return 无
 * @Note   该函数内部带延时，如果是静音操作，会延时等待音量降低到0之后退出该函数。
 */
void AudioADC_SoftMute(ADC_MODULE ADCModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief  功能重置
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_FuncReset(ADC_MODULE ADCModule);

/**
 * @brief  寄存器重置
 * @param  无
 * @return 无
 */
void AudioADC_RegReset(void);

/**
 * @brief  ADC 模块音量设置
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  LeftVol      左声道音量值，0x00:静音, 0x001:-72dB, 0xFFF:0dB
 * @param  RightVol     右声道音量值，0x00:静音, 0x001:-72dB, 0xFFF:0dB
 * @return 无
 */
void AudioADC_VolSet(ADC_MODULE ADCModule, uint16_t LeftVol, uint16_t RightVol);


/**
 * @brief  ADC 模块音量设置（左右声道独立设置）
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  ChannelSel   左右声道选择，0x00: 无；0x1：左声道；0x2：右声道
 * @param  Vol     		音量值，0x00:静音, 0x001:-72dB, 0xFFF:0dB、
 * @note   当ChannelSel为3时，同时选择了左右声道，此时Vol配置对左右声道均有效，左右声道音量值一致
 * @return 无
 */
void AudioADC_VolSetChannel(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, uint16_t Vol);

/**
 * @brief  ADC 模块音量获取
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  *LeftVol      左声道音量值，0x00:静音, 0x001:-72dB, 0xFFF:0dB
 * @param  *RightVol     右声道音量值，0x00:静音, 0x001:-72dB, 0xFFF:0dB
 * @return 无
 */
void AudioADC_VolGet(ADC_MODULE ADCModule, uint16_t* LeftVol, uint16_t* RightVol);

/**
 * @brief  ADC PAG通道选择,
 * @param  ADCModule	0,ADC0模块; 1,ADC1模块
 * @param  ChannelSel   CHANNEL_LEFT:Left;	CHANNEL_RIGHT:Right
 * @param  InputSel 	PGA输入通路选择。具体见AUDIO_ADC_INPUT枚举值含义
 * @note   ADC1只支持MIC，没有AUX选通
 * @return 无
 */
void AudioADC_PGASel(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel);

/**
 * @brief  ADC PAG增益配置
 * @param  ADCModule	0,ADC0模块; 1,ADC1模块
 * @param  ChannelSel   CHANNEL_LEFT:Left;	CHANNEL_RIGHT:Right
 * @param  InputSel 	InputSel PGA输入通路选择。具体见AUDIO_ADC_INPUT枚举值含义
 * @param  Gain 		PGA增益设置。配置范围【0-31】。
 * @return 无
 */
void AudioADC_PGAGainSet(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel, uint16_t Gain);

/**
 * @brief  ADC模块PGA模块模式选择
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  AUDIOMode  	Single:单端; Diff:差分      默认为差分输入
 * @note 差分模式只有ADC1模块支持，且只有Left声道
 * @return 无
 */
void AudioADC_PGAMode(ADC_MODULE ADCModule, AUDIO_Mode AUDIOMode);

/**
 * @brief  AGC模块PGA模块过零检测使能
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn:  	左通道使能
 * @param  IsRightEn: 	右通道使能
 * @return 无
 */
void AudioADC_PGAZeroCrossEnable(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ASDM模块使能DMIC功能
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_DMICEnable(ADC_MODULE ADCModule);

/**
 * @brief  ASDM模块禁能DMIC功能
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_DMICDisable(ADC_MODULE ADCModule);

/**
 * @brief  DMIC降采样率倍数选择
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  DownSampleRate    DOWN_SR_64,64倍降采样; DOWN_SR_128,128倍降采样
 * @return 无
 * @note   当DownSampleRate为DOWN_SR_128时，确保DMIC输出时钟(128*fs)不大于DMIC设备限制
 */
void AudioADC_DMICDownSampleSel(ADC_MODULE ADCModule, ADC_DMIC_DSR DownSampleRate);

/**
 * @brief  Dynamic-Element-Matching enable signal
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn  	左通道使能
 * @param  IsRightEn	右通道使能
 * @return 无
 */
void AudioADC_DynamicElementMatch(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  查询ADC采样过程是否数据溢出
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @return TREU：有溢出过
 */
bool AudioADC_IsOverflow(ADC_MODULE ADCModule);

/**
 * @brief 清除ADC采样过程是否数据溢出标志
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_OverflowClear(ADC_MODULE ADCModule);

/**
 * @brief  ADC 模块AGC模块通道选择
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn  	左通道使能
 * @param  IsRightEn	右通道使能
 * @return 无
 */
void AudioADC_AGCChannelSel(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC 模块用于整理AGC增益
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  GainOffset 	增益偏移设置。【8 ~ 15】-->【-4dB ~ -0.5dB】;【0 ~ 7】-->【0dB ~ 3.5dB】.
 * @return 无
 */
void AudioADC_AGCGainOffset(ADC_MODULE ADCModule, uint8_t GainOffset);

/**
 * @brief  ADC 模块AGC模块增益最大水平
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  MaxLevel     AGC模块增益最大水平.【0 ~ 31】-->【-3 ~ -34dB】
 * @return 无
 */
void AudioADC_AGCMaxLevel(ADC_MODULE ADCModule, uint8_t MaxLevel);

/**
 * @brief  AGC模块增益目标水平
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  TargetLevel	AGC模块增益目标水平.【0 ~ 31】-->【-3 ~ -34dB】
 * @return 无
 */
void AudioADC_AGCTargetLevel(ADC_MODULE ADCModule, uint8_t TargetLevel);

/**
 * @brief  AGC模模块可调节的最大增益
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  MaxGain		AGC模块最大增益.【0 ~ 31】-->【 32.75 ~ -6】,step:-1.25dB.
 * @return 无
 */
void AudioADC_AGCMaxGain(ADC_MODULE ADCModule, uint8_t MaxGain);

/**
 * @brief  AGC模模块可调节的最小增益
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  MinGain		AGC模块最小增益.【0 ~ 31】-->【 32.75 ~ -6dB】,step:-1.25dB.
 * @return 无
 */
void AudioADC_AGCMinGain(ADC_MODULE ADCModule, uint8_t MinGain);

/**
 * @brief  AGC模块帧时间
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  FrameTime	帧时间设置。 单位：ms。 范围【 1 ~ 4096】
 * @return 无
 */
void AudioADC_AGCFrameTime(ADC_MODULE ADCModule, uint16_t FrameTime);

/**
 * @brief  AGC模块保持时间，开始AGC算法自适应增益之前的保持时间。
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  HoldFrames	AGC开始算法之前的保持时间。单位：Frames。范围【0 ~ 31】
 * @note   HoldTime = HoldFrames * FrameTime
 * @return 无
 */
void AudioADC_AGCHoldFrames(ADC_MODULE ADCModule, uint32_t HoldFrames);

/**
 * @brief  AGC模块当输入信号太大时，AGC增益衰减的步进时间设置。
 * @param  ADCModule    	0,ADC0模块; 1,ADC1模块
 * @param  AttackStepTime 	AGC增益增强的步进时间，单位为ms,范围为1 ~ 4096 ms
 * @return 无
 */
void AudioADC_AGCAttackStepTime(ADC_MODULE ADCModule, uint16_t AttackStepTime);

/**
 * @brief  AGC模块当输入信号太小时，AGC增益增强的步进时间设置。
 * @param  ADCModule    	0,ADC0模块; 1,ADC1模块
 * @param  DecayStepTime 	AGC增益增强的步进时间，单位为ms,范围为1 ~ 4096 ms
 * @return 无
 */
void AudioADC_AGCDecayStepTime(ADC_MODULE ADCModule, uint16_t DecayStepTime);

/**
 * @brief  AGC模块AGC噪声阈值设置
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  NoiseThreshold： 噪音阈值设置,范围：【0 ~ 31】对应值：【-90dB ~ -28dB】，step：2dB
 * 						      默认为 01111,即-60 dB
 * @return 无
 */
void AudioADC_AGCNoiseThreshold(ADC_MODULE ADCModule, uint8_t NoiseThreshold);

/**
 * @brief  AGC模块AGC模块噪声模式选择,
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  NoiseMode：	0: ADC输出数据电平与噪声阈值判断，确定该输入是否为噪声；
 * 						1: ADC输入数据电平与噪声阈值判断，确定该输入是否为噪声。
 * @return 无
 */
void AudioADC_AGCNoiseMode(ADC_MODULE ADCModule, uint8_t NoiseMode);

/**
 * @brief  AGC模块AGC模块噪声Gate功能使能
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  NoiseGateEnable： 0:禁止噪声Gate功能; 1:使能噪声Gate功能
 * @return 无
 */
void AudioADC_AGCNoiseGateEnable(ADC_MODULE ADCModule, bool NoiseGateEnable);

/**
 * @brief  AGC模块AGC模块噪声Gate模式选择
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  NoiseGateMode 	0:  当检查到过零信号时，PCM数据mute/unmute
 *							1:  PCM数据立即mute/unmute
 * @return 无
 */
void AudioADC_AGCNoiseGateMode(ADC_MODULE ADCModule, uint8_t NoiseGateMode);

/**
 * @brief  AGC模块AGC模块噪声保持时间设置。
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @param  NoiseHoldTime	噪声持续NoiseHoldTime后，噪声相关算法开始执行。单位（ms）
 * @return 无
 */
void AudioADC_AGCNoiseHoldTime(ADC_MODULE ADCModule, uint8_t NoiseHoldTime);

/**
 * @brief  AGC模块获取AGC增益
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return AGC增益
 * @note   返回的值并不是AGC的实际增益值，AGC实际增益值与寄存器值
 * 		        之间的转换见AGC实际增益值与寄存器值对应表
 */
uint8_t AudioADC_AGCGainGet(ADC_MODULE ADCModule);

/**
 * @brief  AGC模块获取AGC模块静音信息
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return AGC模块静音标志
 */
uint8_t AudioADC_AGCMuteGet(ADC_MODULE ADCModule);

/**
 * @brief  AGC模块获取AGC模块更新标志位
 * @param  ADCModule    0,ADC0模块; 1,ADC1模块
 * @return AGC模块更新标志位
 */
uint8_t AudioADC_AGCUpdateFlagGet(ADC_MODULE ADCModule);

/**
 * @brief  AGC模块清除AGC模块更新标志位
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @return 无
 */
void AudioADC_AGCUpdateFlagClear(ADC_MODULE ADCModule);

/**
 * @brief  BIAS上电
 * @param  无
 * @return 无
 */
void AudioADC_BIASPowerOn(void);

/**
 * @brief  ASDM比较器电流控制信号,只有ASDM0有ASDM_IBSEL_CMP_L
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftCmp 左声道配置值; RightCmp 右边声道配置值
 * @return 无
 */
void AudioADC_ComparatorIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftCmp,  uint8_t RightCmp);

/**
 * @brief  OTA1的偏置电流选择信号
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftIBSEL 左声道电流值; RightCmp 右声道电流值
 * @return 无
 */
void AudioADC_OTA1IBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  OTA2的偏置电流选择信号
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftIBSEL 左声道电流值; RightCmp 右声道电流值
 * @return 无
 */
void AudioADC_OTA2IBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA PD控制
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn 左声道PGAPowerUp; IsRightEn 右声道PGAPowerUp
 * @return 无
 */
void AudioADC_PGAPowerUp(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC PowerUp
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn 左声道PowerUp; IsRightEn 右声道PowerUp
 * @return 无
 */
void AudioADC_PowerUp(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  absmute 强制静音，放在内部,B5只有MIC有ABSMute
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn 左声道PGAAbsMute; IsRightEn 右声道PGAAbsMutep
 * @return 无
 */
void AudioADC_PGAAbsMute(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  锁存器延迟选择信号
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftIBSEL 左声道电流值; IsRightEn 右声道电流值
 * @return 无
 */
void AudioADC_LatchDelayIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA电流控制信号
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftIBSEL 左声道电流值; IsRightEn 右声道电流值
 * @return 无
 */
void AudioADC_PGAIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA MUTE信号
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  IsLeftEn 左声道PGAMute使能; IsRightEn 左声道PGAMute使能
 * @return 无
 */
void AudioADC_PGAMute(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  缓冲器电流控制
 * @param  ADCModule 	0,ADC0模块; 1,ADC1模块
 * @param  LeftBuf 左声道电流值; RightBuf 右声道电流值
 * @return 无
 */
void AudioADC_BufferIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftBuf,  uint8_t RightBuf);

/**
 * @brief  vmid初始化
 * @param  无
 * @return 无
 * @note   只使用audio_adc,不使用dac的时候,可以使用AudioADC_VMIDInit替代AudioDAC_AllPowerOn,来对vmid进行上电。AudioADC_VMIDInit和AudioDAC_AllPowerOn不要重复使用
 */
void AudioADC_VMIDInit(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__AUDIO_ADC_H__

/**
 * @}
 * @}
 */
