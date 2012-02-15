static const int alpha_old_tab[52] = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   4,   4,   5,   6,
  7,   8,   9,  10,  12,  13,  15,  17,  20,  22,
  25,  28,  32,  36,  40,  45,  50,  56,  63,  71,
  80,  90, 101, 113, 127, 144, 162, 182, 203, 226,
  255, 255
};

static const int beta_old_tab[52] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  2,  2,  2,  3,
   3,  3,  3,  4,  4,  4,  6,  6,  7,  7,
   8,  8,  9,  9, 10, 10, 11, 11, 12, 12,
  13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
  18, 18
};

static const int tc0_old_tab[52][3] = {
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

#include "../jz4750e_dblk.h"

#define PICT_FRAME         3
#define B_TYPE         3
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

void decode_init_dblk_hw(H264Context *h)
{
  unsigned int btmp;
  int i,j;
  // alpha beta lut
  btmp = (beta_old_tab[17] << 24) + (alpha_old_tab[17] << 16);
  SET_AB_LUT(0, btmp);
  for (i = 1; i < 18; i++) { 
    btmp = (beta_old_tab[17 + i*2] << 24) + (alpha_old_tab[17 + i*2] << 16) +
      (beta_old_tab[16 + i*2] << 8) + alpha_old_tab[16 + i*2]; 
    SET_AB_LUT((i*4), btmp);
  }
  // tc0 lut
  for (i = 0; i < 18; i++) {
    btmp = (tc0_old_tab[17+i*2][2] << 25) + (tc0_old_tab[17+i*2][1] << 20) +
      (tc0_old_tab[17+i*2][0] << 16) +
      (tc0_old_tab[16+i*2][2] << 9) + (tc0_old_tab[16+i*2][1] << 4) +
      tc0_old_tab[16+i*2][0];
    SET_TC0_LUT((i*4), btmp);
  }

  dat_nod_yuv_t *dat_nod_yuv = (dat_nod_yuv_t *)TCSM1_VADDR_UNCACHE(TCSM1_DBLK_DES_BUF0);

  dat_nod_yuv->nod_hd0_y     = NODE_CMD0;
  dat_nod_yuv->nodhead_y     = 0x340E004D;
  dat_nod_yuv->dat_inadr_y   = TCSM1_RECON_YBUF0;
  dat_nod_yuv->dat_instrd_y  = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_y  = TCSM1_DBLK_YBUF1;
  dat_nod_yuv->dat_outstrd_y = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_y   = TCSM_DBLK_TOP_4LINE_YBUF0;
  dat_nod_yuv->dat_upstrd_y  = RECON_YBUF_LEN;
  dat_nod_yuv->dat_ctrl_y    = 0x1;
  
  dat_nod_yuv->nod_hd0_u = NODE_CMD0;
  dat_nod_yuv->nodhead_u = 0x340E004D;
  dat_nod_yuv->dat_inadr_u = TCSM1_RECON_UBUF0;
  dat_nod_yuv->dat_instrd_u = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_u = TCSM1_DBLK_UBUF1;
  dat_nod_yuv->dat_outstrd_u = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_u = TCSM_DBLK_TOP_4LINE_UBUF0;
  dat_nod_yuv->dat_upstrd_u = RECON_UVBUF_LEN;
  dat_nod_yuv->dat_bsv23_u = 0;
  dat_nod_yuv->dat_bsh23_u = 0;
  dat_nod_yuv->dat_ctrl_u = 0x1;   
  
  dat_nod_yuv->nod_hd0_v = NODE_CMD0;
  dat_nod_yuv->nodhead_v = 0x340E0048;
  dat_nod_yuv->dat_inadr_v = TCSM1_RECON_VBUF0;
  dat_nod_yuv->dat_instrd_v = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_v = TCSM1_DBLK_VBUF1;
  dat_nod_yuv->dat_outstrd_v = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_v = TCSM_DBLK_TOP_4LINE_VBUF0;
  dat_nod_yuv->dat_upstrd_v = RECON_UVBUF_LEN;
  dat_nod_yuv->dat_bsv23_v = 0;
  dat_nod_yuv->dat_bsh23_v = 0;
  dat_nod_yuv->dat_ctrl_v = 0x1;
  
  dat_nod_yuv->anodhead = NODE_CMD0;
  dat_nod_yuv->anodcmd = 0x0001006C;
  dat_nod_yuv->anodinfa = TCSM1_H264_DBLK_INFAR;
  dat_nod_yuv->anodinfd = TCSM1_H264_DBLK_INFDA;

  dat_nod_yuv = (dat_nod_yuv_t *)TCSM1_VADDR_UNCACHE(TCSM1_DBLK_DES_BUF1);

  dat_nod_yuv->nod_hd0_y     = NODE_CMD0;
  dat_nod_yuv->nodhead_y     = 0x340E004D;
  dat_nod_yuv->dat_inadr_y   = TCSM1_RECON_YBUF1;
  dat_nod_yuv->dat_instrd_y  = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_y  = TCSM1_DBLK_YBUF0;
  dat_nod_yuv->dat_outstrd_y = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_y   = TCSM_DBLK_TOP_4LINE_YBUF1;
  dat_nod_yuv->dat_upstrd_y  = RECON_YBUF_LEN;
  dat_nod_yuv->dat_ctrl_y    = 0x1;
  
  dat_nod_yuv->nod_hd0_u = NODE_CMD0;
  dat_nod_yuv->nodhead_u = 0x340E004D;
  dat_nod_yuv->dat_inadr_u = TCSM1_RECON_UBUF1;
  dat_nod_yuv->dat_instrd_u = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_u = TCSM1_DBLK_UBUF0;
  dat_nod_yuv->dat_outstrd_u = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_u = TCSM_DBLK_TOP_4LINE_UBUF1;
  dat_nod_yuv->dat_upstrd_u = RECON_UVBUF_LEN;
  dat_nod_yuv->dat_bsv23_u = 0;
  dat_nod_yuv->dat_bsh23_u = 0;
  dat_nod_yuv->dat_ctrl_u = 0x1;   
  
  dat_nod_yuv->nod_hd0_v = NODE_CMD0;
  dat_nod_yuv->nodhead_v = 0x340E0048;
  dat_nod_yuv->dat_inadr_v = TCSM1_RECON_VBUF1;
  dat_nod_yuv->dat_instrd_v = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_outadr_v = TCSM1_DBLK_VBUF0;
  dat_nod_yuv->dat_outstrd_v = RECON_BUF_STRIDE;
  dat_nod_yuv->dat_upadr_v = TCSM_DBLK_TOP_4LINE_VBUF1;
  dat_nod_yuv->dat_upstrd_v = RECON_UVBUF_LEN;
  dat_nod_yuv->dat_bsv23_v = 0;
  dat_nod_yuv->dat_bsh23_v = 0;
  dat_nod_yuv->dat_ctrl_v = 0x1;
  
  dat_nod_yuv->anodhead = NODE_CMD0;
  dat_nod_yuv->anodcmd = 0x0001006C;
  dat_nod_yuv->anodinfa = TCSM1_H264_DBLK_INFAR;
  dat_nod_yuv->anodinfd = TCSM1_H264_DBLK_INFDA;

  unsigned int *tmp=(unsigned int *)TCSM1_VADDR_UNCACHE(TCSM1_H264_DBLK_INFAR);
  *tmp = TCSM1_H264_DBLK_INFDA;

  SET_DBLK_CTRL(1,0,0); // set dblk end flag
}

#if 0
void filter_mb_dblk(H264_Frame_GlbARGs *dFRM, H264_MB_DecARGs * dMB_aux, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr,
		    unsigned int linesize, unsigned int uvlinesize) 
{

  int mb_xy= mb_x + mb_y*dFRM->mb_stride;
  int mb_type = dMB_aux->mb_type;
  int mvy_limit = IS_INTERLACED(mb_type) ? 2 : 4;
  int dir;

  unsigned int leften = 0;
  unsigned int upen = 0;
  unsigned int bsary[4] = {0,0,0,0};
  uint8_t *bsptr = (uint8_t *)bsary;

  /* FIXME: A given frame may occupy more than one position in
   * the reference list. So ref2frm should be populated with
   * frame numbers, not indices. */
  static const int ref2frm[34] = {-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

  //for sufficiently low qp, filtering wouldn't do anything
  //this is a conservative estimate: could also check beta_offset and more accurate chroma_qp
  if( 1 ){
    int qp_thresh = 15 - dMB_aux->slice_alpha_c0_offset - FFMAX(0, FFMAX(dFRM->chroma_qp_index_offset[0], dFRM->chroma_qp_index_offset[1]));
    int qp = dMB_aux->qscale;
    if(qp <= qp_thresh
       && (mb_x == 0 || ((qp + dMB_aux->qp_left + 1)>>1) <= qp_thresh) 
       && (mb_y == 0 || ((qp + dMB_aux->qp_top + 1)>>1) <= qp_thresh)){
      return;
    }
  }

  /* dir : 0 -> vertical edge, 1 -> horizontal edge */
  for( dir = 0; dir < 2; dir++ )
    {
      int edge;
      const int mbm_type = dir == 0 ? dMB_aux->left_mb_type : dMB_aux->top_mb_type;
      int start = dMB_aux->dblk_start[dir];

      const int edges = (mb_type & (MB_TYPE_16x16|MB_TYPE_SKIP)) == (MB_TYPE_16x16|MB_TYPE_SKIP) ? 1 : 4;
      // how often to recheck mv-based bS when iterating between edges
      const int mask_edge = (mb_type & (MB_TYPE_16x16 | (MB_TYPE_16x8 << dir))) ? 3 : (mb_type & (MB_TYPE_8x16 >> dir)) ? 1 : 0;
      // how often to recheck mv-based bS when iterating along each edge
      const int mask_par0 = mb_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir));

      leften = leften | ((dir == 0) && (start == 0)); 
      upen = (dir == 1) && (start == 0);
      bsptr = (dir) ? (char *)(&bsary[2]) : (char *)bsary;
      bsptr = bsptr + (start<<1);

      /* Calculate bS */
      for( edge = start; edge < edges; edge++ ) {
	const int mbn_type = edge > 0 ? mb_type : mbm_type;
	int16_t bS[4];
	int qp;

	if( (edge&1) && IS_8x8DCT(mb_type) )
	  continue;

	if( IS_INTRA(mb_type) ||
	    IS_INTRA(mbn_type) ) {
	  int value;
	  if (edge == 0) {
	    if ( (!IS_INTERLACED(mb_type) && !IS_INTERLACED(mbm_type))
		 || (( (dFRM->picture_structure != PICT_FRAME)) && (dir == 0))
		 ) {
	      value = 4;
	    } else {
	      value = 3;
	    }
	  } else {
	    value = 3;
	  }
	  bS[0] = bS[1] = bS[2] = bS[3] = value;
	} else {
	  int i, l;
	  int mv_done;

	  if( edge & mask_edge ) {
	    bS[0] = bS[1] = bS[2] = bS[3] = 0;
	    mv_done = 1;
	  }
	  else if( 0 && IS_INTERLACED(mb_type ^ mbn_type)) {
	    bS[0] = bS[1] = bS[2] = bS[3] = 1;
	    mv_done = 1;
	  }
	  else if( mask_par0 && (edge || (mbn_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir)))) ) {
	    int b_idx= 8 + 4 + edge * (dir ? 8:1);
	    int bn_idx= b_idx - (dir ? 8:1);
	    int v = 0;
	    for( l = 0; !v && l < 1 + (dMB_aux->slice_type == B_TYPE); l++ ) {
	      v |= ref2frm[dMB_aux->ref_cache[l][b_idx]+2] != ref2frm[dMB_aux->ref_cache[l][bn_idx]+2] ||
		FFABS( dMB_aux->mv_cache[l][b_idx][0] - dMB_aux->mv_cache[l][bn_idx][0] ) >= 4 ||
		FFABS( dMB_aux->mv_cache[l][b_idx][1] - dMB_aux->mv_cache[l][bn_idx][1] ) >= mvy_limit;
	    }
	    bS[0] = bS[1] = bS[2] = bS[3] = v;
	    mv_done = 1;
	  }
	  else
	    mv_done = 0;

	  //lower filter resolution
	  for( i = 0; i < 4; i++ ) {
	    int x = dir == 0 ? edge : i;
	    int y = dir == 0 ? i    : edge;
	    int b_idx= 8 + 4 + x + 8*y;
	    int bn_idx= b_idx - (dir ? 8:1);

	    if( dMB_aux->non_zero_count_cache[b_idx] != 0 ||
		dMB_aux->non_zero_count_cache[bn_idx] != 0 ) {
	      bS[i] = 2;
	    }
	    else if(!mv_done)
	      {
		bS[i] = 0;
		for( l = 0; l < 1 + (dMB_aux->slice_type == B_TYPE); l++ ) {
		  if( ref2frm[dMB_aux->ref_cache[l][b_idx]+2] != ref2frm[dMB_aux->ref_cache[l][bn_idx]+2] ||
		      FFABS( dMB_aux->mv_cache[l][b_idx][0] - dMB_aux->mv_cache[l][bn_idx][0] ) >= 4 ||
		      FFABS( dMB_aux->mv_cache[l][b_idx][1] - dMB_aux->mv_cache[l][bn_idx][1] ) >= mvy_limit ) {
		    bS[i] = 1;
		    break;
		  }
		}
	      }
	  }
	}

	*bsptr = bS[0] | (bS[1]<<4);
	bsptr++;	
	*bsptr = bS[2] | (bS[3]<<4);
	bsptr++;	

      } // edge
    } // dir

  // CFG
  int qp_left,   qp_curr,   qp_up,  qp;
  int qp_left_u, qp_curr_u, qp_up_u,qp_u;
  int qp_left_v, qp_curr_v, qp_up_v,qp_v;
  int ofst_a, ofst_b;

  qp_left = ( dMB_aux->qscale + dMB_aux->qp_left + 1 ) >> 1;
  qp_curr = dMB_aux->qscale;
  qp_up = ( dMB_aux->qscale + dMB_aux->qp_top + 1 ) >> 1;
  qp = (qp_up << 14) + (qp_left << 7) + qp_curr;
  qp_left_u = ( dMB_aux->chroma_qp[0] + dMB_aux->chroma_qp_left[0] + 1 ) >> 1;
  qp_curr_u = ( dMB_aux->chroma_qp[0] + dMB_aux->chroma_qp[0] + 1 ) >> 1;
  qp_up_u = ( dMB_aux->chroma_qp[0] + dMB_aux->chroma_qp_top[0] + 1 ) >> 1;
  qp_u = (qp_up_u << 14) + (qp_left_u << 7) + qp_curr_u;
  qp_left_v = ( dMB_aux->chroma_qp[1] + dMB_aux->chroma_qp_left[1] + 1 ) >> 1;
  qp_curr_v = ( dMB_aux->chroma_qp[1] + dMB_aux->chroma_qp[1] + 1 ) >> 1;
  qp_up_v = ( dMB_aux->chroma_qp[1] + dMB_aux->chroma_qp_top[1] + 1 ) >> 1;
  qp_v = (qp_up_v << 14) + (qp_left_v << 7) + qp_curr_v;

  ofst_a = dMB_aux->slice_alpha_c0_offset;
  ofst_b = dMB_aux->slice_beta_offset;

  filter_mb_hw(img_y, img_cb, img_cr, linesize, uvlinesize, qp, qp_u, qp_v, bsary, upen, leften, ofst_a, ofst_b);

}

