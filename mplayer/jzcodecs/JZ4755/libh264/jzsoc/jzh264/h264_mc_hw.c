/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264)
 *
* $Id: h264_mc_hw.c,v 1.3 2009/05/12 23:55:28 pwang Exp $
 *
 ****************************************************************************/

#include "../jz4750e_mc.h"
#include "h264_mc_hw.h"
#include "h264_tcsm.h"
#include "mpegvideo.h"
#include "../jzmedia.h"
#include "dsputil.h"
extern unsigned char * frame_buffer;
extern unsigned int phy_fb;

#ifdef JZC_CRC_VER
extern short mpFrame;
#endif

#undef printf
# define H264_6TAP_RND_HI (16<<8)
# define H264_6TAP_RND_LO (16)
# define H264_6TAP_RND_ZO 0  

# define MC_BLK_16X16 0
# define MC_BLK_16X8 1
# define MC_BLK_8X16 2
# define MC_BLK_8X8 3
# define MC_BLK_8X4 4
# define MC_BLK_4X8 5
# define MC_BLK_4X4 6

/*H264 luma interpolate infomation LUT*/
const uint32_t H264LumaIntp[16] = {
  (H0V0<<16) | H264_6TAP_RND_ZO, /*0,0*/
  (H1V0<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H2V0<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H3V0<<16) | H264_6TAP_RND_LO, /*16,0*/

  (H0V1<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H1V1<<16) | H264_6TAP_RND_LO | H264_6TAP_RND_HI, /*16,16*/
  (H2V1<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H3V1<<16) | H264_6TAP_RND_LO | H264_6TAP_RND_HI, /*16,16*/
    
  (H0V2<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H1V2<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H2V2<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H3V2<<16) | H264_6TAP_RND_LO, /*16,0*/

  (H0V3<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H1V3<<16) | H264_6TAP_RND_LO | H264_6TAP_RND_HI, /*16,16*/
  (H2V3<<16) | H264_6TAP_RND_LO, /*16,0*/
  (H3V3<<16) | H264_6TAP_RND_LO | H264_6TAP_RND_HI /*16,16*/
};

/*H264 luma block infomation LUT*/
const uint32_t H264LumaBLK[7] = {
  MC_BLK_H16 | (MC_BLK_W16<<2) | (H264_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W16<<2) | (H264_QPEL<<8),
  MC_BLK_H16 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (H264_QPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (H264_QPEL<<8)
};

/*H264 chroma block infomation LUT*/
const uint32_t  H264ChromaBLK[7] = {
  MC_BLK_H8 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W2<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W2<<2) | (H264_EPEL<<8)
}; 

char *MB_BLOCK_OFFSET_TAB[9] =
{
TCSM_RECON_YBUF0,                     TCSM_RECON_UBUF0,                     TCSM_RECON_VBUF0,
TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE,TCSM_RECON_UBUF0 + 4*RECON_BUF_STRIDE,TCSM_RECON_VBUF0 + 4*RECON_BUF_STRIDE,
TCSM_RECON_YBUF0 + 8                 ,TCSM_RECON_UBUF0 + 4                 ,TCSM_RECON_VBUF0 + 4
};
char *MB_SUBBLOCK_OFFSET_TAB[12] =
{
//8*8
TCSM_RECON_YBUF0,                     TCSM_RECON_UBUF0,                     TCSM_RECON_VBUF0,
TCSM_RECON_YBUF0 + 8                 ,TCSM_RECON_UBUF0 + 4                 ,TCSM_RECON_VBUF0 + 4,
TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE,TCSM_RECON_UBUF0 + 4*RECON_BUF_STRIDE,TCSM_RECON_VBUF0 + 4*RECON_BUF_STRIDE,
TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 8,TCSM_RECON_UBUF0 + 4*RECON_BUF_STRIDE + 4,TCSM_RECON_VBUF0 + 4*RECON_BUF_STRIDE+4
};

char *MB_SUBBLOCK_8X4_OFFSET_TAB[12] =
{
//8*4
TCSM_RECON_YBUF0 + 4*RECON_BUF_STRIDE,TCSM_RECON_UBUF0 + 2*RECON_BUF_STRIDE,TCSM_RECON_VBUF0 + 2*RECON_BUF_STRIDE,
TCSM_RECON_YBUF0 + 4*RECON_BUF_STRIDE + 8,TCSM_RECON_UBUF0 + 2*RECON_BUF_STRIDE + 4,TCSM_RECON_VBUF0 + 2*RECON_BUF_STRIDE+4,
TCSM_RECON_YBUF0 +12*RECON_BUF_STRIDE,TCSM_RECON_UBUF0 + 6*RECON_BUF_STRIDE,TCSM_RECON_VBUF0 + 6*RECON_BUF_STRIDE,
TCSM_RECON_YBUF0 +12*RECON_BUF_STRIDE + 8,TCSM_RECON_UBUF0 + 6*RECON_BUF_STRIDE + 4,TCSM_RECON_VBUF0 + 6*RECON_BUF_STRIDE+4
};

char *MB_SUBBLOCK_4X8_OFFSET_TAB[12] =
{
//4*8
TCSM_RECON_YBUF0 + 4,                 TCSM_RECON_UBUF0 + 2,                 TCSM_RECON_VBUF0 + 2,
TCSM_RECON_YBUF0 + 12                 ,TCSM_RECON_UBUF0 + 6                 ,TCSM_RECON_VBUF0 + 6,
TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE + 4,TCSM_RECON_UBUF0 + 4*RECON_BUF_STRIDE + 2,TCSM_RECON_VBUF0 + 4*RECON_BUF_STRIDE+2,
TCSM_RECON_YBUF0 + 8*RECON_BUF_STRIDE +12,TCSM_RECON_UBUF0 + 4*RECON_BUF_STRIDE + 6,TCSM_RECON_VBUF0 + 4*RECON_BUF_STRIDE+6
};

