/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - GMC interpolation module -
 *
 *  Copyright(C) 2002-2003 Pascal Massimino <skal@planet-d.net>
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
 * $Id: gmc.c,v 1.2 2009/06/05 05:35:39 yli Exp $
 *
 ****************************************************************************/
#ifdef _UCOS_
#include <mplaylib.h>
#include "mplaylib.h"
#else
#include <stdio.h>
#endif

#include "../portab.h"
#include "../global.h"
#include "../encoder.h"

#ifdef JZC_MXU_OPT
#include "../jzsoc/jzmedia.h"
#endif

#include "gmc.h"

/* ************************************************************
 * Pts = 2 or 3
 *
 * Warning! *src is the global frame pointer (that is: adress
 * of pixel 0,0), not the macroblock one.
 * Conversely, *dst is the macroblock top-left adress.
 */
#ifdef JZC_MXU_OPT
void Predict_16x16_C(const NEW_GMC_DATA * const This,
          					 uint8_t *dst, 
          					 const uint8_t *src,
          					 int dststride, 
          					 int srcstride, 
          					 int x, 
          					 int y,
          					 int rounding)
{
  const int W       = This->sW;
  const int H	    = This->sH;
  const int rho     = 3 - This->accuracy;
  const int Rounder = ( (1<<7) - (rounding<<(2*rho)) ) << 16;
  
  const int dUx = This->dU[0];
  const int dVx = This->dV[0];
  const int dUy = This->dU[1];
  const int dVy = This->dV[1];
  
  int Uo = This->Uo + 16*(dUy*y + dUx*x);
  int Vo = This->Vo + 16*(dVy*y + dVx*x);
  
  int i, j;

  dst += 16;
  {
    unsigned int ri = 16;
    unsigned int rj = 16;
    int Offset;
    int u,v;

    uint8_t *srctmp;
    uint32_t tmpf = 0;
    
    S32I2M(xr15,dUx);
    S32I2M(xr14,dVx);
    S32I2M(xr13,dUy);
    S32I2M(xr12,dVy);

    S32I2M(xr11,Uo); // Uo 11
    S32I2M(xr10,Vo); // Vo 10
    
    S32I2M(xr5, Rounder);

    for (j = 16; j>0; --j) 
    {
      D32SLL(xr9,xr11,xr10,xr8, 0x0); // U 9 ,V 8
      D32ASUM_AA(xr11,xr13,xr12,xr10); // += dUy; +=dVy;
   

      for (i = -16; i<0; ++i)
      {
        ri = 16;
        rj = 16;

        // ( U >> 16 ) ,( V >> 16 )
        D32SAR(xr7,xr9,xr8,xr6, 0x8); 
        D32SAR(xr7,xr7,xr6,xr6, 0x8);

        D32SLLV(xr7,xr6, rho); // << rho

        u = S32M2I(xr7);
        v = S32M2I(xr6);
        D32ASUM_AA(xr9,xr15,xr14,xr8); // U += dUx; V += dVx;
        
	if (u > 0 && u <= W)
        {
          ri = MTab[u&15]; 
          Offset = u>>4;
        }
	else
        {
	  if (u > W) 
            Offset = W>>4;
	  else
            Offset = 0;
          
	  ri = MTab[0];
	}

	if (v > 0 && v <= H)
        {
          rj      = MTab[v&15]; 
          Offset += (v>>4)*srcstride; 
        }
	else
        {
	  if (v > H) 
            Offset += (H>>4)*srcstride;
	  rj = MTab[0];
	}


        //--------------------
        // f0 :xr1 , f1 :xr3

        srctmp = src + Offset;
        S32LUI(xr1, 0x0, ptn7);
        S8LDD(xr1,srctmp,0x0,ptn0);
        S8LDD(xr1,srctmp,0x1,ptn2);

        srctmp += srcstride;
        S32LUI(xr3, 0x0, ptn7);
        S8LDD(xr3,srctmp,0x0,ptn0);
        S8LDD(xr3,srctmp,0x1,ptn2);

        tmpf = S32M2I(xr1);

        S32MULU(xr2, xr1, ri,tmpf);     // [xr2 ,xr1] = ri * f0;
       
	S32ALNI(xr1, xr0, xr1, ptn2);

        tmpf = S32M2I(xr3);
        S32MULU(xr4,xr3,ri,tmpf);
        S32I2M(xr4, 0x0fff0000);
        S32AND(xr3,xr3,xr4); // & 0x0fff0000

        S32OR(xr1,xr1,xr3); // f0 |= f1;

        
        //f0 = ( rj*f0 + Rounder ) >> 24;
        tmpf = S32M2I(xr1);
        S32MULU(xr2,xr1,rj,tmpf);
        D32ADD_AA(xr1,xr1,xr5,xr0);
	S32ALNI(xr1,xr0,xr1, ptn1); // >>24
        S8STD(xr1, &(dst[i]), 0x0, ptn0);
      }
      dst += dststride;
    }
  }
}
#else
void Predict_16x16_C(const NEW_GMC_DATA * const This,
		     uint8_t *dst, const uint8_t *src,
		     int dststride, int srcstride, int x, int y, int rounding)
{
  const int W = This->sW;
  const int H	= This->sH;
  const int rho = 3 - This->accuracy;
  const int Rounder = ( (1<<7) - (rounding<<(2*rho)) ) << 16;

  const int dUx = This->dU[0];
  const int dVx = This->dV[0];
  const int dUy = This->dU[1];
  const int dVy = This->dV[1];
  
  int Uo = This->Uo + 16*(dUy*y + dUx*x);
  int Vo = This->Vo + 16*(dVy*y + dVx*x);
  
  int i,j;
  dst += 16;
  for (j=16; j>0; --j) 
  {
    int U = Uo, V = Vo;
    Uo += dUy; Vo += dVy;
    for (i=-16; i<0; ++i) 
      {
	unsigned int f0, f1, ri = 16, rj = 16;
	int Offset;
	int u = ( U >> 16 ) << rho;
	int v = ( V >> 16 ) << rho;
	
	U += dUx; V += dVx;
	
	if (u > 0 && u <= W) 
	  { 
	    ri = MTab[u&15]; 
	    Offset = u>>4;	
	  }
	else 
	  {
	    if (u > W) 
	      Offset = W>>4;
	    else 
	      Offset = 0;
	    ri = MTab[0];
	  }
	
	if (v > 0 && v <= H) 
	  { 
	    rj = MTab[v&15]; 
	    Offset += (v>>4)*srcstride; 
	  }
	else 
	  {
	    if (v > H) 
	      Offset += (H>>4)*srcstride;
	    rj = MTab[0];
	  }
	f0  = src[Offset + 0];
	f0 |= src[Offset + 1] << 16;
	f1  = src[Offset + srcstride + 0];
	f1 |= src[Offset + srcstride + 1] << 16;
	f0  = (ri*f0)>>16;
	f1  = (ri*f1) & 0x0fff0000;
	f0 |= f1;
	f0  = (rj*f0 + Rounder) >> 24;
	dst[i] = (uint8_t)f0;
      }
    dst += dststride;
  }
}
#endif

