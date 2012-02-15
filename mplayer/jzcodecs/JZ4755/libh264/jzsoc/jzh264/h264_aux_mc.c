/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264)
 *
 * $Id: h264_aux_mc.c,v 1.8 2009/05/18 00:48:43 pwang Exp $
 *
 ****************************************************************************/

#include "../jz4750e_mc.h"
#include "h264_mc_hw.h"
#include "h264_tcsm.h"
#include "../jzmedia.h"

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

const unsigned short MB_SUBBLOCK_OFFSET_TAB[12] ={
  //8*8
  0,                     RECON_U_OFST,                     RECON_V_OFST,
  0 + 8                 ,RECON_U_OFST + 4                 ,RECON_V_OFST + 4,
  0 + 8*RECON_BUF_STRIDE,RECON_U_OFST + 4*RECON_BUF_STRIDE,RECON_V_OFST + 4*RECON_BUF_STRIDE,
  0 + 8*RECON_BUF_STRIDE + 8,RECON_U_OFST + 4*RECON_BUF_STRIDE + 4,RECON_V_OFST + 4*RECON_BUF_STRIDE+4
};

const unsigned short MB_SUBBLOCK_8X4_OFFSET_TAB[12] ={
  //8*4
  0 + 4*RECON_BUF_STRIDE,RECON_U_OFST + 2*RECON_BUF_STRIDE,RECON_V_OFST + 2*RECON_BUF_STRIDE,
  0 + 4*RECON_BUF_STRIDE + 8,RECON_U_OFST + 2*RECON_BUF_STRIDE + 4,RECON_V_OFST + 2*RECON_BUF_STRIDE+4,
  0 +12*RECON_BUF_STRIDE,RECON_U_OFST + 6*RECON_BUF_STRIDE,RECON_V_OFST + 6*RECON_BUF_STRIDE,
  0 +12*RECON_BUF_STRIDE + 8,RECON_U_OFST + 6*RECON_BUF_STRIDE + 4,RECON_V_OFST + 6*RECON_BUF_STRIDE+4
};

const unsigned short MB_SUBBLOCK_4X8_OFFSET_TAB[12] ={
  //4*8
  0 + 4,                 RECON_U_OFST + 2,                 RECON_V_OFST + 2,
  0 + 12                 ,RECON_U_OFST + 6                 ,RECON_V_OFST + 6,
  0 + 8*RECON_BUF_STRIDE + 4,RECON_U_OFST + 4*RECON_BUF_STRIDE + 2,RECON_V_OFST + 4*RECON_BUF_STRIDE+2,
  0 + 8*RECON_BUF_STRIDE +12,RECON_U_OFST + 4*RECON_BUF_STRIDE + 6,RECON_V_OFST + 4*RECON_BUF_STRIDE+6
};

