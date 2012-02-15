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
 * $Id: xvid_dec_pbs.c,v 1.1.1.1 2009/10/13 06:37:38 gysun Exp $
 *
 ****************************************************************************/

/* decode an intra macroblock */
static void
decoder_mbintra_pbs(DECODER * dec,
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
  RESIDUAL *residual;
  FIFO_MB *cur_node;
  uint32_t ofst, i, j;
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  uint32_t iQuant = pMB->quant;

  DECLARE_ALIGNED_MATRIX(block, 6, 64, int16_t, CACHE_LINE);

  ofst = y_pos * dec->mb_height + x_pos;
  cur_node = &dec->fifo_node[ofst];
  residual = &dec->fifo_data[ofst];
  //fifo_runv temporary not used

  // generate node_header
  cur_node->mbtype = MBTYPE_MBINTRA;
  cur_node->mode = pMB->mode;
  cur_node->cbp = cbp;
  cur_node->quant = iQuant;
  cur_node->field_dct = pMB->field_dct;
  cur_node->yuv_mpos[0] = cur_node->yuv_mpos[1] = cur_node->yuv_mpos[2] = 0; //temporary not used
  cur_node->yuv_mpos[3] = cur_node->yuv_mpos[4] = cur_node->yuv_mpos[5] = 0; //temporary not used
  cur_node->x_pos = x_pos;
  cur_node->y_pos = y_pos;

  // generate vld_residual (up to now, dezigzag need be executed ahead of time)
  memset(block, 0, 6 * 64 * sizeof(int16_t)); /* clear */
  for (i = 0; i < 6; i++) {
    uint32_t iDcScaler = get_dc_scaler(iQuant, i < 4);
    int16_t predictors[8];
    int start_coeff;

    predict_acdc(dec->mbs, x_pos, y_pos, dec->mb_width, i, &block[i * 64],
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

      block[i * 64 + 0] = dc_dif;
      start_coeff = 1;

      DPRINTF(XVID_DEBUG_COEFF,"block[0] %i\n", dc_dif);
    } else {
      start_coeff = 0;
    }

    if (cbp & (1 << (5 - i))) /* coded */
    {
      int direction = dec->alternate_vertical_scan ? 2 : pMB->acpred_directions[i];
      get_intra_block(bs, &block[i * 64], direction, start_coeff);
    }
    cur_node->yuv_len[i]=64;
    add_acdc(pMB, i, &block[i * 64], iDcScaler, predictors, dec->bs_version);
    for (j=0; j<64; j++) {
      residual->data[i*64 + j] = block[i*64 + j];
    }
  }
}

/* decode an inter macroblock */
static void
decoder_mbinter_pbs(DECODER * dec,
		const MACROBLOCK * pMB,
		const uint32_t x_pos,
		const uint32_t y_pos,
		const uint32_t cbp,
		Bitstream * bs,
		const uint32_t rounding,
		const int ref,
		const int bvop)
{
  uint32_t i;
  VECTOR mv[4]; /* local copy of mvs */
  FIFO_MB *cur_node;

  i = y_pos * dec->mb_height + x_pos;
  cur_node = &dec->fifo_node[i];

  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];
  validate_vector(mv, x_pos, y_pos, dec);

  // generate node_header
  cur_node->mvs[0] = mv[0];
  cur_node->mvs[1] = mv[1];
  cur_node->mvs[2] = mv[2];
  cur_node->mvs[3] = mv[3];

  cur_node->mbtype = MBTYPE_MBINTER;
  cur_node->mode = pMB->mode;
  cur_node->cbp = cbp;
  cur_node->quant = pMB->quant;
  cur_node->bvop_direct = bvop;
  cur_node->direction = dec->alternate_vertical_scan ? 2 : 0;
  cur_node->ref = ref;
  cur_node->rounding = rounding;
  cur_node->field_dct = pMB->field_dct;
  cur_node->yuv_mpos[0] = cur_node->yuv_mpos[1] = cur_node->yuv_mpos[2] = 0;
  cur_node->yuv_mpos[3] = cur_node->yuv_mpos[4] = cur_node->yuv_mpos[5] = 0;
  cur_node->x_pos = x_pos;
  cur_node->y_pos = y_pos;
  // generate entropy_residual
  decoder_entropy_residual(cbp, bs, &dec->fifo_data[i], &dec->fifo_runv[i], cur_node);
}

