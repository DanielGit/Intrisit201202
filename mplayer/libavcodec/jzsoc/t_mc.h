#ifndef __MC_SEL__
#define __MC_SEL__

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
#define MC_V_BASE 0xB3090000
//#define MC_V_BASE mc_base
#define MC_P_BASE 0x13090000

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

//frame_typ
#define FRAME_TYP 0x3c

#define write_reg(reg, val) \
({ i_sw((val), (reg), 0); \
})
#define read_reg(reg, off) \
({ i_lw((reg), (off)); \
})

#define set_blk_info(blk_w, blk_h, itp_fmt, avg) \
({ write_reg( (MC_V_BASE + MC_BLK_INFO),  ((blk_w) & BLK_W_MSK)<<BLK_W_SFT \
| ((blk_h) & BLK_H_MSK)<<BLK_H_SFT \
| ((itp_fmt) & ITP_FMT_MSK)<<ITP_FMT_SFT  | ((avg) & MC_AVG_MSK)<<MC_AVG_SFT);\
})

#define set_blk_info_data(data) \
({ write_reg( (MC_V_BASE + MC_BLK_INFO),  data);\
})

#define set_intp_info(round0, round1, itp_case, irnd, frnd) \
({ write_reg( (MC_V_BASE + MC_INTP_INFO), ((round0) & ITP_RND0_MSK)<<ITP_RND0_SFT \
| ((round1) & ITP_RND1_MSK)<<ITP_RND1_SFT | ((itp_case) & ITP_CASE_MSK)<<ITP_CASE_SFT \
| ((irnd) & ITP_IRND_MSK)<<ITP_IRND_SFT | ((frnd) & ITP_FRND_MSK)<<ITP_FRND_SFT);\
})

#define set_intp_info_data(data) \
({ write_reg( (MC_V_BASE + MC_INTP_INFO),  data);\
})

#define set_mc_ref_addr(ref_addr) \
({ write_reg( (MC_V_BASE + MC_REF_ADDR), ref_addr); \
})
#define set_mc_ref2_addr(ref2_addr) \
({ write_reg( (MC_V_BASE + MC_REF2_ADDR), ref2_addr); \
})
#define set_mc_curr_addr(curr_addr) \
({ write_reg( (MC_V_BASE + MC_CURR_ADDR), curr_addr); \
})
#define set_mc_curr_strd(ref_strd,curr_strd) \
({ write_reg( (MC_V_BASE + MC_CURR_STRD), ((ref_strd) & REF_STRD_MSK)<<REF_STRD_SFT \
| ((curr_strd) & CURR_STRD_MSK)<<CURR_STRD_SFT); \
})

#define set_mc_ref2_strd(ref2_strd) \
({ write_reg( (MC_V_BASE + MC_REF2_STRD), ((ref2_strd) & REF2_STRD_MSK)<<REF2_STRD_SFT); \
})

#define set_tap_coef(coef1,coef2,coef3,coef4,coef5,coef6,coef7,coef8) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG1), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
   write_reg( (MC_V_BASE + MC_TAP_COEF_REG2), ((coef5) & COEF5_MSK)<<COEF5_SFT \
| ((coef6) & COEF6_MSK)<<COEF6_SFT | ((coef7) & COEF7_MSK)<<COEF7_SFT \
| ((coef8) & COEF8_MSK)<<COEF8_SFT); \
})
#define set_tap_coef2(coef1,coef2,coef3,coef4,coef5,coef6,coef7,coef8) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG3), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
   write_reg( (MC_V_BASE + MC_TAP_COEF_REG4), ((coef5) & COEF5_MSK)<<COEF5_SFT \
| ((coef6) & COEF6_MSK)<<COEF6_SFT | ((coef7) & COEF7_MSK)<<COEF7_SFT \
| ((coef8) & COEF8_MSK)<<COEF8_SFT); \
})

#define get_mc_tap_coef(coef1,coef2,coef3,coef4) \
( ((coef1) & COEF1_MSK)<<COEF1_SFT | ((coef2) & COEF2_MSK)<<COEF2_SFT \
| ((coef3) & COEF3_MSK)<<COEF3_SFT | ((coef4) & COEF4_MSK)<<COEF4_SFT ) 

#define set_coef_reg1(coef1,coef2,coef3,coef4) \
  ({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG1), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg2(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG2), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg3(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG3), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define set_coef_reg4(coef1,coef2,coef3,coef4) \
({ write_reg( (MC_V_BASE + MC_TAP_COEF_REG4), ((coef1) & COEF1_MSK)<<COEF1_SFT \
| ((coef2) & COEF2_MSK)<<COEF2_SFT | ((coef3) & COEF3_MSK)<<COEF3_SFT \
| ((coef4) & COEF4_MSK)<<COEF4_SFT); \
})
#define sw_reset_mc() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) \
| MC_RST_MSK<<MC_RST_SFT); \
 write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) \
& MC_RST_MSK<<~MC_RST_SFT); \
})
#define run_mc() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) | 0x5); \
})
#define stop_mc() \
({ write_reg( (MC_V_BASE + MC_CTRL), (read_reg(MC_V_BASE, MC_CTRL)) & ~0x1); \
})
#define polling_mc_end_flag() \
({ read_reg(MC_V_BASE, MC_STATUS); \
})
#define clear_mc_end_flag() \
({ write_reg( (MC_V_BASE + MC_STATUS), 0); \
})
#define set_mc_flag() \
({ write_reg( (MC_V_BASE + MC_STATUS), 3); \
})
#define set_mc_dha(dha_val) \
({ write_reg( (MC_V_BASE + MC_DES_DHA), dha_val); \
})
#define set_mc_dcs() \
({ write_reg( (MC_V_BASE + MC_DCS), 1); \
})
#define set_i_flag() \
({ write_reg( (MC_V_BASE + FRAME_TYP), 0); \
})
#define set_p_flag() \
({ write_reg( (MC_V_BASE + FRAME_TYP), 1); \
})
#define set_b_flag() \
({ write_reg( (MC_V_BASE + FRAME_TYP), 2); \
})
#define mc_cache_flush run_mc
#define mc_polling_end()                                        \
{                                                               \
  do {                                                          \
  } while (polling_mc_end_flag() != 3);                         \
}                         

#define mc_polling_blk_flag()                                   \
{                                                               \
  do {                                                          \
  } while (!polling_mc_end_flag());				\
}                         

#endif