void filter_mb_hw(unsigned char *img_y, unsigned char *img_cb, unsigned char *img_cr, unsigned int linesize, unsigned int uvlinesize,
		  int qp, int qp_u, int qp_v, unsigned int *bsary, unsigned int upen, unsigned int leften, int ofst_a, int ofst_b)
{ 
  int i,j;

  dat_nod_yuv_t *dat_nod_yuv = (dat_nod_yuv_t *)TCSM1_DBLK_DES_BUF0;
  dat_nod_yuv->dat_bsv01_y = bsary[0];
  dat_nod_yuv->dat_bsv23_y = bsary[1];
  dat_nod_yuv->dat_bsh01_y = bsary[2];
  dat_nod_yuv->dat_bsh23_y = bsary[3];
  dat_nod_yuv->dat_ofstab_y = (ofst_a << 24) + (ofst_b << 16) + 0x8001;
  dat_nod_yuv->dat_qp_y = qp;
  dat_nod_yuv->dat_fmt_y = 0xB01 + (upen << 7) + (leften << 6);//y
  dat_nod_yuv->dat_bsv01_u = (bsary[0] & 0x0000FFFF) | (bsary[1] << 16); 
  dat_nod_yuv->dat_bsh01_u = (bsary[2] & 0x0000FFFF) | (bsary[3] << 16);
  dat_nod_yuv->dat_ofstab_u = dat_nod_yuv->dat_ofstab_y;
  dat_nod_yuv->dat_qp_u = qp_u;
  dat_nod_yuv->dat_fmt_u = 0x1501 + (upen << 7) + (leften << 6);//u
  dat_nod_yuv->dat_bsv01_v = dat_nod_yuv->dat_bsv01_u; 
  dat_nod_yuv->dat_bsh01_v = dat_nod_yuv->dat_bsh01_u;
  dat_nod_yuv->dat_ofstab_v = dat_nod_yuv->dat_ofstab_u;
  dat_nod_yuv->dat_qp_v = qp_v;
  dat_nod_yuv->dat_fmt_v = 0x1101 + (upen << 7) + (leften << 6);//v

  unsigned int *tcsm_p;
  unsigned int *tcsm_p1;
  unsigned int *img_p;
  unsigned int *img_p1;
  int start = -1; //leften ? -1 : 0;

  //data in
#if 0
  tcsm_p=(unsigned int *)TCSM_DBLK_YBUF0;
  img_p=(unsigned int *)img_y;
  for (i = 0; i < 16; i++) {
    for (j = start; j < 4; j++) {
      tcsm_p[j] = img_p[j];
    }
    img_p = img_p + (linesize>>2);
    tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
  }
  tcsm_p=(unsigned int *)TCSM_DBLK_UBUF0;
  tcsm_p1=(unsigned int *)TCSM_DBLK_VBUF0;
  img_p=(unsigned int *)img_cb;
  img_p1=(unsigned int *)img_cr;
  for (i = 0; i < 8; i++) {
    for (j = start; j < 2; j++) {
      tcsm_p[j] = img_p[j];
      tcsm_p1[j] = img_p1[j];
    }
    img_p = img_p + (uvlinesize>>2);
    img_p1 = img_p1 + (uvlinesize>>2);
    tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
    tcsm_p1 = tcsm_p1 + (RECON_BUF_STRIDE>>2);
  }//curr in

  if (upen) {
    tcsm_p = (unsigned int *)TCSM_DBLK_TOP_4LINE_YBUF;
    img_p = (unsigned int *)(img_y - linesize*4);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
	tcsm_p[j] = img_p[j];
      }
      img_p = img_p + (linesize>>2);
      tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
    }
    tcsm_p=(unsigned int *)TCSM_DBLK_TOP_4LINE_UBUF;
    tcsm_p1=(unsigned int *)TCSM_DBLK_TOP_4LINE_VBUF;
    img_p=(unsigned int *)(img_cb - uvlinesize*4);
    img_p1=(unsigned int *)(img_cr - uvlinesize*4);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 2; j++) {
	tcsm_p[j] = img_p[j];
	tcsm_p1[j] = img_p1[j];
      }
      img_p = img_p + (uvlinesize>>2);
      img_p1 = img_p1 + (uvlinesize>>2);
      tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
      tcsm_p1 = tcsm_p1 + (RECON_BUF_STRIDE>>2);
    }
  }//up in
