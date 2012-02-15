
#include "../jz4750e_idct.h"

static void chroma_dc_dequant_idct_c(DCTELEM *block, int qmul){
    S16LDD(xr1,block,0, ptn1);
    S16LDD(xr1,block,32,ptn0);  //a,c
    S16LDD(xr2,block,64,ptn1);
    S16LDD(xr2,block,96,ptn0);  //b,d
                
    Q16ADD_AS_WW(xr3,xr1,xr2,xr4); //xr3=(a+b)a, (c+d)c xr4=(a-b)e,(c-d)b 
    S32SFL(xr1,xr3,xr4,xr2,ptn3); //xr1:a,e xr2=c,b
    Q16ADD_AS_WW(xr3,xr1,xr2,xr4);  //xr3:a+c, e+b, xr4=a-c,e-b
    int pp[2]= {0,0};
    short *ppt;
    ppt = (short*)pp;
    S16STD(xr3,ppt,0,ptn1);  //a+c
    S16STD(xr3,ppt,2,ptn0);  //e+b
    S16STD(xr4,ppt,4,ptn0);  //e-b
    S16STD(xr4,ppt,6,ptn1);  //a-c
                
    S32MUL(xr2,xr1,qmul,(int)ppt[0]); //(a+c)*qmul
    S32MUL(xr3,xr2,qmul,(int)ppt[1]); //(e+b)*qmul
    S32MUL(xr4,xr3,qmul,(int)ppt[2]); //(e-b)*qmul
    S32MUL(xr5,xr4,qmul,(int)ppt[3]); //(a-c)*qmul

    D32SAR(xr1,xr1,xr2,xr2,7);
    D32SAR(xr3,xr3,xr4,xr4,7);
    
    S16STD(xr1,block,0,ptn0);
    S16STD(xr2,block,64,ptn0);
    S16STD(xr3,block,96,ptn0);
    S16STD(xr4,block,32,ptn0);
}

__tcsm1_text
void luma_dc_dequant_idct_c(DCTELEM *block, int qmul){
    int i;
    int temp1[8];
    int *tp = temp1-2;
    DCTELEM *blk = block-32;
         
    for (i=0; i<4;i++) {
        if (i==2)
	  blk = blk + 96;
        else
          blk = blk + 32;
        S16LDD(xr1,blk,0,ptn1);
        S16LDD(xr1,blk,32,ptn0);  //xr1:b0,b16 
        S16LDD(xr3,blk,128,ptn1);
        S16LDD(xr3,blk,160,ptn0); //xr3:b64,b80

        Q16ADD_AS_WW(xr5,xr1,xr3,xr6);  //xr5:b0+b64(z0),b16+b80(z3)  xr6:b0-b64(z1),b16-b80(z2)
        S32SFL(xr7,xr5,xr6,xr8,ptn3);   //xr7:z0,z1, xr8:z3,z2
        Q16ADD_AS_WW(xr1,xr7,xr8,xr2); //xr1:z0+z3,z1+z2, xr2:z0-z3,z1-z2

        tp = tp + 2;
        S32ALNI(xr3,xr1,xr1,ptn2);
        S32STD(xr3,tp,0);
        S32STD(xr2,tp,4);
     }

     S32I2M(xr14,128);
     int pp[2]= {0,0};
     short *ppt;
     ppt = (short*)pp;
     tp = (int *)(((short*)temp1)-1);
     
     blk = block - 16;
     for (i=0; i<4; i++)
     {
        tp = (int *)(((short *)tp) + 1);
	S16LDD(xr1,tp,0,ptn1);
        S16LDD(xr1,tp,8,ptn0);  //xr1:t0,t4 
        S16LDD(xr2,tp,16,ptn1);  
        S16LDD(xr2,tp,24,ptn0); //xr2:t8,t12

        Q16ADD_AS_WW(xr3,xr1,xr2,xr4);  //xr3:t0+b8(z0),t4+t12(z3)  xr4:t0-t8(z1),t4-t12(z2)
        S32SFL(xr7,xr3,xr4,xr8,ptn3);   //xr7:z0,z1  xr8:z3,z2
        Q16ADD_AS_WW(xr1,xr7,xr8,xr2);  //xr1:z0+z3,z1+z2  xr2:z0-z3,z1-z2

        S16STD(xr1,ppt,0,ptn1);
        S16STD(xr1,ppt,2,ptn0);
        S16STD(xr2,ppt,4,ptn0);
        S16STD(xr2,ppt,6,ptn1);
                
        S32MUL(xr4,xr3,qmul,(int)ppt[0]);
        S32MUL(xr5,xr4,qmul,(int)ppt[1]);
        S32MUL(xr6,xr5,qmul,(int)ppt[2]);
        S32MUL(xr7,xr6,qmul,(int)ppt[3]);
        
        D32ADD_AA(xr1,xr3,xr14,xr1);    //xr1:(z0+z3)*qmul+128
        D32ADD_AA(xr2,xr4,xr14,xr2);
        D32ADD_AA(xr3,xr5,xr14,xr3);
        D32ADD_AA(xr4,xr6,xr14,xr4);

        D32SAR(xr1,xr1,xr2,xr2,8);
        D32SAR(xr3,xr3,xr4,xr4,8);
        
        if (i == 2)
	  blk = blk + 48;
        else
          blk = blk + 16;
        S16STD(xr1,blk,0,ptn0);
        S16STD(xr2,blk,64,ptn0);
        S16STD(xr3,blk,256,ptn0);
        S16STD(xr4,blk,320,ptn0);
    }
}

