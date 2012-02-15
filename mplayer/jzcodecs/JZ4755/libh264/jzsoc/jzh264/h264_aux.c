/*****************************************************************************
 *
 * JZC H264 decoder AUX-CPU task
 *
 *
 ****************************************************************************/

#include "h264_aux.h"
#include "../jz4750e_mdma.h"

#ifdef JZC_H264_DUMP
extern FILE *jzc_h264_dump_fp;
extern int jzc_dump_fcnt;
#endif // JZC_H264_DUMP

int MB_offset_recon[24] = {
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

#define XCHG2(a,b,t)   t=a;a=b;b=t

static int intra_pred4x4_top[2][16] = {
  {TCSM_RECON_YBUF0, TCSM_RECON_YBUF0, 
   TCSM_RECON_YBUF0 + 3*RECON_BUF_STRIDE, TCSM_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0, TCSM_RECON_YBUF0,
   TCSM_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 3*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF0 + 7*RECON_BUF_STRIDE, TCSM_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0 + 11*RECON_BUF_STRIDE, TCSM_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 7*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 11*RECON_BUF_STRIDE + 12
  },
  {TCSM_RECON_YBUF1, TCSM_RECON_YBUF1, 
   TCSM_RECON_YBUF1 + 3*RECON_BUF_STRIDE, TCSM_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1, TCSM_RECON_YBUF1,
   TCSM_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 3*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF1 + 7*RECON_BUF_STRIDE, TCSM_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1 + 11*RECON_BUF_STRIDE, TCSM_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 7*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 11*RECON_BUF_STRIDE + 12
  }
};
static int intra_pred4x4_left[2][16] = {
  {TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 4, 
   TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0 + 8, TCSM_RECON_YBUF0 + 12,
   TCSM_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF0 + 12*RECON_BUF_STRIDE + 12
  },
  {TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 4, 
   TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1 + 8, TCSM_RECON_YBUF1 + 12,
   TCSM_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 4*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 4,
   TCSM_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 8*RECON_BUF_STRIDE + 12,
   TCSM_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 8, TCSM_RECON_YBUF1 + 12*RECON_BUF_STRIDE + 12
  }
};

#define JZC_SHRINK_OPT

/*
  fifo_wp: aux-cpu task fifo write pointer, only write by main-cpu, only read by aux-cpu
*/
volatile int *fifo_wp;

#ifdef JZC_SHRINK_OPT
void decode_frame_aux(H264Context *h){
#define get_vir_addr(vaddr) (unsigned int)(((unsigned int)(vaddr) + (unsigned int)frame_buffer) - phy_fb)

  int mb_num;
  int mb_start;
  int total_mbs;
  int linesize, uvlinesize;
  int mb_stride;
  H264_MB_DecARGs * dMB_aux;
  int i, j;
  int *block_offset;

  mb_start = 0; //dFRM->first_mb_in_frame;
  total_mbs = dFRM->mb_width * dFRM->mb_height;
  linesize = dFRM->linesize;
  uvlinesize = dFRM->uvlinesize;
  mb_stride = dFRM->mb_stride;

  uint8_t * current_picture_y_data, * current_picture_u_data, * current_picture_v_data;
  current_picture_y_data = get_vir_addr(dFRM->current_picture.y_ptr);
  current_picture_u_data = get_vir_addr(dFRM->current_picture.u_ptr);
  current_picture_v_data = get_vir_addr(dFRM->current_picture.v_ptr);

#undef fprintf
  int mbnum=*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+0);
  //-----debug
      mp_msg(NULL,NULL," -- aux_test \n");


      unsigned int * dbg_info_p = TCSM1_VADDR_UNCACHE(TCSM1_DBG_info);
      volatile *task_done  = TCSM1_VADDR_UNCACHE(TCSM1_TASK_DONE);

      fprintf(dblkrsl_fp," -- AUX_START \n");

      do {
	fprintf(dblkrsl_fp," task_done: 0x%x; mbnum: 0x%x\n",*task_done,
		//*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+0)
		mbnum);
      } while (!(*task_done));

      *fifo_wp=0;
      fprintf(dblkrsl_fp,"info[0]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+0));
      fprintf(dblkrsl_fp,"info[1]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+4));
      fprintf(dblkrsl_fp,"info[2]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+8));
      fprintf(dblkrsl_fp,"info[3]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+12));
      fprintf(dblkrsl_fp,"info[4]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+16));
      fprintf(dblkrsl_fp,"info[5]:0x%x\n",*(unsigned*)TCSM1_VADDR_UNCACHE(TCSM1_DBG_info+20));

      unsigned char * prt_ptr=(TCSM_DBLK_TOP_4LINE_YBUF0);
      fprintf(dblkrsl_fp,"tcsm0:top4line_bu0:0x%x\n",prt_ptr);
      for(i=0;i<4;i++){
	for(j=0;j<16;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=16;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_YBUF0));
      fprintf(dblkrsl_fp,"recon_buf0:0x%x\n",prt_ptr);
      for(i=0;i<16;i++){
	for(j=0;j<16;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_YBUF1-4));
      fprintf(dblkrsl_fp,"dblk_buf1:0x%x\n",prt_ptr);
      for(i=0;i<16;i++){
	for(j=0;j<20;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      //---buf1-------
      prt_ptr=(TCSM_DBLK_TOP_4LINE_YBUF1);
      fprintf(dblkrsl_fp,"tcsm0:top4line1:0x%x\n",prt_ptr);
      for(i=0;i<4;i++){
	for(j=0;j<16;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=16;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_YBUF1));
      fprintf(dblkrsl_fp,"recon_buf1:0x%x\n",prt_ptr);
      for(i=0;i<16;i++){
	for(j=0;j<16;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_YBUF0-4));
      fprintf(dblkrsl_fp,"dblk_buf0:0x%x\n",prt_ptr);
      for(i=0;i<16;i++){
	for(j=0;j<20;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=current_picture_y_data-4;
      fprintf(dblkrsl_fp,"sdram_Y:0x%x\n",prt_ptr);
      for(i=0;i<16;i++){
	for(j=0;j<20;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=linesize;
	fprintf(dblkrsl_fp,"\n");
      }

      //U_buf0
      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_UBUF0));
      fprintf(dblkrsl_fp,"U_recon_buf0:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<8;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_UBUF1-4));
      fprintf(dblkrsl_fp,"U_dblk_buf1:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }
      //U_buf1
      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_UBUF1));
      fprintf(dblkrsl_fp,"U_recon_buf1:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<8;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_UBUF0-4));
      fprintf(dblkrsl_fp,"U_dblk_buf0:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=current_picture_u_data-4;
      fprintf(dblkrsl_fp,"sdram_U:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=uvlinesize;
	fprintf(dblkrsl_fp,"\n");
      }

      //V_buf0
      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_VBUF0));
      fprintf(dblkrsl_fp,"V_recon_buf0:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<8;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_VBUF1-4));
      fprintf(dblkrsl_fp,"V_dblk_buf1:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      //V_buf1
      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_RECON_VBUF1));
      fprintf(dblkrsl_fp,"V_recon_buf1:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<8;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=TCSM1_VADDR_UNCACHE((TCSM1_DBLK_VBUF0-4));
      fprintf(dblkrsl_fp,"V_dblk_buf0:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=RECON_BUF_STRIDE;
	fprintf(dblkrsl_fp,"\n");
      }

      prt_ptr=current_picture_v_data-4;
      fprintf(dblkrsl_fp,"sdram_V:0x%x\n",prt_ptr);
      for(i=0;i<8;i++){
	for(j=0;j<12;j++)
	  {
	    fprintf(dblkrsl_fp,"0x%2x,",prt_ptr[j]);
	  }
	prt_ptr+=uvlinesize;
	fprintf(dblkrsl_fp,"\n");
      }
#undef exit
      if(mpFrame==1)
	exit(0);
  //-----debug
#if 0
  for ( mb_num = mb_start; mb_num < total_mbs; mb_num ++ ) {

    int simple = dMB_aux->simple;
    int mb_x= dMB_aux->mb_x;
    int mb_y= dMB_aux->mb_y;
    int mb_xy= mb_x + mb_y*mb_stride;
    int mb_type= dMB_aux->mb_type;
    uint8_t  *dest_y, *dest_cb, *dest_cr;

    uint8_t *yrecon = AUX_T->h264_yrecon[0];
    uint8_t *urecon = AUX_T->h264_urecon[0];
    uint8_t *vrecon = AUX_T->h264_vrecon[0];
    uint8_t *yrecon_last = AUX_T->h264_yrecon[1];
    uint8_t *urecon_last = AUX_T->h264_urecon[1];
    uint8_t *vrecon_last = AUX_T->h264_vrecon[1];

    int *intra_pred4x4_top_ptr[2];
    int *intra_pred4x4_left_ptr[2];
    intra_pred4x4_top_ptr[0] = intra_pred4x4_top[0];
    intra_pred4x4_left_ptr[0] = intra_pred4x4_left[0];

    int bakup_x0 = mb_x + (mb_y & 1)*mb_stride;
    int bakup_x1 = mb_x + ((mb_y+1) & 1)*mb_stride;

    DCTELEM *block = TCSM_BLOCK_BUF;
    DCTELEM *dct_ptr = TCSM_IDCT_BUF0;
    DCTELEM *idct = TCSM_IDCT_BUF0;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    unsigned int *idct_chain_tab;
    idct_chain_tab = (unsigned int )TCSM_IDCT_DES_BUF0;

    void (*idct_add)(uint8_t *dst, DCTELEM *block, int stride);
    void (*idct_dc_add)(uint8_t *dst, DCTELEM *block, int stride);

    dest_y  = current_picture_y_data + (mb_y * 16* linesize  ) + mb_x * 16;
    dest_cb = current_picture_u_data + (mb_y * 8 * uvlinesize) + mb_x * 8;
    dest_cr = current_picture_v_data + (mb_y * 8 * uvlinesize) + mb_x * 8;

    if (!simple && MB_FIELD) {
      mp_msg(NULL,NULL,"MB_FIELD \n");
      linesize   = AUX_T->mb_linesize   = linesize * 2;
      uvlinesize = AUX_T->mb_uvlinesize = uvlinesize * 2;
      block_offset = &AUX_T->block_offset[24];

      if(mb_y&1){ //FIXME move out of this func?
	dest_y  -= dFRM->linesize*15;
	dest_cb -= dFRM->uvlinesize*7;
	dest_cr -= dFRM->uvlinesize*7;
      }
    }

    if(IS_8x8DCT(mb_type)){
      idct_dc_add = ff_h264_idct8_dc_add_c;
      idct_add = ff_h264_idct8_add_c;
    }else{
      idct_dc_add = ff_h264_idct_dc_add_c;
      idct_add = ff_h264_idct_add_c;
    }

    if (!simple && IS_INTRA_PCM(mb_type)) {
      unsigned int x, y;

      // The pixels are stored in dMB->mb array in the same order as levels,
      // copy them in output in the correct order.
      for(i=0; i<16; i++) {
	for (y=0; y<4; y++) {
	  for (x=0; x<4; x++) {
	    *(dest_y + block_offset[i] + y*linesize + x) = dMB_aux->mb[i*16+y*4+x];
	  }
	}
      }
      for(i=16; i<16+4; i++) {
	for (y=0; y<4; y++) {
	  for (x=0; x<4; x++) {
	    *(dest_cb + block_offset[i] + y*uvlinesize + x) = dMB_aux->mb[i*16+y*4+x];
	  }
	}
      }
      for(i=20; i<20+4; i++) {
	for (y=0; y<4; y++) {
	  for (x=0; x<4; x++) {
	    *(dest_cr + block_offset[i] + y*uvlinesize + x) = dMB_aux->mb[i*16+y*4+x];
	  }
	}
      }
    } else {

      if(IS_INTRA(mb_type)){
	short mb_buf[16*24];
	for(i=0;i<16*24;i++)
	  mb_buf[i]=dMB_aux->mb[i];
	// chroma predict
	h->hpc.pred8x8[ dMB_aux->chroma_pred_mode ](urecon, urecon_last + MB_CHROM_WIDTH, AUX_T->BackupMBbottom_U[bakup_x1]);
	h->hpc.pred8x8[ dMB_aux->chroma_pred_mode ](vrecon, vrecon_last + MB_CHROM_WIDTH, AUX_T->BackupMBbottom_V[bakup_x1]);

	// chroma predict
	if(IS_INTRA4x4(mb_type)){

	  if(IS_8x8DCT(mb_type)){

	    for(i=0; i<16; i+=4){
	      uint8_t * const ptr= dest_y + block_offset[i];
	      const int dir= dMB_aux->intra4x4_pred_mode_cache[ scan8[i] ];
	      const int nnz = dMB_aux->non_zero_count_cache[ scan8[i] ];
	      h->hpc.pred8x8l[ dir ](ptr, (dMB_aux->topleft_samples_available<<i)&0x8000,
				     (dMB_aux->topright_samples_available<<i)&0x4000, linesize);
	      if(nnz){
		if(nnz == 1 && dMB_aux->mb[i*16])
		  idct_dc_add(ptr, mb_buf + i*16, linesize);
		else
		  idct_add(ptr, mb_buf + i*16, linesize);
	      }
	    }

	  }else {
	    intra_pred4x4_top_ptr[0][0] = AUX_T->BackupMBbottom_Y[bakup_x1];
	    intra_pred4x4_top_ptr[0][1] = AUX_T->BackupMBbottom_Y[bakup_x1] + 4;
	    intra_pred4x4_top_ptr[0][4] = AUX_T->BackupMBbottom_Y[bakup_x1] + 8;
	    intra_pred4x4_top_ptr[0][5] = AUX_T->BackupMBbottom_Y[bakup_x1] + 12;
	    intra_pred4x4_left_ptr[0][0] = yrecon_last + MB_LUMA_WIDTH;
	    intra_pred4x4_left_ptr[0][2] = yrecon_last + 4*RECON_BUF_STRIDE + MB_LUMA_WIDTH;
	    intra_pred4x4_left_ptr[0][8] = yrecon_last + 8*RECON_BUF_STRIDE + MB_LUMA_WIDTH;
	    intra_pred4x4_left_ptr[0][10] = yrecon_last + 12*RECON_BUF_STRIDE + MB_LUMA_WIDTH;

	    for(i=0; i<16; i++){

	      uint8_t * src_left = intra_pred4x4_left_ptr[0][i];
	      uint8_t * src_top = intra_pred4x4_top_ptr[0][i];

	      uint8_t * src_topleft;
	      if ( (i==2) || (i==8) || (i==10) )
		src_topleft = src_left - RECON_BUF_STRIDE;
	      else src_topleft = src_top;

	      uint8_t * ptr= yrecon + MB_offset_recon[i];
	      uint8_t *topright;
	      const int dir= dMB_aux->intra4x4_pred_mode_cache[ scan8[i] ];
	      int nnz, tr;

	      if(dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED){
		const int topright_avail= (dMB_aux->topright_samples_available<<i)&0x8000;
		if(!topright_avail){
		  tr= src_top[3]*0x01010101;
		  topright= (uint8_t*) &tr;
		}else{
		  topright= src_top + 4;
		}
	      }else
		topright= NULL;
	      h->hpc.pred4x4[ dir ](ptr, src_left, topright, src_top, src_topleft);

	      nnz = dMB_aux->non_zero_count_cache[ scan8[i] ];
	      //	      mp_msg(NULL,NULL,"predmode:0x%x, nnz:0x%x, mb[%d]:0x%x\n",dir,nnz,i*16,dMB_aux->mb[i*16]);
	      //mp_msg(NULL,NULL,"src_left:0x%x, topright:0x%x, src_top:0x%x, src_topleft:0x%x\n",src_left, topright, src_top, src_topleft);
	      if(nnz){
		if(nnz == 1 && dMB_aux->mb[i*16])
		  idct_dc_add(ptr, mb_buf + i*16, RECON_BUF_STRIDE);
		else
		  idct_add(ptr, mb_buf + i*16, RECON_BUF_STRIDE);
	      }
	    }
	  }

	}else{
	  h->hpc.pred16x16[ dMB_aux->intra16x16_pred_mode ](yrecon, yrecon_last + MB_LUMA_WIDTH, AUX_T->BackupMBbottom_Y[bakup_x1]);

	  h264_luma_dc_dequant_idct_c(mb_buf, dMB_aux->qscale, dMB_aux->dequant4_coeff[0]);
	}

      }else {
	hl_motion_hw(h,dMB_aux);
	//MC_POLLING_END();
      }

#ifdef JZC_AUX_RUN
    if ( mb_num == 0 ) {
      mp_msg(NULL,NULL," -- aux_test \n");

      /*clear fifo_wp before push new task into task_fifo*/ 
      fifo_wp = (int *)TCSM1_FOFO_WP_VADDR;
      *fifo_wp = 0;

      unsigned int * dbg_bgn_p  = TCSM1_VADDR_UNCACHE(TCSM1_DBG_bgn);
      unsigned int * dbg_end_p  = TCSM1_VADDR_UNCACHE(TCSM1_DBG_end);
      unsigned int * dbg_num_p  = TCSM1_VADDR_UNCACHE(TCSM1_DBG_num);
      unsigned int * dbg_info_p = TCSM1_VADDR_UNCACHE(TCSM1_DBG_info);

      dbg_end_p[0] = 0;
      (*fifo_wp)++;

      AUX_START();

      mp_msg(NULL,NULL," -- AUX_START \n");


      do {
	mp_msg(NULL,NULL," dbg_end_p[0]: 0x%x \n",dbg_end_p[0]);
      } while (dbg_end_p[0] != 0xac49);


#if 1
      unsigned char * tmp_std_p;
      unsigned char * tmp_ref_p;
      tmp_std_p = yrecon;
      tmp_ref_p = TCSM1_VADDR_UNCACHE(TCSM1_RECON_YBUF0);
      for ( i=0; i<16; i++ ) {
	for ( j=0; j<16; j++ ) {
	  if (1|| tmp_std_p[j] != tmp_ref_p[j]) {
	    mp_msg(NULL,NULL," at line:%d col:%d \n",i,j);
	    mp_msg(NULL,NULL," std_0x%x : 0x%x \n",&(tmp_std_p[j]),tmp_std_p[j]);
	    mp_msg(NULL,NULL," ref_0x%x : 0x%x \n",&(tmp_ref_p[j]),tmp_ref_p[j]);
	  }
	}
	tmp_std_p += RECON_BUF_STRIDE;
	tmp_ref_p += RECON_BUF_STRIDE;
      }
#endif

#if 0
      unsigned char * tmp_info_p;
      tmp_info_p = TCSM1_VADDR_UNCACHE(TCSM1_RECON_YBUF0);
      for ( i=0; i<16; i++ ) {
	for ( j=0; j<16; j++ ) {
	  mp_msg(NULL,NULL," 0x%x, ",tmp_info_p[j]);
	}
	mp_msg(NULL,NULL,"\n");
	tmp_info_p += RECON_BUF_STRIDE;
      }

      tmp_info_p = yrecon;
      for ( i=0; i<16; i++ ) {
	for ( j=0; j<16; j++ ) {
	  mp_msg(NULL,NULL," 0x%x, ",tmp_info_p[j]);
	}
	mp_msg(NULL,NULL,"\n");
	tmp_info_p += RECON_BUF_STRIDE;
      }
#endif

#if 0
      unsigned * tmp_info_p = TCSM1_VADDR_UNCACHE(TCSM1_MBARGS_BUF);
      unsigned * tmp_corr_p = dMB_aux;
      mp_msg(NULL,NULL," mb[] dbging --- 0x%x: \n",tmp_info_p);
      for ( i=0; i<((sizeof(H264_MB_DecARGs))>>2); i++ ) {
	if ( tmp_corr_p[i] != tmp_info_p[i]) {
	  mp_msg(NULL,NULL," %d: \n",i);
	  mp_msg(NULL,NULL," std at0x%x  : 0x%x \n",&(tmp_corr_p[i]),tmp_corr_p[i]);
	  mp_msg(NULL,NULL," ref : 0x%x \n",tmp_info_p[i]);
	}
      }
#endif

#if 0
      mp_msg(NULL,NULL," TCSM1_FRM_ARGS  : 0x%x \n",TCSM1_FRM_ARGS);
      mp_msg(NULL,NULL," FRM_ARGS_LEN    : 0x%x \n",FRM_ARGS_LEN);
      mp_msg(NULL,NULL," TCSM1_MBARGS_BUF: 0x%x \n",TCSM1_MBARGS_BUF);
      mp_msg(NULL,NULL," MBARGS_BUF_LEN  : 0x%x \n",MBARGS_BUF_LEN);
      mp_msg(NULL,NULL," TCSM1_AUX_T     : 0x%x \n",TCSM1_AUX_T);
      mp_msg(NULL,NULL," AUX_T_LEN       : 0x%x \n",AUX_T_LEN);
      mp_msg(NULL,NULL," TCSM1_GP1_ARG_CHAIN : 0x%x \n",TCSM1_GP1_ARG_CHAIN);
      mp_msg(NULL,NULL," GP1_TRAN_CHAIN_LEN  : 0x%x \n",GP1_TRAN_CHAIN_LEN);
#endif

#if 0
      //dbg_info_p += 50;
      for ( i=0; i<16; i++ ) {
	mp_msg(NULL,NULL," %d: %d \n",i,dbg_info_p[i]);
      }
      dbg_info_p += 16;
      mp_msg(NULL,NULL," dbg_info_p[0]: %d \n",dbg_info_p[0]);
      mp_msg(NULL,NULL," dbg_info_p[1]: %d \n",dbg_info_p[1]);
#endif

#if 0
      mp_msg(NULL,NULL," dbg_info_p[0]: 0x%x \n",dbg_info_p[0]);
      mp_msg(NULL,NULL," dbg_info_p[1]: 0x%x \n",dbg_info_p[1]);
      mp_msg(NULL,NULL," dbg_info_p[2]: 0x%x \n",dbg_info_p[2]);
      mp_msg(NULL,NULL," dbg_info_p[3]: 0x%x \n",dbg_info_p[3]);
      mp_msg(NULL,NULL," dbg_info_p[4]: 0x%x \n",dbg_info_p[4]);
      mp_msg(NULL,NULL," dbg_info_p[5]: 0x%x \n",dbg_info_p[5]);
      mp_msg(NULL,NULL," dbg_info_p[6]: 0x%x \n",dbg_info_p[6]);
#endif

      mp_msg(NULL,NULL," dFRM->dMB_baseaddr: 0x%x \n",dFRM->dMB_baseaddr);
      mp_msg(NULL,NULL," dFRM->dMB_baseaddr_aux: 0x%x \n",dFRM->dMB_baseaddr_aux);

    }
#endif // JZC_AUX_RUN

      tcsm_pred_rslt_out(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize);

      if(!IS_INTRA4x4(mb_type)){

	if(IS_INTRA16x16(mb_type)){
	  for(i=0; i<16; i++){

	    if(dMB_aux->non_zero_count_cache[ scan8[i] ]) {
	      hw_idct_descrip_nod_conf(block + i*16, dct_ptr+i*16);
	      int l;
	      for (l=0; l<16; l++) {
		block[i*16+l]= dMB_aux->mb[i*16+l];
	      } 
	      idct_chain_tab += IDCT_NODE_LEN;
	      idct_cbp[i] = 1;
	    }
	    else if(dMB_aux->mb[i*16])
	      idct_dc_add(dest_y + block_offset[i], dMB_aux->mb + i*16, linesize);

	  }
	}else{
	  const int di = IS_8x8DCT(mb_type) ? 4 : 1;
	  for(i=0; i<16; i+=di){
	    int nnz = dMB_aux->non_zero_count_cache[ scan8[i] ];
	    if(nnz){

	      if(nnz==1 && dMB_aux->mb[i*16])
		idct_dc_add(dest_y + block_offset[i], dMB_aux->mb + i*16, linesize);
	      else {
		if (di==1) {
		  hw_idct_descrip_nod_conf(block + i*16, dct_ptr + i*16);
		  int l;
		  for (l=0; l<16; l++) {
		    block[i*16+l]= dMB_aux->mb[i*16+l];
		  }  
		  idct_chain_tab += IDCT_NODE_LEN;
		  idct_cbp[i] = 1;               
		}
		else
		  idct_add(dest_y + block_offset[i], dMB_aux->mb + i*16, linesize);
	      }
	    }
	  }
	}
      }

      // chroma dc_dq & idct
      uint8_t *dest[2] = {dest_cb, dest_cr};
      idct_add = ff_h264_idct_add_c;
      idct_dc_add = ff_h264_idct_dc_add_c;
      chroma_dc_dequant_idct_c(dMB_aux->mb + 16*16, dMB_aux->chroma_qp[0], dMB_aux->dequant4_coeff[1]);
      chroma_dc_dequant_idct_c(dMB_aux->mb + 16*16+4*16, dMB_aux->chroma_qp[1], dMB_aux->dequant4_coeff[2]);

      for(i=16; i<16+8; i++){
	if(dMB_aux->non_zero_count_cache[ scan8[i] ]) {
	  hw_idct_descrip_nod_conf(block + i*16, dct_ptr + i*16);
	  int l;
	  for (l=0; l<16; l++) {
	    block[i*16+l]= dMB_aux->mb[i*16+l];
	  } 
	  idct_chain_tab += IDCT_NODE_LEN;
	  idct_cbp[i] = 1; 
	} else if (dMB_aux->mb[i*16]) {
	  idct_dc_add(dest[(i&4)>>2] + block_offset[i], dMB_aux->mb + i*16, uvlinesize);
	}
      }

      if ( (unsigned int)idct_chain_tab != TCSM_IDCT_DES_BUF0 ) {
	*(idct_chain_tab - 3) = IDCT_UNLINK_HEAD;
	hw_run_idct_c();
      }
      // polling idct end
      hw_wait_idct_c();
      // add error
      for (i=0; i<16; i++)
	if(idct_cbp[i]){
	  h264_idct_error_add_hw(dest_y + block_offset[i], idct+i*16, linesize, 4);
	  idct_cbp[i] = 0;
	}
      for (i=16; i<16+4; i++) 
	if(idct_cbp[i]){
	  h264_idct_error_add_hw(dest_cb + block_offset[i], idct+i*16, uvlinesize, 4);
	  idct_cbp[i] = 0;
	}
      for (i=20; i<20+4; i++) 
	if(idct_cbp[i]){
	  h264_idct_error_add_hw(dest_cr + block_offset[i], idct+i*16, uvlinesize, 4);
	  idct_cbp[i] = 0; 
	}

    }

    {
      int i;
      uint8_t *tcsm_dst = yrecon_last + MB_LUMA_WIDTH - 1;
      uint8_t *src = dest_y + MB_LUMA_WIDTH - 1;
      for(i=0; i<16; i++){
	tcsm_dst[0] = src[0];
	tcsm_dst += RECON_BUF_STRIDE;
	src += linesize;
      }
      tcsm_dst = urecon_last + MB_CHROM_WIDTH - 1;
      src = dest_cb + MB_CHROM_WIDTH - 1;
      for(i=0; i<8; i++){
	tcsm_dst[0] = src[0];
	tcsm_dst += RECON_BUF_STRIDE;
	src += uvlinesize;
      }
      tcsm_dst = vrecon_last + MB_CHROM_WIDTH - 1;
      src = dest_cr + MB_CHROM_WIDTH - 1;
      for(i=0; i<8; i++){
	tcsm_dst[0] = src[0];
	tcsm_dst += RECON_BUF_STRIDE;
	src += uvlinesize;
      }

      uint32_t *Y_Bottom, *U_Bottom, *V_Bottom;
      uint32_t *src_y_bottom, *src_u_bottom, *src_v_bottom;
      Y_Bottom = AUX_T->BackupMBbottom_Y[bakup_x0];
      U_Bottom = AUX_T->BackupMBbottom_U[bakup_x0];
      V_Bottom = AUX_T->BackupMBbottom_V[bakup_x0];
      src_y_bottom = dest_y + 15*linesize;
      src_u_bottom = dest_cb + 7*uvlinesize;
      src_v_bottom = dest_cr + 7*uvlinesize;

      Y_Bottom[0] = src_y_bottom[0];
      Y_Bottom[1] = src_y_bottom[1];
      Y_Bottom[2] = src_y_bottom[2];
      Y_Bottom[3] = src_y_bottom[3];
      U_Bottom[0] = src_u_bottom[0];
      U_Bottom[1] = src_u_bottom[1];
      V_Bottom[0] = src_v_bottom[0];
      V_Bottom[1] = src_v_bottom[1];

    }

    if(dMB_aux->deblocking_filter) {

      fill_caches(h, mb_type, 1); //FIXME don't fill stuff which isn't used by filter_mb
      filter_mb_dblk(h, dMB_aux, mb_x, mb_y, dest_y, dest_cb, dest_cr, linesize, uvlinesize);

#ifdef JZC_H264_DUMP
      if ( (jzc_dump_fcnt >= DUMP_FRAME_STR) && (jzc_dump_fcnt <= DUMP_FRAME_END) )
	{
	  int dump_i, dump_j;
	  uint8_t *dump_ptr;

	  fprintf(jzc_h264_dump_fp, "---- u dblked buffer, frame:%d, mb_x:%d, mb_y:%d\n",jzc_dump_fcnt,mb_x,mb_y);
	  dump_ptr = dest_cb;
	  for (dump_i=0; dump_i<8; dump_i++) {
	    for (dump_j=0; dump_j<8; dump_j++) {
	      fprintf(jzc_h264_dump_fp, "0x%2x, ",dump_ptr[dump_j]);
	    }
	    dump_ptr += uvlinesize;
	    fprintf(jzc_h264_dump_fp, "\n");
	  }

	  fprintf(jzc_h264_dump_fp, "---- v dblked buffer, frame:%d, mb_x:%d, mb_y:%d\n",jzc_dump_fcnt,mb_x,mb_y);
	  dump_ptr = dest_cr;
	  for (dump_i=0; dump_i<8; dump_i++) {
	    for (dump_j=0; dump_j<8; dump_j++) {
	      fprintf(jzc_h264_dump_fp, "0x%2x, ",dump_ptr[dump_j]);
	    }
	    dump_ptr += uvlinesize;
	    fprintf(jzc_h264_dump_fp, "\n");
	  }

	  fprintf(jzc_h264_dump_fp, "---- y dblked buffer, frame:%d, mb_x:%d, mb_y:%d\n",jzc_dump_fcnt,mb_x,mb_y);
	  dump_ptr = dest_y;
	  for (dump_i=0; dump_i<16; dump_i++) {
	    for (dump_j=0; dump_j<16; dump_j++) {
	      fprintf(jzc_h264_dump_fp, "0x%2x, ",dump_ptr[dump_j]);
	    }
	    dump_ptr += linesize;
	    fprintf(jzc_h264_dump_fp,"\n");
	  }
	}
#endif // JZC_H264_DUMP

    }

    dMB_aux++;

  }
#endif
}
#else // !JZC_SHRINK_OPT
#include "h264_unshrinked_funcs.c"
#endif JZC_SHRINK_OPT
