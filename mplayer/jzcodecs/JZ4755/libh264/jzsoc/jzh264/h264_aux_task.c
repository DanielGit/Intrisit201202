/*****************************************************************************
 *
 * JZC H264 decoder AUX-CPU task
 *
 *
 ****************************************************************************/
#include "h264_jzmacro.h"
#include "h264_aux_type.h"
#include "h264_tcsm.h"
#include "h264_aux.h"
#include "h264_mc_hw.h"
#include "h264_aux_intra.h"
#include "h264_aux_mc.h"
#include "h264_aux_mdma.h"
#include "../jzmedia.h"
#include "../jz4750e_dblk.h"
#include "../jz4750e_idct.h"
#include "h264_aux_pmon.h"

#define XCHG2(a,b,t)   t=a;a=b;b=t


extern int _gp;

#define i_nop  ({__asm__ __volatile__("nop\t#i_nop":::"memory");})

#define i_la(src)                                               			\
({  	unsigned long _dst_;                           					\
	__asm__ __volatile__("la\t""%0,%1""\t#i_la macro":"=r"(_dst_) :"X" (src));      \
	_dst_;})


volatile int *fifo_wp, *task_done;

static const uint8_t scan5[16]={
  1+1*5, 2+1*5, 1+2*5, 2+2*5,
  3+1*5, 4+1*5, 3+2*5, 4+2*5,
  1+3*5, 2+3*5, 1+4*5, 2+4*5,
  3+3*5, 4+3*5, 3+4*5, 4+4*5,
};

const uint16_t MB_offset_recon[24] = {
  /*Y*/
  0,                     4, RECON_BUF_STRIDE*4,   RECON_BUF_STRIDE*4+4,
  8,                    12, RECON_BUF_STRIDE*4+8, RECON_BUF_STRIDE*4+12,
  RECON_BUF_STRIDE*8,  RECON_BUF_STRIDE*8+4, RECON_BUF_STRIDE*12,  RECON_BUF_STRIDE*12+4,
  RECON_BUF_STRIDE*8+8,RECON_BUF_STRIDE*8+12,RECON_BUF_STRIDE*12+8,RECON_BUF_STRIDE*12+12,
  /*U*/
  RECON_U_OFST,                   RECON_U_OFST+4,
  RECON_U_OFST+RECON_BUF_STRIDE*4,RECON_U_OFST+RECON_BUF_STRIDE*4+4,
  /*V*/
  RECON_V_OFST,                   RECON_V_OFST+4,
  RECON_V_OFST+RECON_BUF_STRIDE*4,RECON_V_OFST+RECON_BUF_STRIDE*4+4,
};

static uint16_t intra_pred4x4_top[2][16] = {
  {(uint16_t)TCSM1_BOTTOM_Y_25PIX+4, (uint16_t)TCSM1_BOTTOM_Y_25PIX+8, 
   (uint16_t)TCSM1_RECON_YBUF0 + 3*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_BOTTOM_Y_25PIX+12, (uint16_t)TCSM1_BOTTOM_Y_25PIX+16,
   (uint16_t)TCSM1_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF0 + 7*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF0 + 11*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 12
  },
  {(uint16_t)TCSM1_BOTTOM_Y_25PIX+4, (uint16_t)TCSM1_BOTTOM_Y_25PIX+8,
   (uint16_t)TCSM1_RECON_YBUF1 + 3*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_BOTTOM_Y_25PIX+12, (uint16_t)TCSM1_BOTTOM_Y_25PIX+16,
   (uint16_t)TCSM1_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF1 + 7*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF1 + 11*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 12
  }
};
static uint16_t intra_pred4x4_left[2][16] = {
  {(uint16_t)TCSM1_RECON_YBUF1 + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF0 + 4, 
   (uint16_t)TCSM1_RECON_YBUF1 + MB_LUMA_WIDTH+4*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF0 + 8, (uint16_t)TCSM1_RECON_YBUF0 + 12,
   (uint16_t)TCSM1_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF1 + 8*RECON_BUF_STRIDE + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF1 + 12*RECON_BUF_STRIDE + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 12
  },
  {(uint16_t)TCSM1_RECON_YBUF0 + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF1 + 4, 
   (uint16_t)TCSM1_RECON_YBUF0 + MB_LUMA_WIDTH+4*RECON_BUF_STRIDE, (uint16_t)TCSM1_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF1 + 8, (uint16_t)TCSM1_RECON_YBUF1 + 12,
   (uint16_t)TCSM1_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF0 + 8*RECON_BUF_STRIDE + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF0 + 12*RECON_BUF_STRIDE + MB_LUMA_WIDTH, (uint16_t)TCSM1_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 4,
   (uint16_t)TCSM1_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 12,
   (uint16_t)TCSM1_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 8, (uint16_t)TCSM1_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 12
  }
};