const unsigned short MB_SUBBLOCK_4X4_OFFSET_TAB[4][12] ={
  {0,RECON_U_OFST,RECON_V_OFST,
   0+4,RECON_U_OFST+2,RECON_V_OFST+2,
   0+4*RECON_BUF_STRIDE,RECON_U_OFST+2*RECON_BUF_STRIDE,RECON_V_OFST+2*RECON_BUF_STRIDE,
   0+4*RECON_BUF_STRIDE+4,RECON_U_OFST+2*RECON_BUF_STRIDE+2,RECON_V_OFST+2*RECON_BUF_STRIDE+2
  },
  {0+8,RECON_U_OFST+4,RECON_V_OFST+4,
   0+12,RECON_U_OFST+6,RECON_V_OFST+6,
   0+4*RECON_BUF_STRIDE+8,RECON_U_OFST+2*RECON_BUF_STRIDE+4,RECON_V_OFST+2*RECON_BUF_STRIDE+4,
   0+4*RECON_BUF_STRIDE+12,RECON_U_OFST+2*RECON_BUF_STRIDE+6,RECON_V_OFST+2*RECON_BUF_STRIDE+6
  },
  {
    0+8*RECON_BUF_STRIDE  ,RECON_U_OFST+4*RECON_BUF_STRIDE  ,RECON_V_OFST+4*RECON_BUF_STRIDE,
    0+8*RECON_BUF_STRIDE+4,RECON_U_OFST+4*RECON_BUF_STRIDE+2,RECON_V_OFST+4*RECON_BUF_STRIDE+2,
    0+12*RECON_BUF_STRIDE,RECON_U_OFST+6*RECON_BUF_STRIDE,RECON_V_OFST+6*RECON_BUF_STRIDE,
    0+12*RECON_BUF_STRIDE+4,RECON_U_OFST+6*RECON_BUF_STRIDE+2,RECON_V_OFST+6*RECON_BUF_STRIDE+2
  },
  {0+8*RECON_BUF_STRIDE+8,RECON_U_OFST+4*RECON_BUF_STRIDE+4,RECON_V_OFST+4*RECON_BUF_STRIDE+4,
   0+8*RECON_BUF_STRIDE+12,RECON_U_OFST+4*RECON_BUF_STRIDE+6,RECON_V_OFST+4*RECON_BUF_STRIDE+6,
   0+12*RECON_BUF_STRIDE+8,RECON_U_OFST+6*RECON_BUF_STRIDE+4,RECON_V_OFST+6*RECON_BUF_STRIDE+4,
   0+12*RECON_BUF_STRIDE+12,RECON_U_OFST+6*RECON_BUF_STRIDE+6,RECON_V_OFST+6*RECON_BUF_STRIDE+6
  }
};
unsigned short  STRIDE_TAB[3] = {(RECON_BUF_STRIDE << 1), 3 * RECON_BUF_STRIDE,(RECON_BUF_STRIDE << 2)};
const uint8_t SUB_X_OFFSET[4][4] = {{0, 2, 0, 2}, {4, 6, 4, 6}, {0, 2, 0, 2}, {4, 6, 4, 6}};
const uint8_t SUB_Y_OFFSET[4][4] = {{0, 0, 2, 2}, {0, 0, 2, 2}, {4, 4, 6, 6}, {4, 4, 6, 6}};
const uint8_t SUB_BLOCK_X_OFFSET[4] = {0, 4, 0, 4};
const uint8_t SUB_BLOCK_Y_OFFSET[4] = {0, 0, 4, 4};

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
void ff_emulated_edge_mc_hw(H264_AUX_T *AUX_T, uint8_t *src_yuv, int yuv_i,
			    int src_x_yuv, int src_y_yuv, int w, int h)
{
  uint8_t *buf = H264_TCSM1_EDGE_EMU_BUF;
  uint8_t *src = get_phy_addr_aux(src_yuv);

  int block_w, block_h, src_linesize, src_x, src_y;
  if (yuv_i) {
    src_linesize = AUX_T->mb_uvlinesize;
    block_w = 9;
    src_x = src_x_yuv >> 1;
    src_y = src_y_yuv >> 1;
    w >>= 1;
    h >>= 1;
  } else {
    src_linesize = AUX_T->mb_linesize;
    block_w = 21;
    src_x = src_x_yuv - 2;
    src_y = src_y_yuv - 2;
  }
  block_h = block_w;

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
      buf[x + y*EMU_EDGE_LUMA_STRIDE]= src[x + y*src_linesize];
    }
  }

  //top
  for(y=0; y<start_y; y++){
    for(x=start_x; x<end_x; x++){
      buf[x + y*EMU_EDGE_LUMA_STRIDE]= buf[x + start_y*EMU_EDGE_LUMA_STRIDE];
    }
  }

  //bottom
  for(y=end_y; y<block_h; y++){
    for(x=start_x; x<end_x; x++){
      buf[x + y*EMU_EDGE_LUMA_STRIDE]= buf[x + (end_y-1)*EMU_EDGE_LUMA_STRIDE];
    }
  }

  for(y=0; y<block_h; y++){
    //left
    for(x=0; x<start_x; x++){
      buf[x + y*EMU_EDGE_LUMA_STRIDE]= buf[start_x + y*EMU_EDGE_LUMA_STRIDE];
    }

    //right
    for(x=end_x; x<block_w; x++){
      buf[x + y*EMU_EDGE_LUMA_STRIDE]= buf[end_x - 1 + y*EMU_EDGE_LUMA_STRIDE];
    }
  }
}

