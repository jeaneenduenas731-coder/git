/**
 **************************************************************************************
 * @file    i2s_interface.h
 * @brief   audio i2s interface
 *
 * @author  
 * @version V1.1.0
 *
 * $Created: 2019-01-04 14:01:05$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__I2S_INTERFACE_H__
#define		__I2S_INTERFACE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

#include "i2s.h"

typedef struct __I2SParamCt
{
	uint8_t          IsMasterMode;    //0: master, 1: slave

	uint32_t         SampleRate;      //sample rate

	I2S_DATA_FORMAT  I2sFormat;       //I2S_DATA_FORMAT

	I2S_DATA_LENGTH  I2sBits;         //I2S_DATA_LENGTH
	
	uint8_t          I2sTxRxEnable;   // 0: disable, 1: only tx enable, 2: only rx enable, 3: tx & rx enable
	
	//DMA
	uint8_t          TxPeripheralID;  //i2s tx peripheralID

	void            *TxBuf;           //i2s tx buf

	uint16_t         TxLen;           //i2s tx len
	
	uint8_t          RxPeripheralID;  //i2s rx peripheralID

	void            *RxBuf;           //i2s rx buf

	uint16_t         RxLen;           //i2s rx len
	

} I2SParamCt;

void 	 AudioI2S_Init(I2S_MODULE Module, I2SParamCt *ct);
void 	 AudioI2S_DeInit(I2S_MODULE Module);
uint8_t  AudioI2S_MasterModeGet(I2S_MODULE Module);
I2S_DATA_LENGTH AudioI2S_WordLengthGet(I2S_MODULE Module);
uint16_t AudioI2S_DataLenGet(I2S_MODULE Module);
uint16_t AudioI2S_DataGet(I2S_MODULE Module, void* Buf, uint16_t Len);
uint16_t AudioI2S_DataSpaceLenGet(I2S_MODULE Module);
void     AudioI2S_DataSet(I2S_MODULE Module, void *Buf, uint32_t Len);


uint16_t AudioI2S0_DataLenGet(void);
uint16_t AudioI2S1_DataLenGet(void);
uint16_t AudioI2S0_DataGet(void* Buf, uint16_t Len);
uint16_t AudioI2S1_DataGet(void* Buf, uint16_t Len);
uint16_t AudioI2S0_DataSpaceLenGet(void);
uint16_t AudioI2S1_DataSpaceLenGet(void);
uint16_t AudioI2S0_DataSet(void *Buf, uint16_t Len);
uint16_t AudioI2S1_DataSet(void *Buf, uint16_t Len);

uint16_t AudioI2S0_TX_DataLenGet(void);
uint16_t AudioI2S1_TX_DataLenGet(void);

void AudioI2S_SampleRateChange(I2S_MODULE Module,uint32_t SampleRate);

//0:master mode ;1:slave mode 外设未接不要配slave
#define	I2S_MASTER_MODE			0
#define	I2S_SLAVE_MODE			1
//i2s_port: 0 ---> i2s0  1 ---> i2s1
//gpio_mode: GPIO复用功能配置
//gpio_index：GPIO引脚号
#define SET_I2S_GPIO_VAL(i2s_port,gpio_mode,gpio_index)						((i2s_port<<16)|(gpio_mode<<8)|(gpio_index))
#define GET_I2S_I2S_PORT(val)												((val>>16)&0x01)
#define GET_I2S_GPIO_MODE(val)												((val>>8)&0x0f)
#define GET_I2S_GPIO_INDEX(val)												((val)&0x1f)
#define GET_I2S_GPIO_PORT(val)												(1<<((val)&0x1f))
//i2s_mode: I2S_MASTER_MODE I2S_SLAVE_MODE
#define SET_I2S_GPIO_ALL_VAL(i2s_mode,i2s_port,gpio_mode,gpio_index)		((i2s_mode<<24)|(i2s_port<<16)|(gpio_mode<<8)|(gpio_index))
#define GET_I2S_MODE(val)													((val>>24)&0x01)

//mclk out gpio I2S_MASTER_MODE
#define I2S0_MCLK_OUT_A24									SET_I2S_GPIO_ALL_VAL(I2S_MASTER_MODE,0,0x08,24)
#define I2S1_MCLK_OUT_A0									SET_I2S_GPIO_ALL_VAL(I2S_MASTER_MODE,1,0x06,0)
#define I2S1_MCLK_OUT_A6									SET_I2S_GPIO_ALL_VAL(I2S_MASTER_MODE,1,0x08,6)
#define I2S1_MCLK_OUT_A7									SET_I2S_GPIO_ALL_VAL(I2S_MASTER_MODE,1,0x08,7)
//mclk in gpio  I2S_SLAVE_MODE
#define I2S0_MCLK_IN_A24									SET_I2S_GPIO_ALL_VAL(I2S_SLAVE_MODE,0,0x02,24)
#define I2S1_MCLK_IN_A0										SET_I2S_GPIO_ALL_VAL(I2S_SLAVE_MODE,1,0x02,0)
#define I2S1_MCLK_IN_A6										SET_I2S_GPIO_ALL_VAL(I2S_SLAVE_MODE,1,0x02,6)
#define I2S1_MCLK_IN_A7										SET_I2S_GPIO_ALL_VAL(I2S_SLAVE_MODE,1,0x04,7)
//lrclk gpio
#define I2S0_LRCLK_A20										SET_I2S_GPIO_VAL(0,0x07,20)
#define I2S1_LRCLK_A7										SET_I2S_GPIO_VAL(1,0x03,7)
#define I2S1_LRCLK_A20										SET_I2S_GPIO_VAL(1,0x08,20)
#define I2S1_LRCLK_A28										SET_I2S_GPIO_VAL(1,0x02,28)
#define I2S0IN_I2S1OUT_LRCLK_A20							SET_I2S_GPIO_VAL(0,0x04,20)
#define I2S0OUT_I2S1IN_LRCLK_A20							SET_I2S_GPIO_VAL(0,0x05,20)
#define I2S0IN_I2S1IN_LRCLK_A20								SET_I2S_GPIO_VAL(0,0x06,20)
//bclk gpio
#define I2S0_BCLK_A21										SET_I2S_GPIO_VAL(0,0x05,21)
#define I2S1_BCLK_A9										SET_I2S_GPIO_VAL(1,0x02,9)
#define I2S1_BCLK_A21										SET_I2S_GPIO_VAL(1,0x06,21)
#define I2S1_BCLK_A29										SET_I2S_GPIO_VAL(1,0x02,29)
#define I2S0IN_I2S1OUT_BCLK_A21								SET_I2S_GPIO_VAL(0,0x02,21)
#define I2S0OUT_I2S1IN_BCLK_A21								SET_I2S_GPIO_VAL(0,0x03,21)
#define I2S0IN_I2S1IN_BCLK_A21								SET_I2S_GPIO_VAL(0,0x04,21)
//dout gpio
#define I2S0_DOUT_A22										SET_I2S_GPIO_VAL(0,0x09,22)
#define I2S0_DOUT_A23										SET_I2S_GPIO_VAL(0,0x08,23)
#define I2S1_DOUT_A10										SET_I2S_GPIO_VAL(1,0x06,10)
#define I2S1_DOUT_A30										SET_I2S_GPIO_VAL(1,0x09,30)
#define I2S1_DOUT_A31										SET_I2S_GPIO_VAL(1,0x09,31)
//din gpio
#define I2S0_DIN_A22										SET_I2S_GPIO_VAL(0,0x03,22)
#define I2S0_DIN_A23										SET_I2S_GPIO_VAL(0,0x03,23)
#define I2S1_DIN_A10										SET_I2S_GPIO_VAL(1,0x02,10)
#define I2S1_DIN_A30										SET_I2S_GPIO_VAL(1,0x02,30)
#define I2S1_DIN_A31										SET_I2S_GPIO_VAL(1,0x02,31)

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__ADC_INTERFACE_H__