char *MB_SUBBLOCK_4X4_OFFSET_TAB[4][12] =
     {
       {TCSM_RECON_YBUF0,TCSM_RECON_UBUF0,TCSM_RECON_VBUF0,
        TCSM_RECON_YBUF0+4,TCSM_RECON_UBUF0+2,TCSM_RECON_VBUF0+2,
        TCSM_RECON_YBUF0+4*RECON_BUF_STRIDE,TCSM_RECON_UBUF0+2*RECON_BUF_STRIDE,TCSM_RECON_VBUF0+2*RECON_BUF_STRIDE,
        TCSM_RECON_YBUF0+4*RECON_BUF_STRIDE+4,TCSM_RECON_UBUF0+2*RECON_BUF_STRIDE+2,TCSM_RECON_VBUF0+2*RECON_BUF_STRIDE+2
       },
       {TCSM_RECON_YBUF0+8,TCSM_RECON_UBUF0+4,TCSM_RECON_VBUF0+4,
        TCSM_RECON_YBUF0+12,TCSM_RECON_UBUF0+6,TCSM_RECON_VBUF0+6,
        TCSM_RECON_YBUF0+4*RECON_BUF_STRIDE+8,TCSM_RECON_UBUF0+2*RECON_BUF_STRIDE+4,TCSM_RECON_VBUF0+2*RECON_BUF_STRIDE+4,
        TCSM_RECON_YBUF0+4*RECON_BUF_STRIDE+12,TCSM_RECON_UBUF0+2*RECON_BUF_STRIDE+6,TCSM_RECON_VBUF0+2*RECON_BUF_STRIDE+6
       },
       {
        TCSM_RECON_YBUF0+8*RECON_BUF_STRIDE  ,TCSM_RECON_UBUF0+4*RECON_BUF_STRIDE  ,TCSM_RECON_VBUF0+4*RECON_BUF_STRIDE,
        TCSM_RECON_YBUF0+8*RECON_BUF_STRIDE+4,TCSM_RECON_UBUF0+4*RECON_BUF_STRIDE+2,TCSM_RECON_VBUF0+4*RECON_BUF_STRIDE+2,
        TCSM_RECON_YBUF0+12*RECON_BUF_STRIDE,TCSM_RECON_UBUF0+6*RECON_BUF_STRIDE,TCSM_RECON_VBUF0+6*RECON_BUF_STRIDE,
        TCSM_RECON_YBUF0+12*RECON_BUF_STRIDE+4,TCSM_RECON_UBUF0+6*RECON_BUF_STRIDE+2,TCSM_RECON_VBUF0+6*RECON_BUF_STRIDE+2
       },
       {TCSM_RECON_YBUF0+8*RECON_BUF_STRIDE+8,TCSM_RECON_UBUF0+4*RECON_BUF_STRIDE+4,TCSM_RECON_VBUF0+4*RECON_BUF_STRIDE+4,
        TCSM_RECON_YBUF0+8*RECON_BUF_STRIDE+12,TCSM_RECON_UBUF0+4*RECON_BUF_STRIDE+6,TCSM_RECON_VBUF0+4*RECON_BUF_STRIDE+6,
        TCSM_RECON_YBUF0+12*RECON_BUF_STRIDE+8,TCSM_RECON_UBUF0+6*RECON_BUF_STRIDE+4,TCSM_RECON_VBUF0+6*RECON_BUF_STRIDE+4,
        TCSM_RECON_YBUF0+12*RECON_BUF_STRIDE+12,TCSM_RECON_UBUF0+6*RECON_BUF_STRIDE+6,TCSM_RECON_VBUF0+6*RECON_BUF_STRIDE+6
       }

      };
     int SUB_X_OFFSET[4][4] = {{0, 2, 0, 2}, {4, 6, 4, 6}, {0, 2, 0, 2}, {4, 6, 4, 6}};
     int SUB_Y_OFFSET[4][4] = {{0, 0, 2, 2}, {0, 0, 2, 2}, {4, 4, 6, 6}, {4, 4, 6, 6}};
     int SUB_BLOCK_X_OFFSET[4] = {0, 4, 0, 4};
     int SUB_BLOCK_Y_OFFSET[4] = {0, 0, 4, 4};

/*H264 MC HW init*/
static void h264_mc_init()
{
  int i;
  /*descriptor-table init*/
  //H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM_MC_CHAIN0;
  H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM1_VADDR_UNCACHE(TCSM1_MC_DES_BUF0);
  for(i=0; i<32; i++) {
    /*luma:*/
    h264_mc->YNodeFlag = 0x80000000;
    h264_mc->YNodeHead = H264_YUHEAD;
    h264_mc->YTap2Coef = TAPALN(1,-5,20,20);
    h264_mc->YTap1Coef = TAPALN(1,-5,20,20);
    h264_mc->YMCStatus = 0x0;

    /*chrom_cb:*/
    h264_mc->UNodeFlag = 0x80000000;
    h264_mc->UNodeHead = H264_YUHEAD;
    h264_mc->UIntpInfo = (H1V1<<16) | (32<<8); /*always h264 1/8-pixel interpolate*/
    h264_mc->UMCStatus = 0x0;

    /*chrom_cr:*/
    h264_mc->VNodeFlag = 0x80000000;
    h264_mc->VNodeHead = H264_VHEAD_LINK;
    h264_mc->VMCStatus = 0x0;

    h264_mc++;
  }
  /*MC init config*/
  SET_MC_END_FLAG();
  RUN_MC();
  SET_COEF_REG2(-5,1,0,0);
  SET_COEF_REG4(-5,1,0,0);
}

