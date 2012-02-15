#ifndef __JZSYS_H__
#define __JZSYS_H__

#define _UCOS_

#if defined(_UCOS_)
# define MPALLOC(size)     av_malloc(size)
# define MPFREE(p)         av_free(p)
#else
# define MPALLOC(size)     jz4740_alloc_frame(0, size)
# define MPFREE(p)         
#endif


#if defined(_UCOS_)
#define get_phy_addr(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#define tran_addr_for_aux(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#endif

/*aux get phy addr*/
#if defined(_UCOS_)
#define get_phy_addr_aux(vaddr) (unsigned int)(((unsigned int)(vaddr)) & 0x1FFFFFFF)
#elif defined(JZC_RTL_SIM)
#define get_phy_addr_aux(vaddr) (unsigned int)((((unsigned int)(vaddr)) & 0x0FFFFFFF) | 0x20000000)
#else
#define get_phy_addr_aux(vaddr) (vaddr)
#endif

#if 0
/*main-cpu push addr for aux*/
#if defined(_UCOS_) || defined(JZC_RTL_SIM)
# define tran_addr_for_aux(vaddr) (vaddr)
#else
# define tran_addr_for_aux(vaddr) (unsigned int)(((unsigned int)(vaddr) - (unsigned int)frame_buffer) + phy_fb)
#endif
#endif

#define CACHE_FLUSH_BASE 0x80000000
#define jzc_dcache_wb()				\
  {						\
    int i, va;					\
    va = CACHE_FLUSH_BASE;			\
    for(i=0; i<512; i++) {			\
      i_cache(0x1,va,0);			\
      va += 32;					\
    }						\
    i_sync();					\
  }

#endif /*__JZSYS_H__*/
   
