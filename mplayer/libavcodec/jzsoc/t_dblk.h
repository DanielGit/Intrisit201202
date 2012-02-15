//#undef NDEBUG
//#include <assert.h>
#ifndef __TC_H264_H__
#define __TC_H264_H__


#define TYAW 20
#define TYAH 20


// DATA_FMT
#define VIDEO_FMT 1
// in stride
//#define IN_STRD 20
// out stride
//#define OUT_STRD 20
// up stride
//#define UP_STRD 20

#define DBLK_BASE 0xB30B0000
//#define DBLK_BASE dblk_base
#define OFST_CTR 0x0
#define OFST_FMT 0x4
#define OFST_MBINF 0x8
#define OFST_INADDR 0xC
#define OFST_INSTRIDE 0x10
#define OFST_OUTADDR 0x14
#define OFST_OUTSTRIDE 0x18
#define OFST_UPADDR 0x1C
#define OFST_UPSTRIDE 0x20
#define OFST_QP 0x24
#define OFST_DEPTH 0x28
#define OFST_COEF0 0x2C
#define OFST_COEF1 0x30
#define OFST_COEF2 0x34
#define OFST_COEF3 0x38

#define OFST_AB 0x3C 
#define OFST_TC0 0x84 

#define OFST_DCS 0xcc
#define OFST_DHA 0xd0

// defined at h264data.h
static const int alpha_tab_old[52] = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   4,   4,   5,   6,
  7,   8,   9,  10,  12,  13,  15,  17,  20,  22,
  25,  28,  32,  36,  40,  45,  50,  56,  63,  71,
  80,  90, 101, 113, 127, 144, 162, 182, 203, 226,
  255, 255
};

static const int beta_tab_old[52] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  2,  2,  2,  3,
  3,  3,  3,  4,  4,  4,  6,  6,  7,  7,
  8,  8,  9,  9, 10, 10, 11, 11, 12, 12,
  13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
  18, 18
};

static const int tc0_tab_old[52][3] = {
  { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 
  { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 
  { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
  { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
  { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, 
  { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 1 }, { 1, 1, 1 },
  { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, 
  { 1, 1, 2 }, { 1, 1, 2 }, { 1, 1, 2 }, { 1, 2, 3 }, 
  { 1, 2, 3 }, { 2, 2, 3 }, { 2, 2, 4 }, { 2, 3, 4 },
  { 2, 3, 4 }, { 3, 3, 5 }, { 3, 4, 6 }, { 3, 4, 6 },
  { 4, 5, 7 }, { 4, 5, 8 }, { 4, 6, 9 }, { 5, 7,10 }, 
  { 6, 8,11 }, { 6, 8,13 }, { 7,10,14 }, { 8,11,16 },
  { 9,12,18 }, {10,13,20 }, {11,15,23 }, {13,17,25 }
};


#define write_reg(reg, val) ({   i_sw((val), (reg), 0);   })
#define read_reg(reg, off) ({     i_lw((reg), (off));   })


#define SET_DATA_FMT( MB_SIZE, YUV_FLAG, \
	       INTER_EN, UP_EN, LEFT_EN, VIDEO_FMT) \
     ({write_reg((DBLK_BASE + OFST_FMT), \
((MB_SIZE << 11) + \
(YUV_FLAG << 9) + \
(INTER_EN << 8) + \
(UP_EN << 7) + \
(LEFT_EN << 6) + \
VIDEO_FMT));})

#define SET_IN_ADDR(in_adr) ({write_reg((DBLK_BASE + OFST_INADDR), \
                                        in_adr); })

#define SET_IN_STRIDE(in_stride) ({write_reg((DBLK_BASE + OFST_INSTRIDE), \
                                             in_stride); })

#define SET_OUT_ADDR(out_adr) ({write_reg((DBLK_BASE + OFST_OUTADDR), \
                                          out_adr); })

#define SET_OUT_STRIDE(out_stride) ({write_reg((DBLK_BASE + OFST_OUTSTRIDE), \
                                             out_stride); })

#define SET_UP_ADDR(up_adr) ({write_reg((DBLK_BASE + OFST_UPADDR), \
                                        up_adr); })