#if 0
static int blk_num, mc_des_dirty;

/**
 * Copies a rectangular area of samples to a temporary buffer and replicates the boarder samples.
 * @param buf destination buffer
 * @param src source buffer
 * @param linesize number of bytes between 2 vertically adjacent samples in both the source and destination buffers
 * @param block_w width of block
 * @param block_h height of block
 * @param src_x x coordinate of the top left sample of the block in the source buffer
 * @param src_y y coordinate of the top left sample of the block in the source buffer
 * @param w width of the source buffer
 * @param h height of the source buffer
 */
void ff_emulated_edge_mc_hw(uint8_t *buf, uint8_t *src,
			    int buf_linesize, int src_linesize, int block_w, int block_h,
			    int src_x, int src_y, int w, int h){
    int x, y;
    int start_y, start_x, end_y, end_x;

    if(src_y>= h){
        src+= (h-1-src_y)*src_linesize;
        src_y=h-1;
    }else if(src_y<=-block_h){
        src+= (1-block_h-src_y)*src_linesize;
        src_y=1-block_h;
    }
    if(src_x>= w){
        src+= (w-1-src_x);
        src_x=w-1;
    }else if(src_x<=-block_w){
        src+= (1-block_w-src_x);
        src_x=1-block_w;
    }

    start_y= FFMAX(0, -src_y);
    start_x= FFMAX(0, -src_x);
    end_y= FFMIN(block_h, h-src_y);
    end_x= FFMIN(block_w, w-src_x);

    // copy existing part
    for(y=start_y; y<end_y; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*buf_linesize]= src[x + y*src_linesize];
        }
    }

    //top
    for(y=0; y<start_y; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*buf_linesize]= buf[x + start_y*buf_linesize];
        }
    }

    //bottom
    for(y=end_y; y<block_h; y++){
        for(x=start_x; x<end_x; x++){
            buf[x + y*buf_linesize]= buf[x + (end_y-1)*buf_linesize];
        }
    }

    for(y=0; y<block_h; y++){
       //left
        for(x=0; x<start_x; x++){
            buf[x + y*buf_linesize]= buf[start_x + y*buf_linesize];
        }

       //right
        for(x=end_x; x<block_w; x++){
            buf[x + y*buf_linesize]= buf[end_x - 1 + y*buf_linesize];
        }
    }
}

