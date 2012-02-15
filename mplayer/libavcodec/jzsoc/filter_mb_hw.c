static inline void h264_dblk_init(H264Context *h)
{
    unsigned int i;
    unsigned int btmp;

    // alpha beta lut
    btmp = (beta_tab_old[17] << 24) + (alpha_tab_old[17] << 16);
    SET_AB_LUT(0, btmp);
    for (i = 1; i < 18; i++) { 
      btmp = (beta_tab_old[17 + i*2] << 24) + (alpha_tab_old[17 + i*2] << 16) +
	(beta_tab_old[16 + i*2] << 8) + alpha_tab_old[16 + i*2]; 
      SET_AB_LUT((i*4), btmp);
    }
    // tc0 lut
    for (i = 0; i < 18; i++) {
      btmp = (tc0_tab_old[17+i*2][2] << 25) + (tc0_tab_old[17+i*2][1] << 20) +
	(tc0_tab_old[17+i*2][0] << 16) +
	(tc0_tab_old[16+i*2][2] << 9) + (tc0_tab_old[16+i*2][1] << 4) +
	tc0_tab_old[16+i*2][0];
      SET_TC0_LUT((i*4), btmp);
    }

    // --------------------- node 0 ---------------------
    desp_yuv_t * desp_yuv = (desp_yuv_t *)(TCSM_H264_DBLK_NODE0);

    desp_yuv->nodhead_y = 0xE1;
    desp_yuv->ofst_fmt_y = 0x4;
    desp_yuv->ofst_inadr_y = 0xc;
    desp_yuv->ofst_instrd_y = 0x10; 
    desp_yuv->dat_instrd_y = 20;    // fix
    desp_yuv->ofst_outadr_y = 0x14;
    desp_yuv->ofst_outstrd_y = 0x18;
    desp_yuv->dat_outstrd_y = 20;   // fix
    desp_yuv->ofst_upadr_y = 0x1c;
    desp_yuv->ofst_upstrd_y = 0x20;
//    desp_yuv->dat_upstrd_y = 720;   // fix
    desp_yuv->ofst_qp_y = 0x24;
    desp_yuv->ofst_ofstab_y = 0x28;
    desp_yuv->ofst_bsv01_y = 0x2c;
    desp_yuv->ofst_bsv23_y = 0x30;
    desp_yuv->ofst_bsh01_y = 0x34;
    desp_yuv->ofst_bsh23_y = 0x38;
    desp_yuv->ofst_ctrl_y = 0x0;
    desp_yuv->dat_ctrl_y = 0x1;

    desp_yuv->nodhead_u = 0xB1;
    desp_yuv->ofst_fmt_u = 0x4;
    desp_yuv->ofst_inadr_u = 0xc;
    desp_yuv->ofst_instrd_u = 0x10;
    desp_yuv->dat_instrd_u = 12;    // fix
    desp_yuv->ofst_outadr_u = 0x14;
    desp_yuv->ofst_outstrd_u = 0x18;
    desp_yuv->dat_outstrd_u = 12;   // fix
    desp_yuv->ofst_upadr_u = 0x1c;
    desp_yuv->ofst_upstrd_u = 0x20;
//    desp_yuv->dat_upstrd_u = 360;   // fix
    desp_yuv->ofst_qp_u = 0x24;
    desp_yuv->ofst_bsv01_u = 0x2c; 
    desp_yuv->ofst_bsh01_u = 0x34;
    desp_yuv->ofst_ctrl_u = 0x0;   
    desp_yuv->dat_ctrl_u = 0x1;   

    desp_yuv->nodhead_v = 0x50;
    desp_yuv->ofst_fmt_v = 0x4;
    desp_yuv->ofst_inadr_v = 0xc;
    desp_yuv->ofst_outadr_v = 0x14; 
    desp_yuv->ofst_upadr_v = 0x1c; 
    desp_yuv->ofst_ctrl_v = 0x0;
    desp_yuv->dat_ctrl_v = 0x1;

    // -------------- node 1 -------------------
    desp_yuv = (desp_yuv_t *)(TCSM_H264_DBLK_NODE1);

    desp_yuv->nodhead_y = 0xE1;
    desp_yuv->ofst_fmt_y = 0x4;
    desp_yuv->dat_fmt_y = 0xB01;
    desp_yuv->ofst_inadr_y = 0xc;
    desp_yuv->dat_inadr_y = (unsigned int)h->yrecon[0];
    desp_yuv->ofst_instrd_y = 0x10; 
    desp_yuv->dat_instrd_y = 20;    // fix
    desp_yuv->ofst_outadr_y = 0x14;
    desp_yuv->dat_outadr_y = (unsigned int)h->ydblk[0];
    desp_yuv->ofst_outstrd_y = 0x18;
    desp_yuv->dat_outstrd_y = 20;   // fix
    desp_yuv->ofst_upadr_y = 0x1c;
    desp_yuv->dat_upadr_y = (unsigned int)get_phy_addr(h->dblk_top_4line[0][0]);
    desp_yuv->ofst_upstrd_y = 0x20;
//    desp_yuv->dat_upstrd_y = 720;   // fix
    desp_yuv->ofst_qp_y = 0x24;
    desp_yuv->dat_qp_y = 0;
    desp_yuv->ofst_ofstab_y = 0x28;
    desp_yuv->dat_ofstab_y = 0x8001;
    desp_yuv->ofst_bsv01_y = 0x2c;
    desp_yuv->dat_bsv01_y = 0x0;
    desp_yuv->ofst_bsv23_y = 0x30;
    desp_yuv->dat_bsv23_y = 0x0;
    desp_yuv->ofst_bsh01_y = 0x34;
    desp_yuv->dat_bsh01_y = 0x0;
    desp_yuv->ofst_bsh23_y = 0x38;
    desp_yuv->dat_bsh23_y = 0x0;
    desp_yuv->ofst_ctrl_y = 0x0;
    desp_yuv->dat_ctrl_y = 0x1;

    desp_yuv->nodhead_u = 0xB1;
    desp_yuv->ofst_fmt_u = 0x4;
    desp_yuv->dat_fmt_u = 0x1501;
    desp_yuv->ofst_inadr_u = 0xc;
    desp_yuv->dat_inadr_u = (unsigned int)h->urecon[0];
    desp_yuv->ofst_instrd_u = 0x10;
    desp_yuv->dat_instrd_u = 12;    // fix
    desp_yuv->ofst_outadr_u = 0x14;
    desp_yuv->dat_outadr_u = (unsigned int)h->udblk[0];
    desp_yuv->ofst_outstrd_u = 0x18;
    desp_yuv->dat_outstrd_u = 12;   // fix
    desp_yuv->ofst_upadr_u = 0x1c;
    desp_yuv->dat_upadr_u = (unsigned int)get_phy_addr(h->dblk_top_4line[0][1]);
    desp_yuv->ofst_upstrd_u = 0x20;
//    desp_yuv->dat_upstrd_u = 360;   // fix
    desp_yuv->ofst_qp_u = 0x24;
    desp_yuv->dat_qp_u = 0x0;
    desp_yuv->ofst_bsv01_u = 0x2c; 
    desp_yuv->dat_bsv01_u = 0x0; 
    desp_yuv->ofst_bsh01_u = 0x34;
    desp_yuv->dat_bsh01_u = 0x0;
    desp_yuv->ofst_ctrl_u = 0x0;   
    desp_yuv->dat_ctrl_u = 0x1;   


    desp_yuv->nodhead_v = 0x50;
    desp_yuv->ofst_fmt_v = 0x4;
    desp_yuv->dat_fmt_v = 0x1101;
    desp_yuv->ofst_inadr_v = 0xc;
    desp_yuv->dat_inadr_v = (unsigned int)h->vrecon[0];
    desp_yuv->ofst_outadr_v = 0x14;
    desp_yuv->dat_outadr_v = (unsigned int)h->vdblk[0];
    desp_yuv->ofst_upadr_v = 0x1c;
    desp_yuv->dat_upadr_v = (unsigned int)get_phy_addr(h->dblk_top_4line[0][2]);
    desp_yuv->ofst_ctrl_v = 0x0;
    desp_yuv->dat_ctrl_v = 0x1;

    SET_DBLK_CTRL(1,0,0); // set dblk end flag
}

