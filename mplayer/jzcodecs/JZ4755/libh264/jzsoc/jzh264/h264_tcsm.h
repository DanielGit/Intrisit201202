
#ifndef __H264_TCSM_H__
#define __h264_TCSM_H__
#include "../jzsys.h"
#include "../jzasm.h"

/* TCSM space defined here*/
#define TCSM_BASE 0xF4000000
#define TCSM_BANK0 0xF4000000
#define TCSM_BANK1 0xF4001000
#define TCSM_BANK2 0xF4002000
#define TCSM_BANK3 0xF4003000

#define TCSM1_BASE 0xF4008000
#define TCSM1_VBASE 0x93808000
#define TCSM1_BANK0 0xF4008000
#define TCSM1_BANK1 0xF4009000
#define TCSM1_BANK2 0xF400A000
#define TCSM1_BANK3 0xF400B000
#define TCSM1_BANK4 0xF400C000
#define TCSM1_BANK5 0xF400D000
#define TCSM1_BANK6 0xF400E000
#define TCSM1_BANK7 0xF400F000

#define TCSM1_VADDR(addr)          ((addr & 0x0000FFFF) | 0x93800000)
#define TCSM1_VADDR_UNCACHE(addr)  ((addr & 0x0000FFFF) | 0xB3800000)

#define SPACE_TYPE_CHAR 0x0
#define SPACE_TYPE_SHORT 0x1
#define SPACE_TYPE_WORD 0x2
#define MB_LUMA_WIDTH 16
#define MB_CHROM_WIDTH 8
#define MB_LUMA_EDGED_WIDTH (MB_LUMA_WIDTH + 4)
#define MB_CHROM_EDGED_WIDTH (MB_CHROM_WIDTH + 4)
#define H264MP_MAX_FRAME_WIDTH 1280
#define DEFAULT_H264_MAX_MBNUM 2304//1024x576

#define JZC_CACHE_LINE 32
#define	ALIGN(p,n)     ((uint8_t*)(p)+((((uint8_t*)(p)-(uint8_t*)(0)) & ((n)-1)) ? ((n)-(((uint8_t*)(p)-(uint8_t*)(0)) & ((n)-1))):0))
#define TCSM1_ALN4(x)  ((x & 0x3)? (x + 4 - (x & 0x3)) : x)


#define PMON_BUF_LEN (4)
#define PMON_MC_BUF (TCSM1_BANK7)
#define PMON_IDCT_BUF (PMON_MC_BUF + PMON_BUF_LEN)
#define PMON_DBLK_BUF (PMON_IDCT_BUF + PMON_BUF_LEN)
#define PMON_INTRA_BUF (PMON_DBLK_BUF + PMON_BUF_LEN)
#define PMON_MDMA_BUF (PMON_INTRA_BUF + PMON_BUF_LEN)
#define PMON_SYNC_BUF (PMON_MDMA_BUF + PMON_BUF_LEN)

#define TCSM1_INSN_LEN 0x1000 /*suppose up-to 4bank fill instructions*/
#define TCSM1_TASK_LEN 0x1400 /*suppose up-to 5bank instructions and data*/

/*TCSM1 space for main-aux sync/handshake*/
#define TCSM1_ALN4(x)                ((x & 0x3)? (x + 4 - (x & 0x3)) : x)
#define AUX_COMD_BUF_LEN 32
#define TCSM1_FIFO_WP (TCSM1_BANK5 - 0x700)
#define TCSM1_FOFO_WP_VADDR TCSM1_VADDR_UNCACHE(TCSM1_FIFO_WP)
#define TCSM1_TASK_DONE (TCSM1_FIFO_WP+4)
#define TCSM1_TASK_DONE_VADDR TCSM1_VADDR_UNCACHE(TCSM1_TASK_DONE)
#define TCSM1_AUX_SIKP_FRAME (TCSM1_TASK_DONE+4)

