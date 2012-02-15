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
 * $Id: xvid_arch.h,v 1.11 2009/06/30 03:22:11 yli Exp $
 *
 ****************************************************************************/
#ifndef _XVIDARCH_H_
#define _XVIDARCH_H_

#define MBTYPE_MBINTER 0
#define MBTYPE_MBINTRA 1
#define MBTYPE_MBINTER_FIELD 2
#define MBTYPE_BF_MBINTER 3
#define MBTYPE_MBGMC 4

#define int32_t int
#define uint32_t unsigned int
#define int16_t short
#define uint16_t unsigned short
#define int8_t char
#define uint8_t unsigned char

#define TRUE 1
#define NULL 0

typedef struct
{
  int16_t x;
  int16_t y;
}
VECTOR_AUX;

typedef struct
{
  /*global*/
  uint8_t isfake; /*fake mb task flag*/
  uint8_t mode;
  int16_t next_dmb_len;

  int16_t lastmb_mode;/*last_mb->mode*/  
  uint8_t cbp;
  uint8_t quant;		/* absolute quant */

  /*mb specific*/
  uint8_t bvop_direct;
  uint8_t direction; //for zigzig scan_tables
  uint8_t ref;
  uint8_t rounding;

  /*field */  

  /*mb residual specific*/
  uint8_t yuv_len[6];
  uint8_t field_dct;
  uint8_t field_for_top;
  
  uint8_t yuv_mpos[6];
  uint8_t field_for_bot;
  //  uint8_t pvop_intra;/*intra mb in P_VOP*/
  uint8_t field_pred;/* field in P_VOP*/
 
  int32_t dummy; //ensure residual be cacheline aligned

  /* vectors */
  VECTOR_AUX mvs[4]; //NOTES: VECTOR must be {short y, short x}
  VECTOR_AUX b_mvs[4];

  int16_t residual[6*64];
  //uint8_t runv[6*64];

}FIFO_MB; /* 304 word 1216 byte */

typedef struct
{
  // uint16_t *mpeg_quant_matrices;  
  uint32_t quant_type;
  int32_t interlacing;  
  int32_t bs_version;  
  int32_t quarterpel;
  uint32_t edged_width;

  uint32_t coding_type;//B/P
  uint16_t mb_width;
  uint16_t mb_height;
  
  uint8_t *cur[3];
  uint8_t *refn[2][3];
  #ifdef JZC_MDMA1_OPT
  //
  #else
  uint8_t *qtmp[3];   /*maybe can delete */
  #endif

}FIFO_DECODER; /* 16 word */

#define AUX_VADDR    0xB3800000
#define AUX_START()               \
({                                \
  volatile uint32_t *addr;             \
  addr = (uint32_t *)AUX_VADDR;         \
  *addr = 2;                      \
})

#define TCSM1_VADDR(addr)            ((addr & 0x0000FFFF) | 0x93800000)
#define TCSM1_VADDR_UNCACHE(addr)    ((addr & 0x0000FFFF) | 0xB3800000)


#endif /*_XVIDARCH_H_*/
