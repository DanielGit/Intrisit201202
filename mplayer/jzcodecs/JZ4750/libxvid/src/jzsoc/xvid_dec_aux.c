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
 * xvid_dec_aux.c
 *   -- In JZ Dcore Arch, it servers for aux CPU decode task from main CPU
 *      and manage JZ media HW core/data flow
 *
 * $Id: xvid_dec_aux.c,v 1.1.1.1 2009/10/13 06:37:38 gysun Exp $
 *
 ****************************************************************************/

static void
decoder_mbintra_2pass(FIFO_DECODER const *dec,
		      FIFO_MB const *pMB
		      )
{
  uint32_t i;
  uint32_t next_block;
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  const uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
  const uint32_t iQuant = pMB->quant;
  const uint32_t x_pos = pMB->x_pos;
  const uint32_t y_pos = pMB->y_pos;
  const uint32_t ofst = y_pos * dec->mb_width + x_pos;
  
  RESIDUAL *residual = &dec->fifo_data[ofst];

  for (i=0; i<6; i++) {
    uint32_t iDcScaler = get_dc_scaler(iQuant, i < 4);
    int16_t * const data = &(residual->data[i * 64]);
    if (dec->quant_type == 0) {
      dequant_h263_intra(data, iQuant, iDcScaler, dec->mpeg_quant_matrices);
    } else {
      dequant_mpeg_intra(data, iQuant, iDcScaler, dec->mpeg_quant_matrices);
    }
    idct(data);
  }

  if (dec->interlacing && pMB->field_dct) {
    next_block = stride;
    stride *= 2;
  } else {
    next_block = stride * 8;
  }

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);
  transfer_16to8copy(pY_Cur, &residual->data[0 * 64], stride);
  transfer_16to8copy(pY_Cur + 8, &residual->data[1 * 64], stride);
  transfer_16to8copy(pY_Cur + next_block, &residual->data[2 * 64], stride);
  transfer_16to8copy(pY_Cur + 8 + next_block, &residual->data[3 * 64], stride);
  transfer_16to8copy(pU_Cur, &residual->data[4 * 64], stride2);
  transfer_16to8copy(pV_Cur, &residual->data[5 * 64], stride2);
}

static void
decoder_mb_decode_2pass(FIFO_DECODER const *dec,
			const uint32_t cbp,
			FIFO_MB const *pMB)
{
  DECLARE_ALIGNED_MATRIX(data, 1, 64, int16_t, CACHE_LINE);
  int i, step;
  uint8_t * const pY_Cur;
  uint8_t * const pU_Cur;
  uint8_t * const pV_Cur;
  int16_t const *residual const;
  uint8_t const *runv const;
  const uint32_t mb_height = dec->mb_height;
  const uint32_t mb_xpos = pMB->x_pos;
  const uint32_t mb_ypos = pMB->y_pos;
  int32_t stride = dec->edged_width;
  const uint32_t iQuant = pMB->quant;
  const uint32_t direction = pMB->direction;
  const uint32_t ofst = y_pos * mb_height + x_pos;
  residual = (int16_t *)(&dec->fifo_data[ofst]);
  runv = (uint8_t *)(&dec->fifo_runv[ofst]);

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * (stride/2) + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * (stride/2) + (x_pos << 3);

  typedef void (*get_iq_coeffs_function_t)(
					   int16_t * const data,
					   int16_t const * residual const,
					   uint8_t const * runv const,
					   const uint32_t yuv_len,
					   uint8_t * const yuv_mpos,
					   const int direction,
					   const int quant,
					   uint16_t const *matrix const);

  const get_iq_coeffs_function_t get_iq_coeffs = (dec->quant_type == 0)
    ? (get_iq_coeffs_function_t)get_iq_coeffs_h263
    : (get_iq_coeffs_function_t)get_iq_coeffs_mepg;

  uint8_t *dst[6];
  int32_t strides[6];
  if (dec->interlacing && pMB->field_dct) {
    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 8;
    dst[2] = pY_Cur + stride;
    dst[3] = dst[2] + 8;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;
    strides[0] = strides[1] = strides[2] = strides[3] = stride*2;
    strides[4] = stride/2;
    strides[5] = stride/2;
  } else {
    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 8;
    dst[2] = pY_Cur + 8*stride;
    dst[3] = dst[2] + 8;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;
    strides[0] = strides[1] = strides[2] = strides[3] = stride;
    strides[4] = stride/2;
    strides[5] = stride/2;
  }

  step = 0;
  for (i = 0; i < 6; i++) {
    /* Process only coded blocks */
    if (cbp & (1 << (5 - i))) {
      const uint32_t yuv_len = (uint32_t)pMB->yuv_len[i];
      uint8_t yuv_mpos;

      /* Clear the block */
      memset(&data[0], 0, 64*sizeof(int16_t));

      /*dezigzag and dequantize on the fly */
      get_iq_coeffs(&data[0], &residual[step], &runv[step], yuv_len, &yuv_mpos, direction, iQuant, 
		    dec->mpeg_quant_matrices + 4*64);
      step += yuv_len;

      /* iDCT */
      idct((int16_t * const)&data[0]);

      /* Add this residual to the predicted block */
      transfer_16to8add(dst[i], &data[0], strides[i]);
    }
  }
}