void mc_dir_part_hw(H264_MB_DecARGs *dMB_aux, H264_AUX_T *AUX_T,
		    int n,
		    int dest_y, int dest_cb, int dest_cr,
		    int src_x_offset, int src_y_offset,
		    int blk_size,int list){

  H264_Frame_GlbARGs *dFRM = (H264_Frame_GlbARGs *)TCSM1_FRM_ARGS;

  const int mx= dMB_aux->mv_cache[list][ scan5[n] ][0] + (src_x_offset << 3);
  const int my= dMB_aux->mv_cache[list][ scan5[n] ][1] + (src_y_offset << 3);
  const int luma_xy= (mx&3) + ((my&3)<<2);
  H264_Frame_Ptr *pic= &(dFRM->ref_list[list][dMB_aux->ref_cache[list][ scan5[n] ] ]);
  uint8_t * src_y = pic->y_ptr + (mx>>2) + (my>>2)*dFRM->linesize;
  uint8_t * src_cb= pic->u_ptr + (mx>>3) + (my>>3)*dFRM->uvlinesize;
  uint8_t * src_cr= pic->v_ptr + (mx>>3) + (my>>3)*dFRM->uvlinesize;
  int extra_width= dFRM->emu_edge_width;
  int extra_height= dFRM->emu_edge_height;
  const int full_mx= mx>>2;
  const int full_my= my>>2;
  const int pic_width  = 16*dFRM->mb_width;
#ifdef H264_FIELD_ENABLE
  const int pic_height = 16*dFRM->mb_height >> (MB_MBAFF || FIELD_PICTURE);
#else
  const int pic_height = 16*dFRM->mb_height;
#endif//#ifdef H264_FIELD_ENABLE

  i_movn(extra_width, extra_width - 3, mx & 7);
  i_movn(extra_height, extra_height - 3, my & 7);

  if( full_mx < 0-extra_width
      || full_my < 0-extra_height
      || full_mx + 16/*FIXME*/ > pic_width + extra_width
      || full_my + 16/*FIXME*/ > pic_height + extra_height ){
    /*
      in this case, MV is point outside of the draw_edged frame, 
      since JZ MC HWcore can't be un-cache-able in this case, so we do not use descriptor, 
      but pipe Y-Cb-Cr with HW calculation and SW setting buffer.
      the efficiency maybe a bit low, but this case is singular.
    */
    int i;
    unsigned int src_ptr[3];
    src_ptr[0] = src_y - 2 - 2*AUX_T->mb_linesize;
    src_ptr[1] = src_cb;
    src_ptr[2] = src_cr;

    unsigned int dst_ptr[3];
    dst_ptr[0] = dest_y;
    dst_ptr[1] = dest_cb;
    dst_ptr[2] = dest_cr;

    MC_POLLING_END();

    for (i=0; i<3; i++) {
      uint8_t * src_yuv = src_ptr[i];
      uint8_t * dst_yuv = dst_ptr[i];

      ff_emulated_edge_mc_hw(AUX_T, src_yuv, i, full_mx, full_my, pic_width, pic_height);

      src_yuv = TCSM1_VADDR_FOR_MC(H264_TCSM1_EDGE_EMU_BUF);
      i_movz(src_yuv, src_yuv + 2 + 2*EMU_EDGE_LUMA_STRIDE,i);
      RUN_MC_GER();
      SET_MC_REF_ADDR(src_yuv);
      SET_MC_CURR_ADDR(dst_yuv);
      SET_MC_STRD(EMU_EDGE_LUMA_STRIDE,RECON_BUF_STRIDE);

      if ( i ) {
	SET_BLK_INFO(H264ChromaBLK[blk_size]);
	SET_INTP_INFO((H1V1<<16) | (32<<8));
	SET_COEF_REG1(8-(mx&7),mx&7,0,0);
	SET_COEF_REG3(8-(my&7),my&7,0,0);
      } else {
	SET_BLK_INFO(H264LumaBLK[blk_size]);
	SET_INTP_INFO(H264LumaIntp[luma_xy]);
	SET_COEF_REG1(1,-5,20,20);
      }

      CLEAR_MC_END_FLAG();
      MC_POLLING_BLK_END();
    }
  } else {

    if ( AUX_T->mc_des_dirty == 16 ) {

      MC_POLLING_END();

      volatile unsigned int *infar = (unsigned int *)TCSM1_H264_DBLK_INFAR;
      while ((infar[0])!= TCSM1_H264_DBLK_INFDA) {}

      H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)(AUX_T->h264_mc_des_ptr[0]);
      h264_mc[AUX_T->mc_des_dirty - 1].VNodeHead = H264_VHEAD_UNLINK;
      SET_MC_DHA((uint32_t)h264_mc);
      CLEAR_MC_TTEND();
      SET_MC_DCS();
      MC_POLLING_END();

      AUX_T->mc_des_dirty = 0;
    }

    /*Dynamically config MC with luma block info*/
    //    H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)TCSM_MC_CHAIN0;
    H264_MC_DesNode *h264_mc = (H264_MC_DesNode *)(AUX_T->h264_mc_des_ptr[0]);
    uint8_t blk_num=AUX_T->mc_des_dirty;
    //Luma block
    h264_mc[blk_num].YRefAddr = get_phy_addr_aux(src_y);
    h264_mc[blk_num].YCurrAddr = dest_y;
    h264_mc[blk_num].YBLKInfo = H264LumaBLK[blk_size];
    h264_mc[blk_num].YIntpInfo = H264LumaIntp[luma_xy];
    h264_mc[blk_num].YStride = dFRM->linesize | (RECON_BUF_STRIDE<<16);
	
    //Chroma Cb block
    h264_mc[blk_num].URefAddr = get_phy_addr_aux(src_cb);
    h264_mc[blk_num].UCurrAddr = dest_cb;
    h264_mc[blk_num].UBLKInfo = H264ChromaBLK[blk_size];
    h264_mc[blk_num].UStride = dFRM->uvlinesize | (RECON_BUF_STRIDE<<16);
    h264_mc[blk_num].UTap1Coef = TAPALN(8-(mx&7),mx&7,0,0);
    h264_mc[blk_num].UTap2Coef = TAPALN(8-(my&7),my&7,0,0);
	
    //Chrom Cr block
    h264_mc[blk_num].VNodeHead = H264_VHEAD_LINK;
    h264_mc[blk_num].VRefAddr = get_phy_addr_aux(src_cr);
    h264_mc[blk_num].VCurrAddr = dest_cr;
    AUX_T->mc_des_dirty++;
  }
}   

