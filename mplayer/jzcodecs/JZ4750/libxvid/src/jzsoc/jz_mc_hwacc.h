/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(Real Video)
 *
* $Id: jz_mc_hwacc.h,v 1.2 2009/10/30 00:25:35 dyang Exp $
 *
 ****************************************************************************/

#ifndef __JZC_RV_MC_HWACC_H__
#define __JZC_RV_MC_HWACC_H__
#include "tcsm.h"
#include "jzasm.h"

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
#ifdef _UCOS_
#define MC_V_BASE 0xB3090000
#else
extern  unsigned char *mc_base;
#define MC_V_BASE mc_base
#define MC_P_BASE 0x13090000
#endif

// CTRL
#define MC_CTRL 0x0

#define MC_RUN_SFT 0x0
#define MC_RUN_MSK 0x1
#define MC_RST_SFT 0x1
#define MC_RST_MSK 0x1
#define FLUSH_EN_SFT 0x2
#define FLUSH_EN_MSK 0x1
#define MCC_DIS_SFT 0x3
#define MCC_DIS_MSK 0x1 

// STATUS
#define MC_STATUS 0x4

#define OUT_END_SFT 0x0
#define OUT_END_MSK 0x1
#define DSPT_END_SFT 0x0
#define DSPT_END_MSK 0x1

// REF_ADDR
#define MC_REF_ADDR 0x8
// REF2_ADDR
#define MC_REF2_ADDR 0xc
// CURR_ADDR
#define MC_CURR_ADDR 0x10

// CURR_STRD
#define MC_CURR_STRD 0x14

#define REF_STRD_SFT 0x0
#define REF_STRD_MSK 0xFFF
#define CURR_STRD_SFT 0x10
#define CURR_STRD_MSK 0xFFF

// REF2_STRD
#define MC_REF2_STRD 0x18

#define REF2_STRD_SFT 0x0
#define REF2_STRD_MSK 0xFFF

// BLK_INFO
#define MC_BLK_INFO 0x1c

#define BLK_H_SFT 0x0
#define BLK_H_MSK 0x3
#define BLK_W_SFT 0x2
#define BLK_W_MSK 0x3
#define ITP_FMT_SFT 0x8
#define ITP_FMT_MSK 0xF
#define MC_AVG_SFT 0xC
#define MC_AVG_MSK 0x1

// 
#define MC_INTP_INFO 0x20

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
#define ITP_IRND_EN  1<<ITP_IRND_SFT
#define ITP_FRND_EN  1<<ITP_FRND_SFT

// TAP_COEF
#define MC_TAP_COEF_REG1 0x24
#define MC_TAP_COEF_REG2 0x28
#define MC_TAP_COEF_REG3 0x2C
#define MC_TAP_COEF_REG4 0x30

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
#define MC_DES_DHA 0x34
#define DHA_SFT 0x2
#define DHA_MSK 0x3FFFFFFF

//DCS
#define MC_DCS 0x38
#define DMAE_SFT 0x0
#define DMAE_MSK 0x1
#define TT_SFT 0x2
#define TT_MSK 0x1
#define LINK_SFT 0x4
#define LINK_MSK 0x1

#define write_MCreg(reg, val) \
({ i_sw((val), (reg), 0);     \
})
#define read_MCreg(reg, off)  \
({ i_lw((reg), (off));        \
})