static void
decoder_mbinter_2pass(FIFO_DECODER const *dec,
		      FIFO_MB const *pMB)
{
  //  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
  VECTOR mv[4]; /* local copy of mvs */
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;

  const uint32_t xpos = pMB->x_pos;
  const uint32_t ypos = pMB->y_pos;
  const uint32_t rounding = pMB->rounding;
  const int32_t ref = pMB->ref;
  const int32_t bvop = pMB->bvop_direct;

  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];

  /* Get pointer to memory areas */
  //  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  //  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  //  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

  if ((pMB->mode != MODE_INTER4V) || (bvop)) { /* INTER, INTER_Q, NOT_CODED, FORWARD, BACKWARD */

    uv_dx = mv[0].x;
    uv_dy = mv[0].y;
    if (dec->quarterpel) {
			if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
  				uv_dx = (uv_dx>>1) | (uv_dx&1);
				uv_dy = (uv_dy>>1) | (uv_dy&1);
			}
			else {
        uv_dx /= 2;
        uv_dy /= 2;
      }
    }
    uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
    uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];

    if (dec->quarterpel)
      interpolate16x16_quarterpel(dec->cur.y, dec->refn[ref].y, dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                      mv[0].x, mv[0].y, stride, rounding);
    else
      interpolate16x16_switch(dec->cur.y, dec->refn[ref].y, 16*x_pos, 16*y_pos,
                  mv[0].x, mv[0].y, stride, rounding);

  } else {  /* MODE_INTER4V */

    if(dec->quarterpel) {
			if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
				int z;
				uv_dx = 0; uv_dy = 0;
				for (z = 0; z < 4; z++) {
				  uv_dx += ((mv[z].x>>1) | (mv[z].x&1));
				  uv_dy += ((mv[z].y>>1) | (mv[z].y&1));
				}
			}
			else {
        uv_dx = (mv[0].x / 2) + (mv[1].x / 2) + (mv[2].x / 2) + (mv[3].x / 2);
        uv_dy = (mv[0].y / 2) + (mv[1].y / 2) + (mv[2].y / 2) + (mv[3].y / 2);
      }
    } else {
      uv_dx = mv[0].x + mv[1].x + mv[2].x + mv[3].x;
      uv_dy = mv[0].y + mv[1].y + mv[2].y + mv[3].y;
    }

    uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
    uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];

    if (dec->quarterpel) {
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                  mv[0].x, mv[0].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
                  mv[1].x, mv[1].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
                  mv[2].x, mv[2].y, stride, rounding);
      interpolate8x8_quarterpel(dec->cur.y, dec->refn[0].y , dec->qtmp.y, dec->qtmp.y + 64,
                  dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
                  mv[3].x, mv[3].y, stride, rounding);
    } else {
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos, 16*y_pos,
                mv[0].x, mv[0].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos + 8, 16*y_pos,
                mv[1].x, mv[1].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos, 16*y_pos + 8,
                mv[2].x, mv[2].y, stride, rounding);
      interpolate8x8_switch(dec->cur.y, dec->refn[0].y , 16*x_pos + 8, 16*y_pos + 8,
                mv[3].x, mv[3].y, stride, rounding);
    }
  }

  /* chroma */
  interpolate8x8_switch(dec->cur.u, dec->refn[ref].u, 8 * x_pos, 8 * y_pos,
              uv_dx, uv_dy, stride2, rounding);
  interpolate8x8_switch(dec->cur.v, dec->refn[ref].v, 8 * x_pos, 8 * y_pos,
              uv_dx, uv_dy, stride2, rounding);

  if (cbp)
    //    decoder_mb_decode_2pass(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
    decoder_mb_decode_2pass(dec, cbp, pMB);
}

