/*
  JZ SOC Macros are defined here
*/
#ifndef __JZ_MACRO_H__
#define __JZ_MACRO_H__

//test environment
#define _UCOS_
/*sys related macro*/
#define JZC_SYS
#define JZC_DCORE_ARCH
#define JZC_MC_OPT
//#define JZC_DAC_OPT
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

/*sys-frame-buffer allocate*/
#ifdef _UCOS_
# define JZALLOC(size,align) memalign(align,size);
# define JZFREE(p)           free(p);
#else
# define JZALLOC(size,align) jz4740_alloc_frame(align,size);
# define JZFREE(p)           jz4740_free_devmem();
#endif //_UCOS_

/*xvid alloc function define according to JZOS*/
#ifdef JZC_SYS
# define XVIDMALLOC JZALLOC
# define XVIDFREE   JZFREE 
#else
# define XVIDMALLOC xvid_malloc
# define XVIDFREE   xvid_free
#endif

#ifdef JZC_DBG
# define JZPRINT(x, ...) printf("[%s:%d]\t", __FILE__, __LINE__); printf(x,__VA_ARGS__);
#else
# define JZPRINT(x, ...)
#endif

#define JZ4750

#ifdef JZ4750
#define JZC_MXU_OPT
#define JZC_PRE_OPT
#define JZC_DCSCALE_OPT
#endif
#endif /*__JZ_MACRO_H__*/