#ifdef JZC_MXU_OPT
void Predict_8x8_C(const NEW_GMC_DATA * const This,
		   uint8_t *uDst, const uint8_t *uSrc,
		   uint8_t *vDst, const uint8_t *vSrc,
		   int dststride, int srcstride, int x, int y, int rounding)
{
  const int W	 = This->sW >> 1;
  const int H	 = This->sH >> 1;
  const int rho = 3-This->accuracy;
  const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;
  
  const int32_t dUx = This->dU[0];
  const int32_t dVx = This->dV[0];
  const int32_t dUy = This->dU[1];
  const int32_t dVy = This->dV[1];

  //int32_t Uo, Vo, U, V;


  // int32_t Uo = This->Uco + 8*(dUy*y + dUx*x);
  // int32_t Vo = This->Vco + 8*(dVy*y + dVx*x);
  S32I2M(xr15, This->Uco);
  S32I2M(xr14, This->Vco);

  S32MUL(xr2, xr1, dUy, y);
  S32MADD(xr2, xr1, dUx, x);                          // xr1 = dUy*y + dUx*x
  S32MUL(xr3, xr2, dVy, y);
  S32MADD(xr3, xr2, dVx, x);                          // xr2 = dVy*y + dVx*x
  D32SLL(xr1, xr1, xr2, xr2, 0x3);                    // xr1 = 8*(dUy*y + dUx*x)
                                                      // xr2 = 8*(dVy*y + dVx*x)
  D32ASUM_AA(xr15, xr1, xr2, xr14);                   // xr15 = Uo = This->Uco + 8*(dUy*y + dUx*x)--
                                                      // xr14 = Vo = This->Vco + 8*(dVy*y + dVx*x)--
  S32I2M(xr13, dUx);
  S32I2M(xr12, dVx);
  S32I2M(xr11, dUy);
  S32I2M(xr10, dVy);

  int i, j; 
  int32_t u, v, U, V;
  uint32_t f0, f1, ri, rj;
  int Offset;

  uDst += 8;
  vDst += 8;
  for(j=8; j>0; --j)
  {
    D32SLL(xr9, xr15, xr14, xr8, 0x0);                   // xr9 = U = Uo
                                                         // xr8 = V = Vo
    //    D32ADD_AA(xr9, xr15, xr0, xr7);
    //    D32ADD_AA(xr8, xr14, xr0, xr7); 

    D32ASUM_AA(xr15, xr11, xr10, xr14);                  // xr15 = Uo = Uo + dUy; xr14 = Vo = Vo + dVy

    for(i=-8; i<0; ++i)
    { 
      //     U = S32M2I(xr9);
      //      V = S32M2I(xr8);

      //      u = (U >> 16) << rho;
      //      v = (V >> 16) << rho;

      D32SAR(xr7, xr9, xr8, xr6, 0x8);
      D32SAR(xr7, xr7, xr6, xr6, 0x8);
      D32SLLV(xr7, xr6, rho);                            // xr7 = (U>>16) << rho
                                                         // xr6 = (V>>16) << rho

      D32ASUM_AA(xr9, xr13, xr12, xr8);                  // xr9 = U = U + dUx; xr8 = V = V + dVx  

      u = S32M2I(xr7);
      v = S32M2I(xr6);

      if (u > 0 && u <= W)
      {
	ri = MTab[u&15];
	Offset = u >> 4;
      }
      else 
      {
	if (u>W) 
	  Offset = W>>4;
	else 
	  Offset = 0;
	ri = MTab[0];
      }
	  
      if (v > 0 && v <= H) 
      {
	rj = MTab[v&15];
	Offset += (v>>4)*srcstride;
      } 
      else 
      {
	if (v>H) 
	  Offset += (H>>4)*srcstride;
	rj = MTab[0];
      }

      S32I2M(xr1, 0x0FFF0000);

      S32LUI(xr7, 0x0, ptn7);
      S32LUI(xr6, 0x0, ptn7);      
      S8LDD(xr7, &uSrc[Offset], 0x0, ptn0);
      S8LDD(xr7, &uSrc[Offset], 0x1, ptn2);             // xr7 = uSrc[Offset + 0] | uSrc[Offset + 1] << 16
      S8LDD(xr6, &uSrc[Offset + srcstride], 0x0, ptn0);
      S8LDD(xr6, &uSrc[Offset + srcstride], 0x1, ptn2); // xr6 = uSrc[Offset+srcstride+0]|uSrc[Offset+srcstride+1]<<16

      f0 = S32M2I(xr7);
      f1 = S32M2I(xr6); 
      S32MULU(xr6, xr7, ri, f0);                        // xr7 = ri*f0--u
      S32MULU(xr5, xr6, ri, f1);                        // xr6 = ri*f1--u    

      S32LUI(xr5, 0x0, ptn7);
      S32LUI(xr4, 0x0, ptn7);
      S8LDD(xr5, &vSrc[Offset], 0x0, ptn0);
      S8LDD(xr5, &vSrc[Offset], 0x1, ptn2);             // xr5 = vSrc[Offset + 0] | vSrc[Offset + 1] << 16
      S8LDD(xr4, &vSrc[Offset + srcstride], 0x0, ptn0);
      S8LDD(xr4, &vSrc[Offset + srcstride], 0x1, ptn2); // xr4 = vSrc[Offset+srcstride+0]|vSrc[Offset+srcstride+1]<<16

      f0 = S32M2I(xr5);
      f1 = S32M2I(xr4); 

      S32MULU(xr4, xr5, ri, f0);                        // xr5 = ri*f0--v
      S32MULU(xr3, xr4, ri, f1);                        // xr4 = ri*f1--v

      D32SAR(xr7, xr7, xr5, xr5, 0x8);                  
      D32SAR(xr7, xr7, xr5, xr5, 0x8);                  // xr7 = (ri*f0)>>16--u
                                                        // xr5 = (ri*f0)>>16--v

      S32AND(xr6, xr6, xr1);                            // xr6 = (ri*f1) & 0x0fff0000--u
      S32AND(xr4, xr4, xr1);                            // xr4 = (ri*f1) & 0x0fff0000--v

      S32OR(xr7, xr7, xr6);                             // xr7 = f0 = (ri*f0)>>16 | (ri*f1) & 0x0fff0000--u
      S32OR(xr5, xr5, xr4);                             // xr5 = f0 = (ri*f0)>>16 | (ri*f1) & 0x0fff0000--v

      f0 = S32M2I(xr7);
      S32MULU(xr6, xr7, rj, f0);                        // xr7 = rj*f0--u  
   
      f0 = S32M2I(xr5);
      S32MULU(xr4, xr5, rj, f0);                        // xr5 = rj*f0--v

      S32I2M(xr2, Rounder);
      D32ASUM_AA(xr7, xr2, xr2, xr5);                   // xr7 = (rj*f0 + Rounder)-u
                                                        // xr5 = (rj*f0 + Rounder)-v
      D32SAR(xr7, xr7, xr5, xr5, 0xC);                  
      D32SAR(xr7, xr7, xr5, xr5, 0xC);                 // xr7 = (rj*f0 + Rounder)>> 24 -u
                                                       // xr5 = (rj*f0 + Rounder)>> 24 -v 

      S8STD(xr7, &uDst[i], 0x0, ptn0);                  // uDst[i] = (uint8_t) f0;
      S8STD(xr5, &vDst[i], 0x0, ptn0);                  // vDst[i] = (uint8_t) f0;
    }
    uDst += dststride;
    vDst += dststride;
  }
}
#else
void Predict_8x8_C(const NEW_GMC_DATA * const This,
		   uint8_t *uDst, const uint8_t *uSrc,
		   uint8_t *vDst, const uint8_t *vSrc,
		   int dststride, int srcstride, int x, int y, int rounding)
{
  const int W	 = This->sW >> 1;
  const int H	 = This->sH >> 1;
  const int rho = 3-This->accuracy;
  const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;
  
  const int32_t dUx = This->dU[0];
  const int32_t dVx = This->dV[0];
  const int32_t dUy = This->dU[1];
  const int32_t dVy = This->dV[1];
  
  int32_t Uo = This->Uco + 8*(dUy*y + dUx*x);
  int32_t Vo = This->Vco + 8*(dVy*y + dVx*x);
  
  int i, j;
  
  uDst += 8;
  vDst += 8;
  for (j=8; j>0; --j) 
  {
    int32_t U = Uo, V = Vo; 
    Uo += dUy; Vo += dVy; 
      
    for (i=-8; i<0; ++i) 
    {
      int Offset;
      uint32_t f0, f1, ri, rj;
      int32_t u, v;
      
      u = ( U >> 16 ) << rho;
      v = ( V >> 16 ) << rho;
      U += dUx; V += dVx;
	  
      if (u > 0 && u <= W) 
      {
	ri = MTab[u&15];
	Offset = u>>4;
      } 
      else 
      {
	if (u>W) 
	  Offset = W>>4;
	else 
	  Offset = 0;
	ri = MTab[0];
      }
	  
      if (v > 0 && v <= H) 
      {
	rj = MTab[v&15];
	Offset += (v>>4)*srcstride;
      } 
      else 
      {
	if (v>H) 
	  Offset += (H>>4)*srcstride;
	rj = MTab[0];
      }
	  
      f0	= uSrc[Offset + 0];
      f0 |= uSrc[Offset + 1] << 16;
      f1	= uSrc[Offset + srcstride + 0];
      f1 |= uSrc[Offset + srcstride + 1] << 16;
      f0 = (ri*f0)>>16;
      f1 = (ri*f1) & 0x0fff0000;
      f0 |= f1;
      f0 = (rj*f0 + Rounder) >> 24;
	  
      uDst[i] = (uint8_t)f0;
      
      f0	= vSrc[Offset + 0];
      f0 |= vSrc[Offset + 1] << 16;
      f1	= vSrc[Offset + srcstride + 0];
      f1 |= vSrc[Offset + srcstride + 1] << 16;
      f0 = (ri*f0)>>16;
      f1 = (ri*f1) & 0x0fff0000;
      f0 |= f1;
      f0 = (rj*f0 + Rounder) >> 24;
	  
      vDst[i] = (uint8_t)f0;
    }
    uDst += dststride;
    vDst += dststride;
  }
}
#endif


