
#include "../jzmedia.h"
#include "h264_tcsm.h"
#include "h264_aux_intra.h"


void Intra_pred_chroma(int chroma_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *top)
{
  int i;

  uint8_t *src_top;  // top address
  uint8_t *src_topleft, *src_left;  // left address

  int pred_mode = chroma_pred_mode;

  src_left = src - 0x1;

  switch (pred_mode) {

  case VERT_PRED8x8:
    S32LDD(xr5, top, 0x0);
    S32LDD(xr6, top, 0x4);
    break;

  case HOR_PRED8x8:
    dst -= RECON_BUF_STRIDE;
    for (i=0; i<8; i++) {
      S8LDD(xr1, src_left, 0x0, ptn7);
      src_left = src_left + RECON_BUF_STRIDE;
      S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
      S32STD(xr1, dst, 0x4);
    }
    break;

  case DC_PRED8x8:
    // load top
    S32LDD(xr11, top, 0x0);
    S32LDD(xr12, top, 0x4);
    // load left (4 x 7 = 28 instructions)
    S8LDD(xr7, src_left, 0x0, ptn0);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn3);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn0);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn3);
    // AVG
    D8SUMC(xr1, xr11, xr7);
    Q16ADD_AA_XW(xr2, xr1, xr1, xr0);
    D32SLR(xr3, xr2, xr0, xr0, 0x3);
    S32SFL(xr4, xr3, xr3, xr0, ptn0);
    S32SFL(xr0, xr4, xr4, xr5, ptn3);

    D8SUMC(xr1, xr12, xr8);
    Q16ADD_AA_XW(xr2, xr1, xr1, xr0);
    D32SLR(xr3, xr2, xr0, xr0, 0x3);
    S32SFL(xr4, xr3, xr3, xr0, ptn0);
    S32SFL(xr0, xr4, xr4, xr6, ptn3);

    D32SLR(xr2, xr1, xr0, xr0, 0x2);
    S32SFL(xr3, xr2, xr2, xr4, ptn0);
    S32SFL(xr0, xr3, xr3, xr8, ptn3);
    S32SFL(xr0, xr4, xr4, xr9, ptn3);
    // store
    S32STD(xr5, dst, 0x0);
    S32STD(xr8, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr8, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr8, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr8, dst, 0x4);

    S32SDIV(xr9, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr9, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr9, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr9, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    break;

  case PLANE_PRED8x8:
    src_top = top;
    src_topleft = src_top - 4;
    src_left = src - 0x4;

    //----- H, LOAD -----
    S32LDD(xr1, src_top, -4);  // xr1 <- src_top[-4];  xr1: lt, 0, 0, 0 ;
    S32LDD(xr3, src_top, 0x0);   // xr3 <- src_top[0] ;  xr3: t3, t2, t1, t0 ;
    S32LDDR(xr2, src_top, 0x4);  // xr2 <- src_top[4] ;  xr2: t4, t5, t6, t7 ;
    S32ALNI(xr1, xr3, xr1, ptn1);//                      xr1: t2, t1, t0, lt ;
    S32I2M(xr8, MUL_12); // xr8: 0x00010002 ;
    S32I2M(xr9, MUL_34); // xr9: 0x00030004 ;
    //----- H, SUM -----
    Q8ADDE_SS(xr3, xr2, xr1, xr4);  // xr3[31:16] <- t4-t2 ;  xr3[15:0] <- t5-t1 ;
    // xr4[31:16] <- t6-t0 ;  xr4[15:0] <- t7-lt;

    S32LDD(xr1, src_topleft, 0x0);          // xr1[31:24] <- src_topleft[3] (lt) ;

    D16MUL_WW(xr5, xr8, xr3, xr6);    // xr5 <- 1*(t4-t2) ;  xr6 <- 2*(t5-t1) ;
    D16MAC_AA_WW(xr5, xr9, xr4, xr6); // xr5 <- 1*(t4-t2)+3*(t6-t0) ; xr6 <- 2*(t5-t1)+4*(t7-lt) ;

    S32LDD(xr12, src_left, 0x0);//xr12[31:24] <- src_topleft[stride+3] (l0) ;
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0); // xr3[31:24] <- src_topleft[2*stride+3] (l1) ;

    D32ADD_AA(xr7, xr5, xr6, xr0); // xr7 <- 1*(t4-t2)+3*(t6-t0)+2*(t5-t1)+4*(t7-lt) ;
    //----- V, LOAD -----
    //  S32LDD(xr1, src_topleft, 0x0);          // xr1[31:24] <- src_topleft[3] (lt) ;
    //  S32LDIV(xr12, src_topleft, stride, 0x0);//xr12[31:24] <- src_topleft[stride+3] (l0) ;
    //  S32LDIV(xr3, src_topleft, stride, 0x0); // xr3[31:24] <- src_topleft[2*stride+3] (l1) ;
    S32LDIV(xr4, src_left, RECON_BUF_STRIDE, 0x0); // xr4[31:24] <- src_topleft[3*stride+3] (l2) ;
    S32SFL(xr5, xr12, xr1, xr0, ptn2);      // xr5[31:16] <- l0, lt ;
    S32SFL(xr6, xr4, xr3, xr0, ptn2);       // xr8[31:16] <- l2, l1 ;
    S32SFL(xr10, xr6, xr5, xr0, ptn3);      // xr10[31:0] <- l2, l1, l0, lt ;
    src_left += RECON_BUF_STRIDE;
    S32LDIV(xr4, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr12, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr1, src_left, RECON_BUF_STRIDE, 0x0);
    S32SFL(xr6, xr4, xr3, xr0, ptn2);
    S32SFL(xr5, xr12, xr1, xr0, ptn2);
    S32SFL(xr11, xr6, xr5, xr0, ptn3); // xr11[31:0] <- l4, l5, l6, l7 ;
    //----- V, SUM -----
    Q8ADDE_SS(xr3, xr11, xr10, xr4);

    S32LUI(xr1, 0x1, ptn0); // xr1[31:0]: 0x00000001 ;

    D16MUL_WW(xr5, xr8, xr3, xr6);
    D16MAC_AA_WW(xr5, xr9, xr4, xr6);

    D32ADD_AA(xr13, xr5, xr6, xr0); // xr13 <- 1*(l4-l2)+3*(l6-l0)+2*(l5-l1)+4*(l7-lt) ;

    //----- P, CAL ----- useful XRs:xr13, xr7, xr2, xr11;
    //  S32LUI(xr1, 0x1, ptn0); // xr1[31:0]: 0x00000001 ;
    D32SLL(xr5, xr1, xr1, xr6, 0x4); // xr5: 0x00000010;  xr6: 0x00000010; 
    D32SLL(xr3, xr13, xr7, xr4, 0x4);
    D32ACC_AA(xr5, xr13, xr3, xr0); // xr5: 17*V+16
    D32ACC_AA(xr6, xr7, xr4, xr0);  // xr6: 17*H+16

    Q8ACCE_AA(xr0, xr2, xr11, xr1);  // xr1[15:0]: src1[0] + src2[8] + 1

    D32SLR(xr8, xr5, xr6, xr9, 0x5); // xr8: (17*V+16) >> 5 ;  xr9: (17*H+16) >> 5 ;

    //  Q8ACCE_AA(xr0, xr2, xr11, xr1);  // xr1[15:0]: src1[0] + src2[8] + 1
    D32SLL(xr2, xr1, xr0, xr0, 0x4); // xr2[15:0]: 16*(src1[0] + src2[16] + 1)

    Q16ADD_AA_WW(xr7, xr8, xr9, xr0); // xr7: V+H
    S32I2M(xr4, MUX_H16); // xr4: 0x0000ffff ;
    D32SLL(xr12, xr7, xr0, xr0, 0x1);
    D32ADD_AA(xr5, xr12, xr7, xr0);   // xr5: 3*(V+H)
    //  S32LUI(xr12, 0x3, ptn0); // xr12[31:0]: 0x00000003 ;
    //  D16MUL_WW(xr0, xr7, xr12, xr5);   // xr5: 3*(V+H)

    //  S32I2M(xr4, MUX_H16); // xr4: 0x0000ffff ;

    Q16ADD_SS_WW(xr6, xr2, xr5, xr0); // xr6: 16*(src1[0] + src2[16] + 1) - 3*(V+H)

    //  S32I2M(xr4, MUX_H16); // xr4: 0x0000ffff ;

    S32SFL(xr0, xr8, xr8, xr14, ptn3);// xr14[31:16]: V ;  xr14[15:0]: V ;
    S32SFL(xr0, xr6, xr6, xr5, ptn3); // xr5[31:16]: a ;  xr5[15:0]: a ;
    D32SLL(xr7, xr9, xr0, xr0, 0x1);
    S32SFL(xr0, xr7, xr7, xr8, ptn3); // xr8[31:16]: 2H ;  xr8[15:0]: 2H ;

    //  S32I2M(xr4, MUX_H16); // xr4: 0x0000ffff ;
    S32AND(xr9, xr4, xr9);

    Q16ADD_AA_WW(xr15, xr5, xr9, xr0);   // xr15[31:16]: a ;  xr15[15:0]: a + H ;

    dst -= RECON_BUF_STRIDE;
    //----- SRC, STORE -----
    for (i=0; i<8; i++) {
      Q16ADD_AA_WW(xr1, xr15, xr8, xr0);
      Q16ADD_AA_WW(xr2, xr1, xr8, xr0);
      Q16SAR(xr9, xr15, xr1, xr1, 0x5);
      Q16ADD_AA_WW(xr3, xr2, xr8, xr0);

      Q16SAT(xr10, xr9, xr1);
      //    Q16SAR(xr9, xr15, xr1, xr1, 0x5);
      Q16SAR(xr2, xr2, xr3, xr3, 0x5);

      //    Q16SAT(xr10, xr9, xr1);
      Q16SAT(xr11, xr2, xr3);

      S32SDIVR(xr10, dst, RECON_BUF_STRIDE, 0x0);

      Q16ADD_AA_WW(xr15, xr15, xr14, xr0);

      S32STDR(xr11, dst, 0x4);
    }
    break;

  case LEFT_DC_PRED8x8:
    S8LDD(xr7, src_left, 0x0, ptn0);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn3);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn0);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn3);
    // AVG
    D8SUMC(xr1, xr7, xr8);
    D32SLR(xr2, xr1, xr0, xr0, 0x2);
    S32SFL(xr3, xr2, xr2, xr4, ptn0);
    S32SFL(xr0, xr3, xr3, xr5, ptn3);
    S32SFL(xr0, xr4, xr4, xr6, ptn3);
    // store
    S32STD(xr5, dst, 0x0);
    S32STD(xr5, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr5, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr5, dst, 0x4);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr5, dst, 0x4);
    S32SDIV(xr6, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr6, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr6, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    S32SDIV(xr6, dst, RECON_BUF_STRIDE, 0x0);
    S32STD(xr6, dst, 0x4);
    break;

  case TOP_DC_PRED8x8:
    src_top = top;
    // load top
    S32LDD(xr7, src_top, 0x0);
    S32LDD(xr8, src_top, 0x4);
    // AVG
    D8SUMC(xr1, xr7, xr8);
    D32SLR(xr2, xr1, xr0, xr0, 0x2);
    S32SFL(xr3, xr2, xr2, xr4, ptn0);
    S32SFL(xr0, xr3, xr3, xr5, ptn3);
    S32SFL(xr0, xr4, xr4, xr6, ptn3);
    break;

  case DC_128_PRED8x8:
    //load
    S32LUI(xr5, 0x80, ptn7);
    S32OR(xr6, xr5, xr0);
    break;

  default:
    break;

  }

  if ( (pred_mode == VERT_PRED8x8) ||
       (pred_mode == DC_128_PRED8x8) ||
       (pred_mode == TOP_DC_PRED8x8) ) {

    dst -= RECON_BUF_STRIDE;
    for(i=0; i<8; i++){
      S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
      S32STD(xr6, dst, 0x4);
    }
  }

}