void mc_dir_part_hw(H264Context *h, Picture *pic, int n,  
				  int dest_y, int dest_cb, int dest_cr,
				  int src_x_offset, int src_y_offset,
				  int blk_size,int list){
    MpegEncContext * const s = &h->s;
    const int mx= h->mv_cache[list][ scan8[n] ][0] + src_x_offset*8;
    int my=       h->mv_cache[list][ scan8[n] ][1] + src_y_offset*8;
    const int luma_xy= (mx&3) + ((my&3)<<2);
    uint8_t * src_y = pic->data[0] + (mx>>2) + (my>>2)*h->mb_linesize;
    uint8_t * src_cb= pic->data[1] + (mx>>3) + (my>>3)*h->mb_uvlinesize;
    uint8_t * src_cr= pic->data[2] + (mx>>3) + (my>>3)*h->mb_uvlinesize;
    int extra_width= h->emu_edge_width;
    int extra_height= h->emu_edge_height;
    const int full_mx= mx>>2;
    const int full_my= my>>2;
    const int pic_width  = 16*s->mb_width;
    const int pic_height = 16*s->mb_height >> (MB_MBAFF || FIELD_PICTURE);

    if(mx&7) extra_width -= 3;
    if(my&7) extra_height -= 3;
    
    if(   full_mx < 0-extra_width
       || full_my < 0-extra_height
       || full_mx + 16/*FIXME*/ > pic_width + extra_width
       || full_my + 16/*FIXME*/ > pic_height + extra_height){
      /*
	in this case, MV is point outside of the draw_edged frame, 
	since JZ MC HWcore can't be un-cache-able in this case, so we do not use descriptor, 
	but pipe Y-Cb-Cr with HW calculation and SW setting buffer.
	the efficiency maybe a bit low, but this case is singular.
       */
#     define EMU_EDGE_LUMA_STRIDE 24
#     define EMU_EDGE_CHROM_STRIDE 12
      int i, va;
      //luma
      #if 1
      ff_emulated_edge_mc_hw(s->edge_emu_buffer, src_y - 2 - 2*h->mb_linesize, 
			     EMU_EDGE_LUMA_STRIDE, h->mb_linesize, 16+5, 16+5/*FIXME*/, 
			     full_mx-2, full_my-2, pic_width, pic_height);
      src_y= s->edge_emu_buffer + 2 + 2*EMU_EDGE_LUMA_STRIDE;
      //write back emu_edge buffer
      va = s->edge_emu_buffer;
      for(i=0; i<=16; i++) {
	i_cache(0x19,va,0);
	va += 32;
      }  
      
      MC_CACHE_FLUSH();
      SET_MC_REF_ADDR((int)get_phy_addr(src_y));
      SET_MC_CURR_ADDR(dest_y);
      SET_BLK_INFO(H264LumaBLK[blk_size]);
      SET_INTP_INFO(H264LumaIntp[luma_xy]);
      SET_MC_STRD(EMU_EDGE_LUMA_STRIDE,RECON_BUF_STRIDE);
      SET_COEF_REG1(1,-5,20,20);
      CLEAR_MC_END_FLAG();
      
      //chrom cb
      ff_emulated_edge_mc_hw(s->edge_emu_buffer + EMU_EDGE_LUMA_STRIDE*(16+5), src_cb, 
			     EMU_EDGE_CHROM_STRIDE, h->mb_uvlinesize, 9, 9/*FIXME*/, 
			     (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
      src_cb= s->edge_emu_buffer + EMU_EDGE_LUMA_STRIDE*(16+5);

      //write back emu_edge buffer
      va = src_cb;
      for(i=0; i<=4; i++) {
	i_cache(0x19,va,0);
	va += 32;
      }  
      
      MC_POLLING_BLK_END();
      MC_CACHE_FLUSH();
      SET_MC_REF_ADDR((int)get_phy_addr(src_cb));
      SET_MC_CURR_ADDR(dest_cb);
      SET_BLK_INFO(H264ChromaBLK[blk_size]);
      SET_INTP_INFO((H1V1<<16) | (32<<8));
      SET_MC_STRD(EMU_EDGE_CHROM_STRIDE,RECON_BUF_STRIDE);
      SET_COEF_REG1(8-(mx&7),mx&7,0,0);
      SET_COEF_REG3(8-(my&7),my&7,0,0);
      CLEAR_MC_END_FLAG();

      //chrom cr
      ff_emulated_edge_mc_hw(s->edge_emu_buffer + EMU_EDGE_LUMA_STRIDE*(16+5) + EMU_EDGE_CHROM_STRIDE*9, 
			     src_cr, EMU_EDGE_CHROM_STRIDE, h->mb_uvlinesize, 9, 9/*FIXME*/, 
			     (mx>>3), (my>>3), pic_width>>1, pic_height>>1);
      src_cr= s->edge_emu_buffer + EMU_EDGE_LUMA_STRIDE*(16+5) + EMU_EDGE_CHROM_STRIDE*9;

      //write back emu_edge buffer
      va = src_cr;
      for(i=0; i<=4; i++) {
	i_cache(0x19,va,0);
	va += 32;
      }  

      MC_POLLING_BLK_END();
      MC_CACHE_FLUSH();
      SET_MC_REF_ADDR((int)get_phy_addr(src_cr));
      SET_MC_CURR_ADDR(dest_cr);
      CLEAR_MC_END_FLAG();
      MC_POLLING_BLK_END();
      #endif
    } else {

      mc_des_dirty++;
      /*Dynamically config MC with luma block info*/
  //    H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM_MC_CHAIN0;
       H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM_MC_DES_BUF;
      //Luma block
      h264_mc[blk_num].YRefAddr = (int)get_phy_addr(src_y);
      h264_mc[blk_num].YCurrAddr = dest_y;
      h264_mc[blk_num].YBLKInfo = H264LumaBLK[blk_size];
      h264_mc[blk_num].YIntpInfo = H264LumaIntp[luma_xy];
      h264_mc[blk_num].YStride = h->mb_linesize | (RECON_BUF_STRIDE<<16);

      //Chroma Cb block
      h264_mc[blk_num].URefAddr = (int)get_phy_addr(src_cb);
      h264_mc[blk_num].UCurrAddr = dest_cb;
      h264_mc[blk_num].UBLKInfo = H264ChromaBLK[blk_size];
      h264_mc[blk_num].UStride = h->mb_uvlinesize | (RECON_BUF_STRIDE<<16);
      h264_mc[blk_num].UTap1Coef = TAPALN(8-(mx&7),mx&7,0,0);
      h264_mc[blk_num].UTap2Coef = TAPALN(8-(my&7),my&7,0,0);
    
      //Chrom Cr block
      h264_mc[blk_num].VNodeHead = H264_VHEAD_LINK;
      h264_mc[blk_num].VRefAddr = (int)get_phy_addr(src_cr);
      h264_mc[blk_num].VCurrAddr = dest_cr;
      blk_num++;
    }
}

void mc_part_hw(H264Context *h, int n, 
 	      int dest_y, int dest_cb, int dest_cr,
              int x_offset, int y_offset, int blk_size, int list0, int list1){
    MpegEncContext * const s = &h->s;
    x_offset += 8*s->mb_x;
    y_offset += 8*s->mb_y;
    if (list0 && list1)
     {
     Picture *ref= &h->ref_list[0][ h->ref_cache[0][ scan8[n] ] ];
     mc_dir_part_hw(h, ref, n, dest_y, dest_cb, dest_cr, x_offset, y_offset, blk_size, 0);

     ref= &h->ref_list[1][ h->ref_cache[1][ scan8[n] ] ];
     mc_dir_part_hw(h, ref, n, dest_y + RECON_YBUF_LEN, dest_cb + RECON_UVBUF_LEN, 
     dest_cr + RECON_UVBUF_LEN, x_offset, y_offset, blk_size, 1);
     }
    else if (list0)
     {
    Picture *ref= &h->ref_list[0][ h->ref_cache[0][ scan8[n] ] ];
    mc_dir_part_hw(h, ref, n, dest_y, dest_cb, dest_cr, x_offset, y_offset, blk_size, 0);
     }
    else if(list1) 
     {
    Picture *ref= &h->ref_list[1][ h->ref_cache[1][ scan8[n] ] ];
    mc_dir_part_hw(h, ref, n, dest_y, dest_cb, dest_cr, x_offset, y_offset, blk_size, 1);
     }
}

void hl_motion_hw(H264Context *h){
  MpegEncContext * const s = &h->s;
  const int mb_xy= s->mb_x + s->mb_y*s->mb_stride;
  const int mb_type= s->current_picture.mb_type[mb_xy];

  blk_num =0;
  mc_des_dirty =0;
  assert(IS_INTER(mb_type));

  if(IS_16X16(mb_type)){
   mc_part_hw(h, 0, MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2], 0, 0,MC_BLK_16X16,
   IS_DIR(mb_type, 0, 0),IS_DIR(mb_type, 0, 1));
  }
  else if(IS_16X8(mb_type)){
    mc_part_hw(h, 0,MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2], 0, 0,MC_BLK_16X8,
    IS_DIR(mb_type, 0, 0), IS_DIR(mb_type, 0, 1));

    mc_part_hw(h, 8,MB_BLOCK_OFFSET_TAB[3], MB_BLOCK_OFFSET_TAB[4],MB_BLOCK_OFFSET_TAB[5], 0, 4,MC_BLK_16X8,
    IS_DIR(mb_type, 1, 0), IS_DIR(mb_type, 1, 1));

  }else if(IS_8X16(mb_type)){
   mc_part_hw(h, 0, MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2], 0, 0,MC_BLK_8X16,
   IS_DIR(mb_type, 0, 0), IS_DIR(mb_type, 0, 1));

   mc_part_hw(h, 4,MB_BLOCK_OFFSET_TAB[6], MB_BLOCK_OFFSET_TAB[7],MB_BLOCK_OFFSET_TAB[8], 4, 0,MC_BLK_8X16,
   IS_DIR(mb_type, 1, 0), IS_DIR(mb_type, 1, 1));
  }else{
     int i;
    for(i=0; i<4; i++){
      const int sub_mb_type= h->sub_mb_type[i];
      const int n= 4*i;

      if(IS_SUB_8X8(sub_mb_type)){
        mc_part_hw(h, n, MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],MB_SUBBLOCK_OFFSET_TAB[3*i+2],
        SUB_BLOCK_X_OFFSET[i],SUB_BLOCK_Y_OFFSET[i],
        MC_BLK_8X8, IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));
      }else if(IS_SUB_8X4(sub_mb_type)){
        mc_part_hw(h,n, MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],MB_SUBBLOCK_OFFSET_TAB[3*i+2],
        SUB_BLOCK_X_OFFSET[i],SUB_BLOCK_Y_OFFSET[i],MC_BLK_8X4, IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

        mc_part_hw(h,n+2,MB_SUBBLOCK_8X4_OFFSET_TAB[3*i],MB_SUBBLOCK_8X4_OFFSET_TAB[3*i+1],MB_SUBBLOCK_8X4_OFFSET_TAB[3*i+2],
        SUB_BLOCK_X_OFFSET[i],SUB_BLOCK_Y_OFFSET[i]+2,MC_BLK_8X4,  IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));
      }else if(IS_SUB_4X8(sub_mb_type)){
        mc_part_hw(h, n, MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],MB_SUBBLOCK_OFFSET_TAB[3*i+2],
        SUB_BLOCK_X_OFFSET[i],SUB_BLOCK_Y_OFFSET[i],MC_BLK_4X8, IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));

        mc_part_hw(h,n+1,MB_SUBBLOCK_4X8_OFFSET_TAB[3*i],MB_SUBBLOCK_4X8_OFFSET_TAB[3*i+1],MB_SUBBLOCK_4X8_OFFSET_TAB[3*i+2],
        SUB_BLOCK_X_OFFSET[i]+2,SUB_BLOCK_Y_OFFSET[i],MC_BLK_4X8,IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));
      }else{
         int j;
        for(j=0; j<4; j++){
       mc_part_hw(h,n+j,MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j],MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j+1],
       MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j+2],SUB_X_OFFSET[i][j],SUB_Y_OFFSET[i][j], MC_BLK_4X4,
       IS_DIR(sub_mb_type, 0, 0), IS_DIR(sub_mb_type, 0, 1));
       }
      }
    }
  }

  if(mc_des_dirty){
  H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM_MC_DES_BUF;
  h264_mc[blk_num - 1].VNodeHead = H264_VHEAD_UNLINK;
  SET_MC_DHA(TCSM_MC_DES_BUF);
  CLEAR_MC_TTEND();
  SET_MC_DCS();
  }
  MC_POLLING_END();
  hl_motion_hw_next(h, mb_type);
}

 void hl_motion_hw_next(H264Context *h, int mb_type){
  assert(IS_INTER(mb_type));
//  int list0,list1;
 // int refn0,refn1;
 // char * dest_y = TCSM_RECON_YBUF0;
 // char * dest_cb = TCSM_RECON_UBUF0;
 // char * dest_cr = TCSM_RECON_VBUF0;

  if(IS_16X16(mb_type)){
   Post_MC_AVG_Weight_c(h, 0, 0, mb_type,MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2],16, 16);
  }
 else if(IS_16X8(mb_type)){
   Post_MC_AVG_Weight_c(h, 0, 0,mb_type, MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2],16, 8);
   Post_MC_AVG_Weight_c(h, 1, 8,mb_type, MB_BLOCK_OFFSET_TAB[3], MB_BLOCK_OFFSET_TAB[4],MB_BLOCK_OFFSET_TAB[5],16, 8);
  }else if(IS_8X16(mb_type)){
   Post_MC_AVG_Weight_c(h, 0, 0,mb_type, MB_BLOCK_OFFSET_TAB[0], MB_BLOCK_OFFSET_TAB[1],MB_BLOCK_OFFSET_TAB[2],8 , 16);
   Post_MC_AVG_Weight_c(h, 1, 4,mb_type, MB_BLOCK_OFFSET_TAB[6], MB_BLOCK_OFFSET_TAB[7],MB_BLOCK_OFFSET_TAB[8],8 , 16);
  }else{
    int i;
    for(i=0; i<4; i++){
     const int sub_mb_type= h->sub_mb_type[i];
     const int n= 4*i;

    if(IS_SUB_8X8(sub_mb_type)){
      Post_MC_AVG_Weight_c(h,   0, n,sub_mb_type,MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],
      MB_SUBBLOCK_OFFSET_TAB[3*i+2],8, 8);
      }else if(IS_SUB_8X4(sub_mb_type)){
        Post_MC_AVG_Weight_c(h, 0, n,sub_mb_type,MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],
      MB_SUBBLOCK_OFFSET_TAB[3*i+2],8, 4);
        Post_MC_AVG_Weight_c(h, 0, n+2,sub_mb_type,MB_SUBBLOCK_8X4_OFFSET_TAB[3*i],MB_SUBBLOCK_8X4_OFFSET_TAB[3*i+1],
      MB_SUBBLOCK_8X4_OFFSET_TAB[3*i+2],8, 4);

      }else if(IS_SUB_4X8(sub_mb_type)){
        
        Post_MC_AVG_Weight_c(h, 0, n  , sub_mb_type,MB_SUBBLOCK_OFFSET_TAB[3*i],MB_SUBBLOCK_OFFSET_TAB[3*i+1],
        MB_SUBBLOCK_OFFSET_TAB[3*i+2],4, 8);
        Post_MC_AVG_Weight_c(h, 0, n+1, sub_mb_type,MB_SUBBLOCK_4X8_OFFSET_TAB[3*i],MB_SUBBLOCK_4X8_OFFSET_TAB[3*i+1],
        MB_SUBBLOCK_4X8_OFFSET_TAB[3*i+2],4, 8);
      
     }else{
	int j;
	for(j=0; j<4; j++){
        Post_MC_AVG_Weight_c(h, 0, n+j, sub_mb_type,MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j],MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j+1],
        MB_SUBBLOCK_4X4_OFFSET_TAB[i][3*j+2],4, 4);      
        }
        }  
   }

  }
}

