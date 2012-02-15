
#ifndef __JZ4750E_DBLK_H__
#define __JZ4750E_DBLK_H__

#include "jzh264/h264_tcsm.h"

#define TYAW 20
#define TYAH 20

// DATA_FMT
#define VIDEO_FMT 1

#ifdef AUX_VADDR_SPACE
#define DBLK_BASE 0x130B0000
#else
#define DBLK_BASE 0xB30B0000
#endif
#define OFST_CTR 0x0
#define OFST_FMT 0x4
#define OFST_INADDR 0x34
#define OFST_INSTRIDE 0x30
#define OFST_OUTADDR 0x2c
#define OFST_OUTSTRIDE 0x28
#define OFST_UPADDR 0x1c
#define OFST_UPSTRIDE 0x14
#define OFST_QP 0x8
#define OFST_DEPTH 0xc
#define OFST_COEF0 0x10
#define OFST_COEF1 0x18
#define OFST_COEF2 0x20
#define OFST_COEF3 0x24

#define OFST_AB 0x90 
#define OFST_TC0 0xd8 

#define OFST_DCS 0x84
#define OFST_DHA 0x88
#define OFST_CSA 0x8c

#define OFST_DBG 0x130

#define write_reg(reg, val) ({   i_sw((val), (reg), 0);   })
#define read_reg(reg, off) ({     i_lw((reg), (off));   })

#define set_data_fmt( MB_SIZE, YUV_FLAG, \
	       INTER_EN, UP_EN, LEFT_EN, VIDEO_FMT) \
     ({write_reg((DBLK_BASE + OFST_FMT), \
((MB_SIZE << 11) + \
(YUV_FLAG << 9) + \
(INTER_EN << 8) + \
(UP_EN << 7) + \
(LEFT_EN << 6) + \
VIDEO_FMT));})

#define set_in_addr(in_adr) ({write_reg((DBLK_BASE + OFST_INADDR), \
                                        in_adr); })

#define set_in_stride(in_stride) ({write_reg((DBLK_BASE + OFST_INSTRIDE), \
                                             in_stride); })

#define set_out_addr(out_adr) ({write_reg((DBLK_BASE + OFST_OUTADDR), \
                                          out_adr); })

#define set_out_stride(out_stride) ({write_reg((DBLK_BASE + OFST_OUTSTRIDE), \
                                             out_stride); })

#define set_up_addr(up_adr) ({write_reg((DBLK_BASE + OFST_UPADDR), \
                                        up_adr); })

#define set_up_stride(up_stride) ({write_reg((DBLK_BASE + OFST_UPSTRIDE), \
                                             up_stride); })

#define set_qp( QP_UP, QP_LEFT, QP_CURR) \
({write_reg((DBLK_BASE + OFST_QP), \
            ((QP_UP << 14) + \
             (QP_LEFT << 7) + \
             QP_CURR));})

#define set_depth_offset(OFFSET_A, OFFSET_B, BITDEPTH, \
		    DEPTH_SET) \
({write_reg((DBLK_BASE + OFST_DEPTH), \
            (((OFFSET_A & 0xff) << 24) | \
             ((OFFSET_B & 0xff) << 16) | \
             (BITDEPTH << 12) | \
             DEPTH_SET));})

#define set_h264_coef0( EDGE7, EDGE6, EDGE5, EDGE4,\
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

#define set_h264_coef1(EDGE15, EDGE14, EDGE13, EDGE12, \
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

#define set_h264_coef2(EDGE23, EDGE22, EDGE21, EDGE20, \
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

#define set_h264_coef3(EDGE31, EDGE30, EDGE29, EDGE28, \
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

#define READ_DBLK_DBG() \
({ (read_reg(DBLK_BASE, OFST_DBG)); \
})


#define SET_DBLK_CTRL(DBLK_END, DBLK_RST, DBLK_EN) \
({write_reg((DBLK_BASE + OFST_CTR), \
            ((DBLK_END << 2) + \
             (DBLK_RST << 1) + \
             DBLK_EN));})

#define run_dblk() \
({ write_reg( (DBLK_BASE +  OFST_CTR), (read_reg(DBLK_BASE, OFST_CTR)) | 0x1); \
})
#define POLLING_DBLK_END() \
({ (read_reg(DBLK_BASE, OFST_CTR)) & 0x4; \
})

