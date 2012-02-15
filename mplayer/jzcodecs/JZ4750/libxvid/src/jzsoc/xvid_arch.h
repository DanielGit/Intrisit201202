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
 * xvidarch.h
 *   -- It defines JZ Dcore Arch XviD MB/Frame level struct
 *
 * $Id: xvid_arch.h,v 1.1.1.1 2009/10/13 06:37:38 gysun Exp $
 *
 ****************************************************************************/
#ifndef _XVIDARCH_H_
#define _XVIDARCH_H_

#include "portab.h"
#include "global.h"
#include "tcsm.h"

#define MBTYPE_MBINTER 0
#define MBTYPE_MBINTRA 1
#define MBTYPE_MBINTER_FIELD 2
#define MBTYPE_BF_MBINTER 3
#define MBTYPE_MBGMC 4

typedef struct
{
  int16_t data[6*64];
}RESIDUAL;

typedef struct
{
  uint8_t data[6*64];
}RUNV;

typedef struct
{
  /*global*/
  uint8_t mbtype;
  uint8_t mode;
  uint8_t cbp;
  uint8_t quant;		/* absolute quant */

  /*mb specific*/
  uint8_t bvop_direct; //0|1, bvop for non-bframe, direct for bframe
  uint8_t direction; //for dequant
  uint8_t ref; //0|1 for dec->ref[ref]
  uint8_t rounding;

  /*field */
  uint8_t field_dct;
  uint8_t field_for_top;
  uint8_t field_for_bot;
  uint8_t reserved_2;

  /*mb residual specific*/
  uint8_t yuv_len[6];
  uint8_t yuv_mpos[6];

  int16_t x_pos;
  int16_t y_pos;

  int16_t reserved_3;  
  int16_t nex_residual_len;
  //up to here, total 8 words

  /* vectors */
  VECTOR mvs[4]; //NOTES: VECTOR must be {short y, short x}
  VECTOR b_mvs[4];
  //fixed header size=16 words
}FIFO_MB;

typedef struct
{
  uint16_t *mpeg_quant_matrices;  
  uint32_t quant_type;
  int32_t interlacing;  
  int32_t bs_version;  
  int32_t quarterpel;
  uint32_t edged_width;
  uint32_t mb_width;
  RESIDUAL *fifo_data;
  RUNV *fifo_runv;
  FIFO_MB *fifo_node;
}FIFO_DECODER;

#endif /*_XVIDARCH_H_*/
