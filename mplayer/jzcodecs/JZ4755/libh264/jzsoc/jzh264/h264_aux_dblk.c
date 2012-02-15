
#include "../jz4750e_dblk.h"

#define PICT_FRAME         3
#define B_TYPE         3
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

void filter_mb_dblk(H264_Frame_GlbARGs *dFRM, H264_MB_DecARGs * dMB_aux, H264_AUX_T *AUX_T) 
{
  int mb_type = dMB_aux->mb_type;
  int dir;

  unsigned int leften = 0;
  unsigned int upen = 0;
  unsigned int bsary[4] = {0,0,0,0};
  uint8_t *bsptr = (uint8_t *)bsary;
  int qp;
  int qp_u;
  int qp_v;
  int ofst_a, ofst_b;

  /* FIXME: A given frame may occupy more than one position in
   * the reference list. So ref2frm should be populated with
   * frame numbers, not indices. */
  static const int ref2frm[34] = {-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

  //for sufficiently low qp, filtering wouldn't do anything
  //this is a conservative estimate: could also check beta_offset and more accurate chroma_qp
  if( dMB_aux->deblocking_filter & 0x1 ){
    //upen must be open because cpu move out from dblk_buf
    //leften must be open because left out in next mb
    leften=1;
  }
  else{
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

	if( (edge&1) && IS_8x8DCT(mb_type) ) {
	  bS[0] = bS[1] = bS[2] = bS[3] = 0;
	} else {

	if( IS_INTRA(mb_type) ||
	    IS_INTRA(mbn_type) ) {
	  int value;
#ifdef H264_FIELD_ENABLE
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
#else //H264_FIELD_ENABLE
	  bS[0] = bS[1] = bS[2] = bS[3] = 3;
#endif//H264_FIELD_ENABLE
	} else {
	  int i, l;
	  int mv_done;

	  if( edge & mask_edge ) {
	    bS[0] = bS[1] = bS[2] = bS[3] = 0;
	    mv_done = 1;
	  }
#ifdef H264_FIELD_ENABLE
	  else if(IS_INTERLACED(mb_type ^ mbn_type)) {
	    bS[0] = bS[1] = bS[2] = bS[3] = 1;
	    mv_done = 1;
	  }
#endif//H264_FIELD_ENABLE
	  else if( mask_par0 && (edge || (mbn_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir)))) ) {
	    int b_idx= 5 + 1 + edge * (dir ? 5:1);
	    int bn_idx= b_idx - (dir ? 5:1);
	    int v = 0;
	    for( l = 0; !v && l < 1 + (dMB_aux->slice_type == B_TYPE); l++ ) {
	      v |= ref2frm[dMB_aux->ref_cache[l][b_idx]+2] != ref2frm[dMB_aux->ref_cache[l][bn_idx]+2] ||
		FFABS( dMB_aux->mv_cache[l][b_idx][0] - dMB_aux->mv_cache[l][bn_idx][0] ) >= 4 ||
		FFABS( dMB_aux->mv_cache[l][b_idx][1] - dMB_aux->mv_cache[l][bn_idx][1] ) >= 4;
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
	    int b_idx= 5 + 1 + x + 5*y;
	    int bn_idx= b_idx - (dir ? 5:1);

	    if( dMB_aux->non_zero_count_cache_luma[b_idx] != 0 ||
		dMB_aux->non_zero_count_cache_luma[bn_idx] != 0 ) {
	      bS[i] = 2;
	    }
	    else if(!mv_done)
	      {
		bS[i] = 0;
		for( l = 0; l < 1 + (dMB_aux->slice_type == B_TYPE); l++ ) {
		  if( ref2frm[dMB_aux->ref_cache[l][b_idx]+2] != ref2frm[dMB_aux->ref_cache[l][bn_idx]+2] ||
		      FFABS( dMB_aux->mv_cache[l][b_idx][0] - dMB_aux->mv_cache[l][bn_idx][0] ) >= 4 ||
		      FFABS( dMB_aux->mv_cache[l][b_idx][1] - dMB_aux->mv_cache[l][bn_idx][1] ) >= 4 ) {
		    bS[i] = 1;
		    break;
		  }
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
  int qp_left,   qp_curr,   qp_up;
  int qp_left_u, qp_curr_u, qp_up_u;
  int qp_left_v, qp_curr_v, qp_up_v;

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
  }
  dat_nod_yuv_t *dat_nod_yuv = (dat_nod_yuv_t *)AUX_T->h264_dblk_des_ptr[0];
  dat_nod_yuv->dat_bsv01_y = bsary[0];
  dat_nod_yuv->dat_bsv23_y = bsary[1];
  dat_nod_yuv->dat_bsh01_y = bsary[2];
  dat_nod_yuv->dat_bsh23_y = bsary[3];
  dat_nod_yuv->dat_ofstab_y = ((ofst_a&0xff) << 24) | ((ofst_b&0xff) << 16) | 0x8001;
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

  uint8_t  *dest_y, *dest_cb, *dest_cr;
  int mb_x, mb_y, linesize, uvlinesize;
  mb_x = dMB_aux->mb_x;
  mb_y = dMB_aux->mb_y;
  linesize = dFRM->linesize;
  uvlinesize = dFRM->uvlinesize;

  dest_y  = dFRM->current_picture.y_ptr + (mb_y*16*  linesize) + mb_x*16;
  dest_cb = dFRM->current_picture.u_ptr + (mb_y* 8*uvlinesize) + mb_x*8;
  dest_cr = dFRM->current_picture.v_ptr + (mb_y* 8*uvlinesize) + mb_x*8;

  dat_nod_yuv->dat_upadr_y  = get_phy_addr_aux((uint32_t)(dest_y-4*linesize));
  dat_nod_yuv->dat_upstrd_y = linesize;
  dat_nod_yuv->dat_upadr_u  = get_phy_addr_aux((uint32_t)(dest_cb-4*uvlinesize));
  dat_nod_yuv->dat_upstrd_u = uvlinesize;
  dat_nod_yuv->dat_upadr_v  = get_phy_addr_aux((uint32_t)(dest_cr-4*uvlinesize));
  dat_nod_yuv->dat_upstrd_v = uvlinesize;

}
