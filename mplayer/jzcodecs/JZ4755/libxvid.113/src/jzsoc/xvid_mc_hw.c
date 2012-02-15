/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(XviD)
 *
* $Id: xvid_mc_hw.c,v 1.3 2009/06/01 02:33:08 yzhai Exp $
 *
 ****************************************************************************/
#include "tcsm.h"
#include "xvid_mc_hw.h"

extern  unsigned char *frame_buffer;
extern  unsigned int   phy_fb;

XVID_MC_DesNode * mc;

#if 0
void xvid_mc_init()
{
  int i; 
  mc = (XVID_MC_DesNode *)TCSM1_MC_DES_BUF;
  
  for(i = 0; i < 12; i ++)
  {
    mc->NodeFlag   = DESP_NODE_HEAD_FG;
    mc->NodeHead   = DESP_NODE_HEAD_LK;
    mc->Tap1Coef   = TAPALN(-1,3,-6,20);
    mc->Tap2Coef   = TAPALN(-1,3,-6,20);
    mc->MCStatus   = 0x0;
    mc ++ ;
  }  
  // for qpel , when h-pel is not used, it can be any value.
  SET_COEF_REG2(20,-6,3,-1);
  SET_COEF_REG4(20,-6,3,-1);

  SET_MC_END_FLAG();
  RUN_MC();
}
#endif

void hw_mc_itp_hpel( const uint8_t * const refn,
		     const uint32_t   x,
		     const uint32_t   y,
		     const int32_t    dx,
		     const int        dy,
		     const uint32_t   stride,
		     const uint32_t   rounding,
		     const uint8_t * const curraddr,
		     const int blksize)
{
  uint8_t *Ref;
  uint8_t itpcase;

  Ref  = refn + ((int)((y + (dy>>1)) * stride + x + (dx>>1)));
  mc->RefAddr  = get_phy_addr_aux(Ref);

  mc->Stride   = (RECON_BUF_STRIDE<<16)|(stride);
  mc->BLKInfo  = blksize;
  mc->CurrAddr = curraddr;   

  itpcase      = (((dx & 1) << 1) + (dy & 1));

  mc->IntpInfo = HPEL_IRND_IntpCase_RND1_RND0[rounding][itpcase];
  mc->NodeHead = DESP_NODE_HEAD_LK;
  mc++;
}

void hw_mc_itp_qpel(uint8_t * const refn,
		    const uint32_t x, const uint32_t y,
		    const int32_t dx,  const int dy,
		    const uint32_t stride,
		    const uint32_t rounding,
		    const uint8_t * const curraddr,
		    const int blksize)
{
  uint8_t *Ref = refn + (((int)y*4 + dy) >> 2) * (int)stride + (((int)x*4 + dx) >> 2);
  int8_t itpcase;

  mc->Stride   = (RECON_BUF_STRIDE<<16)|(stride);
  mc->BLKInfo  = blksize;
  mc->CurrAddr = curraddr;
  mc->RefAddr  = get_phy_addr_aux(Ref);

  itpcase      = (dx&3)|((dy&3)<<2);
  mc->IntpInfo = QPEL_IRND_IntpCase_RND1_RND0[rounding][itpcase];
  mc->NodeHead = DESP_NODE_HEAD_LK;
  mc++;
}

void MXU_DirectB_MBAVG(const uint8_t *MCReconBuf, 
		       const uint8_t *MCReconUBuf, 
		       const uint8_t *MCReconBiBuf,
		       const uint8_t *MCReconUBiBuf)
{
  uint32_t i;
  uint8_t* src0 = MCReconBuf - RECON_BUF_STRIDE;
  uint8_t* srcu = MCReconUBuf - RECON_BUF_STRIDE;
  uint8_t* src1 = MCReconBiBuf - RECON_BUF_STRIDE;
  uint8_t* srcu1 = MCReconUBiBuf - RECON_BUF_STRIDE;
  uint8_t* dst  = src0;
  uint8_t* dstu = srcu;
  /*Luma*/
  for (i = 0; i < 16; i++)
    {
      S32LDI(xr1, src0, RECON_BUF_STRIDE);
      S32LDI(xr2, src1, RECON_BUF_STRIDE);
      
      S32LDD(xr3, src0, 4);
      S32LDD(xr4, src1, 4);

      Q8AVGR(xr13, xr1, xr2);
      S32SDI(xr13, dst, RECON_BUF_STRIDE);

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

  /*Chroma*/
  for (i = 0; i < 8; i++)
    {
      S32LDI(xr1, srcu, RECON_BUF_STRIDE);
      S32LDI(xr2, srcu1, RECON_BUF_STRIDE);
      
      S32LDD(xr3, srcu, 4);
      S32LDD(xr4, srcu1, 4);

      Q8AVGR(xr13, xr1, xr2);
      S32SDI(xr13, dstu, RECON_BUF_STRIDE);

      Q8AVGR(xr13, xr3, xr4);
      S32STD(xr13, dstu, 4);

      S32LDD(xr5, srcu, 32);
      S32LDD(xr6, srcu1, 8);

      S32LDD(xr7, srcu, 36);
      S32LDD(xr8, srcu1, 12);

      Q8AVGR(xr13, xr5, xr6);
      S32STD(xr13, dstu, 32);

      Q8AVGR(xr13, xr7, xr8);
      S32STD(xr13, dstu, 36);
    }
}