void mc_part_hw(H264_MB_DecARGs *dMB_aux,
		int part, int mb_type, int n, unsigned short *ofs_tab,
		int x_offset, int y_offset, int blk_size)
{
  H264_AUX_T *AUX_T = (H264_AUX_T *)TCSM1_AUX_T;

  x_offset += (dMB_aux->mb_x << 3);
  y_offset += (dMB_aux->mb_y << 3);
  int dest_y=AUX_T->h264_yrecon[0]+ofs_tab[0];
  int dest_cb=AUX_T->h264_yrecon[0]+ofs_tab[1];
  int dest_cr=AUX_T->h264_yrecon[0]+ofs_tab[2];
 
  int list0, list1, refn0,refn1;
  list0 = IS_DIR(mb_type, part, 0);        list1 = IS_DIR(mb_type, part,1);   

  if (list0)
    {
      mc_dir_part_hw(dMB_aux, AUX_T, n, dest_y, dest_cb, dest_cr, x_offset, y_offset, blk_size, 0);
  
      dest_y =  dest_y  + RECON_YBUF_LEN;
      dest_cb=  dest_cb + RECON_UVBUF_LEN;
      dest_cr=  dest_cr + RECON_UVBUF_LEN;

    }
  if(list1) 
    {
      mc_dir_part_hw(dMB_aux, AUX_T, n, dest_y, dest_cb, dest_cr, x_offset, y_offset, blk_size, 1);
    }

}

void hl_motion_hw(H264_Frame_GlbARGs *dFRM,H264_MB_DecARGs *dMB_aux, H264_AUX_T *AUX_T){
  const int mb_type= dMB_aux->mb_type;
   int blk_size,  type_n, x_offset, y_offset, ofset;
  int count = 0;

  if(IS_16X16(mb_type) ||(IS_16X8(mb_type)) || (IS_8X16(mb_type)))
    {
      if(IS_16X16(mb_type)){
	blk_size = MC_BLK_16X16;
        }
       else if(IS_16X8(mb_type)){
         blk_size = MC_BLK_16X8;  count = 1;
         x_offset = 0; y_offset = 4;  ofset = 6;  type_n = 8;
        }
       else{
         blk_size = MC_BLK_8X16;  count = 1; 
         x_offset = 4; y_offset = 0;  ofset = 3;   type_n = 4;
        }
      mc_part_hw(dMB_aux, 0, mb_type, 0, MB_SUBBLOCK_OFFSET_TAB,0,0,blk_size);
      if (count == 1)
      mc_part_hw(dMB_aux, 1, mb_type, type_n,MB_SUBBLOCK_OFFSET_TAB+ofset,x_offset, y_offset,blk_size); 
    }
  else{
   int i;  
    for(i=0; i<4; i++){
      const int sub_mb_type= dMB_aux->sub_mb_type[i];
      const int n= 4*i;
    
      if(IS_SUB_8X8(sub_mb_type) ||(IS_SUB_8X4(sub_mb_type)) || (IS_SUB_4X8(sub_mb_type))) 
	{
	  if(IS_SUB_8X8(sub_mb_type)){
            count = 0; 	    blk_size = MC_BLK_8X8;
	  }
	  else if(IS_SUB_8X4(sub_mb_type)){
	    blk_size = MC_BLK_8X4;   count = 1;
	    x_offset = 0;            y_offset = 2;
	    ofset = MB_SUBBLOCK_8X4_OFFSET_TAB+3*i;  type_n = n+2;
	  }
	  else{
	    blk_size = MC_BLK_4X8;   count = 1; 
	    x_offset = 2;            y_offset = 0; 
	    ofset = MB_SUBBLOCK_4X8_OFFSET_TAB+3*i;   type_n = n+1;
	  }
	  mc_part_hw(dMB_aux, 0, sub_mb_type, n, MB_SUBBLOCK_OFFSET_TAB+3*i,
          SUB_BLOCK_X_OFFSET[i],SUB_BLOCK_Y_OFFSET[i],blk_size );
	  if (count == 1)
	  mc_part_hw(dMB_aux, 0, sub_mb_type, type_n,ofset,
          SUB_BLOCK_X_OFFSET[i]+x_offset, SUB_BLOCK_Y_OFFSET[i]+y_offset,blk_size);
	}
       else{
	int j;
        for(j=0; j<4; j++){
	  mc_part_hw(dMB_aux, 0, sub_mb_type, n+j,
		     MB_SUBBLOCK_4X4_OFFSET_TAB[i]+3*j,SUB_X_OFFSET[i][j],SUB_Y_OFFSET[i][j], 
		     MC_BLK_4X4);
	}
      }
    }
  }
}