/*TCSM1 space for main cpu --> aux parameters transfer: dMB and dFRM*/
#define TCSM1_FRM_ARGS (TCSM1_FIFO_WP+AUX_COMD_BUF_LEN)
#define TCSM1_FRM_ARGS_VADDR TCSM1_VADDR_UNCACHE(TCSM1_FRM_ARGS)
#define FRM_ARGS_LEN (TCSM1_ALN4(sizeof(H264_Frame_GlbARGs)))
#define TCSM1_MBARGS_BUF (TCSM1_FRM_ARGS+FRM_ARGS_LEN)
#define MBARGS_BUF_LEN (TCSM1_ALN4(sizeof(H264_MB_DecARGs)))
#define TCSM1_MBARGS_BUF2 (TCSM1_MBARGS_BUF + MBARGS_BUF_LEN)

/*AUX global struct*/
#define TCSM1_AUX_T (TCSM1_MBARGS_BUF2+MBARGS_BUF_LEN)
#define AUX_T_LEN (TCSM1_ALN4(sizeof(H264_AUX_T)))

//GP1 chain(ARG used to comm dMB args, TRAN used for AUX MB transfer)
#define TCSM1_GP1_TRAN_CHAIN (TCSM1_AUX_T+AUX_T_LEN)
#define GP1_TRAN_CHAIN_LEN ((4*9)<<SPACE_TYPE_WORD)
#define TCSM1_GP1_ARG_CHAIN (TCSM1_GP1_TRAN_CHAIN+GP1_TRAN_CHAIN_LEN)
#define GP1_ARG_CHAIN_LEN ((4)<<SPACE_TYPE_WORD)

//DBLK chain
#define TCSM1_DBLK_DES_BUF0 (TCSM1_GP1_ARG_CHAIN+GP1_ARG_CHAIN_LEN)
#define DBLK_DES_BUF_LEN (52<<SPACE_TYPE_WORD)
#define TCSM1_DBLK_DES_BUF1 (TCSM1_DBLK_DES_BUF0 + DBLK_DES_BUF_LEN )
//MC chain
#define TCSM1_MC_DES_BUF0 (TCSM1_DBLK_DES_BUF1+DBLK_DES_BUF_LEN)
#define MC_DES_BUF_LEN ((25*16)<<SPACE_TYPE_WORD)
#define TCSM1_MC_DES_BUF1 (TCSM1_MC_DES_BUF0+MC_DES_BUF_LEN)

/* AUX buffer allocate*/

//RECON/dblk/mc_avg buffer
#define DBLK_LEFT_WIDTH 4
#define DBLK_YBUF_LEN (MB_LUMA_EDGED_WIDTH<<SPACE_TYPE_CHAR)
#define DBLK_UVBUF_LEN (MB_CHROM_EDGED_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_YBUF_LEN (MB_LUMA_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_UVBUF_LEN (MB_CHROM_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_BUF_STRIDE (80<<SPACE_TYPE_CHAR)
#define RECON_U_OFST (2*RECON_YBUF_LEN + DBLK_UVBUF_LEN)
#define RECON_V_OFST (RECON_U_OFST + 8*RECON_BUF_STRIDE)

#define TCSM1_RECON_PIXEL_BUF0 (TCSM1_MC_DES_BUF1+MC_DES_BUF_LEN)
#define TCSM1_DBLK_YBUF0 (TCSM1_RECON_PIXEL_BUF0 + DBLK_LEFT_WIDTH)
#define TCSM1_DBLK_UBUF0 (TCSM1_DBLK_YBUF0 + 3*RECON_YBUF_LEN + DBLK_LEFT_WIDTH)
#define TCSM1_DBLK_VBUF0 (TCSM1_DBLK_UBUF0 + 8*RECON_BUF_STRIDE)
#define TCSM1_RECON_YBUF0 (TCSM1_RECON_PIXEL_BUF0 + DBLK_YBUF_LEN)
#define TCSM1_RECON_UBUF0 (TCSM1_RECON_YBUF0 + RECON_U_OFST)
#define TCSM1_RECON_VBUF0 (TCSM1_RECON_YBUF0 + RECON_V_OFST)
#define TCSM1_MCAVG_YBUF0 (TCSM1_RECON_YBUF0 + RECON_YBUF_LEN)
#define TCSM1_MCAVG_UBUF0 (TCSM1_RECON_UBUF0 + RECON_UVBUF_LEN)
#define TCSM1_MCAVG_VBUF0 (TCSM1_RECON_VBUF0 + RECON_UVBUF_LEN)

