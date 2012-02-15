#ifndef __JZC_TCSM_H__
#define __JZC_TCSM_H__
/* TCSM space defined here*/
#define TCSM_BASE 0xF4000000
#define TCSM_BANK0 0xF4000000
#define TCSM_BANK1 0xF4001000
#define TCSM_BANK2 0xF4002000
#define TCSM_BANK3 0xF4003000

#define SPACE_TYPE_CHAR 0x0
#define SPACE_TYPE_SHORT 0x1
#define SPACE_TYPE_WORD 0x2
#define MB_LUMA_SIZE 256
#define MB_CHROM_SIZE 64
#define MB_LUMA_EADGE_SIZE 64
#define MB_CHROM_EADGE_SIZE 32
#define MB_LUMA_WIDTH 16
#define MB_CHROM_WIDTH 8
#define MB_LUMA_EDGED_WIDTH (MB_LUMA_WIDTH + 4)
#define MB_CHROM_EDGED_WIDTH (MB_CHROM_WIDTH + 4)
#define IDCT_BUF_STRIDE ((16 + 4 + 4)*4)<<SPACE_TYPE_SHORT
#define H264_IDCT_DES_NODE_LEN 7
#define H264_IDCT_MB_BLK_NUM (16 + 4 + 4)
#define MB_BUF_SIZE (MB_LUMA_SIZE + MB_CHROM_SIZE*2)
#define IDCT_BUF_SIZE (MB_LUMA_SIZE + MB_CHROM_SIZE*2 + MB_LUMA_EADGE_SIZE + MB_CHROM_EADGE_SIZE*2)
#define H264BS_MAX_FRAME_WIDTH 720
#define FUNC_SURPLUS 2
#if 0
/*Key decode func pointer & space*/
typedef void (__FILL_CACHES__) (H264Context *h, int mb_type, int for_deblock);
typedef __FILL_CACHES__ *__FILL_CACHES_PTR__;
__FILL_CACHES__ static fill_caches;
__FILL_CACHES_PTR__ fill_caches_ptr;
#define TCSM_fill_caches TCSM_BANK0
#define fill_caches_LEN 666

typedef void (__HL_DECODE_MB_SIMPLE__) (H264Context *h);
typedef __HL_DECODE_MB_SIMPLE__ *__HL_DECODE_MB_SIMPLE_PTR__;
__HL_DECODE_MB_SIMPLE__ static hl_decode_mb_simple;
__HL_DECODE_MB_SIMPLE_PTR__ hl_decode_mb_simple_ptr;
#define TCSM_hl_decode_mb_simple (TCSM_fill_caches + ((fill_caches_LEN + FUNC_SURPLUS)<<2))
#define hl_decode_mb_simple_LEN 676

typedef void (__HL_MOTION_HW__) (H264Context *h);
typedef __HL_MOTION_HW__ *__HL_MOTION_HW_PTR__;
__HL_MOTION_HW__ static hl_motion_hw;
__HL_MOTION_HW_PTR__ hl_motion_hw_ptr;
#define TCSM_hl_motion_hw (TCSM_hl_decode_mb_simple + ((hl_decode_mb_simple_LEN + FUNC_SURPLUS)<<2))
#define hl_motion_hw_LEN 496
#define TCSM_FUNC_SPACE_END (TCSM_hl_motion_hw + ((hl_motion_hw_LEN + FUNC_SURPLUS)<<2))
#endif
/*buffer allocate*/
/*IDCT Buffer: stores IDCT result*/
/* +------+------+--...--+------+------+--...--+------+------+--...--+------+
   |  y0  |  y1  |  ...  | y15  |  u0  |  ...  |  u3  |  v0  |  ...  |  v3  |
   |(4x4) |      |       |      |      |       |      |      |       |      |
   |      |      |       |      |      |       |      |      |       |      |
   +------+------+--...--+------+------+--...--+------+------+--...--+------+
   "one-line" data structure:
   y0 start addr: TCSM_IDCT_BUF;
   y1 start addr: TCSM_IDCT_BUF + (4<<SPACE_TYPE_SHORT)
   u0 start addr: TCSM_IDCT_BUF + ( (16*4)<<SPACE_TYPE_SHORT)
   ...
   the buffer stride is ((16 + 4 + 4)*4)<<SPACE_TYPE_SHORT
 */