__tcsm1_text
void ff_h264_idct8_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i;
    S32I2M(xr1,32); 
    S16LDD(xr2,block,0,ptn3);  //xr2:b0,b0
    Q16ADD_AA_LW(xr3,xr1,xr2,xr4);  //xr3(b0+32,b0+32),xr4=xr3;
    Q16SAR(xr5,xr3,xr4,xr6,6);

    uint8_t *dst_mid = dst-stride;
    for(i=0; i<8;i++){
         D32SAR(xr8,xr5,xr6,xr9,0);  //xr8:dc,xr9:dc
         D32SAR(xr10,xr5,xr6,xr11,0);//xr10:dc, xr11:dc
	 S32LDIV(xr1, dst_mid, stride, 0); //d3,d2,d1,d0
         S32LDD(xr2, dst_mid, 4);    //d7,d6,d5,d4
         Q8ACCE_AA(xr8,xr1,xr0,xr9);
         Q16SAT(xr7,xr8,xr9);
         Q8ACCE_AA(xr10,xr2,xr0,xr11);
         Q16SAT(xr12,xr10,xr11);
         S32STD(xr7,dst_mid,0);
         S32STD(xr12,dst_mid,4);
    }
}

__tcsm1_text
void ff_h264_idct_add_c(uint8_t *dst, DCTELEM *block, int stride){
  
    int i;
    block[0] += 32;
    DCTELEM *blk = block - 4;
    uint8_t *dst_mid = dst - stride;
    
    for(i=0; i<4; i++){
        S32LDIV(xr1,blk,8,0);  //b1,b0
        S32LDD(xr2,blk,4);  //b3,b2
     
        S32SFL(xr5,xr1,xr2,xr6,ptn3); //xr5:b1,b3 xr6:b0,b2
        Q16SAR(xr3,xr5,xr0,xr0,1);  //xr3:b1>>1,b3>>1
        Q16ADD_AS_LW(xr7,xr1,xr6,xr8); //xr7:b0+b0,b0+b2(z0)  xr8:b0-b0,b0-b2(z1)
      
        Q16ADD_SS_HW(xr1,xr3,xr5,xr1);  //xr1:b1>>1-b1, b1>>1-b3(z2)
        Q16ADD_AA_HW(xr2,xr5,xr3,xr2);  //xr2:b1+b1>>1,b1+b3>>1(z3) 
        S32SFL(xr5,xr2,xr1,xr6,ptn3);  //xr6:z3,z2
        S32SFL(xr3,xr7,xr8,xr4,ptn3);  //xr4:z0,z1

        Q16ADD_AS_WW(xr1,xr4,xr6,xr2); //xr1:z0+z3,z1+z2 xr2:z0-z3,z1-z2

        S32ALNI(xr3,xr1,xr1,ptn2);      //xr3:z1+z2,z0+z3
        S32STD(xr3,blk,0); 
        S32STD(xr2,blk,4);
    }

    blk = block-1;
    for (i=0; i<4; i++)
    {   
        blk = blk + 1;
        S16LDD(xr1,blk,0,ptn0);
        S16LDD(xr1,blk,8,ptn1);   //b1,b0
        S16LDD(xr2,blk,16,ptn0);
        S16LDD(xr2,blk,24,ptn1);  //b3,b2

        S32SFL(xr5,xr1,xr2,xr6,ptn3); //xr5:b1,b3 xr6:b0,b2
        Q16SAR(xr3,xr5,xr0,xr0,1);  //xr3:b1>>1,b3>>1
        Q16ADD_AS_LW(xr7,xr1,xr6,xr8); //xr7:b0+b0,b0+b2(z0)  xr8:b0-b0,b0-b2(z1)
      
        Q16ADD_SS_HW(xr1,xr3,xr5,xr1);  //xr1:b1>>1-b1, b1>>1-b3(z2)
        Q16ADD_AA_HW(xr2,xr5,xr3,xr2);  //xr2:b1+b1>>1,b1+b3>>1(z3) 
        S32SFL(xr5,xr2,xr1,xr6,ptn3);  //xr6:z3,z2
        S32SFL(xr3,xr7,xr8,xr4,ptn3);  //xr4:z0,z1

        Q16ADD_AS_WW(xr1,xr4,xr6,xr2); //xr1:z0+z3,z1+z2 xr2:z0-z3,z1-z2

        Q16SAR(xr1,xr1,xr2,xr2,6);     //xr1:(z0+z3)>>6,(z1+z2)>>6  xr2:(z0-z3)>>6,(z1-z2)>>6

        S16STD(xr1,blk,0,ptn1);
        S16STD(xr1,blk,8,ptn0);
        S16STD(xr2,blk,16,ptn0);
        S16STD(xr2,blk,24,ptn1);
    }

    blk = block - 4;
    for (i=0; i<4;i++)
    {
        S32LDIV(xr2,blk,8,0); //b1,b0
        S32LDD(xr3,blk,4);   //b3,b2        
       	S32LDIV(xr1, dst_mid, stride, 0); //d3,d2,d1,d0
	Q8ACCE_AA(xr3,xr1,xr0,xr2);
        Q16SAT(xr4,xr3,xr2);
        S32STD(xr4,dst_mid,0);  
    }
}

