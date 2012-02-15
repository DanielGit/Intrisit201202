/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(XviD)
 *
* $Id: xvid_mc_hw.h,v 1.5 2009/07/07 09:22:15 yli Exp $
 *
 ****************************************************************************/

#ifndef __JZC_XVID_MC_HW_H__
#define __JZC_XVID_MC_HW_H__
#include "jzasm.h"
#include "tcsm.h"

#define H0V0 0
#define H1V0 1
#define H2V0 2
#define H3V0 3
#define H0V1 4
#define H1V1 5
#define H2V1 6
#define H3V1 7
#define H0V2 8
#define H1V2 9
#define H2V2 10
#define H3V2 11
#define H0V3 12
#define H1V3 13
#define H2V3 14
#define H3V3 15

#define MC_BLK_W2 0
#define MC_BLK_W4 1
#define MC_BLK_W8 2
#define MC_BLK_W16 3

#define MC_BLK_H2 0
#define MC_BLK_H4 1
#define MC_BLK_H8 2
#define MC_BLK_H16 3

#define	MPEG_HPEL	0
#define	MPEG_QPEL	1
#define	H264_QPEL	2
#define	H264_EPEL	3
#define	H264_WPDT	4
#define	WMV2_QPEL	5
#define	VC1_QPEL	6
#define	RV8_TPEL	7
#define	RV8_CHROM	8
#define	RV9_QPEL	9
#define	RV9_CHROM	10

// MC_REG_BASE
#if defined(JZC_RTL_SIM) || defined(_UCOS_)
# ifdef AUX_VADDR_SPACE
#   define MC_V_BASE 0x13090000
# else
#   define MC_V_BASE 0xb3090000
# endif
#else
//extern unsigned char *mc_base;
//#define MC_V_BASE mc_base
# ifdef AUX_VADDR_SPACE
#   define MC_V_BASE 0x13090000
# else
#   define MC_V_BASE 0xb3090000
# endif
#define MC_P_BASE 0x13090000
#endif //JZC_RTL_SIM

#define MC_CTRL 0x0
#define MC_STATUS 0x4
#define MC_REF_ADDR 0x8
#define MC_CURR_ADDR 0xc
#define MC_BLK_INFO 0x10
#define MC_INTP_INFO 0x14
#define MC_TAP_COEF_REG1 0x18
#define MC_TAP_COEF_REG3 0x1c
#define MC_CURR_STRD 0x20
#define MC_TAP_COEF_REG2 0x24
#define MC_TAP_COEF_REG4 0x28
#define MC_REF2_ADDR 0x2c
#define MC_REF2_STRD 0x30
#define MC_DCS 0x34
#define MC_DES_DHA 0x38
#define MC_DES_CSA 0x3c

// CTRL
#define MC_RUN_SFT 0x0
#define MC_RUN_MSK 0x1
#define MC_RST_SFT 0x1
#define MC_RST_MSK 0x1
#define FLUSH_EN_SFT 0x2
#define FLUSH_EN_MSK 0x1
#define MCC_DIS_SFT 0x3
#define MCC_DIS_MSK 0x1 

// STATUS
#define OUT_END_SFT 0x0
#define OUT_END_MSK 0x1
#define DSPT_END_SFT 0x0
#define DSPT_END_MSK 0x1

// REF_ADDR
// REF2_ADDR
// CURR_ADDR

// CURR_STRD

#define REF_STRD_SFT 0x0
#define REF_STRD_MSK 0xFFF
#define CURR_STRD_SFT 0x10
#define CURR_STRD_MSK 0xFFF

// REF2_STRD

#define REF2_STRD_SFT 0x0
#define REF2_STRD_MSK 0xFFF

// BLK_INFO

#define BLK_H_SFT 0x0
#define BLK_H_MSK 0x3
#define BLK_W_SFT 0x2
#define BLK_W_MSK 0x3
#define ITP_FMT_SFT 0x8
#define ITP_FMT_MSK 0xF
#define MC_AVG_SFT 0xC
#define MC_AVG_MSK 0x1

// 

#define ITP_RND0_SFT 0x0
#define ITP_RND0_MSK 0xFF
#define ITP_RND1_SFT 0x8
#define ITP_RND1_MSK 0xFF
#define ITP_CASE_SFT 0x10
#define ITP_CASE_MSK 0xF
#define ITP_IRND_SFT 0x14
#define ITP_IRND_MSK 0x1
#define ITP_FRND_SFT 0x15
#define ITP_FRND_MSK 0x1

// TAP_COEF

#define COEF1_SFT 0x0
#define COEF1_MSK 0xFF
#define COEF2_SFT 0x8
#define COEF2_MSK 0xFF
#define COEF3_SFT 0x10
#define COEF3_MSK 0xFF
#define COEF4_SFT 0x18
#define COEF4_MSK 0xFF