#define TCSM_IDCT_BUF TCSM_BANK2
#define IDCT_BUF_LEN (IDCT_BUF_SIZE <<SPACE_TYPE_SHORT)
#define IDCT_BUF_END (TCSM_IDCT_BUF+IDCT_BUF_LEN)
/*Block Buffer: stores data after CAVLC/IQ*/
/*it is structured same as IDCT Buffer*/
#define TCSM_BLOCK_BUF (TCSM_IDCT_BUF + IDCT_BUF_LEN)
#define BLOCK_BUF_LEN (MB_BUF_SIZE<<SPACE_TYPE_SHORT)
#define BLOCK_BUF_END (TCSM_BLOCK_BUF+BLOCK_BUF_LEN)
/*Pixel Buffer: stores reconstructed data,include left-4 edge for de-block*/
/*RECON Buffer: stores MC or Intra-Pred result, its ping-pong structured*/
/* +-----------------+-----------------------------------+
   |<-LEFT_EDGE_BUF->|<------------RECON_BUF------------>|
   |      4byte      |         16(luma)/8(chrom)byte     |
   | .               |                                   |
   | .               |                                   |
   | .               |                                   |
   |total: 16(luma)/8(chrom)line                         |
   +-----------------+-----------------------------------+ */
/* RECON Buffer is sturctured as (Raster-Order):
   +---+---+---+---+
   |y0 |y1 |y2 |y3 |
   +---+---+---+---+
   |y4 |y5 |y6 |y7 |
   +---+---+---+---+
   |y8 |y9 |y10|y11|
   +---+---+---+---+
   |y12|y13|y14|y15|
   +---+---+---+---+
   +---+---+
   |u0 |u1 |
   +---+---+
   |u2 |u3 |
   +---+---+
   +---+---+
   |v0 |v1 |
   +---+---+
   |v2 |v3 |
   +---+---+
*/
#define TCSM_PIXEL_BUF0 (TCSM_BLOCK_BUF + BLOCK_BUF_LEN)
#define PIXEL_BUF0_LEN ((320 + 96 + 96 + 96)<<SPACE_TYPE_CHAR)
#define PIXEL_YBUF0_LEN (320<<SPACE_TYPE_CHAR)
#define PIXEL_UBUF0_LEN (96<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_LEFT_EDGE_BUF0 TCSM_PIXEL_BUF0
#define TCSM_DBLK_LEFT_EDGE_YBUF0 TCSM_DBLK_LEFT_EDGE_BUF0
#define TCSM_DBLK_LEFT_EDGE_UBUF0 (TCSM_DBLK_LEFT_EDGE_YBUF0 + PIXEL_YBUF0_LEN)
#define TCSM_DBLK_LEFT_EDGE_VBUF0 (TCSM_DBLK_LEFT_EDGE_UBUF0 + PIXEL_UBUF0_LEN)
#define TCSM_RECON_BUF0 (TCSM_PIXEL_BUF0 + 4)
#define TCSM_RECON_YBUF0 TCSM_RECON_BUF0
#define TCSM_RECON_UBUF0 (TCSM_RECON_YBUF0 + PIXEL_YBUF0_LEN)
#define TCSM_RECON_VBUF0 (TCSM_RECON_UBUF0 + PIXEL_UBUF0_LEN)
#define RECON0_BUF_END (TCSM_RECON_VBUF0 + PIXEL_UBUF0_LEN)