void Intra_pred_luma_4x4(int luma_4x4_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *topright, uint8_t *top, uint8_t *topleft)
{
  uint8_t *src_left; // left address

  int pred_mode = luma_4x4_pred_mode;

  src_left = src - 0x1;

  switch (pred_mode) {
    
  case VERT_PRED: // 0
    //load
    S32LDD(xr1, top, 0x0);
    break;

  case HOR_PRED: // 1
    //load
    S8LDD(xr1, src_left, 0x0, ptn7);
    S8LDI(xr2, src_left, RECON_BUF_STRIDE, ptn7);
    S8LDI(xr3, src_left, RECON_BUF_STRIDE, ptn7);
    S8LDI(xr4, src_left, RECON_BUF_STRIDE, ptn7);
    // store
    S32STD(xr1, dst, 0x0);
    S32SDIV(xr2, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr3, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr4, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case DC_PRED: // 2
    //load
    S32LDD(xr8, top, 0x0); //xr8 <- src_top[0] ;
    S8LDD(xr1, src_left, 0x0, ptn0);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn3);
    //avg
    D8SUMC(xr2, xr1, xr8);
    Q16ADD_AA_XW(xr3, xr2, xr2, xr0);
    D32SLR(xr4, xr3, xr0, xr0, 0x3);
    S32SFL(xr6, xr4, xr4, xr0, ptn0);
    S32SFL(xr0, xr6, xr6, xr1, ptn3);
    break;

  case DIAG_DOWN_LEFT_PRED: // 3
    //load
    S32LDD(xr1, top, 0x0);        // xr1 <- t3, t2, t1, t0 ;
    S32LDD(xr2, topright, 0x0);   // xr2 <- t7, t6, t5, t4 ;
    S32LDDR(xr15, topright, 0x0); //xr15 <- t4, t5, t6, t7 ;

    S32ALNI(xr3, xr2, xr1, ptn2); // xr3: t5, t4, t3, t2 ;
    Q8AVG(xr4, xr1, xr3);
    S32ALNI(xr5, xr2, xr1, ptn3); // xr5: t4, t3, t2, t1 ;
    Q8AVGR(xr6, xr4, xr5);

    S32ALNI(xr7, xr2, xr1, ptn1); // xr7: t6, t5, t4, t3 ;
    S32ALNI(xr8, xr15, xr2, ptn3);// xr8: t7, t7, t6, t5 ;
    Q8AVG(xr9, xr7, xr8);
    Q8AVGR(xr10, xr9, xr2);

    D32SLL(xr11, xr6, xr0, xr0, 0x8);
    S32ALNI(xr12, xr10, xr11, ptn1);
    S32ALNI(xr13, xr10, xr11, ptn2);

    //store
    S32STD(xr6, dst, 0x0);
    S32SDIV(xr13, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr12, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr10, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case DIAG_DOWN_RIGHT_PRED: // 4
    // load right
    S32LDDR(xr8, top, 0x0);  // xr8: t0, t1, t2, t3 ;  high -> low, [31->0];
    S32LDDR(xr9, topleft, -0x4); // xr9[7:0]: lt ;
    // load left
    S32LDD(xr7, topleft, -0x4);          // xr7[31:24]: lt ;
    S8LDD(xr1, src_left, 0x0, ptn0);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn3);
    // alni
    S32ALNI(xr10, xr9, xr8, ptn3); // xr10: lt, t0, t1, t2 ;
    S32ALNI(xr11, xr1, xr7, ptn1); // xr11: l2, l1, l0, lt ;
    S32ALNI(xr12, xr11, xr8, ptn2);// xr12: l0, lt, t0, t1 ;
    S32ALNI(xr13, xr1, xr10, ptn2);// xr13: l1, l0, lt, t0 ;
    // cal
    Q8AVG(xr3, xr1, xr13);
    Q8AVGR(xr4, xr3, xr11); // xr4: src[0,3], src[0,2]/src[1,3], src[0,1]/src[1,2]/src[2,3],
    //      src[0,0]/src[1,1]/src[2,2]/src[3,3] ;
    Q8AVG(xr5, xr8, xr12);
    Q8AVGR(xr6, xr5, xr10); // xr6: src[0,0]/src[1,1]/src[2,2]/src[3,3],
    //      src[1,0]/src[2,1]/src[3,2], src[2,0]/src[3,1], src[3,0] ;
    // alni for store
    D32SLL(xr7, xr6, xr0, xr0, 0x8); // xr7: src[1,0]/src[2,1]/src[3,2], src[2,0]/src[3,1], src[3,0] ;
    S32ALNI(xr8, xr4, xr7, ptn1);
    S32ALNI(xr9, xr4, xr7, ptn2);
    //store
    S32STDR(xr6, dst, 0x0);
    S32SDIVR(xr9, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIVR(xr8, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIVR(xr4, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case VERT_RIGHT_PRED: // 5
    src_left = src -0x4;
    // load right
    S32LDD(xr8, top, 0x0); // xr8: t3, t2, t1, t0 ;  high -> low, [31->0];
    // load left
    S32LDD(xr1, topleft, -0x4); // xr1[31:24] <- src_topleft[3] (lt) ;
    S32LDD(xr2, src_left, 0x0); // xr2[31:24] <- src_topleft[stride+3] (l0) ;
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0); // xr3[31:24] <- src_topleft[2*stride+3] (l1) ;
    S32LDIV(xr4, src_left, RECON_BUF_STRIDE, 0x0); // xr4[31:24] <- src_topleft[3*stride+3] (l2) ;
    S32SFL(xr5, xr2, xr1, xr0, ptn2);       // xr5[31:16] <- l0, lt ;
    S32SFL(xr6, xr4, xr3, xr0, ptn2);       // xr6[31:16] <- l2, l1 ;
    S32SFL(xr7, xr3, xr2, xr0, ptn2);       // xr7[31:16] <- l1, l0 ;
    // alni
    S32ALNI(xr3, xr8, xr1, ptn1); // xr3: t2, t1, t0, lt ;
    S32ALNI(xr4, xr3, xr2, ptn1); // xr4: t1, t0, lt, l0 ;
    // cal
    Q8AVGR(xr1, xr3, xr8); // xr1: 
    Q8AVG(xr9, xr4, xr8);
    Q8AVGR(xr2, xr9, xr3); // xr2:
    Q8AVG(xr10, xr5, xr6);
    Q8AVGR(xr11, xr10, xr7); // xr11: src[0,3], src[0,2], ~, ~ ;
    // alni
    S32ALNI(xr12, xr2, xr11, ptn1);
    D32SLL(xr13, xr11, xr0, xr0, 0x8);
    S32ALNI(xr14, xr1, xr13, ptn1);
    // store
    S32STD(xr1, dst, 0x0);
    S32SDIV(xr2, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr14, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr12, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case HOR_DOWN_PRED: // 6
    src_left = src - 0x4;
    // load TOP
    S32LDDR(xr8, top, 0x0);      // xr8[31:0]: t0, t1, t2, t3 ;
    S32LDDR(xr15, topleft, -0x4);// xr15[7:0]: lt ;
    S32LDD(xr9, topleft, -0x4);  // xr9[31:24]: lt ;
    S32ALNI(xr10, xr15, xr8, ptn3); //xr10[31:0]: lt, t0, t1, t2 ;
    // load LEFT
    S32LDDR(xr1, src_left, 0x0);          // xr1[7:0] <- src_left[3] (l0) ;
    S32LDIVR(xr2, src_left, RECON_BUF_STRIDE, 0x0); // xr2[7:0] <- src_left[stride+3] (l1) ;
    S32LDIVR(xr3, src_left, RECON_BUF_STRIDE, 0x0); // xr3[7:0] <- src_left[2*stride+3] (l2) ;
    S32LDIVR(xr4, src_left, RECON_BUF_STRIDE, 0x0); // xr4[7:0] <- src_left[3*stride+3] (l3) ;
    S32SFL(xr0, xr2, xr1, xr5, ptn2);    // xr5[15:0] <- l1, l0 ;
    S32SFL(xr0, xr4, xr3, xr6, ptn2);    // xr6[15:0] <- l3, l2 ;
    S32SFL(xr0, xr6, xr5, xr7, ptn3);    // xr7[31:0] <- l3, l2, l1, l0 ;
    // ALNI for CAL
    S32ALNI(xr11, xr7, xr9, ptn1);  // xr11: l2, l1, l0, lt ;
    S32ALNI(xr12, xr1, xr10, ptn3); // xr12: l0, lt, t0, t1 ;
    D32SLL(xr0, xr0, xr11, xr13, 0x8); // xr13: l1, l0, lt, 0 ;
    // CAL
    Q8AVGR(xr1, xr11, xr7); // xr1: src[0,3], src[0,2]/src[2,3], src[0,1]/src[2,2], src[0,0]/src[2,1] ;
    Q8AVG(xr2, xr12, xr8);
    Q8AVGR(xr3, xr2, xr10); // xr3: src[1,0]/src[3,1], src[2,0], src[3,0], ~ ;
    Q8AVG(xr4, xr13, xr7);
    Q8AVGR(xr5, xr4, xr11); // xr5: src[1,3], src[1,2]/src[3,3], src[1,1]/src[3,2], ~ ;
    // ALNI for STORE
    S32ALNI(xr8, xr1, xr3, ptn3); // xr8: src[0,0]/src[2,1], src[1,0]/src[3,1], src[2,0], src[3,0] ;
    S32SFL(xr9, xr1, xr5, xr10, ptn0); // xr9: src[0,3], src[1,3], src[0,2]/src[2,3], src[1,2]/src[3,3] ;
    //xr10: src[0,1]/src[2,2], src[1,1]/src[3,2], src[0,0]/src[2,1], ~ ;
    S32SFL(xr11, xr10, xr8, xr0, ptn3); // xr11: src[0,1]/src[2,2], src[1,1]/src[3,2],
    //       src[0,0]/src[2,1], src[1,0]/src[3,1] ;
    S32ALNI(xr12, xr9, xr10, ptn2); // xr12: src[0,2]/src[2,3], src[1,2]/src[3,3],
    //       src[0,1]/src[2,2], src[1,1]/src[3,2] ;
    // STORE
    S32STDR(xr8, dst, 0x0);
    S32SDIVR(xr11, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIVR(xr12, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIVR(xr9, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case VERT_LEFT_PRED: // 7
    //load
    S32LDD(xr1, top, 0x0);      // xr1 <- t3, t2, t1, t0 ;
    S32LDD(xr2, topright, 0x0); // xr2 <- t7, t6, t5, t4 ;

    S32ALNI(xr3, xr2, xr1, ptn3); // xr3: t4, t3, t2, t1 ;
    S32ALNI(xr4, xr2, xr1, ptn2); // xr4: t5, t4, t3, t2 ;
    Q8AVGR(xr11, xr1, xr3);
    Q8AVGR(xr5, xr4, xr3);

    Q8AVG(xr7, xr1, xr4);
    Q8AVGR(xr8, xr7, xr3);

    S32ALNI(xr6, xr2, xr1, ptn1); // xr6: t6, t5, t4, t3 ;
    Q8AVG(xr9, xr3, xr6);
    Q8AVGR(xr10, xr9, xr4);

    //store
    S32STD(xr11, dst, 0x0);
    S32SDIV(xr8, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr5, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr10, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case HOR_UP_PRED: // 8
    //load
    S8LDD(xr1, src_left, 0x0, ptn0);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn3);

    D32SLL(xr2, xr1, xr0, xr0, 0x8);  // xr2: l2, l1, l0, 0 ;
    S32SFL(xr3, xr1, xr1, xr0, ptn0); // xr3: l3, l3, l2, l2;

    Q8AVGR(xr4, xr1, xr2); // xr4: src[2,1]/src[0,2], src[2,0]/src[0,1], src[0,0], ~ ;

    Q8AVG(xr5, xr2, xr3);
    Q8AVGR(xr6, xr5, xr1); // xr6: src[3,1]/src[1,2], src[3,0]/src[1,1], src[1,0], ~ ;

    S32SFL(xr7, xr6, xr4, xr0, ptn0); // xr7: src[3,1]/src[1,2], src[2,1]/src[0,2],
    //      src[3,0]/src[1,1], src[2,0]/src[0,1];

    D32SLR(xr8, xr4, xr6, xr9, 0x8); // xr8: 0, src[2,1]/src[0,2], src[2,0]/src[0,1], src[0,0] ;
    // xr9: 0, src[3,1]/src[1,2], src[3,0]/src[1,1], src[1,0] ;
    S32SFL(xr0, xr9, xr8, xr10, ptn0); // xr10: src[3,0], src[2,0], src[1,0], src[0,0] ;

    S32SFL(xr11, xr3, xr7, xr0, ptn3); // xr11: l3, l3, src[3,1]/src[1,2], src[2,1]/src[0,2] ;

    S32SFL(xr12, xr3, xr3, xr0, ptn3); // xr12: l3, l3, l3, l3 ;

    //store
    S32STD(xr10, dst, 0x0);
    S32SDIV(xr7, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr11, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr12, dst, RECON_BUF_STRIDE, 0x0);
    break;

  case LEFT_DC_PRED: // 9
    S8LDD(xr1, src_left, 0x0, ptn0);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn1);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn2);
    S8LDI(xr1, src_left, RECON_BUF_STRIDE, ptn3);
    //avg
    D8SUMC(xr2, xr1, xr1);
    Q16SLR(xr8, xr2, xr0, xr0, 0x2);
    S32SFL(xr0, xr8, xr8, xr1, ptn1);
    break;

  case TOP_DC_PRED: // 10
    //load
    S32LDD(xr1, top, 0x0); // xr1[31:24] <- src_top[0] ;
    //avg
    D8SUMC(xr2, xr1, xr1);
    Q16SLR(xr8, xr2, xr0, xr0, 0x2);
    S32SFL(xr0, xr8, xr8, xr1, ptn1);
    break;

  case DC_128_PRED: // 11
    // load
    S32LUI(xr1, 0x80, ptn7);
    break;

  default:
    break;

  }

  if ( ( pred_mode == VERT_PRED ) ||
       ( pred_mode == DC_PRED ) ||
       ( pred_mode == LEFT_DC_PRED ) ||
       ( pred_mode == TOP_DC_PRED ) ||
       ( pred_mode == DC_128_PRED ) ) {
    //store
    S32STD(xr1, dst, 0x0);
    S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
    S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
  }

}

void load_left_16x16_mxu(uint8_t * src_left)
{
  S8LDD(xr7, src_left, 0x0, ptn0);
  S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn1);
  S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn2);
  S8LDI(xr7, src_left, RECON_BUF_STRIDE, ptn3);

  S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn0);
  S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn1);
  S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn2);
  S8LDI(xr8, src_left, RECON_BUF_STRIDE, ptn3);

  S8LDI(xr9, src_left, RECON_BUF_STRIDE, ptn0);
  S8LDI(xr9, src_left, RECON_BUF_STRIDE, ptn1);
  S8LDI(xr9, src_left, RECON_BUF_STRIDE, ptn2);
  S8LDI(xr9, src_left, RECON_BUF_STRIDE, ptn3);

  S8LDI(xr10, src_left, RECON_BUF_STRIDE, ptn0);
  S8LDI(xr10, src_left, RECON_BUF_STRIDE, ptn1);
  S8LDI(xr10, src_left, RECON_BUF_STRIDE, ptn2);
  S8LDI(xr10, src_left, RECON_BUF_STRIDE, ptn3);

}