__tcsm1_text
void ff_h264_idct_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i;
    
    S32I2M(xr1,32); 
    S16LDD(xr2,block,0,ptn3);  //xr2:b0,b0
    Q16ADD_AA_LW(xr3,xr1,xr2,xr4);  //xr3(b0+32,b0+32),xr4=xr3;
    Q16SAR(xr5,xr3,xr4,xr6,6);

    uint8_t *dst_mid = dst-stride;
    for(i=0; i<4;i++){
         D32SAR(xr8,xr5,xr6,xr9,0);  //xr8:dc,xr9:dc
         S32LDIV(xr1, dst_mid, stride, 0); //d3,d2,d1,d0
         Q8ACCE_AA(xr8,xr1,xr0,xr9);
         Q16SAT(xr7,xr8,xr9);
         S32STD(xr7,dst_mid,0);
         
    }
}

__tcsm1_text
void ff_h264_idct8_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i;
    block[0] += 32;
    DCTELEM *blk = block -8;
    uint8_t *dst_mid = dst-stride;

    for( i = 0; i < 8; i++ )
    {
        blk = blk + 8;
        S32LDD(xr1,blk,0);   //xr1:b1,b0
        S32LDD(xr2,blk,4);   //xr2:b3,b2
        S32LDD(xr3,blk,8);   //xr3:b5,b4
        S32LDD(xr4,blk,12);  //xr4:b7,b6
          
        S32SFL(xr5,xr1,xr3,xr6,ptn3); //xr5:b1,b5, xr6:b0,b4
        S32SFL(xr7,xr2,xr4,xr8,ptn3); //xr7:b3,b7, xr8:b2,b6
        Q16SAR(xr9,xr7,xr8,xr10,1);   //xr9:b3>>1,b7>>1, xr10:b2>>1,b6>>1

        Q16ADD_AS_HW(xr11,xr6,xr6,xr12); //xr11:b0+b0,b0+b4(a0), xr12:b0-b0,b0-b4(a2)
        Q16ADD_SS_HW(xr13,xr10,xr8,xr13); //xr13:b2>>1-b2, b2>>1-b6(a4);
        Q16ADD_AA_HW(xr14,xr8,xr10,xr14); //xr14:b2+b2>>1, b2+b6>>1(a6);

        S32SFL(xr2,xr11,xr12,xr1,ptn3); //xr1:a0,a2  xr2:no use
        S32SFL(xr4,xr14,xr13,xr3,ptn3); //xr3:a6,a4  xr4:no use  //xr11,xr12,xr13,xr14(no use)

        Q16ADD_AA_WW(xr2,xr7,xr9,xr2);  //xr2:b3+b3>>1,b7+b7>>1
        Q16ADD_SS_WW(xr4,xr5,xr2,xr4);  //xr4:b1-b3-b3>>1,b5-b7-b7>>1  
        Q16ADD_AS_XW(xr11,xr4,xr7,xr12); //xr11:b3+b5-b7-b7>>1,b1+b7-b3-b3>>1(a3)  xr12:-b3+b5-b7-b7>>1(a1),b1-b7-b3-b3>>1

        Q16SAR(xr2,xr5,xr0,xr0,1);   //xr2:b1>>1,b5>>1
        Q16ADD_AA_WW(xr4,xr5,xr2,xr4);  //xr4:b1+b1>>1,b5+b5>>1   //xr2:no use

        S32SFL(xr6,xr5,xr7,xr8,ptn3);  //xr6:b1,b3 xr8:b5,b7
        Q16ADD_AA_WW(xr2,xr8,xr4,xr2);  //xr2:b5+b1+b1>>1,b7+b5+b5>>1
        Q16ADD_AS_XW(xr4,xr2,xr6,xr8);  //xr4:b1+b7+b5+b5>>1, b5+b3+b1+b1>>1(a7)   xr8:-b1+b7+b5+b5>>1(a5),b5-b3+b1+b1>>1
      
        S32SFL(xr5,xr12,xr8,xr6,ptn3);  //xr5:a1,a5
        S32SFL(xr6,xr4,xr11,xr7,ptn3);  //xr7:a7,a3
        Q16SAR(xr6,xr5,xr7,xr8,2);     //xr6:a1>>2,a5>>2  xr8:a7>>2,a3>>2
      
        Q16ADD_AS_WW(xr11,xr8,xr5,xr12);  //xr11:a7>>2+a1(b1),a3>>2+a5  xr12:a7>>2-a1,a3>>2-a5(b5)
        Q16ADD_AS_WW(xr13,xr7,xr6,xr14); //xr13:a7+a1>>2,a3+a5>>2(b3), xr14:a7-a1>>2(b7),a3-a5>>2
        Q16ADD_AS_WW(xr2,xr1,xr3,xr4); //xr2:a0+a6(b0),a2+a4(b2)  xr4:a0-a6(b6),a2-a4(b4)

        S32SFL(xr1,xr14,xr11,xr3,ptn3); //xr1:b7,b1
        S32SFL(xr5,xr12,xr13,xr3,ptn3); //xr3:b5,b3
        S32SFL(xr7,xr1,xr3,xr5,ptn3); //xr7:b7,b5, xr5:b1,b3

        Q16ADD_AS_WW(xr11,xr2,xr7,xr12); //xr11:b0+b7,b2+b5  ,xr12:b0-b7,b2-b5
        Q16ADD_AS_WW(xr13,xr4,xr5,xr14); //xr13:b6+b1,b4+b3,  xr14:b6-b1,b4-b3

        S32ALNI(xr1,xr11,xr11,ptn2);    //xr1:b2+b5,b0+b7
        S32ALNI(xr2,xr14,xr14,ptn2);    //xr2:b4-b3,b6-b1
       
        S32STD(xr1,blk,0);
        S32STD(xr13,blk,4);
        S32STD(xr2,blk,8);
        S32STD(xr12,blk,12);
    }

    blk = block -1;
    for( i = 0; i < 8; i++ )
    {
        blk = blk+1;
        S16LDD(xr1,blk,0,ptn0); //b1,b0
        S16LDD(xr1,blk,16,ptn1);
        S16LDD(xr2,blk,32,ptn0);
        S16LDD(xr2,blk,48,ptn1); //b3,b2
        S16LDD(xr3,blk,64,ptn0);
        S16LDD(xr3,blk,80,ptn1); //b5,b4
        S16LDD(xr4,blk,96,ptn0);
        S16LDD(xr4,blk,112,ptn1);  //b7,b6

        S32SFL(xr5,xr1,xr3,xr6,ptn3); //xr5:b1,b5, xr6:b0,b4
        S32SFL(xr7,xr2,xr4,xr8,ptn3); //xr7:b3,b7, xr8:b2,b6
        Q16SAR(xr9,xr7,xr8,xr10,1);   //xr9:b3>>1,b7>>1, xr10:b2>>1,b6>>1

        Q16ADD_AS_HW(xr11,xr6,xr6,xr12); //xr11:b0+b0,b0+b4(a0), xr12:b0-b0,b0-b4(a2)
        Q16ADD_SS_HW(xr13,xr10,xr8,xr13); //xr13:b2>>1-b2, b2>>1-b6(a4);
        Q16ADD_AA_HW(xr14,xr8,xr10,xr14); //xr14:b2+b2>>1, b2+b6>>1(a6);

        S32SFL(xr2,xr11,xr12,xr1,ptn3); //xr1:a0,a2  xr2:no use
        S32SFL(xr4,xr14,xr13,xr3,ptn3); //xr3:a6,a4  xr4:no use  //xr11,xr12,xr13,xr14(no use)

        Q16ADD_AA_WW(xr2,xr7,xr9,xr2);  //xr2:b3+b3>>1,b7+b7>>1
        Q16ADD_SS_WW(xr4,xr5,xr2,xr4);  //xr4:b1-b3-b3>>1,b5-b7-b7>>1  
        Q16ADD_AS_XW(xr11,xr4,xr7,xr12); //xr11:b3+b5-b7-b7>>1,b1+b7-b3-b3>>1(a3)  xr12:-b3+b5-b7-b7>>1(a1),b1-b7-b3-b3>>1

        Q16SAR(xr2,xr5,xr0,xr0,1);   //xr2:b1>>1,b5>>1
        Q16ADD_AA_WW(xr4,xr5,xr2,xr4);  //xr4:b1+b1>>1,b5+b5>>1   //xr2:no use

        S32SFL(xr6,xr5,xr7,xr8,ptn3);  //xr6:b1,b3 xr8:b5,b7
        Q16ADD_AA_WW(xr2,xr8,xr4,xr2);  //xr2:b5+b1+b1>>1,b7+b5+b5>>1
        Q16ADD_AS_XW(xr4,xr2,xr6,xr8);  //xr4:b1+b7+b5+b5>>1, b5+b3+b1+b1>>1(a7)   xr8:-b1+b7+b5+b5>>1(a5),b5-b3+b1+b1>>1
           
        S32SFL(xr5,xr12,xr8,xr6,ptn3);  //xr5:a1,a5
        S32SFL(xr6,xr4,xr11,xr7,ptn3);  //xr7:a7,a3

        Q16SAR(xr6,xr5,xr7,xr8,2);     //xr6:a1>>2,a5>>2  xr8:a7>>2,a3>>2
      
        Q16ADD_AS_WW(xr11,xr8,xr5,xr12);  //xr11:a7>>2+a1(b1),a3>>2+a5  xr12:a7>>2-a1,a3>>2-a5(b5)
        Q16ADD_AS_WW(xr13,xr7,xr6,xr14); //xr13:a7+a1>>2,a3+a5>>2(b3), xr14:a7-a1>>2(b7),a3-a5>>2
 
        Q16ADD_AS_WW(xr2,xr1,xr3,xr4); //xr2:a0+a6(b0),a2+a4(b2)  xr4:a0-a6(b6),a2-a4(b4)

        S32SFL(xr1,xr14,xr11,xr3,ptn3); //xr1:b7,b1
        S32SFL(xr5,xr12,xr13,xr3,ptn3); //xr3:b5,b3
        S32SFL(xr7,xr1,xr3,xr5,ptn3); //xr7:b7,b5, xr5:b1,b3

        Q16ADD_AS_WW(xr11,xr2,xr7,xr12); //xr11:b0+b7,b2+b5  ,xr12:b0-b7,b2-b5
        Q16ADD_AS_WW(xr13,xr4,xr5,xr14); //xr13:b6+b1,b4+b3,  xr14:b6-b1,b4-b3
       
        Q16SAR(xr11,xr11,xr12,xr12,6);  //xr11:(b0+b7)>>6,(b2+b5)>>6   xr12:(b0-b7)>>6,(b2-b5)>>6
        Q16SAR(xr13,xr13,xr14,xr14,6);  //xr13:(b6+b1)>>6,(b4+b3)>>6   xr14:(b6-b1)>>6,(b4-b3)>>6
       
        S16STD(xr11,blk,0,ptn1);
        S16STD(xr11,blk,16,ptn0);
        S16STD(xr13,blk,32,ptn0);
        S16STD(xr13,blk,48,ptn1);
      
        S16STD(xr14,blk,64,ptn1);
        S16STD(xr14,blk,80,ptn0);
        S16STD(xr12,blk,96,ptn0);
        S16STD(xr12,blk,112,ptn1);
    }

    blk = block -8;
    for (i=0; i<8; i++)
    {
         blk = blk + 8;
         S32LDD(xr3,blk,0);      //b1,b0
         S32LDD(xr4,blk,4);      //b3,b2
         S32LDD(xr5,blk,8);
         S32LDD(xr6,blk,12);

         S32LDIV(xr1, dst_mid, stride, 0); //d3,d2,d1,d0
         S32LDD(xr2, dst_mid, 4);
         Q8ACCE_AA(xr4,xr1,xr0,xr3);
         Q16SAT(xr7,xr4,xr3);
         Q8ACCE_AA(xr6,xr2,xr0,xr5);
         Q16SAT(xr8,xr6,xr5);
         S32STD(xr7,dst_mid,0);
         S32STD(xr8,dst_mid,4);    
    }
}
