
#ifndef _BB_API_H_
#define _BB_API_H_


/***************************************************
 *
 **************************************************/
void Bluetooth_common_init(void* params);
void Bt_init(void* params);
void Ble_init(void);

/***************************************************
 * BT debug infor
 * 在BtStackServiceEntrance循环前调用
 **************************************************/
void BtDebugEnable(void);

void SetBluetoothMode(uint8_t mode);

/***************************************************
 *
 **************************************************/
void rw_main(void);
void rwip_reset(void);

/***************************************************
 * get frequence offset value
 **************************************************/
signed char GetFreqOffsetValue(void);

/***************************************************
 * set bt frequence trim value
 * input: value (0x00~0x1f)
 **************************************************/
void BtSetFreqTrim(unsigned char value);

/***************************************************
 * set bt link supervision timeout
 * input: timeout: 0x0001~0xffff  (default:0x7D00 -- 20s)
 * link supervision timeout = time * 0.625ms (Range: 0.625ms - 40.9s)
 **************************************************/
void BtSetLinkSupervisionTimeout(unsigned int timeout);

/***************************************************
 * get rssi value
 **************************************************/
signed char GetRssiValue(void);

/***************************************************
 * get the company ID of the remote device 
 **************************************************/
unsigned short GetRemoteCompanyID(void);

/***************************************************
 * set rf tx power max level(max: 23 -- 8dbm)
 * set rf page tx power level(max: 23 -- 8dbm)
 * Compatible with B1X to adjust power
 * 23 - txgain:1: maxinum: 8dbm based on dcdc output voltage
  * 22 - txgain:2:  6dbm
  * 21 - txgain:3:  5dbm
  * 20 - txgain:4:  4dbm
  * 19 - txgain:5:  3dbm
  * 18 - txgain:6:  2dbm
  * 17 - txgain:7:  1dbm
  * 16 - txgain:8:  0dbm
 **************************************************/
void SetRfTxPwrMaxLevel(unsigned char level, unsigned char pageLevel);

/***************************************************
 * get the rf chip id
 **************************************************/
unsigned short GetRfChipId(void);


/***************************************************
 * get the rf pll lock state
 * return: -1 = pll unlock(error)
 * 0 = ok
 **************************************************/
signed char GetRfPllLockState(void);

/***************************************************
 * rf power Up/Down
 **************************************************/
void RF_PowerDownByHw(void);
void RF_PowerUpBySw(void);
void RF_PowerDownBySw(void);

void Bt_Set_sniff_clk_src(uint8_t srcClk);// srcClk:0 RC CLK   1:osc 分频到32k
int32_t BtSniffSleepTime(void);

/***************************************************
 * sniff use Hosc/RC
 **************************************************/
void sniff_rc_init_set();//rc initial set
void sniff_cntclk_set(int8_t set);//0:32K 1:32768Hz -- bluetooth count clk set

//user api
void BtCntClkSet(void);

#endif //_BB_API_H_
