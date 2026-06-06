#include <nds32_intrinsic.h>
#include "debug.h"
#include "core_d1088.h"
#include "remap.h"
#include "flash_boot.h"
#include "app_config.h"
#include "bt_config.h"
#include "bt_em_config.h"

#define _MMM(A) ".long "#A" \n\n"
#define MMM(A) _MMM(A)

#define _M(A) 	".byte "#A" \n\n"
#define M(A) 	_M(A)

static void Default_Handler();

#pragma weak GpioInterrupt     = Default_Handler//0
#pragma weak WakeupInterrupt   = Default_Handler//1
#pragma weak PowerkeyInterrupt = Default_Handler//2
#pragma weak UsbInterrupt      = Default_Handler//3
#pragma weak FFTInterrupt      = Default_Handler//4
#pragma weak SWInterrupt       = Default_Handler//5
//#pragma weak SystickInterrupt  = Default_Handler//6
#pragma weak Timer2Interrupt   = Default_Handler//7
#pragma weak Timer3Interrupt   = Default_Handler//8
#pragma weak Timer4Interrupt   = Default_Handler//9
#pragma weak Timer5Interrupt   = Default_Handler//10
#pragma weak Timer6Interrupt   = Default_Handler//11
#pragma weak RtcInterrupt	   = Default_Handler//12
#pragma weak HosccntInterrupt  = Default_Handler//13
#pragma weak rwip_isr		   = Default_Handler//14
#pragma weak rwbt_isr      	   = Default_Handler//15
#pragma weak BLE_Interrupt	   = Default_Handler//16
#pragma weak SPIM_Interrupt    = Default_Handler//17
#pragma weak SPIS_Interrupt    = Default_Handler//18
#pragma weak UART0_Interrupt   = Default_Handler//19
#pragma weak UART1_Interrupt   = Default_Handler//20
#pragma weak SDIO0_Interrupt   = Default_Handler//21
#pragma weak SPDIF0_Interrupt  = Default_Handler//22
#pragma weak IR_Interrupt	   = Default_Handler//23
#pragma weak I2sInterrupt      = Default_Handler//24
#pragma weak SPDIF1_Interrupt  = Default_Handler//25
#pragma weak Timer7Interrupt   = Default_Handler//26
#pragma weak Timer8Interrupt   = Default_Handler//27
#pragma weak I2C_Interrupt     = Default_Handler//28
#pragma weak DMA_D_Interrupt   = Default_Handler//29
#pragma weak DMA_T_Interrupt   = Default_Handler//30
#pragma weak DMA_E_Interrupt   = Default_Handler//31

__attribute__((unused))
static void Default_Handler()
{
	while (1) ;
}
__attribute__((weak))
char *pcTaskGetTaskName( void *pt )
{
	return NULL;
}
__attribute__((unused))
void ExceptionCommHandler(unsigned stack, unsigned exception_num)
{

	unsigned int mask_itype,mask_ipc;
	unsigned int *pstack;

#ifdef CFG_FUNC_DEBUG_USE_TIMER
	extern void PrintfAllInBuffer(void);
	PrintfAllInBuffer();
#endif
	  pstack = (unsigned int *)stack;
	  APP_DBG("Error exception happened\r\n");

	  if(pcTaskGetTaskName(NULL))
	  {
		  APP_DBG("task name:%s\n", pcTaskGetTaskName(NULL));
	  }

	  mask_itype = __nds32__mfsr(NDS32_SR_ITYPE);
	  mask_ipc = __nds32__mfsr(NDS32_SR_IPC);
	  mask_itype &= 0x0F;



	  APP_DBG("PC  = 0x%08x\r\n",mask_ipc);
	  APP_DBG("R0  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R1  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R2  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R3  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R4  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R5  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R6  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R7  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R8  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R9  = 0x%08x\r\n",*pstack++);
	  APP_DBG("R10 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R11 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R12 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R13 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R14 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R15 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R16 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R17 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R18 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R19 = 0x%08x\r\n",*pstack++);
	  APP_DBG("R20 = 0x%08x\r\n",*pstack++);
	  APP_DBG("lp  = 0x%08x\r\n",*pstack++);
	  APP_DBG("sp  = 0x%08x\r\n",(*pstack++)-2048);
	  APP_DBG("exception num = %d\n", exception_num);
	  APP_DBG("Error type num: %d\r\n", mask_itype);
	//APP_DBG("Error exception happened\r\n");
	{
		extern void NVIC_SystemReset(void);
		NVIC_SystemReset();
	}
	while(1) ;
}
extern void boot_ram_init(void);
extern void __c_init_rom(void);
void __c_init()
{

/* Use compiler builtin memcpy and memset */
#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;
//#ifndef USE_DBG_CODE
//	boot_ram_init();
//#endif
	__c_init_rom();
	/* data section will be copied before we remap.
	 * We don't need to copy data section here. */
	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;

	/* Copy data section to RAM */
	size = &_edata - &__data_start;
	MEMCPY(&__data_start, &__data_lmastart, size);

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
	return;
}

