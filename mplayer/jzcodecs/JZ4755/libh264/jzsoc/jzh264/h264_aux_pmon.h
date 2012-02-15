
#ifndef __H264_AUX_PMON_H__
#define __H264_AUX_PMON_H__

#define AUX_PMON_CTRL 0x13800010 //ctrl: 0, diable; 1, enable
#define AUX_PMON_CLK 0x13800014  // clock counter (low 32 bits)
#define AUX_PMON_BR 0x13800018   // branch counter (low 32 bits)
#define AUX_PMON_BLK 0x1380001C  // pipeline stall counter (low 32 bits)
#define AUX_PMON_NRDY 0x13800020 // pipeline frozen counter (low 32 bits)
#define AUX_PMON_EVTH 0x13800024 // counter high (31:24, CLK; 23:16, NRDY; 15:8, BR; 7:0, BLK)

#define aux_pmon_enable() \
  ({ *((volatile int *)AUX_PMON_CTRL) = 1; })
#define aux_pmon_disable() \
  ({ *((volatile int *)AUX_PMON_CTRL) = 0; })

#ifdef PMON_ONLY_CLK
#define aux_pmon_clear()			\
  ({						\
    *((volatile int *)AUX_PMON_CLK) = 0;	\
  })
#else //PMON_ONLY_CLK
#define aux_pmon_clear()			\
  ({						\
    *((volatile int *)AUX_PMON_CLK) = 0;	\
    *((volatile int *)AUX_PMON_BR) = 0;		\
    *((volatile int *)AUX_PMON_BLK) = 0;	\
    *((volatile int *)AUX_PMON_NRDY) = 0;	\
    *((volatile int *)AUX_PMON_EVTH) = 0;	\
  })
#endif //PMON_ONLY_CLK


#ifdef JZC_AUX_PMON

#ifdef PMON_ONLY_CLK

#define AUX_PMON_CREAT(func)			\
  unsigned int func##_aux_clk = 0;
#define AUX_PMON_TRAN(func,addr)					\
  ({									\
    volatile unsigned int * func##_pmon_addr = (unsigned int *)(addr);	\
    func##_pmon_addr[0] = func##_aux_clk;				\
  })
#define AUX_PMON_ON(func)	  \
  ({				  \
    aux_pmon_clear();		  \
    aux_pmon_enable();		  \
  })
#define AUX_PMON_OFF(func)				\
  ({							\
    func##_aux_clk += *((volatile int *)AUX_PMON_CLK);	\
  })

#else //PMON_ONLY_CLK

#define AUX_PMON_CREAT(func,addr)		\
  unsigned int func##_aux_clk = 0;		\
  unsigned int func##_aux_br = 0;		\
  unsigned int func##_aux_blk = 0;		\
  unsigned int func##_aux_nrdy = 0;		\
  volatile int * func##_pmon_addr = (int *)(addr);
#define AUX_PMON_TRAN(func)			\
  ({						\
    func##_pmon_addr[0] = func##_aux_clk;	\
    func##_pmon_addr[1] = func##_aux_br;	\
    func##_pmon_addr[2] = func##_aux_blk;	\
    func##_pmon_addr[3] = func##_aux_nrdy;	\
  })
#define AUX_PMON_ON(func)	  \
  ({				  \
    aux_pmon_clear();		  \
    aux_pmon_enable();		  \
  })
#define AUX_PMON_OFF(func)						\
  ({									\
    aux_pmon_disable();							\
    func##_aux_clk += *((volatile int *)AUX_PMON_CLK);			\
    func##_aux_br += *((volatile int *)AUX_PMON_BR);			\
    func##_aux_blk += *((volatile int *)AUX_PMON_BLK);			\
    func##_aux_nrdy += ((*((volatile int *)AUX_PMON_NRDY)) - 7);	\
  })

#endif //PMON_ONLY_CLK

#else // !JZC_AUX_PMON

#define AUX_PMON_CREAT(func)
#define AUX_PMON_TRAN(func,addr)
#define AUX_PMON_ON(func)
#define AUX_PMON_OFF(func)

#endif // JZC_AUX_PMON done


#endif // __H264_AUX_PMON_H__