#ifdef JZC_MXU_OPT
void get_average_mv_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
		      int x, int y, int qpel)
{

  int i, j;
  int vx = 0, vy =0;

  int32_t uo = Dsp->Uo; 
  int32_t vo = Dsp->Vo; 
  const int32_t dUx = Dsp->dU[0];
  const int32_t dVx = Dsp->dV[0];
  const int32_t dUy = Dsp->dU[1];
  const int32_t dVy = Dsp->dV[1];

  S32I2M(xr15, dUx);              // xr15 = dUx ( Dsp->dU[0])
  S32I2M(xr14, dVx);              // xr14 = dVx ( Dsp->dV[0])
  S32I2M(xr13, dUy);              // xr13 = dUy ( Dsp->dU[1])
  S32I2M(xr12, dVy);              // xr12 = dVy ( Dsp->dV[1])

  S32I2M(xr11, x);                // xr11 = x
  S32I2M(xr10, y);                // xr10 = y

  S32I2M(xr9, uo);                // xr9 = uo ( Dsp->Uo)
  S32I2M(xr8, vo);                // xr8 = vo ( Dsp->Vo)

  S32I2M(xr7, vx);                // xr7 = vx
  S32I2M(xr6, vy);                // xr6 = vy
  
  S32MUL(xr2, xr1, dUy, y);          // tmp64 = signed(dUy) * signed(y)
                                     // xr2 = tmp64 [63:32]; xr1 = tmp64 [31:00] 

  S32MADD(xr2, xr1, dUx, x);         // tmp64 = {xr2,xr1} + signed(dUx)*signed(x)
                                     // xr2 = tmp64 [63:32]
                                        // xr1 = tmp64 [31:00] = dUy*y + dUx*x

  S32MUL(xr3, xr2, dVy, y);          // tmp64 = signed(dVy) * signed(y)
                                     // xr3 = tmp64 [63:32]; xr2 = tmp64 [31:00] 

  S32MADD(xr3, xr2, dVx, x);         // tmp64 = {xr2,xr1} + signed(dVx)*signed(x)
                                     // xr3 = tmp64 [63:32]
                                        // xr2 = tmp64 [31:00] = dVy*y + dVx*x

  D32SLL(xr1, xr1, xr2, xr2, 4);        // xr1 = xr1 << 4 = (dUy*y + dUx*x)<<4  
                                        // xr2 = xr2 << 4 = (dVy*y + dVx*x)<<4  

  D32ASUM_AA(xr9, xr1, xr2, xr8);       // xr9 = xr1+xr9 = uo+(dUy*y + dUx*x)<< 4
                                        // xr8 = xr2+xr8 = vo+(dVy*y + dVx*x)<< 4
  for (j=16; j>0; --j)
  {
    D32ADD_AA(xr1, xr9, xr0, xr2);      // xr1 = U = uo
    D32ADD_AA(xr2, xr8, xr0, xr3);      // xr2 = V = vo

    D32ASUM_AA(xr9, xr13, xr12, xr8);   // xr9 = uo + dU1(uo); xr8 = vo + dV1(vo)

    for (i=16; i>0; --i)
    {
           
      D32SAR(xr3, xr1, xr2, xr4, 0x8);  // xr3 = U >> 8 ; xr4 = V >> 8
      D32SAR(xr3, xr3, xr4, xr4, 0x8);  // xr3 = U >> 8 ; xr4 = V >> 8
    
      D32ASUM_AA(xr1, xr15, xr14, xr2); // xr1 = U + dU0; xr2 = V + dV0
      D32ASUM_AA(xr7, xr3, xr4, xr6);   // xr7 = vx + u(vx); xr6 = vy + v(vy)  
    }
  }  


  vx = S32M2I(xr7);
  vy = S32M2I(xr6);

  vx -= (256*x+120) << (5+Dsp->accuracy);	/* 120 = 15*16/2 */
  vy -= (256*y+120) << (5+Dsp->accuracy);

  mv->x = RSHIFT( vx, 8+Dsp->accuracy - qpel );
  mv->y = RSHIFT( vy, 8+Dsp->accuracy - qpel );

}

