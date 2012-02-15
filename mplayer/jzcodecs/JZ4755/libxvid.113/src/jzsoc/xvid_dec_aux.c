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
 * $Id: xvid_dec_aux.c,v 1.30 2009/07/07 09:39:06 gysun Exp $
 *
 ****************************************************************************/
#define AUX_VADDR_SPACE
#include "xvid_arch.h"
#include "xvid_mc_hw.h"
#include "xvid_dcore.h"

#include "jzmedia.h"
#include "mpeg4_mdma_hw.h"
#include "tcsm.h"
#include "jzasm.h"

#include "xvid_idct.c"
#include "xvid_mc_hw.c"

#define BS_VERSION_BUGGY_CHROMA_ROUNDING 1
#define DIV2ROUND(n)  (((n)>>1)|((n)&1))

#define XCHG2(a,b,t)     {(t)=(a);(a)=(b);(b)=(t);}
#define XCHG3(a,b,c,t)     {(t)=(a);(a)=(b);(b)=(c);(c)=(t);}

/* K = 4 */
const uint32_t roundtab_76[16] =
{ 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };
/* K = 1 */
const uint32_t roundtab_79[4] =
{ 0, 1, 0, 0 };

#ifdef JZC_MDMA1_OPT
R4_XCHG r4xchg[3] = {
  {TCSM1_RECON_YBUF0, TCSM1_RECON_UBUF0},
  {TCSM1_RECON_YBUF1, TCSM1_RECON_UBUF1},
  {TCSM1_RECON_YBUF2, TCSM1_RECON_UBUF2}
};
#else
R4_XCHG r4xchg[2] = {
  {TCSM1_RECON_YBUF0, TCSM1_RECON_UBUF0},
  {TCSM1_RECON_YBUF1, TCSM1_RECON_UBUF1},
};
#endif

R2_XCHG r2xchg[2] = {
  {
    TCSM1_RECON_BYBUF0,
    TCSM1_RECON_BUBUF0,
    TCSM1_IDCT_BUF0,
    TCSM1_MC_DES_BUF0,
  },
  {
    TCSM1_RECON_BYBUF1,
    TCSM1_RECON_BUBUF1,
    TCSM1_IDCT_BUF1,
    TCSM1_MC_DES_BUF1,
  },
};

uint8_t *CurrYbuf, *CurrUbuf;

#ifdef JZC_MDMA1_OPT
R4_XCHG *r4ptr[3];
#else
R4_XCHG *r4ptr[2];
#endif

R2_XCHG *r2ptr[2];

extern int _gp;
volatile int *fifo_wp, *task_done;

#ifdef JZC_AUX_MC_SYNC
volatile uint32_t * mcOver1;
volatile uint32_t * mcOver2;
#endif

#ifdef JZC_AUX_GP1_SYNC
volatile uint32_t * gp1over1;
volatile uint32_t * gp1over2;
#endif

static __inline uint32_t
get_dc_scaler(uint32_t quant,
	      uint32_t lum)
{
  if (quant < 5)
    return 8;

  if (quant < 25 && !lum)
    return (quant + 13) / 2;
  
  if (quant < 9)
    return 2 * quant;
  
  if (quant < 25)
    return quant + 8;
  
  if (lum)
    return 2 * quant - 16;
  else
    return quant - 6;
}

