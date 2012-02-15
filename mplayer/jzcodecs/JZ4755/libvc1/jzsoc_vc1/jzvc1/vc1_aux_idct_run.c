

static void add_pixels_clamped_aux(const DCTELEM *block, uint8_t *restrict pixels,
                          int line_size)
{
  int i;
  DCTELEM *b = block - 2;
  
  for(i=0;i<8;i++) {
    S32LDD(xr1,pixels,0);   // xr1 <- pixels[3,2,1,0]
    S32LDI(xr2,b,4);
    S32LDI(xr3,b,4);
    
    Q8ADDE_AA(xr4,xr1,xr0,xr5);  //xr4<-sign_ext16(pixels[3], pixels[2]),xr5 <- sign_ext16(pixels[1], pixels[0])
    Q16ACCM_AA(xr4,xr3,xr2,xr5);//xr4<-(pixels[3]+block[3],pixels[2]+block[2]),xr5<-(pixels[1]+block[1],pixels[0]+block[0])
    Q16SAT(xr6,xr4,xr5);//xr6<-sat8(pixels[3]+block[3]),sat8(pixels[2]+block[2]),sat8(pixels[1]+block[1]),sat8(pixels[0]+block[0]);
    S32STD(xr6,pixels,0);  // xr6 -> pixels[3,2,1,0]

    S32LDD(xr1,pixels,4);   // xr1 <- pixels[7,6,5,4]
    S32LDI(xr2,b,4);
    S32LDI(xr3,b,4);
    
    Q8ADDE_AA(xr4,xr1,xr0,xr5);  // xr4 <- sign_ext16(pixels[7], pixels[6]),xr5 <- sign_ext16(pixels[5], pixels[4])
    Q16ACCM_AA(xr4,xr3,xr2,xr5);  // xr4 <- (pixels[7]+block[7],pixels[6]+block[6]), xr5 <- (pixels[5]+block[5],pixels[4]+block[4])
    Q16SAT(xr6,xr4,xr5);   // xr6 <- sat8(pixels[7]+block[7]),sat8(pixels[6]+block[6]),sat8(pixels[5]+block[5]),sat8(pixels[4]+block[4]);
    S32STD(xr6,pixels,4);  // xr6 -> pixels[7,6,5,4]
    pixels += line_size;
  }
}

static void put_pixels_clamped_aux(const DCTELEM *block, uint8_t *restrict pixels,
                                 int line_size)
{
  int i;
  
  for(i=0;i<8;i++) {
    S32LDD(xr2,block,0);
    S32LDD(xr3,block,4);
    
    Q16SAT(xr6,xr3,xr2);   // xr6 <- sat8(block[3]),sat8(block[2]),sat8(block[1]),sat8(block[0]);
    S32STD(xr6,pixels,0);  // xr6 -> pixels[3,2,1,0]
    
    S32LDD(xr2,block,8);
    S32LDD(xr3,block,12);
    
    Q16SAT(xr6,xr3,xr2);   // xr6 <- sat8(block[7]),sat8(block[6]),sat8(block[5]),sat8(block[4]);
    S32STD(xr6,pixels,4);  // xr6 -> pixels[7,6,5,4]
    
    pixels += line_size;
    block += 8;
  }
}
      

/** Do inverse transform on 8x8 block
*/