#else

void get_average_mv_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
		      int x, int y, int qpel)
{

  int i, j;
  int vx = 0, vy = 0;
  int32_t uo = Dsp->Uo + 16*(Dsp->dU[1]*y + Dsp->dU[0]*x);
  int32_t vo = Dsp->Vo + 16*(Dsp->dV[1]*y + Dsp->dV[0]*x);
  for (j=16; j>0; --j)
  {
    int32_t U, V;
    U = uo; uo += Dsp->dU[1];
    V = vo; vo += Dsp->dV[1];
    for (i=16; i>0; --i)
    {
      int32_t u,v;
      u = U >> 16; U += Dsp->dU[0]; vx += u;
      v = V >> 16; V += Dsp->dV[0]; vy += v;
    }
  }
  vx -= (256*x+120) << (5+Dsp->accuracy);	/* 120 = 15*16/2 */
  vy -= (256*y+120) << (5+Dsp->accuracy);
  
  mv->x = RSHIFT( vx, 8+Dsp->accuracy - qpel );
  mv->y = RSHIFT( vy, 8+Dsp->accuracy - qpel );
}
#endif

#ifdef JZC_MXU_OPT
void Predict_1pt_16x16_C(const NEW_GMC_DATA * const This,
            						 uint8_t       *Dst, 
            						 const uint8_t *Src,
            						 
            						 int dststride, 
            						 int srcstride, 
            						 int x, 
            						 int y, 
            						 int rounding)