#define TCSM1_RECON_PIXEL_BUF1 (TCSM1_RECON_PIXEL_BUF0 + 16*RECON_BUF_STRIDE)
#define TCSM1_DBLK_YBUF1 (TCSM1_RECON_PIXEL_BUF1 + DBLK_LEFT_WIDTH)
#define TCSM1_DBLK_UBUF1 (TCSM1_DBLK_YBUF1 + 3*RECON_YBUF_LEN + DBLK_LEFT_WIDTH)
#define TCSM1_DBLK_VBUF1 (TCSM1_DBLK_UBUF1 + 8*RECON_BUF_STRIDE)
#define TCSM1_RECON_YBUF1 (TCSM1_RECON_PIXEL_BUF1 + DBLK_YBUF_LEN)
#define TCSM1_RECON_UBUF1 (TCSM1_RECON_YBUF1 + RECON_U_OFST)
#define TCSM1_RECON_VBUF1 (TCSM1_RECON_YBUF1 + RECON_V_OFST)
#define TCSM1_MCAVG_YBUF1 (TCSM1_RECON_YBUF1 + RECON_YBUF_LEN)
#define TCSM1_MCAVG_UBUF1 (TCSM1_RECON_UBUF1 + RECON_UVBUF_LEN)
#define TCSM1_MCAVG_VBUF1 (TCSM1_RECON_VBUF1 + RECON_UVBUF_LEN)

/* end node buffer for hw */
#define NODE_CMD0   0x80000000
#define TCSM1_H264_DBLK_INFAR (TCSM1_RECON_PIXEL_BUF1 + 16*RECON_BUF_STRIDE)
#define TCSM1_H264_DBLK_INFDA 0x1234abcd

/*Intra Back_up Bottom Buffer:*/
#define TCSM1_BOTTOM_Y_25PIX (TCSM1_H264_DBLK_INFAR+4)
#define TCSM1_BOTTOM_Y_25PIX_LEN (25+3)
#define TCSM1_BOTTOM_U_13PIX (TCSM1_BOTTOM_Y_25PIX + TCSM1_BOTTOM_Y_25PIX_LEN)
#define TCSM1_BOTTOM_UV_13PIX_LEN (13+3)
#define TCSM1_BOTTOM_V_13PIX (TCSM1_BOTTOM_U_13PIX + TCSM1_BOTTOM_UV_13PIX_LEN)

// MC EDGE EMULATE BUFFER
#define EMU_EDGE_LUMA_STRIDE 24
#define EMU_EDGE_CHROM_STRIDE 12
#define H264_TCSM1_EDGE_EMU_BUF (TCSM1_BOTTOM_V_13PIX + TCSM1_BOTTOM_UV_13PIX_LEN)
#define H264_EDGE_EMU_BUF_LEN ((EMU_EDGE_LUMA_STRIDE*21)<<SPACE_TYPE_CHAR)

// AUX DEBUG
/*
#define TCSM1_DBG_bgn  (H264_TCSM1_EDGE_EMU_BUF + H264_EDGE_EMU_BUF_LEN)
#define TCSM1_DBG_end  (TCSM1_DBG_bgn + 4)
#define TCSM1_DBG_num  (TCSM1_DBG_end + 4)
#define TCSM1_DBG_info (TCSM1_DBG_num + 4)
*/