#define COEF5_SFT 0x0
#define COEF5_MSK 0xFF
#define COEF6_SFT 0x8
#define COEF6_MSK 0xFF
#define COEF7_SFT 0x10
#define COEF7_MSK 0xFF
#define COEF8_SFT 0x18
#define COEF8_MSK 0xFF

// DHA
#define DHA_SFT 0x2
#define DHA_MSK 0x3FFFFFFF

//DCS
#define DMAE_SFT 0x0
#define DMAE_MSK 0x1
#define TT_SFT 0x2
#define TT_MSK 0x1
#define LINK_SFT 0x4
#define LINK_MSK 0x1

#define SET_MC_REF_ADDR(ref_addr) \
({ write_reg( (MC_V_BASE + MC_REF_ADDR), ref_addr); \
})
#define SET_MC_CURR_ADDR(curr_addr) \
({ write_reg( (MC_V_BASE + MC_CURR_ADDR), curr_addr); \
})
#define SET_BLK_INFO(data) \
({ write_reg( (MC_V_BASE + MC_BLK_INFO),  data);\
})
#define SET_INTP_INFO(data) \
({ write_reg( (MC_V_BASE + MC_INTP_INFO),  data);\
})
#define SET_MC_STRD(ref_strd,curr_strd) \
({ write_reg( (MC_V_BASE + MC_CURR_STRD), ((ref_strd) & REF_STRD_MSK)<<REF_STRD_SFT \
| ((curr_strd) & CURR_STRD_MSK)<<CURR_STRD_SFT); \
})
#define SET_COEF_REG1(coef1,coef2,coef3,coef4) \
  ({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG1), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define SET_COEF_REG2(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG2), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define SET_COEF_REG3(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG3), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define SET_COEF_REG4(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG4), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define SW_RST_MC() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) \
| MC_RST_MSK<<MC_RST_SFT); \
 write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) \
& MC_RST_MSK<<~MC_RST_SFT); \
})


#define VIA_MCBRG  0x15   /*MC access AHB0 via special bridge*/
#define VIA_GERBRG 0x5    /*MC access AHB0 via genernal bridge*/

#define RUN_MC() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) | VIA_MCBRG); \
})
#define RUN_MC_GER() \
({ write_reg( (MC_V_BASE + MC_CTRL), ( (read_reg(MC_V_BASE, MC_CTRL)) & 0xFFFFFFE0 ) | VIA_GERBRG); \
})
#define STOP_MC() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) & ~0x1); \
})
#define READ_MC_END_FLAG() \
({ read_reg(MC_V_BASE, MC_STATUS); \
})
#define CLEAR_MC_END_FLAG() \
({ write_reg( (MC_V_BASE + MC_STATUS), 0); \
})
#define CLEAR_MC_TTEND() \
({ write_reg( (MC_V_BASE + MC_STATUS), 1); \
})
#define SET_MC_END_FLAG() \
({ write_reg( (MC_V_BASE + MC_STATUS), 3); \
})
#define SET_MC_DHA(dha_val) \
({ write_reg( (MC_V_BASE + MC_DES_DHA), dha_val); \
})

#define SET_MC_DCS() \
({ write_reg( (MC_V_BASE + MC_DCS), 1); \
})

#define MC_POLLING_END()					\
{                                                               \
    do {							\
    } while (READ_MC_END_FLAG() != 0x3);			\
}   

#define MC_POLLING_BLK_END()					\
{                                                               \
    do {							\
    } while (!READ_MC_END_FLAG());      			\
}                         

#define TAPALN(coef1,coef2,coef3,coef4) \
( ((coef1) & COEF1_MSK)<<COEF1_SFT      \
| ((coef2) & COEF2_MSK)<<COEF2_SFT      \
| ((coef3) & COEF3_MSK)<<COEF3_SFT      \
| ((coef4) & COEF4_MSK)<<COEF4_SFT)

#define MC_CACHE_FLUSH RUN_MC

typedef struct XVID_MC_DesNode{
  unsigned int NodeFlag;
  unsigned int NodeHead;
  unsigned int Stride;
  unsigned int Tap2Coef; /*second stage filter's tap coeffient*/
  unsigned int Tap1Coef; /*first stage filter's tap coeffient*/
  unsigned int IntpInfo; /*interpolate information*/
  unsigned int BLKInfo;  /*block information*/
  unsigned int CurrAddr; /*current block address*/
  unsigned int RefAddr;  /*reference address*/
  unsigned int MCStatus; /*MC status*/
}XVID_MC_DesNode;

#define DESP_NODE_HEAD_FG  0x80000000
#define DESP_NODE_HEAD_LK  0x2008004d
#define DESP_NODE_HEAD_UK  0x2008004c

//JZC_AUX_MC_SYNC
#define DESP_NODE_HEAD_SYNC 0x20080048
#define DESP_NODE_HEAD_END  0x0001006c

