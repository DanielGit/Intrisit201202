/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264)
 *
* $Id: vc1_mc_hw.c,v 1.3 2009/07/03 07:14:21 gwliu Exp $
 *
 ****************************************************************************/

#include "../jz4750e_mc.h"
#include "vc1_mc_hw.h"
#include "vc1_aux.h"  
#include "../jzmedia.h"

extern uint8_t *luty,*lutuv;
static void vc1_mc_init_aux()
{
  int i; 

  VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)(TCSM1_VADDR_UNCACHE(TCSM1_MC_CHAIN0));
  for(i=0; i<12; i++) {   
    /*luma:*/
    vc1_mc->NodeFlag = 0x80000000;
    vc1_mc->NodeHead = VC1_YUHEAD;
    vc1_mc->MCStatus = 0x0;
    vc1_mc++;
  }  

  vc1_mc = (VC1_MC_DesNode *)(TCSM1_VADDR_UNCACHE(TCSM1_MC_CHAIN1));
  for(i=0; i<12; i++) {
    /*luma:*/
    vc1_mc->NodeFlag = 0x80000000;
    vc1_mc->NodeHead = VC1_YUHEAD;
    vc1_mc->MCStatus = 0x0;
    vc1_mc++;
  } 

  /*MC init config*/
  SET_MC_END_FLAG();
  RUN_MC();
}


static  int mid_pred_mxu(int a, int b, int c)
{
#ifdef HAVE_CMOV
    int i=b;
    asm volatile(
        "cmp    %2, %1 \n\t"
        "cmovg  %1, %0 \n\t"
        "cmovg  %2, %1 \n\t"
        "cmp    %3, %1 \n\t"
        "cmovl  %3, %1 \n\t"
        "cmp    %1, %0 \n\t"
        "cmovg  %1, %0 \n\t"
        :"+&r"(i), "+&r"(a)
        :"r"(b), "r"(c)
    );
    return i;
#elif 0
    int t= (a-b)&((a-b)>>31);
    a-=t;
    b+=t;
    b-= (b-c)&((b-c)>>31);
    b+= (a-b)&((a-b)>>31);

    return b;
#else

    S32I2M(xr1,a);
    S32I2M(xr2,b);
    S32I2M(xr3,c);

    S32MIN(xr4,xr1,xr2);
    S32MAX(xr5,xr1,xr2);
    S32MAX(xr6,xr4,xr3);
    S32MIN(xr7,xr6,xr5);

    return S32M2I(xr7);
#endif
}

static int median4_mxu(int a, int b, int c, int d)
{
  S32I2M(xr1,a);
  S32I2M(xr2,b);
  S32I2M(xr3,c);
  S32I2M(xr4,d);

  S32MAX(xr5,xr1,xr2);
  S32MAX(xr6,xr3,xr4);

  S32MIN(xr7,xr1,xr2);
  S32MIN(xr8,xr3,xr4);

  S32MIN(xr1,xr5,xr6);
  S32MAX(xr2,xr7,xr8);

  D32ADD_AA(xr3,xr1,xr2,xr4);
  
  return S32M2I(xr3)/2;
}
   
/**
 * Copies a rectangular area of samples to a temporary buffer and replicates the boarder samples.
 * @param buf destination buffer
 * @param src source buffer
 * @param linesize number of bytes between 2 vertically adjacent samples in both the source and destination buffers
 * @param block_w width of block
 * @param block_h height of block
 * @param src_x x coordinate of the top left sample of the block in the source buffer
 * @param src_y y coordinate of the top left sample of the block in the source buffer
 * @param w width of the source buffer
 * @param h height of the source buffer
 */

