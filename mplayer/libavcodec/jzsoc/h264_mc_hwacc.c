/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264 Baseline-profile)
 *
* $Id: h264_mc_hwacc.c,v 1.3 2009/03/25 06:36:57 jyu Exp $
 *
 ****************************************************************************/
//extern unsigned char * frame_buffer;
//extern unsigned int phy_fb;
#define get_phy_addr(a) (((unsigned int)(a)) & 0x1FFFFFFF)
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

const uint32_t h264_6tap_rnd_lut[16] = {
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

const uint32_t h264_luma_blk_lut[7] = {
 MC_BLK_H16 | (MC_BLK_W16<<2) | (H264_QPEL<<8),
 MC_BLK_H8 | (MC_BLK_W16<<2) | (H264_QPEL<<8),
 MC_BLK_H16 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
 MC_BLK_H8 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
 MC_BLK_H4 | (MC_BLK_W8<<2) | (H264_QPEL<<8),
 MC_BLK_H8 | (MC_BLK_W4<<2) | (H264_QPEL<<8),
 MC_BLK_H4 | (MC_BLK_W4<<2) | (H264_QPEL<<8)
};

const uint32_t  h264_chrom_blk_lut[7] = {
  MC_BLK_H8 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W2<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W2<<2) | (H264_EPEL<<8)
}; 


/*H264 MC descriptor-Table init*/
static __inline void h264_mc_des_tab_init()
{
  h264_mc_des_node *mc_node = TCSM_H264_MC_DES_TAB;
  int i;
  for(i=0; i<16; i++) {
    /*luma:*/
    mc_node[i].luma_head = 0x91;
    mc_node[i].luma_ref_addr_i = MC_REF_ADDR;
    mc_node[i].luma_ref2_addr_i = MC_REF2_ADDR;
    mc_node[i].luma_cur_addr_i = MC_CURR_ADDR;
    mc_node[i].luma_blk_info_i = MC_BLK_INFO;
    mc_node[i].luma_itp_info_i = MC_INTP_INFO;
    mc_node[i].luma_cur_strd_i = MC_CURR_STRD;
    mc_node[i].luma_ref2_strd_i = MC_REF2_STRD;
    mc_node[i].luma_tap_coef_i = MC_TAP_COEF_REG1;
    mc_node[i].luma_tap_coef_d = get_mc_tap_coef(1,-5,20,20);
    mc_node[i].luma_mc_stat_i = MC_STATUS;
    mc_node[i].luma_mc_stat_d = 0x0;
    /*chrom_cb:*/
    mc_node[i].chrom_cb_head = 0xa1;
    mc_node[i].chrom_cb_ref_addr_i = MC_REF_ADDR;
    mc_node[i].chrom_cb_ref2_addr_i = MC_REF2_ADDR;
    mc_node[i].chrom_cb_cur_addr_i = MC_CURR_ADDR;
    mc_node[i].chrom_cb_blk_info_i = MC_BLK_INFO;
    mc_node[i].chrom_cb_itp_info_i = MC_INTP_INFO;
    mc_node[i].chrom_cb_itp_info_d = (H1V1<<16) | (32<<8);
    mc_node[i].chrom_cb_cur_strd_i = MC_CURR_STRD;
    mc_node[i].chrom_cb_ref2_strd_i = MC_REF2_STRD;
    mc_node[i].chrom_cb_tap_reg1_i = MC_TAP_COEF_REG1;
    mc_node[i].chrom_cb_tap_reg3_i = MC_TAP_COEF_REG3;
    mc_node[i].chrom_cb_mc_stat_i = MC_STATUS;
    mc_node[i].chrom_cb_mc_stat_d = 0x0;
    /*chrom_cr:*/
    mc_node[i].chrom_cr_head = 0x41;
    mc_node[i].chrom_cr_ref_addr_i = MC_REF_ADDR;
    mc_node[i].chrom_cr_ref2_addr_i = MC_REF2_ADDR;
    mc_node[i].chrom_cr_cur_addr_i = MC_CURR_ADDR;
    mc_node[i].chrom_cr_mc_stat_i = MC_STATUS;
    mc_node[i].chrom_cr_mc_stat_d = 0x0;
  }
}

