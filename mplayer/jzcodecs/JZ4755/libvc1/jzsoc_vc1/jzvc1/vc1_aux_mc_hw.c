/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264)
 *
 * $Id: vc1_aux_mc_hw.c,v 1.3 2009/07/03 07:14:21 gwliu Exp $
 *
 ****************************************************************************/

#include "../jz4750e_mc.h"
#include "vc1_mc_hw.h"
#include "vc1_aux.h"
//#include "vc1_tcsm.h"
//#include "../../mpegvideo.h"    
#include "../jzmedia.h"
//#include "../../dsputil.h"

extern uint32_t VC1LumaIntp[16];

# define MC_BLK_16X16 0
# define MC_BLK_16X8 1
# define MC_BLK_8X16 2    
# define MC_BLK_8X8 3
# define MC_BLK_8X4 4
# define MC_BLK_4X8 5
# define MC_BLK_4X4 6  
   
/*VC1 Tabfilter coeff LUT*/
const int32_t VC1LumaTap1[16][4] = {
  {0,0,0,0},     {-4,53,18,-3}, {-1,9,9,-1} ,  {-3,18,53,-4} ,
  {-4,53,18,-3},     {-4,53,18,-3}, {-1,9,9,-1} ,  {-3,18,53,-4} ,
  {-1,9,9,-1},     {-4,53,18,-3}, {-1,9,9,-1} ,  {-3,18,53,-4} ,
  {-3,18,53,-4},     {-4,53,18,-3}, {-1,9,9,-1} ,  {-3,18,53,-4}
};

const int32_t VC1LumaTap2[16][4] = {
  {0,0,0,0},        {0,0,0,0},     {0,0,0,0},     {0,0,0,0}    ,
  {0,0,0,0},    {-4,53,18,-3}, {-4,53,18,-3}, {-4,53,18,-3},
  {0,0,0,0},      {-1,9,9,-1}  , {-1,9,9,-1},   {-1,9,9,-1},
  {0,0,0,0},    {-3,18,53,-4}, {-3,18,53,-4}, {-3,18,53,-4}
};

