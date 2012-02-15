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
 * @file cabac.c
 * Context Adaptive Binary Arithmetic Coder.
 */

//#include <string.h>
#include "../jzmedia.h"
#include "cabac_mxu.h"

static const uint8_t lps_range[64][4]= {
{128,176,208,240}, {128,167,197,227}, {128,158,187,216}, {123,150,178,205},
{116,142,169,195}, {111,135,160,185}, {105,128,152,175}, {100,122,144,166},
{ 95,116,137,158}, { 90,110,130,150}, { 85,104,123,142}, { 81, 99,117,135},
{ 77, 94,111,128}, { 73, 89,105,122}, { 69, 85,100,116}, { 66, 80, 95,110},
{ 62, 76, 90,104}, { 59, 72, 86, 99}, { 56, 69, 81, 94}, { 53, 65, 77, 89},
{ 51, 62, 73, 85}, { 48, 59, 69, 80}, { 46, 56, 66, 76}, { 43, 53, 63, 72},
{ 41, 50, 59, 69}, { 39, 48, 56, 65}, { 37, 45, 54, 62}, { 35, 43, 51, 59},
{ 33, 41, 48, 56}, { 32, 39, 46, 53}, { 30, 37, 43, 50}, { 29, 35, 41, 48},
{ 27, 33, 39, 45}, { 26, 31, 37, 43}, { 24, 30, 35, 41}, { 23, 28, 33, 39},
{ 22, 27, 32, 37}, { 21, 26, 30, 35}, { 20, 24, 29, 33}, { 19, 23, 27, 31},
{ 18, 22, 26, 30}, { 17, 21, 25, 28}, { 16, 20, 23, 27}, { 15, 19, 22, 25},
{ 14, 18, 21, 24}, { 14, 17, 20, 23}, { 13, 16, 19, 22}, { 12, 15, 18, 21},
{ 12, 14, 17, 20}, { 11, 14, 16, 19}, { 11, 13, 15, 18}, { 10, 12, 15, 17},
{ 10, 12, 14, 16}, {  9, 11, 13, 15}, {  9, 11, 12, 14}, {  8, 10, 12, 14},
{  8,  9, 11, 13}, {  7,  9, 11, 12}, {  7,  9, 10, 12}, {  7,  8, 10, 11},
{  6,  8,  9, 11}, {  6,  7,  9, 10}, {  6,  7,  8,  9}, {  2,  2,  2,  2},
};


static const uint8_t mps_state[64]= {
  1, 2, 3, 4, 5, 6, 7, 8,
  9,10,11,12,13,14,15,16,
 17,18,19,20,21,22,23,24,
 25,26,27,28,29,30,31,32,
 33,34,35,36,37,38,39,40,
 41,42,43,44,45,46,47,48,
 49,50,51,52,53,54,55,56,
 57,58,59,60,61,62,62,63,
};

static const uint8_t lps_state[64]= {
  0, 0, 1, 2, 2, 4, 4, 5,
  6, 7, 8, 9, 9,11,11,12,
 13,13,15,15,16,16,18,18,
 19,19,21,21,22,22,23,24,
 24,25,26,26,27,27,28,29,
 29,30,30,30,31,32,32,33,
 33,33,34,34,35,35,35,36,
 36,36,37,37,37,38,38,63,
};

/**
 *
 * @param buf_size size of buf in bits
 */
void ff_init_cabac_decoder_mxu(CABACContext *c, const uint8_t *buf, int buf_size){
    c->bytestream_start=
    c->bytestream= buf;
    c->bytestream_end= buf + buf_size;

#if CABAC_BITS == 16
    c->low =  (*c->bytestream++)<<18;
    c->low+=  (*c->bytestream++)<<10;
#else
    c->low =  (*c->bytestream++)<<10;
#endif
    c->low+= ((*c->bytestream++)<<2) + 2;
    c->range= 0x1FE;

#ifdef JZC_TCSM_OPT
    S32I2M(xr12,ff_h264_lps_range);
    S32I2M(xr13,ff_h264_mlps_state+128);

    S32I2M(xr14,c->low);
    S32I2M(xr15,c->range);
    S32I2M(xr11,c->bytestream);
    S32I2M(xr10,0x8000);
    S32I2M(xr9,0xFFFEFFFF);
    S32I2M(xr8,0x8000);
    S32I2M(xr7,2);
    S32I2M(xr6,0xFFFEFFFF);
    S32I2M(xr5,c->bytestream_start);
#endif
}

void ff_init_cabac_states_mxu(CABACContext *c){
    uint32_t i = 0;
    uint8_t *p_lps_range = lps_range[0]-4;
    uint8_t *p_mps_state = mps_state-4;
    uint8_t *p_lps_state = lps_state-4;
    uint8_t *p_ff_h264_mlps_state1 = &ff_h264_mlps_state[0]+124;
    uint8_t *p_ff_h264_mlps_state2 = &ff_h264_mlps_state[0]+128;//
    uint8_t *p_ff_h264_lps_range = ff_h264_lps_range-128;
    S32I2M(xr15,0x1010101);

    for(i=0; i<16; i++){
      
      /* load 32 */
      S32LDI(xr1,p_lps_range,4);
      S32LDI(xr2,p_lps_range,4);
      S32LDI(xr3,p_lps_range,4);
      S32LDI(xr4,p_lps_range,4);

      S32SFL(xr7,xr2,xr1,xr8,0);
      S32SFL(xr9,xr4,xr3,xr10,0);

      S32SFL(xr11,xr7,xr7,xr12,0);
      S32SFL(xr13,xr8,xr8,xr14,0);
      S32SFL(xr1,xr9,xr9,xr2,0);
      S32SFL(xr3,xr10,xr10,xr4,0);

      S32SDI(xr14,p_ff_h264_lps_range,128); 
      S32SDI(xr4,p_ff_h264_lps_range,4); 
      S32SDI(xr13,p_ff_h264_lps_range,124);
      S32SDI(xr3,p_ff_h264_lps_range,4); 
      S32SDI(xr12,p_ff_h264_lps_range,124);
      S32SDI(xr2,p_ff_h264_lps_range,4); 
      S32SDI(xr11,p_ff_h264_lps_range,124);
      S32SDI(xr1,p_ff_h264_lps_range,4);
      p_ff_h264_lps_range-=128*4-4;

     
      /* part2 */
      S32LDI(xr1,p_mps_state,4); 
     
      Q8ADD_AA(xr3,xr1,xr1);// 2*mps_state[i]+0
      Q8ADD_AA(xr4,xr3,xr15);//2*mps_state[i]+1  
      S32SFL(xr11,xr4,xr3,xr12,0);     
      
      S32SDI(xr12,p_ff_h264_mlps_state1,4);//  ff_h264_mlps_state[128+2*i+1]= 2*mps_state[i]+0;
      S32SDI(xr11,p_ff_h264_mlps_state1,4);
         
      /*part3 */ 
      S32LDI(xr1,p_lps_state,4);   

      Q8ADD_AA(xr3,xr1,xr1);// 2*mps_state[i]+0
      Q8ADD_AA(xr4,xr3,xr15);//2*mps_state[i]+1
    
      S32SFL(xr11,xr3,xr4,xr12,0);
         
      S32ALN(xr11,xr11,xr11,2);
      S32ALN(xr12,xr12,xr12,2);
    
      S32SDI(xr12,p_ff_h264_mlps_state2,-4);
      S32SDI(xr11,p_ff_h264_mlps_state2,-4);
     
   }
    
   ff_h264_mlps_state[127]= 1;
   ff_h264_mlps_state[126]= 0;    

}