#define SET_UP_STRIDE(up_stride) ({write_reg((DBLK_BASE + OFST_UPSTRIDE), \
                                             up_stride); })

#define SET_QP( QP_UP, QP_LEFT, QP_CURR) \
({write_reg((DBLK_BASE + OFST_QP), \
            ((QP_UP << 14) + \
             (QP_LEFT << 7) + \
             QP_CURR));})

#define SET_DEPTH_OFFSET(OFFSET_A, OFFSET_B, BITDEPTH, \
		    DEPTH_SET) \
({write_reg((DBLK_BASE + OFST_DEPTH), \
            (((OFFSET_A & 0xff) << 24) | \
             ((OFFSET_B & 0xff) << 16) | \
             (BITDEPTH << 12) | \
             DEPTH_SET));})

#define SET_H264_COEF0( EDGE7, EDGE6, EDGE5, EDGE4,\
	     EDGE3, EDGE2, EDGE1, EDGE0) \
({write_reg((DBLK_BASE + OFST_COEF0), \
            ((EDGE7 << 28) + \
(EDGE6 << 24) + \
(EDGE5 << 20) + \
(EDGE4 << 16) + \
(EDGE3 << 12) + \
(EDGE2 << 8) + \
(EDGE1 << 4) + \
EDGE0));})

#define SET_H264_COEF1(EDGE15, EDGE14, EDGE13, EDGE12, \
	    EDGE11, EDGE10, EDGE9, EDGE8) \
({write_reg((DBLK_BASE + OFST_COEF1), \
            ((EDGE15 << 28) + \
(EDGE14 << 24) + \
(EDGE13 << 20) + \
(EDGE12 << 16) + \
(EDGE11 << 12) + \
(EDGE10 << 8) + \
(EDGE9 << 4) + \
EDGE8));})

#define SET_H264_COEF2(EDGE23, EDGE22, EDGE21, EDGE20, \
	    EDGE19, EDGE18, EDGE17, EDGE16) \
({write_reg((DBLK_BASE + OFST_COEF2), \
            ((EDGE23 << 28) + \
(EDGE22 << 24) + \
(EDGE21 << 20) + \
(EDGE20 << 16) + \
(EDGE19 << 12) + \
(EDGE18 << 8) + \
(EDGE17 << 4) + \
EDGE16));})

#define SET_H264_COEF3(EDGE31, EDGE30, EDGE29, EDGE28, \
	    EDGE27, EDGE26, EDGE25, EDGE24) \
({write_reg((DBLK_BASE + OFST_COEF3), \
            ((EDGE31 << 28) + \
(EDGE30 << 24) + \
(EDGE29 << 20) + \
(EDGE28 << 16) + \
(EDGE27 << 12) + \
(EDGE26 << 8) + \
(EDGE25 << 4) + \
EDGE24));})


#define SET_AB_LUT(offset, ab) \
({write_reg((DBLK_BASE + OFST_AB + offset), ab);})

#define SET_TC0_LUT(offset, tc0) \
({write_reg((DBLK_BASE + OFST_TC0 + offset), tc0);})

#define SET_DBLK_CTRL(DBLK_END, DBLK_RST, DBLK_EN) \
({write_reg((DBLK_BASE + OFST_CTR), \
            ((DBLK_END << 2) + \
             (DBLK_RST << 1) + \
             DBLK_EN));})

#define RUN_DBLK() \
({ write_reg( (DBLK_BASE +  OFST_CTR), (read_reg(DBLK_BASE, OFST_CTR)) | 0x1); \
})

#define POLL_DBLK_END() \
({ (read_reg(DBLK_BASE, OFST_CTR)) & 0x4;	\
})

#define DBLK_POLLING_END()         \
({ do {} while (!POLL_DBLK_END()); \
})

#define SET_DCS_DBLK(val) \
({write_reg((DBLK_BASE + OFST_DCS), val);})

#define SET_DHA_DBLK(val) \
({write_reg((DBLK_BASE + OFST_DHA), val);})

#define dblk_polling_end DBLK_POLLING_END