/*VC1 luma block infomation LUT*/
const uint32_t VC1LumaBLK1[7] = {
  MC_BLK_H16 | (MC_BLK_W16<<2) | (MPEG_HPEL<<8),
  MC_BLK_H8 | (MC_BLK_W16<<2) | (MPEG_HPEL<<8),
  MC_BLK_H16 | (MC_BLK_W8<<2) | (MPEG_HPEL<<8),
  MC_BLK_H8 | (MC_BLK_W8<<2) | (MPEG_HPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (MPEG_HPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (MPEG_HPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (MPEG_HPEL<<8)
};

/*VC1 luma block infomation LUT*/
const uint32_t VC1LumaBLK[7] = {
  MC_BLK_H16 | (MC_BLK_W16<<2) | (VC1_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W16<<2) | (VC1_QPEL<<8),
  MC_BLK_H16 | (MC_BLK_W8<<2) | (VC1_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W8<<2) | (VC1_QPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (VC1_QPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (VC1_QPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (VC1_QPEL<<8)
};

/*VC1 chroma block infomation LUT*/
const uint32_t  VC1ChromaBLK[7] = {
  MC_BLK_H8 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W8<<2) | (H264_EPEL<<8),
  MC_BLK_H8 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W4<<2) | (H264_EPEL<<8),
  MC_BLK_H4 | (MC_BLK_W2<<2) | (H264_EPEL<<8),
  MC_BLK_H2 | (MC_BLK_W2<<2) | (H264_EPEL<<8)
}; 


const  uint32_t MPEG2LumaIntp[2][4] =
  {
    {(H0V0<<16),(H2V0<<16)|1, (H0V2<<16)|1,(H2V2<<16)|(2 << 8)},

    {(H0V0<<16),(H2V0<<16),(H0V2<<16),(H2V2<<16)|(1<<8)}

  };
const  uint32_t MPEG2LumaIntp1[4] =
  {
    (H0V0<<16)|(1<<20),(H2V0<<16)|(1<<20),(H0V2<<16)|(1<<20),(H2V2<<16)|(1<<20)|(1<<8)
  };

extern int blk_num, mc_des_dirty;


static  int mid_pred_mxu(int a, int b, int c)
{
#ifdef HAVE_CMOV
    int i=b;
    asm volatile(
        "cmp    %2, %1 \n\t"
        "cmovg  %1, %0 \n\t"
        "cmovg  %2, %1 \n\t"
        "cmp    %3, %1 \n\t"
        "cmovl  %3, %1 \n\t"
        "cmp    %1, %0 \n\t"
        "cmovg  %1, %0 \n\t"
        :"+&r"(i), "+&r"(a)
        :"r"(b), "r"(c)
    );
    return i;
#elif 0
    int t= (a-b)&((a-b)>>31);
    a-=t;
    b+=t;
    b-= (b-c)&((b-c)>>31);
    b+= (a-b)&((a-b)>>31);

    return b;
#else

    S32I2M(xr1,a);
    S32I2M(xr2,b);
    S32I2M(xr3,c);

    S32MIN(xr4,xr1,xr2);
    S32MAX(xr5,xr1,xr2);
    S32MAX(xr6,xr4,xr3);
    S32MIN(xr7,xr6,xr5);

    return S32M2I(xr7);
#endif
}

static int median4_mxu(int a, int b, int c, int d)
{
  S32I2M(xr1,a);
  S32I2M(xr2,b);
  S32I2M(xr3,c);
  S32I2M(xr4,d);

  S32MAX(xr5,xr1,xr2);
  S32MAX(xr6,xr3,xr4);

  S32MIN(xr7,xr1,xr2);
  S32MIN(xr8,xr3,xr4);

  S32MIN(xr1,xr5,xr6);
  S32MAX(xr2,xr7,xr8);

  D32ADD_AA(xr3,xr1,xr2,xr4);
  
  return S32M2I(xr3)/2;
}


void vc1_mc_1mv_hw(int dir, uint8_t *emu_edge_buf_mb)
{
  uint8_t *srcY, *srcU, *srcV;
  int dxy, uvdxy, mx, my, uvmx, uvmy, src_x, src_y, uvsrc_x, uvsrc_y;

  uint8_t *dest_y =  VC1_aux_t.recon[0];
  uint8_t *dest_cb=  VC1_aux_t.recon[1];
  uint8_t *dest_cr=  VC1_aux_t.recon[2];

  if(!dFRM->last_data[0])return;
  mx = dMB_aux->vc1_mv[dir][0];
  my = dMB_aux->vc1_mv[dir][1];

  uvmx = (mx + ((mx & 3) == 3)) >> 1;
  uvmy = (my + ((my & 3) == 3)) >> 1;
  if(dFRM->fastuvmc) {
    uvmx = uvmx + ((uvmx<0)?(uvmx&1):-(uvmx&1));
    uvmy = uvmy + ((uvmy<0)?(uvmy&1):-(uvmy&1));
  }
  if(!dir) {
    srcY = dFRM->last_data[0];
    srcU = dFRM->last_data[1];
    srcV = dFRM->last_data[2];
  } else {
    srcY = dFRM->next_data[0];
    srcU = dFRM->next_data[1];
    srcV = dFRM->next_data[2];
  }  

  src_x = dMB_aux->mb_x * 16 + (mx >> 2);
  src_y = dMB_aux->mb_y * 16 + (my >> 2);
  uvsrc_x = dMB_aux->mb_x * 8 + (uvmx >> 2);
  uvsrc_y = dMB_aux->mb_y * 8 + (uvmy >> 2);

  if(dFRM->profile != PROFILE_ADVANCED){
    src_x   = av_clip(  src_x, -16, dFRM->mb_width  * 16);
    src_y   = av_clip(  src_y, -16, dFRM->mb_height * 16);
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->mb_width  *  8);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->mb_height *  8);
  }else{
    src_x   = av_clip(  src_x, -17, dFRM->coded_width);
    src_y   = av_clip(  src_y, -18, dFRM->coded_height + 1);
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->coded_width  >> 1);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->coded_height >> 1);
  }  
    
  srcY += src_y * dFRM->linesize + src_x;
  srcU += uvsrc_y * dFRM->uvlinesize + uvsrc_x;
  srcV += uvsrc_y * dFRM->uvlinesize + uvsrc_x;

  /* for grayscale we should not try to read from unknown area */
  if(dFRM->flags & CODEC_FLAG_GRAY) {
    srcU = TCSM1_EMU_EDGE_BUF + 19 * EMU_EDGE_LUMA_STRIDE;
    srcV = TCSM1_EMU_EDGE_BUF + 19 * EMU_EDGE_LUMA_STRIDE;   
  }
    
  if((dFRM->rangeredfrm || (dMB_aux->mv_mode == MV_PMODE_INTENSITY_COMP)
      || (unsigned)(src_x - dFRM->mspel) > dFRM->h_edge_pos - (mx&3) - 16 - dFRM->mspel*3
      || (unsigned)(src_y - dFRM->mspel) > dFRM->v_edge_pos - (my&3) - 16 - dFRM->mspel*3)){

    uint8_t *emu_edge_buf_y;
    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_y = emu_edge_buf_mb;
    emu_edge_buf_u = emu_edge_buf_y + EMU_BUF_1MV_Y_STRD;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;

    emu_edge_buf_y += dFRM->mspel * (1 + EMU_Y_W);
    int i;
    mc_des_dirty++;
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];

    if (dFRM->mspel) {  
      dxy = ((my & 3) << 2) | (mx & 3);
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK[MC_BLK_16X16];
      vc1_mc[blk_num].IntpInfo = VC1LumaIntp[dxy];
      vc1_mc[blk_num].Tap2Coef=TAPALN(VC1LumaTap2[dxy][0],VC1LumaTap2[dxy][1],VC1LumaTap2[dxy][2],VC1LumaTap2[dxy][3]);
      vc1_mc[blk_num].Tap1Coef=TAPALN(VC1LumaTap1[dxy][0],VC1LumaTap1[dxy][1],VC1LumaTap1[dxy][2],VC1LumaTap1[dxy][3]);   
    } else {
      dxy = (my & 2) | ((mx & 2) >> 1);
      vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[dFRM->rnd][dxy];
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_16X16];
    }

    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_y);
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   EMU_Y_W | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD;
    blk_num ++;  

    if(dFRM->flags & CODEC_FLAG_GRAY) return;
    uvmx = (uvmx&3)<<1;
    uvmy = (uvmy&3)<<1;
    for(i=0;i<2;i++)
      {
	mc_des_dirty++;  
	if(!dFRM->rnd){
	  vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (32<<8); /*always vc1 1/8-pixel interpolate*/	 
	}
	else {
	  vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (28<<8); /*always vc1 1/8-pixel interpolate*/
	} 

	if(!i){
	  vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_u);
	  vc1_mc[blk_num].CurrAddr = dest_cb;
	}
	else
	  {
	    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_v);
	    vc1_mc[blk_num].CurrAddr = dest_cr;
	  }
	vc1_mc[blk_num].BLKInfo = VC1ChromaBLK[0];
	vc1_mc[blk_num].Stride =  EMU_UV_W | ((RECON_BUF_STRD)<<16);
	vc1_mc[blk_num].Tap1Coef = TAPALN(8-(uvmx&7),uvmx&7,0,0);
	vc1_mc[blk_num].Tap2Coef = TAPALN(8-(uvmy&7),uvmy&7,0,0);
	vc1_mc[blk_num].NodeHead = VC1_YUHEAD;     
 
	blk_num ++;

      }  

  } else {

    int i;
    mc_des_dirty++;
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];

    if (dFRM->mspel) {  
      dxy = ((my & 3) << 2) | (mx & 3);
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK[MC_BLK_16X16];
      vc1_mc[blk_num].IntpInfo = VC1LumaIntp[dxy];
      vc1_mc[blk_num].Tap2Coef=TAPALN(VC1LumaTap2[dxy][0],VC1LumaTap2[dxy][1],VC1LumaTap2[dxy][2],VC1LumaTap2[dxy][3]);
      vc1_mc[blk_num].Tap1Coef=TAPALN(VC1LumaTap1[dxy][0],VC1LumaTap1[dxy][1],VC1LumaTap1[dxy][2],VC1LumaTap1[dxy][3]);   
    } else {
      dxy = (my & 2) | ((mx & 2) >> 1);
      vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[dFRM->rnd][dxy];
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_16X16];
    }

    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcY);
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   dFRM->linesize | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD;
    blk_num ++;
    if(dFRM->flags & CODEC_FLAG_GRAY) return;
    mc_mv_chroma_hw(uvmx,uvmy,srcU,srcV,dest_cb,dest_cr);                

  }
}