/*buffer allocate*/
/*BLOCK(IDCT src) Buffer: DeQuant result, IDCT data source*/
/* +------+------+--...--+------+------+--...--+------+------+--...--+------+
   |  y0  |  y1  |  ...  | y15  |  u0  |  ...  |  u3  |  v0  |  ...  |  v3  |
   |(4x4) |      |       |      |      |       |      |      |       |      |
   |      |      |       |      |      |       |      |      |       |      |
   +------+------+--...--+------+------+--...--+------+------+--...--+------+
   "one-line" data structure:
   y0 start addr: TCSM_IDCT_SRC_BUF;
   y1 start addr: TCSM_IDCT_SRC_BUF + (4<<SPACE_TYPE_SHORT)
   u0 start addr: TCSM_IDCT_SRC_BUF + ( (16*4)<<SPACE_TYPE_SHORT)
   ...
   the src buffer stride is ((16 + 4 + 4)*4)<<SPACE_TYPE_SHORT
*/
#define TCSM_BLOCK_BUF TCSM_BANK0
#define BLOCK_BUF_STRIDE (((16 + 4 + 4)*16)<<SPACE_TYPE_SHORT)

/*IDCT DST Buffer: stores IDCT result*/
/* +------+------+--...--+------+------+--...--+------+------+--...--+------+
   |  y0  |  y1  |  ...  | y15  |  u0  |  ...  |  u3  |  v0  |  ...  |  v3  |
   |(4x4) |      |       |      |      |       |      |      |       |      |
   |      |      |       |      |      |       |      |      |       |      |
   +------+------+--...--+------+------+--...--+------+------+--...--+------+
   "one-line" data structure:
   y0 start addr: TCSM_IDCT_DST_BUF;
   y1 start addr: TCSM_IDCT_DST_BUF + (4<<SPACE_TYPE_SHORT)
   u0 start addr: TCSM_IDCT_DST_BUF + ( (16*4)<<SPACE_TYPE_SHORT)
   ...
   the dst buffer stride is ((16 + 4 + 4)*4)<<SPACE_TYPE_SHORT
 */
#define TCSM_IDCT_BUF0 (TCSM_BLOCK_BUF + BLOCK_BUF_STRIDE) // 192
#define IDCT_BUF_STRIDE (((16 + 4 + 4)*16)<<SPACE_TYPE_SHORT)
#define TCSM_IDCT_BUF1 (TCSM_IDCT_BUF0 + IDCT_BUF_STRIDE) // 384

/*Reconstruction/Pixel Buffer:*/
/* Used for MC/Intra output, DBLK input/output. size: 16*56=896 Bytes */
/* 
   -- Buffers are ping-pong buffered
   -- left 4pixel buffer for deblock use 
     +-----+-------------+-------------+-------------+-----+----------+----------+----------+---------
     |     |             |             |             |     |          |          |          |       ^
     | (1) | (2)         |  (3)        |  (4)        | (5) | (6)      | (7)      | (8)      |       |
     |     |             |             |             |     |          |          |          |       |
     |     |  DBLK       |  RECON      |  MC_AVG     |     | DBLK     | RECON    | MC_AVG   |       |
     |     |   Y         |   Y         |   Y         |     |  U       |  U       |  U       |       |
     |     |             |             |             |     |          |          |          |
     |     |             |             |             |     |          |          |          |       16
     |     |             |             |             +-----+----------+----------+----------+      Byte
     |     |             |             |             |     |          |          |          |
     |     |             |             |             | (9) | (10)     | (11)     | (12)     |       |
     |     |             |             |             |     |          |          |          |       |
     |     |             |             |             |     | DBLK     | RECON    |  MC_AVG  |       |
     |     |             |             |             |     |  V       |  V       |   V      |       |
     |     |             |             |             |     |          |          |          |       |
     |<4B> |<- 16Byte -> |<- 16Byte -> |<- 16Byte -> |<4B> |<-8Byte-> |<-8Byte-> |<-8Byte-> |       ^
     +-----+-------------+-------------+-------------+-----+----------+----------+----------+---------
     |<----------------------------------     80 byte    ---------------------------------->|
  Note:
    (1) Luma left buffer, size: 4x16byte, used as deblock output left 4pixel
    (2) Luma deblock output buffer, size 16x16byte, output only
    (3) Luma recon buffer, size: 16x16byte, MC/intra-pred/add_error destination, dblk input
    (4) Luma backward buffer, size: 16x16byte, only for MC-bidir case
    (5) U left buffer, size: 4x8byte, used as deblock output left 4pixel
    (6) U deblock output buffer, size: 8x8byte, deblock output only
    (7) U recon buffer, size: 8x8byte, MC/intra-pred/add_error destination, dblk input
    (8) U backward buffer, size: 8x8byte, only for MC-bidir case
    (9) V left buffer, size: 4x8byte, used as deblock output left 4pixel
    (10) V deblock output buffer, size: 8x8byte, deblock output only
    (11) V recon buffer, size: 8x8byte, MC/intra-pred/add_error destination, dblk input
    (12) V backward buffer, size: 8x8byte, only for MC-bidir case
*/
#define DBLK_LEFT_WIDTH 4
#define DBLK_YBUF_LEN (MB_LUMA_EDGED_WIDTH<<SPACE_TYPE_CHAR)
#define DBLK_UVBUF_LEN (MB_CHROM_EDGED_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_YBUF_LEN (MB_LUMA_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_UVBUF_LEN (MB_CHROM_WIDTH<<SPACE_TYPE_CHAR)
#define RECON_BUF_STRIDE (80<<SPACE_TYPE_CHAR)
#define RECON_U_OFST (2*RECON_YBUF_LEN + DBLK_UVBUF_LEN)
#define RECON_V_OFST (RECON_U_OFST + 8*RECON_BUF_STRIDE)
#define DBLK_U_OFST (3*RECON_YBUF_LEN + DBLK_LEFT_WIDTH)
#define DBLK_V_OFST (DBLK_U_OFST + 8*RECON_BUF_STRIDE)