/* decode an inter macroblock in field mode */
static void
decoder_mbinter_field_pbs(DECODER * dec,
		      const MACROBLOCK * pMB,
		      const uint32_t x_pos,
		      const uint32_t y_pos,
		      const uint32_t cbp,
		      Bitstream * bs,
		      const uint32_t rounding,
		      const int ref,
		      const int bvop)
{
  uint32_t i;
  VECTOR mv[4]; /* local copy of mvs */
  FIFO_MB *cur_node;

  i = y_pos * dec->mb_height + x_pos;
  cur_node = &dec->fifo_node[i];

  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  memset(&mv[2],0,2*sizeof(VECTOR));
  validate_vector(mv, x_pos, y_pos, dec);
  
  // generate node_header
  cur_node->mvs[0] = mv[0];
  cur_node->mvs[1] = mv[1];
  cur_node->mvs[2] = mv[2];
  cur_node->mvs[3] = mv[3];

  cur_node->mbtype = MBTYPE_MBINTER_FIELD;
  cur_node->mode = pMB->mode;
  cur_node->cbp = cbp;
  cur_node->quant = pMB->quant;
  cur_node->bvop_direct = bvop;
  cur_node->direction = dec->alternate_vertical_scan ? 2 : 0;
  cur_node->ref = ref;
  cur_node->rounding = rounding;
  cur_node->field_dct = pMB->field_dct;
  cur_node->field_for_top = pMB->field_for_top;
  cur_node->field_for_bot = pMB->field_for_bot;
  cur_node->yuv_mpos[0] = cur_node->yuv_mpos[1] = cur_node->yuv_mpos[2] = 0;
  cur_node->yuv_mpos[3] = cur_node->yuv_mpos[4] = cur_node->yuv_mpos[5] = 0;
  cur_node->x_pos = x_pos;
  cur_node->y_pos = y_pos;

  // generate entropy_residual
  decoder_entropy_residual(cbp, bs, &dec->fifo_data[i], &dec->fifo_runv[i], cur_node);
}

static void
decoder_mbgmc_pbs(DECODER * dec,
        MACROBLOCK * const pMB,
        const uint32_t x_pos,
        const uint32_t y_pos,
        const uint32_t fcode,
        const uint32_t cbp,
        Bitstream * bs,
        const uint32_t rounding)
{
  const uint32_t stride = dec->edged_width;
  const uint32_t stride2 = stride / 2;

  uint8_t *const pY_Cur=dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  uint8_t *const pU_Cur=dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  uint8_t *const pV_Cur=dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

  NEW_GMC_DATA * gmc_data = &dec->new_gmc_data;

  pMB->mvs[0] = pMB->mvs[1] = pMB->mvs[2] = pMB->mvs[3] = pMB->amv;

  start_timer();

/* this is where the calculations are done */

  gmc_data->predict_16x16(gmc_data,
      dec->cur.y + y_pos*16*stride + x_pos*16, dec->refn[0].y,
      stride, stride, x_pos, y_pos, rounding);

  gmc_data->predict_8x8(gmc_data,
      dec->cur.u + y_pos*8*stride2 + x_pos*8, dec->refn[0].u,
      dec->cur.v + y_pos*8*stride2 + x_pos*8, dec->refn[0].v,
      stride2, stride2, x_pos, y_pos, rounding);

  gmc_data->get_average_mv(gmc_data, &pMB->amv, x_pos, y_pos, dec->quarterpel);

  pMB->amv.x = gmc_sanitize(pMB->amv.x, dec->quarterpel, fcode);
  pMB->amv.y = gmc_sanitize(pMB->amv.y, dec->quarterpel, fcode);

  pMB->mvs[0] = pMB->mvs[1] = pMB->mvs[2] = pMB->mvs[3] = pMB->amv;

  stop_transfer_timer();

  if (cbp)
    decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);

}

