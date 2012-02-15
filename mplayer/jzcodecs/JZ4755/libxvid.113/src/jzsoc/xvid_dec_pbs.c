/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *
 *  Copyright(C) 2002      MinChen <chenm001@163.com>
 *               2002-2004 Peter Ross <pross@xvid.org>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************
 *
 * xvid_dec_pbs.c
 *   -- In JZ Dcore Arch, it servers for main CPU split bitstream decoding
 *      and push task for aux CPU
 * $Id: xvid_dec_pbs.c,v 1.24 2009/07/10 01:56:23 gysun Exp $
 *
 ****************************************************************************/
#ifdef JZC_TCSM0_OPT
int mbnum = 0;
/*block memset by MXU*/
#define JZ_DCACHE_ALLOC 30
void block_memset_MXU(const int16_t *data){
  uint32_t i;
  uint32_t block = (uint32_t)data;
  for(i=0;i<4;i++){
    i_pref(JZ_DCACHE_ALLOC, block, 0);
    S32STD(xr0, block, 0);
    S32STD(xr0, block, 4);
    S32STD(xr0, block, 8);
    S32STD(xr0, block, 12);
    S32STD(xr0, block, 16);
    S32STD(xr0, block, 20);
    S32STD(xr0, block, 24);
    S32STD(xr0, block, 28);
    block += 32;
  }
}
static void
decoder_mbintra_vld_v(DECODER * dec,
		    MACROBLOCK * pMB,
		    const uint32_t x_pos,
		    const uint32_t y_pos,
		    const uint32_t acpred_flag,
		    const uint32_t cbp,
		    Bitstream * bs,
		    const uint32_t quant,
		    const uint32_t intra_dc_threshold,
		      const unsigned int bound)
{
  uint32_t i = 0,j = 0;
  uint32_t iQuant = pMB->quant;
  for (i = 0; i < 6; i++) {
    uint32_t iDcScaler = get_dc_scaler(iQuant, i < 4);
    int16_t predictors[8];
    int start_coeff;
    predict_acdc(dec->mbs, x_pos, y_pos, dec->mb_width, i, block,
       iQuant, iDcScaler, predictors, bound);
    if (!acpred_flag) {
      pMB->acpred_directions[i] = 0;
    }

    if (quant < intra_dc_threshold) {
      int dc_size;
      int dc_dif;
      dc_size = i < 4 ? get_dc_size_lum(bs) : get_dc_size_chrom(bs);
      dc_dif = dc_size ? get_dc_dif(bs, dc_size) : 0;
      if (dc_size > 8) {
        BitstreamSkip(bs, 1); /* marker */
      }
      block[0] = dc_dif;
      start_coeff = 1;

    } else {
      start_coeff = 0;
    }
    p_dMB->yuv_len[i] = 1;
    if (cbp & (1 << (5 - i))) /* coded */
    {
      int direction = dec->alternate_vertical_scan ?
        2 : pMB->acpred_directions[i];
      
      p_dMB->yuv_len[i]=get_intra_block_mxu(bs, block, direction, start_coeff);
    }
    /* resize idct_len of sub_MB */
    if(pMB->acpred_directions[i] == 2){
      p_dMB->yuv_len[i] = 8;
    }
    add_acdc(pMB, i, block, iDcScaler, predictors, dec->bs_version);
    block += p_dMB->yuv_len[i]*8;
  }
  /* lode fifo_mb */
  p_dMB->isfake = 0;
  p_dMB->mode = pMB->mode;
  p_dMB->cbp =pMB->cbp;
  p_dMB->quant = pMB->quant;
  p_dMB->direction = 0; //for dequant

  dMB1->next_dmb_len = (16*4 + (uint32_t)block -(uint32_t)p_dMB->residual);
}

static void
decoder_mbinter_vld_v(DECODER * dec, MACROBLOCK *const pMB,
		    const uint32_t cbp, const uint32_t bs,
		    const uint32_t rounding,const int ref,const int bvop)
{
  int i = 0,j = 0;
  const uint32_t iQuant = pMB->quant;
  const int direction = dec->alternate_vertical_scan ? 2 : 0;
  
  uint8_t *dst[6];
  for (i = 0; i < 6; i++) {
    /* Process only coded blocks */
    p_dMB->yuv_len[i]=0;
    if (cbp & (1 << (5 - i))) {
      block_memset_MXU(block);
      if(dec->quant_type == 0){
	p_dMB->yuv_len[i]= get_inter_block_h263_mxu(bs, block, direction, iQuant, 
						       get_inter_matrix(dec->mpeg_quant_matrices));
      }
      else{
	p_dMB->yuv_len[i]= get_inter_block_mpeg_mxu(bs, block, direction, iQuant, 
						       get_inter_matrix(dec->mpeg_quant_matrices));
      }
      block += p_dMB->yuv_len[i]*8;
    }
  }

  dMB1->next_dmb_len = 16*4 + (uint32_t)block - (uint32_t)p_dMB->residual;
  /* lode fifo_mb */
  p_dMB->isfake = 0;
  p_dMB->mode = pMB->mode;
  p_dMB->cbp = cbp;
  p_dMB->quant = pMB->quant;

  p_dMB->direction = direction; //for dequant
  p_dMB->ref = ref; //0|1 for dec->ref[ref]
  p_dMB->rounding = rounding;
  
  for(i=0;i<4;i++){
    p_dMB->mvs[i].x=(int16_t)pMB->mvs[i].x;
    p_dMB->mvs[i].y=(int16_t)pMB->mvs[i].y;
    p_dMB->b_mvs[i].x=(int16_t)pMB->b_mvs[i].x;
    p_dMB->b_mvs[i].y=(int16_t)pMB->b_mvs[i].y;
  }
}