void Intra_pred_luma_16x16(int luma_16x16_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *top)
{
  uint8_t *src_top;  // top address
  uint8_t *src_left;  // left address
  uint8_t *src_topleft;  // left address
  unsigned int i;

  int pred_mode = luma_16x16_pred_mode;

  src_left = src - 1;

  switch (pred_mode) {

  case DC_PRED8x8:
    src_top = top;
    // load top
    S32LDD(xr11, src_top, 0x0);
    S32LDD(xr12, src_top, 0x4);
    S32LDD(xr13, src_top, 0x8);
    S32LDD(xr14, src_top, 0xc);
    // load left
    load_left_16x16_mxu(src_left);
    // AVG
    D8SUMC(xr1, xr11, xr12);
    D8SUMC(xr2, xr13, xr14);
    D8SUMC(xr3, xr7, xr8);
    D8SUMC(xr4, xr9, xr10);
    Q16ADD_AA_WW(xr5, xr1, xr2, xr0);
    Q16ACC_AA(xr5, xr3, xr4, xr0);
    Q16ADD_AA_XW(xr7, xr5, xr5, xr0);
    D32SLR(xr8, xr7, xr0, xr0, 0x5);
    S32SFL(xr9, xr8, xr8, xr0, ptn0);
    S32SFL(xr0, xr9, xr9, xr1, ptn3);
    break;

  case VERT_PRED8x8:
    src_top = top;
    //load
    S32LDD(xr1, src_top, 0x0);
    S32LDD(xr2, src_top, 0x4);
    S32LDD(xr3, src_top, 0x8);
    S32LDD(xr4, src_top, 0xc);
    //store
    dst -= RECON_BUF_STRIDE;
    for (i=0; i<16; i++) {
      S32SDIV(xr1, dst, RECON_BUF_STRIDE,0x0);
      S32STD(xr2, dst, 0x4);
      S32STD(xr3, dst, 0x8);
      S32STD(xr4, dst, 0xc);
    }
    break;

  case HOR_PRED8x8:
    src_left = src - 0x1;
    dst -= RECON_BUF_STRIDE;
    for (i=0; i<16; i++) {
      S8LDD(xr1, src_left, 0x0, ptn7);
      src_left = src_left + RECON_BUF_STRIDE;
      S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
      S32STD(xr1, dst, 0x4);
      S32STD(xr1, dst, 0x8);
      S32STD(xr1, dst, 0xc);
    }
    break;

  case PLANE_PRED8x8:
    src_top = top;
    src_topleft = src_top - 4;
    src_left = src - 0x4;

    //----- H, LOAD -----
    S32LDD(xr1, src_top, -4);  // xr1 <- src_top[-4];  xr1: lt, 0, 0, 0 ;
    S32LDD(xr5, src_top, 0x0);   // xr5 <- src_top[0] ;  xr5: t3, t2, t1, t0 ;
    S32LDD(xr2, src_top, 0x4);   // xr2 <- src_top[4] ;  xr2: t7, t6, t5, t4 ;
    S32LDDR(xr3, src_top, 0x8);  // xr3 <- src_top[8] ;  xr3: t8, t9, t10, t11 ;
    S32LDDR(xr4, src_top, 0xc);  // xr4 <- src_top[12];  xr4: t12, t13, t14, t15 ;
    S32ALNI(xr1, xr5, xr1, ptn1);  //                    xr1: t2, t1, t0, lt ;
    S32ALNI(xr2, xr2, xr5, ptn1);  //                    xr2: t6, t5, t4, t3 ;   ---xr5 is free to use ;
    S32I2M(xr9, MUL_12);  // xr9 : 0x00010002 ;
    S32I2M(xr10, MUL_34); // xr10: 0x00030004 ;

    //----- H, SUM -----
    Q8ADDE_SS(xr5, xr3, xr2, xr6);  // xr5[31:16] <- t8-t6 ;  xr5[15:0] <- t9-t5 ;
    // xr6[31:16] <- t10-t4;  xr6[15:0] <- t11-t3;

    S32I2M(xr11, MUL_56); // xr11: 0x00050006 ;

    D16MUL_WW(xr13, xr9, xr5, xr14);     // xr13 <- 1*(t8-t6) ;  xr14 <- 2*(t9-t5) ;
    D16MAC_AA_WW(xr13, xr10, xr6, xr14); // xr13 <- 1*(t8-t6)+3*(t10-t4) ; xr14 <- 2*(t9-t5)+4*(t11-t3) ;
    Q8ADDE_SS(xr5, xr4, xr1, xr6);  // xr5[31:16] <- t12-t2;  xr5[15:0] <- t13-t1;
    // xr6[31:16] <- t14-t0;  xr6[15:0] <- t15-lt;

    S32I2M(xr12, MUL_78); // xr12: 0x00070008 ;

    D16MAC_AA_WW(xr13, xr11, xr5, xr14); // xr13 <- 1*(t8-t6)+3*(t10-t4)+5*(t12-t2) ;
    // xr14 <- 2*(t9-t5)+4*(t11-t3)+6*(t13-t1) ;
    D16MAC_AA_WW(xr13, xr12, xr6, xr14); // xr13 <- 1*(t8-t6)+3*(t10-t4)+5*(t12-t2)+7*(t14-t0) ;
    // xr14 <- 2*(t9-t5)+4*(t11-t3)+6*(t13-t1)+8*(t15-lt) ;
    S32LDD(xr1, src_topleft, 0x0);          // xr1[31:24] <- src_topleft[3] (lt) ;
    S32LDD(xr2, src_left, 0x0); // xr2[31:24] <- src_topleft[stride+3] (l0) ;
    D32ADD_AA(xr15, xr13, xr14, xr0); // xr15 <- 1*(t8-t6)+3*(t10-t4)+5*(t12-t2)+7*(t14-t0)
    //       + 2*(t9-t5)+4*(t11-t3)+6*(t13-t1)+8*(t15-lt) ;
    //----- V, LOAD -----
    //  S32LDD(xr1, src_topleft, 0x0);          // xr1[31:24] <- src_topleft[3] (lt) ;
    //  S32LDIV(xr2, src_topleft, stride, 0x0); // xr2[31:24] <- src_topleft[stride+3] (l0) ;
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0); // xr3[31:24] <- src_topleft[2*stride+3] (l1) ;
    S32LDIV(xr8, src_left, RECON_BUF_STRIDE, 0x0); // xr9[31:24] <- src_topleft[3*stride+3] (l2) ;
    S32SFL(xr5, xr2, xr1, xr0, ptn2);       // xr5[31:16] <- l0, lt ;
    S32SFL(xr6, xr8, xr3, xr0, ptn2);       // xr8[31:16] <- l2, l1 ;
    S32SFL(xr7, xr6, xr5, xr0, ptn3);       // xr7[31: 0] <- l2, l1, l0, lt ;

    S32LDIV(xr1, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr2, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr8, src_left, RECON_BUF_STRIDE, 0x0);
    S32SFL(xr5, xr2, xr1, xr0, ptn2);
    S32SFL(xr6, xr8, xr3, xr0, ptn2);
    S32SFL(xr13, xr6, xr5, xr0, ptn3); // xr13[31:0] <- l6, l5, l4, l3 ;

    src_left += RECON_BUF_STRIDE;

    S32LDIV(xr8, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr2, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr1, src_left, RECON_BUF_STRIDE, 0x0);
    S32SFL(xr6, xr8, xr3, xr0, ptn2);
    S32SFL(xr5, xr2, xr1, xr0, ptn2);
    S32SFL(xr14, xr6, xr5, xr0, ptn3); // xr14[31:0] <- l8, l9, l10, l11 ;

    S32LDIV(xr8, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr3, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr2, src_left, RECON_BUF_STRIDE, 0x0);
    S32LDIV(xr1, src_left, RECON_BUF_STRIDE, 0x0);
    S32SFL(xr6, xr8, xr3, xr0, ptn2);
    S32SFL(xr5, xr2, xr1, xr0, ptn2);
    S32SFL(xr1, xr6, xr5, xr0, ptn3); // xr1[31: 0] <- l12, l13, l14, l15 ;

    //----- V, SUM -----
    Q8ADDE_SS(xr5, xr14, xr13, xr6);
    Q8ADDE_SS(xr2, xr1, xr7, xr3);

    D16MUL_WW(xr13, xr9, xr5, xr14);
    D16MAC_AA_WW(xr13, xr10, xr6, xr14);

    D16MAC_AA_WW(xr13, xr11, xr2, xr14);
    D16MAC_AA_WW(xr13, xr12, xr3, xr14);

    D32SLR(xr2, xr11, xr12, xr3, 0x8); // xr2: 0x00000500 ;  xr3: 0x00000700 ;
    D32SLR(xr11, xr2, xr3, xr12, 0x8); //xr11: 0x00000005 ; xr12: 0x00000007 ;

    D32ADD_AA(xr14, xr13, xr14, xr0); // xr14 <- 1*(l8-l6)+3*(l10-l4)+5*(l12-l2)+7*(l14-l0)
    //       + 2*(l9-l5)+4*(l11-l3)+6*(l13-l1)+8*(l15-lt) ;
    //----- P, CAL -----
    //  D32SLR(xr2, xr11, xr12, xr3, 0x8); // xr2: 0x00000500 ;  xr3: 0x00000700 ;
    //  D32SLR(xr11, xr2, xr3, xr12, 0x8); //xr11: 0x00000005 ; xr12: 0x00000007 ;

    D16MUL_WW(xr0, xr15, xr11, xr2); // xr2: 5*H ;
    D16MUL_WW(xr0, xr14, xr11, xr3); // xr3: 5*V ;

    D32SLR(xr8, xr11, xr0, xr0, 0x2); // xr8: 0x00000001 ;
    D32SLL(xr13, xr8, xr0, xr0, 0x5); //xr13: 0x00000020 ;

    Q8ACCE_AA(xr0, xr1, xr4, xr8);   // xr8[15:0]: src1[0] + src2[16] + 1

    D32ADD_AA(xr5, xr2, xr13, xr0); // xr5: 5*H+32 ;
    D32ADD_AA(xr6, xr3, xr13, xr0); // xr6: 5*V+32 ;

    D32SLR(xr2, xr5, xr6, xr3, 0x6); // xr2: ( 5*H+32 ) >> 6 ;  xr3: ( 5*V+32 ) >> 6 ;

    //  Q8ACCE_AA(xr0, xr1, xr4, xr8);   // xr8[15:0]: src1[0] + src2[16] + 1
    D32SLL(xr5, xr8, xr0, xr0, 0x4); // xr5[15:0]: 16*(src1[0] + src2[16] + 1)

    Q16ADD_AA_WW(xr7, xr2, xr3, xr0); // xr7: V+H
    //  S32NOR(xr0, xr0, xr0); // idle
    S32I2M(xr4, MUX_H16); // xr4: 0x0000ffff ;
    D16MUL_WW(xr0, xr7, xr12, xr8);   // xr8: 7*(V+H)

    S32SFL(xr0, xr3, xr3, xr14, ptn3); // xr14[31:16]: V ;  xr14[15:0]: V ;
    D32SLL(xr7, xr2, xr0, xr0, 0x1);

    Q16ADD_SS_WW(xr9, xr5, xr8, xr0); // xr9: 16*(src1[0] + src2[16] + 1) - 7*(V+H)
    S32SFL(xr0, xr9, xr9, xr5, ptn3); // xr5[31:16]: a ;  xr5[15:0]: a ;
    //  S32SFL(xr0, xr3, xr3, xr14, ptn3); // xr14[31:16]: V ;  xr14[15:0]: V ;
    //  D32SLL(xr7, xr2, xr0, xr0, 0x1);
    S32SFL(xr0, xr7, xr7, xr8, ptn3);  // xr8[31:16]: 2H ;  xr8[15:0]: 2H ;

    S32AND(xr2, xr4, xr2);

    Q16ADD_AA_WW(xr15, xr5, xr2, xr0); // xr15[31:16]: a ;  xr15[15:0]: a + H ;

    dst -= RECON_BUF_STRIDE;
    //----- SRC, STORE -----
    for (i=0; i<16; i++) {
      Q16ADD_AA_WW(xr1, xr15, xr8, xr0);
      Q16ADD_AA_WW(xr2, xr1, xr8, xr0);
      Q16SAR(xr9, xr15, xr1, xr1, 0x5);
      Q16ADD_AA_WW(xr3, xr2, xr8, xr0);
      Q16SAT(xr10, xr9, xr1);
      Q16ADD_AA_WW(xr4, xr3, xr8, xr0);
      Q16SAR(xr2, xr2, xr3, xr3, 0x5);
      Q16ADD_AA_WW(xr5, xr4, xr8, xr0);
      Q16SAT(xr11, xr2, xr3);
      Q16ADD_AA_WW(xr6, xr5, xr8, xr0);
      Q16SAR(xr4, xr4, xr5, xr5, 0x5);
      Q16ADD_AA_WW(xr7, xr6, xr8, xr0);
      Q16SAR(xr6, xr6, xr7, xr7, 0x5);
      Q16SAT(xr12, xr4, xr5);
      Q16SAT(xr13, xr6, xr7);

      S32SDIVR(xr10, dst, RECON_BUF_STRIDE, 0x0);
      S32STDR(xr11, dst, 0x4);
      S32STDR(xr12, dst, 0x8);
      //    S32STDR(xr13, dst, 0xc);

      Q16ADD_AA_WW(xr15, xr15, xr14, xr0);

      S32STDR(xr13, dst, 0xc);
    }
    break;

  case LEFT_DC_PRED8x8:
    // load left 
    load_left_16x16_mxu(src_left);
    // AVG
    D8SUMC(xr1, xr7, xr8);
    D8SUMC(xr2, xr9, xr10);
    Q16ADD_AA_WW(xr5, xr1, xr2, xr0);
    Q16ADD_AA_XW(xr7, xr5, xr5, xr0);
    D32SLR(xr8, xr7, xr0, xr0, 0x4);
    S32SFL(xr9, xr8, xr8, xr0, ptn0);
    S32SFL(xr0, xr9, xr9, xr1, ptn3);
    break;

  case TOP_DC_PRED8x8:
    src_top = top;
    // load top
    S32LDD(xr1, src_top, 0x0);
    S32LDD(xr2, src_top, 0x4);
    S32LDD(xr3, src_top, 0x8);
    S32LDD(xr4, src_top, 0xc);
    // AVG
    D8SUMC(xr1, xr1, xr2);
    D8SUMC(xr2, xr3, xr4);
    Q16ADD_AA_WW(xr5, xr1, xr2, xr0);
    Q16ADD_AA_XW(xr7, xr5, xr5, xr0);
    D32SLR(xr8, xr7, xr0, xr0, 0x4);
    S32SFL(xr9, xr8, xr8, xr0, ptn0);
    S32SFL(xr0, xr9, xr9, xr1, ptn3);
    break;

  case DC_128_PRED8x8:
    //load
    S32LUI(xr1, 0x80, ptn7);
    break;

  default:
    break;

  }

  if ( ( pred_mode == DC_PRED8x8 ) ||
       ( pred_mode == LEFT_DC_PRED8x8 ) ||
       ( pred_mode == TOP_DC_PRED8x8 ) ||
       ( pred_mode == DC_128_PRED8x8 ) ) {

    // store
    dst -= RECON_BUF_STRIDE;
    for(i=0; i<16; i++){
      S32SDIV(xr1, dst, RECON_BUF_STRIDE, 0x0);
      S32STD(xr1, dst, 0x4);
      S32STD(xr1, dst, 0x8);
      S32STD(xr1, dst, 0xc);
    }
  }

}