{
  const int W   = This->sW;
  const int H   = This->sH;
  const int rho = 3 - This->accuracy;
  const int32_t Rounder = ( 128 - (rounding << (2*rho)) ) << 16;

  int32_t uo  = This->Uo + (x<<8);
  int32_t vo  = This->Vo + (y<<8);
  uint32_t ri = MTab[uo & 15];
  uint32_t rj = MTab[vo & 15];
	int i, j;
  
  int32_t Offset;

  
  if(vo >= (-16*4) && vo <= H)
    Offset = (vo >> 4)*srcstride;
  else 
  {
    if(vo>H) 
      Offset = ( H >> 4)*srcstride;
    else 
      Offset = -16*srcstride;
    rj = MTab[0];
  }

  if (uo >=( -16*4) && uo<=W)
    Offset += (uo>>4);
  else 
  {
    if (uo > W) 
      Offset += ( W >> 4);
    else
      Offset -= 16;
    ri = MTab[0];
  }

	Dst += 16;
  {
    uint32_t tmpf;
    uint8_t *srctmp;
    uint8_t *dsttmp;

    S32I2M(xr15, Rounder);
    S32I2M(xr5, 0x0fff0000);
    
    for(j = 16; j>0; --j)
    {
      for(i = -16; i<0; ++i)
      {
        // f0
        srctmp = Src + Offset;
        S32LUI(xr1, 0x0, ptn7);
        S8LDD(xr1,srctmp, 0x0, ptn0);
        S8LDD(xr1,srctmp, 0x1, ptn2);
        tmpf = S32M2I(xr1);
        S32MULU(xr2, xr1, ri, tmpf);
        S32ALNI(xr1, xr0, xr1, ptn2); // >> 16
        //S32ALNI(xr2, xr0, xr2, ptn2);

        // f1
        srctmp += srcstride;
        S32LUI(xr3, 0x0, ptn7);
        S8LDD(xr3,srctmp, 0x0, ptn0);
        S8LDD(xr3,srctmp, 0x1, ptn2);
        tmpf = S32M2I(xr3);
        S32MULU(xr4, xr3, rj, tmpf);
        S32AND(xr3,xr3,xr5);

        // f0 |= f1;
        S32OR(xr1,xr1,xr3);

        //f0 = ( rj*f0 + Rounder ) >> 24;
        tmpf = S32M2I(xr1);
        S32MULU(xr2,xr1,rj,tmpf);
        D32ADD_AA(xr1,xr1,xr15,xr0);
        S32ALNI(xr1, xr0, xr1,ptn1); // >>24

        //  Dst[i]
        //dsttmp = &(Dst[i]);
        //S8STD(xr1, dsttmp, 0x0, ptn0)
        S8STD(xr1, &(Dst[i]), 0x0, ptn0);
        
        ++ Offset;
      }
      Offset += srcstride - 16;
      Dst    += dststride;
    }
  }
}
#else
/* ************************************************************
 * simplified version for 1 warp point
 */

