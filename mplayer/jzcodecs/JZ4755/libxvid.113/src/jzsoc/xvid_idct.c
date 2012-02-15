#include "xvid_dcore.h"
#include "jzmedia.h"
uint32_t
dequant_h263_intra(int16_t * data,	uint8_t yuv_len,				
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{     
 	uint32_t i = 0; 
	
	S32LUI(xr9,1,0);
	S32I2M(xr1,quant);
	
	D32SLL(xr5,xr1,xr0,xr0,1);// quant_m_2

	/* quant_add  */
	S32AND(xr15,xr1,xr9);
	S32MOVN(xr2,xr15,xr1);
	D32ADD_SS(xr1,xr1,xr9,xr3);
	S32MOVZ(xr2,xr15,xr1);

	S32I2M(xr3,-2048);
	S32I2M(xr4,2047);

	/* part1 */
	//S32MUL(xr4,xr6,*data,dcscalar);
	S32MUL(xr0,xr6,(int32_t)data[0],dcscalar);
	D16MUL_WW(xr0,xr6,xr9,xr6);

	S32MIN(xr6,xr6,xr4);
	S32MAX(xr6,xr6,xr3);  

	/* part2 */
	data-=2;    
	for (i = 0; i < yuv_len*4; i++) {
	    S32LDI(xr1,data,4);
      
	    D16MUL_LW(xr13,xr9,xr1,xr14);// resave sign of data[i] and data[i+1] 	    
	    D16CPS(xr1,xr1,xr1); 

	    /*  quant_m_2 * acLevel + quant_add */
	    D16MUL_LW(xr7,xr5,xr1,xr8);
            D32ADD_AA(xr7,xr7,xr2,xr0);
	    D32ADD_AA(xr8,xr8,xr2,xr0);
	   
	    /* -2048 < data[i+1] <2047  */
            S32CPS(xr7,xr7,xr13);
	    S32MAX(xr10,xr7,xr3);
	    S32MIN(xr10,xr10,xr4);
	    S32MOVZ(xr10,xr13,xr13);
	    
	    /* -2048 < data[i] <2047  */
	    S32CPS(xr8,xr8,xr14);
	    S32MAX(xr11,xr8,xr3);
	    S32MIN(xr11,xr11,xr4);
	    S32MOVZ(xr11,xr14,xr14);
	   
            S32SFL(xr0,xr10,xr11,xr12,3);
	   
	       S32STD(xr12,data,0);
        }  
	S16STD(xr6,data-(yuv_len*8-2),0,0);// data[0]

	return(0);
}

uint32_t
dequant_mpeg_intra(int16_t * data,
		                         uint8_t yuv_len,
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{
	const uint16_t *intra_matrix = mpeg_quant_matrices;
	int32_t i = 0;
	/* deal with data[0] then save to xr6  */
	
	S32I2M(xr3,-2048);
	S32I2M(xr4,2047);
    	S32I2M(xr5,quant);

	S32MUL(xr0,xr6,(int32_t)data[0],dcscalar);
	S32LUI(xr9,1,0);
	D16MUL_WW(xr0,xr6,xr9,xr6);
	S32MIN(xr6,xr6,xr4);
	S32MAX(xr6,xr6,xr3);

       	data-=2;
	intra_matrix-=2;
	

	for (i = 0; i < yuv_len*4; i++) {
	    S32LDI(xr1,data,4);
	    S32LDI(xr2,intra_matrix,4);

	    D16MUL_LW(xr13,xr9,xr1,xr14); // resave values of data[i] and data[i+1] 
	    D16CPS(xr1,xr1,xr1);         

	    /* abs(level) *( intra_matrix[i]*quant) >> 3   */
	    D16MUL_LW(xr7,xr5,xr2,xr8);
	    S32SFL(xr15,xr7,xr8,xr2,3);
	    D16MUL_WW(xr7,xr1,xr2,xr8);
	    D32SLR(xr7,xr7,xr8,xr8,3); 
	   
	    /* -2048 < data[i+1] < 2047  */
	    S32CPS(xr7,xr7,xr13);
	    S32MAX(xr10,xr7,xr3);
	    S32MIN(xr10,xr10,xr4);

            /* -2048 < data[i] < 2047  */
	    S32CPS(xr8,xr8,xr14);
	    S32MAX(xr11,xr8,xr3);
	    S32MIN(xr11,xr11,xr4);

            S32SFL(xr0,xr10,xr11,xr12,3);
	    S32STD(xr12,data,0);	    
        } 
 
	S16STD(xr6,data-(yuv_len*8-2),0,0);//xr6 to data[0]
	return(0);
}
/*idct*/

#define  wxr5  0x5A827642
#define  wxr6  0x5A8230FC
#define  wxr7  0x7D876A6E
#define  wxr8  0x18F9471D
#define  wxr9  0x6A6E18F9
#define  wxr10  0x471D7D87

void
idct(short *const block, short *const idctdst,const uint8_t yuv_len)
{
  //int xr1, xr2, xr3, xr4, xr5, xr6, xr7, xr8;
  //int xr9, xr10, xr11, xr12, xr13, xr14, xr15;
  short *blk, *dst;
  unsigned int i;// mid0, mid1, tmp0, tmp1;
  
  S32I2M(xr5,wxr5) ;         // xr5(w7, w3)
  S32I2M(xr6,wxr6) ;         // xr6(w9, w8)
  S32I2M(xr7,wxr7) ;         // xr7(w11,w10)
  S32I2M(xr8,wxr8) ;         // xr8(w13,w12)
  S32I2M(xr9,wxr9) ;         // xr9(w6, w0)  
  S32I2M(xr10,wxr10);       


  blk = block - 8;
  dst = idctdst - 8;
  for (i = 0; i < yuv_len; i++)	/* idct rows */
    {
      //  int hi_b, lo_b, hi_c, lo_c;
       blk += 8;
       dst += 8;
      S32LDD(xr1, blk, 0x0);        //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  
      
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  
      
      D16MUL_WW(xr11, xr2, xr5, xr12);         
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);        
      D16MUL_WW(xr13, xr2, xr6, xr14);         
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);        
      D16MUL_HW(xr2, xr1, xr7, xr4);         
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);        
      D16MAC_AS_HW(xr2,xr3,xr10,xr4);        
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);        

      D16MACF_AA_WW(xr2, xr0, xr0, xr4); 
      D16MACF_AA_WW(xr11, xr0, xr0, xr13);       
      D16MACF_AA_WW(xr12, xr0, xr0, xr14); 

      D16MUL_HW(xr4, xr1, xr8, xr15);         
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);        
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);        
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);        
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);

      D16MACF_AA_WW(xr15, xr0, xr0, xr4); 
      Q16ADD_AS_WW(xr11, xr11, xr2, xr2);    
      Q16ADD_AS_XW(xr12, xr12, xr15, xr15);       


       S32SFL(xr11,xr11,xr12,xr12, ptn3);
      
      S32SFL(xr12,xr12,xr11,xr11, ptn3);
      
      S32STD(xr12, dst, 0x0);
      S32STD(xr11, dst, 0x4); 
      S32STD(xr15, dst, 0x8); 
      S32STD(xr2, dst, 0xc);       
    }

  for(i = yuv_len*8;i < 64;i++)
     idctdst[i] = 0;

  blk = idctdst - 2;
  //dst = idctdst - 2;
  for (i = 0; i < 4; i++)		/* idct columns */
    { 
       blk += 2;
       //  dst += 2;
      S32LDD(xr1, blk, 0x00);
      //S32LDI(xr1, blk, 0x04);
      S32LDD(xr3, blk, 0x20);
      S32I2M(xr5,wxr5);
      S32LDD(xr11, blk, 0x40);
      S32LDD(xr13, blk, 0x60);
       
      D16MUL_HW(xr15, xr5, xr1, xr9);//1
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);  //2      

      D16MACF_AA_WW(xr15, xr0, xr0, xr9); //3

      D16MUL_LW(xr10, xr5, xr3, xr9);        //4 
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);        //5

      D16MACF_AA_WW(xr10, xr0, xr0, xr9); //6

      S32LDD(xr2, blk, 0x10);
      S32LDD(xr4, blk, 0x30);
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);//7

      D16MUL_HW(xr10, xr5, xr1, xr1);     //8    
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);       //9 
      D16MACF_AA_WW(xr10, xr0, xr0, xr1); //10

      D16MUL_LW(xr11, xr6, xr3, xr1);        //11
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);        //12

      D16MACF_AA_WW(xr11, xr0, xr0, xr1); //13

      S32LDD(xr12, blk, 0x50);
      S32LDD(xr14, blk, 0x70);
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);//14
      D16MUL_HW(xr11, xr7, xr2, xr13);     //11    
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);        //12
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);        
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);        //14

      D16MACF_AA_WW(xr11, xr0, xr0, xr13); //15

      D16MUL_LW(xr3, xr7, xr2, xr13);         
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);        
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);        
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);        
      
      D16MACF_AA_WW(xr3, xr0, xr0, xr13); 

      D16MUL_LW(xr5, xr8, xr2, xr13);         
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);        
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);        
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr5, xr0, xr0, xr13); 

      D16MUL_HW(xr2, xr8, xr2, xr13);         
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);        
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);        
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr2, xr0, xr0, xr13); 

      Q16ADD_AS_WW(xr15,xr15,xr11,xr11);    
      Q16ADD_AS_WW(xr10,xr10,xr3,xr3);    
      Q16ADD_AS_WW(xr1,xr1,xr5,xr5);          
      Q16ADD_AS_WW(xr9,xr9,xr2,xr2);      

      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10); 
      S32STD(xr1, blk, 0x20); 
      S32STD(xr9, blk, 0x30); 
      S32STD(xr2, blk, 0x40);      
      S32STD(xr5, blk, 0x50); 
      S32STD(xr3, blk, 0x60); 
      S32STD(xr11, blk, 0x70);
    }
 
 
}