#define SRC(x,y) src[(x)+(y)*stride]

static void predict_8x8_load_left(uint8_t *l, int has_topleft, /*int has_topright,*/ uint8_t *src, uint8_t *topleft, int stride )
{
  
  S32LDD(xr1, src, -4);                      // xr1[24-31] <- l0
  
  if(has_topleft)
    {
      uint8_t * topleft_minus_3 = topleft-3;
      S32LDD(xr1, topleft_minus_3, 0);         //xr1[24-31] <- lt
    }
  S8LDI(xr1, src, -1, ptn2);                 //xr1[16-23] <- l0
  src += stride;
  S8LDD(xr1, src, 0, ptn1);   //xr1[8-15] <- l1
  src += stride;
  S8LDD(xr1, src, 0, ptn0);   //xr1[0-7] <- l2
  src += stride;

  S8LDD(xr2, src, 0, ptn3);   //xr2[24-31] <- l3
  src += stride;
  S8LDD(xr2, src, 0, ptn2);   //xr2[16-23] <- l4
  src += stride;
  S8LDD(xr2, src, 0, ptn1);   //xr2[8-15] <- l5
  src += stride;
  S8LDD(xr2, src, 0, ptn0);   //xr2[0-7] <- l6
  src += stride;

  S8LDD(xr3, src, 0, ptn7);   //xr3: l7, l7, l7, l7
  S32ALNI(xr4, xr1, xr2, ptn2);  //xr4: l1, l2, l3, l4
  S32ALNI(xr5, xr1, xr2, ptn1);  //xr5: l0, l1, l2, l3
  
  Q8AVG(xr6, xr1, xr4);  //
  Q8AVGR(xr7, xr6, xr5);
  
  S32ALNI(xr4, xr2, xr3, ptn2);  //xr4: l5, l6, l7, l7
  S32ALNI(xr5, xr2, xr3, ptn1);  //xr5: l4, l5, l6, l7
  
  Q8AVG(xr6, xr2, xr4);
  Q8AVGR(xr8, xr6, xr5);
  
  // store
  S32STDR(xr7, l, 0);
  S32STDR(xr8, l, 4);
}



