
#include <debug.h>
#include "type.h"
#include <string.h>

void GPIOAX_getcnt(unsigned char cnt,unsigned int index1,unsigned int index2)
{
#if 1
	unsigned char iii = 0;

	GPIO_PortAModeSet((1<<index1),0);
	GPIO_RegOneBitClear(GPIO_A_IE,(1<<index1));
	GPIO_RegOneBitSet(GPIO_A_OE,(1<<index1));
	GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index1));

	GPIO_PortAModeSet((1<<index2),0);
	GPIO_RegOneBitClear(GPIO_A_IE,(1<<index2));
	GPIO_RegOneBitSet(GPIO_A_OE,(1<<index2));
	GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index2));

	for(iii=0;iii<8;iii++)
	{
		GPIO_RegOneBitSet(GPIO_A_OUT,(1<<index2));
		if((cnt>>iii)&0x1)
			GPIO_RegOneBitSet(GPIO_A_OUT,(1<<index1));
		else
			GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index1));
		GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index2));
	}

	GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index1));
#endif
}

void GPIOAX_SET(char set,unsigned int index)
{
#if 0
	GPIO_PortAModeSet((1<<index),0);
	GPIO_RegOneBitClear(GPIO_A_IE,(1<<index));
	GPIO_RegOneBitSet(GPIO_A_OE,(1<<index));
	GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index));

	if(set)
	{
		GPIO_RegOneBitSet(GPIO_A_OUT,(1<<index));
	}
	else
	{
		GPIO_RegOneBitClear(GPIO_A_OUT,(1<<index));
	}
#endif
}

uint8_t DBG_Global(char * str,char **fmt, ...)
{
	if('^'==**fmt)
	{
		*fmt += 1;
		return TRUE;
	}
	if(0!=(strstr(str,"device/")))
#ifdef	DEVICE_MODULE_DEBUG
		printf("[DEVICE]:");
#else
		return FALSE;
#endif
	else if(0!=(strstr(str,"services/")))
#ifdef SERVICE_MODULE_DEBUG
		printf("[SERVICE]:");
#else
		return FALSE;
#endif
	else if(0!=(strstr(str,"apps/")))
	{
		if(0!=(strstr(str,"media")))
#ifdef	MEDIA_MODULE_DEBUG
			printf("[APP_MEDIA]:");
#else
				return FALSE;
#endif
		else if(0!=(strstr(str,"bt")))
#ifdef	BT_MODULE_DEBUG
			printf("[APP_BT]:");
#else
				return FALSE;
#endif
		else if(0!=(strstr(str,"main_task")))
#ifdef	MAINTSK_MODULE_DEBUG
			printf("[APP_MAIN]:");
#else
				return FALSE;
#endif
		else if(0!=(strstr(str,"usb_audio")))
#ifdef	USBAUDIO_MODULE_DEBUG
			printf("[APP_USBAUDIO]:");
#else
				return FALSE;
#endif
		else if(0!=(strstr(str,"waiting")))
#ifdef	WAITING_MODULE_DEBUG
			printf("[APP_WAITING]:");
#else
			return FALSE;
#endif
	}

	return TRUE;
}

#ifdef INT_TGL
	const uint8_t DbgIntList[] = TGL_INT_LIST;
	#define TGL_INT_NUM		(sizeof(DbgIntList))
	void dbg_int_in(uint32_t int_num)
	{
		uint8_t i = 0;
		for(i = 0; i < TGL_INT_NUM; i++)
		{
			if(int_num == DbgIntList[i])
			{
				DbgPortOn(TGL_OFFSET + i);
				break;
			}
		}
		if(i == TGL_INT_NUM)
		{
			DbgPortOn(TGL_OFFSET + TGL_INT_NUM);
		}
	}

	void dbg_int_out(uint32_t int_num)
	{
		uint8_t i = 0;
		for(i = 0; i < TGL_INT_NUM; i++)
		{
			if(int_num == DbgIntList[i])
			{
				DbgPortOff(TGL_OFFSET + i);
				break;
			}
		}
		if(i == TGL_INT_NUM)
		{
			DbgPortOff(TGL_OFFSET + TGL_INT_NUM);
		}
	}