static void
decoder_iframe_dcore(DECODER * dec,
		     Bitstream * bs,
		     int quant,
		     int intra_dc_threshold)
{
  uint32_t bound;
  uint32_t x, y,i;
  int16_t curr_len = 832,next_len = 832;
  int flag = 0;
  const uint32_t mb_width = dec->mb_width;
  const uint32_t mb_height = dec->mb_height;
  #ifdef JZC_MDMA1_OPT
  uint8_t  nfake  = (4 - ((dec->mb_width)%4))%4;    
  uint32_t online = 0;
  #endif  
  mbnum = 0;
  fifo_pMB = aux_pMB - 1;
  (*fifo_wp)=-1;
  J1_PMON_ON(vlc);
  /* init clean */
  for(i=0;i<6;i++){
	  block_memset_MXU(dMB0->residual+i*64);
	  block_memset_MXU(dMB1->residual+i*64);
	  block_memset_MXU(dMB2->residual+i*64);
  }
  aux_dec->quant_type = dec->quant_type;
  aux_dec->coding_type = I_VOP;
  aux_dec->interlacing = dec->interlacing;
  aux_dec->bs_version = dec->bs_version;
  aux_dec->quarterpel = dec->quarterpel;
  aux_dec->edged_width = dec->edged_width;
  aux_dec->mb_height = dec->mb_height;
  aux_dec->mb_width = dec->mb_width;
  aux_dec->cur[0] = tran_addr_for_aux((dec->cur.y));
  aux_dec->cur[1] = tran_addr_for_aux((dec->cur.u));
  aux_dec->cur[2] = tran_addr_for_aux((dec->cur.v));
  #ifdef JZC_MDMA1_OPT
  i_cache(0x19,(int32_t)aux_dec, 0);
  i_cache(0x19,(int32_t)aux_dec, 32);
  i_sync();
  #endif

  bound = 0;
     MACROBLOCK *mb;
      uint32_t mcbpc;
      uint32_t cbpc;
      uint32_t acpred_flag;
      uint32_t cbpy;
      uint32_t cbp;

  AUX_START();
  /* first dmb */
  {
    mbnum++;
    p_dMB = dMB1;
    block = dMB1->residual;

    while (BitstreamShowBits(bs, 9) == 1)
         BitstreamSkip(bs, 9);

    mb = &dec->mbs[0];
    mcbpc = get_mcbpc_intra(bs);
    mb->mode = mcbpc & 7;
    cbpc = (mcbpc >> 4);
    acpred_flag = BitstreamGetBit(bs);
    cbpy = get_cbpy(bs, 1);
    cbp = (cbpy << 2) | cbpc;

    if (mb->mode == MODE_INTRA_Q) {
      quant += dquant_table[BitstreamGetBits(bs, 2)];
      if (quant > 31) {
	quant = 31;
      } else if (quant < 1) {
	quant = 1;
      }
    }
    mb->quant = quant;
    mb->mvs[0].x = mb->mvs[0].y =
      mb->mvs[1].x = mb->mvs[1].y =
      mb->mvs[2].x = mb->mvs[2].y =
      mb->mvs[3].x = mb->mvs[3].y =0;

    if (dec->interlacing) {
      mb->field_dct = BitstreamGetBit(bs);
    }
    decoder_mbintra_vld_v(dec, mb, 0, 0, acpred_flag, cbp, bs, quant,
			  intra_dc_threshold, bound);
  }
  /* start from second mb */
  for (y = 0; y < mb_height; y++) {
    for (x = 0; x < mb_width; x++) {
        mbnum++;
	p_dMB = dMB2;
	block = dMB2->residual;

	if(flag == 0) { /* skap first mb */
	  x++;
	  flag = 1;
        }

      while (BitstreamShowBits(bs, 9) == 1)
        BitstreamSkip(bs, 9);

      if (check_resync_marker(bs, 0))
      {
	int32_t fake_i, err_dist;
        bound = read_video_packet_header(bs, dec, 0,
					 &quant, NULL, NULL, &intra_dc_threshold);
        #ifdef JZC_MDMA1_OPT
	online = y;
	// bound maybe greater than the (mb_width * mb_height - 1) : so it is frame 's boundary! yli
	if(bound > (mb_width * mb_height - 1)) bound = (mb_width * mb_height - 1);
	#endif
        x = bound % mb_width;
        y = bound / mb_width;
	

	/*insert fake task*/
	err_dist = bound - mbnum + 1;
	if(err_dist<0){
	  //task going back, ensure mem not cross boundary
	  #ifdef JZC_MDMA1_OPT
	  fifo_pMB += (err_dist + (y - online)* nfake);
	  (*fifo_wp) += (err_dist + (y - online)* nfake);
	  #else
	  fifo_pMB += err_dist;
	  (*fifo_wp) += err_dist;
	  #endif

	} else {
	  #ifdef JZC_MDMA1_OPT
	  err_dist = err_dist + (y - online)* nfake;
	  #endif
	  for(fake_i=0;fake_i<err_dist;fake_i++){
	    fifo_pMB->isfake = 1;
	    fifo_pMB[-1].next_dmb_len = 4;
	    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
	    i_sync();
	    fifo_pMB++;
	    (*fifo_wp)++;
	  }
	}
	mbnum = bound+1;
      }
      mb = &dec->mbs[y * dec->mb_width + x];

      DPRINTF(XVID_DEBUG_MB, "macroblock (%i,%i) %08x\n", x, y, BitstreamShowBits(bs, 32));

      mcbpc = get_mcbpc_intra(bs);
      mb->mode = mcbpc & 7;
      cbpc = (mcbpc >> 4);

      acpred_flag = BitstreamGetBit(bs);

      cbpy = get_cbpy(bs, 1);
      cbp = (cbpy << 2) | cbpc;

      if (mb->mode == MODE_INTRA_Q) {
        quant += dquant_table[BitstreamGetBits(bs, 2)];
        if (quant > 31) {
          quant = 31;
        } else if (quant < 1) {
          quant = 1;
        }
      }
      mb->quant = quant;
      mb->mvs[0].x = mb->mvs[0].y =
      mb->mvs[1].x = mb->mvs[1].y =
      mb->mvs[2].x = mb->mvs[2].y =
      mb->mvs[3].x = mb->mvs[3].y =0;

      if (dec->interlacing) {
        mb->field_dct = BitstreamGetBit(bs);
        DPRINTF(XVID_DEBUG_MB,"deci: field_dct: %i\n", mb->field_dct);
      }
      decoder_mbintra_vld_v(dec, mb, x, y, acpred_flag, cbp, bs, quant,
			    intra_dc_threshold, bound);
      POLLING_MDMA0_END();
      i_sync();
      fifo_pMB++;
      (*fifo_wp)++;      

      /* move data TCSM -> SDRAM */
      curr_len = next_len;
      next_len = dMB1->next_dmb_len;

      MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
      SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
      MDMA0_ARG->TSA = (uint32_t)dMB1;
      MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
      MDMA0_ARG->UNIT = MDMA_UNIT(0,64,curr_len);

      MDMA0_ARG++;
      MDMA0_ARG->TDA = (uint32_t)dMB1+64; 	
      MDMA0_RUN();
      XCHG3(dMB0,dMB1,dMB2,XCHGtmp);

    }
      uint8_t k;
      for(k = 0; k < (nfake); k++){
	p_dMB = dMB2;
        block = dMB2->residual;
	p_dMB->isfake = 1;			
	p_dMB->mode   = 0;		
	dMB1->next_dmb_len = 4; 		
	POLLING_MDMA0_END();
	i_sync();
	fifo_pMB++;
	(*fifo_wp)++;

	curr_len = next_len;
	next_len = dMB1->next_dmb_len;

	MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
	SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
	MDMA0_ARG->TSA = (uint32_t)dMB1;
	MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
	MDMA0_ARG->UNIT = MDMA_UNIT(0,64,curr_len);
	MDMA0_ARG++;
	MDMA0_ARG->TDA = (uint32_t)dMB1+64; 
	MDMA0_RUN();
	XCHG3(dMB0,dMB1,dMB2,XCHGtmp);
      }
        
    if(dec->out_frm)
      output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,0,y,mb_width);
  }
 /* move last mb to sdram */
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;

  MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1 + 1024);
  MDMA0_ARG->TSA = (uint32_t)dMB1;
  MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
  MDMA0_ARG->UNIT = MDMA_UNIT(1,64,next_len);
  SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
  MDMA0_RUN();
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;
  J1_PMON_OFF(vlc);
  /*insert fake task for AUX to finish its pipeline*/
  {
    //fake task 1
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_sync();
    fifo_pMB++;
    (*fifo_wp)++;
    //fake task 2
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_sync();
    fifo_pMB++;
    (*fifo_wp)++;
    //fake task 3
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_cache(0x19,(int32_t)fifo_pMB,0);
    i_sync();

    fifo_pMB++;
    (*fifo_wp)++;
    (*fifo_wp)++;
    J1_PMON_ON(sync);
    do{}while(!(*task_done));
    J1_PMON_OFF(sync);
    *task_done = 0;
  }
}