static void predict_8x8_load_top(uint8_t *t, int has_topleft, int has_topright, uint8_t *src, uint8_t *top, uint8_t *topleft )
{
  
  S32LDDR(xr2, top, 0);                //xr2: t0, t1, t2, t3
  S8LDD(xr1, top, 0, ptn0);           //xr1[0-7]: t0
  if(has_topleft)
    S8LDD(xr1, topleft, 0, ptn0);                 //lt
  S32LDDR(xr3, top, 4);                //xr3: t4, t5, t6, t7
  S8LDD(xr4, top, 7, ptn7);           //xr4: t7, t7, t7, t7
  if(has_topright)
    S8LDD(xr4, top, 8, ptn7);         //xr4: t8, t8, t8, t8
  
  S32ALNI(xr6, xr1, xr2, ptn3);       //xr6: lt, t0, t1, t2
  S32ALNI(xr7, xr2, xr3, ptn1);       //xr7: t1, t2, t3, t4
  Q8AVG(xr8, xr6, xr7);               //
  Q8AVGR(xr9, xr2, xr8);

  S32ALNI(xr5, xr2, xr3, ptn3);       //xr5: t3, t4, t5, t6
  S32ALNI(xr6, xr3, xr4, ptn1);       //xr6: t5, t6, t7, t8
  Q8AVG(xr7, xr5, xr6);
  Q8AVGR(xr8, xr3, xr7);

  //store
  S32STDR(xr9, t, 0);
  S32STDR(xr8, t, 4);
}