static void  vc1_inv_trans_8x8_aux(DCTELEM block[64])
{
    int i;
    DCTELEM *src, *dst;
    src = block;
    dst = block;
    S32I2M(xr7,0x10000F);
    S32I2M(xr8,0x40009);
    S32I2M(xr9,0xF0004);
    S32I2M(xr10,0x90010);
    S32I2M(xr15,4);
   
    for (i = 0; i < 8; i++){
        S32I2M(xr5,0xC0010);
	S32I2M(xr6,0xC0006);

	S32LDD(xr1,src,0x0);        //s1,s0
        S32LDD(xr2,src,0x4);            //s3,s2
        S32LDD(xr3,src,0x8);            //s5,s4,
        S32LDD(xr4,src,0xC);            //s7,s6

        S32SFL(xr1,xr1,xr2,xr2,ptn3);   //xr1:s1,s3, xr2:s0,s2
        S32SFL(xr3,xr3,xr4,xr4,ptn3);   //xr3:s5,s7, xr4:s4,s6

        D16MUL_WW(xr11,xr2,xr5,xr12);       //xr11:12s0,xr12:16s2
	D16MAC_AA_WW(xr11,xr4,xr6,xr12);    //xr11:12s0+12s4(t1), xr12:16s2+6s6(t3)
                
        D16MUL_WW(xr13,xr2,xr6,xr14);       //xr13:12s0, xr14:6s2
	D16MAC_SS_WW(xr13,xr4,xr5,xr14);    //xr13:12s0-12s4(t2), xr14:6s2-16s6(t4)
        
        D32ADD_AS(xr2,xr11,xr12,xr4);       //xr2:t1+t3(t5)  xr4:t1-t3(t8)
        D32ADD_AS(xr11,xr13,xr14,xr12);     //xr11:t2+t4(t6) xr12:t2-t4(t7)  //xr5,6,13,14 no use
    
        D16MUL_HW(xr13,xr1,xr7,xr14);         //xr13:16s1,xr14:15s1
        D16MAC_AS_LW(xr13,xr1,xr9,xr14);      //xr13:16s1+15s3 xr14:15s1-4s3
        
        D16MUL_HW(xr5,xr3,xr10,xr6);          //xr5:9s5 xr6:16s5
        D16MAC_AA_LW(xr5,xr3,xr8,xr6);         //xr5:9s5+4s7  xr6:16s5+9s7

        D32ADD_AS(xr13,xr13,xr5,xr0);         //xr13:16s1+15s3+9s5+4s7(t1)
        D32ADD_AS(xr0,xr14,xr6,xr14);         //xr14:15s1-4s3-16s5-9s7(t2)
 
        D32ADD_AS(xr2,xr2,xr13,xr13);        //xr2:t5+t1  xr13:t5-t1 
        D32ADD_AS(xr11,xr11,xr14,xr14);      //xr11:t6+t2, xr14:t6-t2

        D32ADD_AA(xr2,xr2,xr15,xr0);        //xr2:t5+t1+4;
        D32ADD_AA(xr13,xr13,xr15,xr0);      //xr13:t5-t1+4
	D32ADD_AA(xr11,xr11,xr15,xr0);      //xr11:t6+t2+4
	D32ADD_AA(xr14,xr14,xr15,xr0);      //xr14:t6-t2+4

        D32SARL(xr5,xr11,xr2,3);
        D32SARL(xr6,xr13,xr14,3);
        S32STD(xr5,dst,0x0);
        S32STD(xr6,dst,0xC);             //

	D16MUL_HW(xr5,xr1,xr8,xr6);      //xr5:4s1 xr6:9s1
        D16MAC_SS_LW(xr5,xr1,xr10,xr6);  //xr5:4s1-9s3  xr6:9s1-16s3
        D16MUL_HW(xr13,xr3,xr9,xr14);    //xr13:15s5  xr14:4s5
        D16MAC_SA_LW(xr13,xr3,xr7,xr14); //xr13:15s5-16s7  xr14:4s5+15s7 

        D32ADD_AS(xr1,xr5,xr13,xr0); //xr1:4s1-9s3+15s5-16s7(t4) 
        D32ADD_AS(xr2,xr6,xr14,xr0); //xr2:9s1-16s3+4s5+15s7(t3)

	D32ADD_AS(xr4,xr4,xr1,xr1);   //xr4:t8+t4  xr1:t8-t4
	D32ADD_AS(xr12,xr12,xr2,xr2);   //xr12:t7+t3  xr2:t7-t3

        D32ADD_AA(xr4,xr4,xr15,xr0);   //xr4:t8+t4+4          
        D32ADD_AA(xr1,xr1,xr15,xr0);   //xr1:t8-t4+4 
        D32ADD_AA(xr12,xr12,xr15,xr0); //xr12:t7+t3+4
        D32ADD_AA(xr2,xr2,xr15,xr0);   //xr2:t7-t3+4
       
        D32SARL(xr14,xr4,xr12,3);
        D32SARL(xr3,xr2,xr1,3);
        S32STD(xr14,dst,0x4);
        S32STD(xr3,dst,0x8);
        src += 8;
        dst += 8;
    }
    
    src = block;
    dst = block;
    
    for(i = 0; i < 8; i++){
        S32I2M(xr5,0xC0010);
	S32I2M(xr6,0xC0006);
        S32I2M(xr15,64);
        
        S16LDD(xr1,src,16,ptn1);     //xr1:s8,s24   
	S16LDD(xr1,src,48,ptn0);
	S16LDD(xr2,src,0,ptn1);      //xr2:s0,s16
	S16LDD(xr2,src,32,ptn0);
	S16LDD(xr3,src,80,ptn1);     //xr3:s40,s56 
	S16LDD(xr3,src,112,ptn0);
	S16LDD(xr4,src,64,ptn1);     //xr4:s32,s48
	S16LDD(xr4,src,96,ptn0);

        D16MUL_WW(xr11,xr2,xr5,xr12);       //xr11:12s0,xr12:16s2
	D16MAC_AA_WW(xr11,xr4,xr6,xr12);    //xr11:12s0+12s4(t1), xr12:16s2+6s6(t3)
                
        D16MUL_WW(xr13,xr2,xr6,xr14);       //xr13:12s0, xr14:6s2
	D16MAC_SS_WW(xr13,xr4,xr5,xr14);    //xr13:12s0-12s4(t2), xr14:6s2-16s6(t4)
        
        D32ADD_AS(xr2,xr11,xr12,xr4);       //xr2:t1+t3(t5)  xr4:t1-t3(t8)
        D32ADD_AS(xr11,xr13,xr14,xr12);     //xr11:t2+t4(t6) xr12:t2-t4(t7)  //xr5,6,13,14 no use
    
        D16MUL_HW(xr13,xr1,xr7,xr14);         //xr13:16s1,xr14:15s1
        D16MAC_AS_LW(xr13,xr1,xr9,xr14);      //xr13:16s1+15s3 xr14:15s1-4s3
        
        D16MUL_HW(xr5,xr3,xr10,xr6);          //xr5:9s5 xr6:16s5
        D16MAC_AA_LW(xr5,xr3,xr8,xr6);         //xr5:9s5+4s7  xr6:16s5+9s7

        D32ADD_AS(xr13,xr13,xr5,xr0);         //xr13:16s1+15s3+9s5+4s7(t1)
        D32ADD_AS(xr0,xr14,xr6,xr14);         //xr14:15s1-4s3-16s5-9s7(t2)
 
        D32ADD_AS(xr2,xr2,xr13,xr13);        //xr2:t5+t1  xr13:t5-t1 
        D32ADD_AS(xr11,xr11,xr14,xr14);      //xr11:t6+t2, xr14:t6-t2

        D32ADD_AA(xr2,xr2,xr15,xr0);        //xr2:t5+t1+64;
        D32ADD_AA(xr11,xr11,xr15,xr0);      //xr11:t6+t2+64
        S32I2M(xr15,65);
        D32ADD_AA(xr13,xr13,xr15,xr0);      //xr13:t5-t1+65
	D32ADD_AA(xr14,xr14,xr15,xr0);      //xr14:t6-t2+65
        
        D32SAR(xr2,xr2,xr11,xr11,7); 
        D32SAR(xr13,xr13,xr14,xr14,7);
   
        S16STD(xr2,dst,0,ptn0);
	S16STD(xr11,dst,16,ptn0);               
	S16STD(xr14,dst,96,ptn0);
	S16STD(xr13,dst,112,ptn0);

        D16MUL_HW(xr5,xr1,xr8,xr6);      //xr5:4s1 xr6:9s1
        D16MAC_SS_LW(xr5,xr1,xr10,xr6);  //xr5:4s1-9s3  xr6:9s1-16s3
        D16MUL_HW(xr13,xr3,xr9,xr14);    //xr13:15s5  xr14:4s5
        D16MAC_SA_LW(xr13,xr3,xr7,xr14); //xr13:15s5-16s7  xr14:4s5+15s7 

        D32ADD_AS(xr1,xr5,xr13,xr0); //xr1:4s1-9s3+15s5-16s7(t4) 
        D32ADD_AS(xr2,xr6,xr14,xr0); //xr2:9s1-16s3+4s5+15s7(t3)

	D32ADD_AS(xr4,xr4,xr1,xr1);   //xr4:t8+t4  xr1:t8-t4
	D32ADD_AS(xr12,xr12,xr2,xr2);   //xr12:t7+t3  xr2:t7-t3

	D32ADD_AA(xr2,xr2,xr15,xr0);   //xr2:t7-t3+65
        D32ADD_AA(xr1,xr1,xr15,xr0);   //xr1:t8-t4+65
        S32I2M(xr15,64); 
        D32ADD_AA(xr12,xr12,xr15,xr0); //xr12:t7+t3+64
	D32ADD_AA(xr4,xr4,xr15,xr0);   //xr4:t8+t4+64 

        D32SAR(xr2,xr2,xr1,xr1,7); 
        D32SAR(xr12,xr12,xr4,xr4,7);
         
	S16STD(xr12,dst,32,ptn0);
	S16STD(xr4,dst,48,ptn0);               
	S16STD(xr1,dst,64,ptn0);
	S16STD(xr2,dst,80,ptn0);

        src++;
        dst++;
    }
}