#endif //INT_TGL

#ifdef TASK_TGL
	#include "FreeRTOSConfig.h"
	const uint8_t DbgTaskList[][configMAX_TASK_NAME_LEN] = TGL_TASK_LIST;
	#define TGL_TASK_NUM	(sizeof(DbgTaskList)/(configMAX_TASK_NAME_LEN))
	#ifdef INT_TGL
		#define TASK_OFFSET	(TGL_OFFSET + TGL_INT_NUM + 1)
	#else
		#define TASK_OFFSET	(TGL_OFFSET)
	#endif
	void DbgTaskTGL_in()
	{
		int i = 0;

		for(i=0;i<TGL_TASK_NUM;i++)
		{
			if(memcmp( pcTaskGetTaskName(NULL),DbgTaskList[i],strlen(DbgTaskList[i])) == 0 )
			{
				DbgPortOn(TASK_OFFSET + i);
				break;
			}
		}
	}
	void DbgTaskTGL_out()
	{
		int i = 0;

		for(i=0;i<TGL_TASK_NUM;i++)
		{
			if(memcmp( pcTaskGetTaskName(NULL),DbgTaskList[i],strlen(DbgTaskList[i])) == 0 )
			{
				DbgPortOff(TASK_OFFSET + i);
				break;
			}
		}
	}
#endif //TASK_TGL

#ifdef DBG_IO_TGL
	typedef struct _DBG_IO_ID
	{
		char		PortBank;
		uint8_t		IOIndex;

	} DBG_IO_ID;
	const DBG_IO_ID DbgIoList[] = TGL_PORT_LIST;
	#define DBG_IO_NUM (sizeof(DbgIoList) / sizeof(DBG_IO_ID))


	void DbgPortInit(void)
	{
		if(DBG_IO_NUM != 0)
		{
			uint8_t i;
			APP_DBG("DbgTgl 0~%d:", DBG_IO_NUM - 1);
			for(i = 0; i < DBG_IO_NUM; i++)
			{
				if((DbgIoList[i].PortBank == 'A' && DbgIoList[i].IOIndex >= 31) || (DbgIoList[i].PortBank == 'B' && DbgIoList[i].IOIndex >= 7))
				{
					APP_DBG("%d Port Error!", i);
					continue;
				}
				DbgPortOff(i);
				GPIO_RegOneBitSet(PORT_OE_REG(DbgIoList[i].PortBank), BIT(DbgIoList[i].IOIndex));
				GPIO_RegOneBitClear(PORT_IE_REG(DbgIoList[i].PortBank), BIT(DbgIoList[i].IOIndex));
				SET_MOD_REG(DbgIoList[i].PortBank, BIT(DbgIoList[i].IOIndex), 0);
				APP_DBG("%c%d ",toupper(DbgIoList[i].PortBank), DbgIoList[i].IOIndex);
			}
			APP_DBG("\n");
		}
	}


	void DbgPortOn(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return;
		GPIO_RegOneBitSet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
	}

	void DbgPortOff(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return;
		GPIO_RegOneBitClear(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
	}

	void DbgPortToggle(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return;
		if(GPIO_RegOneBitGet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex)))
		{
			GPIO_RegOneBitClear(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
		}
		else
		{
			GPIO_RegOneBitSet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
		}
	}

	void DbgPortRise(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return;
		if(!GPIO_RegOneBitGet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex)))
		{
			GPIO_RegOneBitSet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
		}

	}

	void DbgPortDown(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return;
		if(GPIO_RegOneBitGet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex)))
		{
			GPIO_RegOneBitClear(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
		}
	}

	bool DbgPortGet(uint8_t Index)
	{
		if(Index >= DBG_IO_NUM)
			return FALSE;
		return GPIO_RegOneBitGet(PORT_OUT_REG(DbgIoList[Index].PortBank), BIT(DbgIoList[Index].IOIndex));
	}
#endif //DBG_IO_TGL