static void predict_8x8_load_topleft(uint8_t *lt, uint8_t *src, uint8_t *top, uint8_t *topleft )
{
  
  S8LDD(xr1, src, -1, ptn3);
  S8LDD(xr2, topleft, 0, ptn3);
  S8LDD(xr3, top, 0, ptn3);
  Q8AVG(xr4, xr1, xr3);
  Q8AVGR(xr5, xr4, xr2);
  S8STD(xr5, lt, 0, ptn3);
}

static void predict_8x8_load_topright( uint8_t *t, int has_topright, uint8_t *top )
{
  if(has_topright) {
    //load
    S32LDDR(xr2, top, 8);           //xr2: t8, t9, t10, t11
    S8LDD(xr1, top, 7, ptn7);       //xr1: t7, t7, t7, t7
      
    S32LDDR(xr3, top, 12);          //xr3: t12, t13, t14, t15
    S8LDD(xr4, top, 15, ptn7);      //xr4: t15, t15, t15, t15
      
    S32ALNI(xr6, xr2, xr3, ptn1);   //xr6: t9, t10, t11, t12
    S32ALNI(xr7, xr1, xr2, ptn3);   //xr7: t7, t8, t9, t10
    Q8AVG(xr8, xr6, xr7);
    Q8AVGR(xr9, xr2, xr8);

    S32ALNI(xr5, xr2, xr3, ptn3);   //xr5: t11, t12, t13, t14
    S32ALNI(xr6, xr3, xr4, ptn1);   //xr6: t13, t14, t15, t15
    Q8AVG(xr7, xr5, xr6);
    Q8AVGR(xr8, xr3, xr7);
  }
  else {
    S8LDD(xr8, top, 7, ptn7);
    S8LDD(xr9, top, 7, ptn7);
  }
  S32STDR(xr9, t, 8);
  S32STDR(xr8, t, 12);
}