/** Do inverse transform on 8x4 part of block
*/
static void vc1_inv_trans_8x4_aux(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;
    off = n * 32;
    src = block + off;
    dst = block + off;

    S32I2M(xr7,0x10000F);
    S32I2M(xr8,0x40009);
    S32I2M(xr9,0xF0004);
    S32I2M(xr10,0x90010);
    S32I2M(xr15,4);
   
    for (i = 0; i < 4; i++){

        S32I2M(xr5,0xC0010);
	S32I2M(xr6,0xC0006);

	S32LDD(xr1,src,0x0);        //s1,s0
        S32LDD(xr2,src,0x4);            //s3,s2
        S32LDD(xr3,src,0x8);            //s5,s4,
        S32LDD(xr4,src,0xC);            //s7,s6

        S32SFL(xr1,xr1,xr2,xr2,ptn3);   //xr1:s1,s3, xr2:s0,s2
        S32SFL(xr3,xr3,xr4,xr4,ptn3);   //xr3:s5,s7, xr4:s4,s6

        D16MUL_WW(xr11,xr2,xr5,xr12);       //xr11:12s0,xr12:16s2
	D16MAC_AA_WW(xr11,xr4,xr6,xr12);    //xr11:12s0+12s4(t1), xr12:16s2+6s6(t3)
                
        D16MUL_WW(xr13,xr2,xr6,xr14);       //xr13:12s0, xr14:6s2
	D16MAC_SS_WW(xr13,xr4,xr5,xr14);    //xr13:12s0-12s4(t2), xr14:6s2-16s6(t4)
        
        D32ADD_AS(xr2,xr11,xr12,xr4);       //xr2:t1+t3(t5)  xr4:t1-t3(t8)
        D32ADD_AS(xr11,xr13,xr14,xr12);     //xr11:t2+t4(t6) xr12:t2-t4(t7)  //xr5,6,13,14 no use
    
        D16MUL_HW(xr13,xr1,xr7,xr14);         //xr13:16s1,xr14:15s1
        D16MAC_AS_LW(xr13,xr1,xr9,xr14);      //xr13:16s1+15s3 xr14:15s1-4s3
        
        D16MUL_HW(xr5,xr3,xr10,xr6);          //xr5:9s5 xr6:16s5
        D16MAC_AA_LW(xr5,xr3,xr8,xr6);         //xr5:9s5+4s7  xr6:16s5+9s7

        D32ADD_AS(xr13,xr13,xr5,xr0);         //xr13:16s1+15s3+9s5+4s7(t1)
        D32ADD_AS(xr0,xr14,xr6,xr14);         //xr14:15s1-4s3-16s5-9s7(t2)
 
        D32ADD_AS(xr2,xr2,xr13,xr13);        //xr2:t5+t1  xr13:t5-t1 
        D32ADD_AS(xr11,xr11,xr14,xr14);      //xr11:t6+t2, xr14:t6-t2

        D32ADD_AA(xr2,xr2,xr15,xr0);        //xr2:t5+t1+4;
        D32ADD_AA(xr13,xr13,xr15,xr0);      //xr13:t5-t1+4
	D32ADD_AA(xr11,xr11,xr15,xr0);      //xr11:t6+t2+4
	D32ADD_AA(xr14,xr14,xr15,xr0);      //xr14:t6-t2+4

        D32SARL(xr5,xr11,xr2,3);
        D32SARL(xr6,xr13,xr14,3);
        S32STD(xr5,dst,0x0);
        S32STD(xr6,dst,0xC);             //

	D16MUL_HW(xr5,xr1,xr8,xr6);      //xr5:4s1 xr6:9s1
        D16MAC_SS_LW(xr5,xr1,xr10,xr6);  //xr5:4s1-9s3  xr6:9s1-16s3
        D16MUL_HW(xr13,xr3,xr9,xr14);    //xr13:15s5  xr14:4s5
        D16MAC_SA_LW(xr13,xr3,xr7,xr14); //xr13:15s5-16s7  xr14:4s5+15s7 

        D32ADD_AS(xr1,xr5,xr13,xr0); //xr1:4s1-9s3+15s5-16s7(t4) 
        D32ADD_AS(xr2,xr6,xr14,xr0); //xr2:9s1-16s3+4s5+15s7(t3)

	D32ADD_AS(xr4,xr4,xr1,xr1);   //xr4:t8+t4  xr1:t8-t4
	D32ADD_AS(xr12,xr12,xr2,xr2);   //xr12:t7+t3  xr2:t7-t3

        D32ADD_AA(xr4,xr4,xr15,xr0);   //xr4:t8+t4+4          
        D32ADD_AA(xr1,xr1,xr15,xr0);   //xr1:t8-t4+4 
        D32ADD_AA(xr12,xr12,xr15,xr0); //xr12:t7+t3+4
        D32ADD_AA(xr2,xr2,xr15,xr0);   //xr2:t7-t3+4
       
        D32SARL(xr14,xr4,xr12,3);
        D32SARL(xr3,xr2,xr1,3);
        S32STD(xr14,dst,0x4);
        S32STD(xr3,dst,0x8);

        src += 8;
        dst += 8;
    }
   
    src = block + off;
    dst = block + off;
    S32I2M(xr15,64);
    S32I2M(xr14,(22<<16)|10);
    S32I2M(xr13,(17<<16)|17);
    for(i = 0; i < 8; i++){

        S16LDD(xr4,src,0,ptn1);
	S16LDD(xr4,src,32,ptn0);  //xr1:s0,s16
	S16LDD(xr3,src,16,ptn1);
	S16LDD(xr3,src,48,ptn0);   //xr2:s8,s24

        D16MUL_WW(xr5,xr13,xr4,xr6);   //xr5:17s0  xr6:17s2
        D32ADD_AS(xr1,xr5,xr6,xr2);
        
        D32ADD_AS(xr5,xr1,xr15,xr0); //xr5:t1+4
        D32ADD_AS(xr6,xr2,xr15,xr0); //xr6:t2+4
        
        D16MUL_HW(xr9,xr14,xr3,xr10);   //xr9:0,22s1(t3) xr10:0,22s3(t4)
        D16MUL_LW(xr7,xr14,xr3,xr8);   //xr7:0,10s1(t5),xr8:0,10s3(t6)
             
        D32ADD_AA(xr3,xr9,xr8,xr11); //xr3:0,t3+t6
        D32ADD_AS(xr11,xr10,xr7,xr4); //xr4:0,t4-t5

        D32ADD_AS(xr1,xr5,xr3,xr2);   //xr1:t1+t3+t6+4  xr2:t1+4-t3-t6
        D32ADD_AS(xr7,xr6,xr4,xr8);   //xr7:+t4    xr8:-t4
        
        D32SARL(xr5,xr8,xr1,7);
	D32SARL(xr6,xr2,xr7,7);

	S16STD(xr5,dst,0,ptn0);      
	S16STD(xr5,dst,16,ptn1);      
	S16STD(xr6,dst,32,ptn0);      
	S16STD(xr6,dst,48,ptn1);

	src ++;
        dst ++;
    } 
}

