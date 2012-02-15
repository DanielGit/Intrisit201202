/*****************************************************************************
 *
 * JZC VC1 decoder AUX-CPU task
 *
 *
 ****************************************************************************/
#define AUX_VADDR_SPACE
  
#include "vc1_tcsm.h"
#include "vc1_aux_type.h"  
#include "vc1_aux.h"  
#include "../jzmedia.h"
#include "vc1_aux_mc_hw.h"
#include "../jz4750e_mc.h"
#include "vc1_jzmacro.h"
#include "../jz4750e_mdma.h"      

  
extern int _gp;

#define i_nop  ({__asm__ __volatile__("nop\t#i_nop":::"memory");})

#define i_la(src)                                               			\
({  	unsigned long _dst_;                           					\
	__asm__ __volatile__("la\t""%0,%1""\t#i_la macro":"=r"(_dst_) :"X" (src));      \
	_dst_;})

#define restrict __restrict  

#define XCHG2(a,b,t)   t=a;a=b;b=t  
#define XCHG3(a,b,c,t)     {(t)=(a);(a)=(b);(b)=(c);(c)=(t);}  


volatile int *fifo_wp, *task_done,*fifo_rp_tcsm;


static void vc1_put_block_aux(DCTELEM block[6][64]);
static void add_pixels_clamped_aux(const DCTELEM *block, uint8_t *restrict pixels,
				   int line_size);
static void put_pixels_clamped_aux(const DCTELEM *block, uint8_t *restrict pixels,
				   int line_size);
static void vc1_v_overlap_aux(uint8_t* src, int stride);
static void vc1_h_overlap_aux(uint8_t* src, int stride);
static void vc1_inv_trans_8x8_aux(DCTELEM block[64]);
static void vc1_inv_trans_8x4_aux(DCTELEM block[64], int n);
static void vc1_inv_trans_4x8_aux(DCTELEM block[64], int n);
static void vc1_inv_trans_4x4_aux(DCTELEM block[64], int n);
static int vc1_decode_p_block_idct(DCTELEM block[64], int n);

void ff_emulated_edge_mc_hw(uint8_t *buf, uint8_t *src,
			    int buf_linesize, int src_linesize, int block_w, int block_h,
			    int src_x, int src_y, int w, int h);

void vc1_mc_1mv_hw(int dir, uint8_t *emu_edge_buf_mb); 
static void vc1_interp_mc(uint8_t *emu_edge_buf_mb);
void vc1_mc_4mv_luma_hw(int n, uint8_t *dest_y, uint8_t *emu_edge_buf_mb);
void vc1_mc_4mv_chroma_hw(uint8_t *emu_edge_buf_mb);
void  mc_mv_chroma_hw(int uvmx,int uvmy,uint8_t *srcU,uint8_t *srcV,uint8_t *dest_cb,uint8_t *dest_cr);
void mc_tmp_copy(char *dest_y, char *dest_cb, char *dest_cr, int linesize, int uvlinesize);
  
VC1_Frame_GlbARGs *dFRM;
VC1_MB_DecARGs *dMB_aux; 
VC1_AUX_T VC1_aux_t;

uint32_t VC1LumaIntp[16];


int blk_num, mc_des_dirty;