void Predict_1pt_16x16_C(const NEW_GMC_DATA * const This,
						 uint8_t *Dst, const uint8_t *Src,
						 int dststride, int srcstride, int x, int y, int rounding)
{
  const int W	 = This->sW;
  const int H	 = This->sH;
  const int rho = 3-This->accuracy;
  const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;
  
  
  int32_t uo = This->Uo + (x<<8);	 /* ((16*x)<<4) */
  int32_t vo = This->Vo + (y<<8);
  uint32_t ri = MTab[uo & 15];
  uint32_t rj = MTab[vo & 15];
  int i, j;
  
  int32_t Offset;
  if (vo>=(-16*4) && vo<=H) 
    Offset = (vo>>4)*srcstride;
  else 
    {
      if (vo>H) 
	Offset = ( H>>4)*srcstride;
      else 
	Offset =-16*srcstride;
      rj = MTab[0];
    }
  if (uo>=(-16*4) && uo<=W) 
    Offset += (uo>>4);
  else 
    {
      if (uo>W) 
	Offset += (W>>4);
      else 
	Offset -= 16;
      ri = MTab[0];
    }
  
  Dst += 16;
  
  for(j=16; j>0; --j, Offset+=srcstride-16)
    {
      for(i=-16; i<0; ++i, ++Offset)
	{
	  uint32_t f0, f1;
	  f0	= Src[ Offset		+0 ];
	  f0 |= Src[ Offset		+1 ] << 16;
	  f1	= Src[ Offset+srcstride +0 ];
	  f1 |= Src[ Offset+srcstride +1 ] << 16;
	  f0 = (ri*f0)>>16;
	  f1 = (ri*f1) & 0x0fff0000;
	  f0 |= f1;
	  f0 = ( rj*f0 + Rounder ) >> 24;
	  Dst[i] = (uint8_t)f0;
	}
      Dst += dststride;
    }
}
#endif


#ifdef JZC_MXU_OPT
void Predict_1pt_8x8_C(const NEW_GMC_DATA * const This,
			 uint8_t *uDst, const uint8_t *uSrc,
			 uint8_t *vDst, const uint8_t *vSrc,
			 int dststride, int srcstride, int x, int y, int rounding)

{
  
  const int W	  = This->sW >> 1;
  const int H	  = This->sH >> 1;
  const int rho = 3 - This->accuracy;
  const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;

  int32_t uo   = This->Uco + (x<<7);
  int32_t vo   = This->Vco + (y<<7);
  uint32_t rri = MTab[uo & 15];
  uint32_t rrj = MTab[vo & 15];
  int32_t Offset;


  // get the Offset , rri, rrj
  if (vo>=(-8*4) && vo<=H) 
    Offset  = (vo>>4)*srcstride;
  else
  {
    if (vo>H)
      Offset = ( H>>4)*srcstride;
    else
      Offset =-8*srcstride;
    rrj = MTab[0];
  }

  //----------------------
  if (uo>=(-8*4) && uo<=W)
    Offset  += (uo>>4);
  else
  {
    if (uo>W)
      Offset += ( W>>4);
    else
      Offset -= 8;
    rri = MTab[0];
  }

  uDst += 8;
  vDst += 8;
  int i, j;
  {
    uint8_t *usrctmp;
    uint8_t *vsrctmp;
    uint8_t *dsttmp;
    uint32_t tmpf = 0;

    S32I2M(xr13, Rounder);
    S32I2M(xr15, 0x0fff0000);
    
    //uint32_t f0; xr1
    //uint32_t f1; xr3
    
    for(j = 8; j > 0; --j, Offset += (srcstride-8))
    {
      for(i=-8; i<0; ++i, Offset ++)
      {
        // u-------------------------------------
        usrctmp = uSrc + Offset;
        S32LUI(xr1, 0x0, ptn7);
        S8LDD(xr1, usrctmp, 0x0, ptn0);
        S8LDD(xr1, usrctmp, 0x1, ptn2);
        usrctmp += srcstride;
        S32LUI(xr3, 0x0, ptn7);
        S8LDD(xr3, usrctmp, 0x0, ptn0);
        S8LDD(xr3, usrctmp, 0x1, ptn2);

        // rri * f0
        tmpf = S32M2I(xr1);
        S32MULU(xr2, xr1, rri, tmpf);
        // rri * f1
        tmpf = S32M2I(xr3);
        S32MULU(xr4, xr3, rri, tmpf);

        // (rri*f0)>>16
	S32ALNI(xr1, xr0,xr1, ptn2);

        // (rri*f1) & 0x0fff0000
        S32AND(xr3, xr3, xr15);

        // f0 |= f1;
        S32OR(xr1,xr1,xr3);

        //f0 = ( rrj*f0 + Rounder ) >> 24
        tmpf = S32M2I(xr1);
        S32MULU(xr2, xr1, rrj, tmpf);
        D32ADD_AA(xr1, xr1 , xr13, xr2); // +Rounder

	S32ALNI(xr1,xr0,xr1, ptn1); // >>24

        // store
        S8STD(xr1, &(uDst[i]), 0x0, ptn0);

        // v-------------------------------------
        vsrctmp = vSrc + Offset;
        S32LUI(xr1, 0x0, ptn7);
        S8LDD(xr1, vsrctmp, 0x0, ptn0);
        S8LDD(xr1, vsrctmp, 0x1, ptn2);
        vsrctmp += srcstride;
        S32LUI(xr3, 0x0, ptn7);
        S8LDD(xr3, vsrctmp, 0x0, ptn0);
        S8LDD(xr3, vsrctmp, 0x1, ptn2);

        // rri * f0
        tmpf = S32M2I(xr1);
        S32MULU(xr2,xr1, rri, tmpf);
        // rri * f1
        tmpf = S32M2I(xr3);
        S32MULU(xr4,xr3, rri, tmpf);

        // (rri*f0)>>16
        S32ALNI(xr1, xr2,xr1, ptn2);
        S32ALNI(xr2, xr0,xr2, ptn2);

        // (rri*f1) & 0x0fff0000
        S32AND(xr3, xr3, xr15);

        // f0 |= f1;
        S32OR(xr1,xr1,xr3);

        //f0 = ( rrj*f0 + Rounder ) >> 24
        tmpf = S32M2I(xr1);
        S32MULU(xr2, xr1, rrj, tmpf);
        D32ADD_AA(xr1, xr1 , xr13, xr0);
        S32ALNI(xr1,xr0,xr1, ptn1); // >>24

        
        // store
        S8STD(xr1, &(vDst[i]), 0, ptn0);
      }
      uDst += dststride;
      vDst += dststride;

    }
  }

}

