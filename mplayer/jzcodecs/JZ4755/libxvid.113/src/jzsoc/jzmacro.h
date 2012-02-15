/*
  JZ SOC Macros are defined here
*/
#ifndef __JZ_MACRO_H__
#define __JZ_MACRO_H__

/*
  Notes: OS relate macro:
     --_UCOS_:  in _UCOS_ os platform
     --JZC_FPGA_TEST: in JZ FPGA platform
     -- in linux mplayer package environment: both _UCOS_ and JZC_FPGA_TEST should be undefined  
 */

//#define JZC_FPGA_TEST
#define _UCOS_
#ifdef _UCOS_
# undef malloc
# define malloc uc_malloc
# undef free
# define free uc_free
#endif

#if defined(JZC_FPGA_TEST) && defined(_UCOS_)
# error "JZC_FPGA_TEST and _UCOS_ defined simultaneously!"
#endif

//test environment
/*sys related macro*/
#define JZC_SYS
//#define JZC_DCORE_ARCH

//#define JZC_CRC_VER
//#define JZC_MAIN_PMON
#if !defined(JZC_MAIN_PMON)
# define J1_PMON_ON(func)
# define J1_PMON_OFF(func)
# define J1_PMON_CREAT(func)
# define J1_PMON_CLEAR(func)
#endif
#define STA_CCLK
//#define STA_DCC
//#define STA_ICC
//#define STA_UINSN
//#define STA_INSN
//#define STA_UINSN
#if defined(STA_INSN)
#   define J1_PMON_FILE_NAME "jz4750e.j1_pmon.insn"
#elif defined(STA_UINSN)
#   define J1_PMON_FILE_NAME "jz4750e.j1_pmon.uinsn"
#elif defined(STA_CCLK)
#   define J1_PMON_FILE_NAME "jz4750e.j1_pmon.cclk"
#elif defined(STA_DCC)
#   define J1_PMON_FILE_NAME "jz4750e.j1_pmon.dcc"
#elif defined(STA_ICC)
#   define J1_PMON_FILE_NAME "jz4750e.j1_pmon.icc"
#else
#   error "If JZC_MAIN_PMON defined, one of STA_INSN/STA_CCLK/STA_DCC/STA_ICC must be defined!"
#endif

/*xvid alloc function define according to JZOS*/
#ifdef JZC_SYS

#ifdef _UCOS_
# define XVIDMALLOC xvid_malloc
# define XVIDFREE   xvid_free
#else
# define XVIDMALLOC(size,align) jz4740_alloc_frame(align,size)
# define XVIDFREE(p)            jz4740_free_devmem()
#endif //_UCOS_

#else
# define XVIDMALLOC xvid_malloc
# define XVIDFREE   xvid_free
#endif

#if defined(_UCOS_)
#define get_phy_addr(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#elif defined(JZC_RTL_SIM)
#define get_phy_addr(vaddr) (unsigned int)((((unsigned int)(vaddr)) & 0x0FFFFFFF) | 0x20000000)
#elif defined(JZC_FPGA_TEST)
#define get_phy_addr(vaddr) (unsigned int)(((unsigned int)(vaddr) - (unsigned int)frame_buffer) + phy_fb)
#endif

/*aux get phy addr*/
#if defined(_UCOS_)
#define get_phy_addr_aux(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#elif defined(JZC_RTL_SIM)
#define get_phy_addr_aux(vaddr) (unsigned int)((((unsigned int)(vaddr)) & 0x0FFFFFFF) | 0x20000000)
#else
#define get_phy_addr_aux(vaddr) (vaddr)
#endif

/*main-cpu push addr for aux*/
#if defined(_UCOS_) || defined(JZC_RTL_SIM)
# define tran_addr_for_aux(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#elif defined(JZC_FPGA_TEST)
# define tran_addr_for_aux(vaddr) (unsigned int)(((unsigned int)(vaddr) - (unsigned int)frame_buffer) + phy_fb)
#else
# define tran_addr_for_aux(vaddr) get_phy_addr(vaddr)
#endif


#ifdef JZC_DBG
# define JZPRINT(x, ...) printf("[%s:%d]\t", __FILE__, __LINE__); printf(x,__VA_ARGS__);
#else
# define JZPRINT(x, ...)
#endif

/* test mxu macro*/
#define JZC_MXU_OPT
#define JZC_TCSM_OPT
#define JZC_MC_OPT
#define JZC_AUX_OPT
#define JZC_TCSM_OPT_1
#define JZC_TCSM0_OPT
#define JZC_MDMA1_OPT
#define JZC_AUX_MC_SYNC
#define JZC_AUX_GP1_SYNC
#endif /*__JZ_MACRO_H__*/