/** Do motion compensation for 4-MV macroblock - luminance block
 */
void vc1_mc_4mv_luma_hw(int n, uint8_t *dest_y, uint8_t *emu_edge_buf_mb)
{
  uint8_t *srcY;
  int dxy, mx, my, src_x, src_y;
  int off;                       

  if(!dFRM->last_data[0]) return;

  mx = dMB_aux->vc1_mv[n][0];
  my = dMB_aux->vc1_mv[n][1];

  srcY = dFRM->last_data[0];
    
  off = dFRM->linesize * 4 * (n&2) + (n&1) * 8;

  src_x = dMB_aux->mb_x * 16 + (n&1) * 8 + (mx >> 2);
  src_y = dMB_aux->mb_y * 16 + (n&2) * 4 + (my >> 2);

  if(dFRM->profile != PROFILE_ADVANCED){
    src_x   = av_clip(  src_x, -16, dFRM->mb_width  * 16);
    src_y   = av_clip(  src_y, -16, dFRM->mb_height * 16);
  }else{
    src_x   = av_clip(  src_x, -17, dFRM->coded_width);
    src_y   = av_clip(  src_y, -18, dFRM->coded_height + 1);
  }

  int i,j,va;
  srcY += src_y * dFRM->linesize + src_x;

  if(dFRM->rangeredfrm || (dMB_aux->mv_mode == MV_PMODE_INTENSITY_COMP)
     || (unsigned)(src_x - dFRM->mspel) > dFRM->h_edge_pos - (mx&3) - 8 - dFRM->mspel*2
     || (unsigned)(src_y - dFRM->mspel) > dFRM->v_edge_pos - (my&3) - 8 - dFRM->mspel*2){

    uint8_t *emu_edge_buf_y;
    emu_edge_buf_y = emu_edge_buf_mb;

    emu_edge_buf_y += dFRM->mspel * (1 + EMU_Y_4MV_W);
    mc_des_dirty++; 
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
    if(dFRM->mspel) {
      dxy = ((my & 3) << 2) | (mx & 3);
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK[MC_BLK_8X8];
      vc1_mc[blk_num].IntpInfo = VC1LumaIntp[dxy];  
      vc1_mc[blk_num].Tap2Coef=TAPALN(VC1LumaTap2[dxy][0],VC1LumaTap2[dxy][1],VC1LumaTap2[dxy][2],VC1LumaTap2[dxy][3]);
      vc1_mc[blk_num].Tap1Coef=TAPALN(VC1LumaTap1[dxy][0],VC1LumaTap1[dxy][1],VC1LumaTap1[dxy][2],VC1LumaTap1[dxy][3]);  
    }
    else   
      {
	dxy = (my & 2) | ((mx & 2) >> 1);
	vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[dFRM->rnd][dxy];
	vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_8X8];
      }
 
    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_y);
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   EMU_Y_4MV_W | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD; 

    blk_num ++;  	 	 	 

  } else {          
    mc_des_dirty++; 
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
    if(dFRM->mspel) {
      dxy = ((my & 3) << 2) | (mx & 3);
      vc1_mc[blk_num].BLKInfo =  VC1LumaBLK[MC_BLK_8X8];
      vc1_mc[blk_num].IntpInfo = VC1LumaIntp[dxy];  
      vc1_mc[blk_num].Tap2Coef=TAPALN(VC1LumaTap2[dxy][0],VC1LumaTap2[dxy][1],VC1LumaTap2[dxy][2],VC1LumaTap2[dxy][3]);
      vc1_mc[blk_num].Tap1Coef=TAPALN(VC1LumaTap1[dxy][0],VC1LumaTap1[dxy][1],VC1LumaTap1[dxy][2],VC1LumaTap1[dxy][3]);  
    }
    else   
      {
	dxy = (my & 2) | ((mx & 2) >> 1);
	vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[dFRM->rnd][dxy];
	vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_8X8];
      }
 
    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcY);
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   dFRM->linesize | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD; 

    blk_num ++;  	 	 	 
	 
  }   
    
}