#ifdef MOTION_NEXT_ADD

  void hl_motion_hw_next(H264_MB_DecARGs *dMB_aux, H264_AUX_T *AUX_T, int mb_type){
  int blk_size,  type_n, ofset , W, H;
  int count = 0;
  if(IS_16X16(mb_type) ||(IS_16X8(mb_type)) || (IS_8X16(mb_type)))
    {
      if(IS_16X16(mb_type)){
        W = 16; H = 16;
      }
      else if(IS_16X8(mb_type)){
	W = 16; H = 8;	count = 1;  ofset = 6;  type_n = 8;
      }
      else{
	W = 8; H = 16;  count = 1;  ofset = 3;   type_n = 4;
      }
      Post_MC_AVG_Weight_c(dMB_aux, AUX_T, 0, 0, mb_type, MB_SUBBLOCK_OFFSET_TAB, W, H);
      if (count == 1)
      Post_MC_AVG_Weight_c(dMB_aux, AUX_T, 1, type_n,mb_type, MB_SUBBLOCK_OFFSET_TAB+ofset,W , H);
    }

  else{       
   int i;
    for(i=0; i<4; i++){
      const int sub_mb_type= AUX_T->sub_mb_type[i];
      const int n= 4*i;
 
      if(IS_SUB_8X8(sub_mb_type) ||(IS_SUB_8X4(sub_mb_type)) || (IS_SUB_4X8(sub_mb_type)))
	{
	  if(IS_SUB_8X8(sub_mb_type)){
	    count = 0; W = 8; H = 8;
	  }
	  else if(IS_SUB_8X4(sub_mb_type)){
	    W = 8; H = 4; count = 1;     
	    ofset = MB_SUBBLOCK_8X4_OFFSET_TAB+3*i;  type_n = n+2;
	  }
	  else{
	    W = 4; H = 8; count = 1;         
	    ofset = MB_SUBBLOCK_4X8_OFFSET_TAB+3*i;  type_n = n+1;
	  }
	  Post_MC_AVG_Weight_c(dMB_aux, AUX_T, 0, n, sub_mb_type, MB_SUBBLOCK_OFFSET_TAB + 3 * i, W, H);
	  if (count == 1)
	  Post_MC_AVG_Weight_c(dMB_aux, AUX_T, 0, type_n,sub_mb_type, ofset ,W , H);
	}   
      else{
	int j;
	for(j=0; j<4; j++){
	  Post_MC_AVG_Weight_c(dMB_aux, AUX_T, 0, n+j, sub_mb_type,MB_SUBBLOCK_4X4_OFFSET_TAB[i]+3*j,4, 4);      
	}
      }  
    }
  }
}

 void  Post_MC_AVG_Weight_c(H264_MB_DecARGs *dMB_aux, H264_AUX_T *AUX_T, int part, int n, int mb_type,
                            unsigned short *ofs_tab ,int  W,int H)
{
    int dest_y  = AUX_T->h264_yrecon[1]+ofs_tab[0];
    int dest_cb = AUX_T->h264_yrecon[1]+ofs_tab[1];
    int dest_cr = AUX_T->h264_yrecon[1]+ofs_tab[2];
 
   int list0,list1,refn0,refn1;
   list0 = IS_DIR(mb_type, part, 0);        list1 = IS_DIR(mb_type, part,1);
   int AND_val = list0 && list1;

   if((dMB_aux->use_weight==2 && AND_val && (dMB_aux->implicit_weight[n] != 32)) || dMB_aux->use_weight==1)
    { 
       int log2_denom_y,log2_denom_u,log2_denom_v, offset_y, offset_u, offset_v;
       char weight0_y,weight0_u,weight0_v,weight1_y,weight1_u,weight1_v;
       if(AND_val)
	 {
	   if(dMB_aux->use_weight == 2)
	     {
	       weight0_y = dMB_aux->implicit_weight[n];
	       weight0_u = weight0_y;
	       weight0_v = weight0_y;

	       weight1_y = 64 - weight0_y;
	       weight1_u = weight1_y;
	       weight1_v = weight1_y;

	       offset_y =  offset_u =  offset_v = 32;
	       log2_denom_y =  log2_denom_u =  log2_denom_v =  6;                    
	     }
	   else
	     { 
	       log2_denom_y = dMB_aux->luma_log2_weight_denom ;
	       log2_denom_u = dMB_aux->chroma_log2_weight_denom ;
	       log2_denom_v = log2_denom_u ;

	       weight0_y = dMB_aux->luma_weight[0][n];
	       weight0_u = dMB_aux->chroma_weight[0][n][0];
	       weight0_v = dMB_aux->chroma_weight[0][n][1];

	       weight1_y = dMB_aux->luma_weight[1][n];
	       weight1_u = dMB_aux->chroma_weight[1][n][0];
	       weight1_v = dMB_aux->chroma_weight[1][n][1];

	       offset_y = dMB_aux->luma_offset[0][n] + dMB_aux->luma_offset[1][n];
	       offset_u = dMB_aux->chroma_offset[0][n][0] + dMB_aux->chroma_offset[1][n][0];
	       offset_v = dMB_aux->chroma_offset[0][n][1] + dMB_aux->chroma_offset[1][n][1];         
         
	       offset_y = ((offset_y + 1) | 1) << log2_denom_y;
	       offset_u = ((offset_u + 1) | 1) << log2_denom_u;
	       offset_v = ((offset_v + 1) | 1) << log2_denom_v;

	       log2_denom_y = log2_denom_y + 1;
	       log2_denom_u = log2_denom_u + 1;
	       log2_denom_v = log2_denom_u + 1;
          
	     }
	 }
       else
	 {
	   int list = list1 ? 1 : 0;
	   log2_denom_y = dMB_aux->luma_log2_weight_denom;
	   weight0_y    = dMB_aux->luma_weight[list][n];
	   weight1_y    = 0;
	   offset_y     = dMB_aux->luma_offset[list][n];
   
	   offset_y <<= log2_denom_y;
	   i_movn(offset_y, offset_y +1<<(log2_denom_y-1),log2_denom_y);
	   if(dMB_aux->use_weight_chroma)
	     {
	       log2_denom_u = dMB_aux->chroma_log2_weight_denom;
	       log2_denom_v = log2_denom_u;

      
	       weight0_u = dMB_aux->chroma_weight[list][n][0];
	       weight0_v = dMB_aux->chroma_weight[list][n][1];
         
	       weight1_u = 0;
	       weight1_v = 0;
  
	       offset_u = dMB_aux->chroma_offset[list][n][0];
	       offset_v = dMB_aux->chroma_offset[list][n][1]; 
       
	       offset_u <<= log2_denom_u;
	       i_movn(offset_u, offset_u +1<<(log2_denom_u-1),log2_denom_u);
	       offset_v <<= log2_denom_v;
	       i_movn(offset_v, offset_v +1<<(log2_denom_v-1),log2_denom_v);
	     }
       
	 }
       int x,y;
       uint8_t * dst_y = dest_y - 4;            
       uint8_t * dst_u = dest_cb - 2;
       uint8_t * dst_v = dest_cr - 2;

       uint8_t *src_y = dst_y +  RECON_YBUF_LEN;
       uint8_t *src_u = dst_u +  RECON_UVBUF_LEN;
       uint8_t *src_v = dst_v +  RECON_UVBUF_LEN;

       uint8_t *src_y_tmp = src_y;
       uint8_t *src_u_tmp = src_u;
       uint8_t *src_v_tmp = src_v;  

       uint8_t *dst_y_tmp = dst_y;
       uint8_t *dst_u_tmp = dst_u;
       uint8_t *dst_v_tmp = dst_v;

       S8LDD(xr2,&weight1_y,0,7);
       S8LDD(xr3,&weight0_y,0,7);
       S16LDD(xr4,&offset_y,0,3);
       S8LDD(xr15,&weight1_u,0,7);
       S8LDD(xr14,&weight0_u,0,7);
       S8LDD(xr1,&weight1_v,0,7);
       S8LDD(xr12,&weight0_v,0,7);
       S16LDD(xr13,&offset_u,0,3);
       S16LDD(xr8,&offset_v,0,3);
            
       for(y=0; y<(H/4); y++)
	 {
	   for(x=0; x<(W/4); x++)
	     {          
	       S32LDI(xr5,dst_y,4);
	       S32LDI(xr6,src_y,4);	    
	       Q8MULSU(xr11,xr3,xr5,xr7);
	       Q8MACSU_AA(xr11,xr2,xr6,xr7);          
	       Q16ACCM_AA(xr11,xr4,xr4,xr7);
	       Q16SLRV(xr11,xr7,(log2_denom_y));
	       Q16SAT(xr10,xr11,xr7);
	       S32STD(xr10,dst_y,0);

	       S32LDD(xr5,dst_y,RECON_BUF_STRIDE);
	       S32LDD(xr6,src_y,RECON_BUF_STRIDE);	   
	       Q8MULSU(xr11,xr3,xr5,xr7);
	       Q8MACSU_AA(xr11,xr2,xr6,xr7); 	
	       Q16ACCM_AA(xr11,xr4,xr4,xr7);
	       Q16SLRV(xr11,xr7,(log2_denom_y));
	       Q16SAT(xr10,xr11,xr7);
	       S32STD(xr10,dst_y,RECON_BUF_STRIDE);

	       S32LDD(xr5,dst_y,2*RECON_BUF_STRIDE);
	       S32LDD(xr6,src_y,2*RECON_BUF_STRIDE);	    
	       Q8MULSU(xr11,xr3,xr5,xr7);
	       Q8MACSU_AA(xr11,xr2,xr6,xr7); 
	       Q16ACCM_AA(xr11,xr4,xr4,xr7);
	       Q16SLRV(xr11,xr7,(log2_denom_y));
	       Q16SAT(xr10,xr11,xr7);
	       S32STD(xr10,dst_y,2*RECON_BUF_STRIDE);

	       S32LDD(xr5,dst_y,3*RECON_BUF_STRIDE);
	       S32LDD(xr6,src_y,3*RECON_BUF_STRIDE);	   
	       Q8MULSU(xr11,xr3,xr5,xr7);
	       Q8MACSU_AA(xr11,xr2,xr6,xr7); 
	       Q16ACCM_AA(xr11,xr4,xr4,xr7);
	       Q16SLRV(xr11,xr7,(log2_denom_y));
	       Q16SAT(xr10,xr11,xr7);
	       S32STD(xr10,dst_y,3*RECON_BUF_STRIDE);
            
	       S16LDI(xr5,dst_u,2,0);
	       S16LDI(xr6,src_u,2,0);
	       Q8MULSU(xr0,xr14,xr5,xr7);
	       Q8MULSU(xr0,xr15,xr6,xr9);
	       Q16ACC_AA(xr7,xr9,xr13,xr0);	       
	       Q16SLRV(xr0,xr7,(log2_denom_u));
	       Q16SAT(xr10,xr0,xr7);
	       S16STD(xr10,dst_u,0,0);

	       S16LDD(xr5,dst_u,RECON_BUF_STRIDE,0);
	       S16LDD(xr6,src_u,RECON_BUF_STRIDE,0);	    
	       Q8MULSU(xr0,xr14,xr5,xr7);
	       Q8MULSU(xr0,xr15,xr6,xr9);
	       Q16ACC_AA(xr7,xr9,xr13,xr0); 
	       Q16SLRV(xr0,xr7,(log2_denom_u));
	       Q16SAT(xr10,xr0,xr7);
	       S16STD(xr10,dst_u,RECON_BUF_STRIDE,0);

	       S16LDI(xr5,dst_v,2,0);
	       S16LDI(xr6,src_v,2,0);
	       Q8MULSU(xr0,xr12,xr5,xr7);
	       Q8MULSU(xr0,xr1,xr6,xr9);
	       Q16ACC_AA(xr7,xr9,xr8,xr0);	       
	       Q16SLRV(xr0,xr7,(log2_denom_v));
	       Q16SAT(xr10,xr0,xr7);
	       S16STD(xr10,dst_v,0,0);

	       S16LDD(xr5,dst_v,RECON_BUF_STRIDE,0);
	       S16LDD(xr6,src_v,RECON_BUF_STRIDE,0);
	       Q8MULSU(xr0,xr12,xr5,xr7);
	       Q8MULSU(xr0,xr1,xr6,xr9);
	       Q16ACC_AA(xr7,xr9,xr8,xr0); 
	       Q16SLRV(xr0,xr7,(log2_denom_v));
	       Q16SAT(xr10,xr0,xr7);
	       S16STD(xr10,dst_v,RECON_BUF_STRIDE,0);
	     }
	   dst_y = dst_y_tmp +  STRIDE_TAB[2];
	   dst_u = dst_u_tmp +  STRIDE_TAB[0];
	   dst_v = dst_v_tmp +  STRIDE_TAB[0];

	   src_y = src_y_tmp + STRIDE_TAB[2];
	   src_u = src_u_tmp + STRIDE_TAB[0];
	   src_v = src_v_tmp + STRIDE_TAB[0];

	   dst_y_tmp +=  STRIDE_TAB[2];
	   dst_u_tmp +=  STRIDE_TAB[0];
	   dst_v_tmp +=  STRIDE_TAB[0];

	   src_y_tmp += STRIDE_TAB[2];
	   src_u_tmp += STRIDE_TAB[0];
	   src_v_tmp += STRIDE_TAB[0];
	 }
    }
   else 
   {  
     if(AND_val)//   MXU_DirectB_MBAVG(dest_y, dest_cb ,dest_cr, W ,H);               
       {
	 int i,j,m,n;
	 uint8_t * src_y0 = dest_y - 4;
	 uint8_t * src_u0 = dest_cb - 2;
	 uint8_t * src_v0 = dest_cr - 2;

	 uint8_t * src_y1 = src_y0 + MB_LUMA_WIDTH;
	 uint8_t * src_u1 = src_u0 + MB_CHROM_WIDTH;
	 uint8_t * src_v1 = src_v0 + MB_CHROM_WIDTH;
 
	 uint8_t * src_tmp_y0 = src_y0;
	 uint8_t * src_tmp_y1 = src_y1;
 
	 uint8_t * src_tmp_u0 = src_u0;
	 uint8_t * src_tmp_u1 = src_u1;
 
	 uint8_t * src_tmp_v0 = src_v0;
	 uint8_t * src_tmp_v1 = src_v1;
 

	 for(m = 0 ; m < (H/4); m ++)
	   {
    
	     for(n = 0 ; n < (W/4) ; n++)
	       {
		 S32LDI(xr1, src_y0, 4);
		 S32LDI(xr2, src_y1, 4);
		 Q8AVGR(xr13, xr1, xr2);
		 S32STD(xr13, src_y0, 0);

		 S32LDD(xr1, src_y0,  RECON_BUF_STRIDE);
		 S32LDD(xr2, src_y1,  RECON_BUF_STRIDE);

		 Q8AVGR(xr13, xr1, xr2);
		 S32STD(xr13, src_y0,  RECON_BUF_STRIDE);

		 S32LDD(xr1, src_y0,  2*RECON_BUF_STRIDE);
		 S32LDD(xr2, src_y1,  2*RECON_BUF_STRIDE);

		 Q8AVGR(xr13, xr1, xr2);
		 S32STD(xr13, src_y0, 2*RECON_BUF_STRIDE);

		 S32LDD(xr1, src_y0,  3*RECON_BUF_STRIDE);
		 S32LDD(xr2, src_y1,  3*RECON_BUF_STRIDE);

		 Q8AVGR(xr13, xr1, xr2);
		 S32STD(xr13, src_y0, 3*RECON_BUF_STRIDE);
      
		 S16LDI(xr5, src_u0, 2, 0);
		 S16LDI(xr6, src_u1, 2, 0);

		 S16LDI(xr7, src_v0, 2, 0);
		 S16LDI(xr8, src_v1, 2, 0);
 
		 Q8AVGR(xr13, xr5, xr6);
		 S16STD(xr13, src_u0, 0, 0);

		 Q8AVGR(xr13, xr7, xr8);
		 S16STD(xr13, src_v0, 0, 0);

		 S16LDD(xr5, src_u0, RECON_BUF_STRIDE, 0);
		 S16LDD(xr6, src_u1, RECON_BUF_STRIDE, 0);

		 S16LDD(xr7, src_v0, RECON_BUF_STRIDE, 0);
		 S16LDD(xr8, src_v1, RECON_BUF_STRIDE, 0);

		 Q8AVGR(xr13, xr5, xr6);
		 S16STD(xr13, src_u0, RECON_BUF_STRIDE, 0);

		 Q8AVGR(xr13, xr7, xr8);
		 S16STD(xr13, src_v0, RECON_BUF_STRIDE, 0);

     
	       }
	     src_y0 =  src_tmp_y0 +  STRIDE_TAB[2];
	     src_y1 =  src_tmp_y1 +  STRIDE_TAB[2];
	     src_u0 =  src_tmp_u0 +  STRIDE_TAB[0];
	     src_u1 =  src_tmp_u1 +  STRIDE_TAB[0];
	     src_v0 =  src_tmp_v0 +  STRIDE_TAB[0];
	     src_v1 =  src_tmp_v1 +  STRIDE_TAB[0];

	     src_tmp_y0 +=  STRIDE_TAB[2];
	     src_tmp_y1 +=  STRIDE_TAB[2];
	     src_tmp_u0 +=  STRIDE_TAB[0];
	     src_tmp_u1 +=  STRIDE_TAB[0];
	     src_tmp_v0 +=  STRIDE_TAB[0];
	     src_tmp_v1 +=  STRIDE_TAB[0];
	   }
       }
   }   
} 

#endif //#ifdef MOTION_NEXT_ADD