/** Do inverse transform on 4x8 parts of block
*/
static void vc1_inv_trans_4x8_aux(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;
 
    off = n * 4;
    src = block + off;
    dst = block + off;
    S32I2M(xr15,4);
    S32I2M(xr14,(22<<16)|10);
    S32I2M(xr13,(17<<16)|17);  
    for(i = 0; i < 8; i++){

        S32LDD(xr1,src,0x0);    //s1,s0
	S32LDD(xr2,src,0x4);    //s3,s2

	S32SFL(xr3,xr1,xr2,xr4,ptn3);  //xr3:s1,s3  xr4:s0,s2
        D16MUL_WW(xr5,xr13,xr4,xr6);   //xr5:17s0  xr6:17s2

        D32ADD_AS(xr1,xr5,xr6,xr2);
        D32ADD_AS(xr5,xr1,xr15,xr0); //xr5:t1+4
        D32ADD_AS(xr6,xr2,xr15,xr0); //xr6:t2+4
        
        D16MUL_HW(xr9,xr14,xr3,xr10);   //xr9:0,22s1(t3) xr10:0,22s3(t4)
        D16MUL_LW(xr7,xr14,xr3,xr8);   //xr7:0,10s1(t5),xr8:0,10s3(t6)
             
        D32ADD_AA(xr3,xr9,xr8,xr11); //xr3:0,t3+t6
        D32ADD_AS(xr11,xr10,xr7,xr4); //xr4:0,t4-t5

        D32ADD_AS(xr1,xr5,xr3,xr2);   //xr1:t1+t3+t6+4  xr2:t1+4-t3-t6
        D32ADD_AS(xr7,xr6,xr4,xr8);   //xr7:+t4    xr8:-t4
        
        D32SARL(xr5,xr8,xr1,3);
	D32SARL(xr6,xr2,xr7,3);

        S32STD(xr5,dst,0x0);
        S32STD(xr6,dst,0x4);

        src += 8;
        dst += 8;
    }   

    src = block + off;
    dst = block + off;
    S32I2M(xr7,0x10000F);
    S32I2M(xr8,0x40009);
    S32I2M(xr9,0xF0004);
    S32I2M(xr10,0x90010);
    for(i = 0; i < 4; i++){
        S32I2M(xr5,0xC0010);
	S32I2M(xr6,0xC0006);
        S32I2M(xr15,64);
        
        S16LDD(xr1,src,16,ptn1);     //xr1:s8,s24   
	S16LDD(xr1,src,48,ptn0);
	S16LDD(xr2,src,0,ptn1);      //xr2:s0,s16
	S16LDD(xr2,src,32,ptn0);
	S16LDD(xr3,src,80,ptn1);     //xr3:s40,s56 
	S16LDD(xr3,src,112,ptn0);
	S16LDD(xr4,src,64,ptn1);     //xr4:s32,s48
	S16LDD(xr4,src,96,ptn0);

        D16MUL_WW(xr11,xr2,xr5,xr12);       //xr11:12s0,xr12:16s2
	D16MAC_AA_WW(xr11,xr4,xr6,xr12);    //xr11:12s0+12s4(t1), xr12:16s2+6s6(t3)
                
        D16MUL_WW(xr13,xr2,xr6,xr14);       //xr13:12s0, xr14:6s2
	D16MAC_SS_WW(xr13,xr4,xr5,xr14);    //xr13:12s0-12s4(t2), xr14:6s2-16s6(t4)
        
        D32ADD_AS(xr2,xr11,xr12,xr4);       //xr2:t1+t3(t5)  xr4:t1-t3(t8)
        D32ADD_AS(xr11,xr13,xr14,xr12);     //xr11:t2+t4(t6) xr12:t2-t4(t7)  //xr5,6,13,14 no use
    
        D16MUL_HW(xr13,xr1,xr7,xr14);         //xr13:16s1,xr14:15s1
        D16MAC_AS_LW(xr13,xr1,xr9,xr14);      //xr13:16s1+15s3 xr14:15s1-4s3
        
        D16MUL_HW(xr5,xr3,xr10,xr6);          //xr5:9s5 xr6:16s5
        D16MAC_AA_LW(xr5,xr3,xr8,xr6);         //xr5:9s5+4s7  xr6:16s5+9s7

        D32ADD_AS(xr13,xr13,xr5,xr0);         //xr13:16s1+15s3+9s5+4s7(t1)
        D32ADD_AS(xr0,xr14,xr6,xr14);         //xr14:15s1-4s3-16s5-9s7(t2)
 
        D32ADD_AS(xr2,xr2,xr13,xr13);        //xr2:t5+t1  xr13:t5-t1 
        D32ADD_AS(xr11,xr11,xr14,xr14);      //xr11:t6+t2, xr14:t6-t2

        D32ADD_AA(xr2,xr2,xr15,xr0);        //xr2:t5+t1+64;
        D32ADD_AA(xr11,xr11,xr15,xr0);      //xr11:t6+t2+64
        S32I2M(xr15,65);
        D32ADD_AA(xr13,xr13,xr15,xr0);      //xr13:t5-t1+65
	D32ADD_AA(xr14,xr14,xr15,xr0);      //xr14:t6-t2+65
        
        D32SAR(xr2,xr2,xr11,xr11,7); 
        D32SAR(xr13,xr13,xr14,xr14,7);
   
        S16STD(xr2,dst,0,ptn0);
	S16STD(xr11,dst,16,ptn0);               
	S16STD(xr14,dst,96,ptn0);
	S16STD(xr13,dst,112,ptn0);

        D16MUL_HW(xr5,xr1,xr8,xr6);      //xr5:4s1 xr6:9s1
        D16MAC_SS_LW(xr5,xr1,xr10,xr6);  //xr5:4s1-9s3  xr6:9s1-16s3
        D16MUL_HW(xr13,xr3,xr9,xr14);    //xr13:15s5  xr14:4s5
        D16MAC_SA_LW(xr13,xr3,xr7,xr14); //xr13:15s5-16s7  xr14:4s5+15s7 

        D32ADD_AS(xr1,xr5,xr13,xr0); //xr1:4s1-9s3+15s5-16s7(t4) 
        D32ADD_AS(xr2,xr6,xr14,xr0); //xr2:9s1-16s3+4s5+15s7(t3)

	D32ADD_AS(xr4,xr4,xr1,xr1);   //xr4:t8+t4  xr1:t8-t4
	D32ADD_AS(xr12,xr12,xr2,xr2);   //xr12:t7+t3  xr2:t7-t3

	D32ADD_AA(xr2,xr2,xr15,xr0);   //xr2:t7-t3+65
        D32ADD_AA(xr1,xr1,xr15,xr0);   //xr1:t8-t4+65
        S32I2M(xr15,64); 
        D32ADD_AA(xr12,xr12,xr15,xr0); //xr12:t7+t3+64
	D32ADD_AA(xr4,xr4,xr15,xr0);   //xr4:t8+t4+64 

        D32SAR(xr2,xr2,xr1,xr1,7); 
        D32SAR(xr12,xr12,xr4,xr4,7);
         
	S16STD(xr12,dst,32,ptn0);
	S16STD(xr4,dst,48,ptn0);               
	S16STD(xr1,dst,64,ptn0);
	S16STD(xr2,dst,80,ptn0);

        src++;
        dst++;
    }
}