#if 0
int median4_hw(int a, int b, int c, int d)
{
  if(a < b) {
    if(c < d) return (FFMIN(b, d) + FFMAX(a, c)) / 2;
    else      return (FFMIN(b, c) + FFMAX(a, d)) / 2;
  } else {
    if(c < d) return (FFMIN(a, d) + FFMAX(b, c)) / 2;            
    else      return (FFMIN(a, c) + FFMAX(b, d)) / 2;
  }
}         
#endif
     
/** Do motion compensation for 4-MV macroblock - both chroma blocks
 */
void vc1_mc_4mv_chroma_hw(uint8_t *emu_edge_buf_mb)
{
  uint8_t *srcU, *srcV;
  int uvdxy, uvmx, uvmy, uvsrc_x, uvsrc_y;
  int i,j, idx, tx = 0, ty = 0;
  int mvx[4], mvy[4], intra[4];
  static const int count[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
  uint8_t *dest_cb=  VC1_aux_t.recon[1];
  uint8_t *dest_cr=  VC1_aux_t.recon[2];

 
  if(!dFRM->last_data[0])return;
  if(dFRM->flags & CODEC_FLAG_GRAY) return;

  for(i = 0; i < 4; i++) {
    mvx[i] = dMB_aux->vc1_mv[i][0];
    mvy[i] = dMB_aux->vc1_mv[i][1];  
    intra[i] = dMB_aux->vc1_blk_index[i];
  }

  /* calculate chroma MV vector from four luma MVs */
  idx = (intra[3] << 3) | (intra[2] << 2) | (intra[1] << 1) | intra[0];
  if(!idx) { // all blocks are inter
    tx = median4_mxu(mvx[0], mvx[1], mvx[2], mvx[3]);
    ty = median4_mxu(mvy[0], mvy[1], mvy[2], mvy[3]);
  } else if(count[idx] == 1) { // 3 inter blocks
    switch(idx) {
    case 0x1:
      tx = mid_pred_mxu(mvx[1], mvx[2], mvx[3]);
      ty = mid_pred_mxu(mvy[1], mvy[2], mvy[3]);
      break;
    case 0x2:
      tx = mid_pred_mxu(mvx[0], mvx[2], mvx[3]);
      ty = mid_pred_mxu(mvy[0], mvy[2], mvy[3]);
      break;
    case 0x4:
      tx = mid_pred_mxu(mvx[0], mvx[1], mvx[3]);
      ty = mid_pred_mxu(mvy[0], mvy[1], mvy[3]);
      break;
    case 0x8:
      tx = mid_pred_mxu(mvx[0], mvx[1], mvx[2]);
      ty = mid_pred_mxu(mvy[0], mvy[1], mvy[2]);
      break;
    }
  } else if(count[idx] == 2) {
    int t1 = 0, t2 = 0;
    for(i=0; i<3;i++) if(!intra[i]) {t1 = i; break;}
    for(i= t1+1; i<4; i++)if(!intra[i]) {t2 = i; break;}
    tx = (mvx[t1] + mvx[t2]) / 2;
    ty = (mvy[t1] + mvy[t2]) / 2;
  } else {
    return; //no need to do MC for inter blocks
  }

  uvmx = (tx + ((tx&3) == 3)) >> 1;
  uvmy = (ty + ((ty&3) == 3)) >> 1;
  if(dFRM->fastuvmc) {
    uvmx = uvmx + ((uvmx<0)?(uvmx&1):-(uvmx&1));
    uvmy = uvmy + ((uvmy<0)?(uvmy&1):-(uvmy&1));
  }

  uvsrc_x = dMB_aux->mb_x * 8 + (uvmx >> 2);
  uvsrc_y = dMB_aux->mb_y * 8 + (uvmy >> 2);

  if(dFRM->profile != PROFILE_ADVANCED){
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->mb_width  *  8);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->mb_height *  8);
  }else{
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->coded_width  >> 1);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->coded_height >> 1);
  }

  srcU = dFRM->last_data[1] + uvsrc_y * dFRM->uvlinesize + uvsrc_x;
  srcV = dFRM->last_data[2] + uvsrc_y * dFRM->uvlinesize + uvsrc_x;
  if(dFRM->rangeredfrm || (dMB_aux->mv_mode == MV_PMODE_INTENSITY_COMP)
     || (unsigned)uvsrc_x > (dFRM->h_edge_pos >> 1) - 9
     || (unsigned)uvsrc_y > (dFRM->v_edge_pos >> 1) - 9){

    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_u = emu_edge_buf_mb;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;

      VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
  /* Chroma MC always uses qpel bilinear */
  uvmx = (uvmx&3)<<1;
  uvmy = (uvmy&3)<<1;
  for(i=0;i<2;i++)
    {
      mc_des_dirty++;  
      if(!dFRM->rnd){
	vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (32<<8); /*always vc1 1/8-pixel interpolate*/	 
      }
      else {
	vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (28<<8); /*always vc1 1/8-pixel interpolate*/
      } 

      if(!i){
	vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_u);
	vc1_mc[blk_num].CurrAddr = dest_cb;
      }
      else
	{
	  vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_v);
	  vc1_mc[blk_num].CurrAddr = dest_cr;
        }
      vc1_mc[blk_num].BLKInfo = VC1ChromaBLK[0];
      vc1_mc[blk_num].Stride =  EMU_UV_W| ((RECON_BUF_STRD)<<16);
      vc1_mc[blk_num].Tap1Coef = TAPALN(8-(uvmx&7),uvmx&7,0,0);
      vc1_mc[blk_num].Tap2Coef = TAPALN(8-(uvmy&7),uvmy&7,0,0);
      vc1_mc[blk_num].NodeHead = VC1_YUHEAD;     
 
      blk_num ++;
    }
  } else {
    mc_mv_chroma_hw(uvmx,uvmy,srcU,srcV,dest_cb,dest_cr);
  }

}