void  Post_MC_AVG_Weight_c(H264Context *h, int part, int n, int mb_type,uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,int  W,int H)
{
   int list0,list1,refn0,refn1;
   list0 = IS_DIR(mb_type, part, 0);        list1 = IS_DIR(mb_type, part,1);
   refn0 = h->ref_cache[0][ scan8[n] ];  refn1 = h->ref_cache[1][ scan8[n] ];

   if((h->use_weight==2 && list0 && list1 && (h->implicit_weight[refn0][refn1] != 32)) || h->use_weight==1)
    {
       if(list0 && list1)
       MXU_DirectB_Weight_MBAVG(h, refn0, refn1, list0, list1, dest_y, dest_cb, dest_cr,W,H);
       else
       {
       int list = list1 ? 1 : 0;
       int refn = h->ref_cache[list][ scan8[0] ];
       MXU_DirectB_Weight(h, refn,list, dest_y, dest_cb, dest_cr,W,H);
       }
    }
   else
   {
       if(list0 && list1)
       MXU_DirectB_MBAVG(dest_y, dest_cb, dest_cr, W, H);
   }
}

void mc_tmp_copy(char *dest_y, char *dest_cb, char *dest_cr, int linesize, int uvlinesize){
  char *dst_tmp, *src_tmp; int i, j;
  src_tmp = (char *)TCSM_RECON_YBUF0;//TCSM_YRECON_BUF;
  dst_tmp = (char *)dest_y;
  for(j=0;j<16;j++){
    for(i=0;i<16;i++)
      dst_tmp[i] = src_tmp[i];
    src_tmp += RECON_BUF_STRIDE;
    dst_tmp += linesize;
  }
  src_tmp = (char *)TCSM_RECON_UBUF0;//TCSM_URECON_BUF;
  dst_tmp = (char *)dest_cb;
  for(j=0;j<8;j++){
    for(i=0;i<8;i++)
      {dst_tmp[i] = src_tmp[i];
      }
    src_tmp += RECON_BUF_STRIDE;
    dst_tmp += uvlinesize;
  }
  src_tmp = (char *)TCSM_RECON_VBUF0;//TCSM_VRECON_BUF;
  dst_tmp = (char *)dest_cr;
  for(j=0;j<8;j++){
    for(i=0;i<8;i++)
      {dst_tmp[i] = src_tmp[i];
      }
    src_tmp += RECON_BUF_STRIDE;
    dst_tmp += uvlinesize;
  }
  
}

