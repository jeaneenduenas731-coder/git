#include "alg_user_api.h"
#include "debug.h"

const PLATFORM_INTERFACE_ALG_T AlgFlashApi =
{
	.AlgFlashInit = SpiFlashInit,
	.AlgFlashErase = SpiFlashErase,
	.AlgFlashRead = SpiFlashRead,
	.AlgFlashWrite = SpiFlashWrite,
};

void AlgUserDemo(void)
{
	unsigned char buf[40];
	int alg_ret,i;

	APP_DBG("\n*************************ALG_KEY1_EXAMPLE*****************************\n");

	alg_ret = AccessCredential(0);
	APP_DBG("Example 3 :AccessCredential Done!!! ret=%d\n",alg_ret);

	if(Get_AlgVersion(buf))
		APP_DBG("AlgVersion: %s\n",buf);
	else
		APP_DBG("Get_AlgVersion Fail!!\n");

	for(i=0;i<sizeof(buf);i++)
		buf[i] = i;

	if(Alg_UserTest(buf,sizeof(buf)))
	{
		for(i=0;i<sizeof(buf);i++)
			APP_DBG("%02x ",buf[i]);
	}
	else
		APP_DBG("Alg_UserTest Fail!!\n");

	APP_DBG("\n*************************ALG_KEY1_EXAMPLE END*************************\n\n");
}