#define TCSM_PIXEL_BUF1 (TCSM_PIXEL_BUF0 + PIXEL_BUF0_LEN)
#define PIXEL_BUF1_LEN ((320 + 96 + 96 + 96)<<SPACE_TYPE_CHAR)
#define PIXEL_YBUF1_LEN (320<<SPACE_TYPE_CHAR)
#define PIXEL_UBUF1_LEN (96<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_LEFT_EDGE_BUF1 TCSM_PIXEL_BUF1
#define TCSM_DBLK_LEFT_EDGE_YBUF1 TCSM_DBLK_LEFT_EDGE_BUF1
#define TCSM_DBLK_LEFT_EDGE_UBUF1 (TCSM_DBLK_LEFT_EDGE_YBUF1 + PIXEL_YBUF1_LEN)
#define TCSM_DBLK_LEFT_EDGE_VBUF1 (TCSM_DBLK_LEFT_EDGE_UBUF1 + PIXEL_UBUF1_LEN)
#define TCSM_RECON_BUF1 (TCSM_PIXEL_BUF1 + 4)
#define TCSM_RECON_YBUF1 TCSM_RECON_BUF1
#define TCSM_RECON_UBUF1 (TCSM_RECON_YBUF1 + PIXEL_YBUF1_LEN)
#define TCSM_RECON_VBUF1 (TCSM_RECON_UBUF1 + PIXEL_UBUF1_LEN)
#define RECON1_BUF_END (TCSM_RECON_VBUF1 + PIXEL_UBUF1_LEN)

/*De-Block Buffer: stores DBLK destination data*/
/*it is structured same as Pixel Buffer*/
// !!! lst 96B prevent dma overwrite top dat
#define TCSM_DBLK_BUF0 (TCSM_PIXEL_BUF1 + PIXEL_BUF1_LEN)
#define DBLK_BUF0_LEN ((320 + 96 + 96 + 96)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_YBUF0 (TCSM_DBLK_BUF0 + 4)
#define DBLK_YBUF0_LEN (320<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_UBUF0 (TCSM_DBLK_YBUF0 + DBLK_YBUF0_LEN)
#define DBLK_UBUF0_LEN (96<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_VBUF0 (TCSM_DBLK_UBUF0 + DBLK_UBUF0_LEN)
#define DBLK0_BUF_END (TCSM_DBLK_VBUF0 + DBLK_UBUF0_LEN)

#define TCSM_DBLK_BUF1 (TCSM_DBLK_BUF0 + DBLK_BUF0_LEN)
#define DBLK_BUF1_LEN ((320 + 96 + 96 + 96)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_YBUF1 (TCSM_DBLK_BUF1 + 4)
#define DBLK_YBUF1_LEN (320<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_UBUF1 (TCSM_DBLK_YBUF1 + DBLK_YBUF1_LEN)
#define DBLK_UBUF1_LEN (96<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_VBUF1 (TCSM_DBLK_UBUF1 + DBLK_UBUF1_LEN)
#define DBLK1_BUF_END (TCSM_DBLK_VBUF1 + DBLK_UBUF1_LEN)

/*DBLK_TOP_4LINE Buffer: stores top 4-line for next MB line's deblock use*/
#define TCSM_DBLK_TOP_4LINE_BUF (TCSM_DBLK_BUF1 + DBLK_BUF1_LEN)
#define DBLK_TOP_4LINE_BUF_LEN ((H264BS_MAX_FRAME_WIDTH*4*2+2)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_TOP_4LINE_YBUF TCSM_DBLK_TOP_4LINE_BUF
#define DBLK_TOP_4LINE_YBUF_LEN ((H264BS_MAX_FRAME_WIDTH*4)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_TOP_4LINE_UBUF (TCSM_DBLK_TOP_4LINE_YBUF + DBLK_TOP_4LINE_YBUF_LEN)
#define DBLK_TOP_4LINE_UBUF_LEN (((H264BS_MAX_FRAME_WIDTH/2)*4)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_TOP_4LINE_VBUF (TCSM_DBLK_TOP_4LINE_UBUF + DBLK_TOP_4LINE_UBUF_LEN)
#define TCSM_BUF_END (TCSM_DBLK_TOP_4LINE_BUF + DBLK_TOP_4LINE_BUF_LEN)

