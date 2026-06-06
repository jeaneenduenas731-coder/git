/**
 *******************************************************************************
 * @file    bt_app_spp.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   Spp callback events and actions
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#include "type.h"
#include "debug.h"

#include "bt_manager.h"
#include "bt_config.h"
#include "bt_spp_api.h"
#include "mode_task.h"

#if (BT_SPP_SUPPORT)
void BtSppCallback(BT_SPP_CALLBACK_EVENT event, BT_SPP_CALLBACK_PARAMS * param)
{
	//DBG("boot_BtSppCallback %x\n",event);
	switch(event)
	{
		case BT_STACK_EVENT_SPP_CONNECTED:
			DBG("SPP EVENT:connected\n");
			break;
		
		case BT_STACK_EVENT_SPP_DISCONNECTED:
			DBG("SPP EVENT:disconnect\n");			
			break;
		
		case BT_STACK_EVENT_SPP_DATA_RECEIVED:
			{
			#if 0
				uint16_t i;
				for(i=0;i<param->paramsLen;i++)
				{
					DBG("0x%02x ", param->params.sppReceivedData[i]);
					if((i!=0)&&(i%16 == 0))
						DBG("\n");
				}
			#endif	
			}
			break;
		
		case BT_STACK_EVENT_SPP_DATA_SENT:
			break;
		
		default:
			break;
	}
}

void BtSppHookFunc(void)
{

}

#endif