void  MXU_DirectB_Weight_MBAVG(H264Context *h, int refn0, int refn1, int list0, int list1, uint8_t *dest_y,uint8_t * dest_cb,uint8_t * dest_cr,int W, int H)
{
       if(h->use_weight == 2){
         int weight0 = h->implicit_weight[refn0][refn1];
         int weight1 = 64 - weight0;

         biweight_h264_luma_c(dest_y,  (dest_y  + RECON_YBUF_LEN), RECON_BUF_STRIDE, 5, weight0, weight1, 0,W ,H);
         biweight_h264_chroma_c(dest_cb, (dest_cb + RECON_UVBUF_LEN), RECON_BUF_STRIDE, 5, weight0, weight1, 0,W ,H);
         biweight_h264_chroma_c(dest_cr, (dest_cr + RECON_UVBUF_LEN), RECON_BUF_STRIDE, 5, weight0, weight1, 0,W ,H);
        }
    else{

         biweight_h264_luma_c(dest_y, (dest_y  + RECON_YBUF_LEN), RECON_BUF_STRIDE, h->luma_log2_weight_denom, h->luma_weight[0][refn0],
 h->luma_weight[1][refn1], h->luma_offset[0][refn0] + h->luma_offset[1][refn1],W,H);
         biweight_h264_chroma_c(dest_cb,(dest_cb + RECON_UVBUF_LEN), RECON_BUF_STRIDE, h->chroma_log2_weight_denom,h->chroma_weight[0][refn0][0], h->chroma_weight[1][refn1][0],h->chroma_offset[0][refn0][0] + h->chroma_offset[1][refn1][0],W,H);
         biweight_h264_chroma_c(dest_cr, (dest_cr + RECON_UVBUF_LEN), RECON_BUF_STRIDE, h->chroma_log2_weight_denom,h->chroma_weight[0][refn0][1], h->chroma_weight[1][refn1][1],h->chroma_offset[0][refn0][1] + h->chroma_offset[1][refn1][1],W,H);

       }  
}