/*
 * SRC - the source buffer
 * BLK - the destination buffer
 *
 * Then the function does the 8->16 bit transfer and this serie of operations :
 *
 *    SRC (16bit) = SRC
 *    DST (8bit)  = max(min(SRC, 255), 0)
 */
void 
transfer_16to8copy_mxu(uint8_t * const dst,
		       const int16_t * const src,
		       uint32_t stride)
{
  int32_t *src_data = (int32_t *)src - 1;
  uint8_t *dst_mid = dst - stride;
  uint32_t i;
  
  for (i = 0; i < 8; i++) {
    S32LDI(xr1,src_data,4);
    S32LDI(xr2,src_data,4);
    S32LDI(xr3,src_data,4);
    S32LDI(xr4,src_data,4);

    Q16SAT(xr5,xr2,xr1);
    Q16SAT(xr6,xr4,xr3);
    S32SDIV(xr5,dst_mid,stride,0);
    S32STD(xr6,dst_mid,0x4);
  }
}

/*
 * SRC - the source buffer
 * DST - the destination buffer
 *
 * Then the function does the 16->8 bit transfer and this serie of operations :
 *
 *    SRC (16bit) = SRC
 *    DST (8bit)  = max(min(DST+SRC, 255), 0)
 */
void
transfer_16to8add_mxu(uint8_t * const dst,
		      const int16_t * const src,
		      uint32_t stride)
{
  int32_t *src_data = (int32_t *)src - 1;
  uint8_t *dst_mid = dst - stride;        
  uint32_t i;
  
  for (i = 0; i < 8; i++) {
    S32LDIV(xr5,dst_mid,stride,0);
    S32LDI(xr1,src_data,4);
    S32LDI(xr2,src_data,4);
    Q8ACCE_AA(xr2, xr5, xr0, xr1);

    S32LDD(xr6,dst_mid,4);
    S32LDI(xr3,src_data,4);
    S32LDI(xr4,src_data,4);
    Q8ACCE_AA(xr4, xr6, xr0, xr3);

    Q16SAT(xr5, xr2, xr1);
    S32STD(xr5,dst_mid,0);
    Q16SAT(xr6, xr4, xr3);    
    S32STD(xr6,dst_mid,4);
  }
}
