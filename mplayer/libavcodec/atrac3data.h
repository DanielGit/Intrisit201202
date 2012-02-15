/*
 * Atrac 3 compatible decoder data
 * Copyright (c) 2006-2007 Maxim Poliakovski
 * Copyright (c) 2006-2007 Benjamin Larsson
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
 * @file atrac3data.h
 * Atrac 3 AKA RealAudio 8 compatible decoder data
 */

#ifndef AVCODEC_ATRAC3DATA_H
#define AVCODEC_ATRAC3DATA_H

#include <stdint.h>
//Here is added by jyu at 2008-11-25
#include "fft_fix.h"

#define  ATRAC3FIXED
#ifdef ATRAC3FIXED

//typedef int   atrac_sample_t;
typedef FFTSample_fix atrac_sample_t;
typedef int   atrac_level_t;

# define ATRAC3_SAMPLE_FRACBITS         31
# define ATRAC3_LEVEL_FRACBITS_26       29// [27, 29] is best, where noise is least. 
# define ATRAC3_LEVEL_FRACBITS_15       15

#define SAMPLE(x, level) ((atrac_sample_t)((x) * ((1 << (level)) - 1)))
#define LEVEL(x, level)  ((atrac_level_t)((x) * ((1 <<(level) ) - 1)))
#define LEVEL_R(x)  ((float)(x)/(float)(1<<ATRAC3_LEVEL_FRACBITS - 1))

#if 0
#define ATRAC3_MUL3(a,b,fr)  \
 ({                   \
      long long tmp;   \
      int hi;         \
      unsigned int lo;\
      tmp =   ((long long)(a) * (b)+(1<<(fr-1)));\
       hi = tmp >> 32; \
       lo = (tmp & 0xFFFFFFFF);\
       ((hi << (32 -fr)) | (lo >>fr));\
  })

#define ATRAC3_MADD(s,a,b) \
  do{               \
      long sum;\
      sum = ((long long)(a) * (b)+(1<<(ATRAC3_SAMPLE_FRACBITS-1))) >> ATRAC3_SAMPLE_FRACBITS; \
      s += sum;                                                     \
     }while(0)
#endif
#define ATRAC3_MUL3(a,b,fr)      \
({                        \
  long hi;                \
  unsigned long lo;       \
  asm("mult %2, %3\n"     \
     :"=h"(hi),"=l"(lo)   \
     :"%r"(a),"r"(b));    \
  hi = ((hi << (32-(fr))) | (lo>>(fr)));\
  lo = (lo & (1<<(fr-1)))?1:0;     \
  (hi+lo);                \
})
#define ATRAC3_MUL4E(a,b)   \
do{                       \
    asm __volatile(        \
         "mult %0, %1"     \
       ::"%r"(a), "r"(b)   \
       :"hi","lo");         \
}while(0) 

#define ATRAC3_MADD4E(a,b) \
do{               \
  asm __volatile("madd %0,%1"        \
      ::"%r"(a), "r"(b)   \
      :"hi","lo");        \
}while(0)
#define ATRAC3_MFLW(ra, rb, idx)   \
do{                         \
   asm __volatile("lw %0,%2(%1)"  \
       :"=r"(ra)            \
       :"d"(rb),"I"(idx));  \
}while(0)
#define ATRAC3_EXTRACT_E(hi,lo, fr)   \
({                            \
   asm __volatile("mfhi %0"    \
          :"=d"(hi)            \
          ::"hi");             \
   asm __volatile("mflo %0"    \
          :"=d"(lo)            \
          ::"lo");             \
    (((hi) << (32 - (fr))) | ((lo) >> (fr)));\
})

#else  //ATRAC3FIXED

typedef float atrac_sample_t;
typedef float atrac_level_t;

#define LEVEL_26_1  1.0

#define SAMPLE(x) (x)
#define LEVEL(x)  (x)
#define LEVEL_R(x)  (x)
#define LEVEL_26(x)  (x)
#define LEVEL_15(x)  (x)

#define MUL(a,b) ((a) * (b))
#define MUL_L_26(a,b) ((a) * (b))
#define MUL_L_15(a,b) ((a) * (b))
#define MADD(s,a,b) ((s) += ((a) * (b)))
#define ATRAC3_MUL3(a,b,fr) ((a) * (b))
#define MADD4(hi,lo,a,b) ((hi) += ((a) * (b)))
#define EXTRACT(hi,lo, fr) ( (hi) )
#endif  //ATRAC3FIXED



/* VLC tables */

static const uint8_t huffcode1[9] = {
  0x0,0x4,0x5,0xC,0xD,0x1C,0x1D,0x1E,0x1F,
};