/* decode an B-frame direct & interpolate macroblock */
static void
decoder_bf_interpolate_mbinter_pbs(DECODER * dec,
				   IMAGE forward,
				   IMAGE backward,
				   MACROBLOCK * pMB,
				   const uint32_t x_pos,
				   const uint32_t y_pos,
				   Bitstream * bs,
				   const int direct)
{
  const uint32_t cbp = pMB->cbp;
  uint32_t i;
  VECTOR mv[4]; /* local copy of mvs */
  FIFO_MB *cur_node;

  i = y_pos * dec->mb_height + x_pos;
  cur_node = &dec->fifo_node[i];

  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];
  validate_vector(mv, x_pos, y_pos, dec);
  cur_node->mvs[0] = mv[0];
  cur_node->mvs[1] = mv[1];
  cur_node->mvs[2] = mv[2];
  cur_node->mvs[3] = mv[3];

  mv[0] = pMB->b_mvs[0];
  mv[1] = pMB->b_mvs[1];
  mv[2] = pMB->b_mvs[2];
  mv[3] = pMB->b_mvs[3];
  validate_vector(mv, x_pos, y_pos, dec);
  cur_node->b_mvs[0] = mv[0];
  cur_node->b_mvs[1] = mv[1];
  cur_node->b_mvs[2] = mv[2];
  cur_node->b_mvs[3] = mv[3];

  // generate node_header
  cur_node->mbtype = MBTYPE_BF_MBINTER;
  cur_node->mode = pMB->mode;
  cur_node->cbp = cbp;
  cur_node->quant = pMB->quant;
  cur_node->bvop_direct = direct;
  cur_node->direction = dec->alternate_vertical_scan ? 2 : 0;
  cur_node->field_dct = pMB->field_dct;
  cur_node->yuv_mpos[0] = cur_node->yuv_mpos[1] = cur_node->yuv_mpos[2] = 0;
  cur_node->yuv_mpos[3] = cur_node->yuv_mpos[4] = cur_node->yuv_mpos[5] = 0;
  cur_node->x_pos = x_pos;
  cur_node->y_pos = y_pos;
  // generate entropy_residual
  decoder_entropy_residual(cbp, bs, &dec->fifo_data[i], &dec->fifo_runv[i], cur_node);
}

/*JZ Dcore aux CPU global arguments init*/
static inline void 
aux_frame_arg_init(DECODER * dec)
{
#define SD_MBNUM 2304
  uint32_t total_size; /*total size for task fifo allocation*/
  FIFO_DECODER *xvid_dFRM;
  xvid_dFRM = (FIFO_DECODER *)TCSM1_AUX_FRM_ARG;
  dec->total_mbnum = SD_MBNUM;
  total_size = ((dec->total_mbnum+2)*sizeof(RESIDUAL) + JZ_CACHELINE +
		(dec->total_mbnum+2)*sizeof(RUNV) + JZ_CACHELINE +
		(dec->total_mbnum+2)*sizeof(FIFO_MB) + JZ_CACHELINE
		);
  dcore_fifo = XVIDMALLOC(total_size, 0);
  dec->fifo_data = (RESIDUAL *)ALIGN32(dcore_fifo);
  dec->fifo_runv = (RUNV *)ALIGN32(dec->fifo_data + (dec->total_mbnum+2)*sizeof(RESIDUAL));
  dec->fifo_node = (FIFO_MB *)ALIGN32(dec->fifo_runv + (dec->total_mbnum+2)*sizeof(RUNV));

  xvid_dFRM->fifo_data = (RESIDUAL *)dec->fifo_data;
  xvid_dFRM->fifo_runv = (RUNV *)dec->fifo_runv;
  xvid_dFRM->fifo_node = (FIFO_MB *)dec->fifo_node;
}

static void 
aux_frame_arg_set(DECODER * dec)
{
  FIFO_DECODER *xvid_dFRM;
  xvid_dFRM = (FIFO_DECODER *)TCSM1_AUX_FRM_ARG;
  if((dec->mb_width*dec->mb_height) > dec->total_mbnum){
    uint32_t total_size; /*total size for task fifo allocation*/
    dec->total_mbnum = (dec->mb_width*dec->mb_height);
    XVIDFREE(dcore_fifo);

    total_size = ((dec->total_mbnum+2)*sizeof(RESIDUAL) + JZ_CACHELINE +
		  (dec->total_mbnum+2)*sizeof(RUNV) + JZ_CACHELINE +
		  (dec->total_mbnum+2)*sizeof(FIFO_MB) + JZ_CACHELINE
		  );
    dcore_fifo = XVIDMALLOC(total_size, 0);
    dec->fifo_data = (RESIDUAL *)ALIGN32(dcore_fifo);
    dec->fifo_runv = (RUNV *)ALIGN32(dec->fifo_data + (dec->total_mbnum+2)*sizeof(RESIDUAL));
    dec->fifo_node = (FIFO_MB *)ALIGN32(dec->fifo_runv + (dec->total_mbnum+2)*sizeof(RUNV));

    xvid_dFRM->fifo_data = (RESIDUAL *)dec->fifo_data;
    xvid_dFRM->fifo_runv = (RUNV *)dec->fifo_runv;
    xvid_dFRM->fifo_node = (FIFO_MB *)dec->fifo_node;
  }

  xvid_dFRM->quant_type = dec->quant_type;
  xvid_dFRM->interlacing = dec->interlacing;
  xvid_dFRM->bs_version = dec->bs_version;
  xvid_dFRM->quarterpel = dec->quarterpel;
  xvid_dFRM->edged_width = dec->edged_width;
  xvid_dFRM->mb_width = dec->mb_width;
}

static inline void 
aux_frame_arg_delete()
{
  XVIDFREE(dcore_fifo);
}
