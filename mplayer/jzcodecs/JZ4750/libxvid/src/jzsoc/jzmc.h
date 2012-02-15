


#ifndef __JZ_MC_H__
#define __JZ_MC_H__

#include "jz_mc_hwacc.h"

extern uint32_t get_phy_addr(uint32_t);
static RV_MC_DesNode *MC;
static int blkindex;

static inline void xvid_mc_init(){
  int blk;
  MC = (RV_MC_DesNode *)TCSM_MC_DSPLINE0;
  for(blk=0;blk<DESP_NODE_COUNT;blk++){
#ifdef JZC_MC_NOAVG
    MC[blk].NodeHead = 0x61;
#else
    MC[blk].NodeHead = 0x81;
#endif
    MC[blk].RefAddr[0] = MC_REF_ADDR;
    MC[blk].CurrAddr[0] = MC_CURR_ADDR;
    MC[blk].BLKInfo[0] = MC_BLK_INFO;
    MC[blk].IntpInfo[0] = MC_INTP_INFO;
    //   MC[blk].Tap1Coef[0] = MC_TAP_COEF_REG1;
    //  MC[blk].Tap2Coef[0] = MC_TAP_COEF_REG3;
    // MC[blk].Tap1Coef[1] = TAPALN(-1,3,-6,20);
    // MC[blk].Tap2Coef[1] = TAPALN(-1,3,-6,20);  
     MC[blk].FrmSride[0] = MC_CURR_STRD;
#ifdef JZC_MC_NOAVG
#else
    MC[blk].Ref2Addr[0] = MC_REF2_ADDR;
    MC[blk].BufStrd[0]  = MC_REF2_STRD;
#endif
    MC[blk].MCStatus[0] = MC_STATUS;
    MC[blk].MCStatus[1] = 0x0;
  }

  MC = (RV_MC_DesNode *)TCSM_MC_DSPLINE1;
  for(blk=0;blk<DESP_NODE_COUNT;blk++){
#ifdef JZC_MC_NOAVG
    MC[blk].NodeHead = 0x61;
#else
    MC[blk].NodeHead = 0x81;
#endif
    MC[blk].RefAddr[0] = MC_REF_ADDR;
    MC[blk].CurrAddr[0] = MC_CURR_ADDR;
    MC[blk].BLKInfo[0] = MC_BLK_INFO;
    MC[blk].IntpInfo[0] = MC_INTP_INFO;
    //   MC[blk].Tap1Coef[0] = MC_TAP_COEF_REG1;
    //  MC[blk].Tap2Coef[0] = MC_TAP_COEF_REG3;
    // MC[blk].Tap1Coef[1] = TAPALN(-1,3,-6,20);
    // MC[blk].Tap2Coef[1] = TAPALN(-1,3,-6,20);  
     MC[blk].FrmSride[0] = MC_CURR_STRD;
#ifdef JZC_MC_NOAVG
#else
    MC[blk].Ref2Addr[0] = MC_REF2_ADDR;
    MC[blk].BufStrd[0]  = MC_REF2_STRD;
#endif
    MC[blk].MCStatus[0] = MC_STATUS;
    MC[blk].MCStatus[1] = 0x0;
  }


  set_mc_flag(); 
  run_mc();

#if 1
  set_coef_reg1(-1,3,-6,20); 
  set_coef_reg2(20,-6,3,-1);
  set_coef_reg3(-1,3,-6,20); 
  set_coef_reg4(20,-6,3,-1);
#endif
#ifdef JZC_MCDES_USE_EXTBUF
  set_mc_dha(get_phy_addr(MC));
#else
  //  
#endif
  // set_ref2_strd(RECON_LUMA_STRIDE);
 
}



#if 0
static inline hw_mc_config_switch(uint8_t *dst, uint8_t *ref, int dst_stride, int ref_stride, int x, int y, int16_t dx, int16_t dy, int blktype,int rounding,int avg)

