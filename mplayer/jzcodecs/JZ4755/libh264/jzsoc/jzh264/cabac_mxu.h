/*
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file cabac.h
 * Context Adaptive Binary Arithmetic Coder.
 */

#ifndef CABAC_MXU_H
#define CABAC_MXU_H


//#undef NDEBUG
#include <assert.h>

#define CABAC_BITS 16
#define CABAC_MASK ((1<<CABAC_BITS)-1)
#define BRANCHLESS_CABAC_DECODER 1
//#define ARCH_X86_DISABLED 1



static av_always_inline int get_cabac_inline_mxu(CABACContext *c, uint8_t * const state){

#define LOW          "0"
#define RANGE        "4"
#define BYTESTART   "12"
#define BYTE        "16"
#define BYTEEND     "20"

    int bit;
    
    __asm__ __volatile__(
	"S32M2I    xr12, $10                    \n\t"  // $10 <- ff_h264_lps_range
	//"S32M2I    xr15, $8                     \n\t"  // $8  <- range
	//"S32M2I    xr14, $9                     \n\t"  // $9  <- low
	"lw        $9  , "LOW"(%2)              \n\t" // $9=c.low
	"lw        $8  , "RANGE"(%2)            \n\t" // $8=c.range"

	"andi      $11 , $8   , 0xC0            \n\t"  // $11 <- range & 0xC0
	"lbu       $12 , 0(%1)                  \n\t"  // $12 <- *state
	"addu      $10 , $10  , $12             \n\t"  // $10 <- ff_h264_lps_range + s
	"LXBU      $10 , $10  , $11  , 1        \n\t"  // $10 <- RangeLPS
	"subu      $8  , $8   , $10             \n\t"  // $8  <- range - RangeLPS

	"sll       $11 , $8   , 0x11            \n\t"  // $11 <- (c->range<<(CABAC_BITS+1))
	"subu      $13 , $11  , $9              \n\t"  // $13 <- (c->range<<(CABAC_BITS+1)) - low
	"sra       $13 , $13  , 0x1F            \n\t"  // $13 <- lps_mask = (range - low) >> 31
	"and       $14 , $11  , $13             \n\t"  // $14 <- (c->range<<(CABAC_BITS+1)) & lps_mask
	"subu      $9  , $9   , $14             \n\t"  // $9  <- low - (((c->range<<(CABAC_BITS+1)) & lps_mask)
	"movn      $8  , $10  , $13             \n\t"  // if ( lps_mask != 0) range = RangeLPS
	"xor       $12 , $12  , $13             \n\t"  // s ^= lps_mask;
	"and       %0  , $12  , 1               \n\t"  // bit= s&1;
	"S32M2I    xr13, $13                    \n\t"  // $13 <- ff_h264_mlps_state+128
	"LXBU      $12 , $13  , $12  , 0        \n\t"  // $12 <- (ff_h264_mlps_state+128)[s]
	"sb        $12 , 0(%1)                  \n\t"  // *state <- $12
	
	"clz       $13 , $8                     \n\t"  // $13 <- ff_h264_norm_shift[c->range]
	"subu      $13 , $13  , 23              \n\t"  //

	"sllv      $8  , $8   , $13             \n\t"  // c->range <<= ff_h264_norm_shift[c->range];
	"sllv      $9  , $9   , $13             \n\t"  // c->low <<= ff_h264_norm_shift[c->range];
	//"S32I2M    xr15, $8                     \n\t"  // save range
	"sw        $8  , "RANGE"(%2)            \n\t"  // save range
	"andi      $11 , $9   , 0xFFFF          \n\t"  // $11 <- (c->low & CABAC_MASK)
	//"S32I2M    xr14, $9                     \n\t"  // save low
	"sw        $9  , "LOW"(%2)              \n\t"  // save low
	"beq       $11 , $0   , 1f              \n\t"  // if ( !(c->low & CABAC_MASK) ) goto __refill2
	

	"jr        $31                          \n\t"  // return
		

	"1:                                     \n\t"
	"addi      $10 , $9   , -1              \n\t"  //// $10 <- c->low-1
	"xor       $10 , $9   , $10             \n\t"  // $10 <- c->low ^ (c->low-1)
	"clz       $10 , $10                    \n\t"
	"ori       $13 , $0   , 0xF             \n\t"  // $13 <- 15
	"sub       $10 , $13  , $10             \n\t"  // $10 <- i= 7 - ff_h264_norm_shift[x>>(CABAC_BITS-1)];
	
	//"S32M2I    xr11, $12                    \n\t"  // $12 <- bitstream
	"lw        $12 , "BYTE"(%2)             \n\t" // $12=bitstream
	"S8LDD     xr10, $12  , 0    , 3        \n\t"  // xr10[24-31] <- bitstream[0]
	"S8LDD     xr10, $12  , 1    , 2        \n\t"  // xr10[16-23] <- bitstream[1]
	"S32EXTR   xr10, xr0  , $0   , 17       \n\t"  // xr10 <- Extract 17 bits 
	"D32SLLV   xr9 , xr10 , $10             \n\t"  // 0xFFFEFFFF << i, ( Extracted bits ) << i
	"S32I2M    xr14, $9                     \n\t"  // xr14 <- low
	"S32AND    xr14, xr14 , xr9             \n\t"  // low = low & (0xFFFEFFFF << i)
	
	"S32OR     xr14, xr14 , xr10            \n\t"  // low |= ( Extracted bits ) << i
	"S32STD    xr14, %2   , 0               \n\t"  // store c->low
	"S32OR     xr10, xr0  , xr8             \n\t"  // xr10 <- 0x8000
	"S32MOVN   xr9 , xr6  , xr6             \n\t"  // xr9 <- 0xFFFEFFFF
	//"D32ADD    xr11, xr11 , xr7  , xr0, 0   \n\t"  //
	"addi      $12 , $12  , 2               \n\t"  // bytestream += 2;
	"sw        $12 , "BYTE"(%2)             \n\t"  // store bytestream
	
		
	:"=&r"(bit)
	:"r"(state),"r"(c)
	:"memory"
    );
    
    return bit;

}