void __cpu_init()
{
	unsigned int tmp;
	extern char __sdk_code_start;
	/* turn on BTB */
//	tmp = 0x0;
//	__nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

    /* turn off RTP */
    tmp = __nds32__mfsr(NDS32_SR_MISC_CTL);
    tmp |= 0x02;
    __nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

	tmp = __nds32__mfsr(NDS32_SR_MMU_CTL);
	tmp |= 0x800000;
	__nds32__mtsr(tmp,NDS32_SR_MMU_CTL);
	
	/* disable all hardware interrupts */
	__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01)
		__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#endif

#if defined(CFG_EVIC)
	/* set EVIC, vector size: 4 bytes, base: 0x0 */
	__nds32__mtsr(0x1<<13, NDS32_SR_IVB);
#else
# if defined(USE_C_EXT)
	/* If we use v3/v3m toolchain and want to use
	 * C extension please use USE_C_EXT in CFLAGS
	 */
#ifdef __NDS32_ISA_V3__
	/* set IVIC, vector size: 4 bytes, base: 0x0 */
	__nds32__mtsr(0x0, NDS32_SR_IVB);
#else
	/* set IVIC, vector size: 16 bytes, base: 0x0 */
	__nds32__mtsr(0x1<<14, NDS32_SR_IVB);
#endif
# else
	/* set IVIC, vector size: 4 bytes, base: 0x0
	 * If we use v3/v3m toolchain and want to use
	 * assembly version please don't use USE_C_EXT
	 * in CFLAGS */
	__nds32__mtsr((uint32_t)(&__sdk_code_start), NDS32_SR_IVB);
# endif
#endif
	/* Set PSW INTL to 0 */
	tmp = __nds32__mfsr(NDS32_SR_PSW);
	tmp = tmp & 0xfffffff9;
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	/* Set PSW CPL to 7 to allow any priority */
	tmp = tmp | 0x70008;
#endif
	__nds32__mtsr_dsb(tmp, NDS32_SR_PSW);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	/* Check interrupt priority programmable*
	* IVB.PROG_PRI_LVL
	*      0: Fixed priority       -- no exist ir18 1r19
	*      1: Programmable priority
	*/
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01) {
		/* Set PPL2FIX_EN to 0 to enable Programmable
	 	* Priority Level */
		__nds32__mtsr(0x0, NDS32_SR_INT_CTRL);
		/* Check IVIC numbers (IVB.NIVIC) */
		if ((__nds32__mfsr(NDS32_SR_IVB) & 0x0E)>>1 == 5) {	// 32IVIC
			/* set priority HW9: 0, HW13: 1, HW19: 2,
			* HW#-: 0 */
			__nds32__mtsr(0xAAAAAEAB, NDS32_SR_INT_PRI);
			__nds32__mtsr(0x2AAAAAAA, NDS32_SR_INT_PRI2);
		} else {
			/* set priority HW0: 0, HW1: 1, HW2: 2, HW3: 3
			* HW4-: 0 */
			__nds32__mtsr(0x000000e4, NDS32_SR_INT_PRI);
		}
	}