/*MC REG init*/
static __inline void h264_mc_reg_init()
{
  set_mc_flag();
  set_mc_dha(TCSM_H264_MC_DES_TAB);
  set_coef_reg2(-5,1,0,0);
}


static __inline void h264_mc_init()
{
  h264_mc_reg_init();
  h264_mc_des_tab_init();
}


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

static inline void mc_dir_part_hw(H264Context *h, Picture *pic, int n, int list, 
                                  uint8_t *src2_y, uint8_t *src2_cb, uint8_t *src2_cr,
				  int dest_y, int dest_cb, int dest_cr,
				  int src_x_offset, int src_y_offset, uint32_t B_frame){
    MpegEncContext * const s = &h->s;
    int blk_num=h->blk_num;
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
    int blk_size = h->blk_size;
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
      
      mc_cache_flush();
      //      set_mc_ref_addr( ((int)src_y & 0x0FFFFFFF) | 0x20000000); 
//      set_mc_ref_addr((int)(src_y - frame_buffer) + phy_fb);
      set_mc_ref_addr(get_phy_addr(src_y)) ;
      set_mc_ref2_addr(get_phy_addr(src2_y)) ;
      set_mc_curr_addr(dest_y);
      set_blk_info_data((h264_luma_blk_lut[blk_size]|(B_frame<<MC_AVG_SFT)));
      //set_blk_info_data((h264_luma_blk_lut[blk_size]));
      set_intp_info_data(h264_6tap_rnd_lut[luma_xy]);
      set_mc_curr_strd(EMU_EDGE_LUMA_STRIDE,20);
      set_mc_ref2_strd(h->mb_linesize);
      set_coef_reg1(1,-5,20,20);
      clear_mc_end_flag();
      
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
      
      mc_polling_blk_flag();
      mc_cache_flush();
      //      set_mc_ref_addr( ((int)src_cb & 0x0FFFFFFF) | 0x20000000);
      //set_mc_ref_addr((int)(src_cb - frame_buffer) + phy_fb);

      set_mc_ref_addr(get_phy_addr(src_cb)) ;
      set_mc_ref2_addr(get_phy_addr(src2_cb)) ;
      set_mc_curr_addr(dest_cb);
      set_blk_info_data((h264_chrom_blk_lut[blk_size]|(B_frame<<MC_AVG_SFT)));
      //set_blk_info_data((h264_chrom_blk_lut[blk_size]));
      set_intp_info_data((H1V1<<16) | (32<<8));
      set_mc_curr_strd(EMU_EDGE_CHROM_STRIDE,(MB_CHROM_WIDTH + 4));
      set_coef_reg1(8-(mx&7),mx&7,0,0);
      set_coef_reg3(8-(my&7),my&7,0,0);
      set_mc_ref2_strd(h->mb_uvlinesize);
      clear_mc_end_flag();

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

      mc_polling_blk_flag();
      mc_cache_flush();
      //      set_mc_ref_addr( ((int)src_cr & 0x0FFFFFFF) | 0x20000000);
      //set_mc_ref_addr((int)(src_cr - frame_buffer) + phy_fb);
      set_mc_ref_addr(get_phy_addr(src_cr)) ;
      set_mc_ref2_addr(get_phy_addr(src2_cr)) ;
      set_mc_curr_addr(dest_cr);
      set_mc_ref2_strd(h->mb_uvlinesize);
      clear_mc_end_flag();
      mc_polling_blk_flag();

    } else {
      h->mc_des_dirty++;
      /*Dynamically config MC with luma block info*/
      h264_mc_des_node *mc_node = TCSM_H264_MC_DES_TAB;
      //    int *mc_des_tab = (TCSM_H264_MC_DES_TAB + blk_num*(H264_MC_DES_NODE_LEN<<2) );
      //Luma block
      //      mc_node[blk_num].luma_ref_addr_d = ((int)src_y & 0x0FFFFFFF) | 0x20000000;
      //mc_node[blk_num].luma_ref_addr_d = (int)(src_y - frame_buffer) + phy_fb;
      mc_node[blk_num].luma_ref_addr_d = get_phy_addr(src_y);
      mc_node[blk_num].luma_ref2_addr_d = get_phy_addr(src2_y);
      mc_node[blk_num].luma_cur_addr_d = dest_y;
      mc_node[blk_num].luma_blk_info_d = (h264_luma_blk_lut[blk_size]|(B_frame<<MC_AVG_SFT));
      //mc_node[blk_num].luma_blk_info_d = (h264_luma_blk_lut[blk_size]);
      mc_node[blk_num].luma_itp_info_d = h264_6tap_rnd_lut[luma_xy];
      mc_node[blk_num].luma_cur_strd_d = h->mb_linesize | ((MB_LUMA_WIDTH + 4)<<16);
      mc_node[blk_num].luma_ref2_strd_d = h->mb_linesize;

      //Chroma Cb block
      //      mc_node[blk_num].chrom_cb_ref_addr_d = ((int)src_cb & 0x0FFFFFFF) | 0x20000000;
      //mc_node[blk_num].chrom_cb_ref_addr_d = (int)(src_cb - frame_buffer) + phy_fb;
      mc_node[blk_num].chrom_cb_ref_addr_d = get_phy_addr(src_cb);
      mc_node[blk_num].chrom_cb_ref2_addr_d = get_phy_addr(src2_cb);
      mc_node[blk_num].chrom_cb_cur_addr_d = dest_cb;
      mc_node[blk_num].chrom_cb_blk_info_d = (h264_chrom_blk_lut[blk_size]|(B_frame<<MC_AVG_SFT));
      //mc_node[blk_num].chrom_cb_blk_info_d = (h264_chrom_blk_lut[blk_size]);
      mc_node[blk_num].chrom_cb_cur_strd_d = h->mb_uvlinesize | ((MB_CHROM_WIDTH + 4)<<16);
      mc_node[blk_num].chrom_cb_ref2_strd_d = h->mb_uvlinesize ;
      mc_node[blk_num].chrom_cb_tap_reg1_d = get_mc_tap_coef(8-(mx&7),mx&7,0,0);
      mc_node[blk_num].chrom_cb_tap_reg3_d = get_mc_tap_coef(8-(my&7),my&7,0,0);
    
      //Chrom Cr block
      mc_node[blk_num].chrom_cr_head = 0x41;
      //      mc_node[blk_num].chrom_cr_ref_addr_d = ((int)src_cr & 0x0FFFFFFF) | 0x20000000;
      //mc_node[blk_num].chrom_cr_ref_addr_d = (int)(src_cr - frame_buffer) + phy_fb;
      mc_node[blk_num].chrom_cr_ref_addr_d = get_phy_addr(src_cr);
      mc_node[blk_num].chrom_cr_ref2_addr_d = get_phy_addr(src2_cr);
      mc_node[blk_num].chrom_cr_cur_addr_d = dest_cr;
      blk_num++;
    }
    h->blk_num=blk_num;
}
#define CACHE_FLUSH_BASE 0x80000000
static void jzc_dcache_wb()
{
  int i, va;
  va = CACHE_FLUSH_BASE;
  for(i=0; i<512; i++) {
    i_cache(0x1,va,0);
    va += 32;
  }  
}

static void jzc_dcache_wbi()
{
  int i, va;
  va = CACHE_FLUSH_BASE;
  for(i=0; i<512; i++) {
    i_cache(0x15,va,0);
    va += 32;
  }  
}