__tcsm1_main int main(){

  int c, i;
  c = i_la(_gp);
  S32I2M(xr16,3);
  uint16_t x=0,y=0;
  uint32_t XCHGtmp = 0;

  fifo_wp = (int *)TCSM1_FIFO_WP;
  task_done = (int *)TCSM1_TASK_DONE;
 
  FIFO_DECODER *dec = (FIFO_DECODER *)TCSM1_DFRM_ADDR;
  FIFO_MB *dMB[3];

  M4_MDMA_DesNode *MDMA1_ARG;
  int32_t fifo_rp = 0;
  uint32_t *fifo_start = (uint32_t *)TCSM1_FIFO_START; 
  uint32_t fifo_pstr = *fifo_start;
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  uint8_t last_cbp = 0;
  uint8_t last_avg = 0;
  uint8_t curr_avg = 0;
  uint8_t *lastYbuf = TCSM1_RECON_YBUF0;
  uint8_t *lastUbuf = TCSM1_RECON_UBUF0;
  uint8_t mc_dirty;
  
  uint32_t mbnum = 0; //total MB number to dec
  
  #ifdef JZC_MDMA1_OPT
  uint8_t* declst4y   = dec->cur[0];
  uint8_t* declst4u   = dec->cur[1];
  uint8_t* declst4v   = dec->cur[2];
  uint8_t trans       = 1;
  uint8_t cnt4        = 0; 
  uint8_t idx4        = 0; 
  uint8_t trans_width = 64;               
  uint16_t mb_width  = dec->mb_width;
  uint16_t mb_height = dec->mb_height;
  uint32_t nfor      = (mb_height * (mb_width + ((4 - (mb_width%4))%4)));
  M4_MDMA_DesNode *MDMA1_TRANS = (M4_MDMA_DesNode *)TCSM1_GP1_TRAN_CHAIN;
  #else
  uint32_t  mbcnt4 = 0;
  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
  uint8_t *last_pY = dec->cur[0];
  uint8_t *last_pU = dec->cur[1];
  uint8_t *last_pV = dec->cur[2];
  #endif
  
  MDMA1_ARG = (M4_MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
  SET_MDMA1_DHA((uint32_t)MDMA1_ARG);

  /*tcsm recon buffer pointer init*/
  #ifdef JZC_MDMA1_OPT
  r4ptr[0] = (R4_XCHG *)&r4xchg[0];
  r4ptr[1] = (R4_XCHG *)&r4xchg[1];
  r4ptr[2] = (R4_XCHG *)&r4xchg[2];
  #else
  r4ptr[0] = (R4_XCHG *)&r4xchg[0];
  r4ptr[1] = (R4_XCHG *)&r4xchg[1];
  #endif
  r2ptr[0] = (R2_XCHG *)&r2xchg[0];
  r2ptr[1] = (R2_XCHG *)&r2xchg[1];

  CurrYbuf = (uint8_t *)r4ptr[0]->CurrReconY;
  CurrUbuf = (uint8_t *)r4ptr[0]->CurrReconU;

  dMB[0] = (FIFO_MB *)TCSM1_MBARGS_BUF0;
  dMB[1] = (FIFO_MB *)TCSM1_MBARGS_BUF1;
  dMB[2] = (FIFO_MB *)TCSM1_MBARGS_BUF2;

  #ifdef JZC_AUX_MC_SYNC
  uint32_t *mcsyncnd;
  mcOver1 = (uint32_t)TCSM1_MC_SYNCFLAG1_ADDR; 
  mcOver2 = (uint32_t)TCSM1_MC_SYNCFLAG2_ADDR;
  *mcOver1 = 1;
  *mcOver2 = 0;
  #endif

  #ifdef JZC_AUX_GP1_SYNC
  gp1over1 = (uint32_t)TCSM1_GP1_SYNCFLAG1_ADDR;
  gp1over2 = (uint32_t)TCSM1_GP1_SYNCFLAG2_ADDR;
  *gp1over1 = 1;
  *gp1over2 = 1;
  #endif
  
  //============================================
  // before enter into aux MB dec loop, MDMA1 fetch 2 task so that its 3-rolling buffer
  // has no data-reliance during MB-dec loop
  /* 
     the 1st task fetch for J1<->AUX pipe needs
     AXU fetch the 1st task, then starts all the other MB tasks in current frame
   */
  /*AUX<->J1 task sync!*/
  do{
  }while(fifo_rp >= *fifo_wp);

  fifo_rp++;

  do{
  }while(fifo_rp >= *fifo_wp);
  
  MDMA1_ARG = (M4_MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
  MDMA1_ARG->TSA = get_phy_addr_aux(fifo_pstr);
  MDMA1_ARG->TDA = (uint32_t)dMB[0];
  MDMA1_ARG->STRD = MDMA_STRD(64, 64);

  #ifdef JZC_AUX_GP1_SYNC
  MDMA1_ARG->UNIT = MDMA_UNIT(0,64,sizeof(FIFO_MB));
  MDMA1_ARG ++;
  MDMA1_ARG->TSA  = (uint32_t)0x13080000;//TCSM0_IDLE_STATUS;
  MDMA1_ARG->TDA  = (uint32_t)gp1over1;
  MDMA1_ARG->STRD = MDMA_STRD(4, 4);
  MDMA1_ARG->UNIT = MDMA_UNIT(1, 4, 4); 
  #else
  MDMA1_ARG->UNIT = MDMA_UNIT(1,64,sizeof(FIFO_MB));
  #endif
  MDMA1_RUN();
  fifo_pstr += sizeof(FIFO_MB);	    

  /* 
     the 2nd task fetch for J1<->AUX pipe needs
     AXU fetch the 1st task, then starts all the other MB tasks in current frame
   */
  /*AUX<->J1 task sync!*/
  fifo_rp++;

  do{
  }while(fifo_rp >= *fifo_wp);

  // yli
  #ifdef JZC_AUX_GP1_SYNC
  do{}
  while(*gp1over1 != 0);
  *gp1over1 = 1;
  MDMA1_ARG = (M4_MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
  #else
  POLLING_MDMA1_END();
  #endif
 
  MDMA1_ARG->TSA = get_phy_addr_aux(fifo_pstr);
  MDMA1_ARG->TDA = (uint32_t)dMB[1];
  MDMA1_ARG->STRD = MDMA_STRD(64, 64);
  #ifdef JZC_AUX_GP1_SYNC
  MDMA1_ARG->UNIT = MDMA_UNIT(0,64,sizeof(FIFO_MB));
  MDMA1_ARG ++;
  MDMA1_ARG->TSA  = (uint32_t)0x13080000;//TCSM0_IDLE_STATUS;
  MDMA1_ARG->TDA  = (uint32_t)gp1over1;
  MDMA1_ARG->STRD = MDMA_STRD(4, 4);
  MDMA1_ARG->UNIT = MDMA_UNIT(1, 4, 4); 
  #else
  MDMA1_ARG->UNIT = MDMA_UNIT(1,64,sizeof(FIFO_MB));
  #endif
  MDMA1_RUN();
  fifo_pstr += sizeof(FIFO_MB);	    

  #ifdef JZC_MDMA1_OPT
  while(mbnum < nfor + 1) 
  #else
  while(mbnum < dec->mb_height*dec->mb_width + 1) 
  #endif
  {
    
    mc_dirty = 0;
    curr_avg = 0;
    mc = r2ptr[0]->MCDes;

    if(dMB[0]->isfake){
      /*do nothing*/
    } else {
      if(dec->coding_type == 0){          /* I_VOP  */
	decoder_mbintra_2pass(dec,x,y,dMB[0]);
      }else if(dec->coding_type == 1){   /* P_VOP  */
	if((dMB[0]->mode==MODE_INTRA)||(dMB[0]->mode == MODE_INTRA_Q)){
	  decoder_mbintra_2pass(dec,x,y,dMB[0]);
	}
#if 0
	else if(dMB[0]->field_pred){
	  decoder_mbinter_field_2pass(dec,x,y,dMB[0]);
	  mc_dirty = 1;
	}
#endif
	else {
	  decoder_mbinter_2pass(dec,x,y,dMB[0]);
	  mc_dirty = 1;
	}
      }else {                            /* B_VOP  */
	switch (dMB[0]->mode) {
	case MODE_DIRECT:
	case MODE_DIRECT_NONE_MV:
	case MODE_INTERPOLATE:
	  decoder_bf_interpolate_mbinter_2pass(dec,x,y,dMB[0]);
	  mc_dirty = 1;
	  curr_avg = 1;
	  break;
	case MODE_BACKWARD:
	case MODE_FORWARD:
	  decoder_mbinter_2pass(dec,x,y, dMB[0]); 
	  mc_dirty = 1;
	  break;
	default:
	  break;
	}
      }
    }

    #ifdef JZC_AUX_MC_SYNC
    do{
    }while((*mcOver1) != 1);
    *mcOver1 = 0;
    #else
    MC_POLLING_END();
    #endif
    if(mc_dirty)
    {
      #ifdef JZC_AUX_MC_SYNC

      mc[-1].NodeHead = DESP_NODE_HEAD_SYNC; //0x20080048;
      mcsyncnd        = (uint32_t *)mc;
      mcsyncnd[0]     = DESP_NODE_HEAD_FG;   //0x80000000;  // node header flag
      mcsyncnd[1]     = DESP_NODE_HEAD_END;  //0x0001006c;  // node type :address 
      mcsyncnd[2]     = (uint32_t *)mcOver2;                // address
      mcsyncnd[3]     = (uint32_t)0x1;                      // data
      #else
      mc[-1].NodeHead = DESP_NODE_HEAD_UK;
      #endif
      SET_MC_DHA((uint32_t)r2ptr[0]->MCDes);        
      SET_MC_DCS();                         
    }
    if(last_avg)
      MXU_DirectB_MBAVG(lastYbuf, lastUbuf, r2ptr[1]->ReconYBi, r2ptr[1]->ReconUBi);
    
    if(last_cbp){
      int16_t *residual;
      residual = r2ptr[1]->IDCTBuf;
      for (i = 0; i < 4; i++) {
	/* Process only coded blocks */
	if (last_cbp & (1 << (5 - i))) {
	  transfer_16to8add_mxu(lastYbuf+Blk4VOffset[i],residual,RECON_BUF_STRIDE);
	  residual += 64;
	}
      }
      if (last_cbp & (1 << (5 - 4))) {
	transfer_16to8add_mxu(lastUbuf,residual,RECON_BUF_STRIDE);
	residual += 64;
      }
      if (last_cbp & (1 << (5 - 5))) {
	transfer_16to8add_mxu(lastUbuf+32,residual,RECON_BUF_STRIDE);
      }
    }

    last_cbp = dMB[0]->cbp;
    if(!mc_dirty)
    {
      last_cbp = 0;
      #ifdef JZC_AUX_MC_SYNC
      *mcOver2 = 1;
      #endif
    }
    else
    if(dMB[0]->cbp)
    {
      decoder_mb_decode_2pass(dec, dMB[0]->cbp,x,y, dMB[0]);
    }
    
    /*AUX<->J1 task sync!*/
    fifo_rp++;
    do{
    }while(fifo_rp >= *fifo_wp);

    
#ifdef JZC_AUX_GP1_SYNC
    do
    {
    }while(*gp1over1 != 0);
    *gp1over1 = 1;

#else
    POLLING_MDMA1_END(); 
#endif
    #ifdef JZC_MDMA1_OPT
    MDMA1_ARG       = (M4_MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
    MDMA1_ARG->TSA  = get_phy_addr_aux(fifo_pstr);
    MDMA1_ARG->TDA  = (uint32_t)dMB[2];
    MDMA1_ARG->STRD = MDMA_STRD(64, 64);
    #ifdef JZC_AUX_GP1_SYNC
    MDMA1_ARG->UNIT = MDMA_UNIT((0), 64, dMB[1]->next_dmb_len);
    #else
    MDMA1_ARG->UNIT = MDMA_UNIT((trans), 64, dMB[1]->next_dmb_len);
    #endif
    MDMA1_ARG ++;

    MDMA1_ARG->TSA  = (uint32_t)(r4ptr[2]->CurrReconY + RECON_BUF_STRIDE * 4 * cnt4);
    MDMA1_ARG->TDA  = get_phy_addr_aux(declst4y + stride * 4 * cnt4);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride);
    MDMA1_ARG->UNIT = MDMA_UNIT(0, 64, 64*4);  
    MDMA1_ARG ++;
    
    MDMA1_ARG->TSA  = (uint32_t)(r4ptr[2]->CurrReconU + RECON_BUF_STRIDE * 2 * cnt4);
    MDMA1_ARG->TDA  = get_phy_addr_aux(declst4u + stride2 * 2 * cnt4);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    MDMA1_ARG->UNIT = MDMA_UNIT(0, 32, 64); 
    MDMA1_ARG ++;
    
    MDMA1_ARG->TSA  = (uint32_t)(((r4ptr[2]->CurrReconU) + 32) + RECON_BUF_STRIDE * 2 * cnt4);
    MDMA1_ARG->TDA  = get_phy_addr_aux(declst4v + stride2 * 2 * cnt4);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    #ifdef JZC_AUX_GP1_SYNC
    MDMA1_ARG->UNIT = MDMA_UNIT(0, 32, 64);
    MDMA1_ARG ++;
    if(trans == 1)  MDMA1_ARG -= 3;
    MDMA1_ARG->TSA  = (uint32_t)0x13080000; //TCSM0_IDLE_STATUS;
    MDMA1_ARG->TDA  = (uint32_t)gp1over2;
    MDMA1_ARG->STRD = MDMA_STRD(4, 4);
    MDMA1_ARG->UNIT = MDMA_UNIT(1, 4, 4);
    #else
    MDMA1_ARG->UNIT = MDMA_UNIT(1, 32, 64);
    #endif
    MDMA1_RUN();

    fifo_pstr = fifo_pstr + sizeof(FIFO_MB); 
    
    mbnum ++; 
    x     ++; 
    cnt4  ++; 
    last_avg = curr_avg; 
    #ifdef JZC_AUX_MC_SYNC
    XCHG2(mcOver1, mcOver2, XCHGtmp); // yli mc end
    #endif
    
    #ifdef JZC_AUX_GP1_SYNC
    XCHG2(gp1over1, gp1over2, XCHGtmp); // yli gp1 end
    #endif

    XCHG2(r2ptr[0], r2ptr[1],   XCHGtmp);    
    XCHG3(dMB[0],dMB[1],dMB[2], XCHGtmp);    
    lastYbuf = CurrYbuf; 
    lastUbuf = CurrUbuf; 

    CurrYbuf += 16; 
    CurrUbuf += 8;  
    
    if(cnt4 == 4) 
    {
      cnt4        = 0;    
      trans       = 0; 
      
      declst4y = dec->cur[0] + (y << 4) * stride  + ((idx4*4) << 4);
      declst4u = dec->cur[1] + (y << 3) * stride2 + ((idx4*4) << 3);
      declst4v = dec->cur[2] + (y << 3) * stride2 + ((idx4*4) << 3);
      

      XCHG3(r4ptr[0],r4ptr[1],r4ptr[2], XCHGtmp); 
      CurrYbuf = (uint8_t *)r4ptr[0]->CurrReconY; 
      CurrUbuf = (uint8_t *)r4ptr[0]->CurrReconU; 

      idx4 ++ ;        
      if(x == (mb_width + ((4 - mb_width%4)%4)))
      {
        x = 0;
	y ++;
        idx4 = 0;
      }
    }

    #else
    MDMA1_ARG = (M4_MDMA_DesNode *)TCSM1_GP1_ARG_CHAIN;
    MDMA1_ARG->TSA = get_phy_addr_aux(fifo_pstr);
    MDMA1_ARG->TDA = (uint32_t)dMB[2];
    MDMA1_ARG->STRD = MDMA_STRD(64, 64);
    MDMA1_ARG->UNIT = MDMA_UNIT(0,64,dMB[1]->next_dmb_len);
    MDMA1_ARG++;
    /*  y  */
    MDMA1_ARG->TSA = (uint32_t)lastYbuf;
    MDMA1_ARG->TDA = get_phy_addr_aux(last_pY);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride);
    MDMA1_ARG->UNIT = MDMA_UNIT(0,16,64*4);
    MDMA1_ARG++;
    /* u */
    MDMA1_ARG->TSA = (uint32_t)lastUbuf;
    MDMA1_ARG->TDA = get_phy_addr_aux(last_pU);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    MDMA1_ARG->UNIT = MDMA_UNIT(0,8,64);
    MDMA1_ARG++;
    /* v  */
    MDMA1_ARG->TSA = (uint32_t)lastUbuf + 32;
    MDMA1_ARG->TDA = get_phy_addr_aux(last_pV);
    MDMA1_ARG->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    #ifdef JZC_AUX_GP1_SYNC
    MDMA1_ARG->UNIT = MDMA_UNIT(0,8,64);
    MDMA1_ARG ++;

    MDMA1_ARG->TSA  = (uint32_t)0x13080000;//TCSM0_IDLE_STATUS;
    MDMA1_ARG->TDA  = (uint32_t)gp1over2;
    MDMA1_ARG->STRD = MDMA_STRD(4, 4);
    MDMA1_ARG->UNIT = MDMA_UNIT(1, 4, 4);    
    #else
    MDMA1_ARG->UNIT = MDMA_UNIT(1,8,64);
    #endif
    MDMA1_RUN();
    
    last_pY = dec->cur[0] + (y << 4) * stride + (x << 4);
    last_pU = dec->cur[1] + (y << 3) * stride2 + (x << 3);
    last_pV = dec->cur[2] + (y << 3) * stride2 + (x << 3);
    
    lastYbuf = CurrYbuf;
    lastUbuf = CurrUbuf;
    last_avg = curr_avg;
    fifo_pstr=fifo_pstr+sizeof(FIFO_MB);	      

    /*XCHG buffer pointer*/
    #ifdef JZC_AUX_MC_SYNC
    XCHG2(mcOver1, mcOver2, XCHGtmp); // yli mc end
    #endif
    
    #ifdef JZC_AUX_GP1_SYNC
    XCHG2(gp1over1, gp1over2, XCHGtmp); // yli gp1 end
    #endif

    XCHG2(r2ptr[0],r2ptr[1],XCHGtmp);
    XCHG3(dMB[0],dMB[1],dMB[2],XCHGtmp);

    mbnum++;
    x ++;
    if(x == dec->mb_width){
      /*go to one-line's ending point*/
      x = 0;
      y ++;

      XCHG2(r4ptr[0],r4ptr[1],XCHGtmp);
      CurrYbuf = (uint8_t *)r4ptr[0]->CurrReconY;
      CurrUbuf = (uint8_t *)r4ptr[0]->CurrReconU;
      mbcnt4 = 0;
    } else {
      mbcnt4 ++;
      CurrYbuf += 16;
      CurrUbuf += 8;
      if(mbcnt4 == 4){
	mbcnt4 = 0;
	XCHG2(r4ptr[0],r4ptr[1],XCHGtmp);
	CurrYbuf = (uint8_t *)r4ptr[0]->CurrReconY;
	CurrUbuf = (uint8_t *)r4ptr[0]->CurrReconU;
      }
    }
  #endif
  } /*while num*/
  #ifdef JZC_MDMA1_OPT
  {
    POLLING_MDMA1_END(); 
    SET_MDMA1_DHA((uint32_t)TCSM1_GP1_TRAN_CHAIN);
    MDMA1_TRANS = (M4_MDMA_DesNode *)TCSM1_GP1_TRAN_CHAIN;    

    MDMA1_TRANS->TSA = (uint32_t)(r4ptr[2]->CurrReconY);
    MDMA1_TRANS->TDA = get_phy_addr_aux(declst4y);
    MDMA1_TRANS->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride);
    MDMA1_TRANS->UNIT = MDMA_UNIT(0, 64, 64*16);
    MDMA1_TRANS++;
    
    MDMA1_TRANS->TSA = (uint32_t)(r4ptr[2]->CurrReconU);
    MDMA1_TRANS->TDA = get_phy_addr_aux(declst4u);
    MDMA1_TRANS->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    MDMA1_TRANS->UNIT = MDMA_UNIT(0, 32, 32*8);
    MDMA1_TRANS++;
    
    MDMA1_TRANS->TSA = (uint32_t)(r4ptr[2]->CurrReconU) + 32;
    MDMA1_TRANS->TDA = get_phy_addr_aux(declst4v);
    MDMA1_TRANS->STRD = MDMA_STRD(RECON_BUF_STRIDE, stride2);
    MDMA1_TRANS->UNIT = MDMA_UNIT(1, 32, 32*8);
    MDMA1_RUN();
  }
  #endif

  POLLING_MDMA1_END(); 
  *task_done = 1;
  i_nop;
  i_nop;
  __asm__ __volatile__ ("wait");
}

void
decoder_mbintra_2pass(FIFO_DECODER const *dec,
		      const uint16_t x_pos,
		      const uint16_t y_pos,
		      FIFO_MB const *pMB
		      )
{
  uint32_t i;
  const uint32_t iQuant = pMB->quant;
  int16_t *residual = pMB->residual;
  uint16_t *quant_matrix = (uint16_t *)TCSM1_QUANT_MATRIX;  
  int16_t * data = pMB->residual;
  int16_t * error = r2ptr[0]->IDCTBuf;

  for (i=0; i<4; i++) {
    	    uint32_t iDcScaler = get_dc_scaler(iQuant, 1);
	    if (dec->quant_type == 0) {
	      dequant_h263_intra(data, pMB->yuv_len[i],iQuant, iDcScaler, quant_matrix);
	    } else {
	      dequant_mpeg_intra(data, pMB->yuv_len[i],iQuant, iDcScaler, quant_matrix);
	    }
	    idct(data, error,pMB->yuv_len[i]);
	    transfer_16to8copy_mxu(CurrYbuf+Blk4VOffset[i],error,RECON_BUF_STRIDE);
	    data += pMB->yuv_len[i]*8;
	  
  }

  {
	    uint32_t iDcScaler = get_dc_scaler(iQuant, 0);
	    if (dec->quant_type == 0) {
	      dequant_h263_intra(data,pMB->yuv_len[4], iQuant, iDcScaler, quant_matrix);
	    } else {
	      dequant_mpeg_intra(data, pMB->yuv_len[4],iQuant, iDcScaler, quant_matrix);
	    }
	    idct(data, error,pMB->yuv_len[4]);
	    transfer_16to8copy_mxu(CurrUbuf,error,RECON_BUF_STRIDE);
	    data += pMB->yuv_len[4]*8;
  }

  {
	    uint32_t iDcScaler = get_dc_scaler(iQuant, 0);
	    if (dec->quant_type == 0) {
	      dequant_h263_intra(data,pMB->yuv_len[5], iQuant, iDcScaler, quant_matrix);
	    } else {
	      dequant_mpeg_intra(data, pMB->yuv_len[5],iQuant, iDcScaler, quant_matrix);
	    }
	    idct(data, error,pMB->yuv_len[5]);
	    
	    transfer_16to8copy_mxu(CurrUbuf+32,error,RECON_BUF_STRIDE);
  }
}

void
decoder_mb_decode_2pass(FIFO_DECODER const *dec,
			const uint32_t cbp,
			const uint16_t x_pos,
			const uint16_t y_pos,
			FIFO_MB const *pMB)
{
  int i;
  int16_t *residual = pMB->residual;
  int16_t * error = r2ptr[0]->IDCTBuf;
   
  for (i = 0; i < 4; i++) {
    /* Process only coded blocks */
    if (cbp & (1 << (5 - i))) {
      idct(residual, error,pMB->yuv_len[i]);
      residual += pMB->yuv_len[i]*8;
      error += 64;
    }
  }
  if (cbp & (1 << (5 - 4))) {
    idct(residual, error,pMB->yuv_len[4]);
    residual += pMB->yuv_len[4]*8;
    error += 64;
  }
  if (cbp & (1 << (5 - 5))) {
    idct(residual, error,pMB->yuv_len[5]);
  }
}


void
decoder_mbinter_2pass(FIFO_DECODER const *dec,
		      const uint16_t x_pos,
		      const uint16_t y_pos,
		      FIFO_MB const *pMB)
{

  VECTOR_AUX mv[4]; /* local copy of mvs */
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;

  
  const uint32_t rounding = pMB->rounding;
  const int32_t ref = pMB->ref;
  // const int32_t bvop = pMB->bvop_direct;
  const int32_t bvop = dec->coding_type;

  int uv_dx,uv_dy;
  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];

  if ((pMB->mode != MODE_INTER4V) || (bvop==2)) { /* #define MODE_INTER4V 2*/

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
      hw_mc_itp_qpel(dec->refn[ref][0], 16*x_pos, 16*y_pos, mv[0].x, mv[0].y, stride, 
		     rounding, CurrYbuf, MPEG_QPEL_16X16_BLK_INF);
    else
      hw_mc_itp_hpel(dec->refn[ref][0], 16*x_pos, 16*y_pos, mv[0].x, mv[0].y, stride, 
		     rounding, CurrYbuf, MPEG_HPEL_16X16_BLK_INF);
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
      hw_mc_itp_qpel(dec->refn[0][0], 16*x_pos, 16*y_pos, mv[0].x, mv[0].y, stride, rounding, 
		 CurrYbuf, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(dec->refn[0][0], 16*x_pos+8, 16*y_pos, mv[1].x, mv[1].y, stride, rounding, 
		 CurrYbuf+8, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(dec->refn[0][0], 16*x_pos, 16*y_pos+8, mv[2].x, mv[2].y, stride, rounding, 
		 CurrYbuf+8*RECON_BUF_STRIDE, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(dec->refn[0][0], 16*x_pos+8, 16*y_pos+8, mv[3].x, mv[3].y, stride, rounding, 
		 CurrYbuf+8*RECON_BUF_STRIDE+8, MPEG_QPEL_8X8_BLK_INF);
    } else {
      hw_mc_itp_hpel(dec->refn[0][0], 16*x_pos, 16*y_pos, mv[0].x, mv[0].y, stride, rounding, 
		 CurrYbuf, MPEG_HPEL_8X8_BLK_INF);
      hw_mc_itp_hpel(dec->refn[0][0], 16*x_pos+8, 16*y_pos, mv[1].x, mv[1].y, stride, rounding, 
		 CurrYbuf+8, MPEG_HPEL_8X8_BLK_INF);
      hw_mc_itp_hpel(dec->refn[0][0], 16*x_pos, 16*y_pos+8, mv[2].x, mv[2].y, stride, rounding, 
		 CurrYbuf+8*RECON_BUF_STRIDE, MPEG_HPEL_8X8_BLK_INF);
      hw_mc_itp_hpel(dec->refn[0][0], 16*x_pos+8, 16*y_pos+8, mv[3].x, mv[3].y, stride, rounding, 
		 CurrYbuf+8*RECON_BUF_STRIDE+8, MPEG_HPEL_8X8_BLK_INF);
    }
  }

  hw_mc_itp_hpel(dec->refn[ref][1],  8 * x_pos, 8 * y_pos, uv_dx, uv_dy, stride2, rounding, 
		 CurrUbuf, MPEG_HPEL_8X8_BLK_INF);
  hw_mc_itp_hpel(dec->refn[ref][2],  8 * x_pos, 8 * y_pos, uv_dx, uv_dy, stride2, rounding, 
		 CurrUbuf+32, MPEG_HPEL_8X8_BLK_INF);
  /*
  if (pMB->cbp)
    decoder_mb_decode_2pass(dec, pMB->cbp,x_pos,y_pos, pMB);
  */
}

#if 0
void
decoder_mbinter_field_2pass(FIFO_DECODER const * dec,
				     const uint16_t x_pos,
				     const uint16_t y_pos,
			    FIFO_MB const *pMB)
{
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;

  const uint32_t rounding = pMB->rounding;
  const int32_t ref = pMB->ref;
  // const int32_t bvop = pMB->bvop_direct;
     const int32_t bvop = dec->coding_type;

  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

  int uvtop_dx, uvtop_dy;
  int uvbot_dx, uvbot_dy;
  VECTOR mv[4]; /* local copy of mvs */

  /* Get pointer to memory areas */
  pY_Cur = dec->cur->y + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur->u + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur->v + (y_pos << 3) * stride2 + (x_pos << 3);
  
  mv[0] = pMB->mvs[0];
  mv[1] = pMB->mvs[1];
  mv[2] = pMB->mvs[2];
  mv[3] = pMB->mvs[3];

  if((pMB->mode!=MODE_INTER4V) || (bvop ==2))   /* #define MODE_INTER4V 2*/
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
      interpolate8x8_switch(dec->cur->y,dec->refn[ref]->y+pMB->field_for_top*stride,
                            16*x_pos,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);
      /* top field right part */
      interpolate8x8_switch(dec->cur->y,dec->refn[ref]->y+pMB->field_for_top*stride,
                            16*x_pos+8,8*y_pos,mv[0].x, mv[0].y>>1,2*stride, rounding);

      /* Interpolate bottom field left part(we use double stride for every 2nd line) */
      interpolate8x8_switch(dec->cur->y+stride,dec->refn[ref]->y+pMB->field_for_bot*stride,
                            16*x_pos,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);
      /* Bottom field right part */
      interpolate8x8_switch(dec->cur->y+stride,dec->refn[ref]->y+pMB->field_for_bot*stride,
                            16*x_pos+8,8*y_pos,mv[1].x, mv[1].y>>1,2*stride, rounding);

      /* Interpolate field1 U */
      interpolate8x4_switch(dec->cur->u,dec->refn[ref]->u+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
      
      /* Interpolate field1 V */
      interpolate8x4_switch(dec->cur->v,dec->refn[ref]->v+pMB->field_for_top*stride2,
                            8*x_pos,4*y_pos,uvtop_dx,DIV2ROUND(uvtop_dy),stride,rounding);
    
      /* Interpolate field2 U */
      interpolate8x4_switch(dec->cur->u+stride2,dec->refn[ref]->u+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    
      /* Interpolate field2 V */
      interpolate8x4_switch(dec->cur->v+stride2,dec->refn[ref]->v+pMB->field_for_bot*stride2,
                            8*x_pos,4*y_pos,uvbot_dx,DIV2ROUND(uvbot_dy),stride,rounding);
    }
  } 
  else 
  {
    /* We don't expect 4 motion vectors in interlaced mode */
  }

  /* Must add error correction? */
  if(pMB->cbp)
    //   decoder_mb_decode_2pass(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
    decoder_mb_decode_2pass(dec, pMB->cbp, x_pos,y_pos,pMB);

}
#endif

void
decoder_bf_interpolate_mbinter_2pass(FIFO_DECODER const *dec,
				     const uint16_t x_pos,
				     const uint16_t y_pos,
				     FIFO_MB const *pMB
				    )
{ 
  uint32_t stride = dec->edged_width;
  uint32_t stride2 = stride / 2;
  int uv_dx, uv_dy;
  int b_uv_dx, b_uv_dy;
  uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
  //const uint32_t cbp = pMB->cbp;

  uint8_t *forward[3];
  uint8_t *backward[3];
  forward[0] = dec->refn[1][0];
  forward[1] = dec->refn[1][1];
  forward[2] = dec->refn[1][2];
  backward[0] = dec->refn[0][0];
  backward[1] = dec->refn[0][1];
  backward[2] = dec->refn[0][2];

  pY_Cur = dec->cur[0] + (y_pos << 4) * stride + (x_pos << 4);
  pU_Cur = dec->cur[1] + (y_pos << 3) * stride2 + (x_pos << 3);
  pV_Cur = dec->cur[2] + (y_pos << 3) * stride2 + (x_pos << 3);

  if (!pMB->bvop_direct) {
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
    if(!pMB->bvop_direct) {
      hw_mc_itp_qpel(forward[0], 16*x_pos, 16*y_pos, pMB->mvs[0].x, pMB->mvs[0].y, stride, 0, 
		     CurrYbuf, MPEG_QPEL_16X16_BLK_INF);
    } else {
      hw_mc_itp_qpel(forward[0], 16*x_pos, 16*y_pos, pMB->mvs[0].x, pMB->mvs[0].y, stride, 0, 
		     CurrYbuf, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(forward[0], 16*x_pos+8, 16*y_pos, pMB->mvs[1].x, pMB->mvs[1].y, stride, 0, 
		     CurrYbuf+8, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(forward[0], 16*x_pos, 16*y_pos+8, pMB->mvs[2].x, pMB->mvs[2].y, stride, 0, 
		     CurrYbuf+8*RECON_BUF_STRIDE, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(forward[0], 16*x_pos+8, 16*y_pos+8, pMB->mvs[3].x, pMB->mvs[3].y, stride, 0, 
		     CurrYbuf+8*RECON_BUF_STRIDE+8, MPEG_QPEL_8X8_BLK_INF);
    }
  } else {
    hw_mc_itp_hpel(forward[0], 16*x_pos, 16*y_pos, pMB->mvs[0].x, pMB->mvs[0].y, stride, 0, 
		   CurrYbuf, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(forward[0], 16*x_pos+8, 16*y_pos, pMB->mvs[1].x, pMB->mvs[1].y, stride, 0, 
		   CurrYbuf+8, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(forward[0], 16*x_pos, 16*y_pos+8, pMB->mvs[2].x, pMB->mvs[2].y, stride, 0, 
		   CurrYbuf+8*RECON_BUF_STRIDE, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(forward[0], 16*x_pos+8, 16*y_pos+8, pMB->mvs[3].x, pMB->mvs[3].y, stride, 0, 
		   CurrYbuf+8*RECON_BUF_STRIDE+8, MPEG_HPEL_8X8_BLK_INF);
  }

  hw_mc_itp_hpel(forward[1],  8 * x_pos, 8 * y_pos, uv_dx, uv_dy, stride2, 0, 
		 CurrUbuf, MPEG_HPEL_8X8_BLK_INF);
  hw_mc_itp_hpel(forward[2],  8 * x_pos, 8 * y_pos, uv_dx, uv_dy, stride2, 0, 
		 CurrUbuf+32, MPEG_HPEL_8X8_BLK_INF);

  if(dec->quarterpel) {
    if(!pMB->bvop_direct) {
      hw_mc_itp_qpel(backward[0], 16*x_pos, 16*y_pos, pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0, 
		     r2ptr[0]->ReconYBi, MPEG_QPEL_16X16_BLK_INF);
    } else {
      hw_mc_itp_qpel(backward[0], 16*x_pos, 16*y_pos, pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0, 
		     r2ptr[0]->ReconYBi, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(backward[0], 16*x_pos+8, 16*y_pos, pMB->b_mvs[1].x, pMB->b_mvs[1].y, stride, 0, 
		     r2ptr[0]->ReconYBi+8, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(backward[0], 16*x_pos, 16*y_pos+8, pMB->b_mvs[2].x, pMB->b_mvs[2].y, stride, 0, 
		     r2ptr[0]->ReconYBi+8*RECON_BUF_STRIDE, MPEG_QPEL_8X8_BLK_INF);
      hw_mc_itp_qpel(backward[0], 16*x_pos+8, 16*y_pos+8, pMB->b_mvs[3].x, pMB->b_mvs[3].y, stride, 0, 
		     r2ptr[0]->ReconYBi+8*RECON_BUF_STRIDE+8, MPEG_QPEL_8X8_BLK_INF);
    }
  } else {
    hw_mc_itp_hpel(backward[0], 16*x_pos, 16*y_pos, pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0, 
		   r2ptr[0]->ReconYBi, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(backward[0], 16*x_pos+8, 16*y_pos, pMB->b_mvs[1].x, pMB->b_mvs[1].y, stride, 0, 
		   r2ptr[0]->ReconYBi+8, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(backward[0], 16*x_pos, 16*y_pos+8, pMB->b_mvs[2].x, pMB->b_mvs[2].y, stride, 0, 
		   r2ptr[0]->ReconYBi+8*RECON_BUF_STRIDE, MPEG_HPEL_8X8_BLK_INF);
    hw_mc_itp_hpel(backward[0], 16*x_pos+8, 16*y_pos+8, pMB->b_mvs[3].x, pMB->b_mvs[3].y, stride, 0, 
		   r2ptr[0]->ReconYBi+8*RECON_BUF_STRIDE+8, MPEG_HPEL_8X8_BLK_INF);
  }

  hw_mc_itp_hpel(backward[1],  8 * x_pos, 8 * y_pos, b_uv_dx, b_uv_dy, stride2, 0, 
		 r2ptr[0]->ReconUBi, MPEG_HPEL_8X8_BLK_INF);
  hw_mc_itp_hpel(backward[2],  8 * x_pos, 8 * y_pos, b_uv_dx, b_uv_dy, stride2, 0, 
		 r2ptr[0]->ReconUBi+8, MPEG_HPEL_8X8_BLK_INF);

  /*
  if (cbp)
    decoder_mb_decode_2pass(dec, cbp, x_pos,y_pos, pMB);
  */
}