#define TCSM_RECON_PIXEL_BUF0 (TCSM_IDCT_BUF1 + IDCT_BUF_STRIDE) // 1152

#define TCSM_DBLK_YBUF0 (TCSM_RECON_PIXEL_BUF0 + DBLK_LEFT_WIDTH)
#define TCSM_DBLK_UBUF0 (TCSM_DBLK_YBUF0 + 3*RECON_YBUF_LEN + DBLK_LEFT_WIDTH)
#define TCSM_DBLK_VBUF0 (TCSM_DBLK_UBUF0 + 8*RECON_BUF_STRIDE)

#define TCSM_RECON_YBUF0 (TCSM_RECON_PIXEL_BUF0 + DBLK_YBUF_LEN)
#define TCSM_RECON_UBUF0 (TCSM_RECON_YBUF0 + RECON_U_OFST)
#define TCSM_RECON_VBUF0 (TCSM_RECON_YBUF0 + RECON_V_OFST)

#define TCSM_MCAVG_YBUF0 (TCSM_RECON_YBUF0 + RECON_YBUF_LEN)
#define TCSM_MCAVG_UBUF0 (TCSM_RECON_UBUF0 + RECON_UVBUF_LEN)
#define TCSM_MCAVG_VBUF0 (TCSM_RECON_VBUF0 + RECON_UVBUF_LEN)

#define TCSM_RECON_PIXEL_BUF1 (TCSM_RECON_PIXEL_BUF0 + 16*RECON_BUF_STRIDE) // 2432

#define TCSM_DBLK_YBUF1 (TCSM_RECON_PIXEL_BUF1 + DBLK_LEFT_WIDTH)
#define TCSM_DBLK_UBUF1 (TCSM_DBLK_YBUF1 + 3*RECON_YBUF_LEN + DBLK_LEFT_WIDTH)
#define TCSM_DBLK_VBUF1 (TCSM_DBLK_UBUF1 + 8*RECON_BUF_STRIDE)

#define TCSM_RECON_YBUF1 (TCSM_RECON_PIXEL_BUF1 + DBLK_YBUF_LEN)
#define TCSM_RECON_UBUF1 (TCSM_RECON_YBUF1 + RECON_U_OFST)
#define TCSM_RECON_VBUF1 (TCSM_RECON_YBUF1 + RECON_V_OFST)