#endif
	/* enable FPU if the CPU support FPU */
#if defined(__NDS32_EXT_FPU_DP__) || defined(__NDS32_EXT_FPU_SP__)
	tmp = __nds32__mfsr(NDS32_SR_FUCOP_EXIST);
	if ((tmp & 0x00010001) == 0x00010001) {
		tmp = __nds32__mfsr(NDS32_SR_FUCOP_CTL);
		__nds32__mtsr_dsb((tmp | 0x1), NDS32_SR_FUCOP_CTL);

		/* Denormalized flush-to-Zero mode on */
		tmp =__nds32__fmfcsr();
		tmp |= (1 << 12);
		__nds32__fmtcsr(tmp);
		__nds32__dsb();
	}
#endif

	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_PEND2), NDS32_SR_INT_PEND2);  //Çå³ýpending

	return;
}

#define HSP_CTL_offHSPEN        0
#define HSP_CTL_offSCHM         1
#define HSP_CTL_offSUSER        2
#define HSP_CTL_offUSER         3
#define HSP_ENABLE              (1 << HSP_CTL_offHSPEN)
#define HSP_DISABLE             (0 << HSP_CTL_offHSPEN)
#define HSP_SCHM_OVERFLOW       (0 << HSP_CTL_offSCHM)
#define HSP_SCHM_TOPRECORD      (1 << HSP_CTL_offSCHM)
#define HSP_SUPERUSER           (1 << HSP_CTL_offSUSER)
#define HSP_USER                (1 << HSP_CTL_offUSER)
void HardwareStackProtectEnable(void)
{
//	if (!(__nds32__mfsr(NDS32_SR_MSC_CFG) & (1 << 27)))
//	{
//		//APP_DBG("CPU does NOT support HW Stack protection/recording.\n");
//	}
//	else
//	{
//		//__nds32__mtsr(__nds32__mfsr(NDS32_SR_HSP_CTL) & ~0x0f, NDS32_SR_HSP_CTL);
//		__nds32__mtsr(__nds32__mfsr(NDS32_SR_HSP_CTL) & ~HSP_ENABLE, NDS32_SR_HSP_CTL);
//
//		//__nds32__mtsr(0x20003000, NDS32_SR_SP_BOUND);
//		__nds32__mtsr((TCM_SRAM_START_ADDR+(TCM_SIZE*1024)), NDS32_SR_SP_BOUND);
//
//		__nds32__mtsr(HSP_ENABLE | HSP_SCHM_OVERFLOW | HSP_SUPERUSER, NDS32_SR_HSP_CTL);
//
//		//APP_DBG("CPU support HW Stack protection/recording.\n");
//	}
}
void SetFshcCacheDecryptAddr(uint32_t addr)
{
#define REG_FSHC_CACHE_DECRYPT_ADDR  (*(volatile unsigned long *) 0x40007064)
	REG_FSHC_CACHE_DECRYPT_ADDR = addr;
}

void DBUS_Access_Area_Init(uint32_t start_addr)
{
	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;
	int temp = (int)(&__data_lmastart + (&_edata - &__data_start));
	SetFshcCacheDecryptAddr(start_addr < temp ? temp : start_addr);
}


void Chip_LDOOffDisable(void);
void EnableIDCache_EmSize(uint32_t EM_SIZE);
void __init()
{
/*----------------------------------------------------------
   !!  Users should NOT add any code before this comment  !!
------------------------------------------------------------*/
	__cpu_init();
	//EnableIDCache_EmSize(BB_EM_SIZE); //add by peter
#if CFG_D16K_MEM16K_EN
	extern void I16K_D16K_Set(void);
	I16K_D16K_Set();
#else
	EnableIDCache();
#endif
	HardwareStackProtectEnable(); //add by peter
	//Chip_LDOOffDisable();//add by Sam, 20180521
	__c_init();     //copy data section, clean bss
}