#define MPEG_HPEL_16X16_BLK_INF 0x0000000f
#define MPEG_HPEL_8X8_BLK_INF   0x0000000a
#define MPEG_HPEL_8X4_BLK_INF   0x00000009

#define MPEG_QPEL_16X16_BLK_INF 0x0000010f
#define MPEG_QPEL_8X8_BLK_INF   0x0000010a

const static unsigned short Blk4VOffset[] = {
  0,                  8,
  8*RECON_BUF_STRIDE, 8*RECON_BUF_STRIDE+8,
};

const static unsigned int HPEL_IRND_IntpCase_RND1_RND0[2][4] = {
  {
    (H0V0  << 16) | (0<<8) |(0),
    (H0V2  << 16) | (0<<8) |(1),
    (H2V0  << 16) | (0<<8) |(1),
    (H2V2  << 16) | (2<<8) |(0),
  }, // rounding = 0;
  {
    (H0V0  << 16) | (0<<8) |(0),
    (H0V2  << 16) | (0<<8) |(0),
    (H2V0  << 16) | (0<<8) |(0),
    (H2V2  << 16) | (1<<8) |(0),
  } // rounding = 1;
};

/*
Passes to be performed

  case 0:         copy
  case 2:         h-pass
  case 1/3:       h-pass + h-avrg
  case 8:                           v-pass
  case 10:        h-pass          + v-pass
  case 9/11:      h-pass + h-avrg + v-pass
  case 4/12:                        v-pass + v-avrg
  case 6/14:      h-pass          + v-pass + v-avrg
  case 5/13/7/15: h-pass + h-avrg + v-pass + v-avrg
 
  0  1  2  3         h0v0 h1v0 h2v0 h3v0
  4  5  6  7   ==>   h0v1
  8  9  10 11        h0v2
  12 13 14 15        h0v3
*/
// ITPINOF : IRND ,ITPCASE, RND1, RND0 -- ITPCASE, RND1, RND0 , IRND

const static uint32_t QPEL_IRND_IntpCase_RND1_RND0[2][16]=
{
  // IRND = 0 = 1 - 1
  {
    (H0V0 << 16) | (16 << 8) | (16) | (0 << 20), // 0 
    (H1V0 << 16) | (16 << 8) | (16) | (0 << 20),
    (H2V0 << 16) | (16 << 8) | (16) | (0 << 20),
    (H3V0 << 16) | (16 << 8) | (16) | (0 << 20),
    
    (H0V1 << 16) | (16 << 8) | (16) | (0 << 20),
    (H1V1 << 16) | (16 << 8) | (16) | (0 << 20), // 5
    (H2V1 << 16) | (16 << 8) | (16) | (0 << 20),
    (H3V1 << 16) | (16 << 8) | (16) | (0 << 20),
    
    (H0V2 << 16) | (16 << 8) | (16) | (0 << 20),
    (H1V2 << 16) | (16 << 8) | (16) | (0 << 20),
    (H2V2 << 16) | (16 << 8) | (16) | (0 << 20), //10
    (H3V2 << 16) | (16 << 8) | (16) | (0 << 20),
    
    (H0V3 << 16) | (16 << 8) | (16) | (0 << 20),
    (H1V3 << 16) | (16 << 8) | (16) | (0 << 20),
    (H2V3 << 16) | (16 << 8) | (16) | (0 << 20),
    (H3V3 << 16) | (16 << 8) | (16) | (0 << 20), // 16
  },
  // IRND = 1 = 1 - 0 = 1 - rounding ;
  {
    (H0V0 << 16) | (15 << 8) | (15) | (0 << 20), // 0 
    (H1V0 << 16) | (15 << 8) | (15) | (1 << 20),
    (H2V0 << 16) | (15 << 8) | (15) | (0 << 20),
    (H3V0 << 16) | (15 << 8) | (15) | (1 << 20),
    
    (H0V1 << 16) | (15 << 8) | (15) | (1 << 20),
    (H1V1 << 16) | (15 << 8) | (15) | (1 << 20), // 5
    (H2V1 << 16) | (15 << 8) | (15) | (1 << 20),
    (H3V1 << 16) | (15 << 8) | (15) | (1 << 20),
    
    (H0V2 << 16) | (15 << 8) | (15) | (0 << 20),
    (H1V2 << 16) | (15 << 8) | (15) | (1 << 20),
    (H2V2 << 16) | (15 << 8) | (15) | (0 << 20), //10
    (H3V2 << 16) | (15 << 8) | (15) | (1 << 20),
    
    (H0V3 << 16) | (15 << 8) | (15) | (1 << 20),
    (H1V3 << 16) | (15 << 8) | (15) | (1 << 20),
    (H2V3 << 16) | (15 << 8) | (15) | (1 << 20),
    (H3V3 << 16) | (15 << 8) | (15) | (1 << 20), // 16
  },
};

#endif /*__JZC_XVID_MC_HW_H__*/
