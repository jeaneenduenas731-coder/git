#include <stdio.h>
#include <string.h>
#include "type.h"
#include "app_config.h"
#include "flash_table.h"
#include "spi_flash.h"
#include "flash_interface.h"
#include "debug.h"

/***********************************************************************************
 索引表使用说明（flash_table_init）：
 1、flash容量：
 	1.1、flash地址0xD4位置读取容量信息（4/8/16/32/64/128Mb）。
	1.2、1.1不合法直接读取flash实际容量（8/16/32Mb）。
	1.3、1.2不合法设置容量为8Mb。
	1.4、如修改容量请修改宏 FLASH_CAPACTITY_TYPE
 2、索引表地址：
	2.1、描述1中定义的flash容量减去12KB位置（末尾8KB兼容旧SDK不做改动）。
	2.2、旧SDK没有索引表。
 3、用户数据地址：
	3.1、按名称name在索引表中找到对应地址offset：
		typedef struct _FlashTablePackage
		{
			uint8_t id;
			uint8_t enable_flag;
			uint8_t name[MAX_PACKAGE_NAME_LEN];
			uint8_t offset[4];
			uint8_t check_sum;
			uint8_t	reserve[9];
		} FlashTablePackage;
	3.2、在索引表中没有找到返回0。
		flash_table_read(REMIND_PACKAGE_NAME);
	3.3、8Mb SDK默认使用的数据，没有索引表/未在索引表中找到按 FLASH_DATA_MAP 排列。
		//末尾8KB参量化区域(兼容旧SDK做的保留)
		uint8_t	reserve_1		[1024 * 4];		//size:4KB
		uint8_t	reserve_2		[1024 * 4];		//size:4KB

		uint8_t	flash_table		[1024 * 4];		//size:4KB	索引表
		uint8_t	sys_parameter	[1024 * 4];		//size:4KB	8MB SDK参量化数据
		uint8_t	bt_data			[1024 * 8];		//size:8KB  bt数据 配对记录等
		uint8_t	bp_data			[1024 * 8];		//size:8KB	breakpoint数据
 4、output下文件说明
	4.1、flash.img			flash镜像文件（打包了全部数据），download工具下载
	4.2、BT_Audio_APP.bin	编译器编译产生，烧录代码或者加密使用。
	4.3、main_merge.mva		升级文件包（打包了全部数据）。download工具下载/生产工具使用
***********************************************************************************/

#define MAX_PACKAGE_NAME_LEN		16
//包长度32
#define ONE_PAGEAGE_LEN				32	
typedef struct _FlashTablePackage
{
	uint8_t id;
	uint8_t enable_flag;
	uint8_t name[MAX_PACKAGE_NAME_LEN];
	uint8_t offset[4];
	uint8_t check_sum;
	uint8_t	reserve[9];
} FlashTablePackage;

struct
{
	uint32_t 	remind_addr;
#if defined(CFG_WIRELESS_EN) && WIRELESS_BP_EN
	uint32_t	wireless_bp_addr;
#endif
	uint32_t	bp_data_addr;
	uint32_t	bt_data_addr;
	uint32_t	user_config_addr;
	uint32_t	bt_config_addr;
	uint32_t	sys_parameter_addr;
	uint32_t	effect_data_addr;

	uint32_t	flash_capacity;
	bool		flash_table_valid;
}flash_table_info;

#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
uint32_t	rec_addr_start;
#endif

static const struct
{
	char *		package_name;	//包名、定义的名称和ini文件中的名称需要完全一致
	uint32_t * 	addr;			//地址
	uint32_t	offset;			//未找到情况下，默认偏移
}FlashTableReadMap[] =
{
	{"sys_parameter",	&flash_table_info.sys_parameter_addr,		SYS_PARA_OFFSET},
	{"bp_data",			&flash_table_info.bp_data_addr,				BP_DATA_OFFSET},
	{"bt_data",			&flash_table_info.bt_data_addr,				BT_DATA_OFFSET},
#if defined(CFG_WIRELESS_EN) && WIRELESS_BP_EN
	{"wireless_bp",		&flash_table_info.wireless_bp_addr,			BP_WIRELESS_OFFSET},
#endif
	{"remind",			&flash_table_info.remind_addr,				0},
#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
	{"effect_data",		&flash_table_info.effect_data_addr,			EFFECT_DATA_OFFSET},
#endif
#if defined(CFG_FUNC_RECORD_EXTERN_FLASH_EN) && !defined(USE_EXTERN_FLASH_SPACE)
	{"rec_data",		&rec_addr_start,				0},
#endif
};

uint32_t flash_table_read(char * name)
{
	uint8_t len;
	uint32_t i;
	uint32_t addr;
	FlashTablePackage package;

	len = strlen(name);
	if(len > MAX_PACKAGE_NAME_LEN)
		len = MAX_PACKAGE_NAME_LEN;

	for(i=0;i<(FLASH_TABLE_SIZE/ONE_PAGEAGE_LEN);i++)
	{
		addr = flash_table_info.flash_capacity - FLASH_TABLE_OFFSET + (i * ONE_PAGEAGE_LEN);
		if(SpiFlashRead(addr,(uint8_t*)&package,ONE_PAGEAGE_LEN,10) != FLASH_NONE_ERR)
		{
			return 0;
		}
		if(memcmp(package.name,name,len) == 0)//找到
		{
			int32_t addr;
			uint8_t check = 0;
			uint8_t *p = (uint8_t*)&package;

			addr = package.offset[3];
			addr <<= 8;
			addr |= package.offset[2];
			addr <<= 8;
			addr |= package.offset[1];
			addr <<= 8;
			addr |= package.offset[0];
#if 0
			addr += flash_table_info.flash_capacity;
#endif
			for(i=0;i<22;i++)
			{
				check +=  p[i];
			}
			//APP_DBG("\nflash_table_read = %x %x  %lx %s\n",check,package.check_sum,addr,package.name);
			if(addr < 0 || check != package.check_sum)
				return  0;
			else
				return addr;
		}
	}
	return 0;
}