void  MXU_DirectB_Weight(H264Context *h, int  refn,int list, uint8_t * dest_y,uint8_t * dest_cb,uint8_t * dest_cr,int W,int H)
{
        weight_h264_luma_c(dest_y, RECON_BUF_STRIDE, h->luma_log2_weight_denom,h->luma_weight[list][refn], h->luma_offset[list][refn],W,H);
        if(h->use_weight_chroma){
        weight_h264_chroma_c(dest_cb, RECON_BUF_STRIDE, h->chroma_log2_weight_denom,
                             h->chroma_weight[list][refn][0], h->chroma_offset[list][refn][0],W,H);
        weight_h264_chroma_c(dest_cr,  RECON_BUF_STRIDE, h->chroma_log2_weight_denom,
                             h->chroma_weight[list][refn][1], h->chroma_offset[list][refn][1],W,H);
        }
}


void MXU_DirectB_MBAVG(const uint8_t *MCYReconBuf,const uint8_t *MCUReconBuf,const uint8_t *MCVReconBuf, int w ,int h)
{
  int i,j,m,n;
  uint8_t * src_y0 = MCYReconBuf;
  uint8_t * src_y1 = src_y0 + MB_LUMA_WIDTH;
  uint8_t * dst_y  = src_y0;

  uint8_t * src_u0 = MCUReconBuf;
  uint8_t * src_u1 = src_u0 + MB_CHROM_WIDTH;
  uint8_t * dst_u  = src_u0;

  uint8_t * src_v0 = MCVReconBuf;
  uint8_t * src_v1 = src_v0 + MB_CHROM_WIDTH;
  uint8_t * dst_v  = src_v0;

  uint8_t * src_tmp_y0 = src_y0;
  uint8_t * src_tmp_y1 = src_y1;
  uint8_t * dst_tmp_y  = dst_y ;

  uint8_t * src_tmp_u0 = src_u0;
  uint8_t * src_tmp_u1 = src_u1;
  uint8_t * dst_tmp_u  = dst_u;

  uint8_t * src_tmp_v0 = src_v0;
  uint8_t * src_tmp_v1 = src_v1;
  uint8_t * dst_tmp_v  = dst_v;


  for(m = 0 ; m < (h/4); m ++)
   {
    
   for(n = 0 ; n < (w/4) ; n++)
   {
      S32LDD(xr1, src_y0, 0);
      S32LDD(xr2, src_y1, 0);
      Q8AVGR(xr13, xr1, xr2);
      S32STD(xr13, dst_y, 0);

      S32LDD(xr1, src_y0,  RECON_BUF_STRIDE);
      S32LDD(xr2, src_y1,  RECON_BUF_STRIDE);

      Q8AVGR(xr13, xr1, xr2);
      S32STD(xr13, dst_y,  RECON_BUF_STRIDE);

      S32LDD(xr1, src_y0, 2 * RECON_BUF_STRIDE);
      S32LDD(xr2, src_y1, 2 * RECON_BUF_STRIDE);

      Q8AVGR(xr13, xr1, xr2);
      S32STD(xr13, dst_y, 2 * RECON_BUF_STRIDE);

      S32LDD(xr1, src_y0, 3 * RECON_BUF_STRIDE);
      S32LDD(xr2, src_y1, 3 * RECON_BUF_STRIDE);

      Q8AVGR(xr13, xr1, xr2);
      S32STD(xr13, dst_y, 3 * RECON_BUF_STRIDE);
      
      S16LDD(xr5, src_u0, 0, 0);
      S16LDD(xr6, src_u1, 0, 0);

      S16LDD(xr7, src_v0, 0, 0);
      S16LDD(xr8, src_v1, 0, 0);

      Q8AVGR(xr13, xr5, xr6);
      S16STD(xr13, dst_u, 0, 0);

      Q8AVGR(xr13, xr7, xr8);
      S16STD(xr13, dst_v, 0, 0);

      S16LDD(xr5, src_u0, RECON_BUF_STRIDE, 0);
      S16LDD(xr6, src_u1, RECON_BUF_STRIDE, 0);

      S16LDD(xr7, src_v0, RECON_BUF_STRIDE, 0);
      S16LDD(xr8, src_v1, RECON_BUF_STRIDE, 0);

      Q8AVGR(xr13, xr5, xr6);
      S16STD(xr13, dst_u, RECON_BUF_STRIDE, 0);

      Q8AVGR(xr13, xr7, xr8);
      S16STD(xr13, dst_v, RECON_BUF_STRIDE, 0);

      src_y0 += 4;
      src_y1 += 4;
      src_u0 += 2;
      src_u1 += 2;
      src_v0 += 2;
      src_v1 += 2;
      dst_y  += 4;
      dst_u  += 2;
      dst_v  += 2;
  }
      src_y0 =  src_tmp_y0 + 4 * RECON_BUF_STRIDE;
      src_y1 =  src_tmp_y1 + 4 * RECON_BUF_STRIDE;
      src_u0 =  src_tmp_u0 + 2 * RECON_BUF_STRIDE;
      src_u1 =  src_tmp_u1 + 2 * RECON_BUF_STRIDE;
      src_v0 =  src_tmp_v0 + 2 * RECON_BUF_STRIDE;
      src_v1 =  src_tmp_v1 + 2 * RECON_BUF_STRIDE;
      dst_y  =  dst_tmp_y  + 4 * RECON_BUF_STRIDE;
      dst_u  =  dst_tmp_u  + 2 * RECON_BUF_STRIDE;
      dst_v  =  dst_tmp_v  + 2 * RECON_BUF_STRIDE;

      src_tmp_y0 += 4 * RECON_BUF_STRIDE;
      src_tmp_y1 += 4 * RECON_BUF_STRIDE;
      src_tmp_u0 += 2 * RECON_BUF_STRIDE;
      src_tmp_u1 += 2 * RECON_BUF_STRIDE;
      src_tmp_v0 += 2 * RECON_BUF_STRIDE;
      src_tmp_v1 += 2 * RECON_BUF_STRIDE;
      dst_tmp_y  += 4 * RECON_BUF_STRIDE;
      dst_tmp_u  += 2 * RECON_BUF_STRIDE;
      dst_tmp_v  += 2 * RECON_BUF_STRIDE;
 }
}

 void biweight_h264_luma_c(uint8_t *dst, uint8_t *src, int stride, int log2_denom, int weight0, int weight1, int offset, int W, int H)
         {
            int x,y;
            offset = ((offset + 1) | 1) << log2_denom;
            for(y=0; y<H; y++, dst += stride, src += stride)
            {
             for(x=0; x<W; x+=4)
             {
             dst[x+0] = av_clip_uint8( (src[x+0]*weight1 + dst[x+0]*weight0 + offset) >> (log2_denom+1));
             dst[x+1] = av_clip_uint8( (src[x+1]*weight1 + dst[x+1]*weight0 + offset) >> (log2_denom+1));
             dst[x+2] = av_clip_uint8( (src[x+2]*weight1 + dst[x+2]*weight0 + offset) >> (log2_denom+1));
             dst[x+3] = av_clip_uint8( (src[x+3]*weight1 + dst[x+3]*weight0 + offset) >> (log2_denom+1));
             }
           }
         }
  void biweight_h264_chroma_c(uint8_t *dst, uint8_t *src, int stride, int log2_denom, int weight0, int weight1, int offset, int W, int H)
        {
         int x,y;
         offset = ((offset + 1) | 1) << log2_denom;
         for(y=0; y<H/2; y++)
            {
              for(x=0; x<W/2; x+=2)
              {
              dst[x+0] = av_clip_uint8( (src[x+0]*weight1 + dst[x+0]*weight0 + offset) >> (log2_denom+1));
              dst[x+1] = av_clip_uint8( (src[x+1]*weight1 + dst[x+1]*weight0 + offset) >> (log2_denom+1));
              }
              dst += RECON_BUF_STRIDE;
              src += RECON_BUF_STRIDE;
           }
        }
  void weight_h264_luma_c(uint8_t *block, int stride, int log2_denom, int weight, int offset, int W, int H){
      int x,y;                                                                                                                    offset <<= log2_denom;
      if(log2_denom) offset += 1<<(log2_denom-1);
      for(y=0; y<H; y++, block += stride)
      {
             for(x=0; x<W; x+=4)
             {
             block[x+0] = av_clip_uint8( (block[x+0]*weight + offset) >> log2_denom);
             block[x+1] = av_clip_uint8( (block[x+1]*weight + offset) >> log2_denom);
             block[x+2] = av_clip_uint8( (block[x+2]*weight + offset) >> log2_denom);
             block[x+3] = av_clip_uint8( (block[x+3]*weight + offset) >> log2_denom);
             }
      }
    }
 void weight_h264_chroma_c(uint8_t *block, int stride, int log2_denom, int weight, int offset, int W, int H){
    int x,y;
      offset <<= log2_denom;
      if(log2_denom) offset += 1<<(log2_denom-1);
      for(y=0; y<H/2; y++)
            {
              for(x=0; x<W/2; x+=2)
              {
               block[x+0] = av_clip_uint8( (block[x+0]*weight + offset) >> log2_denom);
               block[x+1] = av_clip_uint8( (block[x+1]*weight + offset) >> log2_denom);
              }
              block += stride;
              block += stride;
           }
   }

#endif // if 0