{
  uint8_t *pRef,*pCur;
  int16_t mvdx, mvdy;


  pRef = (uint8_t*)get_phy_addr(ref  + ((dx>>1)+x)+((dy>>1)+(y))*ref_stride);
  pCur = dst;//get_phy_addr(dst  + x+y*dst_stride);
 
  mvdx = (dx&0x1);
  mvdy = (dy&0x1);
  
  if(avg)
    {
      /*config MC*/ 
      MC[blkindex].NodeHead |= NODE_LINK;
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
      MC[blkindex].Ref2Addr[1] = (uint32_t)pCur;
      MC[blkindex].BufStrd[1] =  dst_stride;
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][blktype]|MC_USE_AVG;// RV9LumaBLK_tab[MC_BLK_W16XH16][0][0];
      MC[blkindex].IntpInfo[1] =  HPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdx<<1)+mvdy];  

    }
  else
    {
         /*config MC*/ 
      MC[blkindex].NodeHead |= NODE_LINK;
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][blktype];// RV9LumaBLK_tab[MC_BLK_W16XH16][0][0];
      MC[blkindex].IntpInfo[1] =  HPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdx<<1)+mvdy];  
      
    }
   
  blkindex++;

}
#else
#define hw_mc_config_switch(dst,ref,dst_stride,ref_stride,x,y,dx, dy,blktype,rounding,avg) \
  do{\
    uint8_t *pRef,*pCur;			\
    int16_t mvdx, mvdy;				\
    pRef = (uint8_t*)get_phy_addr(ref  + ((dx>>1)+x)+((dy>>1)+(y))*ref_stride);	\
    pCur = dst;\  
    mvdx = (dx&0x1);						     \
    mvdy = (dy&0x1);						     \
    if(avg)							     \
    {\
      MC[blkindex].NodeHead |= NODE_LINK;		\
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;	\
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;	\
      MC[blkindex].Ref2Addr[1] = (uint32_t)pCur;	\
      MC[blkindex].BufStrd[1] =  dst_stride;			\
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;		\
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][blktype]|MC_USE_AVG;\  
      MC[blkindex].IntpInfo[1] =  HPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdx<<1)+mvdy];\  
      }						\
    else						\
      {							\
	MC[blkindex].NodeHead |= NODE_LINK;		\
	MC[blkindex].RefAddr[1] =  (uint32_t)pRef;	\
	MC[blkindex].CurrAddr[1] = (uint32_t)pCur;		\
	MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;		\
	MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][blktype]; \ 
     	MC[blkindex].IntpInfo[1] =  HPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdx<<1)+mvdy]; \  
    }\
  blkindex++;\
  }while(0);

#endif



#if 0
/*the ref2 address set as dst address defaultly*/
static inline hw_mc_config_quarter(uint8_t *dst, uint8_t *ref, int dst_stride, int ref_stride, int x, int y, int16_t dx, int16_t dy, int blktype, int rounding, int avg)
{
  uint8_t *pRef,*pCur;
  int16_t mvdx, mvdy;


  pRef = (uint8_t*)get_phy_addr(ref  + ((dx>>2)+x)+((dy>>2)+(y))*ref_stride);
  pCur = dst;//get_phy_addr(dst  + x+y*dst_stride);
 
  mvdx = (dx&0x3);
  mvdy = (dy&0x3);
  
  if(avg)
    {
      /*config MC*/ 
      MC[blkindex].NodeHead |= NODE_LINK;
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
      MC[blkindex].Ref2Addr[1] = (uint32_t)pCur;
      MC[blkindex].BufStrd[1] =  dst_stride;
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[1][blktype]|MC_USE_AVG;// RV9LumaBLK_tab[MC_BLK_W16XH16][0][0];
      MC[blkindex].IntpInfo[1] =  QPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdy<<2)+mvdx];  

    }
  else
    {
         /*config MC*/ 
      MC[blkindex].NodeHead |= NODE_LINK;
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[1][blktype];// RV9LumaBLK_tab[MC_BLK_W16XH16][0][0];
      MC[blkindex].IntpInfo[1] =  QPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdy<<2)+mvdx];  
      
    }
   
  blkindex++;
  
}