/*==============================================================
  HISTORY:
  2008-3-25:
  1)delete Y_TRANS, FIELD_MODE in L91; delete (Y_TRANS << 14) + \ and (FIELD_MODE << 13) + \ in L95,96;
  2)delete QP_UP_INTER, QP_UP_LEFT in L124; delete (QP_UP_INTER << 27) + \ (QP_UP_LEFT << 21) + \ in L126,127;
  3)delete OFFSET_SET in L131; delete (OFFSET_SET << 1) + \ in L135
  4)L192,irq?
              
*/

#define FIN_HEIGHT 16				
#define FIN_WIDTH 20
#define FUP_HEIGHT 4
#define FUP_WIDTH 16
#define FOUT_HEIGHT 16
#define FOUT_WIDTH 20
#define YBS_ARY 32
#define FINUV_HEIGHT 8
#define FINUV_WIDTH 12
#define FUPUV_HEIGHT 4
#define FUPUV_WIDTH 8
#define FOUTUV_HEIGHT 8
#define FOUTUV_WIDTH 12
#define UVBS_ARY 16

typedef struct {
  
  unsigned int nodhead_y;
  unsigned int ofst_fmt_y;    // 0x4
  unsigned int dat_fmt_y;
  unsigned int ofst_inadr_y;  // 0xc
  unsigned int dat_inadr_y;
  unsigned int ofst_instrd_y; // 0x10
  unsigned int dat_instrd_y;
  unsigned int ofst_outadr_y; // 0x14
  unsigned int dat_outadr_y; 
  unsigned int ofst_outstrd_y;// 0x18
  unsigned int dat_outstrd_y;
  unsigned int ofst_upadr_y;  // 0x1c
  unsigned int dat_upadr_y;
  unsigned int ofst_upstrd_y; // 0x20
  unsigned int dat_upstrd_y; 
  unsigned int ofst_qp_y;     // 0x24
  unsigned int dat_qp_y; 
  unsigned int ofst_ofstab_y; // 0x28
  unsigned int dat_ofstab_y; 
  unsigned int ofst_bsv01_y;  // 0x2C
  unsigned int dat_bsv01_y; 
  unsigned int ofst_bsv23_y;  // 0x30
  unsigned int dat_bsv23_y; 
  unsigned int ofst_bsh01_y;  // 0x34
  unsigned int dat_bsh01_y; 
  unsigned int ofst_bsh23_y;  // 0x38
  unsigned int dat_bsh23_y; 
  unsigned int ofst_ctrl_y;   // 0x0
  unsigned int dat_ctrl_y;
  unsigned int nodhead_u;
  unsigned int ofst_fmt_u;    // 0x4
  unsigned int dat_fmt_u;
  unsigned int ofst_inadr_u;  // 0xc
  unsigned int dat_inadr_u;
  unsigned int ofst_instrd_u; // 0x10
  unsigned int dat_instrd_u;
  unsigned int ofst_outadr_u; // 0x14
  unsigned int dat_outadr_u; 
  unsigned int ofst_outstrd_u;// 0x18
  unsigned int dat_outstrd_u;
  unsigned int ofst_upadr_u;  // 0x1c
  unsigned int dat_upadr_u;
  unsigned int ofst_upstrd_u; // 0x20
  unsigned int dat_upstrd_u; 
  unsigned int ofst_qp_u;     // 0x24
  unsigned int dat_qp_u; 
  unsigned int ofst_bsv01_u;  // 0x2C
  unsigned int dat_bsv01_u; 
  unsigned int ofst_bsh01_u;  // 0x34
  unsigned int dat_bsh01_u; 
  unsigned int ofst_ctrl_u;   // 0x0
  unsigned int dat_ctrl_u;
  unsigned int nodhead_v;
  unsigned int ofst_fmt_v;    // 0x4
  unsigned int dat_fmt_v;
  unsigned int ofst_inadr_v;  // 0xc
  unsigned int dat_inadr_v;
  unsigned int ofst_outadr_v; // 0x14
  unsigned int dat_outadr_v; 
  unsigned int ofst_upadr_v;  // 0x1c
  unsigned int dat_upadr_v;
  unsigned int ofst_ctrl_v;   // 0x0
  unsigned int dat_ctrl_v;

} desp_yuv_t;

#endif