void flash_table_check(void)
{
	uint32_t addr;
	FlashTablePackage package;

	flash_table_info.flash_table_valid = 0;

	addr = flash_table_info.flash_capacity - FLASH_TABLE_OFFSET;
	if(SpiFlashRead(addr,(uint8_t*)&package,ONE_PAGEAGE_LEN,10) != FLASH_NONE_ERR)
	{
		return;
	}	

	//判断第一条记录是否有效 
	{
		uint8_t check = 0;
		uint8_t *p = (uint8_t*)&package;
		uint32_t i;

		for(i=0;i<22;i++)
		{
			check +=  p[i];
		}
		if(check != package.check_sum)
			return ;
	}	

	//判断末尾是否是 do not use this
	addr = flash_table_info.flash_capacity - FLASH_TABLE_OFFSET + FLASH_TABLE_SIZE - ONE_PAGEAGE_LEN;
	if(SpiFlashRead(addr,(uint8_t*)&package,ONE_PAGEAGE_LEN,10) != FLASH_NONE_ERR)
	{
		return;
	}	

	uint8_t *p = (uint8_t*)&package;
	if(memcmp(p+2,"do not use this",15) == 0)
	{
		flash_table_info.flash_table_valid = 1;
	}
}

bool flash_table_is_valid(void)
{
	return flash_table_info.flash_table_valid;
}

void flash_table_init(void)
{
	SPI_FLASH_INFO *ret = SpiFlashInfoGet();
	uint8_t			buf[2],i;
	uint8_t			valid;

	memset(&flash_table_info,0,sizeof(flash_table_info));
	valid = 0;
	
	if(SpiFlashRead(FLASH_CAPACTITY_ADDR,buf,1,10) == FLASH_NONE_ERR)
	{
		switch(buf[0])
		{
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
			case 128:
				flash_table_info.flash_capacity = (1024 * 1024 * buf[0])/8;
				valid = 1;
				break;
			default:
				flash_table_info.flash_capacity = ret->Capacity;
				break;
		}
	}
	else
		flash_table_info.flash_capacity = ret->Capacity;

	if( valid == 0 &&
		flash_table_info.flash_capacity != 0x100000 &&
		flash_table_info.flash_capacity != 0x200000 &&
		flash_table_info.flash_capacity != 0x300000 &&
		flash_table_info.flash_capacity != 0x400000
	  )
	{
		flash_table_info.flash_capacity = FLASH_DEFAULT_CAPACTITY;
	}

	flash_table_check();

	APP_DBG("read flash Capacity = 0x%lx 0x%lx,FlashTable: %d\n",ret->Capacity,flash_table_info.flash_capacity,flash_table_info.flash_table_valid);

	for(i = 0;i < sizeof(FlashTableReadMap)/sizeof(FlashTableReadMap[0]);i++)
	{
		*(FlashTableReadMap[i].addr) = flash_table_read(FlashTableReadMap[i].package_name);
		if(*(FlashTableReadMap[i].addr) == 0 && FlashTableReadMap[i].offset != 0)
		{
			*(FlashTableReadMap[i].addr) = flash_table_info.flash_capacity - FlashTableReadMap[i].offset;
		}
		APP_DBG("%s = 0x%lx\n",FlashTableReadMap[i].package_name,*(FlashTableReadMap[i].addr));
	}
#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
	#ifdef USE_EXTERN_FLASH_SPACE
		rec_addr_start = CFG_PARA_RECORDS_FLASH_BEGIN_ADDR;
	#else
		APP_DBG("rec_addr_start = 0x%lx\n",rec_addr_start);
	#endif
#endif
#if 1
	flash_table_info.user_config_addr = (flash_table_info.flash_capacity - USER_CONFIG_OFFSET);
	flash_table_info.bt_config_addr = (flash_table_info.flash_capacity - BT_CONFIG_OFFSET);
#else
	flash_table_info.user_config_addr = ret->Capacity - USER_CONFIG_OFFSET;
	flash_table_info.bt_config_addr = ret->Capacity - BT_CONFIG_OFFSET;
#endif

	APP_DBG("user_config_addr = 0x%lx\n",flash_table_info.user_config_addr);
	APP_DBG("bt_config_addr = 0x%lx\n",flash_table_info.bt_config_addr);
}

uint32_t get_remind_addr(void)
{
	return flash_table_info.remind_addr;
}

#if defined(CFG_WIRELESS_EN) && WIRELESS_BP_EN
uint32_t get_wireless_bp_addr(void)
{
	return flash_table_info.wireless_bp_addr;
}
#endif

uint32_t get_bp_data_addr(void)
{
	return flash_table_info.bp_data_addr;
}

uint32_t get_bt_data_addr(void)
{
	return flash_table_info.bt_data_addr;
}

uint32_t get_user_config_addr(void)
{
	return flash_table_info.user_config_addr;
}

uint32_t get_bt_config_addr(void)
{
	return flash_table_info.bt_config_addr;
}

uint32_t get_sys_parameter_addr(void)
{
	return flash_table_info.sys_parameter_addr;
}

uint32_t get_effect_data_addr(void)
{
	return flash_table_info.effect_data_addr;
}