__h264_cabac_tcsm_text__
static int av_noinline get_cabac_mxu(CABACContext *c, uint8_t * const state){
    return get_cabac_inline_mxu(c,state);
}

__h264_cabac_tcsm_text__
static int av_noinline get_cabac_bypass_mxu(CABACContext *c){

    int bit;
    int temp=0xffff;
    asm volatile(
		 "lw $8,0(%1)                          \n\t" // $8=c.low
		 "lw $9,4(%1)                          \n\t" // $9=c.range
		 // "S32M2I xr14,$8                       \n\t" // $8=c.low
		 //"S32M2I xr15,$9                       \n\t" // $9=c.range

		 "sll $9,$9,17                         \n\t" // range=range<<17
		 "add $8,$8,$8                         \n\t" // low=low+low
		 "sub $8,$8,$9                         \n\t" // low=low-range
		 "sra %0,$8,31                         \n\t" // cdq $10=edx
      		 "and $9,$9,%0                         \n\t" // low=low & range
		 "add $8,$8,$9                         \n\t" // low=low+range
		 "andi $10,$8,0xffff                   \n\t" // get lower 16bits of low               
		 "bne $10,$0,1f                        \n\t" // if($20==0)

		  "lw $11,16(%1)                        \n\t" // $11=bitstream
		 //"S32M2I xr11,$11                      \n\t" // $11=c.bitstream
		 "lbu $12,0($11)                       \n\t"
		 "lbu $13,1($11)                       \n\t"
		 "sll $12,$12,9                        \n\t"
		 "sll $13,$13,1                        \n\t"
		 "add $14,$12,$13                      \n\t"
		 "addi $11,$11,2                       \n\t" // bitstream=bitstream+2
		 "add $8,$8,$14                        \n\t" // low=low+bitstram
		 "sub $8,$8,%2                         \n\t" // low=0xffff-low
		  "sw $11,16(%1)                        \n\t" // bitstram=bitstram
		 //"S32I2M xr11,$11                      \n\t"

		 "1:                                   \n\t"
		  "sw $8,0(%1)                          \n\t" //c->low=low	
		 //"S32I2M xr14,$8                       \n\t"
		 :"=r"(bit)
		 :"r"(c),"r"(temp)
		 :"memory"
		 );
     
    return bit+1;

}


__h264_cabac_tcsm_text__
static int av_noinline get_cabac_bypass_sign_mxu(CABACContext *c, int val){

    int temp=0xffff;
    asm volatile(
		 "lw $8,0(%1)                          \n\t" // $8=c->low
		 "lw $9,4(%1)                          \n\t" // $9=c->range
		 //"S32M2I xr14,$8                       \n\t" // $8=c.low
		 //"S32M2I xr15,$9                       \n\t" // $9=c.range
		 "sll $9,$9,17                         \n\t" // range=range<<17
		 "add $8,$8,$8                         \n\t" // low=low+low
		 "sub $8,$8,$9                         \n\t" // low=low-range
		 "sra $10,$8,31                        \n\t" // cdq $10=edx
      		 "and $9,$9,$10                        \n\t" // low=low & range
		 "add $8,$8,$9                         \n\t" // low=low+range
		 "xor %0,%0,$10                        \n\t"
		 "sub %0,%0,$10                        \n\t"
		 
		 "andi $16,$8,0xffff                   \n\t" // get lower 16bits of low       
		 "bne $16,$0,1f                        \n\t" // if($20==0)
		 "lw $11,16(%1)                        \n\t" // $11=bitstream
		 //"S32M2I xr11,$11                      \n\t" // $11=c.bitstream
		 "lbu $12,0($11)                       \n\t"
		 "lbu $13,1($11)                       \n\t"
		 "sll $12,$12,9                        \n\t"
		 "sll $13,$13,1                        \n\t"
		 "add $14,$12,$13                      \n\t"
		 "addi $11,$11,2                       \n\t" // bitstream=bitstream+2
		 "add $8,$8,$14                        \n\t" // low=low+bitstram
		 "sub $8,$8,%2                         \n\t" // low=0xffff-low
		 "sw $11,16(%1)                        \n\t" // bitstram=bitstram
		 //"S32I2M xr11,$11                      \n\t"
		 
		 "1:                                   \n\t"
		 "sw $8,0(%1)                          \n\t" //c->low=low
		 //"S32I2M xr14,$8                       \n\t"	  
		 :"+r"(val)
		 :"r"(c),"r"(temp)
		 :"memory","$16"
		 );
     
    return val;
}


#endif /* CABAC_MXU_H */