/* for P_VOP set gmc_warp to NULL ,S_VOP set gmc_warp */
static void
decoder_pframe_dcore(DECODER * dec,
		     Bitstream * bs,
		     int rounding,
		     int quant,
		     int fcode,
		     int intra_dc_threshold,
		     const WARPPOINTS *const gmc_warp)
{
  uint32_t x=0, y=0,i; //mbnum = 0;
  uint32_t bound;
  int cp_mb=0, st_mb=0;
  int16_t curr_len = 832,next_len = 832;
  int flag = 0;
  const uint32_t mb_width = dec->mb_width;
  const uint32_t mb_height = dec->mb_height;
  mbnum = 0;
  #ifdef JZC_MDMA1_OPT
  uint8_t nfake = (4 - ((dec->mb_width)%4))%4;
  uint32_t online = 0;
  #endif  

  if (!dec->is_edged[0]) {
    start_timer();
    J1_PMON_ON(expand);
    image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
		   dec->width, dec->height, dec->bs_version);
    dec->is_edged[0] = 1;
    J1_PMON_OFF(expand);
    stop_edges_timer();
  }

#ifdef JZC_MC_OPT
  jz_dcache_wb();
  MC_CACHE_FLUSH();
#endif

  if (gmc_warp) {
    /* accuracy: 0==1/2, 1=1/4, 2=1/8, 3=1/16 */
    generate_GMCparameters( dec->sprite_warping_points,
			    dec->sprite_warping_accuracy, gmc_warp,
			    dec->width, dec->height, &dec->new_gmc_data);
  }

  (*fifo_wp) = -1;
  fifo_pMB  = aux_pMB - 1;
  aux_dec->quant_type = dec->quant_type;
  aux_dec->coding_type = P_VOP;
  aux_dec->interlacing = dec->interlacing;
  aux_dec->bs_version = dec->bs_version;
  aux_dec->quarterpel = dec->quarterpel;
  aux_dec->edged_width = dec->edged_width;
  aux_dec->mb_height = dec->mb_height;
  aux_dec->mb_width = dec->mb_width;
  aux_dec->cur[0] = tran_addr_for_aux((dec->cur.y));
  aux_dec->cur[1] = tran_addr_for_aux((dec->cur.u));
  aux_dec->cur[2] = tran_addr_for_aux((dec->cur.v));
  aux_dec->refn[0][0] = tran_addr_for_aux((dec->refn[0].y));
  aux_dec->refn[0][1] = tran_addr_for_aux((dec->refn[0].u));
  aux_dec->refn[0][2] = tran_addr_for_aux((dec->refn[0].v));
  aux_dec->refn[1][0] = tran_addr_for_aux((dec->refn[1].y));
  aux_dec->refn[1][1] = tran_addr_for_aux((dec->refn[1].u));
  aux_dec->refn[1][2] = tran_addr_for_aux((dec->refn[1].v));

  #ifdef JZC_MDMA1_OPT
  i_cache(0x19,(int32_t)aux_dec, 0);
  i_cache(0x19,(int32_t)aux_dec, 32);
  i_sync();
  #endif
  bound = 0;
 J1_PMON_ON(vlc);
  /* init clean */
  for(i=0;i<6;i++){
	  block_memset_MXU(dMB0->residual+i*64);
	  block_memset_MXU(dMB1->residual+i*64);
	  block_memset_MXU(dMB2->residual+i*64);
  }
  
  AUX_START();
  MACROBLOCK *mb;
  uint32_t mcbpc, cbpc, cbpy, cbp;
  /* first dmb */
  {
    mbnum++;
    p_dMB = dMB1;
    block = dMB1->residual;
    
      /* skip stuffing */
      while (BitstreamShowBits(bs, 10) == 1)
        BitstreamSkip(bs, 10);
      mb = &dec->mbs[0];
      if (!(BitstreamGetBit(bs))) { /* block _is_ coded */
        uint32_t intra, acpred_flag = 0;
        int mcsel = 0;    /* mcsel: '0'=local motion, '1'=GMC */

        cp_mb++;
        mcbpc = get_mcbpc_inter(bs);
        mb->mode = mcbpc & 7;
        cbpc = (mcbpc >> 4);
        intra = (mb->mode == MODE_INTRA || mb->mode == MODE_INTRA_Q);

        if (gmc_warp && (mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q))
          mcsel = BitstreamGetBit(bs);
        else if (intra)
          acpred_flag = BitstreamGetBit(bs);

        cbpy = get_cbpy(bs, intra);
        cbp = (cbpy << 2) | cbpc;

        if (mb->mode == MODE_INTER_Q || mb->mode == MODE_INTRA_Q) {
          int dquant = dquant_table[BitstreamGetBits(bs, 2)];
          quant += dquant;
          if (quant > 31) {
            quant = 31;
          } else if (quant < 1) {
            quant = 1;
          }
        }
        mb->quant = quant;

        mb->field_pred=0;
        if (dec->interlacing) {
          if (cbp || intra) {
            mb->field_dct = BitstreamGetBit(bs);
          }

          if ((mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q) && !mcsel) {
            mb->field_pred = BitstreamGetBit(bs);
            if (mb->field_pred) {
              mb->field_for_top = BitstreamGetBit(bs);
              mb->field_for_bot = BitstreamGetBit(bs);
            }
          }
        }

	/*get mv*/
        if (mcsel) { 
          decoder_mbgmc(dec, mb, 0, 0, fcode, cbp, bs, rounding);
	  goto OVER;
	  
        } else if (mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q) {

          if(dec->interlacing) {
            /* Get motion vectors interlaced, field_pred is handled there */
            get_motion_vector_interlaced(dec, bs, 0, 0, 0, mb, fcode, bound);
          } else {
            get_motion_vector(dec, bs, 0, 0, 0, &mb->mvs[0], fcode, bound);
            mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
          }
        } else if (mb->mode == MODE_INTER4V ) {
          /* interlaced missing here */
          get_motion_vector(dec, bs, 0, 0, 0, &mb->mvs[0], fcode, bound);
          get_motion_vector(dec, bs, 0, 0, 1, &mb->mvs[1], fcode, bound);
          get_motion_vector(dec, bs, 0, 0, 2, &mb->mvs[2], fcode, bound);
          get_motion_vector(dec, bs, 0, 0, 3, &mb->mvs[3], fcode, bound);
        } else { /* MODE_INTRA, MODE_INTRA_Q */
          mb->mvs[0].x = mb->mvs[1].x = mb->mvs[2].x = mb->mvs[3].x = 0;
          mb->mvs[0].y = mb->mvs[1].y = mb->mvs[2].y = mb->mvs[3].y = 0;
	  for(i=0;i<6;i++)
        	  block_memset_MXU(block+i*64);
	  decoder_mbintra_vld_v(dec, mb, 0, 0, acpred_flag, cbp, bs, quant,
                  intra_dc_threshold, bound);
	  goto OVER;
        }

        /* See how to decode */
	decoder_mbinter_vld_v(dec,mb,cbp,bs,rounding,0,0);
      } else if (gmc_warp) {  /* a not coded S(GMC)-VOP macroblock */
        mb->mode = MODE_NOT_CODED_GMC;
        mb->quant = quant;
      
	decoder_mbgmc(dec, mb, 0, 0, fcode, 0x00, bs, rounding);

        if(dec->out_frm && cp_mb > 0) {
          output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,0,cp_mb);
          cp_mb = 0;
        }
        st_mb = x+1;
      } else { /* not coded P_VOP macroblock */
        mb->mode = MODE_NOT_CODED;
        mb->quant = quant;
	
        mb->mvs[0].x = mb->mvs[1].x = mb->mvs[2].x = mb->mvs[3].x = 0;
        mb->mvs[0].y = mb->mvs[1].y = mb->mvs[2].y = mb->mvs[3].y = 0;
        mb->field_pred=0; /* (!) */
      decoder_mbinter_vld_v(dec,mb,0,bs,rounding,0,0);
	if(dec->out_frm && cp_mb > 0) {
	  output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
            cp_mb = 0;
        }
        st_mb = x+1;
      }
  }
 OVER:
  /* start from second mb */
  for (y = 0; y < mb_height; y++) {
    cp_mb = st_mb = 0;
    for (x = 0; x < mb_width; x++) {
      // MACROBLOCK *mb;
      //uint32_t mcbpc, cbpc, cbpy, cbp;

      mbnum++;
      p_dMB = dMB2;
      block = dMB2->residual;

      if(flag == 0) { /* skap first mb */
	x++;
	flag = 1;
      }

      /* skip stuffing */
      while (BitstreamShowBits(bs, 10) == 1)
        BitstreamSkip(bs, 10);
      
      if (check_resync_marker(bs, fcode - 1)) {
	int32_t fake_i, err_dist;
        bound = read_video_packet_header(bs, dec, fcode - 1,
          &quant, &fcode, NULL, &intra_dc_threshold);
	#ifdef JZC_MDMA1_OPT
	online = y;
	// bound maybe greater than the (mb_width * mb_height - 1) : so it is frame 's boundary! yli
	if(bound > (mb_width * mb_height - 1)) bound = (mb_width * mb_height - 1);
	#endif
        x = bound % mb_width;
        y = bound / mb_width;

	/*insert fake task*/
	err_dist = bound - mbnum + 1;
	if(err_dist<0){
	  //task going back, ensure mem not cross boundary
	  #ifdef JZC_MDMA1_OPT
	  fifo_pMB   += (err_dist + (y - online)* nfake);
	  (*fifo_wp) += (err_dist + (y - online)* nfake);
	  #else
	  fifo_pMB   += err_dist;
	  (*fifo_wp) += err_dist;
	  #endif

	} else {
	#ifdef JZC_MDMA1_OPT
	  err_dist = err_dist + (y - online)* nfake;
	#endif
	  for(fake_i=0;fake_i<err_dist;fake_i++){
	    fifo_pMB->isfake = 1;
	    fifo_pMB[-1].next_dmb_len = 4;
	    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
	    i_sync();
	    fifo_pMB++;
	    (*fifo_wp)++;
	  }
	}
	mbnum = bound+1;
      }
      mb = &dec->mbs[y * dec->mb_width + x];

      DPRINTF(XVID_DEBUG_MB, "macroblock (%i,%i) %08x\n", x, y, BitstreamShowBits(bs, 32));

      if (!(BitstreamGetBit(bs))) { /* block _is_ coded */
        uint32_t intra, acpred_flag = 0;
        int mcsel = 0;    /* mcsel: '0'=local motion, '1'=GMC */

        cp_mb++;
        mcbpc = get_mcbpc_inter(bs);
        mb->mode = mcbpc & 7;
        cbpc = (mcbpc >> 4);

        DPRINTF(XVID_DEBUG_MB, "mode %i\n", mb->mode);
        DPRINTF(XVID_DEBUG_MB, "cbpc %i\n", cbpc);

        intra = (mb->mode == MODE_INTRA || mb->mode == MODE_INTRA_Q);

        if (gmc_warp && (mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q))
          mcsel = BitstreamGetBit(bs);
        else if (intra)
          acpred_flag = BitstreamGetBit(bs);

        cbpy = get_cbpy(bs, intra);
        DPRINTF(XVID_DEBUG_MB, "cbpy %i mcsel %i \n", cbpy,mcsel);

        cbp = (cbpy << 2) | cbpc;

        if (mb->mode == MODE_INTER_Q || mb->mode == MODE_INTRA_Q) {
          int dquant = dquant_table[BitstreamGetBits(bs, 2)];
          DPRINTF(XVID_DEBUG_MB, "dquant %i\n", dquant);
          quant += dquant;
          if (quant > 31) {
            quant = 31;
          } else if (quant < 1) {
            quant = 1;
          }
          DPRINTF(XVID_DEBUG_MB, "quant %i\n", quant);
        }
        mb->quant = quant;

        mb->field_pred=0;
        if (dec->interlacing) {
          if (cbp || intra) {
            mb->field_dct = BitstreamGetBit(bs);
            DPRINTF(XVID_DEBUG_MB,"decp: field_dct: %i\n", mb->field_dct);
          }

          if ((mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q) && !mcsel) {
            mb->field_pred = BitstreamGetBit(bs);
            DPRINTF(XVID_DEBUG_MB, "decp: field_pred: %i\n", mb->field_pred);

            if (mb->field_pred) {
              mb->field_for_top = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_top: %i\n", mb->field_for_top);
              mb->field_for_bot = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_bot: %i\n", mb->field_for_bot);
            }
          }
        }

	/*get mv*/
        if (mcsel) { 
          decoder_mbgmc(dec, mb, x, y, fcode, cbp, bs, rounding);
	  continue;
	  
        } else if (mb->mode == MODE_INTER || mb->mode == MODE_INTER_Q) {

          if(dec->interlacing) {
            /* Get motion vectors interlaced, field_pred is handled there */
            get_motion_vector_interlaced(dec, bs, x, y, 0, mb, fcode, bound);
          } else {
            get_motion_vector(dec, bs, x, y, 0, &mb->mvs[0], fcode, bound);
            mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
          }
        } else if (mb->mode == MODE_INTER4V ) {
          /* interlaced missing here */
          get_motion_vector(dec, bs, x, y, 0, &mb->mvs[0], fcode, bound);
          get_motion_vector(dec, bs, x, y, 1, &mb->mvs[1], fcode, bound);
          get_motion_vector(dec, bs, x, y, 2, &mb->mvs[2], fcode, bound);
          get_motion_vector(dec, bs, x, y, 3, &mb->mvs[3], fcode, bound);
        } else { /* MODE_INTRA, MODE_INTRA_Q */
          mb->mvs[0].x = mb->mvs[1].x = mb->mvs[2].x = mb->mvs[3].x = 0;
          mb->mvs[0].y = mb->mvs[1].y = mb->mvs[2].y = mb->mvs[3].y = 0;
	  for(i=0;i<6;i++)
        	  block_memset_MXU(block+i*64);
	  decoder_mbintra_vld_v(dec, mb, x, y, acpred_flag, cbp, bs, quant,
                  intra_dc_threshold, bound);
	  goto MOVE_DATA;
        }

        /* See how to decode */
	decoder_mbinter_vld_v(dec,mb,cbp,bs,rounding,0,0);
      } else if (gmc_warp) {  /* a not coded S(GMC)-VOP macroblock */
        mb->mode = MODE_NOT_CODED_GMC;
        mb->quant = quant;
      
	decoder_mbgmc(dec, mb, x, y, fcode, 0x00, bs, rounding);

        if(dec->out_frm && cp_mb > 0) {
          output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
          cp_mb = 0;
        }
        st_mb = x+1;
      } else { /* not coded P_VOP macroblock */
        mb->mode = MODE_NOT_CODED;
        mb->quant = quant;
	
        mb->mvs[0].x = mb->mvs[1].x = mb->mvs[2].x = mb->mvs[3].x = 0;
        mb->mvs[0].y = mb->mvs[1].y = mb->mvs[2].y = mb->mvs[3].y = 0;
        mb->field_pred=0; /* (!) */
      decoder_mbinter_vld_v(dec,mb,0,bs,rounding,0,0);
	if(dec->out_frm && cp_mb > 0) {
	  output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
            cp_mb = 0;
        }
        st_mb = x+1;
      }
    MOVE_DATA:
      //
      POLLING_MDMA0_END();
      i_sync();
      fifo_pMB++;
      (*fifo_wp)++;      

      /* move data TCSM -> SDRAM */
      curr_len = next_len;
      next_len = dMB1->next_dmb_len;

      MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
      SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
      MDMA0_ARG->TSA = (uint32_t)dMB1;
      MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
      MDMA0_ARG->UNIT = MDMA_UNIT(1,64,curr_len); 	
      MDMA0_RUN();
      XCHG3(dMB0,dMB1,dMB2,XCHGtmp);

      //
    } //row over
    uint8_t k;
    for(k = 0; k < (nfake); k++){
      p_dMB = dMB2;
      block = dMB2->residual;
      p_dMB->isfake = 1;			
      p_dMB->mode   = 0;		
      dMB1->next_dmb_len = 4; 		
      POLLING_MDMA0_END();
      i_sync();
      fifo_pMB++;
      (*fifo_wp)++;

      curr_len = next_len;
      next_len = dMB1->next_dmb_len;

      MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
      SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
      MDMA0_ARG->TSA = (uint32_t)dMB1;
      MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
      MDMA0_ARG->UNIT = MDMA_UNIT(1,64,curr_len);
      MDMA0_RUN();
      XCHG3(dMB0,dMB1,dMB2,XCHGtmp);
    }
    if(dec->out_frm && cp_mb > 0)
      output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
  }
 /* move last mb to sdram */
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;

  MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1 + 1024);
  MDMA0_ARG->TSA = (uint32_t)dMB1;
  MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
  MDMA0_ARG->UNIT = MDMA_UNIT(1,64,next_len);
  SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
  MDMA0_RUN();
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;

 J1_PMON_OFF(vlc);
  /*insert fake task for AUX to finish its pipeline*/
  {

    //fake task 1
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_sync();
    fifo_pMB++;
    (*fifo_wp)++;
    //fake task 2
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_sync();
    fifo_pMB++;
    (*fifo_wp)++;
    //fake task 3
    fifo_pMB->isfake = 1;
    fifo_pMB[-1].next_dmb_len = 4;
    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
    i_cache(0x19,(int32_t)fifo_pMB,0);
    i_sync();

    fifo_pMB++;
    (*fifo_wp)++;
    (*fifo_wp)++;

    J1_PMON_ON(sync);
    do{}while(!(*task_done));
    J1_PMON_OFF(sync);
    *task_done = 0;
  }
}