static uint32_t predict_8x8_dc( uint8_t *src)
{
  S32LDD(xr2, src, 0);
  S32LDD(xr3, src, 4);
  Q8SAD(xr5, xr2, xr0, xr0);
  Q8SAD(xr0, xr3, xr0, xr5);
  S32I2M(xr7,4);
  D32ADD_AA(xr6, xr5, xr7, xr0);
  D32SAR(xr6, xr6, xr0, xr0, 3);
  S32SFL(xr0, xr6, xr6, xr5, ptn1);
  S32SFL(xr0, xr5, xr5, xr1, ptn2);
  return S32M2I(xr1);
}


// The group number of each pred mode, one group means assigning same value to the elements after prediction.
static uint8_t group_num[6] = {15, 15, 22, 22, 22, 22};

// The element count of each group.
static uint8_t group_elements[6][22] = 
  {
    { 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1 },
    { 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1 },
    { 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1 },
    { 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1 }
  };

static uint8_t elements_start_points[6][24] =
  {
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0, 0, 0, 0, 0, 0, 0, 1, -1 },
    { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
    { 0x06, 0x07, 0x04, 0x05, 0x02, 0x03, 0x01, 0x00, 0x11, 0x10, 0x21, 0x20, 0x31, 0x30, 0x41, 0x40, 0x51, 0x50, 0x61, 0x60, 0x71, 0x70, 1, 2 },
    { 0x07, 0x17, 0x06, 0x16, 0x05, 0x15, 0x04, 0x14, 0x03, 0x13, 0x02, 0x12, 0x01, 0x11, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 2, 1 },
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x16, 0x17, 0x26, 0x27, 0x36, 0x37, 0x46, 0x47, 0x56, 0x57, 0x66, 0x67, 0x76, 0x77, 1, -2 },
    { 0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 2, -1 }
  };


// Indicates the calculation method of reference value, whose result is assigned to the elements of each group.
// bit 1: summing directon . 0 -- forward; 1 -- backward.
//
// bits 2 -- 3: summing mode:
//        0 -- ( A + 3*B + 2 ) >> 2
//        1 -- A
//        2 -- ( A + B + 1 ) >> 1
//        3 -- ( A + 2*B + C + 2 ) >> 2
//
// bits 4 -- 8: index of reference value, the reference value table looks like this:
//
//     ref_val: | l7 | l6 | l5 | l4 | l3 | l2 | l1 | l0 | lt | t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7 | t8 
//     --------------------------------------------------------------------------------------------------
//       index: |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17
//
//              | t9 | t10| t11| t12| t13| t14| t15
//              -----------------------------------
//              | 18 | 19 | 20 | 21 | 22 | 23 | 24
//

static uint8_t ref_val_flags[6][22] =
  {
    { 
      (3<<5)|9 , (3<<5)|10, (3<<5)|11, (3<<5)|12, (3<<5)|13, (3<<5)|14, (3<<5)|15, (3<<5)|16, (3<<5)|17,
      (3<<5)|18, (3<<5)|19, (3<<5)|20, (3<<5)|21, (3<<5)|22, (0<<5)|23, 0, 0, 0, 0, 0, 0, 0
    },
    {
      (3<<5)|0 , (3<<5)|1 , (3<<5)|2 , (3<<5)|3 , (3<<5)|4 , (3<<5)|5 , (3<<5)|6 , (3<<5)|7 , (3<<5)|8 ,
      (3<<5)|9 , (3<<5)|10, (3<<5)|11, (3<<5)|12, (3<<5)|13, (3<<5)|14, 0, 0, 0, 0, 0, 0, 0
    },
    {
      (3<<5)|2 , (3<<5)|1 , (3<<5)|4 , (3<<5)|3 , (3<<5)|6 , (3<<5)|5 , 
      (3<<5)|7 , (2<<5)|8 , (3<<5)|8 , (2<<5)|9 , (3<<5)|9 , (2<<5)|10,
      (3<<5)|10, (2<<5)|11, (3<<5)|11, (2<<5)|12, (3<<5)|12, (2<<5)|13,
      (3<<5)|13, (2<<5)|14, (3<<5)|14, (2<<5)|15
    },
    {
      (1<<7)|(2<<5)|1 , (1<<7)|(3<<5)|2 , (1<<7)|(2<<5)|2 , 
      (1<<7)|(3<<5)|3 , (1<<7)|(2<<5)|3 , (1<<7)|(3<<5)|4 , 
      (1<<7)|(2<<5)|4 , (1<<7)|(3<<5)|5 , (1<<7)|(2<<5)|5 , 
      (1<<7)|(3<<5)|6 , (1<<7)|(2<<5)|6 , (1<<7)|(3<<5)|7 , 
      (1<<7)|(2<<5)|7 , (1<<7)|(3<<5)|8 , (1<<7)|(2<<5)|8 , 
      (1<<7)|(3<<5)|9 , (1<<7)|(3<<5)|10, (1<<7)|(3<<5)|11, 
      (1<<7)|(3<<5)|12, (1<<7)|(3<<5)|13, (1<<7)|(3<<5)|14, 
      (1<<7)|(3<<5)|15 
    },
    {
      (2<<5)|9 , (3<<5)|9 , (2<<5)|10, (3<<5)|10, (2<<5)|11, (3<<5)|11, 
      (2<<5)|12, (3<<5)|12, (2<<5)|13, (3<<5)|13, (2<<5)|14, (3<<5)|14, 
      (2<<5)|15, (3<<5)|15, (2<<5)|16, (3<<5)|16, (2<<5)|17, (3<<5)|17, 
      (2<<5)|18, (3<<5)|18, (2<<5)|19, (3<<5)|19
    },
    {
      (1<<7)|(2<<5)|7 , (1<<7)|(3<<5)|7 , (1<<7)|(2<<5)|6 , (1<<7)|(3<<5)|6 , 
      (1<<7)|(2<<5)|5 , (1<<7)|(3<<5)|5 , (1<<7)|(2<<5)|4 , (1<<7)|(3<<5)|4 , 
      (1<<7)|(2<<5)|3 , (1<<7)|(3<<5)|3 , (1<<7)|(2<<5)|2 , (1<<7)|(3<<5)|2 , 
      (1<<7)|(2<<5)|1 , (1<<7)|(0<<5)|1 , (1<<5)|0 ,        (1<<5)|0 , 
      (1<<5)|0 ,        (1<<5)|0,         (1<<5)|0 ,        (1<<5)|0 , 
      (1<<5)|0 ,        (1<<5)|0
    }
  };