__tcsm1_main int main() {  

    int c;       
    S32I2M(xr16,3);
    c = i_la(_gp);

    task_done  = (int *)TCSM1_TASK_DONE;
    fifo_wp = (int *)TCSM1_FIFO_WP;
    fifo_rp_tcsm = (int *)TCSM1_FIFO_RP;
    int fifo_rp = 0;
    *fifo_rp_tcsm = fifo_rp;

    MDMA_DesNode *MDMA1_TRAN = (MDMA_DesNode *)TCSM1_GP1_TRAN_CHAIN;
    MDMA_DesNode *MDMA1_TRAN2 = (MDMA_DesNode *)TCSM1_GP1_TRAN_CHAIN2;
    MDMA_DesNode *MDMA1_ARG = (MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
    VC1_MB_DecARGs *dMBsrc;
    VC1_MB_DecARGs *dMB2 = (VC1_MB_DecARGs *)TCSM1_MBARGS_BUF2;
    dMB_aux = (VC1_MB_DecARGs *)TCSM1_MBARGS_BUF;
    dFRM = (VC1_Frame_GlbARGs *)TCSM1_FRM_ARGS;
    VC1_MB_DecARGs *dMBbase = dFRM->dMB_baseaddr_aux;
    int m,n;
    int i, j;
    int off = 0, mb_pos = 0;
    unsigned int XCHGtmp;
    int count;
    int auxMBs = dFRM->mb_width * dFRM->mb_height;
    const int linesize= dFRM->slinesize[0]; //not s->linesize as this would be wrong for field pics
    const int uvlinesize= dFRM->slinesize[1];
    const int mb_size= 4 - dFRM->lowres;
    int block_size = 8>>dFRM->lowres;
    uint8_t vc1_dst_idx[6] = {0,0,0,0,1,2};    
   
    VC1LumaIntp[0] = (H0V0<<16);
    VC1LumaIntp[1] = (H1V0<<16)                   |(32-(dFRM->rnd));
    VC1LumaIntp[2] = (H2V0<<16)                   |(8 -(dFRM->rnd));
    VC1LumaIntp[3] = (H3V0<<16)                   |(32-(dFRM->rnd));

    VC1LumaIntp[4] = (H0V1<<16)                   |(31+(dFRM->rnd));
    VC1LumaIntp[5] = (H1V1<<16)|((31+(dFRM->rnd))<<8)|(32-(dFRM->rnd));
    VC1LumaIntp[6] = (H2V1<<16)|((31+(dFRM->rnd))<<8)|(8 -(dFRM->rnd));
    VC1LumaIntp[7] = (H3V1<<16)|((31+(dFRM->rnd))<<8)|(32-(dFRM->rnd));
    
    VC1LumaIntp[8] = (H0V2<<16)                   |(7 +(dFRM->rnd));    
    VC1LumaIntp[9] = (H1V2<<16)|((7 +(dFRM->rnd))<<8)|(32-(dFRM->rnd));
    VC1LumaIntp[10]= (H2V2<<16)|((7 +(dFRM->rnd))<<8)|(8 -(dFRM->rnd));
    VC1LumaIntp[11]= (H3V2<<16)|((7 +(dFRM->rnd))<<8)|(32-(dFRM->rnd));
    VC1LumaIntp[12]= (H0V3<<16)                   |(31+(dFRM->rnd));
    VC1LumaIntp[13]= (H1V3<<16)|((31+(dFRM->rnd))<<8)|(32-(dFRM->rnd));
    VC1LumaIntp[14]= (H2V3<<16)|((31+(dFRM->rnd))<<8)|(8 -(dFRM->rnd));
    VC1LumaIntp[15]= (H3V3<<16)|((31+(dFRM->rnd))<<8)|(32-(dFRM->rnd));

    VC1_aux_t.recon[0] = TCSM1_YRECON_BUF;
    VC1_aux_t.recon[1] = TCSM1_URECON_BUF;
    VC1_aux_t.recon[2] = TCSM1_VRECON_BUF;

    VC1_aux_t.recon_xchg[0] = TCSM1_YRECON_XCH_BUF;
    VC1_aux_t.recon_xchg[1] = TCSM1_URECON_XCH_BUF;  
    VC1_aux_t.recon_xchg[2] = TCSM1_VRECON_XCH_BUF;

    VC1_aux_t.recon_xchg1[0] = TCSM1_YRECON_XCH_BUF1;
    VC1_aux_t.recon_xchg1[1] = TCSM1_URECON_XCH_BUF1;  
    VC1_aux_t.recon_xchg1[2] = TCSM1_VRECON_XCH_BUF1;

    VC1_aux_t.recon_avg[0] = TCSM1_YRECON_AVG_BUF;
    VC1_aux_t.recon_avg[1] = TCSM1_URECON_AVG_BUF;  
    VC1_aux_t.recon_avg[2] = TCSM1_VRECON_AVG_BUF;

    VC1_aux_t.recon_xchg_avg[0] = TCSM1_YRECON_XCH_AVG_BUF;
    VC1_aux_t.recon_xchg_avg[1] = TCSM1_URECON_XCH_AVG_BUF;  
    VC1_aux_t.recon_xchg_avg[2] = TCSM1_VRECON_XCH_AVG_BUF;

    VC1_aux_t.mc_chain[0] = TCSM1_MC_CHAIN0;
    VC1_aux_t.mc_chain[1] = TCSM1_MC_CHAIN1;

    uint32_t current_picture_ptr[3];
    int vc1_b_mc_skipped;
    int vc1_direct;
    int bfmv_type;
    int vc1_fourmv;
    int vc1_skipped;
    int pintra;
    int bintra;
    int dmb_len;
    int nnz_flag;
    int vc1_mb_has_coeffs[2];
    
    int vc1_intra[6];
    int pb_val[6];
    int ttblock[6];
    int subblockpat[6];

    current_picture_ptr[0] = dFRM->current_picture_save.ptr[0];
    current_picture_ptr[1] = dFRM->current_picture_save.ptr[1];
    current_picture_ptr[2] = dFRM->current_picture_save.ptr[2];

    dMBsrc = dMBbase;

    do{
    }while(fifo_rp == *fifo_wp);
    fifo_rp++;
    *fifo_rp_tcsm = fifo_rp;
    MDMA1_ARG->TSA = get_phy_addr_aux((uint32_t)dMBsrc);
    MDMA1_ARG->TDA = (uint32_t)dMB_aux;
    MDMA1_ARG->STRD = MDMA_STRD(64, 64);
    MDMA1_ARG->UNIT = MDMA_UNIT(1,64,(sizeof(VC1_MB_DecARGs)));
    SET_MDMA1_DHA((uint32_t)TCSM1_GP1_ARG_CHAIN);
    MDMA1_RUN();
    dMBsrc++;
    POLLING_MDMA1_END();

    do{
    }while(fifo_rp == *fifo_wp);
    fifo_rp++;
    *fifo_rp_tcsm = fifo_rp;
    MDMA1_ARG->TSA = get_phy_addr_aux((uint32_t)dMBsrc);
    MDMA1_ARG->TDA = (uint32_t)dMB2;
    MDMA1_ARG->STRD = MDMA_STRD(64, 64);
    MDMA1_ARG->UNIT = MDMA_UNIT(1,64,(sizeof(VC1_MB_DecARGs)));
    SET_MDMA1_DHA((uint32_t)TCSM1_GP1_ARG_CHAIN);
    MDMA1_RUN();  
    dMBsrc++;
    POLLING_MDMA1_END();


    for(count = 0; count<auxMBs+1; count++){

    dFRM->current_picture.ptr[0] = dFRM->current_picture_save.ptr[0] + ((dMB_aux->mb_x - 1) << mb_size);
    dFRM->current_picture.ptr[1] = dFRM->current_picture_save.ptr[1] + ((dMB_aux->mb_x - 1) << (mb_size - dFRM->chroma_x_shift));
    dFRM->current_picture.ptr[2] = dFRM->current_picture_save.ptr[2] + ((dMB_aux->mb_x - 1) << (mb_size - dFRM->chroma_x_shift));
  
    if(!(dFRM->pict_type==B_TYPE && dFRM->draw_horiz_band && dFRM->picture_structure==PICT_FRAME))
    {
      dFRM->current_picture.ptr[0] += dMB_aux->mb_y *   linesize << mb_size;
      dFRM->current_picture.ptr[1] += dMB_aux->mb_y * uvlinesize << (mb_size - dFRM->chroma_y_shift);
      dFRM->current_picture.ptr[2] += dMB_aux->mb_y * uvlinesize << (mb_size - dFRM->chroma_y_shift);
    }

    dFRM->current_picture.ptr[0]+= 2*block_size;
    dFRM->current_picture.ptr[1]+= block_size;
    dFRM->current_picture.ptr[2]+= block_size;

    blk_num =0;    
    mc_des_dirty =0; 

    if (count<auxMBs) {
      int mb_pos = dMB_aux->mb_y * dFRM->mb_width + dMB_aux->mb_x;
      uint8_t *emu_edge_buf_mb = dFRM->emu_edge_buf_aux + mb_pos * 2*EMU_BUF_STRD;
      
      if (mb_pos >= DEFAULT_VC1_MAX_MBNUM1){
	  emu_edge_buf_mb = dFRM->emu_edge_buf_aux + (mb_pos-DEFAULT_VC1_MAX_MBNUM1) * 2*EMU_BUF_STRD;
      }

    if(dFRM->pict_type == P_TYPE){
      if (!dMB_aux->vc1_fourmv) {
	if (((!dMB_aux->vc1_skipped) && (!dMB_aux->pintra)) || (dMB_aux->vc1_skipped)){
	  vc1_mc_1mv_hw(0,emu_edge_buf_mb);
	}
      }else{
	uint8_t *dest_y;
	for (i=0; i<4; i++) {
	  dest_y =  VC1_aux_t.recon[0] +  RECON_BUF_STRD * 4 * (i&2) + (i&1) * 8;
	  if (((!dMB_aux->vc1_skipped) && (!dMB_aux->sintra[i])) || (dMB_aux->vc1_skipped)) {
	    vc1_mc_4mv_luma_hw(i,dest_y,emu_edge_buf_mb);
	  }
	  emu_edge_buf_mb += EMU_BUF_4MV_Y_STRD;
	}
	vc1_mc_4mv_chroma_hw(emu_edge_buf_mb);
      }
    } else if (dFRM->pict_type == B_TYPE && (!(dFRM->bi_type))) {
      if (!dMB_aux->vc1_b_mc_skipped){
	if(dFRM->use_ic) {
	  dMB_aux->mv_mode2 = dMB_aux->mv_mode;
	  dMB_aux->mv_mode = MV_PMODE_INTENSITY_COMP;
	}
	if(dMB_aux->vc1_direct || (dMB_aux->bfmv_type == BMV_TYPE_INTERPOLATED)) {
	  vc1_mc_1mv_hw(0,emu_edge_buf_mb);
	} else {
	  if(dFRM->use_ic && (dMB_aux->bfmv_type == BMV_TYPE_BACKWARD)) dMB_aux->mv_mode = dMB_aux->mv_mode2;
	  vc1_mc_1mv_hw((dMB_aux->bfmv_type == BMV_TYPE_BACKWARD),emu_edge_buf_mb);
	}
	if(dFRM->use_ic) dMB_aux->mv_mode = dMB_aux->mv_mode2;

	emu_edge_buf_mb += EMU_BUF_STRD;
	if(dMB_aux->vc1_direct || (dMB_aux->bfmv_type == BMV_TYPE_INTERPOLATED)) {
	  vc1_interp_mc(emu_edge_buf_mb);
	}
      }
    }

    }
   
    MC_POLLING_END();
    if(mc_des_dirty){
      VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
      vc1_mc[blk_num - 1].NodeHead = VC1_YUHEAD_UNLINK;
      SET_MC_DHA((uint32_t)vc1_mc);
      CLEAR_MC_TTEND();
      SET_MC_DCS(); 
    }

    if (count) {
    if (dFRM->pict_type == B_TYPE && (!(dFRM->bi_type))) {
      if (!vc1_b_mc_skipped){
	if(vc1_direct || (bfmv_type == BMV_TYPE_INTERPOLATED)) {
	  DirectB_MBAVG_MXU();
	}
      }
    }

    if (dFRM->pict_type == B_TYPE && (!(dFRM->bi_type))) {
      if (vc1_skipped) {
	goto end_of_mb;
      } else if (!vc1_direct) {
	if(!vc1_mb_has_coeffs[0] && !pintra)
	  goto end_of_mb;
	if(!(pintra && !vc1_mb_has_coeffs[0]))
	  if(bfmv_type == BMV_TYPE_INTERPOLATED)
	    if(!vc1_mb_has_coeffs[1])
	      goto end_of_mb;
      }
    }
       
#ifdef JZC_DMA_OPT
    int nnz_index = 0;
    int mb_recon[32];
#endif
    //begin idct and add residual
    if(dFRM->pict_type == I_TYPE || (dFRM->pict_type == B_TYPE && dFRM->bi_type)){
      for(i = 0; i < 6; i++) {
	vc1_inv_trans_8x8_aux(&dMB_aux->mb[i*64]);
	if(dFRM->profile != PROFILE_ADVANCED){
	  if(!dFRM->res_fasttx && !dFRM->res_x8)
	    for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] -= 16;
	  if(dFRM->pq >= 9 && dFRM->overlap) {
	    for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] += 128;
	  }
	}else{
	  for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] += 128;
	}
      }  
      {
	DCTELEM *block = dMB_aux->mb;
	uint8_t *Y;
	int ys, us, vs;  
 	if(dFRM->rangeredfrm) {
	  int i, j, k;
	  for(k = 0; k < 6; k++)
	    for(j = 0; j < 8; j++)
	      for(i = 0; i < 8; i++)
		block[k*64 + i + j*8] = ((block[k*64 + i + j*8] - 128) << 1) + 128;

	}
	Y = VC1_aux_t.recon_xchg[0];   
	put_pixels_clamped_aux(block, Y, RECON_BUF_STRD);
	put_pixels_clamped_aux(block + 64, Y + 8, RECON_BUF_STRD);
	Y += RECON_BUF_STRD * 8;
	put_pixels_clamped_aux(block + 64*2, Y, RECON_BUF_STRD);
	put_pixels_clamped_aux(block + 64*3, Y + 8, RECON_BUF_STRD);
	if(!(dFRM->flags & CODEC_FLAG_GRAY)) {
	  put_pixels_clamped_aux(block + 64*4,VC1_aux_t.recon_xchg[1], RECON_BUF_STRD);
	  put_pixels_clamped_aux(block + 64*5,VC1_aux_t.recon_xchg[2], RECON_BUF_STRD);
	}
      }

    } else {
      if(dFRM->pict_type == P_TYPE)
	if (!vc1_skipped)
	  if (!vc1_fourmv)
	    for (i=0; i<6; i++)
	      vc1_intra[i] = pintra;

      if(dFRM->pict_type == B_TYPE)
	for (i=0; i<6; i++)
	  vc1_intra[i] = bintra;

      if( !((dFRM->pict_type == P_TYPE) && (vc1_skipped)) ) {
	for (i=0; i<6; i++){
	  off = (i & 4) ? 0 : ((i & 1) * 8 + (i & 2) * 4 * dFRM->linesize);
	  int off1 = (i & 4) ? 0 : ((i & 1) * 8 + (i & 2) * 4 * RECON_BUF_STRD);
	  int m,n;
	  int stride = (i&4)?dFRM->uvlinesize:dFRM->linesize;
#ifdef JZC_DMA_OPT
	  DCTELEM *pblock = 0;
	  if (vc1_intra[i]) {
	    if((i>3) && (dFRM->flags & CODEC_FLAG_GRAY))
	      continue;
	    
	    if(nnz_flag & (1<<i)){
	      vc1_inv_trans_8x8_aux(&dMB_aux->mb[nnz_index*64]);
	      pblock = &dMB_aux->mb[nnz_index*64];
	      nnz_index++;
	    }else{
	      pblock = (DCTELEM *)mb_recon;
	      for(j = 0; j < 32; j++) ((int *)pblock)[j] = 0;
	    }
	    if(dFRM->rangeredfrm)
	      for(j = 0; j < 64; j++) pblock[j] <<= 1;
	    for(j = 0; j < 64; j++) pblock[j] += 128;
	    if(!dFRM->res_fasttx && dFRM->res_x8 && (dFRM->pict_type == P_TYPE))
	      for(j = 0; j < 64; j++) pblock[j] += 16;
            
	    put_pixels_clamped_aux(pblock, VC1_aux_t.recon_xchg[vc1_dst_idx[i]] + off1, RECON_BUF_STRD);

#if 0               
	    ///////////////dblk start//////////            
	    {
		  uint8_t *src_yuv_sdram_top = s->dest[VC1_dMB.vc1_dst_idx[i]] + off - 2*(s->linesize >> ((i & 4) >> 2));
		  uint8_t *dest_yuv_tcsm_top  = v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1 - 2*RECON_BUF_STRD;
		  int k, l, p, q;
		  for(l = 0; l < 2; l++) {
		    for(k = 0; k < 8; k++) {
		      dest_yuv_tcsm_top[k] = src_yuv_sdram_top[k];
		    }
		    dest_yuv_tcsm_top  += RECON_BUF_STRD;
		    src_yuv_sdram_top += s->linesize >> ((i & 4) >> 2);
		  }
		}
		//	if( (i==0) || (i==2) || (i==4) || (i==5) )
                {
		  uint8_t * src_yuv_sdram_left  = s->dest[VC1_dMB.vc1_dst_idx[i]] + off  - 2;
		  uint8_t * dest_yuv_tcsm_left = v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1 - 2;
		  int k, l, p, q;
		    for(l = 0; l < 2; l++) {
		      for(k = 0,p = 0,q = 0; k < 8; k++) {
			dest_yuv_tcsm_left[q] = src_yuv_sdram_left[p];
			p+=(s->linesize >> ((i & 4) >> 2));
			q+=RECON_BUF_STRD;
		      }
		      src_yuv_sdram_left  += 1;
		      dest_yuv_tcsm_left += 1;
		    }  
	        }

              	// dblk
		if(v->pq >= 9 && v->overlap) {
		  if(VC1_dMB.vc1_c_avail[i])
		    s->dsp.vc1_h_overlap(v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1, RECON_BUF_STRD);
		  if(VC1_dMB.vc1_a_avail[i])
		    s->dsp.vc1_v_overlap(v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1, RECON_BUF_STRD);
		}

  	{
		  uint8_t *src_yuv_sdram_top = s->dest[VC1_dMB.vc1_dst_idx[i]] + off - 2*(s->linesize >> ((i & 4) >> 2));
		  uint8_t *dest_yuv_tcsm_top  = v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1 - 2*RECON_BUF_STRD;
		  int k, l, p, q;
		  for(l = 0; l < 2; l++) {
		    for(k = 0; k < 8; k++) {
		      src_yuv_sdram_top[k] = dest_yuv_tcsm_top[k];
		    }
		    dest_yuv_tcsm_top  += RECON_BUF_STRD;
		    src_yuv_sdram_top += s->linesize >> ((i & 4) >> 2);
		  }
		}
		{
		  uint8_t * src_yuv_sdram_left  = s->dest[VC1_dMB.vc1_dst_idx[i]] + off  - 2;
		  uint8_t * dest_yuv_tcsm_left = v->recon_xchg[VC1_dMB.vc1_dst_idx[i]] + off1 - 2;
		  int k, l, p, q;
		    for(l = 0; l < 2; l++) {
		      for(k = 0,p = 0,q = 0; k < 8; k++) {
			src_yuv_sdram_left[p] = dest_yuv_tcsm_left[q];
			p+=(s->linesize >> ((i & 4) >> 2));
			q+=RECON_BUF_STRD;
		      }
		      src_yuv_sdram_left  += 1;
		      dest_yuv_tcsm_left += 1;
		    }  
	        }
		///////////////dblk end///////////
#endif

	  }else if(pb_val[i]) {
	    
	    if(nnz_flag & (1<<i)){
	      pblock = dMB_aux->mb + nnz_index*64;
	      int j = 0;
	      switch(ttblock[i]) {
	      case TT_8X8: 
		vc1_inv_trans_8x8_aux(pblock);
		break;
	      case TT_4X4:
		for(j = 0; j < 4; j++) {
		  if(!(subblockpat[i] & (1 << (3 - j))))
		    vc1_inv_trans_4x4_aux(pblock, j);
		}
		break;
	      case TT_8X4:
		for(j = 0; j < 2; j++) {
		  if(!(subblockpat[i] & (1 << (1 - j))))
		    vc1_inv_trans_8x4_aux(pblock, j);
		}
		break;
	      case TT_4X8:
		for(j = 0; j < 2; j++) {
		  if(!(subblockpat[i] & (1 << (1 - j))))
		    vc1_inv_trans_4x8_aux(pblock, j);
		}
		break;
	      }
	      if((i<4) || !(dFRM->flags & CODEC_FLAG_GRAY))
		add_pixels_clamped_aux(pblock, VC1_aux_t.recon_xchg[vc1_dst_idx[i]] + off1,RECON_BUF_STRD);

	      nnz_index++;
	    } 
	  }
#else
	  if (vc1_intra[i]) {
	    if((i>3) && (dFRM->flags & CODEC_FLAG_GRAY))
	      continue;
	    vc1_inv_trans_8x8_aux(&dMB_aux->mb[i*64]);
	    if(dFRM->rangeredfrm)
	      for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] <<= 1;
	    for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] += 128;
	    if(!dFRM->res_fasttx && dFRM->res_x8 && (dFRM->pict_type == P_TYPE))
	      for(j = 0; j < 64; j++) dMB_aux->mb[i*64+j] += 16;

	    put_pixels_clamped_aux(&dMB_aux->mb[i*64], VC1_aux_t.recon_xchg[vc1_dst_idx[i]] + off1, RECON_BUF_STRD);

	  }else if(pb_val[i]) {
	    {
	      DCTELEM *block = dMB_aux->mb + i*64;
	      int j = 0;
	      switch(ttblock[i]) {
	      case TT_8X8: 
		vc1_inv_trans_8x8_aux(block);
		break;
	      case TT_4X4:
		for(j = 0; j < 4; j++) {
		  if(!(subblockpat[i] & (1 << (3 - j))))
		    vc1_inv_trans_4x4_aux(block, j);
		}
		break;
	      case TT_8X4:
		for(j = 0; j < 2; j++) {
		  if(!(subblockpat[i] & (1 << (1 - j))))
		    vc1_inv_trans_8x4_aux(block, j);
		}
		break;
	      case TT_4X8:
		for(j = 0; j < 2; j++) {
		  if(!(subblockpat[i] & (1 << (1 - j))))
		    vc1_inv_trans_4x8_aux(block, j);
		}
		break;
	      }
	    }

	    if((i<4) || !(dFRM->flags & CODEC_FLAG_GRAY))
	      add_pixels_clamped_aux(&dMB_aux->mb[i*64], VC1_aux_t.recon_xchg[vc1_dst_idx[i]] + off1,RECON_BUF_STRD);

	  }
#endif
	} 
      }
    }

    }

    end_of_mb:
 
    MDMA1_TRAN[0].TSA  = VC1_aux_t.recon_xchg[0];
    MDMA1_TRAN[0].TDA  = get_phy_addr_aux(current_picture_ptr[0]);
    MDMA1_TRAN[0].STRD = MDMA_STRD(RECON_BUF_STRD, dFRM->linesize);
    MDMA1_TRAN[0].UNIT = MDMA_UNIT(0,16,256);

    MDMA1_TRAN[1].TSA  = VC1_aux_t.recon_xchg[1];
    MDMA1_TRAN[1].TDA  = get_phy_addr_aux(current_picture_ptr[1]);
    MDMA1_TRAN[1].STRD = MDMA_STRD(RECON_BUF_STRD, dFRM->uvlinesize);
    MDMA1_TRAN[1].UNIT = MDMA_UNIT(0,8,64);

    MDMA1_TRAN[2].TSA  = VC1_aux_t.recon_xchg[2];
    MDMA1_TRAN[2].TDA  = get_phy_addr_aux(current_picture_ptr[2]);
    MDMA1_TRAN[2].STRD = MDMA_STRD(RECON_BUF_STRD, dFRM->uvlinesize);
    MDMA1_TRAN[2].UNIT = MDMA_UNIT(0,8,64);

    {// back up idct/mc_avg info for next MB
      int i,j;
      vc1_b_mc_skipped = dMB_aux->vc1_b_mc_skipped;
      vc1_direct = dMB_aux->vc1_direct;
      bfmv_type = dMB_aux->bfmv_type;
      vc1_skipped = dMB_aux->vc1_skipped;
      pintra = dMB_aux->pintra;
      bintra = dMB_aux->bintra;
      vc1_mb_has_coeffs[0] = dMB_aux->vc1_mb_has_coeffs[0];
      vc1_mb_has_coeffs[1] = dMB_aux->vc1_mb_has_coeffs[1];
      vc1_fourmv = dMB_aux->vc1_fourmv;
      dmb_len = dMB_aux->dmb_len;
      nnz_flag = dMB_aux->nnz_flag;

      current_picture_ptr[0] = dFRM->current_picture.ptr[0];
      current_picture_ptr[1] = dFRM->current_picture.ptr[1];
      current_picture_ptr[2] = dFRM->current_picture.ptr[2];

      for (i=0; i<6; i++) {
	vc1_intra[i] = dMB_aux->vc1_intra[i];
	pb_val[i] = dMB_aux->pb_val[i];
	ttblock[i] = dMB_aux->ttblock[i];
	subblockpat[i] = dMB_aux->subblockpat[i];     
      }
    }

    do{
    }while(fifo_rp == *fifo_wp);
    fifo_rp++;
    *fifo_rp_tcsm = fifo_rp;

    POLLING_MDMA1_END();
    if((unsigned int)dMBsrc ==
       ((unsigned int)dMBbase + DEFAULT_VC1_MAX_MBNUM1*sizeof(VC1_MB_DecARGs))
       ){
      dMBsrc = dMBbase;
    }
   
    MDMA1_TRAN[3].TSA  = get_phy_addr_aux((uint32_t)dMBsrc);
    MDMA1_TRAN[3].TDA  = (uint32_t)dMB_aux;
    MDMA1_TRAN[3].STRD = MDMA_STRD(64, 64);