#else

void Predict_1pt_8x8_C(const NEW_GMC_DATA * const This,
						 uint8_t *uDst, const uint8_t *uSrc,
						 uint8_t *vDst, const uint8_t *vSrc,
						 int dststride, int srcstride, int x, int y, int rounding)
{
  const int W	 = This->sW >> 1;
  const int H	 = This->sH >> 1;
  const int rho = 3-This->accuracy;
  const int32_t Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;
  
  int32_t uo = This->Uco + (x<<7);
  int32_t vo = This->Vco + (y<<7);
  uint32_t rri = MTab[uo & 15];
  uint32_t rrj = MTab[vo & 15];
  int i, j;
  
  int32_t Offset;
  if (vo>=(-8*4) && vo<=H)
    Offset	= (vo>>4)*srcstride;
  else 
    {
      if (vo>H) 
	Offset = ( H>>4)*srcstride;
      else 
	Offset =-8*srcstride;
      rrj = MTab[0];
    }
  if (uo>=(-8*4) && uo<=W) 
    Offset	+= (uo>>4);
  else 
    {
      if (uo>W) 
	Offset += ( W>>4);
      else 
	Offset -= 8;
      rri = MTab[0];
    }
  
  uDst += 8;
  vDst += 8;
  for(j=8; j>0; --j, Offset+=srcstride-8)
    {
      for(i=-8; i<0; ++i, Offset++)
	{
	  uint32_t f0, f1;
	  f0	= uSrc[ Offset + 0 ];
	  f0 |= uSrc[ Offset + 1 ] << 16;
	  f1	= uSrc[ Offset + srcstride + 0 ];
	  f1 |= uSrc[ Offset + srcstride + 1 ] << 16;
	  f0 = (rri*f0)>>16;
	  f1 = (rri*f1) & 0x0fff0000;
	  f0 |= f1;
	  f0 = ( rrj*f0 + Rounder ) >> 24;
	  uDst[i] = (uint8_t)f0;
	  
	  f0	= vSrc[ Offset + 0 ];
	  f0 |= vSrc[ Offset + 1 ] << 16;
	  f1	= vSrc[ Offset + srcstride + 0 ];
	  f1 |= vSrc[ Offset + srcstride + 1 ] << 16;
	  f0 = (rri*f0)>>16;
	  f1 = (rri*f1) & 0x0fff0000;
	  f0 |= f1;
	  f0 = ( rrj*f0 + Rounder ) >> 24;
	  vDst[i] = (uint8_t)f0;
	}
      uDst += dststride;
      vDst += dststride;
    }
}
#endif



#ifdef JZC_MXU_OPT
void get_average_mv_1pt_C(const NEW_GMC_DATA * const Dsp, 
                          VECTOR * const mv,
                          int x, 
                          int y, 
                          int qpel)
{
  // load & left shift
  S32I2M(xr15, 4); 
  S32I2M(xr1, (Dsp->Uo));
  S32I2M(xr2, (Dsp->Vo));

  D32SLLV(xr1, xr2, qpel); // qpel < 8

  // if else
  S32SLT(xr3, xr1,xr0);
  S32SLT(xr4, xr2,xr0);
  // cal
  D32ASUM_SS(xr1,xr3,xr4,xr2);
  D32ASUM_AA(xr1,xr15,xr15,xr2);
  D32SAR(xr1, xr1, xr2, xr2, 3);

  // store
  mv->x = S32M2I(xr1);
  mv->y = S32M2I(xr2);
}

#else

void get_average_mv_1pt_C(const NEW_GMC_DATA * const Dsp, VECTOR * const mv,
							int x, int y, int qpel)
{
  mv->x = RSHIFT(Dsp->Uo<<qpel, 3);
  mv->y = RSHIFT(Dsp->Vo<<qpel, 3);
}
#endif


/* *************************************************************
 * Warning! It's Accuracy being passed, not 'resolution'!
 */