#else
#define hw_mc_config_quarter(dst,ref,dst_stride,ref_stride,x,y,dx, dy,blktype,rounding,avg) \
  do{\
    uint8_t *pRef,*pCur;			\
    int16_t mvdx, mvdy;				\
    pRef = (uint8_t*)get_phy_addr(ref  + ((dx>>2)+x)+((dy>>2)+(y))*ref_stride);	\
    pCur = dst;\  	     
    mvdx = (dx&0x3);						     \
    mvdy = (dy&0x3);						     \
    if(avg)							     \
    {\
      MC[blkindex].NodeHead |= NODE_LINK;		\
      MC[blkindex].RefAddr[1] =  (uint32_t)pRef;	\
      MC[blkindex].CurrAddr[1] = (uint32_t)pCur;	\
      MC[blkindex].Ref2Addr[1] = (uint32_t)pCur;	\
      MC[blkindex].BufStrd[1] =  dst_stride;			\
      MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;		\
      MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[1][blktype]|MC_USE_AVG;\  
      MC[blkindex].IntpInfo[1] =  QPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdy<<1)+mvdx];\  
      }						\
    else						\
      {							\
	MC[blkindex].NodeHead |= NODE_LINK;		\
	MC[blkindex].RefAddr[1] =  (uint32_t)pRef;	\
	MC[blkindex].CurrAddr[1] = (uint32_t)pCur;		\
	MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;		\
	MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[1][blktype]; \  
     	MC[blkindex].IntpInfo[1] =  QPEL_IRND_IntpCase_RND1_RND0[rounding][(mvdy<<2)+mvdx]; \  
    }\
  blkindex++;\
  }while(0);


#endif


static inline hw_mc_iframe_config(uint8_t *dst, uint8_t *ref, int dst_stride, int ref_stride, int  blktype)

{
  uint8_t *pRef,*pCur;

  pRef = (uint8_t*)ref;
  pCur = (uint8_t*)get_phy_addr(dst);
 
  // printf("dst_stride is %d.\n",dst_stride);
  /*config MC*/ 
  MC[blkindex].NodeHead |= NODE_LINK;
  MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
  MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
  MC[blkindex].FrmSride[1] = (dst_stride<<16)+ref_stride;
  MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][blktype]; 
  MC[blkindex].IntpInfo[1] = 0;// HPEL_IRND_IntpCase_RND1_RND0[0][0];  

#if 0
  printf("ref addr is %x\n", MC[blkindex].RefAddr[1]);
  printf("curr addr is %x\n", MC[blkindex].CurrAddr[1]);
  printf("frm stride  is %x\n", MC[blkindex].FrmSride[1]);
  printf("blk info  is %x\n", MC[blkindex].BLKInfo[1]);
  printf("IntpInfo  is %x\n", MC[blkindex].IntpInfo[1]);
#endif
      
   
  blkindex++;

}


static inline hw_mc_memset_config(uint8_t *dst, uint8_t *ref)

{
  uint8_t *pRef,*pCur;

  pRef = (uint8_t*)ref;
  pCur = (uint8_t*)dst;
 
  // printf("dst_stride is %d.\n",dst_stride);
  /*config MC*/ 
  MC[blkindex].NodeHead |= NODE_LINK;
  MC[blkindex].RefAddr[1] =  (uint32_t)pRef;
  MC[blkindex].CurrAddr[1] = (uint32_t)pCur;
  MC[blkindex].FrmSride[1] = (16<<16)+16;
  MC[blkindex].BLKInfo[1] = XVIDLumaBLK_tab[0][MC_BLK_W16XH16]; 
  MC[blkindex].IntpInfo[1] = 0;// HPEL_IRND_IntpCase_RND1_RND0[0][0];  

#if 0
  printf("ref addr is %x\n", MC[blkindex].RefAddr[1]);
  printf("curr addr is %x\n", MC[blkindex].CurrAddr[1]);
  printf("frm stride  is %x\n", MC[blkindex].FrmSride[1]);
  printf("blk info  is %x\n", MC[blkindex].BLKInfo[1]);
  printf("IntpInfo  is %x\n", MC[blkindex].IntpInfo[1]);
#endif
      
   
  blkindex++;

}

#endif //__JZ_MC_H__





