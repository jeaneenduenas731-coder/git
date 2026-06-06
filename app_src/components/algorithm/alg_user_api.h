#ifndef  _ALGORITHM_USER_API_H_
#define  _ALGORITHM_USER_API_H_

unsigned char AccessCredential(unsigned char *param);

void Alg_RamInit(void);

unsigned char Get_AlgVersion(unsigned char * string);

unsigned char Alg_UserTest(unsigned char* buf ,int len);

#include "spi_flash.h"

typedef struct _PLATFORM_INTERFACE_ALG_T
{
	SPI_FLASH_ERR_CODE (*AlgFlashInit)(uint32_t flash_clk, SPIFLASH_IO_MODE IOMode, bool HpmEn, FSHC_CLK_MODE ClkSrc);
	void (*AlgFlashErase)(ERASE_TYPE_ENUM erase_type, uint32_t index, bool IsSuspend);
	SPI_FLASH_ERR_CODE (*AlgFlashRead)(uint32_t StartAddr, uint8_t* Buffer, uint32_t Length, uint32_t TimeOut);
	SPI_FLASH_ERR_CODE (*AlgFlashWrite)(uint32_t Addr, uint8_t	*Buffer, uint32_t 	Length, uint32_t IsSuspend);
}PLATFORM_INTERFACE_ALG_T;

extern const PLATFORM_INTERFACE_ALG_T AlgFlashApi;

#endif