__attribute__ ((section(".stub_section"),used)) __attribute__((naked))
void stub(void)
{
__asm__ __volatile__(

    		".long 0xFFFFFFFF \n\n"	//0xA4
    		".long 0xFFFFFFFF \n\n" //0xA8
    		".long 0xFFFFFFFF \n\n" //0xAC
    		MMM(CONST_DATA_ADDR)	//0xB0 constant data @ 0x8C
    		MMM(USER_DATA_ADDR)		//0xB4 user data
    		M(CFG_SDK_VER_CHIPID)	//0xB8	
			M(CFG_SDK_MAJOR_VERSION)//0xB9	
			M(CFG_SDK_MINOR_VERSION)//0xBA	
			M(CFG_SDK_PATCH_VERSION)//0xBB
    		".long 0xFFFFFFFF \n\n"	//0xBC code crc
//    		".long 0xB0BEBDC9 \n\n"	//0xC0 magic number
			MMM(CFG_SDK_MAGIC_NUMBER)//0xC0 magic number
    		".long 0x00000706 \n\n"	//0xC4 32KHz external oscillator input/output capacitance calibration value
    		".long 0xFFFFFFFF \n\n"	//0xC8 fast code crc	@ 0xA4
    		".long 0xFFFFFFFF \n\n"	//0xCC fast code crc	@ 0xA4
    		".long 0xFFFFFFFF \n\n"	//0xD0 fast code crc	@ 0xA4
//    		".long 0xFFFFFFFF \n\n"	//0xD4 fast code crc	@ 0xA4
			M(FLASH_CAPACTITY_TYPE)
			M(0xFF)
			M(0xFF)
			M(0xFF)
			MMM(AUDIO_EFFECT_ADDR)	//0xD8 audio effect data
			".long 0xFFFFFFFF \n\n"	//0xDC code size
			".long 0xFFFFFFFF \n\n"	//0xE0 flashboot
		    ".rept (0x100-0xE4)/4 \n\n"
    		".long 0xFFFFFFFF \n\n"
		    ".endr \n\n"
			".long 0x00FFFFFF \n\n"
		    ".short 0xFFFF \n\n"

    );
}


const uint32_t MPUConfigTable[8][7] =
{
	{MPU_ENTRY_ENABLE, 0,  		     0x1FFFFFFF,	        0, 				        CACHEABILITY_WRITE_BACK, 		EXECUTABLE_USER,	ACCESS_READ 	},
	{MPU_ENTRY_ENABLE, 0x20000000, 	 0x40000+CFG_D16KMEM16K_RAM_SIZE-BB_EM_SIZE,	0x20000000,		        CACHEABILITY_WRITE_THROUGH, 	EXECUTABLE_USER,	ACCESS_RW		},
	{MPU_ENTRY_ENABLE, 0x40000000, 	 0x1FFFFFFF,	        0x40000000,		        CACHEABILITY_DEVICE,			EXECUTABLE_USER,	ACCESS_RW		},
//	{MPU_ENTRY_ENABLE, 0x60000000, 	 0x1FFFFFFF,	        0x60000000,		        CACHEABILITY_WRITE_THROUGH,		EXECUTABLE_USER,	ACCESS_RW		},
	{MPU_ENTRY_ENABLE, 0x80000000,	 BB_EM_SIZE,		    0x20040000+CFG_D16KMEM16K_RAM_SIZE-BB_EM_SIZE, 	CACHEABILITY_DEVICE,			EXECUTABLE_USER,	ACCESS_RW		},
	{MPU_ENTRY_DISABLE},
	{MPU_ENTRY_DISABLE},
	{MPU_ENTRY_DISABLE},
	{MPU_ENTRY_DISABLE},
};