#define READ_DCS_DBLK() \
({ (read_reg(DBLK_BASE, OFST_DCS)); \
})
#define READ_DHA_DBLK() \
({ (read_reg(DBLK_BASE, OFST_DHA)); \
})
#define READ_CSA_DBLK() \
({ (read_reg(DBLK_BASE, OFST_CSA)); \
})

#define SET_DCS_DBLK(val) \
({write_reg((DBLK_BASE + OFST_DCS), val);})

#define SET_DHA_DBLK(val) \
({write_reg((DBLK_BASE + OFST_DHA), val);})

#define set_csa(val) \
({write_reg((DBLK_BASE + OFST_CSA), val);})

#define data_fmt( MB_SIZE, YUV_FLAG, \
	       INTER_EN, UP_EN, LEFT_EN, VIDEO_FMT) \
((MB_SIZE << 11) + \
(YUV_FLAG << 9) + \
(INTER_EN << 8) + \
(UP_EN << 7) + \
(LEFT_EN << 6) + \
VIDEO_FMT)
#define qp( QP_UP, QP_LEFT, QP_CURR) \
((QP_UP << 14) + \
(QP_LEFT << 7) + \
QP_CURR)
#define depth_offset(OFFSET_A, OFFSET_B, BITDEPTH, \
		    DEPTH_SET) \
(((OFFSET_A & 0xff) << 24) | \
((OFFSET_B & 0xff) << 16) | \
(BITDEPTH << 12) | \
DEPTH_SET)
#define set_h264_bs( EDGE7, EDGE6, EDGE5, EDGE4,\
	     EDGE3, EDGE2, EDGE1, EDGE0) \
((EDGE7 << 28) + \
(EDGE6 << 24) + \
(EDGE5 << 20) + \
(EDGE4 << 16) + \
(EDGE3 << 12) + \
(EDGE2 << 8) + \
(EDGE1 << 4) + \
EDGE0)

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
  unsigned int nod_hd0_y;
  unsigned int nodhead_y;
  unsigned int dat_inadr_y;
  unsigned int dat_instrd_y;
  unsigned int dat_outadr_y;
  unsigned int dat_outstrd_y;
  unsigned int dat_bsh23_y; 	      		   
  unsigned int dat_bsh01_y; 	      		   
  unsigned int dat_upadr_y;
  unsigned int dat_bsv23_y; 	      		   
  unsigned int dat_upstrd_y;
  unsigned int dat_bsv01_y; 	      		   
  unsigned int dat_ofstab_y;
  unsigned int dat_qp_y;
  unsigned int dat_fmt_y;
  unsigned int dat_ctrl_y;
  unsigned int nod_hd0_u;
  unsigned int nodhead_u;
  unsigned int dat_inadr_u;
  unsigned int dat_instrd_u;
  unsigned int dat_outadr_u;
  unsigned int dat_outstrd_u;
  unsigned int dat_bsh23_u; 	      		   
  unsigned int dat_bsh01_u; 	      		   
  unsigned int dat_upadr_u;
  unsigned int dat_bsv23_u; 	      		   
  unsigned int dat_upstrd_u;
  unsigned int dat_bsv01_u; 	      		   
  unsigned int dat_ofstab_u;
  unsigned int dat_qp_u;
  unsigned int dat_fmt_u;
  unsigned int dat_ctrl_u;
  unsigned int nod_hd0_v;
  unsigned int nodhead_v;
  unsigned int dat_inadr_v;
  unsigned int dat_instrd_v;
  unsigned int dat_outadr_v;
  unsigned int dat_outstrd_v;
  unsigned int dat_bsh23_v; 	      		   
  unsigned int dat_bsh01_v; 	      		   
  unsigned int dat_upadr_v;
  unsigned int dat_bsv23_v; 	      		   
  unsigned int dat_upstrd_v;
  unsigned int dat_bsv01_v; 	      		   
  unsigned int dat_ofstab_v;
  unsigned int dat_qp_v;
  unsigned int dat_fmt_v;
  unsigned int dat_ctrl_v;
  unsigned int anodhead;
  unsigned int anodcmd;
  unsigned int anodinfa;
  unsigned int anodinfd;
} dat_nod_yuv_t;//52words

#endif //__JZ4750E_DBLK_H__