/** Do inverse transform on 4x4 part of block
*/

static void vc1_inv_trans_4x4_aux(DCTELEM block[64], int n)
{
    int i;
    register int t1,t2,t3,t4,t5,t6;
    DCTELEM *src, *dst;
    int off;

    off = (n&1) * 4 + (n&2) * 16;
    src = block + off;
    dst = block + off;

    S32I2M(xr15,4);
    S32I2M(xr14,(22<<16)|10);
    S32I2M(xr13,(17<<16)|17);  
    for(i = 0; i < 4; i++){

        S32LDD(xr1,src,0x0);    //s1,s0
	S32LDD(xr2,src,0x4);    //s3,s2

	S32SFL(xr3,xr1,xr2,xr4,ptn3);  //xr3:s1,s3  xr4:s0,s2
        D16MUL_WW(xr5,xr13,xr4,xr6);   //xr5:17s0  xr6:17s2

        D32ADD_AS(xr1,xr5,xr6,xr2);
        D32ADD_AS(xr5,xr1,xr15,xr0); //xr5:t1+4
        D32ADD_AS(xr6,xr2,xr15,xr0); //xr6:t2+4
        
        D16MUL_HW(xr9,xr14,xr3,xr10);   //xr9:0,22s1(t3) xr10:0,22s3(t4)
        D16MUL_LW(xr7,xr14,xr3,xr8);   //xr7:0,10s1(t5),xr8:0,10s3(t6)
             
        D32ADD_AA(xr3,xr9,xr8,xr11); //xr3:0,t3+t6
        D32ADD_AS(xr11,xr10,xr7,xr4); //xr4:0,t4-t5

        D32ADD_AS(xr1,xr5,xr3,xr2);   //xr1:t1+t3+t6+4  xr2:t1+4-t3-t6
        D32ADD_AS(xr7,xr6,xr4,xr8);   //xr7:+t4    xr8:-t4
        
        D32SARL(xr5,xr8,xr1,3);
	D32SARL(xr6,xr2,xr7,3);

        S32STD(xr5,dst,0x0);
        S32STD(xr6,dst,0x4);

        src += 8;
        dst += 8;
    }

    src = block + off;
    dst = block + off;
    S32I2M(xr15,64);

    for(i = 0; i < 4; i++){
        S16LDD(xr4,src,0,ptn1);
	S16LDD(xr4,src,32,ptn0);  //xr1:s0,s16
	S16LDD(xr3,src,16,ptn1);
	S16LDD(xr3,src,48,ptn0);   //xr2:s8,s24

        D16MUL_WW(xr5,xr13,xr4,xr6);   //xr5:17s0  xr6:17s2
        D32ADD_AS(xr1,xr5,xr6,xr2);
        
        D32ADD_AS(xr5,xr1,xr15,xr0); //xr5:t1+4
        D32ADD_AS(xr6,xr2,xr15,xr0); //xr6:t2+4
        
        D16MUL_HW(xr9,xr14,xr3,xr10);   //xr9:0,22s1(t3) xr10:0,22s3(t4)
        D16MUL_LW(xr7,xr14,xr3,xr8);   //xr7:0,10s1(t5),xr8:0,10s3(t6)
             
        D32ADD_AA(xr3,xr9,xr8,xr11); //xr3:0,t3+t6
        D32ADD_AS(xr11,xr10,xr7,xr4); //xr4:0,t4-t5

        D32ADD_AS(xr1,xr5,xr3,xr2);   //xr1:t1+t3+t6+4  xr2:t1+4-t3-t6
        D32ADD_AS(xr7,xr6,xr4,xr8);   //xr7:+t4    xr8:-t4
        
        D32SARL(xr5,xr8,xr1,7);
	D32SARL(xr6,xr2,xr7,7);

	S16STD(xr5,dst,0,ptn0);      
	S16STD(xr5,dst,16,ptn1);      
	S16STD(xr6,dst,32,ptn0);      
	S16STD(xr6,dst,48,ptn1);
        src ++;
        dst ++;
    }
}
