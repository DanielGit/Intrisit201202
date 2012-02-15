#ifndef __JZ4750E_IDCT_H__
#define __JZ4750E_IDCT_H__
#include "jzasm.h"

// IDCT_REG_BASE
#if defined(JZC_RTL_SIM) || defined(_UCOS_)
# ifdef AUX_VADDR_SPACE
#   define IDCT_V_BASE 0x130c0000
# else
#   define IDCT_V_BASE 0xb30c0000
# endif
#define IDCT_P_BASE 0xb30C0000
#else
//extern unsigned char *idct_base;
//#define IDCT_V_BASE idct_base
# ifdef AUX_VADDR_SPACE
#   define IDCT_V_BASE 0x130c0000
# else
#   define IDCT_V_BASE 0xb30c0000
# endif
#define IDCT_P_BASE 0xb30C0000
#endif //JZC_RTL_SIM

//dsp node
#define IDCT_NODE_LEN 4
#define IDCT_NODE_CMD 1
#define IDCT_UNLINK_HEAD 0x1402004c
#define IDCT_LINK_HEAD  0x1402004d
#define IDCT_END_HEAD 0x10024//fix lhuang

// chain dsph
#define DSPH_DTC_SFT 0x4
#define DSPH_DTC_MSK 0x7F
#define DSPH_CY_SFT 0x1
#define DSPH_CY_MSK 0x1
#define DSPH_LINK_SFT 0x0
#define DSPH_LINK_MSK 0x1

// chain addr
#define DSPA_OFST_SFT 0x18
#define DSPA_OFST_MSK 0x7F
#define DSPH_MASK_SFT 0x0
#define DSPH_MASK_MSK 0xFFFFFF

//DCS
/*
#define DDMA_DCS_DMAE_SFT 0x0
#define DDMA_DCS_DMAE_MSK 0x1
#define DDMA_DCS_IE_SFT 0x1
#define DDMA_DCS_IE_MSK 0x1
#define DDMA_DCS_TT_SFT 0x2
#define DDMA_DCS_TT_MSK 0x1
#define DDMA_DCS_IRQ_SFT 0x3
#define DDMA_DCS_IRQ_MSK 0x1
#define DDMA_DCS_LINK_SFT 0x4
#define DDMA_DCS_LINK_MSK 0x1
*/

// GCSR
#define REG_GCSR 0x0

#define IDCT_EN_SFT 0x0
#define IDCT_EN_MSK 0x1
#define IDCT_START_SFT 0x1
#define IDCT_START_MSK 0x1
#define IDCT_DESP_SFT 0x2
#define IDCT_DESP_MSK 0x1
#define IDCT_IRQ_EN_SFT 0x3
#define IDCT_IRQ_EN_MSK 0x1
#define IDCT_FRESH_SFT 0x4
#define IDCT_FRESH_MSK 0x1
#define IDCT_OVER_PROT_SFT 0x5
#define IDCT_OVER_PROT_MSK 0x1
//#define IDCT_END_SFT 0x10
//#define IDCT_END_MSK 0x1
#define IDCT_LINK_SFT 0x10
#define IDCT_LINK_MSK 0x1


#define IDCT_ERRO_SFT 0x11
#define IDCT_ERRO_MSK 0x3

//END
#define REG_END 0x4

// DHA
#define REG_DDMA_DHA 0x8
#define DDMA_DHA_SFT 0x0
#define DDMA_DHA_MSK 0xFFFFFFFF


// VTR 
#define REG_VTR 0xC

#define IDCT_TYPE_SFT 0x0
#define IDCT_TYPE_MSK 0x3
#define IDCT_VIDEO_SFT 0x10
#define IDCT_VIDEO_MSK 0x3


#define HAMA 0x0
#define H264 0x1
#define REAL 0x2
#define WMV 0x3

// INPUT_ADDR
#define REG_INPUT_ADDR 0x10

// OUT_ADDR
#define REG_OUT_ADDR 0x14

// STRIDE
#define REG_STR 0x18
#define IN_STR_SFT 0x0
#define IN_STR_MSK 0xFF
#define OUT_STR_SFT 0x10
#define OUT_STR_MSK 0xFF

// DCS
#define REG_DCS 0x1C

#define set_type_video(video, type) \
({ write_reg( (IDCT_V_BASE + REG_VTR), ((type & IDCT_TYPE_MSK)<<IDCT_TYPE_SFT)| \
((video & IDCT_VIDEO_MSK)<<IDCT_VIDEO_SFT)); \
})

