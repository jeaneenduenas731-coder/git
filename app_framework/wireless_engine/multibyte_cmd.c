#include <stdlib.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "app_config.h"
#include "multibyte_cmd.h"

#ifdef CFG_2T1R_SEND_MULTIBYTE_CMD_EN

MultiByteCMD MultibyteCmd;

uint8_t user_CmdBuf[8]={0x55, 0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

//底层返回值0表示success，所以取反
uint8_t wireless_user_cmd_tx_result_get(void)
{
	return !(MultibyteCmd.result);
}

void wireless_user_cmd_tx_result_clean(void)
{
	MultibyteCmd.result = 1;
}

////////TX API///////////////////////////
//2T1R RX端作为多命令字的TX,需要主动发起命令字传送
void wireless_user_cmd_data_updata(uint8_t* cmd_data, uint8_t len)
{
	MultibyteCmd.len = len;
	memcpy(MultibyteCmd.DataBuf, cmd_data, len);
}

void wireless_user_cmd_send(void)
{
	wireless_2t1r_master_send_cmd(MultibyteCmd.DataBuf, MultibyteCmd.len, wireless_user_cmd_tx_result_callback);
}

//底层回调函数，内部不要有打印
void wireless_user_cmd_tx_result_callback(unsigned char result)
{
	//DBG("result = %d\n", result);
	MultibyteCmd.result = result;
}


////////RX API///////////////////////////
//底层回调函数，内部不要有打印，否则整个射频异常
void wireless_user_cmd_rx_data_callback(unsigned char result,unsigned char* data, unsigned char len)
{
	MultibyteCmd.result = result;
	MultibyteCmd.len = len;
	memcpy(MultibyteCmd.DataBuf, data, len);
}

///////2T1R TX端作为多命令字的RX，需要初始化回调函数
void wireless_user_cmd_rx_int(void)
{
	wireless_2t1r_slave_reg_cmd_data_cb(wireless_user_cmd_rx_data_callback);
}
//用户获取到cmd之后做处理，留个用户
//如果结果错误，可以通过正向的1字节命名字通知无线另一端
void wireless_user_cmd_rx_data_get(void)
{
	uint8_t i;
	//if(wireless_user_cmd_tx_result_get())
	{
		for(i=0;i<CMD_LEN;i++)
		{
			DBG("%x\n", MultibyteCmd.DataBuf[i]);
		}
	}
	//user doing...
}

#endif