#endif

  //HW TIME
  SET_DCS_DBLK(0x1);

  volatile unsigned int *infar = (unsigned int *)TCSM1_H264_DBLK_INFAR;
  while ((*infar)!= TCSM1_H264_DBLK_INFDA) {
  }
  *infar = 0;

#if 0
  tcsm_p=(unsigned int *)TCSM_DBLK_YBUF0;
  img_p=(unsigned int *)img_y;
  //data out
  for (i = 0; i < 16; i++) {
    for (j = start; j < 4; j++) {
      img_p[j] = tcsm_p[j];
    }
    img_p = img_p + (linesize>>2);
    tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
  }
  tcsm_p=(unsigned int *)TCSM_DBLK_UBUF0;
  tcsm_p1=(unsigned int *)TCSM_DBLK_VBUF0;
  img_p=(unsigned int *)img_cb;
  img_p1=(unsigned int *)img_cr;
  for (i = 0; i < 8; i++) {
    for (j = start; j < 2; j++) {
      img_p[j]  = tcsm_p[j] ;
      img_p1[j] = tcsm_p1[j];
    }
    img_p = img_p + (uvlinesize>>2);
    img_p1 = img_p1 + (uvlinesize>>2);
    tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
    tcsm_p1 = tcsm_p1 + (RECON_BUF_STRIDE>>2);
  }//curr out

  if (upen) {
    tcsm_p = (unsigned int *)TCSM_DBLK_TOP_4LINE_YBUF;
    img_p = (unsigned int *)(img_y - linesize*4);/*4lines*/
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
	img_p[j] = tcsm_p[j];
      }
      img_p = img_p + (linesize>>2);
      tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
    }
    tcsm_p=(unsigned int *)TCSM_DBLK_TOP_4LINE_UBUF;
    tcsm_p1=(unsigned int *)TCSM_DBLK_TOP_4LINE_VBUF;
    img_p=(unsigned int *)(img_cb - uvlinesize*4);/*4lines*/
    img_p1=(unsigned int *)(img_cr - uvlinesize*4);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 2; j++) {
	img_p[j]  = tcsm_p[j] ;
	img_p1[j] = tcsm_p1[j];
      }
      img_p = img_p + (uvlinesize>>2);
      img_p1 = img_p1 + (uvlinesize>>2);
      tcsm_p = tcsm_p + (RECON_BUF_STRIDE>>2);
      tcsm_p1 = tcsm_p1 + (RECON_BUF_STRIDE>>2);
    }
  }//up out
#endif

}
#endif // if 0