static void
decoder_bframe_dcore(DECODER * dec,
		     Bitstream * bs,
		     int quant,
		     int fcode_forward,
		     int fcode_backward)
{
  uint32_t x=0, y=0;// mbnum = 0;
  int16_t curr_len = 832,next_len = 832;
  int flag = 0;
  mbnum = 0;
  VECTOR mv,temp_fmv = {0,0},temp_bmv = {0,0};
  const VECTOR zeromv = {0,0};
  int i;
  #ifdef JZC_MDMA1_OPT
  uint8_t nfake = (4 - (dec->mb_width%4))%4;    
  uint32_t online = 0;
  #endif  
  
  if (!dec->is_edged[0]) {
    start_timer();
    J1_PMON_ON(expand);
    image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
            dec->width, dec->height, dec->bs_version);
    dec->is_edged[0] = 1;
    J1_PMON_OFF(expand);
    stop_edges_timer();
  }

  if (!dec->is_edged[1]) {
    start_timer();
    J1_PMON_ON(expand);
    image_setedges(&dec->refn[1], dec->edged_width, dec->edged_height,
            dec->width, dec->height, dec->bs_version);
    dec->is_edged[1] = 1;
    J1_PMON_OFF(expand);
    stop_edges_timer();
  }

#ifdef JZC_MC_OPT
  jz_dcache_wb();
  MC_CACHE_FLUSH();