void generate_GMCparameters( int nb_pts, const int accuracy,
								 const WARPPOINTS *const pts,
								 const int width, const int height,
								 NEW_GMC_DATA *const gmc)
{
  gmc->sW = width  << 4;
  gmc->sH = height << 4;
  gmc->accuracy = accuracy;
  gmc->num_wp = nb_pts;
  
  /* reduce the number of points, if possible */
  if (nb_pts<2 || (pts->duv[2].x==0 && pts->duv[2].y==0 && pts->duv[1].x==0 && pts->duv[1].y==0 )) 
    {
      if (nb_pts<2 || (pts->duv[1].x==0 && pts->duv[1].y==0)) 
	{
	  if (nb_pts<1 || (pts->duv[0].x==0 && pts->duv[0].y==0)) 
	    {
	      nb_pts = 0;
	    }
	  else 
	    nb_pts = 1;
	}
      else 
	nb_pts = 2;
    }
  
  /* now, nb_pts stores the actual number of points required for interpolation */
  
  if (nb_pts<=1)
    {
      if (nb_pts==1) 
	{
	  /* store as 4b fixed point */
	  gmc->Uo = pts->duv[0].x << accuracy;
	  gmc->Vo = pts->duv[0].y << accuracy;
	  gmc->Uco = ((pts->duv[0].x>>1) | (pts->duv[0].x&1)) << accuracy;	 /* DIV2RND() */
	  gmc->Vco = ((pts->duv[0].y>>1) | (pts->duv[0].y&1)) << accuracy;	 /* DIV2RND() */
	}
      else 
	{	/* zero points?! */
	  gmc->Uo	= gmc->Vo	= 0;
	  gmc->Uco = gmc->Vco = 0;
	}
      
      gmc->predict_16x16	= Predict_1pt_16x16_C;
      gmc->predict_8x8	= Predict_1pt_8x8_C;
      gmc->get_average_mv = get_average_mv_1pt_C;
    }
  else 
    {		/* 2 or 3 points */
      const int rho	 = 3 - accuracy;	/* = {3,2,1,0} for Acc={0,1,2,3} */
      int Alpha = log2bin(width-1);
      int Ws = 1 << Alpha;

      gmc->dU[0] = 16*Ws + RDIV( 8*Ws*pts->duv[1].x, width );	 /* dU/dx */
      gmc->dV[0] =		 RDIV( 8*Ws*pts->duv[1].y, width );	 /* dV/dx */
      
      if (nb_pts==2) 
	{
	  gmc->dU[1] = -gmc->dV[0];	/* -Sin */
	  gmc->dV[1] =	gmc->dU[0] ;	/* Cos */
	}
      else
	{
	  const int Beta = log2bin(height-1);
	  const int Hs = 1<<Beta;
	  gmc->dU[1] =		 RDIV( 8*Hs*pts->duv[2].x, height );	 /* dU/dy */
	  gmc->dV[1] = 16*Hs + RDIV( 8*Hs*pts->duv[2].y, height );	 /* dV/dy */
	  if (Beta>Alpha) 
	    {
	      gmc->dU[0] <<= (Beta-Alpha);
	      gmc->dV[0] <<= (Beta-Alpha);
	      Alpha = Beta;
	      Ws = Hs;
	    }
	  else 
	    {
	      gmc->dU[1] <<= Alpha - Beta;
	      gmc->dV[1] <<= Alpha - Beta;
	    }
	}
      /* upscale to 16b fixed-point */
      gmc->dU[0] <<= (16-Alpha - rho);
      gmc->dU[1] <<= (16-Alpha - rho);
      gmc->dV[0] <<= (16-Alpha - rho);
      gmc->dV[1] <<= (16-Alpha - rho);
      
      gmc->Uo  = ( pts->duv[0].x   <<(16+ accuracy)) + (1<<15);
      gmc->Vo  = ( pts->duv[0].y   <<(16+ accuracy)) + (1<<15);
      gmc->Uco = ((pts->duv[0].x-1)<<(17+ accuracy)) + (1<<17);
      gmc->Vco = ((pts->duv[0].y-1)<<(17+ accuracy)) + (1<<17);
      gmc->Uco = (gmc->Uco + gmc->dU[0] + gmc->dU[1])>>2;
      gmc->Vco = (gmc->Vco + gmc->dV[0] + gmc->dV[1])>>2;
      
      gmc->predict_16x16	= Predict_16x16_C;
      gmc->predict_8x8	= Predict_8x8_C;
      gmc->get_average_mv = get_average_mv_C;


    }

}

/* *******************************************************************
 * quick and dirty routine to generate the full warped image
 * (pGMC != NULL) or just all average Motion Vectors (pGMC == NULL) */

void
generate_GMCimage(	const NEW_GMC_DATA *const gmc_data, /* [input] precalculated data */
			const IMAGE *const pRef,		/* [input] */
			const int mb_width,
			const int mb_height,
			const int stride,
			const int stride2,
			const int fcode, 				/* [input] some parameters... */
			const int32_t quarterpel,		/* [input] for rounding avgMV */
			const int reduced_resolution,	/* [input] ignored */
			const int32_t rounding,			/* [input] for rounding image data */
			MACROBLOCK *const pMBs, 		/* [output] average motion vectors */
			IMAGE *const pGMC)				/* [output] full warped image */
{
  
  unsigned int mj,mi;
  VECTOR avgMV;
  
  for (mj = 0; mj < (unsigned int)mb_height; mj++)
    {
      for (mi = 0; mi < (unsigned int)mb_width; mi++) 
	{
	  const int mbnum = mj*mb_width+mi;
	  if (pGMC)
	    {
	      gmc_data->predict_16x16(gmc_data,
				      pGMC->y + mj*16*stride + mi*16, pRef->y,
				      stride, stride, mi, mj, rounding);
	      
	      gmc_data->predict_8x8(gmc_data,
				    pGMC->u + mj*8*stride2 + mi*8, pRef->u,
				    pGMC->v + mj*8*stride2 + mi*8, pRef->v,
				    stride2, stride2, mi, mj, rounding);
	    }
	  gmc_data->get_average_mv(gmc_data, &avgMV, mi, mj, quarterpel);
	  
	  pMBs[mbnum].amv.x = gmc_sanitize(avgMV.x, quarterpel, fcode);
	  pMBs[mbnum].amv.y = gmc_sanitize(avgMV.y, quarterpel, fcode);
	  
	  pMBs[mbnum].mcsel = 0; /* until mode decision */
	}
    }
}