#define set_ddma_dha(dha) \
({ write_reg( (IDCT_V_BASE + REG_DDMA_DHA), ((dha & DDMA_DHA_MSK)<<DDMA_DHA_SFT)); \
})

#define set_ddma_chain_header(addr, link, cy, dtc) \
({ addr = ((link & DSPH_LINK_MSK)<<DSPH_LINK_SFT)| \
((cy & DSPH_CY_MSK)<<DSPH_CY_SFT)| \
((dtc & DSPH_DTC_MSK)<<DSPH_DTC_SFT); \
})

#define set_ddma_chain_nod_addr(addr, ofst, mask) \
({ addr = ofst ;\
})

#define set_ddma_chain_nod_data_ctrl(addr, type, video) \
({ addr =  ((type & IDCT_TYPE_MSK)<<IDCT_TYPE_SFT)| \
((video & IDCT_VIDEO_MSK)<<IDCT_VIDEO_SFT); \
})

#define set_input_addr(input_addr) \
({ write_reg( (IDCT_V_BASE + REG_INPUT_ADDR), input_addr); \
})

#define set_out_addr(out_addr) \
({ write_reg( (IDCT_V_BASE + REG_OUT_ADDR), out_addr); \
})

#define set_stride(input_stride, out_stride)			    \
  ({ write_reg( (IDCT_V_BASE + REG_STR), ((input_stride & IN_STR_MSK))| \
		((out_stride & OUT_STR_MSK)<<OUT_STR_SFT));\
})

#define fresh_idct(flag) \
({ write_reg( (IDCT_V_BASE + REG_GCSR), ((read_reg(IDCT_V_BASE, REG_GCSR)) \
& (~(IDCT_FRESH_MSK<<IDCT_FRESH_SFT))| \
((flag & IDCT_FRESH_MSK) <<IDCT_FRESH_SFT))); \
})

#define irq_enable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), ((read_reg(IDCT_V_BASE, REG_GCSR)) \
&(~(IDCT_IRQ_EN_MSK<<IDCT_FRESH_SFT))) \
| (IDCT_IRQ_EN_MSK<<IDCT_IRQ_EN_SFT)); \
})

#define desp_enable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) \
| (IDCT_DESP_MSK<<IDCT_DESP_SFT)); \
})

#define desp_disable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) \
& (~(IDCT_DESP_MSK<<IDCT_DESP_SFT)));			\
})


#define irq_disable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) \
& (~(IDCT_IRQ_EN_MSK<<IDCT_IRQ_EN_SFT))); \
})

#define run_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) | 0x2); \
})

#define enable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) | 0x1); \
})

#define disable_idct() \
({ write_reg( (IDCT_V_BASE + REG_GCSR), (read_reg(IDCT_V_BASE, REG_GCSR)) | ~0x1); \
})

#define polling_end_flag() \
({ (read_reg(IDCT_V_BASE, REG_END)); \
})

#define clean_end_flag() \
({ write_reg( (IDCT_V_BASE + REG_END), 0); \
})

#define set_over_flow_protect(over_flow_protect) \
({ write_reg( (IDCT_V_BASE + REG_GCSR), ((read_reg(IDCT_V_BASE, REG_GCSR)) \
&(~(IDCT_OVER_PROT_MSK<<IDCT_OVER_PROT_SFT)) \
|((over_flow_protect & IDCT_OVER_PROT_MSK)<<IDCT_OVER_PROT_SFT))); \
})

#define set_idct_dcs() \
({ write_reg( (IDCT_V_BASE + REG_DCS), 0x1); \
})

#ifdef JZC_DCORE_ARCH
# define EXT_ADDR(addr)      get_phy_addr(addr)
#endif

#define hw_idct_descrip_nod_conf(dct_src, dct_ptr)	\
({                                  \
idct_chain_tab[0]= 0x80000000;  \
idct_chain_tab[1] = IDCT_LINK_HEAD ; \
idct_chain_tab[3] = dct_src; \
idct_chain_tab[2] = dct_ptr ; \
 })

#define hw_run_idct_c() \ 
({clean_end_flag();	\
  set_idct_dcs();	\
})

#define hw_wait_idct_c() \
({   do { \
} while (!polling_end_flag() ) ; \
})

#define idct_polling_end()					\
{                                                               \
    do {							\
    } while (!polling_end_flag() );	          		\
}                         

#endif //__JZ4750E_IDCT_H__