void  mc_mv_chroma_hw(int uvmx,int uvmy,uint8_t *srcU,uint8_t *srcV,uint8_t *dest_cb,uint8_t *dest_cr){

  int i,j;
   
  VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
  /* Chroma MC always uses qpel bilinear */
  uvmx = (uvmx&3)<<1;
  uvmy = (uvmy&3)<<1;
  for(i=0;i<2;i++)
    {
      mc_des_dirty++;  
      if(!dFRM->rnd){
	vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (32<<8); /*always vc1 1/8-pixel interpolate*/	 
      }
      else {
	vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (28<<8); /*always vc1 1/8-pixel interpolate*/
      } 

      if(!i){
	vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcU);
	vc1_mc[blk_num].CurrAddr = dest_cb;
      }
      else
	{
	  vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcV);
	  vc1_mc[blk_num].CurrAddr = dest_cr;
        }
      vc1_mc[blk_num].BLKInfo = VC1ChromaBLK[0];
      vc1_mc[blk_num].Stride =  dFRM->uvlinesize | ((RECON_BUF_STRD)<<16);
      vc1_mc[blk_num].Tap1Coef = TAPALN(8-(uvmx&7),uvmx&7,0,0);
      vc1_mc[blk_num].Tap2Coef = TAPALN(8-(uvmy&7),uvmy&7,0,0);
      vc1_mc[blk_num].NodeHead = VC1_YUHEAD;     
 
      blk_num ++;

    }       
}