static void unfilter_mb_hw( H264Context *h, int mb_x, int mb_y) 
{
  uint8_t bsary[32];
  desp_yuv_t * dblk_node = (desp_yuv_t *)h->dblk_des_ptr[0];

  MpegEncContext * const s = &h->s;
  const int mb_xy= mb_x + mb_y*s->mb_stride;

  if(mb_xy) {
    SET_DHA_DBLK(h->dblk_des_ptr[1]);
    SET_DCS_DBLK(0x1);
  }

  int dir;
  int start;
  int edge;

#define ABS(a) ((a) >= 0 ? (a) : (-(a)))

  unsigned int qpleft = s->current_picture.qscale_table[mb_xy -1];
  unsigned int qpcurr = s->current_picture.qscale_table[mb_xy];
  unsigned int qpup = s->current_picture.qscale_table[h->top_mb_xy];
  qpleft = (qpleft + qpcurr + 1) >> 1;
  qpup = (qpup + qpcurr + 1) >> 1;

  unsigned int ofst_a = h->slice_alpha_c0_offset;
  unsigned int ofst_b = h->slice_beta_offset;
  // get_chroma_qp
  unsigned int qpleft_uv = h->pps.chroma_qp_table[0][qpleft & 0xff];
  unsigned int qpcurr_uv = h->pps.chroma_qp_table[0][qpcurr & 0xff];
  unsigned int qpup_uv = h->pps.chroma_qp_table[0][qpup & 0xff];;
  qpleft_uv = (qpleft_uv + qpcurr_uv + 1) >> 1;
  qpup_uv = (qpup_uv + qpcurr_uv + 1) >> 1;

  unsigned int leften = (mb_x != 0);
  unsigned int upen = (mb_y != 0);
  unsigned int bottomen = (mb_y == (s->mb_height - 1));

  // ----------------- replace start from dtmp -------------------

    unsigned int dtmp;

    uint8_t * tcsm_yi = h->yrecon[0];
    uint8_t * tcsm_ui = h->urecon[0];
    uint8_t * tcsm_vi = h->vrecon[0];

    uint8_t * tcsm_y = h->ydblk[0];
    uint8_t * tcsm_u = h->udblk[0];
    uint8_t * tcsm_v = h->vdblk[0];

    uint8_t * tcsm_yt = h->yt4l;
    uint8_t * tcsm_ut = h->ut4l;
    uint8_t * tcsm_vt = h->vt4l;

    /*3-step pipe cached for the 2nd following MB's pixel transfer use*/
    h->dblk_top_4line[1][0] = h->dblk_top_4line[0][0];
    h->dblk_top_4line[1][1] = h->dblk_top_4line[0][1];
    h->dblk_top_4line[1][2] = h->dblk_top_4line[0][2];
    h->top_edge_wb[1] = h->top_edge_wb[0];
    h->left_edge_wb[1] = h->left_edge_wb[0];
    h->bottom_edge_wb[1] = h->bottom_edge_wb[0];

    h->dblk_top_4line[0][0] = (unsigned int)tcsm_yt;
    h->dblk_top_4line[0][1] = (unsigned int)tcsm_ut;
    h->dblk_top_4line[0][2] = (unsigned int)tcsm_vt;
    tcsm_yt = get_phy_addr(h->yt4l);
    tcsm_ut = get_phy_addr(h->ut4l);
    tcsm_vt = get_phy_addr(h->vt4l);
    h->top_edge_wb[0] = upen;
    h->left_edge_wb[0] = leften;
    h->bottom_edge_wb[0] = bottomen;

    // yuv use same bs, uv ary are part of y
    S32STD(xr0,bsary,0);
    S32STD(xr0,bsary,4);
    S32STD(xr0,bsary,8);
    S32STD(xr0,bsary,12);
    S32STD(xr0,bsary,16);
    S32STD(xr0,bsary,20);
    S32STD(xr0,bsary,24);
    S32STD(xr0,bsary,28);

    // for h v
    dblk_node->dat_fmt_y = 0xB01 + (upen << 7) + (leften << 6);
    dblk_node->dat_upstrd_y=h->mb_linesize;
    dblk_node->dat_inadr_y = tcsm_yi;
    dblk_node->dat_outadr_y = tcsm_y;
    dblk_node->dat_upadr_y = (unsigned int)tcsm_yt;
    dblk_node->dat_qp_y = ((qpup << 14) + (qpleft << 7) + qpcurr);
    dblk_node->dat_ofstab_y = (ofst_a << 24) + (ofst_b << 16) + 0x8001;
    dblk_node->dat_bsv01_y = ((bsary[7] << 28) + (bsary[6] << 24) + (bsary[5] << 20) + 
			      (bsary[4] << 16) + (bsary[3] << 12) + (bsary[2] << 8) + 
			      (bsary[1] << 4) + bsary[0]
			      );

    dblk_node->dat_bsv23_y = ((bsary[15] << 28) + (bsary[14] << 24) + (bsary[13] << 20) + 
			      (bsary[12] << 16) + (bsary[11] << 12) + (bsary[10] << 8) + 
			      (bsary[9] << 4) + bsary[8]
			      );
    
    dblk_node->dat_bsh01_y = ((bsary[23] << 28) + (bsary[22] << 24) + (bsary[21] << 20) + 
			      (bsary[20] << 16) + (bsary[19] << 12) + (bsary[18] << 8) + 
			      (bsary[17] << 4) + bsary[16]
			      );
    
    dblk_node->dat_bsh23_y = ((bsary[31] << 28) + (bsary[30] << 24) + (bsary[29] << 20) + 
			      (bsary[28] << 16) + (bsary[27] << 12) + (bsary[26] << 8) + 
			      (bsary[25] << 4) + bsary[24]
			      );

    // ------------ u node ------------------------
    dblk_node->dat_fmt_u = 0x1501 + (upen << 7) + (leften << 6);
    dblk_node->dat_upstrd_u = h->mb_uvlinesize;
    dblk_node->dat_inadr_u = tcsm_ui;
    dblk_node->dat_outadr_u = tcsm_u; 
    dblk_node->dat_upadr_u = (unsigned int)tcsm_ut;
    dblk_node->dat_qp_u = ((qpup_uv << 14) + (qpleft_uv << 7) + qpcurr_uv); 
    dblk_node->dat_bsv01_u = ((bsary[11] << 28) + (bsary[10] << 24) + (bsary[9] << 20) + 
			      (bsary[8] << 16) + (bsary[3] << 12) + (bsary[2] << 8) + 
			      (bsary[1] << 4) + bsary[0]
			      );
    
    dblk_node->dat_bsh01_u = ((bsary[27] << 28) + (bsary[26] << 24) + (bsary[25] << 20) + 
			      (bsary[24] << 16) + (bsary[19] << 12) + (bsary[18] << 8) + 
			      (bsary[17] << 4) + bsary[16]
			      );


    // ------------ v node ------------------------
    dblk_node->dat_fmt_v = 0x1101 + (upen << 7) + (leften << 6);
    dblk_node->dat_inadr_v = tcsm_vi;
    dblk_node->dat_outadr_v = tcsm_v;
    dblk_node->dat_upadr_v = (unsigned int)tcsm_vt;

#undef ABS
}
static void filter_mb_hw( H264Context *h, int mb_x, int mb_y) 
{
  uint8_t bsary[32];
  desp_yuv_t * dblk_node = (desp_yuv_t *)h->dblk_des_ptr[0];

  MpegEncContext * const s = &h->s;
  const int mb_xy= mb_x + mb_y*s->mb_stride;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    const int mvy_limit = IS_INTERLACED(mb_type) ? 2 : 4;
    static const int ref2frm[34] = {-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                    16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

  if(mb_xy) {
    SET_DHA_DBLK(h->dblk_des_ptr[1]);
    SET_DCS_DBLK(0x1);
  }

  int dir;
  int start;
  int edge;

#define ABS(a) ((a) >= 0 ? (a) : (-(a)))

  unsigned int qpleft = s->current_picture.qscale_table[mb_xy -1];
  unsigned int qpcurr = s->current_picture.qscale_table[mb_xy];
  unsigned int qpup = s->current_picture.qscale_table[h->top_mb_xy];

  unsigned int ofst_a = h->slice_alpha_c0_offset;
  unsigned int ofst_b = h->slice_beta_offset;
  // get_chroma_qp
  unsigned int qpleft_uv = h->pps.chroma_qp_table[0][qpleft & 0xff];
  unsigned int qpcurr_uv = h->pps.chroma_qp_table[0][qpcurr & 0xff];
  unsigned int qpup_uv = h->pps.chroma_qp_table[0][qpup & 0xff];;
  qpleft = (qpleft + qpcurr + 1) >> 1;
  qpup = (qpup + qpcurr + 1) >> 1;
  qpleft_uv = (qpleft_uv + qpcurr_uv + 1) >> 1;
  qpup_uv = (qpup_uv + qpcurr_uv + 1) >> 1;

  unsigned int leften = (mb_x != 0);
  unsigned int upen = (mb_y != 0);
  unsigned int bottomen = (mb_y == (s->mb_height - 1));


  // ----------------- replace start from dtmp -------------------

    unsigned int dtmp;
    uint8_t * tcsm_yi = h->yrecon[0];
    uint8_t * tcsm_ui = h->urecon[0];
    uint8_t * tcsm_vi = h->vrecon[0];

    uint8_t * tcsm_y = h->ydblk[0];
    uint8_t * tcsm_u = h->udblk[0];
    uint8_t * tcsm_v = h->vdblk[0];

    uint8_t * tcsm_yt = h->yt4l;
    uint8_t * tcsm_ut = h->ut4l;
    uint8_t * tcsm_vt = h->vt4l;


    /*3-step pipe cached for the 2nd following MB's pixel transfer use*/
    h->dblk_top_4line[1][0] = h->dblk_top_4line[0][0];
    h->dblk_top_4line[1][1] = h->dblk_top_4line[0][1];
    h->dblk_top_4line[1][2] = h->dblk_top_4line[0][2];
    h->top_edge_wb[1] = h->top_edge_wb[0];
    h->left_edge_wb[1] = h->left_edge_wb[0];
    h->bottom_edge_wb[1] = h->bottom_edge_wb[0];

    h->dblk_top_4line[0][0] = (unsigned int)tcsm_yt;
    h->dblk_top_4line[0][1] = (unsigned int)tcsm_ut;
    h->dblk_top_4line[0][2] = (unsigned int)tcsm_vt;
    tcsm_yt = get_phy_addr(h->yt4l);
    tcsm_ut = get_phy_addr(h->ut4l);
    tcsm_vt = get_phy_addr(h->vt4l);
    h->top_edge_wb[0] = upen;
    h->left_edge_wb[0] = leften;
    h->bottom_edge_wb[0] = bottomen;

    // yuv use same bs, uv ary are part of y
    S32STD(xr0,bsary,0);
    S32STD(xr0,bsary,4);
    S32STD(xr0,bsary,8);
    S32STD(xr0,bsary,12);
    S32STD(xr0,bsary,16);
    S32STD(xr0,bsary,20);
    S32STD(xr0,bsary,24);
    S32STD(xr0,bsary,28);

    for( dir = 0; dir < 2; dir++ )
    {
        const int mbm_xy = dir == 0 ? mb_xy -1 : h->top_mb_xy;

        const int edges = (mb_type & (MB_TYPE_16x16|MB_TYPE_SKIP))
                                  == (MB_TYPE_16x16|MB_TYPE_SKIP) ? 1 : 4;
        // how often to recheck mv-based bS when iterating between edges
        const int mask_edge = (mb_type & (MB_TYPE_16x16 | (MB_TYPE_16x8 << dir))) ? 3 :
                              (mb_type & (MB_TYPE_8x16 >> dir)) ? 1 : 0;
        // how often to recheck mv-based bS when iterating along each edge
        const int mask_par0 = mb_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir));

	start = 0;

	/* test picture boundary */
	if( ( dir == 0 && mb_x == 0 ) || ( dir == 1 && mb_y == 0 ) ) {
	  start = 1;
	}

        if (h->deblocking_filter==2 && h->slice_table[mbm_xy] != h->slice_table[mb_xy]){
            start = 1;
        }



	/* Calculate bS */
	for( edge = start; edge < edges; edge++ ) {
	  /* mbn_xy: neighbour macroblock (how that works for field ?) */
	  int mbn_xy = edge > 0 ? mb_xy : mbm_xy;
          const int mbn_type = s->current_picture.mb_type[mbn_xy];

	  int bS[4];
	  int qp;
	  if( IS_INTRA( s->current_picture.mb_type[mb_xy] ) ||
	      IS_INTRA( s->current_picture.mb_type[mbn_xy] ) ) {
	    bS[0] = bS[1] = bS[2] = bS[3] = ( edge == 0 ? 4 : 3 );
	  } else {
             int i, l;
             int mv_done;

             if( edge & mask_edge ) {
                 bS[0] = bS[1] = bS[2] = bS[3] = 0;
                 mv_done = 1;
             }
             else if( mask_par0 && (edge || (mbn_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir)))) ) {
                 int b_idx= 8 + 4 + edge * (dir ? 8:1);
                 int bn_idx= b_idx - (dir ? 8:1);
                 int v = 0;
                 for( l = 0; !v && l < 1 + (h->slice_type == B_TYPE); l++ ) {
                     v |= ref2frm[h->ref_cache[l][b_idx]+2] != ref2frm[h->ref_cache[l][bn_idx]+2] ||
                          ABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                          ABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit;
                 }
                 bS[0] = bS[1] = bS[2] = bS[3] = v;
                 mv_done = 1;
             }
             else
                 mv_done = 0;

#if 1
	    for( i = 0; i < 4; i++ ) {
	      int x = dir == 0 ? edge : i;
	      int y = dir == 0 ? i    : edge;
	      int b_idx= 8 + 4 + x + 8*y;
	      int bn_idx= b_idx - (dir ? 8:1);

	      if( h->non_zero_count_cache[b_idx] != 0 ||
		  h->non_zero_count_cache[bn_idx] != 0 ) {
		bS[i] = 2;
	      }
              else if(!mv_done)
              {
                  bS[i] = 0;
                  for( l = 0; l < 1 + (h->slice_type == B_TYPE); l++ ) {
                      if( ref2frm[h->ref_cache[l][b_idx]+2] != ref2frm[h->ref_cache[l][bn_idx]+2] ||
                          FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                          FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit ) {
                          bS[i] = 1;
                          break;
                      }
                  }
              }

	    }
#else
	      int x = dir == 0 ? edge : 0;
	      int y = dir == 0 ? 0    : edge;
	      int b_idx= 8 + 4 + x + 8*y;
	      int bn_idx= b_idx - (dir ? 8:1);

	      if( h->non_zero_count_cache[b_idx] != 0 ||
		  h->non_zero_count_cache[bn_idx] != 0 ) {

		bS[0]=bS[1]=bS[2]=bS[3] = 2;
	      }
	      else if( h->slice_type == P_TYPE ) {
		if( h->ref_cache[0][b_idx] != h->ref_cache[0][bn_idx] ||
		    ABS( h->mv_cache[0][b_idx][0] - h->mv_cache[0][bn_idx][0] ) >= 4 ||
		    ABS( h->mv_cache[0][b_idx][1] - h->mv_cache[0][bn_idx][1] ) >= 4 )
	        	bS[0]=bS[1]=bS[2]=bS[3] = 1;
		else
		     bS[0]=bS[1]=bS[2]=bS[3] = 0;
	      }
	      else {
		/* FIXME Add support for B frame */
		return;
	      }

#endif

	    if(bS[0]+bS[1]+bS[2]+bS[3] == 0)
	      continue;
	  }

	  for (dtmp = 0; dtmp < 4; dtmp++) {
	    *(bsary+(edge*4)+(dir*16)+dtmp) = bS[dtmp];
	  }
	
	} // for edge
      } // for h v

    dblk_node->dat_fmt_y = 0xB01 + (upen << 7) + (leften << 6);
    dblk_node->dat_upstrd_y=h->mb_linesize;
    dblk_node->dat_inadr_y = tcsm_yi;
    dblk_node->dat_outadr_y = tcsm_y;
    dblk_node->dat_upadr_y = (unsigned int)tcsm_yt;
    dblk_node->dat_qp_y = ((qpup << 14) + (qpleft << 7) + qpcurr);
    dblk_node->dat_ofstab_y = ((ofst_a&0xFF)<<24)|((ofst_b&0xFF)<<16)|0x8001;
    dblk_node->dat_bsv01_y = ((bsary[7] << 28) + (bsary[6] << 24) + (bsary[5] << 20) + 
			      (bsary[4] << 16) + (bsary[3] << 12) + (bsary[2] << 8) + 
			      (bsary[1] << 4) + bsary[0]
			      );

    dblk_node->dat_bsv23_y = ((bsary[15] << 28) + (bsary[14] << 24) + (bsary[13] << 20) + 
			      (bsary[12] << 16) + (bsary[11] << 12) + (bsary[10] << 8) + 
			      (bsary[9] << 4) + bsary[8]
			      );
    
    dblk_node->dat_bsh01_y = ((bsary[23] << 28) + (bsary[22] << 24) + (bsary[21] << 20) + 
			      (bsary[20] << 16) + (bsary[19] << 12) + (bsary[18] << 8) + 
			      (bsary[17] << 4) + bsary[16]
			      );
    
    dblk_node->dat_bsh23_y = ((bsary[31] << 28) + (bsary[30] << 24) + (bsary[29] << 20) + 
			      (bsary[28] << 16) + (bsary[27] << 12) + (bsary[26] << 8) + 
			      (bsary[25] << 4) + bsary[24]
			      );

    // ------------ u node ------------------------
    dblk_node->dat_fmt_u = 0x1501 + (upen << 7) + (leften << 6);
    dblk_node->dat_upstrd_u = h->mb_uvlinesize;
    dblk_node->dat_inadr_u = tcsm_ui;
    dblk_node->dat_outadr_u = tcsm_u; 
    dblk_node->dat_upadr_u = (unsigned int)tcsm_ut;
    dblk_node->dat_qp_u = ((qpup_uv << 14) + (qpleft_uv << 7) + qpcurr_uv); 
    dblk_node->dat_bsv01_u = ((bsary[11] << 28) + (bsary[10] << 24) + (bsary[9] << 20) + 
			      (bsary[8] << 16) + (bsary[3] << 12) + (bsary[2] << 8) + 
			      (bsary[1] << 4) + bsary[0]
			      );
    
    dblk_node->dat_bsh01_u = ((bsary[27] << 28) + (bsary[26] << 24) + (bsary[25] << 20) + 
			      (bsary[24] << 16) + (bsary[19] << 12) + (bsary[18] << 8) + 
			      (bsary[17] << 4) + bsary[16]
			      );


    // ------------ v node ------------------------
    dblk_node->dat_fmt_v = 0x1101 + (upen << 7) + (leften << 6);
    dblk_node->dat_inadr_v = tcsm_vi;
    dblk_node->dat_outadr_v = tcsm_v;
    dblk_node->dat_upadr_v = (unsigned int)tcsm_vt;

#undef ABS
}
