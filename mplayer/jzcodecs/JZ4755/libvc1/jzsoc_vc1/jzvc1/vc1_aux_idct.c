
#ifdef JZC_MXU_OPT
static void vc1_v_overlap_aux(uint8_t* src, int stride)
{   
    int i;    
    uint8_t *src1 = src - 2*stride;
    uint8_t *src2 = src1;
    uint8_t *src3 = src1;
    S32I2M(xr15,0x30004);
    S32I2M(xr14,0x40003);   

    for(i=0;i<2;i++){
      S32LDD(xr1,src1,0);               //xr1:src[-2*stride]+3(a) src[-2*stride]+2 src[-2*stride]+1 src[-2*stride]+0
      S32LDIV(xr2,src1,stride,0);       //xr2:src[-1*stride]+3(b) src[-1*stride]+2 src[-1*stride]+1 src[-1*stride]+0
      S32LDIV(xr3,src1,stride,0);       //xr3:src[0]+3{c}         src[0]+2         src[0]+1 src[0]+0
      S32LDIV(xr4,src1,stride,0);       //xr4:src[1]+3{d}         src[1]+2         src[1]+1 src[1]+0
      	   
      Q8ADDE_SS(xr5,xr1,xr4,xr6);       //xr5:a3-d3  a2-d2   xr6:a1-d1 a0-d0
      Q16SAR(xr7,xr5,xr6,xr8,0);        //xr7:a3-d3  a2-d2   xr8:a1-d1 a0-d0
      Q8ACCE_SS(xr5,xr2,xr3,xr6);       //xr5:a3-d3+b3-c3  a2-d2+b2-c2   xr6:a1-d1+b1-c1 a0-d0+b0-c0
     
      Q16ACC_AA(xr7,xr15,xr0,xr8);
      Q16ACC_AA(xr5,xr14,xr0,xr6);      
 
      Q16SAR(xr7,xr7,xr8,xr8,3);
      Q16SAR(xr5,xr5,xr6,xr6,3);
   
      Q8ADDE_AA(xr9,xr1,xr0,xr10);       //xr9:src[-2*stride]+3(a) src[-2*stride]+2 xr10:src[-2*stride]+1 src[-2*stride]+0
      Q16ADD_SS_WW(xr9,xr9,xr7,xr0);   //xr9:src[-2*stride]+3(a)-d1  src[-2*stride]+2 -d1
      Q16ADD_SS_WW(xr10,xr10,xr8,xr0); //xr10:src[-2*stride]+1 - d1  src[-2*stride]+0 -d1

      Q8ACCE_AA(xr7,xr4,xr0,xr8);      //xr7:src[1]+3{d}+d1         src[1]+2+d1    xr8: src[1]+1 +d1 src[1]+0+d1
      S32SFL(xr0,xr9,xr10,xr9,ptn1);
      S32SFL(xr0,xr7,xr8,xr7,ptn1);
      S32STD(xr9,src2,0);  
  
      Q8ADDE_AA(xr9,xr2,xr0,xr10); 
      Q16ADD_SS_WW(xr9,xr9,xr5,xr0);
      Q16ADD_SS_WW(xr10,xr10,xr6,xr0);
      Q8ACCE_AA(xr5,xr3,xr0,xr6);

      S32SFL(xr0,xr9,xr10,xr9,ptn1);
      S32SDIV(xr9,src2,stride,0);

      S32SFL(xr0,xr5,xr6,xr5,ptn1);
      S32SDIV(xr5,src2,stride,0);

      S32SDIV(xr7,src2,stride,0);
      src1 = src3 + 4;
      src2 = src3 + 4;       
    }
}
#else
static void vc1_v_overlap_aux(uint8_t* src, int stride)
{
    int i;
    int a, b, c, d;
    int d1, d2;
    int rnd = 1;
    for(i = 0; i < 8; i++) {
        a = src[-2*stride];
        b = src[-stride];
        c = src[0];
        d = src[stride];
        d1 = (a - d + 3 + rnd) >> 3;
        d2 = (a - d + b - c + 4 - rnd) >> 3;

        src[-2*stride] = a - d1;
        src[-stride] = b - d2;
        src[0] = c + d2;
        src[stride] = d + d1;
        src++;
        rnd = !rnd;
    }
}

#endif

/** Apply overlap transform to vertical edge
*/