//static void DirectB_MBAVG_MXU(uint8_t *MCReconBuf,uint8_t *MCUReconBuf,uint8_t *MCVReconBuf)
static void DirectB_MBAVG_MXU()
{  
  int i;
  uint8_t* src0 = VC1_aux_t.recon_xchg[0] - RECON_BUF_STRD;
  uint8_t* src1 = VC1_aux_t.recon_xchg_avg[0] - RECON_BUF_STRD;
  uint8_t* dst  = src0;  

  uint8_t* srcU = VC1_aux_t.recon_xchg[1] - RECON_BUF_STRD;
  uint8_t* srcU1 = VC1_aux_t.recon_xchg_avg[1] - RECON_BUF_STRD;
  uint8_t* dstU  = srcU;  

  uint8_t* srcV = VC1_aux_t.recon_xchg[2] - RECON_BUF_STRD;
  uint8_t* srcV1 = VC1_aux_t.recon_xchg_avg[2] - RECON_BUF_STRD;
  uint8_t* dstV = srcV;  

  /*Luma*/  
  for (i = 0; i < 16; i++) {
    S32LDI(xr1, src0, RECON_BUF_STRD);
    S32LDI(xr2, src1, RECON_BUF_STRD);
      
    S32LDD(xr3, src0, 4);
    S32LDD(xr4, src1, 4);

    Q8AVGR(xr13, xr1, xr2);
    S32SDI(xr13, dst, RECON_BUF_STRD);

    Q8AVGR(xr13, xr3, xr4);
    S32STD(xr13, dst, 4);

    S32LDD(xr5, src0, 8);
    S32LDD(xr6, src1, 8);

    S32LDD(xr7, src0, 12);
    S32LDD(xr8, src1, 12);

    Q8AVGR(xr13, xr5, xr6);
    S32STD(xr13, dst, 8);

    Q8AVGR(xr13, xr7, xr8);
    S32STD(xr13, dst, 12);
  }
  for (i = 0; i < 8; i++) {
    S32LDI(xr1, srcU, RECON_BUF_STRD);
    S32LDI(xr2, srcU1, RECON_BUF_STRD);
    S32LDD(xr3, srcU, 4);
    S32LDD(xr4, srcU1, 4);

    Q8AVGR(xr13, xr1, xr2);
    S32SDI(xr13, dstU, RECON_BUF_STRD);

    Q8AVGR(xr13, xr3, xr4);
    S32STD(xr13, dstU, 4);

    S32LDI(xr1, srcV, RECON_BUF_STRD);
    S32LDI(xr2, srcV1, RECON_BUF_STRD);
    S32LDD(xr3, srcV, 4);
    S32LDD(xr4, srcV1, 4);

    Q8AVGR(xr13, xr1, xr2);
    S32SDI(xr13, dstV, RECON_BUF_STRD);

    Q8AVGR(xr13, xr3, xr4);
    S32STD(xr13, dstV, 4);   
  }  
}