#endif

  (*fifo_wp) = -1;
  fifo_pMB  = aux_pMB - 1;
  aux_dec->quant_type = dec->quant_type;
  aux_dec->coding_type = B_VOP;
  aux_dec->interlacing = dec->interlacing;
  aux_dec->bs_version = dec->bs_version;
  aux_dec->quarterpel = dec->quarterpel;
  aux_dec->edged_width = dec->edged_width;
  aux_dec->mb_height = dec->mb_height;
  aux_dec->mb_width = dec->mb_width;
  
  aux_dec->cur[0] = tran_addr_for_aux((dec->cur.y));
  aux_dec->cur[1] = tran_addr_for_aux((dec->cur.u));
  aux_dec->cur[2] = tran_addr_for_aux((dec->cur.v));
  aux_dec->refn[0][0] = tran_addr_for_aux((dec->refn[0].y));
  aux_dec->refn[0][1] = tran_addr_for_aux((dec->refn[0].u));
  aux_dec->refn[0][2] = tran_addr_for_aux((dec->refn[0].v));
  aux_dec->refn[1][0] = tran_addr_for_aux((dec->refn[1].y));
  aux_dec->refn[1][1] = tran_addr_for_aux((dec->refn[1].u));
  aux_dec->refn[1][2] = tran_addr_for_aux((dec->refn[1].v));
  #ifdef JZC_MDMA1_OPT
  i_cache(0x19,(int32_t)aux_dec, 0);
  i_cache(0x19,(int32_t)aux_dec, 32);
  i_sync();
  #else
  aux_dec->qtmp[0] = tran_addr_for_aux((dec->qtmp.y));
  aux_dec->qtmp[1] = tran_addr_for_aux((dec->qtmp.u));
  aux_dec->qtmp[2] = tran_addr_for_aux((dec->qtmp.v));
  #endif