static const uint8_t huffbits1[9] = {
  1,3,3,4,4,5,5,5,5,
};

static const uint8_t huffcode2[5] = {
  0x0,0x4,0x5,0x6,0x7,
};

static const uint8_t huffbits2[5] = {
  1,3,3,3,3,
};

static const uint8_t huffcode3[7] = {
0x0,0x4,0x5,0xC,0xD,0xE,0xF,
};

static const uint8_t huffbits3[7] = {
  1,3,3,4,4,4,4,
};

static const uint8_t huffcode4[9] = {
  0x0,0x4,0x5,0xC,0xD,0x1C,0x1D,0x1E,0x1F,
};

static const uint8_t huffbits4[9] = {
  1,3,3,4,4,5,5,5,5,
};

static const uint8_t huffcode5[15] = {
  0x0,0x2,0x3,0x8,0x9,0xA,0xB,0x1C,0x1D,0x3C,0x3D,0x3E,0x3F,0xC,0xD,
};

static const uint8_t huffbits5[15] = {
  2,3,3,4,4,4,4,5,5,6,6,6,6,4,4
};

static const uint8_t huffcode6[31] = {
  0x0,0x2,0x3,0x4,0x5,0x6,0x7,0x14,0x15,0x16,0x17,0x18,0x19,0x34,0x35,
  0x36,0x37,0x38,0x39,0x3A,0x3B,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x8,0x9,
};

static const uint8_t huffbits6[31] = {
  3,4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,4,4
};

static const uint8_t huffcode7[63] = {
  0x0,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,0x10,0x11,0x24,0x25,0x26,0x27,0x28,
  0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x68,0x69,0x6A,0x6B,0x6C,
  0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,
  0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0x2,0x3,
};

static const uint8_t huffbits7[63] = {
  3,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,4,4
};

static const uint8_t huff_tab_sizes[7] = {
  9, 5, 7, 9, 15, 31, 63,
};

static const uint8_t* huff_codes[7] = {
  huffcode1,huffcode2,huffcode3,huffcode4,huffcode5,huffcode6,huffcode7,
};

static const uint8_t* huff_bits[7] = {
  huffbits1,huffbits2,huffbits3,huffbits4,huffbits5,huffbits6,huffbits7,
};

/* selector tables */

static const uint8_t CLCLengthTab[8] = {0, 4, 3, 3, 4, 4, 5, 6};
static const int8_t seTab_0[4] = {0, 1, -2, -1};
static const int8_t decTable1[18] = {0,0, 0,1, 0,-1, 1,0, -1,0, 1,1, 1,-1, -1,1, -1,-1};


static const uint16_t subbandTab[33] = {
  0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224,
  256, 288, 320, 352, 384, 416, 448, 480, 512, 576, 640, 704, 768, 896, 1024
};

/* transform data */

static const float qmf_48tap_half[24] = {
   -0.00001461907, -0.00009205479, -0.000056157569, 0.00030117269,
    0.0002422519,-0.00085293897, -0.0005205574, 0.0020340169,
    0.00078333891, -0.0042153862, -0.00075614988, 0.0078402944,
   -0.000061169922, -0.01344162, 0.0024626821, 0.021736089,
   -0.007801671, -0.034090221, 0.01880949, 0.054326009,
   -0.043596379, -0.099384367, 0.13207909, 0.46424159
};
/* tables for the scalefactor decoding */

static const float iMaxQuant[8] = {
  0.0, 1.0/1.5, 1.0/2.5, 1.0/3.5, 1.0/4.5, 1.0/7.5, 1.0/15.5, 1.0/31.5
};
/* joint stereo related tables */
static const float matrixCoeffs[8] = {0.0, 2.0, 2.0, 2.0, 0.0, 0.0, 1.0, 1.0};

//iMaxQuant_fix[i] = SAMPLE(iMaxQuant[i]), (1<<31) - 1
static atrac_sample_t iMaxQuant_fix[8];

//matrixCoeffs_fix[i] = matrixCoeffs[i]*(1<<15)
static atrac_sample_t matrixCoeffs_fix[8];


// chWeigh0[i] = {0, 1/7,2/7,3/7,4/7, 5/7,6/7, 7/7} * (1<<15)
// chWeigh0_fix[i] = chWeigh0[i] * (1<<15)
static atrac_sample_t chWeigh0_fix[8];


// chWeigh1_fix[i] = sqrt(2 - chWeigh0[i]*chWeigh0[i])  * (1<<15)
static atrac_sample_t chWeigh1_fix[8];

#endif // AVCODEC_ATRAC3DATA_H