#ifdef JZC_DMA_OPT
    MDMA1_TRAN[3].UNIT = MDMA_UNIT(1,64,(sizeof(VC1_MB_DecARGs)-(6-dMB2->dmb_len)*64*sizeof(DCTELEM)));
#else
    MDMA1_TRAN[3].UNIT = MDMA_UNIT(1,64,(sizeof(VC1_MB_DecARGs)));
#endif
    dMBsrc++;

    SET_MDMA1_DHA((uint32_t)MDMA1_TRAN);
    MDMA1_RUN();


    XCHG2(VC1_aux_t.mc_chain[0], VC1_aux_t.mc_chain[1],XCHGtmp);

    XCHG2(MDMA1_TRAN, MDMA1_TRAN2,XCHGtmp);

    XCHG3(VC1_aux_t.recon_xchg1[0], VC1_aux_t.recon_xchg[0], VC1_aux_t.recon[0], XCHGtmp);
    XCHG3(VC1_aux_t.recon_xchg1[1], VC1_aux_t.recon_xchg[1], VC1_aux_t.recon[1], XCHGtmp);
    XCHG3(VC1_aux_t.recon_xchg1[2], VC1_aux_t.recon_xchg[2], VC1_aux_t.recon[2], XCHGtmp);
    //recon<-xchg1, xchg1<-xchg, xchg<-recon

    XCHG2(VC1_aux_t.recon_avg[0], VC1_aux_t.recon_xchg_avg[0],XCHGtmp);
    XCHG2(VC1_aux_t.recon_avg[1], VC1_aux_t.recon_xchg_avg[1],XCHGtmp);
    XCHG2(VC1_aux_t.recon_avg[2], VC1_aux_t.recon_xchg_avg[2],XCHGtmp);

    XCHG2(dMB_aux,dMB2,XCHGtmp);
    
 }// mbnum for   

    *task_done = TRUE;
    /*
      task_done: aux-cpu task done flag, only write by aux-cpu, only read by main-cpu
    */
      
  i_nop;  
  i_nop;    
  i_nop;      
  i_nop;  
  __asm__ __volatile__ ("wait");

}

#include "vc1_aux_idct_run.c"
#include "vc1_aux_mc_hw.c"