#ifdef JZC_MXU_OPT
static void vc1_h_overlap_aux(uint8_t* src, int stride)
{
  int i;
  uint8_t *src1 = src - 2;  
  uint8_t *src2 = src1;
  S32I2M(xr15,0x40003);

  for(i=0; i<4; i++)  
    {
      S16LDD(xr1,src1,0,ptn0);     //xr1:src[-1] src[-2]
      S8LDD (xr1,src1,2,ptn3);       
      S8LDD (xr1,src1,3,ptn2);     //xr1:src[0] src[1] src[-1] src[-2]
    
      src1 += stride;

      S16LDD(xr2,src1,0,ptn0);
      S8LDD (xr2,src1,2,ptn3);
      S8LDD (xr2,src1,3,ptn2);    //xr2: src[0+stride] src[1+stride]  src[-1+stride] src[-2+stride]

      src1 += stride;
      
      S32SFL(xr3,xr1,xr2,xr4,ptn3);   //xr4:src[-1] src[-2]    src[-1+stride] src[-2+stride]
                                      //xr3:src[0]  src[1]     src[0+stride]  src[1+stride]
      Q8ADDE_SS(xr5,xr4,xr3,xr6);     //xr5: src[-1]-src[0](b-c)               src[-2]-src[1] (a-d)              
                                      //xr6: src[-1+stride]-src[0+stride](b-c) src[-2+stride]-src[1+stride](a-d)  
     
      S32SFL(xr5,xr5,xr6,xr6,ptn3);   //xr6:src[-2]-src[1] (a-d)      src[-2+stride]-src[1+stride](a-d)
                                      //xr5:src[-1]-src[0] (b-c)      src[-1+stride]-src[0+stride](b-c)
      Q16ADD_AA_WW(xr7,xr5,xr6,xr0);  //xr7:(a-d) +  (b-c)   (a-d) +  (b-c)(stride)
   
      S32ALNI(xr8,xr7,xr7,ptn2);      //xr8:(a-d) +  (b-c)(stride)   (a-d) +  (b-c)

      S32SFL(xr6,xr6,xr8,xr8,ptn3);   //xr6:src[-2]-src[1] (a-d)              (a-d) +  (b-c)(stride)
                                      //xr8:src[-2+stride]-src[1+stride](a-d) (a-d) +  (b-c)     
      
      Q16ADD_AA_HW(xr6,xr15,xr6,xr0);
      Q16ADD_AA_LW(xr8,xr15,xr8,xr0);

      Q16SAR(xr6,xr6,xr8,xr8,3);      //xr6:d1 d2(stride)  xr8:d1(stride) d2

      S32ALNI(xr8,xr8,xr8,ptn2);      //xr8:d2 d1(stride)
      S32SFL(xr9,xr8,xr6,xr10,ptn3);  //xr9:d2 d1  xr10:d1(stride) d2(stride)
      
      Q8ADDE_AA(xr11,xr1,xr0,xr12);      //xr11:src[0]  src[1]  xr12:src[-1] src[-2]

      Q16ADD_AS_WW(xr11,xr11,xr9,xr0);//xr11:src[0] + d2  src[1] + d1
      Q16ADD_SA_WW(xr12,xr12,xr9,xr0);//xr12:src[-1]- d2  src[-2] - d1
 
      S8STD (xr11,src2,2,ptn2);
      S8STD (xr11,src2,3,ptn0);

      S8STD (xr12,src2,0,ptn0);
      S8STD (xr12,src2,1,ptn2);
      src2 += stride;

      Q8ADDE_AA(xr11,xr2,xr0,xr12);      //xr11:src[0]  src[1]  xr12:src[-1] src[-2]
      Q16ADD_AS_XW(xr11,xr11,xr10,xr0);//xr11: src[1]  + d1  src[0] + d2
      Q16ADD_SA_XW(xr12,xr12,xr10,xr0);//xr12: src[-2] - d1  src[-1]- d2 
      S8STD (xr11,src2,2,ptn0);
      S8STD (xr11,src2,3,ptn2);
      S8STD (xr12,src2,0,ptn2);
      S8STD (xr12,src2,1,ptn0);
      src2 += stride;
    }  
}

#else
static void vc1_h_overlap_aux(uint8_t* src, int stride)
{
    int i;
    int a, b, c, d;
    int d1, d2;
    int rnd = 1;
 
    for(i = 0; i < 8; i++) {
        a = src[-2];
        b = src[-1];
        c = src[0];
        d = src[1];

        d1 = (a - d + 3 + rnd) >> 3;
        d2 = (a - d + b - c + 4 - rnd) >> 3;

        src[-2] = a - d1;
        src[-1] = b - d2;
        src[0] = c + d2;
        src[1] = d + d1;

        src += stride;
        rnd = !rnd;
    }
}
#endif