#define TCSM_MCAVG_YBUF1 (TCSM_RECON_YBUF1 + RECON_YBUF_LEN)
#define TCSM_MCAVG_UBUF1 (TCSM_RECON_UBUF1 + RECON_UVBUF_LEN)
#define TCSM_MCAVG_VBUF1 (TCSM_RECON_VBUF1 + RECON_UVBUF_LEN)

/*DBLK_TOP_4LINE Buffer: stores top 4-line for next MB line's deblock use*/
//#define TCSM_DBLK_TOP_4LINE_YBUF (TCSM_RECON_PIXEL_BUF1 + 16*RECON_BUF_STRIDE) // 4864
#define TCSM_DBLK_TOP_4LINE_YBUF0 (H264_TCSM1_EDGE_EMU_BUF + H264_EDGE_EMU_BUF_LEN)
#define DBLK_TOP_4LINE_YBUF_LEN ((RECON_YBUF_LEN*4)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_TOP_4LINE_UBUF0 (TCSM_DBLK_TOP_4LINE_YBUF0 + DBLK_TOP_4LINE_YBUF_LEN)
#define DBLK_TOP_4LINE_UVBUF_LEN ((RECON_UVBUF_LEN*4)<<SPACE_TYPE_CHAR)
#define TCSM_DBLK_TOP_4LINE_VBUF0 (TCSM_DBLK_TOP_4LINE_UBUF0 + DBLK_TOP_4LINE_UVBUF_LEN)

#define TCSM_DBLK_TOP_4LINE_YBUF1 (TCSM_DBLK_TOP_4LINE_VBUF0 + DBLK_TOP_4LINE_UVBUF_LEN)
#define TCSM_DBLK_TOP_4LINE_UBUF1 (TCSM_DBLK_TOP_4LINE_YBUF1 + DBLK_TOP_4LINE_YBUF_LEN)
#define TCSM_DBLK_TOP_4LINE_VBUF1 (TCSM_DBLK_TOP_4LINE_UBUF1 + DBLK_TOP_4LINE_UVBUF_LEN)

/* HW Descriptor-Table space */
#define TCSM_IDCT_DES_BUF0 TCSM_BANK2
#define IDCT_DES_BUF_LEN ((24*4)<<SPACE_TYPE_WORD)
#define TCSM_IDCT_DES_BUF1 (TCSM_IDCT_DES_BUF0 + IDCT_DES_BUF_LEN)

#define TCSM_DBLK_DES_BUF0 (TCSM_IDCT_DES_BUF1 + IDCT_DES_BUF_LEN)
//#define DBLK_DES_BUF_LEN (52<<SPACE_TYPE_WORD)
#define TCSM_DBLK_DES_BUF1 (TCSM_DBLK_DES_BUF0 + DBLK_DES_BUF_LEN)

#define NODE_CMD0   0x80000000
#define TCSM_H264_DBLK_INFAR (TCSM_DBLK_DES_BUF1 + DBLK_DES_BUF_LEN)
#define TCSM_H264_DBLK_INFDA 0x1234abcd

#define TCSM_MC_DES_BUF (TCSM_H264_DBLK_INFAR + 32)
//#define MC_DES_BUF_LEN (800<<SPACE_TYPE_WORD)


/******************************************************************
 * TCSM0 bank management.
 *-----  V_No.1 ------------
 * Since J1 and HW can't accecc the same bank at a time,
 * Bank3 only accessed by HW,(here is GP1)
 * Bank2 write by J1, read by HW,(here is GP0)
 * Bank0/Bank1 only used by J1(store insns).
 *-----  V_No.2 ------------
 * Since J1 and HW can't accecc the same bank at a time,
 * Bank3 only accessed by HW(store back_up_bottom, access by GP1)
 * Bank0 only used by HW(store dMB_tcsm0, access by GP0)
 * Bank1 and Bank2 are parser dependent:
 *  First, Bank1 store cavlc instructions, Bank2 store cabac instructions.
 *  Then, if it's CAVLC, Bank2 store dMB_tcsm1, else, Bank1 store dMB_tcsm1.
 *******************************************************************/

