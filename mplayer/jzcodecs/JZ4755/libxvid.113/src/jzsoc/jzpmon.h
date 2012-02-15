#ifndef __JZ4750E_PMON_H__
#define __JZ4750E_PMON_H__
/* ===================================================================== */
/*  MAIN CPU PMON Macros                                                 */
/* ===================================================================== */
#include "jzasm.h"

#ifdef JZC_MAIN_PMON

#define J1_PMON_GETRC() i_mfc0_2(16, 6)
#define J1_PMON_GETLC() i_mfc0_2(16, 5)
#define J1_PMON_GETRH() (i_mfc0_2(16, 4) & 0xFFFF)
#define J1_PMON_GETLH() ((i_mfc0_2(16, 4)>>16) & 0xFFFF)

#ifdef STA_CCLK
#define J1_PMON_ON(func)\
  ({  i_mtc0_2(0, 16, 4);\
    i_mtc0_2(0, 16, 5);\
    i_mtc0_2(0, 16, 6);\
    i_mtc0_2( (i_mfc0_2(16, 7) & 0xFFFF0FFF) | 0x0100, 16, 7);	\
  })
#define J1_PMON_OFF(func)  i_mtc0_2(i_mfc0_2(16, 7) & ~0x100, 16, 7); func##_pmon_val += J1_PMON_GETRC()

#elif defined(STA_DCC)
#define J1_PMON_ON(func)\
  ({  i_mtc0_2(0, 16, 4);\
    i_mtc0_2(0, 16, 5);\
    i_mtc0_2(0, 16, 6);\
    i_mtc0_2( (i_mfc0_2(16, 7) & 0xFFFF0FFF) | 0x1100, 16, 7);	\
  })
#define J1_PMON_OFF(func)  i_mtc0_2(i_mfc0_2(16, 7) & ~0x100, 16, 7); func##_pmon_val += J1_PMON_GETRC()

#elif defined(STA_ICC)
#define J1_PMON_ON(func)\
  ({  i_mtc0_2(0, 16, 4);\
    i_mtc0_2(0, 16, 5);\
    i_mtc0_2(0, 16, 6);\
    i_mtc0_2( (i_mfc0_2(16, 7) & 0xFFFF0FFF) | 0x1100, 16, 7);	\
  })
#define J1_PMON_OFF(func)  i_mtc0_2(i_mfc0_2(16, 7) & ~0x100, 16, 7); func##_pmon_val += J1_PMON_GETLC()

#elif defined(STA_INSN)
#define J1_PMON_ON(func)\
  ({  i_mtc0_2(0, 16, 4);\
    i_mtc0_2(0, 16, 5);\
    i_mtc0_2(0, 16, 6);\
    i_mtc0_2( (i_mfc0_2(16, 7) & 0xFFFF0FFF) | 0x2100, 16, 7);	\
  })
#define J1_PMON_OFF(func)  i_mtc0_2(i_mfc0_2(16, 7) & ~0x100, 16, 7); func##_pmon_val += J1_PMON_GETRC()

#elif defined(STA_UINSN) /*useless instruction*/
#define J1_PMON_ON(func)\
  ({  i_mtc0_2(0, 16, 4);\
    i_mtc0_2(0, 16, 5);\
    i_mtc0_2(0, 16, 6);\
    i_mtc0_2( (i_mfc0_2(16, 7) & 0xFFFF0FFF) | 0x2100, 16, 7);	\
  })
#define J1_PMON_OFF(func)  i_mtc0_2(i_mfc0_2(16, 7) & ~0x100, 16, 7); func##_pmon_val += J1_PMON_GETLC()
#endif

#define J1_PMON_CREAT(func)			\
  unsigned long long func##_pmon_val = 0;	

#define J1_PMON_CLEAR(func)                     \
  func##_pmon_val = 0;

#endif // JZC_MAIN_PMON done


/* ===================================================================== */
/*  AUX CPU PMON Macros                                                 */
/* ===================================================================== */

#ifdef JZC_AUX_PMON

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
#define aux_pmon_clear()			\
  ({						\
    *((volatile int *)AUX_PMON_CLK) = 0;	\
    *((volatile int *)AUX_PMON_BR) = 0;		\
    *((volatile int *)AUX_PMON_BLK) = 0;	\
    *((volatile int *)AUX_PMON_NRDY) = 0;	\
    *((volatile int *)AUX_PMON_EVTH) = 0;	\
  })

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

#ifdef PRECISE40
#define AUX_PMON_OFF(func)						\
  ({									\
    aux_pmon_disable();							\
    unsigned long long tmp_clk, tmp_br, tmp_blk, tmp_nrdy, tmp_evth;	\
    tmp_clk = *((volatile int *)AUX_PMON_CLK);				\
    tmp_br = *((volatile int *)AUX_PMON_BR);				\
    tmp_blk = *((volatile int *)AUX_PMON_BLK);				\
    tmp_nrdy = *((volatile int *)AUX_PMON_NRDY);			\
    tmp_evth = *((volatile int *)AUX_PMON_EVTH);			\
    func##_aux_clk += ((tmp_evth & 0xFF000000) << 8) | tmp_clk;		\
    func##_aux_br += ((tmp_evth & 0x0000FF00) << 24) | tmp_br;		\
    func##_aux_blk += ((tmp_evth & 0x000000FF) << 32) | tmp_blk;	\
    func##_aux_nrdy += ((tmp_evth & 0x00FF0000) << 16) | tmp_nrdy;	\
  })
#else // !PRECISE40
#define AUX_PMON_OFF(func)						\
  ({									\
    aux_pmon_disable();							\
    func##_aux_clk += *((volatile int *)AUX_PMON_CLK);			\
    func##_aux_br += *((volatile int *)AUX_PMON_BR);			\
    func##_aux_blk += *((volatile int *)AUX_PMON_BLK);			\
    func##_aux_nrdy += ((*((volatile int *)AUX_PMON_NRDY)) - 7);	\
  })
#endif // PRECISE40 doen

#else // !JZC_AUX_PMON

#define AUX_PMON_CREAT(func,addr)
#define AUX_PMON_TRAN(func)
#define AUX_PMON_ON(func)
#define AUX_PMON_OFF(func)

#endif // JZC_AUX_PMON done

#endif /*__JZ4750E_PMON_H__*/