void luma_dc_dequant_idct_c(DCTELEM *block, int qmul);
static void chroma_dc_dequant_idct_c(DCTELEM *block, int qmul);
void ff_h264_idct8_dc_add_c(uint8_t *dst, DCTELEM *block, int stride);
void ff_h264_idct8_add_c   (uint8_t *dst, DCTELEM *block, int stride);
void ff_h264_idct_dc_add_c (uint8_t *dst, DCTELEM *block, int stride);
void ff_h264_idct_add_c    (uint8_t *dst, DCTELEM *block, int stride);

void filter_mb_dblk(H264_Frame_GlbARGs *dFRM, H264_MB_DecARGs * dMB_aux, H264_AUX_T *AUX_T);

__tcsm1_main int main() {

  int c;
  S32I2M(xr16,3);
  c = i_la(_gp); 
 
  int fifo_rp = 0;
  unsigned int XCHGtmp;
  H264_Frame_GlbARGs *dFRM = (H264_Frame_GlbARGs *)TCSM1_FRM_ARGS;
  H264_MB_DecARGs *dMB_aux = (H264_MB_DecARGs *)TCSM1_MBARGS_BUF;
  H264_MB_DecARGs *dMB2 = (H264_MB_DecARGs *)TCSM1_MBARGS_BUF2;
  H264_AUX_T *AUX_T = (H264_AUX_T *)TCSM1_AUX_T;
  MDMA_DesNode *MDMA1_TRAN = (MDMA_DesNode *)TCSM1_GP1_TRAN_CHAIN;
  MDMA_DesNode *MDMA1_ARG = (MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
  H264_MB_DecARGs *dMBsrc;

  fifo_wp = (int *)TCSM1_FIFO_WP;
  dMBsrc = (H264_MB_DecARGs *)(dFRM->dMB_baseaddr_aux);
  int mb_num;
  int mb_start;
  int total_mbs;    

  mb_start = dFRM->first_mb_in_frame;
  total_mbs = dFRM->mb_width * dFRM->mb_height;

  int i;

  uint16_t *intra_pred4x4_top_ptr[2];
  intra_pred4x4_top_ptr[0]=intra_pred4x4_top[0];
  intra_pred4x4_top_ptr[1]=intra_pred4x4_top[1];
  uint16_t *intra_pred4x4_left_ptr[2];
  intra_pred4x4_left_ptr[0]=intra_pred4x4_left[0];
  intra_pred4x4_left_ptr[1]=intra_pred4x4_left[1];

  uint8_t mb_x_d1, mb_y_d1;
  uint8_t mb_x_d2, mb_y_d2;
  mb_x_d1 = mb_y_d1 = mb_x_d2 = mb_y_d2 =0;

  AUX_PMON_CREAT(mc);
  AUX_PMON_CREAT(idct);
  AUX_PMON_CREAT(dblk);
  AUX_PMON_CREAT(intra);
  AUX_PMON_CREAT(mdma);
  AUX_PMON_CREAT(sync);

  do{
  }while(fifo_rp == *fifo_wp);
  fifo_rp++;

  MDMA1_ARG->TSA = get_phy_addr_aux((uint32_t)dMBsrc);
  MDMA1_ARG->TDA = (uint32_t)dMB_aux;
  MDMA1_ARG->STRD = MDMA_STRD(64, 64);
  MDMA1_ARG->UNIT = MDMA_UNIT(1,64,(sizeof(H264_MB_DecARGs)));
  SET_MDMA1_DHA((uint32_t)TCSM1_GP1_ARG_CHAIN);
  MDMA1_RUN();
  dMBsrc++;
  POLLING_MDMA1_END();

  char bakup_x0 = dFRM->mb_width-1;
  char bakup_x1 = 0;
  int mb_type_last= 0;

  volatile unsigned int *infar = (unsigned int *)TCSM1_H264_DBLK_INFAR;
  
  uint8_t non_zero_count_cache_luma_last[16];

  for ( mb_num = mb_start; (mb_num < total_mbs+2); mb_num ++ ) {

    AUX_PMON_ON(sync);

    do{
    }while(fifo_rp == *fifo_wp);
    fifo_rp++;

    int gp1_tran_start = 0;
    int gp1_tran_len   = sizeof(H264_MB_DecARGs);
    if (dMB_aux->next_mb_no_weight_flag) {
      gp1_tran_start = (53 << 2);
      gp1_tran_len -= (53 << 2);
    }
    if (dMB_aux->next_mb_skip_flag) {
      gp1_tran_len -= (192 << 2);
    }

    AUX_PMON_OFF(sync);

    AUX_PMON_ON(mdma);
    AUX_PMON_OFF(mdma);

    AUX_PMON_ON(intra);

    MDMA1_ARG->TSA = (get_phy_addr_aux((uint32_t)dMBsrc) + gp1_tran_start);
    MDMA1_ARG->TDA = ((uint32_t)dMB2 + gp1_tran_start);
    MDMA1_ARG->STRD = MDMA_STRD(64, 64);
    MDMA1_ARG->UNIT = MDMA_UNIT(1,64,gp1_tran_len);
    dMBsrc++;

    POLLING_MDMA1_END(); //ensure curr dblked MB has been trans out

    SET_MDMA1_DHA((uint32_t)TCSM1_GP1_ARG_CHAIN);
    MDMA1_RUN();

    int mb_x= dMB_aux->mb_x;
    int mb_y= dMB_aux->mb_y;
    int mb_type= dMB_aux->mb_type;

    if (dMB_aux->curr_mb_skip_flag)
      for(i=0; i<24; i++)
	dMB_aux->mb[i*16] = 0;
 
    AUX_T->mc_des_dirty=0;

    if(IS_INTRA(mb_type_last)){
      // chroma predict
      Intra_pred_chroma(dMB_aux->chroma_pred_mode, AUX_T->h264_urecon[1], 
			AUX_T->h264_urecon[0] + MB_CHROM_WIDTH, TCSM1_BOTTOM_U_13PIX+4);
      Intra_pred_chroma(dMB_aux->chroma_pred_mode, AUX_T->h264_vrecon[1], 
			AUX_T->h264_vrecon[0] + MB_CHROM_WIDTH, TCSM1_BOTTOM_V_13PIX+4);

      // luma predict
      if(IS_INTRA4x4(mb_type_last)){
	if(IS_8x8DCT(mb_type_last)){
	  for(i=0; i<16; i+=4){
	    uint8_t * src_left = (uint32_t)(intra_pred4x4_left_ptr[1][i]) | 0xF4000000;
	    uint8_t * src_top = (uint32_t)(intra_pred4x4_top_ptr[1][i]) | 0xF4000000;
	    uint8_t * src_topleft;
	    if ( i==8 )
	      src_topleft = src_left - RECON_BUF_STRIDE - 1;
	    else src_topleft = src_top - 1;

	    uint8_t * ptr= AUX_T->h264_yrecon[1] + MB_offset_recon[i];
	    int dir= dMB_aux->intra4x4_pred_mode_cache[ i ];
	    int nnz = non_zero_count_cache_luma_last[i];
	    Intra_pred_luma_8x8l(dir,ptr,(dMB_aux->topleft_samples_available<<i)&0x8000,
				 (dMB_aux->topright_samples_available<<i)&0x4000, src_left, src_top, src_topleft, RECON_BUF_STRIDE);
	    if(nnz){
	      if(nnz == 1 && dMB_aux->mb[i*16])
		ff_h264_idct8_dc_add_c(ptr, dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	      else
		ff_h264_idct8_add_c(ptr, dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	    }
	  }
	}else {
	  for(i=0; i<16; i++){
	    uint8_t * src_left = (uint32_t)(intra_pred4x4_left_ptr[1][i]) | 0xf4000000;
	    uint8_t * src_top = (uint32_t)(intra_pred4x4_top_ptr[1][i]) | 0xf4000000;

	    uint8_t * src_topleft;
	    if ( (i==2) || (i==8) || (i==10) )
	      src_topleft = src_left - RECON_BUF_STRIDE;
	    else src_topleft = src_top;

	    uint8_t * ptr= AUX_T->h264_yrecon[1] + MB_offset_recon[i];
	    uint8_t *topright;
	    int dir= dMB_aux->intra4x4_pred_mode_cache[ i ];
	    int nnz, tr;

	    if(dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED){
	      int topright_avail= (dMB_aux->topright_samples_available<<i)&0x8000;
	      if(!topright_avail){
		tr= src_top[3]*0x01010101;
		topright= (uint8_t*) &tr;
	      }else{
		topright= src_top + 4;
	      }
	    }
	    Intra_pred_luma_4x4(dir, ptr, src_left, topright, src_top, src_topleft);

	    nnz = non_zero_count_cache_luma_last[i];
	    if(nnz){
	      if(nnz == 1 && dMB_aux->mb[i*16])
		ff_h264_idct_dc_add_c(ptr, dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	      else
		ff_h264_idct_add_c(ptr, dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	    }

	  }

	}
      }else{
	Intra_pred_luma_16x16(dMB_aux->intra16x16_pred_mode, AUX_T->h264_yrecon[1], 
			      AUX_T->h264_yrecon[0] + MB_LUMA_WIDTH, TCSM1_BOTTOM_Y_25PIX+4);
	luma_dc_dequant_idct_c(dMB_aux->mb, dMB_aux->dequant4_coeff[0]);
	for(i=0; i<16; i++){
	  if(non_zero_count_cache_luma_last[i])
	    ff_h264_idct_add_c(AUX_T->h264_yrecon[1] + MB_offset_recon[i], 
			       dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	  else if(dMB_aux->mb[i*16])
	    ff_h264_idct_dc_add_c(AUX_T->h264_yrecon[1] + MB_offset_recon[i],
				  dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	}  
      }
    }

    AUX_PMON_OFF(intra);

    if(IS_INTER(mb_type)){
      hl_motion_hw(dFRM, dMB_aux, AUX_T);
    }

    AUX_PMON_ON(mc);
    MC_POLLING_END();
    AUX_PMON_OFF(mc);

    AUX_PMON_ON(dblk);
    while ((*infar)!= TCSM1_H264_DBLK_INFDA) {}
    AUX_PMON_OFF(dblk);

    if(AUX_T->mc_des_dirty){
      H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)(AUX_T->h264_mc_des_ptr[0]);
      h264_mc[AUX_T->mc_des_dirty - 1].VNodeHead = H264_VHEAD_UNLINK;
      SET_MC_DHA((uint32_t)h264_mc);
      CLEAR_MC_TTEND();
      SET_MC_DCS();
    }

    if(IS_INTRA(mb_type)){
      uint32_t * bakup_src = AUX_T->BackupMBbottom_Y[bakup_x1]-4;
      uint32_t * bakup_dst = TCSM1_BOTTOM_Y_25PIX;
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];
      bakup_dst[2] = bakup_src[2];
      bakup_dst[3] = bakup_src[3];
      bakup_dst[4] = bakup_src[4];
      bakup_dst[5] = bakup_src[5];
      bakup_dst[6] = bakup_src[6];

      bakup_src = AUX_T->BackupMBbottom_U[bakup_x1]-4;
      bakup_dst = TCSM1_BOTTOM_U_13PIX;
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];
      bakup_dst[2] = bakup_src[2];
      bakup_dst[3] = bakup_src[3];

      bakup_src = AUX_T->BackupMBbottom_V[bakup_x1]-4;
      bakup_dst = TCSM1_BOTTOM_V_13PIX;
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];
      bakup_dst[2] = bakup_src[2];
      bakup_dst[3] = bakup_src[3];
    }

    if(IS_INTER(mb_type_last))
      hl_motion_hw_next(dMB_aux,AUX_T,mb_type_last);
   
    *(uint32_t*)&AUX_T->sub_mb_type[0] = *(uint32_t*)&dMB_aux->sub_mb_type[0];
    *(uint32_t*)&AUX_T->sub_mb_type[2] = *(uint32_t*)&dMB_aux->sub_mb_type[2];

    AUX_PMON_ON(idct);
    AUX_PMON_OFF(idct);

    if (IS_INTRA_PCM(mb_type)) {
      unsigned int x, y;
      for(i=0; i<24; i++) {
	for (y=0; y<4; y++) {
	  for (x=0; x<4; x++) {
	    *(AUX_T->h264_yrecon[0] + MB_offset_recon[i] + y*RECON_BUF_STRIDE + x) = dMB_aux->mb[i*16+y*4+x];
	  }
	}
      }
    } 

    {
      int linesize, uvlinesize;
      uint8_t *dest_y_d2, *dest_u_d2, *dest_v_d2;

      linesize = dFRM->linesize;
      uvlinesize = dFRM->uvlinesize;

      dest_y_d2 = dFRM->current_picture.y_ptr + (mb_y_d2 * 16* linesize  ) + mb_x_d2 * 16;
      dest_u_d2 = dFRM->current_picture.u_ptr + (mb_y_d2 * 8 * uvlinesize) + mb_x_d2 * 8;
      dest_v_d2 = dFRM->current_picture.v_ptr + (mb_y_d2 * 8 * uvlinesize) + mb_x_d2 * 8;

      //move dblked MB out
      MDMA1_TRAN[0].TSA = AUX_T->h264_ydblk[1]-4;
      MDMA1_TRAN[0].TDA = get_phy_addr_aux((uint32_t)dest_y_d2-4);

      MDMA1_TRAN[1].TSA = AUX_T->h264_ydblk[1]-4+DBLK_U_OFST;
      MDMA1_TRAN[1].TDA = get_phy_addr_aux((uint32_t)dest_u_d2-4);
      //MDMA1_TRAN[1].UNIT = MDMA_UNIT(1,12,96);

      MDMA1_TRAN[2].TSA = AUX_T->h264_ydblk[1]-4+DBLK_V_OFST;
      MDMA1_TRAN[2].TDA = get_phy_addr_aux((uint32_t)dest_v_d2-4);

      POLLING_MDMA1_END(); //ensure next MB's args has been received

      SET_MDMA1_DHA((uint32_t)MDMA1_TRAN);
      MDMA1_RUN();
    }

    //---------idct fo inter---------------
    if(IS_INTER(mb_type_last)){
      void (*idct_dc_add)(uint8_t *dst, DCTELEM *block, int stride);
      void (*idct_add)(uint8_t *dst, DCTELEM *block, int stride);
      int di;
      if(IS_8x8DCT(mb_type_last)){
        idct_dc_add = ff_h264_idct8_dc_add_c;
        idct_add = ff_h264_idct8_add_c;
	di = 4;
      }else{
        idct_dc_add = ff_h264_idct_dc_add_c;
        idct_add = ff_h264_idct_add_c;
	di = 1;
      }

      for(i=0; i<16; i+=di){
	int nnz = non_zero_count_cache_luma_last[i];
	if(nnz){
	  if(nnz==1 && dMB_aux->mb[i*16])
	    idct_dc_add(AUX_T->h264_yrecon[1] + MB_offset_recon[i], 
			dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	  else
	    idct_add(AUX_T->h264_yrecon[1] + MB_offset_recon[i], 
		     dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	}
      }
    }

    //------------- chroma idct------------
    if(mb_type_last){
      chroma_dc_dequant_idct_c(dMB_aux->mb + 16*16, dMB_aux->dequant4_coeff[1]);
      chroma_dc_dequant_idct_c(dMB_aux->mb + 16*16+4*16, dMB_aux->dequant4_coeff[2]);
      for(i=16; i<16+8; i++){
	if(dMB_aux->non_zero_count_cache_chroma[ i - 16 ]) {
	  ff_h264_idct_add_c(AUX_T->h264_yrecon[1] + MB_offset_recon[i], 
			     dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	} else if (dMB_aux->mb[i*16]) {
	  ff_h264_idct_dc_add_c(AUX_T->h264_yrecon[1] + MB_offset_recon[i], 
				dMB_aux->mb + i*16, RECON_BUF_STRIDE);
	}
      }
    }

    if(!(mb_x==0 && mb_y==0)){
      SET_DHA_DBLK((unsigned int)AUX_T->h264_dblk_des_ptr[1]);
      *infar = 0;
      SET_DCS_DBLK(0x1);
    }

    filter_mb_dblk(dFRM, dMB_aux, AUX_T);

    {
      uint32_t * bakup_src = AUX_T->h264_yrecon[1] + 15*RECON_BUF_STRIDE;
      uint32_t * bakup_dst = AUX_T->BackupMBbottom_Y[bakup_x0];
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];
      bakup_dst[2] = bakup_src[2];
      bakup_dst[3] = bakup_src[3];

      bakup_src = AUX_T->h264_urecon[1] + 7*RECON_BUF_STRIDE;
      bakup_dst = AUX_T->BackupMBbottom_U[bakup_x0];
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];

      bakup_src = AUX_T->h264_vrecon[1] + 7*RECON_BUF_STRIDE;
      bakup_dst = AUX_T->BackupMBbottom_V[bakup_x0];
      bakup_dst[0] = bakup_src[0];
      bakup_dst[1] = bakup_src[1];
    }

    mb_x_d2 = mb_x_d1;
    mb_y_d2 = mb_y_d1;
    mb_x_d1 = mb_x;
    mb_y_d1 = mb_y;

    mb_type_last=mb_type;

    for(i=0;i<16;i++)
      non_zero_count_cache_luma_last[i]=dMB_aux->non_zero_count_cache_luma[ scan5[i] ];

    // abnormal exit
    if (dMB_aux->deblocking_filter & 0x8) break;

    bakup_x0=(bakup_x0==((dFRM->mb_width)-1))?0:(bakup_x0+1);//hit second line's tail
    bakup_x1=(bakup_x1==((dFRM->mb_width)-1))?0:(bakup_x1+1);
    XCHG2(AUX_T->h264_yrecon[0],AUX_T->h264_yrecon[1],XCHGtmp);
    XCHG2(AUX_T->h264_urecon[0],AUX_T->h264_urecon[1],XCHGtmp);
    XCHG2(AUX_T->h264_vrecon[0],AUX_T->h264_vrecon[1],XCHGtmp);
    XCHG2(AUX_T->h264_ydblk[0],AUX_T->h264_ydblk[1],XCHGtmp);
    XCHG2(AUX_T->h264_dblk_des_ptr[0],AUX_T->h264_dblk_des_ptr[1],XCHGtmp);
    XCHG2(AUX_T->h264_mc_des_ptr[0],AUX_T->h264_mc_des_ptr[1],XCHGtmp);
    XCHG2(intra_pred4x4_top_ptr[0],intra_pred4x4_top_ptr[1],XCHGtmp);
    XCHG2(intra_pred4x4_left_ptr[0],intra_pred4x4_left_ptr[1],XCHGtmp);
    XCHG2(dMB_aux,dMB2,XCHGtmp);

  }

  AUX_PMON_TRAN(mc,PMON_MC_BUF);
  AUX_PMON_TRAN(idct,PMON_IDCT_BUF);
  AUX_PMON_TRAN(dblk,PMON_DBLK_BUF);
  AUX_PMON_TRAN(intra,PMON_INTRA_BUF);
  AUX_PMON_TRAN(mdma,PMON_MDMA_BUF);
  AUX_PMON_TRAN(sync,PMON_SYNC_BUF);

  /*
    task_done: aux-cpu task done flag, only write by aux-cpu, only read by main-cpu
  */
  MDMA1_ARG->TSA = (TCSM1_TASK_DONE);
  MDMA1_ARG->TDA = (TCSM0_TASK_DONE);
  MDMA1_ARG->STRD = MDMA_STRD(4,4);
  MDMA1_ARG->UNIT = MDMA_UNIT(1,4,4);
  SET_MDMA1_DHA((uint32_t)TCSM1_GP1_ARG_CHAIN);
  MDMA1_RUN();

  i_nop;  
  i_nop;    
  i_nop;      
  i_nop;  
  __asm__ __volatile__ ("wait");

}

/*assemble reconstruction functions*/
#include "h264_aux_dblk.c"
#include "h264_aux_intra.c"
#include "h264_aux_idct.c"
#include "h264_aux_mc.c"