static void
decoder_mbinter_field_2pass(FIFO_DECODER const * dec,
			    FIFO_MB const *pMB)
{
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;

  const uint32_t xpos = pMB->x_pos;
  const uint32_t ypos = pMB->y_pos;
  const uint32_t rounding = pMB->rounding;
  const int32_t ref = pMB->ref;
  const int32_t bvop = pMB->bvop_direct;

  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

  int uvtop_dx, uvtop_dy;
  int uvbot_dx, uvbot_dy;
  VECTOR mv[4]; /* local copy of mvs */

  /* Get pointer to memory areas */
  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);
  
  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];

  if((pMB->mode!=MODE_INTER4V) || (bvop))   /* INTER, INTER_Q, NOT_CODED, FORWARD, BACKWARD */
  { 
    /* Prepare top field vector */
    uvtop_dx = DIV2ROUND(mv[0].x);
    uvtop_dy = DIV2ROUND(mv[0].y);

    /* Prepare bottom field vector */
    uvbot_dx = DIV2ROUND(mv[1].x);
    uvbot_dy = DIV2ROUND(mv[1].y);

    if(dec->quarterpel)
    {
      /* NOT supported */
    }
    else
    {
      /* Interpolate top field left part(we use double stride for every 2nd line) */
      interpolate8x8_switch(dec->cur.y,dec->refn[ref].y+pMB->field_for_top*stride,
                            16*x_pos,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);
      /* top field right part */
      interpolate8x8_switch(dec->cur.y,dec->refn[ref].y+pMB->field_for_top*stride,
                            16*x_pos+8,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);

      /* Interpolate bottom field left part(we use double stride for every 2nd line) */
      interpolate8x8_switch(dec->cur.y+stride,dec->refn[ref].y+pMB->field_for_bot*stride,
                            16*x_pos,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);
      /* Bottom field right part */
      interpolate8x8_switch(dec->cur.y+stride,dec->refn[ref].y+pMB->field_for_bot*stride,
                            16*x_pos+8,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);

      /* Interpolate field1 U */
      interpolate8x4_switch(dec->cur.u,dec->refn[ref].u+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
      
      /* Interpolate field1 V */
      interpolate8x4_switch(dec->cur.v,dec->refn[ref].v+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
    
      /* Interpolate field2 U */
      interpolate8x4_switch(dec->cur.u+stride2,dec->refn[ref].u+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    
      /* Interpolate field2 V */
      interpolate8x4_switch(dec->cur.v+stride2,dec->refn[ref].v+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    }
  } 
  else 
  {
    /* We don't expect 4 motion vectors in interlaced mode */
  }

  /* Must add error correction? */
  if(cbp)
    //   decoder_mb_decode_2pass(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
   decoder_mb_decode_2pass(dec, cbp, pMB);
}


static void
decoder_bf_interpolate_mbinter_2pass(FIFO_DECODER const *dec,
				     FIFO_MB const *pMB,
				     const uint32_t x_pos,
				     const uint32_t y_pos,
				     const int direct)
{
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  int uv_dx, uv_dy;
  int b_uv_dx, b_uv_dy;
  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
  const uint32_t cbp = pMB->cbp;

  IMAGE forward = dec->refn[1];
  IMAGE backward = dec->refn[0];

  pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

  if (!direct) {
    uv_dx = pMB->mvs[0].x;
    uv_dy = pMB->mvs[0].y;
    b_uv_dx = pMB->b_mvs[0].x;
    b_uv_dy = pMB->b_mvs[0].y;

    if (dec->quarterpel) {
      if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
	uv_dx = (uv_dx>>1) | (uv_dx&1);
	uv_dy = (uv_dy>>1) | (uv_dy&1);
	b_uv_dx = (b_uv_dx>>1) | (b_uv_dx&1);
	b_uv_dy = (b_uv_dy>>1) | (b_uv_dy&1);
      }
      else {
        uv_dx /= 2;
        uv_dy /= 2;
        b_uv_dx /= 2;
        b_uv_dy /= 2;
      }
    }

    uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
    uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];
    b_uv_dx = (b_uv_dx >> 1) + roundtab_79[b_uv_dx & 0x3];
    b_uv_dy = (b_uv_dy >> 1) + roundtab_79[b_uv_dy & 0x3];

  } else {
    if (dec->quarterpel) { /* for qpel the /2 shall be done before summation. We've done it right in the encoder in the past. */
      /* TODO: figure out if we ever did it wrong on the encoder side. If yes, add some workaround */
      if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING) {
	int z;
	uv_dx = 0; uv_dy = 0;
	b_uv_dx = 0; b_uv_dy = 0;
	for (z = 0; z < 4; z++) {
	  uv_dx += ((pMB->mvs[z].x>>1) | (pMB->mvs[z].x&1));
	  uv_dy += ((pMB->mvs[z].y>>1) | (pMB->mvs[z].y&1));
	  b_uv_dx += ((pMB->b_mvs[z].x>>1) | (pMB->b_mvs[z].x&1));
	  b_uv_dy += ((pMB->b_mvs[z].y>>1) | (pMB->b_mvs[z].y&1));
	}
      }
      else {
	uv_dx = (pMB->mvs[0].x / 2) + (pMB->mvs[1].x / 2) + (pMB->mvs[2].x / 2) + (pMB->mvs[3].x / 2);
	uv_dy = (pMB->mvs[0].y / 2) + (pMB->mvs[1].y / 2) + (pMB->mvs[2].y / 2) + (pMB->mvs[3].y / 2);
	b_uv_dx = (pMB->b_mvs[0].x / 2) + (pMB->b_mvs[1].x / 2) + (pMB->b_mvs[2].x / 2) + (pMB->b_mvs[3].x / 2);
	b_uv_dy = (pMB->b_mvs[0].y / 2) + (pMB->b_mvs[1].y / 2) + (pMB->b_mvs[2].y / 2) + (pMB->b_mvs[3].y / 2);
      } 
    } else {
      uv_dx = pMB->mvs[0].x + pMB->mvs[1].x + pMB->mvs[2].x + pMB->mvs[3].x;
      uv_dy = pMB->mvs[0].y + pMB->mvs[1].y + pMB->mvs[2].y + pMB->mvs[3].y;
      b_uv_dx = pMB->b_mvs[0].x + pMB->b_mvs[1].x + pMB->b_mvs[2].x + pMB->b_mvs[3].x;
      b_uv_dy = pMB->b_mvs[0].y + pMB->b_mvs[1].y + pMB->b_mvs[2].y + pMB->b_mvs[3].y;
    }

    uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
    uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];
    b_uv_dx = (b_uv_dx >> 3) + roundtab_76[b_uv_dx & 0xf];
    b_uv_dy = (b_uv_dy >> 3) + roundtab_76[b_uv_dy & 0xf];
  }

  if(dec->quarterpel) {
    if(!direct) {
      interpolate16x16_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
				  dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
				  pMB->mvs[0].x, pMB->mvs[0].y, stride, 0);
    } else {
      interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
				dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
				pMB->mvs[0].x, pMB->mvs[0].y, stride, 0);
      interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
				dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
				pMB->mvs[1].x, pMB->mvs[1].y, stride, 0);
      interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
				dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
				pMB->mvs[2].x, pMB->mvs[2].y, stride, 0);
      interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
				dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
				pMB->mvs[3].x, pMB->mvs[3].y, stride, 0);
    }
  } else {
    interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos, 16 * y_pos,
			  pMB->mvs[0].x, pMB->mvs[0].y, stride, 0);
    interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos + 8, 16 * y_pos,
			  pMB->mvs[1].x, pMB->mvs[1].y, stride, 0);
    interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos, 16 * y_pos + 8,
			  pMB->mvs[2].x, pMB->mvs[2].y, stride, 0);
    interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos + 8, 16 * y_pos + 8,
			  pMB->mvs[3].x, pMB->mvs[3].y, stride, 0);
  }

  interpolate8x8_switch(dec->cur.u, forward.u, 8 * x_pos, 8 * y_pos, uv_dx,
			uv_dy, stride2, 0);
  interpolate8x8_switch(dec->cur.v, forward.v, 8 * x_pos, 8 * y_pos, uv_dx,
			uv_dy, stride2, 0);


  if(dec->quarterpel) {
    if(!direct) {
      interpolate16x16_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
				      dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
				      pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0);
    } else {
      interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
				    dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
				    pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0);
      interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
				    dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
				    pMB->b_mvs[1].x, pMB->b_mvs[1].y, stride, 0);
      interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
				    dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
				    pMB->b_mvs[2].x, pMB->b_mvs[2].y, stride, 0);
      interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
				    dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
				    pMB->b_mvs[3].x, pMB->b_mvs[3].y, stride, 0);
    }
  } else {
    interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos, 16 * y_pos,
			      pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0);
    interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos + 8,
			      16 * y_pos, pMB->b_mvs[1].x, pMB->b_mvs[1].y, stride, 0);
    interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos,
			      16 * y_pos + 8, pMB->b_mvs[2].x, pMB->b_mvs[2].y, stride, 0);
    interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos + 8,
			      16 * y_pos + 8, pMB->b_mvs[3].x, pMB->b_mvs[3].y, stride, 0);
  }

  interpolate8x8_add_switch(dec->cur.u, backward.u, 8 * x_pos, 8 * y_pos,
			    b_uv_dx, b_uv_dy, stride2, 0);
  interpolate8x8_add_switch(dec->cur.v, backward.v, 8 * x_pos, 8 * y_pos,
			    b_uv_dx, b_uv_dy, stride2, 0);

  if (cbp)
    decoder_mb_decode_2pass(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
}