/*Back_up Bottom Buffer:*/
/* Used for Intra_Prediction. size H264MP_MAX_FRAME_WIDTH*2 */
//#define TCSM_BACKUPBOTTOM_YBUF0 TCSM_BANK3
#define TCSM_BACKUPBOTTOM_YBUF0 (H264_TCSM1_EDGE_EMU_BUF + H264_EDGE_EMU_BUF_LEN)
#define BACKUPBOTTOM_YBUF_LEN (H264MP_MAX_FRAME_WIDTH<<SPACE_TYPE_CHAR)
#define TCSM_BACKUPBOTTOM_YBUF1 (TCSM_BACKUPBOTTOM_YBUF0 + 0)
#define TCSM_BACKUPBOTTOM_UBUF0 (TCSM_BACKUPBOTTOM_YBUF1 + BACKUPBOTTOM_YBUF_LEN)
#define BACKUPBOTTOM_UVBUF_LEN ((H264MP_MAX_FRAME_WIDTH>>1)<<SPACE_TYPE_CHAR)
#define TCSM_BACKUPBOTTOM_UBUF1 (TCSM_BACKUPBOTTOM_UBUF0 + 0)

#define TCSM_BACKUPBOTTOM_VBUF0 (TCSM_BACKUPBOTTOM_UBUF1 + BACKUPBOTTOM_UVBUF_LEN)
#define TCSM_BACKUPBOTTOM_VBUF1 (TCSM_BACKUPBOTTOM_VBUF0 + 0)

/* UV double DMA-OUT buffer */
#define U_DOUBLE_OUT_BUF0 (TCSM_BACKUPBOTTOM_VBUF1 + BACKUPBOTTOM_UVBUF_LEN)
#define UV_DOUBLE_OUT_STRD (16 << SPACE_TYPE_CHAR)
#define UV_DOUBLE_OUT_LEN (UV_DOUBLE_OUT_STRD * 8)
#define U_DOUBLE_OUT_BUF1 (U_DOUBLE_OUT_BUF0 + UV_DOUBLE_OUT_LEN)
#define V_DOUBLE_OUT_BUF0 (U_DOUBLE_OUT_BUF1 + UV_DOUBLE_OUT_LEN)
#define V_DOUBLE_OUT_BUF1 (V_DOUBLE_OUT_BUF0 + UV_DOUBLE_OUT_LEN)

/*dMB Buffer:*/
/* Store One dMB, then move out sdram by GP0 */
#define TCSM_DMB_BUF (TCSM_BANK0)
#define DMB_BUF_LEN MBARGS_BUF_LEN
#define TCSM_DMB_BUF2 (TCSM_DMB_BUF + 0/*DMB_BUF_LEN*/)

#define dMB_WEIGHT_ARG_LEN (53 << 2)
#define dMB_CURR_ARG_LEN (79 << 2)
#define dMB_RESIDUAL_ARG_LEN (DMB_BUF_LEN - dMB_CURR_ARG_LEN - dMB_WEIGHT_ARG_LEN)

//GP0 chain
#define TCSM_MDMA0_DES_BUF (TCSM_DMB_BUF2 + DMB_BUF_LEN)
#define MDMA0_DES_BUF_LEN ((4*6)<<SPACE_TYPE_WORD)

/*TCSM0 Functions' Buffer:*/
#define TCSM_CAVLC_FUNC_BUF (TCSM_BANK0)
#define CAVLC_FUNC_BUF_LEN (0x1000) // up to 1 banks for Main CAVLC instructions

#define TCSM_CABAC_FUNC_BUF (TCSM_BANK2)
#define CABAC_FUNC_BUF_LEN (0x1000) // up to 1 banks for Main CABAC instructions

#define TCSM_BANK_LEN 0x1000

#define TCSM0_TASK_DONE (TCSM_CAVLC_FUNC_BUF + 0x1FF0)

#endif /*__H264_TCSM_H__*/