uint8_t t[16];
uint8_t refs[25];

void Intra_pred_luma_8x8l(int luma_8x8l_pred_mode, uint8_t *dst, int has_topleft, int has_topright, uint8_t *src, uint8_t *top, uint8_t *topleft, int stride)
{
  uint8_t ref_vals[22];
  int i,j;
  //void *ret_addr;
  
  uint32_t dc = 0;


  if(luma_8x8l_pred_mode >= DIAG_DOWN_LEFT_PRED && luma_8x8l_pred_mode < LEFT_DC_PRED)
    goto bias_pred;
  if(luma_8x8l_pred_mode == DC_128_PRED) {
    dc = 0x80808080;
    goto predict8x8_dc;
  }
  if(luma_8x8l_pred_mode == HOR_PRED) {
    
    predict_8x8_load_left(refs, has_topleft, src, topleft, stride);

    uint8_t *ll = refs-1;
        
    for( i = 0; i < 8; i++)
      {
	S8LDI(xr1, ll, 1, ptn7);
	S32STD(xr1, dst, 0);
	S32STD(xr1, dst, 4);
	dst += stride;
      }
    
    return;
  }

  if(luma_8x8l_pred_mode == VERT_PRED) {
    predict_8x8_load_top(t, has_topleft, has_topright, src, top, topleft);

    /*for( i = 0; i < 8; i++ )
      dst[i] = t[i];
      for( i = 1; i < 8; i++ )
      *(uint64_t*)(dst+i*stride) = *(uint64_t*)dst;*/
    S32LDD(xr1, t, 0);
    S32LDD(xr2, t, 4);
    
    for( i = 0; i < 8; i++ ) {
      S32STD(xr1, dst, 0);
      S32STD(xr2, dst, 4);
      dst += stride;
    }

    return;
  }
  if(  luma_8x8l_pred_mode == LEFT_DC_PRED ){
    
    predict_8x8_load_left(refs, has_topleft, src, topleft, stride);


    /*for( i = 0; i < 8; i++) {
      dc += refs[i];
      }

      dc = ((dc+4)>>3) * 0x01010101;*/
    
    dc = predict_8x8_dc(refs);
  
  }
  else if( /*luma_8x8l_pred_mode == DC_PRED ||*/ luma_8x8l_pred_mode == TOP_DC_PRED ){
    //__asm__ __volatile__("jal load_top\n\t":::"$31");
    predict_8x8_load_top(t, has_topleft, has_topright, src, top, topleft);


    /*uint32_t dc_tmp = 0;
      for( i = 0; i < 8; i++) {
      dc_tmp += t[i];
      }

      dc = ((dc_tmp+4)>>3) * 0x01010101;*/
    
    dc = predict_8x8_dc(t);
  }
  else if(luma_8x8l_pred_mode == DC_PRED)
    {
      //__asm__ __volatile__("jal load_left\n\t":::"$31");
      predict_8x8_load_left(refs, has_topleft, src, topleft, stride);
      predict_8x8_load_top(t, has_topleft, has_topright, src, top, topleft);
      /*for( i = 0; i < 8; i++) {
        dc += refs[i]+t[i];
	}
	dc = ((dc+8)>>4) * 0x01010101;*/
      S32LDD(xr2, refs, 0);
      S32LDD(xr3, refs, 4);
      Q8SAD(xr5, xr2, xr0, xr0);
      Q8SAD(xr0, xr3, xr0, xr5);
      S32LDD(xr2, t, 0);
      S32LDD(xr3, t, 4);
      Q8SAD(xr0, xr2, xr0, xr5);
      Q8SAD(xr0, xr3, xr0, xr5);
      S32I2M(xr7,8);
      D32ADD_AA(xr6, xr5, xr7, xr0);
      D32SAR(xr6, xr6, xr0, xr0, 4);
      S32SFL(xr0, xr6, xr6, xr5, ptn1);
      S32SFL(xr0, xr5, xr5, xr1, ptn2);
      dc = S32M2I(xr1);
    }

 predict8x8_dc:
  
  for( i = 0; i < 8; i++ ) {
    ((uint32_t*)dst)[0] =
      ((uint32_t*)dst)[1] = dc;
    dst += stride;
  }
  return;

 bias_pred:
    
  
  switch(luma_8x8l_pred_mode) {
  case DIAG_DOWN_LEFT_PRED:
  case VERT_LEFT_PRED:
    
    predict_8x8_load_top( t, has_topleft, has_topright, src, top, topleft );
    predict_8x8_load_topright( t, has_topright, top );
    
    memcpy( &refs[9], t, 16);
    
    break;
  case DIAG_DOWN_RIGHT_PRED:
  case VERT_RIGHT_PRED:
  case HOR_DOWN_PRED:
    
    predict_8x8_load_top( t, has_topleft, has_topright, src, top, topleft );
    predict_8x8_load_topleft( &refs[8], src, top, topleft );
    memcpy( &refs[9], t, 8);
  case HOR_UP_PRED:
    
    predict_8x8_load_left(refs, has_topleft, src, topleft, stride);
    S32LDD(xr1, refs, 0);
    S32LDD(xr2, refs, 4);
    S32STDR(xr2, refs, 0);
    S32STDR(xr1, refs, 4);
    
    break;
  }
  
  
  int func_index = luma_8x8l_pred_mode - 3;

  for( i = 0; i < group_num[func_index]; i++ )
    {
      int diff = 1-((ref_val_flags[func_index][i]>>7)<<1);
      uint8_t ref_count = (ref_val_flags[func_index][i]>>5)&3;
      uint8_t ref_index = ref_val_flags[func_index][i] & 31;
      int ref_val = refs[ref_index];
      switch(ref_count)
	{
	case 3:
	  ref_val += 2*refs[ref_index+diff]+refs[ref_index+2*diff]+2;
	  ref_val >>= 2;
	  break;
	case 2:
	  ref_val += refs[ref_index+diff]+1;
	  ref_val >>= 1;
	  break;
	case 1:
	  break;
	case 0:
	  ref_val += 3*refs[ref_index+diff]+2;
	  ref_val >>= 2;
	  break;
	default:
	  break;
	}
      ref_vals[i] = ref_val;
    }
	
  for( i = 0; i < group_num[func_index]; i++ )
    {
      int x = elements_start_points[func_index][i] >> 4;
      int y = elements_start_points[func_index][i] & 15;
      int diff_x = ((char)elements_start_points[func_index][22]);
      int diff_y = ((char)elements_start_points[func_index][23]);
      for( j = 0; j < group_elements[func_index][i]; j++ )
	{
	  dst[x+y*stride] = ref_vals[i];
	  x += diff_x;
	  y += diff_y;
	}
    }
}

#undef SRC