__vc1_J1_tcsm_text__
void ff_emulated_edge_mc_hw(uint8_t *buf, uint8_t *src,
			    int buf_linesize, int src_linesize, int block_w, int block_h,
			    int src_x, int src_y, int w, int h){
  int x, y;
  int start_y, start_x, end_y, end_x;

  if(src_y>= h){
    src+= (h-1-src_y)*src_linesize;
    src_y=h-1;
  }else if(src_y<=-block_h){
    src+= (1-block_h-src_y)*src_linesize;
    src_y=1-block_h;
  }
  if(src_x>= w){
    src+= (w-1-src_x);
    src_x=w-1;
  }else if(src_x<=-block_w){
    src+= (1-block_w-src_x);
    src_x=1-block_w;
  }

  start_y= FFMAX(0, -src_y);
  start_x= FFMAX(0, -src_x);
  end_y= FFMIN(block_h, h-src_y);
  end_x= FFMIN(block_w, w-src_x); 

  // copy existing part
  for(y=start_y; y<end_y; y++){
    for(x=start_x; x<end_x; x++){
      buf[x + y*buf_linesize]= src[x + y*src_linesize];
    }
  }

  //top
  for(y=0; y<start_y; y++){
    for(x=start_x; x<end_x; x++){
      buf[x + y*buf_linesize]= buf[x + start_y*buf_linesize];
    }
  }

  //bottom
  for(y=end_y; y<block_h; y++){
    for(x=start_x; x<end_x; x++){
      buf[x + y*buf_linesize]= buf[x + (end_y-1)*buf_linesize];
    }
  }

  for(y=0; y<block_h; y++){
    //left
    for(x=0; x<start_x; x++){
      buf[x + y*buf_linesize]= buf[start_x + y*buf_linesize];
    }

    //right
    for(x=end_x; x<block_w; x++){
      buf[x + y*buf_linesize]= buf[end_x - 1 + y*buf_linesize];
    }
  }
}
__vc1_J1_tcsm_text__
void vc1_mc_1mv_emu(VC1Context *v, int dir, uint8_t *emu_edge_buf_mb)
{
  MpegEncContext *s = &v->s;

  uint8_t *srcY, *srcU, *srcV;
  int dxy, uvdxy, mx, my, uvmx, uvmy, src_x, src_y, uvsrc_x, uvsrc_y;
    
  if(!v->s.last_picture.data[0]) return;

  mx = s->mv[dir][0][0];
  my = s->mv[dir][0][1];

  // store motion vectors for further use in B frames
  if(s->pict_type == P_TYPE) {
    s->current_picture.motion_val[1][s->block_index[0]][0] = mx;
    s->current_picture.motion_val[1][s->block_index[0]][1] = my;
  }

  uvmx = (mx + ((mx & 3) == 3)) >> 1;
  uvmy = (my + ((my & 3) == 3)) >> 1;
  if(v->fastuvmc) {
    uvmx = uvmx + ((uvmx<0)?(uvmx&1):-(uvmx&1));
    uvmy = uvmy + ((uvmy<0)?(uvmy&1):-(uvmy&1));
  }
  if(!dir) {  
    srcY = s->last_picture.data[0];
    srcU = s->last_picture.data[1];  
    srcV = s->last_picture.data[2];
  } else {
    srcY = s->next_picture.data[0];
    srcU = s->next_picture.data[1];                  
    srcV = s->next_picture.data[2]; 
  }  
  
  src_x = s->mb_x * 16 + (mx >> 2);
  src_y = s->mb_y * 16 + (my >> 2);
  uvsrc_x = s->mb_x * 8 + (uvmx >> 2);
  uvsrc_y = s->mb_y * 8 + (uvmy >> 2);

  if(v->profile != PROFILE_ADVANCED){
    src_x   = av_clip(  src_x, -16, s->mb_width  * 16);
    src_y   = av_clip(  src_y, -16, s->mb_height * 16);
    uvsrc_x = av_clip(uvsrc_x,  -8, s->mb_width  *  8);
    uvsrc_y = av_clip(uvsrc_y,  -8, s->mb_height *  8);
  }else{
    src_x   = av_clip(  src_x, -17, s->avctx->coded_width);
    src_y   = av_clip(  src_y, -18, s->avctx->coded_height + 1);
    uvsrc_x = av_clip(uvsrc_x,  -8, s->avctx->coded_width  >> 1);
    uvsrc_y = av_clip(uvsrc_y,  -8, s->avctx->coded_height >> 1);
  }
    
  srcY += src_y * s->linesize + src_x;
  srcU += uvsrc_y * s->uvlinesize + uvsrc_x;
  srcV += uvsrc_y * s->uvlinesize + uvsrc_x;

  if((v->rangeredfrm || (v->mv_mode_emu == MV_PMODE_INTENSITY_COMP)
      || (unsigned)(src_x - s->mspel) > s->h_edge_pos - (mx&3) - 16 - s->mspel*3
      || (unsigned)(src_y - s->mspel) > s->v_edge_pos - (my&3) - 16 - s->mspel*3)){

    int i,j;

    srcY -= s->mspel * (1 + s->linesize);

    uint8_t *emu_edge_buf_y;
    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_y = emu_edge_buf_mb;
    emu_edge_buf_u = emu_edge_buf_y + EMU_BUF_1MV_Y_STRD;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;

    ff_emulated_edge_mc_hw(emu_edge_buf_y, srcY, EMU_Y_W, s->linesize,
			   17+s->mspel*2, 17+s->mspel*2,
			   src_x - s->mspel, src_y - s->mspel,
			   s->h_edge_pos, s->v_edge_pos);

    ff_emulated_edge_mc_hw(emu_edge_buf_u, srcU, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    ff_emulated_edge_mc_hw(emu_edge_buf_v, srcV, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    /* if we deal with range reduction we need to scale source blocks */
    if(v->rangeredfrm) {
      int i, j;
      uint8_t *src, *src2;
      uint8_t mspel = s->mspel * 2;
      src = emu_edge_buf_y;       
      for(j = 0; j < 17 + mspel; j++) {
	for(i = 0; i < 17 + mspel; i++)
	  src[i] = ((src[i] - 128) >> 1) + 128;

	src += EMU_Y_W;
      }

      src = emu_edge_buf_u;
      src2 = emu_edge_buf_v;
      for(j = 0; j < 9; j++) {
	for(i = 0; i < 9; i++) {
	  src[i] = ((src[i] - 128) >> 1) + 128;
	  src2[i] = ((src2[i] - 128) >> 1) + 128;
	}

	src += EMU_UV_W;
	src2 += EMU_UV_W;
      }
    }
    /* if we deal with intensity compensation we need to scale source blocks */
    if(v->mv_mode_emu == MV_PMODE_INTENSITY_COMP) {
      int i, j;
      uint8_t *src, *src2;
      uint8_t mspel = s->mspel * 2; 
      src = emu_edge_buf_y;
      for(j = 0; j < 17 + mspel; j++) {
	for(i = 0; i < 17 + mspel; i++)
	  src[i] = luty[src[i]];

	src += EMU_Y_W;
      }

      src = emu_edge_buf_u;
      src2 = emu_edge_buf_v;
      for(j = 0; j < 9; j++) {
	for(i = 0; i < 9; i++) {
	  src[i] = lutuv[src[i]];
	  src2[i] = lutuv[src2[i]];
	}
	src += EMU_UV_W;
	src2 += EMU_UV_W;
      }
    }

    {
      int i;
      char *va = (uint32_t)emu_edge_buf_y;
      int emu_buf_cache_len = ((EMU_BUF_STRD + 31) >> 5) + 1;
      for (i=0; i<emu_buf_cache_len; i++){
	i_cache(0x19,va,0);
	va += 32;
      }
      i_sync();
    }


  }
}

void vc1_mc_4mv_luma_emu(VC1Context *v, int n, uint8_t *emu_edge_buf_mb)
{
  MpegEncContext *s = &v->s;

  uint8_t *srcY;
  int dxy, mx, my, src_x, src_y;
  int off;                       
  
  
  if(!v->s.last_picture.data[0]) return;

  mx = s->mv[0][n][0];
  my = s->mv[0][n][1];

  srcY = s->last_picture.data[0];
    
  off = s->linesize * 4 * (n&2) + (n&1) * 8;

  src_x = s->mb_x * 16 + (n&1) * 8 + (mx >> 2);
  src_y = s->mb_y * 16 + (n&2) * 4 + (my >> 2);

  if(v->profile != PROFILE_ADVANCED){
    src_x   = av_clip(  src_x, -16, s->mb_width  * 16);
    src_y   = av_clip(  src_y, -16, s->mb_height * 16);
  }else{
    src_x   = av_clip(  src_x, -17,  s->avctx->coded_width);
    src_y   = av_clip(  src_y, -18,  s->avctx->coded_height + 1);
  }

  int i,j,va;
  srcY += src_y * s->linesize + src_x;

  if(v->rangeredfrm || (v->mv_mode == MV_PMODE_INTENSITY_COMP)
     || (unsigned)(src_x - s->mspel) > s->h_edge_pos - (mx&3) - 8 - s->mspel*2
     || (unsigned)(src_y - s->mspel) > s->v_edge_pos - (my&3) - 8 - s->mspel*2){
 
    srcY -= s->mspel * (1 + s->linesize);
    
    uint8_t *emu_edge_buf_y = emu_edge_buf_mb;
   
    ff_emulated_edge_mc_hw(emu_edge_buf_y, srcY, EMU_Y_4MV_W, s->linesize,
			   9+s->mspel*2, 9+s->mspel*2,
			   src_x - s->mspel, src_y - s->mspel,
			   s->h_edge_pos, s->v_edge_pos);

    /* if we deal with range reduction we need to scale source blocks */
    if(v->rangeredfrm) {
      int i, j;
      uint8_t *src;  
      uint8_t mspel = s->mspel * 2;
      src = emu_edge_buf_y;
      for(j = 0; j < 9 + mspel; j++) {
	for(i = 0; i < 9 + mspel; i++)
	  src[i] = ((src[i] - 128) >> 1) + 128;

	src += EMU_Y_4MV_W;
      }
    }
    /* if we deal with intensity compensation we need to scale source blocks */
    if(v->mv_mode == MV_PMODE_INTENSITY_COMP) {
      int i, j;
      uint8_t *src;
      uint8_t mspel = s->mspel * 2;
      src = emu_edge_buf_y;
      for(j = 0; j < 9 + mspel; j++) {
	for(i = 0; i < 9 + mspel; i++)
	  src[i] = luty[src[i]];

	src += EMU_Y_4MV_W;
      }
    } 
    {
      int i;
      char *va = (uint32_t)emu_edge_buf_y;
      int emu_buf_cache_len = ((EMU_BUF_4MV_Y_STRD + 31) >> 5) + 1;
      for (i=0; i<emu_buf_cache_len; i++){
	i_cache(0x19,va,0);
	va += 32;
      }
      i_sync();
    }

  }
}


//__vc1_J1_tcsm_text__
void vc1_mc_4mv_chroma_emu(VC1Context *v, uint8_t *emu_edge_buf_mb)
{
  MpegEncContext *s = &v->s;

  uint8_t *srcU, *srcV;
  int uvdxy, uvmx, uvmy, uvsrc_x, uvsrc_y;
  int i,j, idx, tx = 0, ty = 0;
  int mvx[4], mvy[4], intra[4];
  static const int count[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
 
  if(!v->s.last_picture.data[0])return;
  if(s->flags & CODEC_FLAG_GRAY) return;

  for(i = 0; i < 4; i++) {
    mvx[i] = s->mv[0][i][0];
    mvy[i] = s->mv[0][i][1];  
    intra[i] = dMB_tcsm->vc1_blk_index[i];
  }

  /* calculate chroma MV vector from four luma MVs */
  idx = (intra[3] << 3) | (intra[2] << 2) | (intra[1] << 1) | intra[0];
  if(!idx) { // all blocks are inter
    tx = median4_mxu(mvx[0], mvx[1], mvx[2], mvx[3]);
    ty = median4_mxu(mvy[0], mvy[1], mvy[2], mvy[3]);
  } else if(count[idx] == 1) { // 3 inter blocks
    switch(idx) {
    case 0x1:
      tx = mid_pred_mxu(mvx[1], mvx[2], mvx[3]);
      ty = mid_pred_mxu(mvy[1], mvy[2], mvy[3]);
      break;
    case 0x2:
      tx = mid_pred_mxu(mvx[0], mvx[2], mvx[3]);
      ty = mid_pred_mxu(mvy[0], mvy[2], mvy[3]);
      break;
    case 0x4:
      tx = mid_pred_mxu(mvx[0], mvx[1], mvx[3]);
      ty = mid_pred_mxu(mvy[0], mvy[1], mvy[3]);
      break;
    case 0x8:
      tx = mid_pred_mxu(mvx[0], mvx[1], mvx[2]);
      ty = mid_pred_mxu(mvy[0], mvy[1], mvy[2]);
      break;
    }
  } else if(count[idx] == 2) {
    int t1 = 0, t2 = 0;
    for(i=0; i<3;i++) if(!intra[i]) {t1 = i; break;}
    for(i= t1+1; i<4; i++)if(!intra[i]) {t2 = i; break;}
    tx = (mvx[t1] + mvx[t2]) / 2;
    ty = (mvy[t1] + mvy[t2]) / 2;
  } else {  
    s->current_picture.motion_val[1][s->block_index[0]][0] = 0;
    s->current_picture.motion_val[1][s->block_index[0]][1] = 0;
    return; //no need to do MC for inter blocks
  }
  s->current_picture.motion_val[1][s->block_index[0]][0] = tx;
  s->current_picture.motion_val[1][s->block_index[0]][1] = ty;

  uvmx = (tx + ((tx&3) == 3)) >> 1;
  uvmy = (ty + ((ty&3) == 3)) >> 1;
  if(v->fastuvmc) {
    uvmx = uvmx + ((uvmx<0)?(uvmx&1):-(uvmx&1));
    uvmy = uvmy + ((uvmy<0)?(uvmy&1):-(uvmy&1));
  }

  uvsrc_x = s->mb_x * 8 + (uvmx >> 2);
  uvsrc_y = s->mb_y * 8 + (uvmy >> 2);

  if(v->profile != PROFILE_ADVANCED){
    uvsrc_x = av_clip(uvsrc_x,  -8, s->mb_width  *  8);
    uvsrc_y = av_clip(uvsrc_y,  -8, s->mb_height *  8);
  }else{
    uvsrc_x = av_clip(uvsrc_x,  -8,  s->avctx->coded_width  >> 1);
    uvsrc_y = av_clip(uvsrc_y,  -8,  s->avctx->coded_height >> 1);
  }

  srcU = s->last_picture.data[1] + uvsrc_y * s->uvlinesize + uvsrc_x;
  srcV = s->last_picture.data[2] + uvsrc_y * s->uvlinesize + uvsrc_x;

  if(v->rangeredfrm || (v->mv_mode == MV_PMODE_INTENSITY_COMP)
     || (unsigned)uvsrc_x > (s->h_edge_pos >> 1) - 9
     || (unsigned)uvsrc_y > (s->v_edge_pos >> 1) - 9){

    int i,j,va;     

    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_u = emu_edge_buf_mb;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;
    
    ff_emulated_edge_mc_hw(emu_edge_buf_u, srcU, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    ff_emulated_edge_mc_hw(emu_edge_buf_v, srcV, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);
                     
    /* if we deal with range reduction we need to scale source blocks */
    if(v->rangeredfrm) {
      int i, j;
      uint8_t *src, *src2;
      src = emu_edge_buf_u;
      src2 = emu_edge_buf_v;
      for(j = 0; j < 9; j++) {
	for(i = 0; i < 9; i++) {
	  src[i] = ((src[i] - 128) >> 1) + 128;
	  src2[i] = ((src2[i] - 128) >> 1) + 128;
	}
	src += EMU_UV_W;
	src2 +=  EMU_UV_W;
      }
    }  
    /* if we deal with intensity compensation we need to scale source blocks */
    if(v->mv_mode == MV_PMODE_INTENSITY_COMP) {
      int i, j;
      uint8_t *src, *src2;
      src = emu_edge_buf_u;
      src2 = emu_edge_buf_v;
      for(j = 0; j < 9; j++) {
	for(i = 0; i < 9; i++) {
	  src[i] = lutuv[src[i]];
	  src2[i] = lutuv[src2[i]];
	}
	src +=  EMU_UV_W;
	src2 +=  EMU_UV_W;
      }
    }
    {
      int i;
      char *va = (uint32_t)emu_edge_buf_u;
      int emu_buf_cache_len = ((2*EMU_BUF_UV_STRD + 31) >> 5) + 1;
      for (i=0; i<emu_buf_cache_len; i++){
	i_cache(0x19,va,0);
	va += 32;
      }
      i_sync();
    }  

  }
}

//__vc1_J1_tcsm_text__
void vc1_interp_mc_emu(VC1Context *v, uint8_t *emu_edge_buf_mb)
{
    MpegEncContext *s = &v->s;    
    uint8_t *srcY, *srcU, *srcV;
    int dxy, uvdxy, mx, my, uvmx, uvmy, src_x, src_y, uvsrc_x, uvsrc_y;

    if(!s->next_picture.data[0])return;

    mx = s->mv[1][0][0];
    my = s->mv[1][0][1];

    uvmx = (mx + ((mx & 3) == 3)) >> 1;
    uvmy = (my + ((my & 3) == 3)) >> 1;
    if(v->fastuvmc) {
        uvmx = uvmx + ((uvmx<0)?-(uvmx&1):(uvmx&1));
        uvmy = uvmy + ((uvmy<0)?-(uvmy&1):(uvmy&1));
    }
    srcY = s->next_picture.data[0];
    srcU = s->next_picture.data[1];                  
    srcV = s->next_picture.data[2]; 
 
    src_x = s->mb_x * 16 + (mx >> 2);
    src_y = s->mb_y * 16 + (my >> 2);
    uvsrc_x = s->mb_x * 8 + (uvmx >> 2);
    uvsrc_y = s->mb_y * 8 + (uvmy >> 2);

    if(v->profile != PROFILE_ADVANCED){
        src_x   = av_clip(  src_x, -16, s->mb_width  * 16);
        src_y   = av_clip(  src_y, -16, s->mb_height * 16);
        uvsrc_x = av_clip(uvsrc_x,  -8, s->mb_width  *  8);
        uvsrc_y = av_clip(uvsrc_y,  -8, s->mb_height *  8);
    }else{
        src_x   = av_clip(  src_x, -17,  s->avctx->coded_width);
        src_y   = av_clip(  src_y, -18,  s->avctx->coded_height + 1);
        uvsrc_x = av_clip(uvsrc_x,  -8,  s->avctx->coded_width  >> 1);
        uvsrc_y = av_clip(uvsrc_y,  -8,  s->avctx->coded_height >> 1);
    }
   
    srcY += src_y * s->linesize + src_x;
    srcU += uvsrc_y * s->uvlinesize + uvsrc_x;
    srcV += uvsrc_y * s->uvlinesize + uvsrc_x;

    /* for grayscale we should not try to read from unknown area */
    if(s->flags & CODEC_FLAG_GRAY) {
        srcU = s->edge_emu_buffer + 18 * s->linesize;
        srcV = s->edge_emu_buffer + 18 * s->linesize;
    }

    if(v->rangeredfrm
       || (unsigned)src_x > s->h_edge_pos - (mx&3) - 16
       || (unsigned)src_y > s->v_edge_pos - (my&3) - 16){

    int i,j;
    srcY -= s->mspel * (1 + s->linesize);

    uint8_t *emu_edge_buf_y;
    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_y = emu_edge_buf_mb;
    emu_edge_buf_u = emu_edge_buf_y + EMU_BUF_1MV_Y_STRD;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;

    ff_emulated_edge_mc_hw(emu_edge_buf_y, srcY, EMU_Y_W, s->linesize,
			   17+s->mspel*2, 17+s->mspel*2,
			   src_x - s->mspel, src_y - s->mspel,
			   s->h_edge_pos, s->v_edge_pos);

    ff_emulated_edge_mc_hw(emu_edge_buf_u, srcU, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    ff_emulated_edge_mc_hw(emu_edge_buf_v, srcV, EMU_UV_W, s->uvlinesize,
			   8+1, 8+1,
			   uvsrc_x, uvsrc_y,
			   s->h_edge_pos >> 1, s->v_edge_pos >> 1);

    /* if we deal with range reduction we need to scale source blocks */
    if(v->rangeredfrm) {
      int i, j;
      uint8_t *src, *src2;
      src = emu_edge_buf_y;
      for(j = 0; j < 17 + s->mspel*2; j++) {
	for(i = 0; i < 17 + s->mspel*2; i++)
	  src[i] = ((src[i] - 128) >> 1) + 128;

	src += EMU_Y_W;
      }

      src = emu_edge_buf_u;
      src2 = emu_edge_buf_v;
      for(j = 0; j < 9; j++) {
	for(i = 0; i < 9; i++) {
	  src[i] = ((src[i] - 128) >> 1) + 128;
	  src2[i] = ((src2[i] - 128) >> 1) + 128;
	}

	src += EMU_UV_W;
	src2 += EMU_UV_W;
      }
    }

    {
      int i;
      char *va = (uint32_t)emu_edge_buf_y;
      int emu_buf_cache_len = ((EMU_BUF_STRD + 31) >> 5) + 1;
      for (i=0; i<emu_buf_cache_len; i++){
	i_cache(0x19,va,0);
	va += 32;
      }
      i_sync();
    }
       
    }
}