#define set_blk_info(blk_w, blk_h, itp_fmt, avg) \
({ write_MCreg( (MC_V_BASE + MC_BLK_INFO),  ((blk_w) & BLK_W_MSK)<<BLK_W_SFT \
| ((blk_h) & BLK_H_MSK)<<BLK_H_SFT \
| ((itp_fmt) & ITP_FMT_MSK)<<ITP_FMT_SFT  | ((avg) & MC_AVG_MSK)<<MC_AVG_SFT);\
})
#define set_curr_strd(ref_strd,curr_strd) \
({ write_MCreg( (MC_V_BASE + MC_CURR_STRD), ((ref_strd) & REF_STRD_MSK)<<REF_STRD_SFT \
| ((curr_strd) & CURR_STRD_MSK)<<CURR_STRD_SFT); \
})
#define set_ref2_strd(ref2_strd) \
({ write_MCreg( (MC_V_BASE + MC_REF2_STRD), ((ref2_strd) & REF2_STRD_MSK)<<REF2_STRD_SFT); \
})
#define set_coef_reg1(coef1,coef2,coef3,coef4) \
  ({ write_MCreg( (MC_V_BASE + MC_TAP_COEF_REG1), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg2(coef1,coef2,coef3,coef4) \
({ write_MCreg( (MC_V_BASE + MC_TAP_COEF_REG2), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg3(coef1,coef2,coef3,coef4) \
({ write_MCreg( (MC_V_BASE + MC_TAP_COEF_REG3), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg4(coef1,coef2,coef3,coef4) \
({ write_MCreg( (MC_V_BASE + MC_TAP_COEF_REG4), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define sw_reset_mc() \
({ write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) \
| MC_RST_MSK<<MC_RST_SFT); \
 write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) \
& MC_RST_MSK<<~MC_RST_SFT); \
})
#define run_mc() \
do { \
	write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) | 0x5); \
} while (0)

#define stop_mc() \
({ write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) & ~0x1); \
})
#define polling_mc_end_flag() \
({ read_MCreg(MC_V_BASE, MC_STATUS); \
})

#define mc_polling_end()					\
{                                                               \
  do {								\
    } while (polling_mc_end_flag() != 0x3);			\
}   

#define set_mc_dis() \
do { \
	write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) | 0x8); \
} while (0)

#define clear_mc_dis() \
do { \
	write_MCreg( (MC_V_BASE + MC_CTRL), (read_MCreg(MC_V_BASE, MC_CTRL)) &~ 0x8); \
} while (0)


#define clear_mc_end_flag() \
({ write_MCreg( (MC_V_BASE + MC_STATUS), 0); \
})
#define set_mc_flag() \
({ write_MCreg( (MC_V_BASE + MC_STATUS), 3); \
})
#define set_mc_dha(dha_val) \
({ write_MCreg( (MC_V_BASE + MC_DES_DHA), dha_val); \
})
#define set_mc_dcs() \
do { \
	write_MCreg( (MC_V_BASE + MC_STATUS), 1);  \
	write_MCreg((MC_V_BASE + MC_DCS), 1);      \
} while(0)


#define I32 int
#define U32 unsigned int 

#define MCCacheFlush run_mc
#define set_MCstride set_curr_strd



#define NODE_LINK 0x1
#define MC_USE_AVG 0x1000

typedef struct RV_MC_DesNode{
  U32 NodeHead;
  U32 RefAddr[2]; /*reference address. [0]: index; [1]: data*/
  U32 CurrAddr[2]; /*current block address*/
  U32 FrmSride[2]; /*frame stride*/
  U32 BLKInfo[2]; /*block information*/
  U32 IntpInfo[2]; /*interpolate information*/
  //  U32 Tap1Coef[2]; /*first stage filter's tap coeffient*/
  // U32 Tap2Coef[2]; /*second stage filter's tap coeffient*/
#ifdef JZC_MC_NOAVG
#else
  U32 Ref2Addr[2]; /*2nd reference address in Bi-dir prediction*/
  U32 BufStrd[2]; /*buffer strd*/
#endif
  U32 MCStatus[2]; /*MC status*/
}RV_MC_DesNode;


//extern RV_MC_DesNode *MC;

# define BIDIR_AVG 0x1000
# define MC_BLK_W16XH16 0
# define MC_BLK_W8XH8 1



/*XVID BLK information LUT*/

const static U32 XVIDLumaBLK_tab[2][2] =  // XVIDLumaBLK_tab[ITP_FMT][BLK_WH]
 {
   //MPEG_HPEL
   {
     ((MPEG_HPEL<<8)|(MC_BLK_W16<<2) | MC_BLK_H16),
     ((MPEG_HPEL<<8)|(MC_BLK_W8<<2) | MC_BLK_H8), ///?????
   },
   //MPEG_QPEL
   {
     ((MPEG_QPEL<<8)|(MC_BLK_W16<<2) | MC_BLK_H16),
     ((MPEG_QPEL<<8)|(MC_BLK_W8<<2) | MC_BLK_H8),
   }
  
 };


const static U32 HPEL_IRND_IntpCase_RND1_RND0[2][4] = {

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

const static U32 QPEL_IRND_IntpCase_RND1_RND0[2][16]=
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




/*RV Tabfilter coeff LUT*/
#define TAPALN(coef1,coef2,coef3,coef4) \
( ((coef1) & COEF1_MSK)<<COEF1_SFT      \
| ((coef2) & COEF2_MSK)<<COEF2_SFT      \
| ((coef3) & COEF3_MSK)<<COEF3_SFT      \
| ((coef4) & COEF4_MSK)<<COEF4_SFT)


#endif /*__JZC_RV_MC_HWACC_H__*/