J1_PMON_ON(vlc);
  /* init clean */
  for(i=0;i<6;i++){
	  block_memset_MXU(dMB0->residual+i*64);
	  block_memset_MXU(dMB1->residual+i*64);
	  block_memset_MXU(dMB2->residual+i*64);
  }

  AUX_START();
  
  /* first dmb */
  {
    mbnum++;
    p_dMB = dMB1;
    block = dMB1->residual;
    
      MACROBLOCK *mb = &dec->mbs[0];
      MACROBLOCK *last_mb = &dec->last_mbs[0];
      const int fcode_max = (fcode_forward>fcode_backward) ? fcode_forward : fcode_backward;
      int intra_dc_threshold; /* fake variable */
      mv =
	mb->b_mvs[0] = mb->b_mvs[1] = mb->b_mvs[2] = mb->b_mvs[3] =
	mb->mvs[0] = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = zeromv;
      mb->quant = quant;
      dec->p_fmv = dec->p_bmv = zeromv;
      if (last_mb->mode == MODE_NOT_CODED) {
        mb->cbp = 0;
        mb->mode = MODE_FORWARD;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,1,1);
	goto OVER;
      }
      /* get mb->mode or cbp or quant or field by modb */
      if (!BitstreamGetBit(bs)) { /* modb=='0' */
        const uint8_t modb2 = BitstreamGetBit(bs);

        mb->mode = get_mbtype(bs);

        if (!modb2)   /* modb=='00' */
          mb->cbp = BitstreamGetBits(bs, 6);
        else
          mb->cbp = 0;

        if (mb->mode && mb->cbp) {
          quant += get_dbquant(bs);
          if (quant > 31)
            quant = 31;
          else if (quant < 1)
            quant = 1;
        }
        mb->quant = quant;

        if (dec->interlacing) {
          if (mb->cbp) {
            mb->field_dct = BitstreamGetBit(bs);
          }

          if (mb->mode) {
            mb->field_pred = BitstreamGetBit(bs);
            DPRINTF(XVID_DEBUG_MB, "decp: field_pred: %i\n", mb->field_pred);

            if (mb->field_pred) {
              mb->field_for_top = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_top: %i\n", mb->field_for_top);
              mb->field_for_bot = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_bot: %i\n", mb->field_for_bot);
            }
          }
        }

      } else {  /*modb = '1' mb_type is not present*/
        mb->mode = MODE_DIRECT_NONE_MV;
        mb->cbp = 0;
      }

      switch (mb->mode) {
      case MODE_DIRECT:
        get_b_motion_vector(bs, &mv, 1, zeromv, dec, 0, 0);

      case MODE_DIRECT_NONE_MV:
	//get mvs[],b_mvs
        for (i = 0; i < 4; i++) {
          mb->mvs[i].x = last_mb->mvs[i].x*dec->time_bp/dec->time_pp + mv.x;
          mb->mvs[i].y = last_mb->mvs[i].y*dec->time_bp/dec->time_pp + mv.y;

          mb->b_mvs[i].x = (mv.x)
            ?  mb->mvs[i].x - last_mb->mvs[i].x
            : last_mb->mvs[i].x*(dec->time_bp - dec->time_pp)/dec->time_pp;
          mb->b_mvs[i].y = (mv.y)
            ? mb->mvs[i].y - last_mb->mvs[i].y
            : last_mb->mvs[i].y*(dec->time_bp - dec->time_pp)/dec->time_pp;
        }
	p_dMB->bvop_direct = 1;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);
	break;
      case MODE_INTERPOLATE:
	//get mvs[],b_mvs,dec->p_fmv ,dec->p_bmv
        get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, 0, 0);
        dec->p_fmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];

        get_b_motion_vector(bs, &mb->b_mvs[0], fcode_backward, dec->p_bmv, dec, 0, 0);
        dec->p_bmv = mb->b_mvs[1] = mb->b_mvs[2] = mb->b_mvs[3] = mb->b_mvs[0];
	p_dMB->bvop_direct = 0;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);
	temp_fmv = dec->p_fmv,temp_bmv = dec->p_bmv;
	break;
      case MODE_BACKWARD:
        get_b_motion_vector(bs, &mb->mvs[0], fcode_backward, dec->p_bmv, dec, 0, 0);
        dec->p_bmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
	temp_bmv = dec->p_bmv;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);	
	break;
      case MODE_FORWARD:
        get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, 0, 0);
        dec->p_fmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
	temp_fmv = dec->p_fmv;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,1,1);	
	break;
      default:
        DPRINTF(XVID_DEBUG_ERROR,"Not supported B-frame mb_type = %i\n", mb->mode);
      }
  }
    OVER:
  /* resave the fmv/bmv in first row */
  for (y = 0; y < dec->mb_height; y++) {
    /* Initialize Pred Motion Vector */
    dec->p_fmv = dec->p_bmv = zeromv;
    for (x = 0; x < dec->mb_width; x++) {
      if(flag == 0) { /* skap first mb */
	x++;
	flag = 1;
	dec->p_fmv = temp_fmv;
	dec->p_bmv = temp_bmv;
      }
      MACROBLOCK *mb = &dec->mbs[y * dec->mb_width + x];
      MACROBLOCK *last_mb = &dec->last_mbs[y * dec->mb_width + x];
      const int fcode_max = (fcode_forward>fcode_backward) ? fcode_forward : fcode_backward;
      int intra_dc_threshold; /* fake variable */

      mbnum++;
      p_dMB = dMB2;
      block = dMB2->residual;
      if (check_resync_marker(bs, fcode_max  - 1)) {

	int32_t fake_i, err_dist;
        int bound = read_video_packet_header(bs, dec, fcode_max - 1, &quant,
					     &fcode_forward, &fcode_backward, 
					     &intra_dc_threshold);
	#ifdef JZC_MDMA1_OPT
	online = y;
	// bound maybe greater than the (mb_width * mb_height - 1) : so it is frame 's boundary! yli
	if(bound > (dec->mb_width * dec->mb_height - 1)) bound = (dec->mb_width * dec->mb_height - 1);
	#endif
        x = bound % dec->mb_width;
        y = bound / dec->mb_width;
        /* reset predicted macroblocks */
        dec->p_fmv = dec->p_bmv = zeromv;

	/*insert fake task*/
	err_dist = bound - mbnum + 1;
	if(err_dist<0){
	  //task going back, ensure mem not cross boundary
	  #ifdef JZC_MDMA1_OPT
	  fifo_pMB += (err_dist + (y - online)* nfake); 
	  (*fifo_wp) += (err_dist + (y - online)* nfake);
	  #else
	  fifo_pMB += err_dist;
	  (*fifo_wp) += err_dist;
	  #endif
	} else {
	#ifdef JZC_MDMA1_OPT
	  err_dist = err_dist + (y - online)* nfake;
	#endif
	  for(fake_i=0;fake_i<err_dist;fake_i++){
	    fifo_pMB->isfake = 1;
	    fifo_pMB[-1].next_dmb_len = 4;
	    i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
	    i_sync();
	    fifo_pMB++;
	    (*fifo_wp)++;
	  }
	}
	mbnum = bound+1;
      }

      mv =
	mb->b_mvs[0] = mb->b_mvs[1] = mb->b_mvs[2] = mb->b_mvs[3] =
	mb->mvs[0] = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = zeromv;
      mb->quant = quant;
      
      /*
       * skip if the co-located P_VOP macroblock is not coded
       * if not codec in co-located S_VOP macroblock is _not_
       * automatically skipped
       */
      if (last_mb->mode == MODE_NOT_CODED) {
        mb->cbp = 0;
        mb->mode = MODE_FORWARD;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,1,1);
	goto MOVE_DATA;
      }
      /* get mb->mode or cbp or quant or field by modb */
      if (!BitstreamGetBit(bs)) { /* modb=='0' */
        const uint8_t modb2 = BitstreamGetBit(bs);

        mb->mode = get_mbtype(bs);

        if (!modb2)   /* modb=='00' */
          mb->cbp = BitstreamGetBits(bs, 6);
        else
          mb->cbp = 0;

        if (mb->mode && mb->cbp) {
          quant += get_dbquant(bs);
          if (quant > 31)
            quant = 31;
          else if (quant < 1)
            quant = 1;
        }
        mb->quant = quant;

        if (dec->interlacing) {
          if (mb->cbp) {
            mb->field_dct = BitstreamGetBit(bs);
            DPRINTF(XVID_DEBUG_MB,"decp: field_dct: %i\n", mb->field_dct);
          }

          if (mb->mode) {
            mb->field_pred = BitstreamGetBit(bs);
            DPRINTF(XVID_DEBUG_MB, "decp: field_pred: %i\n", mb->field_pred);

            if (mb->field_pred) {
              mb->field_for_top = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_top: %i\n", mb->field_for_top);
              mb->field_for_bot = BitstreamGetBit(bs);
              DPRINTF(XVID_DEBUG_MB,"decp: field_for_bot: %i\n", mb->field_for_bot);
            }
          }
        }

      } else {  /*modb = '1' mb_type is not present*/
        mb->mode = MODE_DIRECT_NONE_MV;
        mb->cbp = 0;
      }

      switch (mb->mode) {
      case MODE_DIRECT:
        get_b_motion_vector(bs, &mv, 1, zeromv, dec, x, y);

      case MODE_DIRECT_NONE_MV:
	//get mvs[],b_mvs
        for (i = 0; i < 4; i++) {
          mb->mvs[i].x = last_mb->mvs[i].x*dec->time_bp/dec->time_pp + mv.x;
          mb->mvs[i].y = last_mb->mvs[i].y*dec->time_bp/dec->time_pp + mv.y;

          mb->b_mvs[i].x = (mv.x)
            ?  mb->mvs[i].x - last_mb->mvs[i].x
            : last_mb->mvs[i].x*(dec->time_bp - dec->time_pp)/dec->time_pp;
          mb->b_mvs[i].y = (mv.y)
            ? mb->mvs[i].y - last_mb->mvs[i].y
            : last_mb->mvs[i].y*(dec->time_bp - dec->time_pp)/dec->time_pp;
        }
	p_dMB->bvop_direct = 1;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);
	break;
      case MODE_INTERPOLATE:
	//get mvs[],b_mvs,dec->p_fmv ,dec->p_bmv
        get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, x, y);
        dec->p_fmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];

        get_b_motion_vector(bs, &mb->b_mvs[0], fcode_backward, dec->p_bmv, dec, x, y);
        dec->p_bmv = mb->b_mvs[1] = mb->b_mvs[2] = mb->b_mvs[3] = mb->b_mvs[0];
	p_dMB->bvop_direct = 0;
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);
	break;
      case MODE_BACKWARD:
        get_b_motion_vector(bs, &mb->mvs[0], fcode_backward, dec->p_bmv, dec, x, y);
        dec->p_bmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,0,1);	
	break;
      case MODE_FORWARD:
        get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, x, y);
        dec->p_fmv = mb->mvs[1] = mb->mvs[2] = mb->mvs[3] = mb->mvs[0];
	decoder_mbinter_vld_v(dec,mb,mb->cbp,bs,0,1,1);	
	break;
      default:
        DPRINTF(XVID_DEBUG_ERROR,"Not supported B-frame mb_type = %i\n", mb->mode);
      }
    MOVE_DATA:
      POLLING_MDMA0_END();
      i_sync();
      fifo_pMB++;
      (*fifo_wp)++;      

      /* move data TCSM -> SDRAM */
      curr_len = next_len;
      next_len = dMB1->next_dmb_len;

      MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
      SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
      MDMA0_ARG->TSA = (uint32_t)dMB1;
      MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
      MDMA0_ARG->UNIT = MDMA_UNIT(1,64,curr_len);	
      MDMA0_RUN();
      XCHG3(dMB0,dMB1,dMB2,XCHGtmp);

    } /* End of for */

    uint8_t k;
    for(k = 0; k < (nfake); k++){
      p_dMB = dMB2;
      block = dMB2->residual;
      p_dMB->isfake = 1;			
      p_dMB->mode   = 0;		
      dMB1->next_dmb_len = 4; 		
      POLLING_MDMA0_END();
      i_sync();
      fifo_pMB++;
      (*fifo_wp)++;

      curr_len = next_len;
      next_len = dMB1->next_dmb_len;

      MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1+1024);
      SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
      MDMA0_ARG->TSA = (uint32_t)dMB1;
      MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
      MDMA0_ARG->UNIT = MDMA_UNIT(1,64,curr_len);
      MDMA0_RUN();
      XCHG3(dMB0,dMB1,dMB2,XCHGtmp);
    }
        
  }
 /* move last mb to sdram */
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;

  MDMA0_ARG = (M4_MDMA_DesNode *)((uint32_t)dMB1 + 1024);
  MDMA0_ARG->TSA = (uint32_t)dMB1;
  MDMA0_ARG->TDA = get_phy_addr(fifo_pMB);
  MDMA0_ARG->UNIT = MDMA_UNIT(1,64,next_len);
  SET_MDMA0_DHA((uint32_t)MDMA0_ARG);
  MDMA0_RUN();
  POLLING_MDMA0_END();
  fifo_pMB++;
  (*fifo_wp)++;

J1_PMON_OFF(vlc);
  /*insert fake task for AUX to finish its pipeline*/
  //fake task 1
  fifo_pMB->isfake = 1;
  fifo_pMB[-1].next_dmb_len = 4;
  i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
  i_sync();
  fifo_pMB++;
  (*fifo_wp)++;
  //fake task 2
  fifo_pMB->isfake = 1;
  fifo_pMB[-1].next_dmb_len = 4;
  i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
  i_sync();
  fifo_pMB++;
  (*fifo_wp)++;
  //fake task 3
  fifo_pMB->isfake = 1;
  fifo_pMB[-1].next_dmb_len = 4;
  i_cache(0x19,(int32_t)&fifo_pMB[-1], 0);
  i_cache(0x19,(int32_t)fifo_pMB,0);
  i_sync();
    
  fifo_pMB++;
  (*fifo_wp)++;
  (*fifo_wp)++;

  J1_PMON_ON(sync);
  do{}while(!(*task_done));
  J1_PMON_OFF(sync);
  *task_done = 0;
}

#else //JZC_TCSM0_OPT
//VER_200907007_yli_change_mc_gp1_polling_to_self_config:1.8
#endif //JZC_TCSM0_OPT