/* HW Descriptor-Table space */
#define TCSM_H264_MC_RND_LUT (TCSM_DBLK_BUF1 + DBLK_BUF1_LEN)
//#define TCSM_H264_MC_RND_LUT TCSM_BANK3  /*16word*/
#define TCSM_H264_LUMA_BLK_LUT (TCSM_H264_MC_RND_LUT + (16<<2) )   /*7word*/
#define TCSM_H264_CHROM_BLK_LUT (TCSM_H264_LUMA_BLK_LUT + (7<<2) ) /*7word*/
#define TCSM_H264_MC_DES_TAB (TCSM_H264_CHROM_BLK_LUT + (7<<2) )   /*624word*/ 
//#define TCSM_H264_DBLK_NODE0 (TCSM_H264_MC_DES_TAB + (624<<2) )     /*63word*/ 
#define TCSM_H264_DBLK_NODE0 (TCSM_H264_MC_DES_TAB + (784<<2) )     /*63word*/ 
#define TCSM_H264_DBLK_NODE1 (TCSM_H264_DBLK_NODE0 + (63<<2) )     /*63word*/ 
#define TCSM_IDCT_DESC_TABLE (TCSM_H264_DBLK_NODE1 + (63<<2) )   /*120word*/
#define TCSM_DES_TAB_END (TCSM_IDCT_DESC_TABLE + (120<<2) ) 

#define __tcsm_text__ __attribute__ ((__section__ (".tcsm_text")))

static int intra_pred4x4_top[2][16] = {
 {TCSM_RECON_YBUF0, TCSM_RECON_YBUF0, 
  TCSM_RECON_YBUF0 + 3*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF0 + 3*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0, TCSM_RECON_YBUF0,
  TCSM_RECON_YBUF0 + 3*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 3*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF0 + 7*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF0 + 7*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0 + 11*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF0 + 11*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0 + 7*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 7*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF0 + 11*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 11*MB_LUMA_EDGED_WIDTH + 12
  },
 {TCSM_RECON_YBUF1, TCSM_RECON_YBUF1, 
  TCSM_RECON_YBUF1 + 3*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF1 + 3*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1, TCSM_RECON_YBUF1,
  TCSM_RECON_YBUF1 + 3*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 3*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF1 + 7*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF1 + 7*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1 + 11*MB_LUMA_EDGED_WIDTH, TCSM_RECON_YBUF1 + 11*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1 + 7*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 7*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF1 + 11*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 11*MB_LUMA_EDGED_WIDTH + 12
  }
};

static int intra_pred4x4_left[2][16] = {
 {TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 4, 
  TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 4*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0 + 8, TCSM_RECON_YBUF0 + 12,
  TCSM_RECON_YBUF0 + 4*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 4*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 8*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0, TCSM_RECON_YBUF0 + 12*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF0 + 8*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 8*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF0 + 12*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF0 + 12*MB_LUMA_EDGED_WIDTH + 12
  },
 {TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 4, 
  TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 4*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1 + 8, TCSM_RECON_YBUF1 + 12,
  TCSM_RECON_YBUF1 + 4*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 4*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 8*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1, TCSM_RECON_YBUF1 + 12*MB_LUMA_EDGED_WIDTH + 4,
  TCSM_RECON_YBUF1 + 8*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 8*MB_LUMA_EDGED_WIDTH + 12,
  TCSM_RECON_YBUF1 + 12*MB_LUMA_EDGED_WIDTH + 8, TCSM_RECON_YBUF1 + 12*MB_LUMA_EDGED_WIDTH + 12
  }
};

static int *intra_pred4x4_top_ptr[2];
static int *intra_pred4x4_left_ptr[2];

#define push2tcsm(func)   					\
({                                                              \
  int tcsm_i;                                                   \
  uint32_t * func##_addr = (uint32_t)func;                      \
  uint32_t * func##_tcsm = TCSM_##func;                         \
  for(tcsm_i=0; tcsm_i<=(func##_LEN); tcsm_i++)                 \
    func##_tcsm[tcsm_i] = func##_addr[tcsm_i];                  \
  func##_ptr = TCSM_##func;                                     \
})

#define i_lw(src0,src1)  								\
({unsigned long _dst_;								\
	__asm__ __volatile__("lw\t %0,%2(%1)\t#i_lw":"=r"(_dst_):"r"(src0),"X"(src1)); 	\
	_dst_;})
#define i_sw(src0,src1,offset)	\
({ __asm__ __volatile__("sw\t %0,%2(%1)\t#i_sw"::"r"(src0),"r"(src1),"X"(offset):"memory");})

#endif /*__JZC_TCSM_H__*/