static void vc1_interp_mc(uint8_t *emu_edge_buf_mb)
{
  uint8_t *srcY, *srcU, *srcV;
  int dxy, uvdxy, mx, my, uvmx, uvmy, src_x, src_y, uvsrc_x, uvsrc_y;

  uint8_t *dest_y =  VC1_aux_t.recon_avg[0];
  uint8_t *dest_cb=  VC1_aux_t.recon_avg[1];
  uint8_t *dest_cr=  VC1_aux_t.recon_avg[2];
 
  if(!dFRM->next_data[0])return;
  mx = dMB_aux->vc1_mv[1][0];
  my = dMB_aux->vc1_mv[1][1];

  uvmx = (mx + ((mx & 3) == 3)) >> 1;
  uvmy = (my + ((my & 3) == 3)) >> 1;
  if(dFRM->fastuvmc) {
    uvmx = uvmx + ((uvmx<0)?-(uvmx&1):(uvmx&1));
    uvmy = uvmy + ((uvmy<0)?-(uvmy&1):(uvmy&1));
  }

  srcY = dFRM->next_data[0];
  srcU = dFRM->next_data[1];
  srcV = dFRM->next_data[2];

  src_x = dMB_aux->mb_x * 16 + (mx >> 2);
  src_y = dMB_aux->mb_y * 16 + (my >> 2);
  uvsrc_x = dMB_aux->mb_x * 8 + (uvmx >> 2);
  uvsrc_y = dMB_aux->mb_y * 8 + (uvmy >> 2);

  if(dFRM->profile != PROFILE_ADVANCED){
    src_x   = av_clip(  src_x, -16, dFRM->mb_width  * 16);
    src_y   = av_clip(  src_y, -16, dFRM->mb_height * 16);
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->mb_width  *  8);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->mb_height *  8);
  }else{
    src_x   = av_clip(  src_x, -17, dFRM->coded_width);
    src_y   = av_clip(  src_y, -18, dFRM->coded_height + 1);
    uvsrc_x = av_clip(uvsrc_x,  -8, dFRM->coded_width  >> 1);
    uvsrc_y = av_clip(uvsrc_y,  -8, dFRM->coded_height >> 1);
  }
   
  srcY += src_y * dFRM->linesize + src_x;
  srcU += uvsrc_y * dFRM->uvlinesize + uvsrc_x;
  srcV += uvsrc_y * dFRM->uvlinesize + uvsrc_x;

  /* for grayscale we should not try to read from unknown area */
  if(dFRM->flags & CODEC_FLAG_GRAY) {
    srcU = TCSM1_EMU_EDGE_BUF + 19 * EMU_EDGE_LUMA_STRIDE;
    srcV = TCSM1_EMU_EDGE_BUF + 19 * EMU_EDGE_LUMA_STRIDE;
  }
  
  if(dFRM->rangeredfrm
     || (unsigned)src_x > dFRM->h_edge_pos - (mx&3) - 16
     || (unsigned)src_y > dFRM->v_edge_pos - (my&3) - 16){

    uint8_t *emu_edge_buf_y;
    uint8_t *emu_edge_buf_u;
    uint8_t *emu_edge_buf_v;
    emu_edge_buf_y = emu_edge_buf_mb;
    emu_edge_buf_u = emu_edge_buf_y + EMU_BUF_1MV_Y_STRD;
    emu_edge_buf_v = emu_edge_buf_u + EMU_BUF_UV_STRD;

    emu_edge_buf_y += dFRM->mspel * (1 + EMU_Y_W);
    mx >>= 1;      
    my >>= 1;
    dxy = ((my & 1) << 1) | (mx & 1);
    int i;   
    mc_des_dirty++;
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
    vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[0][dxy];
    vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_16X16];
    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_y);
     
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   EMU_Y_W | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD;
    blk_num ++;

    if(dFRM->flags & CODEC_FLAG_GRAY) return;
    /* Chroma MC always uses qpel blilinear */
    uvdxy = ((uvmy & 3) << 2) | (uvmx & 3);
    uvmx = (uvmx&3)<<1;
    uvmy = (uvmy&3)<<1;

    for(i=0;i<2;i++) {
      mc_des_dirty++;      
      vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (32<<8); /*always vc1 1/8-pixel interpolate*/	 
     
      if(!i){
	vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_u);
	vc1_mc[blk_num].CurrAddr = dest_cb;
      }
      else
	{
	  vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux((unsigned int)emu_edge_buf_v);
	  vc1_mc[blk_num].CurrAddr = dest_cr;
        }  
      vc1_mc[blk_num].BLKInfo = VC1ChromaBLK[0];
      vc1_mc[blk_num].Stride =  EMU_UV_W | ((RECON_BUF_STRD)<<16);
      vc1_mc[blk_num].Tap1Coef = TAPALN(8-(uvmx&7),uvmx&7,0,0);
      vc1_mc[blk_num].Tap2Coef = TAPALN(8-(uvmy&7),uvmy&7,0,0);
      vc1_mc[blk_num].NodeHead = VC1_YUHEAD;     
 
      blk_num ++;
    }
  } else {
    mx >>= 1;      
    my >>= 1;
    dxy = ((my & 1) << 1) | (mx & 1);
    int i;   
    mc_des_dirty++;
    VC1_MC_DesNode *vc1_mc = (VC1_MC_DesNode *)VC1_aux_t.mc_chain[0];
    vc1_mc[blk_num].IntpInfo = MPEG2LumaIntp[0][dxy];
    vc1_mc[blk_num].BLKInfo =  VC1LumaBLK1[MC_BLK_16X16];
    vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcY);
     
    vc1_mc[blk_num].CurrAddr = dest_y;  
    vc1_mc[blk_num].Stride =   dFRM->linesize | ((RECON_BUF_STRD)<<16);
    vc1_mc[blk_num].NodeHead = VC1_YUHEAD;
    blk_num ++;

    if(dFRM->flags & CODEC_FLAG_GRAY) return;
    /* Chroma MC always uses qpel blilinear */
    uvdxy = ((uvmy & 3) << 2) | (uvmx & 3);
    uvmx = (uvmx&3)<<1;
    uvmy = (uvmy&3)<<1;

    for(i=0;i<2;i++) {
      mc_des_dirty++;      
      vc1_mc[blk_num].IntpInfo = (H1V1<<16) | (32<<8); /*always vc1 1/8-pixel interpolate*/	 
     
      if(!i){
	vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcU);
	vc1_mc[blk_num].CurrAddr = dest_cb;
      }
      else
	{
	  vc1_mc[blk_num].RefAddr = (int)get_phy_addr_aux(srcV);
	  vc1_mc[blk_num].CurrAddr = dest_cr;
        }  
      vc1_mc[blk_num].BLKInfo = VC1ChromaBLK[0];
      vc1_mc[blk_num].Stride =  dFRM->uvlinesize | ((RECON_BUF_STRD)<<16);
      vc1_mc[blk_num].Tap1Coef = TAPALN(8-(uvmx&7),uvmx&7,0,0);
      vc1_mc[blk_num].Tap2Coef = TAPALN(8-(uvmy&7),uvmy&7,0,0);
      vc1_mc[blk_num].NodeHead = VC1_YUHEAD;     
 
      blk_num ++;
    }
  }
}
