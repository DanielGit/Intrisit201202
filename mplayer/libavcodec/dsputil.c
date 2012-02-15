/*
 * DSP utils
 * Copyright (c) 2000, 2001 Fabrice Bellard.
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * gmc & q-pel & 32/64 bit based MC by Michael Niedermayer <michaelni@gmx.at>
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
 * @file dsputil.c
 * DSP utils
 */

#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"
#include "simple_idct.h"
#include "faandct.h"
#include "h263.h"
#include "snow.h"
#define JZ4740_MXU_OPT
#ifdef JZ4740_MXU_OPT
#include "jzmedia.h"
#endif

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

//#undef printf

#ifndef __LINUX__
const uint8_t ff_h263_loop_filter_strength[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9,10,10,10,11,11,11,12,12,12
};
#endif

/* snow.c */
void ff_spatial_dwt(int *buffer, int width, int height, int stride, int type, int decomposition_count);

/* vorbis.c */
void vorbis_inverse_coupling(float *mag, float *ang, int blocksize);

/* flacenc.c */
void ff_flac_compute_autocorr(const int32_t *data, int len, int lag, double *autoc);

uint8_t ff_cropTbl[256 + 2 * MAX_NEG_CROP] = {0, };
uint32_t ff_squareTbl[512] = {0, };

const uint8_t ff_zigzag_direct[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

/* Specific zigzag scan for 248 idct. NOTE that unlike the
   specification, we interleave the fields */
const uint8_t ff_zigzag248_direct[64] = {
     0,  8,  1,  9, 16, 24,  2, 10,
    17, 25, 32, 40, 48, 56, 33, 41,
    18, 26,  3, 11,  4, 12, 19, 27,
    34, 42, 49, 57, 50, 58, 35, 43,
    20, 28,  5, 13,  6, 14, 21, 29,
    36, 44, 51, 59, 52, 60, 37, 45,
    22, 30,  7, 15, 23, 31, 38, 46,
    53, 61, 54, 62, 39, 47, 55, 63,
};

/* not permutated inverse zigzag_direct + 1 for MMX quantizer */
DECLARE_ALIGNED_8(uint16_t, inv_zigzag_direct16[64]) = {0, };

const uint8_t ff_alternate_horizontal_scan[64] = {
    0,  1,   2,  3,  8,  9, 16, 17,
    10, 11,  4,  5,  6,  7, 15, 14,
    13, 12, 19, 18, 24, 25, 32, 33,
    26, 27, 20, 21, 22, 23, 28, 29,
    30, 31, 34, 35, 40, 41, 48, 49,
    42, 43, 36, 37, 38, 39, 44, 45,
    46, 47, 50, 51, 56, 57, 58, 59,
    52, 53, 54, 55, 60, 61, 62, 63,
};

const uint8_t ff_alternate_vertical_scan[64] = {
    0,  8,  16, 24,  1,  9,  2, 10,
    17, 25, 32, 40, 48, 56, 57, 49,
    41, 33, 26, 18,  3, 11,  4, 12,
    19, 27, 34, 42, 50, 58, 35, 43,
    51, 59, 20, 28,  5, 13,  6, 14,
    21, 29, 36, 44, 52, 60, 37, 45,
    53, 61, 22, 30,  7, 15, 23, 31,
    38, 46, 54, 62, 39, 47, 55, 63,
};

/* a*inverse[b]>>32 == a/b for all 0<=a<=65536 && 2<=b<=255 */
const uint32_t ff_inverse[256]={
         0, 4294967295U,2147483648U,1431655766, 1073741824,  858993460,  715827883,  613566757,
 536870912,  477218589,  429496730,  390451573,  357913942,  330382100,  306783379,  286331154,
 268435456,  252645136,  238609295,  226050911,  214748365,  204522253,  195225787,  186737709,
 178956971,  171798692,  165191050,  159072863,  153391690,  148102321,  143165577,  138547333,
 134217728,  130150525,  126322568,  122713352,  119304648,  116080198,  113025456,  110127367,
 107374183,  104755300,  102261127,   99882961,   97612894,   95443718,   93368855,   91382283,
  89478486,   87652394,   85899346,   84215046,   82595525,   81037119,   79536432,   78090315,
  76695845,   75350304,   74051161,   72796056,   71582789,   70409300,   69273667,   68174085,
  67108864,   66076420,   65075263,   64103990,   63161284,   62245903,   61356676,   60492498,
  59652324,   58835169,   58040099,   57266231,   56512728,   55778797,   55063684,   54366675,
  53687092,   53024288,   52377650,   51746594,   51130564,   50529028,   49941481,   49367441,
  48806447,   48258060,   47721859,   47197443,   46684428,   46182445,   45691142,   45210183,
  44739243,   44278014,   43826197,   43383509,   42949673,   42524429,   42107523,   41698712,
  41297763,   40904451,   40518560,   40139882,   39768216,   39403370,   39045158,   38693400,
  38347923,   38008561,   37675152,   37347542,   37025581,   36709123,   36398028,   36092163,
  35791395,   35495598,   35204650,   34918434,   34636834,   34359739,   34087043,   33818641,
  33554432,   33294321,   33038210,   32786010,   32537632,   32292988,   32051995,   31814573,
  31580642,   31350127,   31122952,   30899046,   30678338,   30460761,   30246249,   30034737,
  29826162,   29620465,   29417585,   29217465,   29020050,   28825284,   28633116,   28443493,
  28256364,   28071682,   27889399,   27709467,   27531842,   27356480,   27183338,   27012373,
  26843546,   26676816,   26512144,   26349493,   26188825,   26030105,   25873297,   25718368,
  25565282,   25414008,   25264514,   25116768,   24970741,   24826401,   24683721,   24542671,
  24403224,   24265352,   24129030,   23994231,   23860930,   23729102,   23598722,   23469767,
  23342214,   23216040,   23091223,   22967740,   22845571,   22724695,   22605092,   22486740,
  22369622,   22253717,   22139007,   22025474,   21913099,   21801865,   21691755,   21582751,
  21474837,   21367997,   21262215,   21157475,   21053762,   20951060,   20849356,   20748635,
  20648882,   20550083,   20452226,   20355296,   20259280,   20164166,   20069941,   19976593,
  19884108,   19792477,   19701685,   19611723,   19522579,   19434242,   19346700,   19259944,
  19173962,   19088744,   19004281,   18920561,   18837576,   18755316,   18673771,   18592933,
  18512791,   18433337,   18354562,   18276457,   18199014,   18122225,   18046082,   17970575,
  17895698,   17821442,   17747799,   17674763,   17602325,   17530479,   17459217,   17388532,
  17318417,   17248865,   17179870,   17111424,   17043522,   16976156,   16909321,   16843010,
};

/* Input permutation for the simple_idct_mmx */
static const uint8_t simple_mmx_permutation[64]={
        0x00, 0x08, 0x04, 0x09, 0x01, 0x0C, 0x05, 0x0D,
        0x10, 0x18, 0x14, 0x19, 0x11, 0x1C, 0x15, 0x1D,
        0x20, 0x28, 0x24, 0x29, 0x21, 0x2C, 0x25, 0x2D,
        0x12, 0x1A, 0x16, 0x1B, 0x13, 0x1E, 0x17, 0x1F,
        0x02, 0x0A, 0x06, 0x0B, 0x03, 0x0E, 0x07, 0x0F,
        0x30, 0x38, 0x34, 0x39, 0x31, 0x3C, 0x35, 0x3D,
        0x22, 0x2A, 0x26, 0x2B, 0x23, 0x2E, 0x27, 0x2F,
        0x32, 0x3A, 0x36, 0x3B, 0x33, 0x3E, 0x37, 0x3F,
};

static int pix_sum_c(uint8_t * pix, int line_size)
{
    int s, i, j;

    s = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j += 8) {
            s += pix[0];
            s += pix[1];
            s += pix[2];
            s += pix[3];
            s += pix[4];
            s += pix[5];
            s += pix[6];
            s += pix[7];
            pix += 8;
        }
        pix += line_size - 16;
    }
    return s;
}

static int pix_norm1_c(uint8_t * pix, int line_size)
{
    int s, i, j;
    uint32_t *sq = ff_squareTbl + 256;

    s = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j += 8) {
#if 0
            s += sq[pix[0]];
            s += sq[pix[1]];
            s += sq[pix[2]];
            s += sq[pix[3]];
            s += sq[pix[4]];
            s += sq[pix[5]];
            s += sq[pix[6]];
            s += sq[pix[7]];
#else
#if LONG_MAX > 2147483647
            register uint64_t x=*(uint64_t*)pix;
            s += sq[x&0xff];
            s += sq[(x>>8)&0xff];
            s += sq[(x>>16)&0xff];
            s += sq[(x>>24)&0xff];
            s += sq[(x>>32)&0xff];
            s += sq[(x>>40)&0xff];
            s += sq[(x>>48)&0xff];
            s += sq[(x>>56)&0xff];
#else
            register uint32_t x=*(uint32_t*)pix;
            s += sq[x&0xff];
            s += sq[(x>>8)&0xff];
            s += sq[(x>>16)&0xff];
            s += sq[(x>>24)&0xff];
            x=*(uint32_t*)(pix+4);
            s += sq[x&0xff];
            s += sq[(x>>8)&0xff];
            s += sq[(x>>16)&0xff];
            s += sq[(x>>24)&0xff];
#endif
#endif
            pix += 8;
        }
        pix += line_size - 16;
    }
    return s;
}

static void bswap_buf(uint32_t *dst, uint32_t *src, int w){
    int i;
#ifdef JZ4750_OPT
    dst -= 1;
    src -= 1;
    for(i=0; i+8<=w; i+=8){
        S32LDIR(xr1, src, 0x4);
        S32LDIR(xr2, src, 0x4);
        S32LDIR(xr3, src, 0x4);
        S32LDIR(xr4, src, 0x4);
        S32LDIR(xr5, src, 0x4);
        S32LDIR(xr6, src, 0x4);
        S32LDIR(xr7, src, 0x4);
        S32LDIR(xr8, src, 0x4);
        S32SDI(xr1, dst, 0x4);
        S32SDI(xr2, dst, 0x4);
        S32SDI(xr3, dst, 0x4);
        S32SDI(xr4, dst, 0x4);
        S32SDI(xr5, dst, 0x4);
        S32SDI(xr6, dst, 0x4);
        S32SDI(xr7, dst, 0x4);
        S32SDI(xr8, dst, 0x4);
    }
    for(;i<w; i++){
        S32LDIR(xr8, src, 0x4);
        S32SDI(xr8, dst, 0x4);
    }
#else
    for(i=0; i+8<=w; i+=8){
        dst[i+0]= bswap_32(src[i+0]);
        dst[i+1]= bswap_32(src[i+1]);
        dst[i+2]= bswap_32(src[i+2]);
        dst[i+3]= bswap_32(src[i+3]);
        dst[i+4]= bswap_32(src[i+4]);
        dst[i+5]= bswap_32(src[i+5]);
        dst[i+6]= bswap_32(src[i+6]);
        dst[i+7]= bswap_32(src[i+7]);
    }
    for(;i<w; i++){
        dst[i+0]= bswap_32(src[i+0]);
    }
#endif
}

static int sse4_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h)
{
    int s, i;
    uint32_t *sq = ff_squareTbl + 256;

    s = 0;
    for (i = 0; i < h; i++) {
        s += sq[pix1[0] - pix2[0]];
        s += sq[pix1[1] - pix2[1]];
        s += sq[pix1[2] - pix2[2]];
        s += sq[pix1[3] - pix2[3]];
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}

static int sse8_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h)
{
    int s, i;
    uint32_t *sq = ff_squareTbl + 256;

    s = 0;
    for (i = 0; i < h; i++) {
        s += sq[pix1[0] - pix2[0]];
        s += sq[pix1[1] - pix2[1]];
        s += sq[pix1[2] - pix2[2]];
        s += sq[pix1[3] - pix2[3]];
        s += sq[pix1[4] - pix2[4]];
        s += sq[pix1[5] - pix2[5]];
        s += sq[pix1[6] - pix2[6]];
        s += sq[pix1[7] - pix2[7]];
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}

static int sse16_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    uint32_t *sq = ff_squareTbl + 256;

    s = 0;
    for (i = 0; i < h; i++) {
        s += sq[pix1[ 0] - pix2[ 0]];
        s += sq[pix1[ 1] - pix2[ 1]];
        s += sq[pix1[ 2] - pix2[ 2]];
        s += sq[pix1[ 3] - pix2[ 3]];
        s += sq[pix1[ 4] - pix2[ 4]];
        s += sq[pix1[ 5] - pix2[ 5]];
        s += sq[pix1[ 6] - pix2[ 6]];
        s += sq[pix1[ 7] - pix2[ 7]];
        s += sq[pix1[ 8] - pix2[ 8]];
        s += sq[pix1[ 9] - pix2[ 9]];
        s += sq[pix1[10] - pix2[10]];
        s += sq[pix1[11] - pix2[11]];
        s += sq[pix1[12] - pix2[12]];
        s += sq[pix1[13] - pix2[13]];
        s += sq[pix1[14] - pix2[14]];
        s += sq[pix1[15] - pix2[15]];

        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}


#ifdef CONFIG_SNOW_ENCODER //dwt is in snow.c
static inline int w_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int w, int h, int type){
    int s, i, j;
    const int dec_count= w==8 ? 3 : 4;
    int tmp[32*32];
    int level, ori;
    static const int scale[2][2][4][4]={
      {
        {
            // 9/7 8x8 dec=3
            {268, 239, 239, 213},
            {  0, 224, 224, 152},
            {  0, 135, 135, 110},
        },{
            // 9/7 16x16 or 32x32 dec=4
            {344, 310, 310, 280},
            {  0, 320, 320, 228},
            {  0, 175, 175, 136},
            {  0, 129, 129, 102},
        }
      },{
        {
            // 5/3 8x8 dec=3
            {275, 245, 245, 218},
            {  0, 230, 230, 156},
            {  0, 138, 138, 113},
        },{
            // 5/3 16x16 or 32x32 dec=4
            {352, 317, 317, 286},
            {  0, 328, 328, 233},
            {  0, 180, 180, 140},
            {  0, 132, 132, 105},
        }
      }
    };

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j+=4) {
            tmp[32*i+j+0] = (pix1[j+0] - pix2[j+0])<<4;
            tmp[32*i+j+1] = (pix1[j+1] - pix2[j+1])<<4;
            tmp[32*i+j+2] = (pix1[j+2] - pix2[j+2])<<4;
            tmp[32*i+j+3] = (pix1[j+3] - pix2[j+3])<<4;
        }
        pix1 += line_size;
        pix2 += line_size;
    }

    ff_spatial_dwt(tmp, w, h, 32, type, dec_count);

    s=0;
    assert(w==h);
    for(level=0; level<dec_count; level++){
        for(ori= level ? 1 : 0; ori<4; ori++){
            int size= w>>(dec_count-level);
            int sx= (ori&1) ? size : 0;
            int stride= 32<<(dec_count-level);
            int sy= (ori&2) ? stride>>1 : 0;

            for(i=0; i<size; i++){
                for(j=0; j<size; j++){
                    int v= tmp[sx + sy + i*stride + j] * scale[type][dec_count-3][level][ori];
                    s += FFABS(v);
                }
            }
        }
    }
    assert(s>=0);
    return s>>9;
}

static int w53_8_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size,  8, h, 1);
}

static int w97_8_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size,  8, h, 0);
}

static int w53_16_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size, 16, h, 1);
}

static int w97_16_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size, 16, h, 0);
}

int w53_32_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size, 32, h, 1);
}

int w97_32_c(void *v, uint8_t * pix1, uint8_t * pix2, int line_size, int h){
    return w_c(v, pix1, pix2, line_size, 32, h, 0);
}
#endif

static void get_pixels_c(DCTELEM *restrict block, const uint8_t *pixels, int line_size)
{
    int i;

    /* read the pixels */
    for(i=0;i<8;i++) {
        block[0] = pixels[0];
        block[1] = pixels[1];
        block[2] = pixels[2];
        block[3] = pixels[3];
        block[4] = pixels[4];
        block[5] = pixels[5];
        block[6] = pixels[6];
        block[7] = pixels[7];
        pixels += line_size;
        block += 8;
    }
}

static void diff_pixels_c(DCTELEM *restrict block, const uint8_t *s1,
                          const uint8_t *s2, int stride){
    int i;

    /* read the pixels */
    for(i=0;i<8;i++) {
        block[0] = s1[0] - s2[0];
        block[1] = s1[1] - s2[1];
        block[2] = s1[2] - s2[2];
        block[3] = s1[3] - s2[3];
        block[4] = s1[4] - s2[4];
        block[5] = s1[5] - s2[5];
        block[6] = s1[6] - s2[6];
        block[7] = s1[7] - s2[7];
        s1 += stride;
        s2 += stride;
        block += 8;
    }
}


static void put_pixels_clamped_c(const DCTELEM *block, uint8_t *restrict pixels,
                                 int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<8;i++) {
        pixels[0] = cm[block[0]];
        pixels[1] = cm[block[1]];
        pixels[2] = cm[block[2]];
        pixels[3] = cm[block[3]];
        pixels[4] = cm[block[4]];
        pixels[5] = cm[block[5]];
        pixels[6] = cm[block[6]];
        pixels[7] = cm[block[7]];

        pixels += line_size;
        block += 8;
    }
}

static void put_pixels_clamped4_c(const DCTELEM *block, uint8_t *restrict pixels,
                                 int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<4;i++) {
        pixels[0] = cm[block[0]];
        pixels[1] = cm[block[1]];
        pixels[2] = cm[block[2]];
        pixels[3] = cm[block[3]];

        pixels += line_size;
        block += 8;
    }
}

static void put_pixels_clamped2_c(const DCTELEM *block, uint8_t *restrict pixels,
                                 int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<2;i++) {
        pixels[0] = cm[block[0]];
        pixels[1] = cm[block[1]];

        pixels += line_size;
        block += 8;
    }
}

static void put_signed_pixels_clamped_c(const DCTELEM *block,
                                        uint8_t *restrict pixels,
                                        int line_size)
{
    int i, j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (*block < -128)
                *pixels = 0;
            else if (*block > 127)
                *pixels = 255;
            else
                *pixels = (uint8_t)(*block + 128);
            block++;
            pixels++;
        }
        pixels += (line_size - 8);
    }
}

static void add_pixels_clamped_c(const DCTELEM *block, uint8_t *restrict pixels,
                          int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<8;i++) {
        pixels[0] = cm[pixels[0] + block[0]];
        pixels[1] = cm[pixels[1] + block[1]];
        pixels[2] = cm[pixels[2] + block[2]];
        pixels[3] = cm[pixels[3] + block[3]];
        pixels[4] = cm[pixels[4] + block[4]];
        pixels[5] = cm[pixels[5] + block[5]];
        pixels[6] = cm[pixels[6] + block[6]];
        pixels[7] = cm[pixels[7] + block[7]];
        pixels += line_size;
        block += 8;
    }
}

static void add_pixels_clamped4_c(const DCTELEM *block, uint8_t *restrict pixels,
                          int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<4;i++) {
        pixels[0] = cm[pixels[0] + block[0]];
        pixels[1] = cm[pixels[1] + block[1]];
        pixels[2] = cm[pixels[2] + block[2]];
        pixels[3] = cm[pixels[3] + block[3]];
        pixels += line_size;
        block += 8;
    }
}

static void add_pixels_clamped2_c(const DCTELEM *block, uint8_t *restrict pixels,
                          int line_size)
{
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    /* read the pixels */
    for(i=0;i<2;i++) {
        pixels[0] = cm[pixels[0] + block[0]];
        pixels[1] = cm[pixels[1] + block[1]];
        pixels += line_size;
        block += 8;
    }
}

static void add_pixels8_c(uint8_t *restrict pixels, DCTELEM *block, int line_size)
{
    int i;
    for(i=0;i<8;i++) {
        pixels[0] += block[0];
        pixels[1] += block[1];
        pixels[2] += block[2];
        pixels[3] += block[3];
        pixels[4] += block[4];
        pixels[5] += block[5];
        pixels[6] += block[6];
        pixels[7] += block[7];
        pixels += line_size;
        block += 8;
    }
}

static void add_pixels4_c(uint8_t *restrict pixels, DCTELEM *block, int line_size)
{
    int i;
    for(i=0;i<4;i++) {
        pixels[0] += block[0];
        pixels[1] += block[1];
        pixels[2] += block[2];
        pixels[3] += block[3];
        pixels += line_size;
        block += 4;
    }
}

static int sum_abs_dctelem_c(DCTELEM *block)
{
    int sum=0, i;
    for(i=0; i<64; i++)
        sum+= FFABS(block[i]);
    return sum;
}
#ifdef JZ4740_MXU_OPT
static void put_pixels8_c(uint8_t *dst, const uint8_t *src, int stride, int h){
  
  uint32_t src_aln,src_rs;
  int i;
  src_aln = (((uint32_t)src - stride) & 0xfffffffc);
  src_rs = 4 - ((uint32_t)src & 3);
  dst -= stride;
  for(i=0; i<h; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDD(xr2,src_aln,0x4);
    S32LDD(xr3,src_aln,0x8);
    S32ALN(xr1,xr2,xr1,src_rs); 
    S32ALN(xr2,xr3,xr2,src_rs); 
    S32SDIV(xr1,dst,stride,0);
    S32STD(xr2,dst,4);
  }
}
#else
static void put_pixels8_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){
    int i;
    uint8_t *dst,*src;
    dst = block;
    src = pixels;
    for(i=0; i<h; i++){
       dst[0]= src[0];
       dst[1]= src[1];
       dst[2]= src[2];
       dst[3]= src[3];     
       dst[4]= src[4];
       dst[5]= src[5];
       dst[6]= src[6];
       dst[7]= src[7];
       src += line_size;
       dst += line_size;
    }
}

#endif
static void avg_pixels8_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){
    int i;
    for(i=0; i<h; i++){
        *((uint32_t*)(block  ))=  rnd_avg32(*((uint32_t*)(block  )),  AV_RN32(pixels ));
        *((uint32_t*)(block+4))=  rnd_avg32(*((uint32_t*)(block+4)), AV_RN32(pixels+4));
        pixels+=line_size;
        block +=line_size;
    }

}

#ifdef JZ4740_MXU_OPT
 static void put_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2,
                                      int dst_stride, int src_stride1, int src_stride2, int h){
    int i;
    uint32_t src_aln1, src_rs1, src_aln2, src_rs2;

    src_aln1 = (uint32_t)src1 & 0xfffffffc;
    src_rs1 = 4 - ((uint32_t)src1 & 3);
    src_aln2 = (uint32_t)src2 & 0xfffffffc;
    src_rs2 = 4 - ((uint32_t)src2 & 3);

     dst -= dst_stride;
     src_aln1 -= src_stride1;
     src_aln2 -= src_stride2;
    
     for(i=0; i<h; i++) {
      S32LDIV(xr1, src_aln1, src_stride1, 0);
      S32LDD(xr2, src_aln1, 4);
      S32LDD(xr3, src_aln1, 8);
      S32LDIV(xr4, src_aln2, src_stride2, 0);
      S32LDD(xr5, src_aln2, 4);
      S32LDD(xr6, src_aln2, 8);

      S32ALN(xr1, xr2, xr1, src_rs1);
      S32ALN(xr4, xr5, xr4, src_rs2);
      Q8AVGR(xr1, xr4, xr1);

      S32ALN(xr2, xr3, xr2, src_rs1);
      S32ALN(xr5, xr6, xr5, src_rs2);
      Q8AVGR(xr2, xr5, xr2);

      S32SDIV(xr1, dst, dst_stride, 0);
      S32STD(xr2, dst, 4);
    }
}
#else
static  void put_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride,
int src_stride1, int src_stride2, int h){
    int i;
    for(i=0; i<h; i++){
        uint32_t a,b;
        a= AV_RN32(&src1[i*src_stride1  ]);
        b= AV_RN32(&src2[i*src_stride2  ]);
        *((uint32_t*)&dst[i*dst_stride  ])= rnd_avg32(a, b);
        a= AV_RN32(&src1[i*src_stride1+4]);
        b= AV_RN32(&src2[i*src_stride2+4]);
        *((uint32_t*)&dst[i*dst_stride+4])= rnd_avg32(a, b);
    }
}
#endif
static  void avg_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride,
int src_stride1, int src_stride2, int h){
    int i;
    for(i=0; i<h; i++){
        uint32_t a,b;
        a= AV_RN32(&src1[i*src_stride1  ]);
        b= AV_RN32(&src2[i*src_stride2  ]);
        *((uint32_t*)&dst[i*dst_stride  ])= rnd_avg32(*((uint32_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));
        a= AV_RN32(&src1[i*src_stride1+4]);
        b= AV_RN32(&src2[i*src_stride2+4]);
        *((uint32_t*)&dst[i*dst_stride+4])= rnd_avg32(*((uint32_t*)&dst[i*dst_stride+4]), rnd_avg32(a, b));
    }
}

#if 0

#define PIXOP2(OPNAME, OP) \
static void OPNAME ## _pixels(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint64_t*)block), AV_RN64(pixels));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
\
static void OPNAME ## _no_rnd_pixels_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int i;\
    for(i=0; i<h; i++){\
        const uint64_t a= AV_RN64(pixels  );\
        const uint64_t b= AV_RN64(pixels+1);\
        OP(*((uint64_t*)block), (a&b) + (((a^b)&0xFEFEFEFEFEFEFEFEULL)>>1));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
\
static void OPNAME ## _pixels_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int i;\
    for(i=0; i<h; i++){\
        const uint64_t a= AV_RN64(pixels  );\
        const uint64_t b= AV_RN64(pixels+1);\
        OP(*((uint64_t*)block), (a|b) - (((a^b)&0xFEFEFEFEFEFEFEFEULL)>>1));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
\
static void OPNAME ## _no_rnd_pixels_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int i;\
    for(i=0; i<h; i++){\
        const uint64_t a= AV_RN64(pixels          );\
        const uint64_t b= AV_RN64(pixels+line_size);\
        OP(*((uint64_t*)block), (a&b) + (((a^b)&0xFEFEFEFEFEFEFEFEULL)>>1));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
\
static void OPNAME ## _pixels_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int i;\
    for(i=0; i<h; i++){\
        const uint64_t a= AV_RN64(pixels          );\
        const uint64_t b= AV_RN64(pixels+line_size);\
        OP(*((uint64_t*)block), (a|b) - (((a^b)&0xFEFEFEFEFEFEFEFEULL)>>1));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
\
static void OPNAME ## _pixels_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
        int i;\
        const uint64_t a= AV_RN64(pixels  );\
        const uint64_t b= AV_RN64(pixels+1);\
        uint64_t l0=  (a&0x0303030303030303ULL)\
                    + (b&0x0303030303030303ULL)\
                    + 0x0202020202020202ULL;\
        uint64_t h0= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
                   + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
        uint64_t l1,h1;\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            uint64_t a= AV_RN64(pixels  );\
            uint64_t b= AV_RN64(pixels+1);\
            l1=  (a&0x0303030303030303ULL)\
               + (b&0x0303030303030303ULL);\
            h1= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
              + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
            OP(*((uint64_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0F0F0F0F0FULL));\
            pixels+=line_size;\
            block +=line_size;\
            a= AV_RN64(pixels  );\
            b= AV_RN64(pixels+1);\
            l0=  (a&0x0303030303030303ULL)\
               + (b&0x0303030303030303ULL)\
               + 0x0202020202020202ULL;\
            h0= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
              + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
            OP(*((uint64_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0F0F0F0F0FULL));\
            pixels+=line_size;\
            block +=line_size;\
        }\
}\
\
static void OPNAME ## _no_rnd_pixels_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
        int i;\
        const uint64_t a= AV_RN64(pixels  );\
        const uint64_t b= AV_RN64(pixels+1);\
        uint64_t l0=  (a&0x0303030303030303ULL)\
                    + (b&0x0303030303030303ULL)\
                    + 0x0101010101010101ULL;\
        uint64_t h0= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
                   + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
        uint64_t l1,h1;\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            uint64_t a= AV_RN64(pixels  );\
            uint64_t b= AV_RN64(pixels+1);\
            l1=  (a&0x0303030303030303ULL)\
               + (b&0x0303030303030303ULL);\
            h1= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
              + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
            OP(*((uint64_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0F0F0F0F0FULL));\
            pixels+=line_size;\
            block +=line_size;\
            a= AV_RN64(pixels  );\
            b= AV_RN64(pixels+1);\
            l0=  (a&0x0303030303030303ULL)\
               + (b&0x0303030303030303ULL)\
               + 0x0101010101010101ULL;\
            h0= ((a&0xFCFCFCFCFCFCFCFCULL)>>2)\
              + ((b&0xFCFCFCFCFCFCFCFCULL)>>2);\
            OP(*((uint64_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0F0F0F0F0FULL));\
            pixels+=line_size;\
            block +=line_size;\
        }\
}\
\
CALL_2X_PIXELS(OPNAME ## _pixels16_c    , OPNAME ## _pixels_c    , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_x2_c , OPNAME ## _pixels_x2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_y2_c , OPNAME ## _pixels_y2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_xy2_c, OPNAME ## _pixels_xy2_c, 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_x2_c , OPNAME ## _no_rnd_pixels_x2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_y2_c , OPNAME ## _no_rnd_pixels_y2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_xy2_c, OPNAME ## _no_rnd_pixels_xy2_c, 8)

#define op_avg(a, b) a = ( ((a)|(b)) - ((((a)^(b))&0xFEFEFEFEFEFEFEFEULL)>>1) )
#else // 64 bit variant

#define PIXOP2(OPNAME, OP) \
static void OPNAME ## _pixels2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint16_t*)(block  )), AV_RN16(pixels  ));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static void OPNAME ## _pixels4_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint32_t*)(block  )), AV_RN32(pixels  ));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static inline void OPNAME ## _no_rnd_pixels8_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels8_c(block, pixels, line_size, h);\
}\
\
static inline void OPNAME ## _no_rnd_pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN32(&src1[i*src_stride1  ]);\
        b= AV_RN32(&src2[i*src_stride2  ]);\
        OP(*((uint32_t*)&dst[i*dst_stride  ]), no_rnd_avg32(a, b));\
        a= AV_RN32(&src1[i*src_stride1+4]);\
        b= AV_RN32(&src2[i*src_stride2+4]);\
        OP(*((uint32_t*)&dst[i*dst_stride+4]), no_rnd_avg32(a, b));\
    }\
}\
\
static inline void OPNAME ## _pixels4_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN32(&src1[i*src_stride1  ]);\
        b= AV_RN32(&src2[i*src_stride2  ]);\
        OP(*((uint32_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));\
    }\
}\
\
static inline void OPNAME ## _pixels2_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN16(&src1[i*src_stride1  ]);\
        b= AV_RN16(&src2[i*src_stride2  ]);\
        OP(*((uint16_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));\
    }\
}\
\
static inline void OPNAME ## _pixels16_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    OPNAME ## _pixels8_l2(dst  , src1  , src2  , dst_stride, src_stride1, src_stride2, h);\
    OPNAME ## _pixels8_l2(dst+8, src1+8, src2+8, dst_stride, src_stride1, src_stride2, h);\
}\
\
static inline void OPNAME ## _no_rnd_pixels16_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    OPNAME ## _no_rnd_pixels8_l2(dst  , src1  , src2  , dst_stride, src_stride1, src_stride2, h);\
    OPNAME ## _no_rnd_pixels8_l2(dst+8, src1+8, src2+8, dst_stride, src_stride1, src_stride2, h);\
}\
\
static inline void OPNAME ## _no_rnd_pixels8_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _no_rnd_pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels8_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels8_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _no_rnd_pixels8_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _no_rnd_pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels8_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels8_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels8_l4(uint8_t *dst, const uint8_t *src1, uint8_t *src2, uint8_t *src3, uint8_t *src4,\
                 int dst_stride, int src_stride1, int src_stride2,int src_stride3,int src_stride4, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a, b, c, d, l0, l1, h0, h1;\
        a= AV_RN32(&src1[i*src_stride1]);\
        b= AV_RN32(&src2[i*src_stride2]);\
        c= AV_RN32(&src3[i*src_stride3]);\
        d= AV_RN32(&src4[i*src_stride4]);\
        l0=  (a&0x03030303UL)\
           + (b&0x03030303UL)\
           + 0x02020202UL;\
        h0= ((a&0xFCFCFCFCUL)>>2)\
          + ((b&0xFCFCFCFCUL)>>2);\
        l1=  (c&0x03030303UL)\
           + (d&0x03030303UL);\
        h1= ((c&0xFCFCFCFCUL)>>2)\
          + ((d&0xFCFCFCFCUL)>>2);\
        OP(*((uint32_t*)&dst[i*dst_stride]), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
        a= AV_RN32(&src1[i*src_stride1+4]);\
        b= AV_RN32(&src2[i*src_stride2+4]);\
        c= AV_RN32(&src3[i*src_stride3+4]);\
        d= AV_RN32(&src4[i*src_stride4+4]);\
        l0=  (a&0x03030303UL)\
           + (b&0x03030303UL)\
           + 0x02020202UL;\
        h0= ((a&0xFCFCFCFCUL)>>2)\
          + ((b&0xFCFCFCFCUL)>>2);\
        l1=  (c&0x03030303UL)\
           + (d&0x03030303UL);\
        h1= ((c&0xFCFCFCFCUL)>>2)\
          + ((d&0xFCFCFCFCUL)>>2);\
        OP(*((uint32_t*)&dst[i*dst_stride+4]), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
    }\
}\
\
static inline void OPNAME ## _pixels4_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels4_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels4_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels4_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels2_x2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels2_l2(block, pixels, pixels+1, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _pixels2_y2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    OPNAME ## _pixels2_l2(block, pixels, pixels+line_size, line_size, line_size, line_size, h);\
}\
\
static inline void OPNAME ## _no_rnd_pixels8_l4(uint8_t *dst, const uint8_t *src1, uint8_t *src2, uint8_t *src3, uint8_t *src4,\
                 int dst_stride, int src_stride1, int src_stride2,int src_stride3,int src_stride4, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a, b, c, d, l0, l1, h0, h1;\
        a= AV_RN32(&src1[i*src_stride1]);\
        b= AV_RN32(&src2[i*src_stride2]);\
        c= AV_RN32(&src3[i*src_stride3]);\
        d= AV_RN32(&src4[i*src_stride4]);\
        l0=  (a&0x03030303UL)\
           + (b&0x03030303UL)\
           + 0x01010101UL;\
        h0= ((a&0xFCFCFCFCUL)>>2)\
          + ((b&0xFCFCFCFCUL)>>2);\
        l1=  (c&0x03030303UL)\
           + (d&0x03030303UL);\
        h1= ((c&0xFCFCFCFCUL)>>2)\
          + ((d&0xFCFCFCFCUL)>>2);\
        OP(*((uint32_t*)&dst[i*dst_stride]), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
        a= AV_RN32(&src1[i*src_stride1+4]);\
        b= AV_RN32(&src2[i*src_stride2+4]);\
        c= AV_RN32(&src3[i*src_stride3+4]);\
        d= AV_RN32(&src4[i*src_stride4+4]);\
        l0=  (a&0x03030303UL)\
           + (b&0x03030303UL)\
           + 0x01010101UL;\
        h0= ((a&0xFCFCFCFCUL)>>2)\
          + ((b&0xFCFCFCFCUL)>>2);\
        l1=  (c&0x03030303UL)\
           + (d&0x03030303UL);\
        h1= ((c&0xFCFCFCFCUL)>>2)\
          + ((d&0xFCFCFCFCUL)>>2);\
        OP(*((uint32_t*)&dst[i*dst_stride+4]), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
    }\
}\
static inline void OPNAME ## _pixels16_l4(uint8_t *dst, const uint8_t *src1, uint8_t *src2, uint8_t *src3, uint8_t *src4,\
                 int dst_stride, int src_stride1, int src_stride2,int src_stride3,int src_stride4, int h){\
    OPNAME ## _pixels8_l4(dst  , src1  , src2  , src3  , src4  , dst_stride, src_stride1, src_stride2, src_stride3, src_stride4, h);\
    OPNAME ## _pixels8_l4(dst+8, src1+8, src2+8, src3+8, src4+8, dst_stride, src_stride1, src_stride2, src_stride3, src_stride4, h);\
}\
static inline void OPNAME ## _no_rnd_pixels16_l4(uint8_t *dst, const uint8_t *src1, uint8_t *src2, uint8_t *src3, uint8_t *src4,\
                 int dst_stride, int src_stride1, int src_stride2,int src_stride3,int src_stride4, int h){\
    OPNAME ## _no_rnd_pixels8_l4(dst  , src1  , src2  , src3  , src4  , dst_stride, src_stride1, src_stride2, src_stride3, src_stride4, h);\
    OPNAME ## _no_rnd_pixels8_l4(dst+8, src1+8, src2+8, src3+8, src4+8, dst_stride, src_stride1, src_stride2, src_stride3, src_stride4, h);\
}\
\
static inline void OPNAME ## _pixels2_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
        int i, a0, b0, a1, b1;\
        a0= pixels[0];\
        b0= pixels[1] + 2;\
        a0 += b0;\
        b0 += pixels[2];\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            a1= pixels[0];\
            b1= pixels[1];\
            a1 += b1;\
            b1 += pixels[2];\
\
            block[0]= (a1+a0)>>2; /* FIXME non put */\
            block[1]= (b1+b0)>>2;\
\
            pixels+=line_size;\
            block +=line_size;\
\
            a0= pixels[0];\
            b0= pixels[1] + 2;\
            a0 += b0;\
            b0 += pixels[2];\
\
            block[0]= (a1+a0)>>2;\
            block[1]= (b1+b0)>>2;\
            pixels+=line_size;\
            block +=line_size;\
        }\
}\
\
static inline void OPNAME ## _pixels4_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
        int i;\
        const uint32_t a= AV_RN32(pixels  );\
        const uint32_t b= AV_RN32(pixels+1);\
        uint32_t l0=  (a&0x03030303UL)\
                    + (b&0x03030303UL)\
                    + 0x02020202UL;\
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2)\
                   + ((b&0xFCFCFCFCUL)>>2);\
        uint32_t l1,h1;\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            uint32_t a= AV_RN32(pixels  );\
            uint32_t b= AV_RN32(pixels+1);\
            l1=  (a&0x03030303UL)\
               + (b&0x03030303UL);\
            h1= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
            a= AV_RN32(pixels  );\
            b= AV_RN32(pixels+1);\
            l0=  (a&0x03030303UL)\
               + (b&0x03030303UL)\
               + 0x02020202UL;\
            h0= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
        }\
}\
\
static inline void OPNAME ## _pixels8_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int j;\
    for(j=0; j<2; j++){\
        int i;\
        const uint32_t a= AV_RN32(pixels  );\
        const uint32_t b= AV_RN32(pixels+1);\
        uint32_t l0=  (a&0x03030303UL)\
                    + (b&0x03030303UL)\
                    + 0x02020202UL;\
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2)\
                   + ((b&0xFCFCFCFCUL)>>2);\
        uint32_t l1,h1;\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            uint32_t a= AV_RN32(pixels  );\
            uint32_t b= AV_RN32(pixels+1);\
            l1=  (a&0x03030303UL)\
               + (b&0x03030303UL);\
            h1= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
            a= AV_RN32(pixels  );\
            b= AV_RN32(pixels+1);\
            l0=  (a&0x03030303UL)\
               + (b&0x03030303UL)\
               + 0x02020202UL;\
            h0= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
        }\
        pixels+=4-line_size*(h+1);\
        block +=4-line_size*h;\
    }\
}\
\
static inline void OPNAME ## _no_rnd_pixels8_xy2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h)\
{\
    int j;\
    for(j=0; j<2; j++){\
        int i;\
        const uint32_t a= AV_RN32(pixels  );\
        const uint32_t b= AV_RN32(pixels+1);\
        uint32_t l0=  (a&0x03030303UL)\
                    + (b&0x03030303UL)\
                    + 0x01010101UL;\
        uint32_t h0= ((a&0xFCFCFCFCUL)>>2)\
                   + ((b&0xFCFCFCFCUL)>>2);\
        uint32_t l1,h1;\
\
        pixels+=line_size;\
        for(i=0; i<h; i+=2){\
            uint32_t a= AV_RN32(pixels  );\
            uint32_t b= AV_RN32(pixels+1);\
            l1=  (a&0x03030303UL)\
               + (b&0x03030303UL);\
            h1= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
            a= AV_RN32(pixels  );\
            b= AV_RN32(pixels+1);\
            l0=  (a&0x03030303UL)\
               + (b&0x03030303UL)\
               + 0x01010101UL;\
            h0= ((a&0xFCFCFCFCUL)>>2)\
              + ((b&0xFCFCFCFCUL)>>2);\
            OP(*((uint32_t*)block), h0+h1+(((l0+l1)>>2)&0x0F0F0F0FUL));\
            pixels+=line_size;\
            block +=line_size;\
        }\
        pixels+=4-line_size*(h+1);\
        block +=4-line_size*h;\
    }\
}\
\
CALL_2X_PIXELS(OPNAME ## _pixels16_c  , OPNAME ## _pixels8_c  , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_x2_c , OPNAME ## _pixels8_x2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_y2_c , OPNAME ## _pixels8_y2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _pixels16_xy2_c, OPNAME ## _pixels8_xy2_c, 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_c  , OPNAME ## _pixels8_c         , 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_x2_c , OPNAME ## _no_rnd_pixels8_x2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_y2_c , OPNAME ## _no_rnd_pixels8_y2_c , 8)\
CALL_2X_PIXELS(OPNAME ## _no_rnd_pixels16_xy2_c, OPNAME ## _no_rnd_pixels8_xy2_c, 8)\

#define op_avg(a, b) a = rnd_avg32(a, b)
#endif
#define op_put(a, b) a = b

#ifdef JZ4740_MXU_OPT
//put_pixels2_mxu (need not op by MXU)
static void put_pixels4_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){
  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  for(i=0; i<h; i++) {
    S32LDD(xr1,src_aln,0);
    S32LDD(xr2,src_aln,4);
    src_aln += stride;

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
  
    S32SDIV(xr1,dst,stride,0);
  }
}

static void put_pixels8_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){
  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  src_aln -= stride;
  for(i=0; i<h; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDD(xr2,src_aln,4);
    S32LDD(xr3,src_aln,8);

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
    S32ALN(xr2,xr3,xr2,src_rs);  //xr2 <- src[7:4]
  
    S32SDIV(xr1,dst,stride,0);
    S32STD(xr2,dst,4);
  }
}

static void put_pixels16_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){

  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  src_aln -= stride;
  for(i=0; i<h; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDD(xr2,src_aln,4);
    S32LDD(xr3,src_aln,8);
    S32LDD(xr4,src_aln,12);
    S32LDD(xr5,src_aln,16);

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
    S32ALN(xr2,xr3,xr2,src_rs);  //xr2 <- src[7:4]
    S32ALN(xr3,xr4,xr3,src_rs);  //xr3 <- src[11:8]
    S32ALN(xr4,xr5,xr4,src_rs);  //xr2 <- src[16:12]
  
    S32SDIV(xr1,dst,stride,0);
    S32STD(xr2,dst,4);
    S32STD(xr3,dst,8);
    S32STD(xr4,dst,12);
  }
}

#endif

PIXOP2(avg, op_avg)
PIXOP2(put, op_put)
#undef op_avg
#undef op_put

#define avg2(a,b) ((a+b+1)>>1)
#define avg4(a,b,c,d) ((a+b+c+d+2)>>2)

static void put_no_rnd_pixels16_l2_c(uint8_t *dst, const uint8_t *a, const uint8_t *b, int stride, int h){
    put_no_rnd_pixels16_l2(dst, a, b, stride, stride, stride, h);
}

static void put_no_rnd_pixels8_l2_c(uint8_t *dst, const uint8_t *a, const uint8_t *b, int stride, int h){
    put_no_rnd_pixels8_l2(dst, a, b, stride, stride, stride, h);
}

static void gmc1_c(uint8_t *dst, uint8_t *src, int stride, int h, int x16, int y16, int rounder)
{
    const int A=(16-x16)*(16-y16);
    const int B=(   x16)*(16-y16);
    const int C=(16-x16)*(   y16);
    const int D=(   x16)*(   y16);
    int i;

    for(i=0; i<h; i++)
    {
        dst[0]= (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1] + rounder)>>8;
        dst[1]= (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2] + rounder)>>8;
        dst[2]= (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3] + rounder)>>8;
        dst[3]= (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4] + rounder)>>8;
        dst[4]= (A*src[4] + B*src[5] + C*src[stride+4] + D*src[stride+5] + rounder)>>8;
        dst[5]= (A*src[5] + B*src[6] + C*src[stride+5] + D*src[stride+6] + rounder)>>8;
        dst[6]= (A*src[6] + B*src[7] + C*src[stride+6] + D*src[stride+7] + rounder)>>8;
        dst[7]= (A*src[7] + B*src[8] + C*src[stride+7] + D*src[stride+8] + rounder)>>8;
        dst+= stride;
        src+= stride;
    }
}

void ff_gmc_c(uint8_t *dst, uint8_t *src, int stride, int h, int ox, int oy,
                  int dxx, int dxy, int dyx, int dyy, int shift, int r, int width, int height)
{
    int y, vx, vy;
    const int s= 1<<shift;

    width--;
    height--;

    for(y=0; y<h; y++){
        int x;

        vx= ox;
        vy= oy;
        for(x=0; x<8; x++){ //XXX FIXME optimize
            int src_x, src_y, frac_x, frac_y, index;

            src_x= vx>>16;
            src_y= vy>>16;
            frac_x= src_x&(s-1);
            frac_y= src_y&(s-1);
            src_x>>=shift;
            src_y>>=shift;

            if((unsigned)src_x < width){
                if((unsigned)src_y < height){
                    index= src_x + src_y*stride;
                    dst[y*stride + x]= (  (  src[index         ]*(s-frac_x)
                                           + src[index       +1]*   frac_x )*(s-frac_y)
                                        + (  src[index+stride  ]*(s-frac_x)
                                           + src[index+stride+1]*   frac_x )*   frac_y
                                        + r)>>(shift*2);
                }else{
                    index= src_x + av_clip(src_y, 0, height)*stride;
                    dst[y*stride + x]= ( (  src[index         ]*(s-frac_x)
                                          + src[index       +1]*   frac_x )*s
                                        + r)>>(shift*2);
                }
            }else{
                if((unsigned)src_y < height){
                    index= av_clip(src_x, 0, width) + src_y*stride;
                    dst[y*stride + x]= (  (  src[index         ]*(s-frac_y)
                                           + src[index+stride  ]*   frac_y )*s
                                        + r)>>(shift*2);
                }else{
                    index= av_clip(src_x, 0, width) + av_clip(src_y, 0, height)*stride;
                    dst[y*stride + x]=    src[index         ];
                }
            }

            vx+= dxx;
            vy+= dyx;
        }
        ox += dxy;
        oy += dyy;
    }
}

static inline void put_tpel_pixels_mc00_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    switch(width){
    case 2: put_pixels2_c (dst, src, stride, height); break;
    case 4: put_pixels4_c (dst, src, stride, height); break;
    case 8: put_pixels8_c (dst, src, stride, height); break;
    case 16:put_pixels16_c(dst, src, stride, height); break;
    }
}

static inline void put_tpel_pixels_mc10_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (683*(2*src[j] + src[j+1] + 1)) >> 11;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc20_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (683*(src[j] + 2*src[j+1] + 1)) >> 11;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc01_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (683*(2*src[j] + src[j+stride] + 1)) >> 11;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc11_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (2731*(4*src[j] + 3*src[j+1] + 3*src[j+stride] + 2*src[j+stride+1] + 6)) >> 15;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc12_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (2731*(3*src[j] + 2*src[j+1] + 4*src[j+stride] + 3*src[j+stride+1] + 6)) >> 15;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc02_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (683*(src[j] + 2*src[j+stride] + 1)) >> 11;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc21_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (2731*(3*src[j] + 4*src[j+1] + 2*src[j+stride] + 3*src[j+stride+1] + 6)) >> 15;
      }
      src += stride;
      dst += stride;
    }
}

static inline void put_tpel_pixels_mc22_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (2731*(2*src[j] + 3*src[j+1] + 3*src[j+stride] + 4*src[j+stride+1] + 6)) >> 15;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc00_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    switch(width){
    case 2: avg_pixels2_c (dst, src, stride, height); break;
    case 4: avg_pixels4_c (dst, src, stride, height); break;
    case 8: avg_pixels8_c (dst, src, stride, height); break;
    case 16:avg_pixels16_c(dst, src, stride, height); break;
    }
}

static inline void avg_tpel_pixels_mc10_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((683*(2*src[j] + src[j+1] + 1)) >> 11) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc20_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((683*(src[j] + 2*src[j+1] + 1)) >> 11) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc01_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((683*(2*src[j] + src[j+stride] + 1)) >> 11) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc11_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((2731*(4*src[j] + 3*src[j+1] + 3*src[j+stride] + 2*src[j+stride+1] + 6)) >> 15) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc12_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((2731*(3*src[j] + 2*src[j+1] + 4*src[j+stride] + 3*src[j+stride+1] + 6)) >> 15) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc02_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((683*(src[j] + 2*src[j+stride] + 1)) >> 11) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc21_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((2731*(3*src[j] + 4*src[j+1] + 2*src[j+stride] + 3*src[j+stride+1] + 6)) >> 15) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}

static inline void avg_tpel_pixels_mc22_c(uint8_t *dst, const uint8_t *src, int stride, int width, int height){
    int i,j;
    for (i=0; i < height; i++) {
      for (j=0; j < width; j++) {
        dst[j] = (dst[j] + ((2731*(2*src[j] + 3*src[j+1] + 3*src[j+stride] + 4*src[j+stride+1] + 6)) >> 15) + 1) >> 1;
      }
      src += stride;
      dst += stride;
    }
}
#ifdef JZ4740_MXU_OPT
static void put_mpeg4_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){
  int i;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int ct0;
  mul_ct0=0x14141414;   //20 20 20 20
  mul_ct1=0x06060606;   // 6  6  6  6
  mul_ct2=0x03030303;   // 3  3  3  3
  mul_ct3=0x01010101;   // 1  1  1  1
      ct0=0x00100010;   //   16    16
  src_aln0 = (((uint32_t)src - srcStride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src) & 3);
  S32I2M(xr15,mul_ct0); //xr15:20 20 20 20
  S32I2M(xr14,mul_ct1); //xr14:6  6  6  6
  S32I2M(xr13,mul_ct2); //xr13:3  3  3  3
  S32I2M(xr12,mul_ct3); //xr12:1  1  1  1
  S32I2M(xr11,    ct0); //xr11:  16    16
  dst -= dstStride;
  for(i=0; i<h; i++)
  {
  S32LDIV(xr1,src_aln0,srcStride,0);
  S32LDD(xr2,src_aln0,4);
  S32LDD(xr3,src_aln0,8);
  S32ALN(xr1,xr2,xr1,src_rs0);  //xr1:src[3] src[2] src[1] src[0]
  S32ALN(xr2,xr3,xr2,src_rs0);  //xr2:src[7] src[6] src[5] src[4]  
  #ifdef JZ4750_OPT
  S32ALNI(xr5,xr1,xr1,1); //xr5:src[2] src[1] src[0] src[3]
  S32ALNI(xr6,xr1,xr1,2); //xr6:src[1] src[0] src[3] src[2]
  S32ALNI(xr7,xr1,xr1,3); //xr7:src[0] src[3] src[2] src[1]
  S32ALNI(xr8,xr1,xr7,1); //xr8:src[2] src[1] src[0] src[0]
  S32ALNI(xr9,xr8,xr6,1); //xr9:src[1] src[0] src[0] src[1]
  S32ALNI(xr10,xr9,xr8,1);//xr10: src[0] src[0] src[1] src[2]
  #else
  S32ALN(xr5,xr1,xr1,1); //xr5:src[2] src[1] src[0] src[3]
  S32ALN(xr6,xr1,xr1,2); //xr6:src[1] src[0] src[3] src[2]
  S32ALN(xr7,xr1,xr1,3); //xr7:src[0] src[3] src[2] src[1]
  S32ALN(xr8,xr1,xr7,1); //xr8:src[2] src[1] src[0] src[0]
  S32ALN(xr9,xr8,xr6,1); //xr9:src[1] src[0] src[0] src[1]
  S32ALN(xr10,xr9,xr8,1);//xr10: src[0] src[0] src[1] src[2]
  #endif
  Q8MUL(xr5,xr1,xr15,xr4); //xr5:src[3]*20 src[2]*20 xr4:src[1]*20 src[0]*20
  Q8MAC_SS(xr5,xr8,xr14,xr4);//xr5:src[3]*20-src[2]*6 src[2]*20-src[1]*6 
                             //xr4:src[1]*20-src[0]*6 src[0]*20-src[0]*6
  Q8MAC_AA(xr5,xr9,xr13,xr4);//xr5:xr3+src[1]*3 xr3+src[0]*3 xr4:xr4+src[0]*3 xr4+src[1]*3
  Q8MAC_SS(xr5,xr10,xr12,xr4);//xr5:xr3-src[0]  xr3-src[0]  xr4:xr4-src[1] xr4-src[2]
  #ifdef JZ4750_OPT
  S32ALNI(xr8,xr2,xr1,3);     //xr8:src[4] src[3] src[2] src[1]
  S32ALNI(xr9,xr2,xr1,2);     //xr9:src[5] src[4] src[3] src[2]
  S32ALNI(xr10,xr2,xr1,1);    //xr10:src[6]src[5] src[4] src[3]
  #else
  S32ALN(xr8,xr2,xr1,3);     //xr8:src[4] src[3] src[2] src[1]
  S32ALN(xr9,xr2,xr1,2);     //xr9:src[5] src[4] src[3] src[2]
  S32ALN(xr10,xr2,xr1,1);    //xr10:src[6]src[5] src[4] src[3]
  #endif
  Q8MAC_AA(xr5,xr8,xr15,xr4);//xr5:xr3+src[4]*20 xr3+src[3]*20 xr4:xr4+src[2]*20 xr4+src[1]*20
  Q8MAC_SS(xr5,xr9,xr14,xr4);//xr5:xr3-src[5]*6  xr3-src[4]*6  xr4:xr4-src[3]*6  xr4-src[2]*6
  Q8MAC_AA(xr5,xr10,xr13,xr4);
  Q8MAC_SS(xr5,xr2,xr12,xr4);

  Q16ACC_AA(xr5,xr11,xr0,xr4); 
  Q16SAR(xr5,xr5,xr4,xr4,5);
  Q16SAT(xr5,xr5,xr4);        //xr5:dst[3] dst[2] dst[1] dst[0]
 
  ///////////////////////////////////////////
  Q8MUL(xr1,xr2,xr15,xr4);    //xr1:src[7]*20 src[6]*20 xr4:src[5]*20 src[4]*20
  Q8MAC_SS(xr1,xr10,xr14,xr4);
  Q8MAC_AA(xr1,xr9 ,xr13,xr4);
  Q8MAC_SS(xr1,xr8 ,xr12,xr4);
  
  S32LDD(xr6,src_aln0,0xc);   
  S32ALN(xr3,xr6,xr3,src_rs0); //xr3:src[11] src[10] src[9] src[8]
  #ifdef JZ4750_OPT
  S32ALNI(xr7,xr3,xr2,1); //xr7:src[10] src[9]  src[8] src[7]
  S32ALNI(xr8,xr3,xr2,2); //xr8:src[9]  src[8]  src[7] src[6]
  S32ALNI(xr9,xr3,xr2,3); //xr9:src[8]  src[7]  src[6] src[5]
  #else
  S32ALN(xr7,xr3,xr2,1); //xr7:src[10] src[9]  src[8] src[7]
  S32ALN(xr8,xr3,xr2,2); //xr8:src[9]  src[8]  src[7] src[6]
  S32ALN(xr9,xr3,xr2,3); //xr9:src[8]  src[7]  src[6] src[5]
  #endif
  Q8MAC_AA(xr1,xr9,xr15,xr4);
  Q8MAC_SS(xr1,xr8,xr14,xr4);
  Q8MAC_AA(xr1,xr7,xr13,xr4);
  Q8MAC_SS(xr1,xr3,xr12,xr4);
  Q16ACC_AA(xr1,xr11,xr0,xr4);
  Q16SAR(xr1,xr1,xr4,xr4,5);
  Q16SAT(xr1,xr1,xr4);        //xr1:dst[7] dst[6] dst[5] dst[4]
  ///////////////////////////////////////////  
  Q8MUL(xr4,xr3,xr15,xr2);
  Q8MAC_SS(xr4,xr7,xr14,xr2);
  Q8MAC_AA(xr4,xr8,xr13,xr2);
  Q8MAC_SS(xr4,xr9,xr12,xr2);
  S32LDD(xr10,src_aln0,0x10); 
  S32ALN(xr6,xr10,xr6,src_rs0); //xr6:src[15] src[14] src[13] src[12]
  #ifdef JZ4750_OPT
  S32ALNI(xr7,xr6,xr3,1);//xr7 :src[14] src[13] src[12] src[11]
  S32ALNI(xr8,xr6,xr3,2);//xr8 :src[13] src[12] src[11] src[10]
  S32ALNI(xr9,xr6,xr3,3);//xr9 :src[12] src[11] src[10] src[9]
  #else
  S32ALN(xr7,xr6,xr3,1);//xr7 :src[14] src[13] src[12] src[11]
  S32ALN(xr8,xr6,xr3,2);//xr8 :src[13] src[12] src[11] src[10]
  S32ALN(xr9,xr6,xr3,3);//xr9 :src[12] src[11] src[10] src[9]
  #endif
  Q8MAC_AA(xr4,xr7,xr13,xr2);
  Q8MAC_SS(xr4,xr8,xr14,xr2);
  Q8MAC_SS(xr4,xr6,xr12,xr2);
  Q8MAC_AA(xr4,xr9,xr15,xr2);

  Q16ACC_AA(xr4,xr11,xr0,xr2);
  Q16SAR(xr4,xr4,xr2,xr2,5);
  Q16SAT(xr4,xr4,xr2);       //xr4:dst[11] dst[10] dst[9] dst[8]
  //////////////////////////////////////////////////
  Q8MUL(xr2,xr6,xr15,xr3);  //xr2:src[15]*20 src[14]*20 xr3:src[13]*20 src[12]*20
  Q8MAC_SS(xr2,xr7,xr14,xr3);
  Q8MAC_AA(xr2,xr8,xr13,xr3);
  Q8MAC_SS(xr2,xr9,xr12,xr3);
  S32LDD(xr7,src_aln0,0x14); 
  S32ALN(xr10,xr7,xr10,src_rs0);//xr10:src[19] src[18] src[17] src[16]
  #ifdef JZ4750_OPT
  S32ALNI(xr7,xr10,xr6,3); //xr7:src[16]src[15] src[14] src[13]
  S32ALNI(xr8,xr10,xr7, 3); //xr8:src[16]src[16]src[15] src[14]
  S32ALNI(xr9,xr6,xr6, 1); //xr9:src[14] src[13] src[12] src[15]
  #else
  S32ALN(xr7,xr10,xr6,3); //xr7:src[16]src[15] src[14] src[13]
  S32ALN(xr8,xr10,xr7, 3); //xr8:src[16]src[16]src[15] src[14]
  S32ALN(xr9,xr6,xr6, 1); //xr9:src[14] src[13] src[12] src[15]
  #endif
  Q8MAC_AA(xr2,xr7,xr15,xr3);
  Q8MAC_SS(xr2,xr8,xr14,xr3);
  #ifdef JZ4750_OPT
  S32ALNI(xr7,xr9,xr8,3);     //xr7:src[15] src[16] src[16] src[15]
  S32ALNI(xr8,xr8,xr7,3);     //xr8:src[14] src[15] src[16] src[16]
  #else
  S32ALN(xr7,xr9,xr8,3);     //xr7:src[15] src[16] src[16] src[15] 
  S32ALN(xr8,xr8,xr7,3);     //xr8:src[14] src[15] src[16] src[16]
  #endif
  Q8MAC_AA(xr2,xr7,xr13,xr3);
  Q8MAC_SS(xr2,xr8,xr12,xr3);
  Q16ACC_AA(xr2,xr11,xr0,xr3);
  Q16SAR(xr2,xr2,xr3,xr3,5);
  Q16SAT(xr2,xr2,xr3);      //xr2:dst[15] dst[14] dst[13] dst[12]  
  S32SDIV(xr5,dst,dstStride,0x0);
  S32STD(xr1,dst,0x4);
  S32STD(xr4,dst,0x8);
  S32STD(xr2,dst,0xc);
}
}
#else
static void put_mpeg4_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){
  uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
  int i;
  for(i=0; i<h; i++)
   {
   dst[ 0]=cm[(((src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]))+16)>>5];
   dst[ 1]=cm[(((src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]))+16)>>5];
   dst[ 2]=cm[(((src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]))+16)>>5];
   dst[ 3]=cm[(((src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]))+16)>>5];
   dst[ 4]=cm[(((src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]))+16)>>5];
   dst[ 5]=cm[(((src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]))+16)>>5];
   dst[ 6]=cm[(((src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]))+16)>>5];
   dst[ 7]=cm[(((src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]))+16)>>5];
   dst[ 8]=cm[(((src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]))+16)>>5];
   dst[ 9]=cm[(((src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]))+16)>>5];
   dst[10]=cm[(((src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]))+16)>>5];
   dst[11]=cm[(((src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]))+16)>>5];
   dst[12]=cm[(((src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]))+16)>>5];
   dst[13]=cm[(((src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]))+16)>>5];
   dst[14]=cm[(((src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]))+16)>>5];
   dst[15]=cm[(((src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]))+16)>>5];
   dst+=dstStride;
   src+=srcStride;
   }
}
#endif
static void put_no_rnd_mpeg4_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){
   uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
   int i;    
   for(i=0; i<h; i++)
   {
   dst[ 0]=cm[(((src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]))+15)>>5];
   dst[ 1]=cm[(((src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]))+15)>>5];
   dst[ 2]=cm[(((src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]))+15)>>5];
   dst[ 3]=cm[(((src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]))+15)>>5];
   dst[ 4]=cm[(((src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]))+15)>>5];
   dst[ 5]=cm[(((src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]))+15)>>5];
   dst[ 6]=cm[(((src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]))+15)>>5];
   dst[ 7]=cm[(((src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]))+15)>>5];
   dst[ 8]=cm[(((src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]))+15)>>5];
   dst[ 9]=cm[(((src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]))+15)>>5];
   dst[10]=cm[(((src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]))+15)>>5];
   dst[11]=cm[(((src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]))+15)>>5];
   dst[12]=cm[(((src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]))+15)>>5];
   dst[13]=cm[(((src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]))+15)>>5];
   dst[14]=cm[(((src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]))+15)>>5];
   dst[15]=cm[(((src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]))+15)>>5];
   dst+=dstStride;
   src+=srcStride;
   }
}

static void avg_mpeg4_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
   uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
   int i;
   for(i=0; i<h; i++)
   {
   dst[ 0]=(dst[0]+cm[(((src[ 0]+src[ 1])*20 - (src[ 0]+src[ 2])*6 + (src[ 1]+src[ 3])*3 - (src[ 2]+src[ 4]))+16)>>5]+1) >>1;
   dst[ 1]=(dst[1]+cm[(((src[ 1]+src[ 2])*20 - (src[ 0]+src[ 3])*6 + (src[ 0]+src[ 4])*3 - (src[ 1]+src[ 5]))+16)>>5]+1) >>1;
   dst[ 2]=(dst[2]+cm[(((src[ 2]+src[ 3])*20 - (src[ 1]+src[ 4])*6 + (src[ 0]+src[ 5])*3 - (src[ 0]+src[ 6]))+16)>>5]+1) >>1;
   dst[ 3]=(dst[3]+cm[(((src[ 3]+src[ 4])*20 - (src[ 2]+src[ 5])*6 + (src[ 1]+src[ 6])*3 - (src[ 0]+src[ 7]))+16)>>5]+1) >>1;
   dst[ 4]=(dst[4]+cm[(((src[ 4]+src[ 5])*20 - (src[ 3]+src[ 6])*6 + (src[ 2]+src[ 7])*3 - (src[ 1]+src[ 8]))+16)>>5]+1) >>1;
   dst[ 5]=(dst[5]+cm[(((src[ 5]+src[ 6])*20 - (src[ 4]+src[ 7])*6 + (src[ 3]+src[ 8])*3 - (src[ 2]+src[ 9]))+16)>>5]+1) >>1;
   dst[ 6]=(dst[6]+cm[(((src[ 6]+src[ 7])*20 - (src[ 5]+src[ 8])*6 + (src[ 4]+src[ 9])*3 - (src[ 3]+src[10]))+16)>>5]+1) >>1;
   dst[ 7]=(dst[7]+cm[(((src[ 7]+src[ 8])*20 - (src[ 6]+src[ 9])*6 + (src[ 5]+src[10])*3 - (src[ 4]+src[11]))+16)>>5]+1) >>1;
   dst[ 8]=(dst[8]+cm[(((src[ 8]+src[ 9])*20 - (src[ 7]+src[10])*6 + (src[ 6]+src[11])*3 - (src[ 5]+src[12]))+16)>>5]+1) >>1;
   dst[ 9]=(dst[9]+cm[(((src[ 9]+src[10])*20 - (src[ 8]+src[11])*6 + (src[ 7]+src[12])*3 - (src[ 6]+src[13]))+16)>>5]+1) >>1;
   dst[10]=(dst[10]+cm[(((src[10]+src[11])*20 - (src[ 9]+src[12])*6 + (src[ 8]+src[13])*3 - (src[ 7]+src[14]))+16)>>5]+1) >>1;
   dst[11]=(dst[11]+cm[(((src[11]+src[12])*20 - (src[10]+src[13])*6 + (src[ 9]+src[14])*3 - (src[ 8]+src[15]))+16)>>5]+1) >>1;
   dst[12]=(dst[12]+cm[(((src[12]+src[13])*20 - (src[11]+src[14])*6 + (src[10]+src[15])*3 - (src[ 9]+src[16]))+16)>>5]+1) >>1;
   dst[13]=(dst[13]+cm[(((src[13]+src[14])*20 - (src[12]+src[15])*6 + (src[11]+src[16])*3 - (src[10]+src[16]))+16)>>5]+1) >>1;
   dst[14]=(dst[14]+cm[(((src[14]+src[15])*20 - (src[13]+src[16])*6 + (src[12]+src[16])*3 - (src[11]+src[15]))+16)>>5]+1) >>1;
   dst[15]=(dst[15]+cm[(((src[15]+src[16])*20 - (src[14]+src[16])*6 + (src[13]+src[15])*3 - (src[12]+src[14]))+16)>>5]+1) >>1;
   dst+=dstStride;
   src+=srcStride;
   }
}
#ifdef JZ4740_MXU_OPT
static void put_mpeg4_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
  int i,j;
  uint32_t src_aln0,src_rs0,src_aln,dst_aln,dst_aln1;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int ct0;
  mul_ct0=0x14141414;   //20 20 20 20
  mul_ct1=0x06060606;   // 6  6  6  6
  mul_ct2=0x03030303;   // 3  3  3  3
  mul_ct3=0x01010101;   // 1  1  1  1
      ct0=0x00100010;   //   16    16

  src_aln0 = (((uint32_t)src) & 0xFFFFFFFC);
  src_aln  = src_aln0;
  src_rs0 = 4-(((uint32_t)src) & 3); 
  dst_aln = (uint32_t)dst;
  dst_aln1 = dst_aln;
  S32I2M(xr15,mul_ct0); //xr15:20 20 20 20
  S32I2M(xr14,mul_ct1); //xr14:6  6  6  6
  S32I2M(xr13,mul_ct2); //xr13:3  3  3  3
  S32I2M(xr12,mul_ct3); //xr12:1  1  1  1
  S32I2M(xr11,    ct0); //xr11:  16    16

  dst_aln -= dstStride;
 for(i=0; i<4; i++)
 { 
  S32LDD(xr1,src_aln0,0);
  S32LDD(xr2,src_aln0,4);
  S32ALN(xr1,xr2,xr1,src_rs0);   //xr1:src[3]~src[0]

  S32LDIV(xr3,src_aln0,srcStride,0);
  S32LDD(xr4,src_aln0,4);
  S32ALN(xr3,xr4,xr3,src_rs0);   //xr3:src[11]~src[8]

  S32LDIV(xr5,src_aln0,srcStride,0);
  S32LDD(xr6,src_aln0,4);
  S32ALN(xr5,xr6,xr5,src_rs0);   //xr5:src[19]~src[16]
 
  S32LDIV(xr7,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr7,xr8,xr7,src_rs0);   //xr7:src[27]~src[24]
  
  S32LDIV(xr9,src_aln0,srcStride,0);
  S32LDD(xr10,src_aln0,4);
  S32ALN(xr9,xr10,xr9,src_rs0);  //xr9:src[35]~src[32]
  
  Q8MUL(xr2,xr1,xr15,xr4);    
  Q8MAC_AA(xr2,xr3,xr15,xr4);
  Q8MAC_SS(xr2,xr1,xr14,xr4);
  Q8MAC_SS(xr2,xr5,xr14,xr4);
  Q8MAC_AA(xr2,xr3,xr13,xr4);
  Q8MAC_AA(xr2,xr7,xr13,xr4);
  Q8MAC_SS(xr2,xr5,xr12,xr4);
  Q8MAC_SS(xr2,xr9,xr12,xr4);
  
  Q16ACC_AA(xr2,xr11,xr0,xr4);
  Q16SAR(xr2,xr2,xr4,xr4,5);
  Q16SAT(xr2,xr2,xr4);       //xr2:dst[3]~dst[0] 
  S32SDIV(xr2,dst_aln,dstStride,0x0);
  ///////////////////////////////////////
  S32LDIV(xr2,src_aln0,srcStride,0);
  S32LDD(xr10,src_aln0,4);
  S32ALN(xr2,xr10,xr2,src_rs0);//xr2:src[43]~src[40]
  
  Q8MUL(xr4,xr3,xr15,xr8);
  Q8MAC_AA(xr4,xr5,xr15,xr8);
  Q8MAC_SS(xr4,xr1,xr14,xr8);
  Q8MAC_SS(xr4,xr7,xr14,xr8);
  Q8MAC_AA(xr4,xr1,xr13,xr8);
  Q8MAC_AA(xr4,xr9,xr13,xr8);
  Q8MAC_SS(xr4,xr3,xr12,xr8);
  Q8MAC_SS(xr4,xr2,xr12,xr8);
  
  Q16ACC_AA(xr4,xr11,xr0,xr8);
  Q16SAR(xr4,xr4,xr8,xr8,5);
  Q16SAT(xr4,xr4,xr8);       //xr4:dst[dstStride+3]~dst[dstStride]
  S32SDIV(xr4,dst_aln,dstStride,0x0);
 /////////////////////////////////
  S32LDIV(xr4,src_aln0,srcStride,0);
  S32LDD(xr10,src_aln0,4);
  S32ALN(xr4,xr10,xr4,src_rs0);//xr4:src[51]~src[48]

  Q8MUL(xr8,xr5,xr15,xr6);
  Q8MAC_AA(xr8,xr7,xr15,xr6);
  Q8MAC_SS(xr8,xr3,xr14,xr6);
  Q8MAC_SS(xr8,xr9,xr14,xr6);
  Q8MAC_AA(xr8,xr1,xr13,xr6);
  Q8MAC_AA(xr8,xr2,xr13,xr6);
  Q8MAC_SS(xr8,xr1,xr12,xr6);
  Q8MAC_SS(xr8,xr4,xr12,xr6);

  Q16ACC_AA(xr8,xr11,xr0,xr6);
  Q16SAR(xr8,xr8,xr6,xr6,5);
  Q16SAT(xr8,xr8,xr6);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
 ///////////////////////////four  line/////////
  S32LDIV(xr6,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr6,xr8,xr6,src_rs0);//xr6:src[59]~src[56]
  
  Q8MUL(xr8,xr7,xr15,xr10);
  Q8MAC_AA(xr8,xr9,xr15,xr10);
  Q8MAC_SS(xr8,xr5,xr14,xr10);
  Q8MAC_SS(xr8,xr2,xr14,xr10);
  Q8MAC_AA(xr8,xr3,xr13,xr10);
  Q8MAC_AA(xr8,xr4,xr13,xr10);
  Q8MAC_SS(xr8,xr1,xr12,xr10);
  Q8MAC_SS(xr8,xr6,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
  ////////////////////////five line/////////////
  S32LDIV(xr1,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr1,xr8,xr1,src_rs0);//xr1:src[67]~src[64]

  Q8MUL(xr8,xr9,xr15,xr10);
  Q8MAC_AA(xr8,xr2,xr15,xr10);
  Q8MAC_SS(xr8,xr7,xr14,xr10);
  Q8MAC_SS(xr8,xr4,xr14,xr10);
  Q8MAC_AA(xr8,xr5,xr13,xr10);
  Q8MAC_AA(xr8,xr6,xr13,xr10);
  Q8MAC_SS(xr8,xr3,xr12,xr10);
  Q8MAC_SS(xr8,xr1,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
  //////////////////////six line////////////////  
  S32LDIV(xr3,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr3,xr8,xr3,src_rs0);//xr3:src[75]~src[72]

  Q8MUL(xr8,xr2,xr15,xr10);
  Q8MAC_AA(xr8,xr4,xr15,xr10);
  Q8MAC_SS(xr8,xr9,xr14,xr10);
  Q8MAC_SS(xr8,xr6,xr14,xr10);
  Q8MAC_AA(xr8,xr7,xr13,xr10);
  Q8MAC_AA(xr8,xr1,xr13,xr10);
  Q8MAC_SS(xr8,xr5,xr12,xr10);
  Q8MAC_SS(xr8,xr3,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
 //////////////////////////////////////////////////
  S32LDIV(xr5,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr5,xr8,xr5,src_rs0);//xr5:src[83]~src[80]

  Q8MUL(xr8,xr4,xr15,xr10);
  Q8MAC_AA(xr8,xr6,xr15,xr10);
  Q8MAC_SS(xr8,xr2,xr14,xr10);
  Q8MAC_SS(xr8,xr1,xr14,xr10);
  Q8MAC_AA(xr8,xr9,xr13,xr10);
  Q8MAC_AA(xr8,xr3,xr13,xr10);
  Q8MAC_SS(xr8,xr7,xr12,xr10);
  Q8MAC_SS(xr8,xr5,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
 /////////////////////////////////////////////
  S32LDIV(xr7,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr7,xr8,xr7,src_rs0);//xr7:src[91]~src[88]

  Q8MUL(xr8,xr6,xr15,xr10);
  Q8MAC_AA(xr8,xr1,xr15,xr10);
  Q8MAC_SS(xr8,xr4,xr14,xr10);
  Q8MAC_SS(xr8,xr3,xr14,xr10);
  Q8MAC_AA(xr8,xr2,xr13,xr10);
  Q8MAC_AA(xr8,xr5,xr13,xr10);
  Q8MAC_SS(xr8,xr9,xr12,xr10);
  Q8MAC_SS(xr8,xr7,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
 ///////////////////////////////////////////
  S32LDIV(xr9,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr9,xr8,xr9,src_rs0);//xr9:src[99]~src[96]

  Q8MUL(xr8,xr1,xr15,xr10);
  Q8MAC_AA(xr8,xr3,xr15,xr10);
  Q8MAC_SS(xr8,xr6,xr14,xr10);
  Q8MAC_SS(xr8,xr5,xr14,xr10);
  Q8MAC_AA(xr8,xr4,xr13,xr10);
  Q8MAC_AA(xr8,xr7,xr13,xr10);
  Q8MAC_SS(xr8,xr2,xr12,xr10);
  Q8MAC_SS(xr8,xr9,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
  /////////////////////////////////////////////////
  S32LDIV(xr2,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr2,xr8,xr2,src_rs0);//xr2:src[91]~src[88]

  Q8MUL(xr8,xr3,xr15,xr10);
  Q8MAC_AA(xr8,xr5,xr15,xr10);
  Q8MAC_SS(xr8,xr1,xr14,xr10);
  Q8MAC_SS(xr8,xr7,xr14,xr10);
  Q8MAC_AA(xr8,xr6,xr13,xr10);
  Q8MAC_AA(xr8,xr9,xr13,xr10);
  Q8MAC_SS(xr8,xr4,xr12,xr10);
  Q8MAC_SS(xr8,xr2,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
  ///////////////////////////////////////////////////
  S32LDIV(xr4,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr4,xr8,xr4,src_rs0);//xr7:src[91]~src[88]

  Q8MUL(xr8,xr5,xr15,xr10);
  Q8MAC_AA(xr8,xr7,xr15,xr10);
  Q8MAC_SS(xr8,xr3,xr14,xr10);
  Q8MAC_SS(xr8,xr9,xr14,xr10);
  Q8MAC_AA(xr8,xr1,xr13,xr10);
  Q8MAC_AA(xr8,xr2,xr13,xr10);
  Q8MAC_SS(xr8,xr6,xr12,xr10);
  Q8MAC_SS(xr8,xr4,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
/////////////////////////////////////////////
  S32LDIV(xr6,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr6,xr8,xr6,src_rs0);//xr7:src[91]~src[88]

  Q8MUL(xr8,xr7,xr15,xr10);
  Q8MAC_AA(xr8,xr9,xr15,xr10);
  Q8MAC_SS(xr8,xr5,xr14,xr10);
  Q8MAC_SS(xr8,xr2,xr14,xr10);
  Q8MAC_AA(xr8,xr3,xr13,xr10);
  Q8MAC_AA(xr8,xr4,xr13,xr10);
  Q8MAC_SS(xr8,xr1,xr12,xr10);
  Q8MAC_SS(xr8,xr6,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
///////////////////////////////////////////
  S32LDIV(xr1,src_aln0,srcStride,0);
  S32LDD(xr8,src_aln0,4);
  S32ALN(xr1,xr8,xr1,src_rs0);//xr7:src[91]~src[88]

  Q8MUL(xr8,xr9,xr15,xr10);
  Q8MAC_AA(xr8,xr2,xr15,xr10);
  Q8MAC_SS(xr8,xr7,xr14,xr10);
  Q8MAC_SS(xr8,xr4,xr14,xr10);
  Q8MAC_AA(xr8,xr5,xr13,xr10);
  Q8MAC_AA(xr8,xr6,xr13,xr10);
  Q8MAC_SS(xr8,xr3,xr12,xr10);
  Q8MAC_SS(xr8,xr1,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
////////////////////////////////////////
  Q8MUL(xr8,xr2,xr15,xr10);
  Q8MAC_AA(xr8,xr4,xr15,xr10);
  Q8MAC_SS(xr8,xr9,xr14,xr10);
  Q8MAC_SS(xr8,xr6,xr14,xr10);
  Q8MAC_AA(xr8,xr7,xr13,xr10);
  Q8MAC_AA(xr8,xr1,xr13,xr10);
  Q8MAC_SS(xr8,xr5,xr12,xr10);
  Q8MAC_SS(xr8,xr1,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
///////////////////////////////////////////////////
  Q8MUL(xr8,xr4,xr15,xr10);
  Q8MAC_AA(xr8,xr6,xr15,xr10);
  Q8MAC_SS(xr8,xr2,xr14,xr10);
  Q8MAC_SS(xr8,xr1,xr14,xr10);
  Q8MAC_AA(xr8,xr9,xr13,xr10);
  Q8MAC_AA(xr8,xr1,xr13,xr10);
  Q8MAC_SS(xr8,xr7,xr12,xr10);
  Q8MAC_SS(xr8,xr6,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32SDIV(xr8,dst_aln,dstStride,0x0);
 ////////////////////////////////////////////////
  Q8MUL(xr8,xr6,xr15,xr10);
  Q8MAC_AA(xr8,xr1,xr15,xr10);
  Q8MAC_SS(xr8,xr4,xr14,xr10);
  Q8MAC_SS(xr8,xr1,xr14,xr10);
  Q8MAC_AA(xr8,xr2,xr13,xr10);
  Q8MAC_AA(xr8,xr6,xr13,xr10);
  Q8MAC_SS(xr8,xr9,xr12,xr10);
  Q8MAC_SS(xr8,xr4,xr12,xr10);

  Q16ACC_AA(xr8,xr11,xr0,xr10);
  Q16SAR(xr8,xr8,xr10,xr10,5);
  Q16SAT(xr8,xr8,xr10);       //xr8:dst[2*dstStride+3]~dst[2*dstStride]
  S32STD(xr8,dst_aln,0x0);
  src_aln0 = src_aln + 4;
  src_aln += 4;
  dst_aln = dst_aln1 +4;
  dst_aln1 += 4;
  } 
}
#else
static void put_mpeg4_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
 uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
 int i;
 const int w=16;
 for(i=0; i<w; i++)
 {
 const int src0= src[0*srcStride];
 const int src1= src[1*srcStride];
 const int src2= src[2*srcStride];
 const int src3= src[3*srcStride];
 const int src4= src[4*srcStride];
 const int src5= src[5*srcStride];
 const int src6= src[6*srcStride];
 const int src7= src[7*srcStride];
 const int src8= src[8*srcStride];
 const int src9= src[9*srcStride];
 const int src10= src[10*srcStride];
 const int src11= src[11*srcStride];
 const int src12= src[12*srcStride];
 const int src13= src[13*srcStride];
 const int src14= src[14*srcStride];
 const int src15= src[15*srcStride];
 const int src16= src[16*srcStride];
 dst[ 0*dstStride]= cm[(((src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ))+16)>>5];
 dst[ 1*dstStride]= cm[(((src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ))+16)>>5];
 dst[ 2*dstStride]= cm[(((src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ))+16)>>5];
 dst[ 3*dstStride]= cm[(((src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ))+16)>>5];
 
 dst[ 4*dstStride]= cm[(((src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ))+16)>>5];
 dst[ 5*dstStride]= cm[(((src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ))+16)>>5];
 dst[ 6*dstStride]= cm[(((src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10))+16)>>5];
 dst[ 7*dstStride]= cm[(((src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11))+16)>>5];
 
 dst[ 8*dstStride]= cm[(((src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12))+16)>>5];
 dst[ 9*dstStride]= cm[(((src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13))+16)>>5];
 dst[10*dstStride]= cm[(((src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14))+16)>>5];
 dst[11*dstStride]= cm[(((src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15))+16)>>5];
 
 dst[12*dstStride]= cm[(((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16))+16)>>5];
 dst[13*dstStride]= cm[(((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16))+16)>>5];
 dst[14*dstStride]= cm[(((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15))+16)>>5];
 dst[15*dstStride]= cm[(((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14))+16)>>5];
  
 dst++;
 src++;
 }
}
#endif
static void put_no_rnd_mpeg4_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
 uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
 int i;
 const int w=16;
 for(i=0; i<w; i++)
 {
 const int src0= src[0*srcStride];
 const int src1= src[1*srcStride];
 const int src2= src[2*srcStride];
 const int src3= src[3*srcStride];
 const int src4= src[4*srcStride];
 const int src5= src[5*srcStride];
 const int src6= src[6*srcStride];
 const int src7= src[7*srcStride];
 const int src8= src[8*srcStride];
 const int src9= src[9*srcStride];
 const int src10= src[10*srcStride];
 const int src11= src[11*srcStride];
 const int src12= src[12*srcStride];
 const int src13= src[13*srcStride];
 const int src14= src[14*srcStride];
 const int src15= src[15*srcStride];
 const int src16= src[16*srcStride];
 dst[ 0*dstStride]= cm[(((src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ))+15)>>5];
 dst[ 1*dstStride]= cm[(((src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ))+15)>>5];
 dst[ 2*dstStride]= cm[(((src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ))+15)>>5];
 dst[ 3*dstStride]= cm[(((src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ))+15)>>5];
 dst[ 4*dstStride]= cm[(((src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ))+15)>>5];
 dst[ 5*dstStride]= cm[(((src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ))+15)>>5];
 dst[ 6*dstStride]= cm[(((src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10))+15)>>5];
 dst[ 7*dstStride]= cm[(((src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11))+15)>>5];
 dst[ 8*dstStride]= cm[(((src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12))+15)>>5];
 dst[ 9*dstStride]= cm[(((src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13))+15)>>5];
 dst[10*dstStride]= cm[(((src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14))+15)>>5];
 dst[11*dstStride]= cm[(((src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15))+15)>>5];
 dst[12*dstStride]= cm[(((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16))+15)>>5];
 dst[13*dstStride]= cm[(((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16))+15)>>5];
 dst[14*dstStride]= cm[(((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15))+15)>>5];
 dst[15*dstStride]= cm[(((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14))+15)>>5];
 dst++;
 src++;
 }
}

static void avg_mpeg4_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
 uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
 int i;
 const int w=16;
 for(i=0; i<w; i++)
 {
 const int src0= src[0*srcStride];
 const int src1= src[1*srcStride];
 const int src2= src[2*srcStride];
 const int src3= src[3*srcStride];
 const int src4= src[4*srcStride];
 const int src5= src[5*srcStride];
 const int src6= src[6*srcStride];
 const int src7= src[7*srcStride];
 const int src8= src[8*srcStride];
 const int src9= src[9*srcStride];
 const int src10= src[10*srcStride];
 const int src11= src[11*srcStride];
 const int src12= src[12*srcStride];
 const int src13= src[13*srcStride];
 const int src14= src[14*srcStride];
 const int src15= src[15*srcStride];
 const int src16= src[16*srcStride];
 dst[ 0*dstStride]= (dst[ 0*dstStride]+cm[(((src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ))+16)>>5] + 1 )>>1;
 dst[ 1*dstStride]= (dst[ 1*dstStride]+cm[(((src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ))+16)>>5] + 1 )>>1;
 dst[ 2*dstStride]= (dst[ 2*dstStride]+cm[(((src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ))+16)>>5] + 1 )>>1;
 dst[ 3*dstStride]= (dst[ 3*dstStride]+cm[(((src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ))+16)>>5] + 1 )>>1;
 dst[ 4*dstStride]= (dst[ 4*dstStride]+cm[(((src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ))+16)>>5] + 1 )>>1;
 dst[ 5*dstStride]= (dst[ 5*dstStride]+cm[(((src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ))+16)>>5] + 1 )>>1;
 dst[ 6*dstStride]= (dst[ 6*dstStride]+cm[(((src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10))+16)>>5] + 1 )>>1;
 dst[ 7*dstStride]= (dst[ 7*dstStride]+cm[(((src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11))+16)>>5] + 1 )>>1;
 dst[ 8*dstStride]= (dst[ 8*dstStride]+cm[(((src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12))+16)>>5] + 1 )>>1;
 dst[ 9*dstStride]= (dst[ 9*dstStride]+cm[(((src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13))+16)>>5] + 1 )>>1;
 dst[10*dstStride]= (dst[10*dstStride]+cm[(((src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14))+16)>>5] + 1 )>>1;
 dst[11*dstStride]= (dst[11*dstStride]+cm[(((src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15))+16)>>5] + 1 )>>1;
 dst[12*dstStride]= (dst[12*dstStride]+cm[(((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16))+16)>>5] + 1 )>>1;
 dst[13*dstStride]= (dst[13*dstStride]+cm[(((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16))+16)>>5] + 1 )>>1;
 dst[14*dstStride]= (dst[14*dstStride]+cm[(((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15))+16)>>5] + 1 )>>1;
 dst[15*dstStride]= (dst[15*dstStride]+cm[(((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14))+16)>>5] + 1 )>>1;
 dst++;
 src++;
 }
}

#if 0
#define TPEL_WIDTH(width)\
static void put_tpel_pixels ## width ## _mc00_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc00_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc10_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc10_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc20_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc20_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc01_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc01_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc11_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc11_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc21_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc21_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc02_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc02_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc12_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc12_c(dst, src, stride, width, height);}\
static void put_tpel_pixels ## width ## _mc22_c(uint8_t *dst, const uint8_t *src, int stride, int height){\
    void put_tpel_pixels_mc22_c(dst, src, stride, width, height);}
#endif


#define QPEL_MC(r, OPNAME, RND, OP) \
static void OPNAME ## mpeg4_qpel8_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));\
        OP(dst[1], (src[1]+src[2])*20 - (src[0]+src[3])*6 + (src[0]+src[4])*3 - (src[1]+src[5]));\
        OP(dst[2], (src[2]+src[3])*20 - (src[1]+src[4])*6 + (src[0]+src[5])*3 - (src[0]+src[6]));\
        OP(dst[3], (src[3]+src[4])*20 - (src[2]+src[5])*6 + (src[1]+src[6])*3 - (src[0]+src[7]));\
        OP(dst[4], (src[4]+src[5])*20 - (src[3]+src[6])*6 + (src[2]+src[7])*3 - (src[1]+src[8]));\
        OP(dst[5], (src[5]+src[6])*20 - (src[4]+src[7])*6 + (src[3]+src[8])*3 - (src[2]+src[8]));\
        OP(dst[6], (src[6]+src[7])*20 - (src[5]+src[8])*6 + (src[4]+src[8])*3 - (src[3]+src[7]));\
        OP(dst[7], (src[7]+src[8])*20 - (src[6]+src[8])*6 + (src[5]+src[7])*3 - (src[4]+src[6]));\
        dst+=dstStride;\
        src+=srcStride;\
    }\
}\
\
static void OPNAME ## mpeg4_qpel8_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int w=8;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<w; i++)\
    {\
        const int src0= src[0*srcStride];\
        const int src1= src[1*srcStride];\
        const int src2= src[2*srcStride];\
        const int src3= src[3*srcStride];\
        const int src4= src[4*srcStride];\
        const int src5= src[5*srcStride];\
        const int src6= src[6*srcStride];\
        const int src7= src[7*srcStride];\
        const int src8= src[8*srcStride];\
        OP(dst[0*dstStride], (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4));\
        OP(dst[1*dstStride], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5));\
        OP(dst[2*dstStride], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6));\
        OP(dst[3*dstStride], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7));\
        OP(dst[4*dstStride], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8));\
        OP(dst[5*dstStride], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));\
        OP(dst[6*dstStride], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));\
        OP(dst[7*dstStride], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));\
        dst++;\
        src++;\
    }\
}\
\
static void OPNAME ## qpel8_mc00_c (uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## pixels8_c(dst, src, stride, 8);\
}\
\
static void OPNAME ## qpel8_mc10_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[64];\
    put ## RND ## mpeg4_qpel8_h_lowpass(half, src, 8, stride, 8);\
    OPNAME ## pixels8_l2(dst, src, half, stride, stride, 8, 8);\
}\
\
static void OPNAME ## qpel8_mc20_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## mpeg4_qpel8_h_lowpass(dst, src, stride, stride, 8);\
}\
\
static void OPNAME ## qpel8_mc30_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[64];\
    put ## RND ## mpeg4_qpel8_h_lowpass(half, src, 8, stride, 8);\
    OPNAME ## pixels8_l2(dst, src+1, half, stride, stride, 8, 8);\
}\
\
static void OPNAME ## qpel8_mc01_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t half[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(half, full, 8, 16);\
    OPNAME ## pixels8_l2(dst, full, half, stride, 16, 8, 8);\
}\
\
static void OPNAME ## qpel8_mc02_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    copy_block9(full, src, 16, stride, 9);\
    OPNAME ## mpeg4_qpel8_v_lowpass(dst, full, stride, 16);\
}\
\
static void OPNAME ## qpel8_mc03_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t half[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(half, full, 8, 16);\
    OPNAME ## pixels8_l2(dst, full+16, half, stride, 16, 8, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc11_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l4(dst, full, halfH, halfV, halfHV, stride, 16, 8, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc11_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full, 8, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH, halfHV, stride, 8, 8, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc31_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full+1, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l4(dst, full+1, halfH, halfV, halfHV, stride, 16, 8, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc31_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full+1, 8, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH, halfHV, stride, 8, 8, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc13_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l4(dst, full+16, halfH+8, halfV, halfHV, stride, 16, 8, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc13_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full, 8, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH+8, halfHV, stride, 8, 8, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc33_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full  , 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full+1, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l4(dst, full+17, halfH+8, halfV, halfHV, stride, 16, 8, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc33_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full+1, 8, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH+8, halfHV, stride, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc21_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, src, 8, stride, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH, halfHV, stride, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc23_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[72];\
    uint8_t halfHV[64];\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, src, 8, stride, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfH+8, halfHV, stride, 8, 8, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc12_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfV, halfHV, stride, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc12_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full, 8, 8, 16, 9);\
    OPNAME ## mpeg4_qpel8_v_lowpass(dst, halfH, stride, 8);\
}\
void ff_ ## OPNAME ## qpel8_mc32_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    uint8_t halfV[64];\
    uint8_t halfHV[64];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfV, full+1, 8, 16);\
    put ## RND ## mpeg4_qpel8_v_lowpass(halfHV, halfH, 8, 8);\
    OPNAME ## pixels8_l2(dst, halfV, halfHV, stride, 8, 8, 8);\
}\
static void OPNAME ## qpel8_mc32_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[16*9];\
    uint8_t halfH[72];\
    copy_block9(full, src, 16, stride, 9);\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, full, 8, 16, 9);\
    put ## RND ## pixels8_l2(halfH, halfH, full+1, 8, 8, 16, 9);\
    OPNAME ## mpeg4_qpel8_v_lowpass(dst, halfH, stride, 8);\
}\
static void OPNAME ## qpel8_mc22_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[72];\
    put ## RND ## mpeg4_qpel8_h_lowpass(halfH, src, 8, stride, 9);\
    OPNAME ## mpeg4_qpel8_v_lowpass(dst, halfH, stride, 8);\
}\
static void OPNAME ## qpel16_mc00_c (uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## pixels16_c(dst, src, stride, 16);\
}\
\
static void OPNAME ## qpel16_mc10_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[256];\
    put ## RND ## mpeg4_qpel16_h_lowpass(half, src, 16, stride, 16);\
    OPNAME ## pixels16_l2(dst, src, half, stride, stride, 16, 16);\
}\
\
static void OPNAME ## qpel16_mc20_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## mpeg4_qpel16_h_lowpass(dst, src, stride, stride, 16);\
}\
\
static void OPNAME ## qpel16_mc30_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[256];\
    put ## RND ## mpeg4_qpel16_h_lowpass(half, src, 16, stride, 16);\
    OPNAME ## pixels16_l2(dst, src+1, half, stride, stride, 16, 16);\
}\
\
static void OPNAME ## qpel16_mc01_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t half[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(half, full, 16, 24);\
    OPNAME ## pixels16_l2(dst, full, half, stride, 24, 16, 16);\
}\
\
static void OPNAME ## qpel16_mc02_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    copy_block17(full, src, 24, stride, 17);\
    OPNAME ## mpeg4_qpel16_v_lowpass(dst, full, stride, 24);\
}\
\
static void OPNAME ## qpel16_mc03_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t half[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(half, full, 16, 24);\
    OPNAME ## pixels16_l2(dst, full+24, half, stride, 24, 16, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc11_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l4(dst, full, halfH, halfV, halfHV, stride, 24, 16, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc11_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full, 16, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH, halfHV, stride, 16, 16, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc31_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full+1, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l4(dst, full+1, halfH, halfV, halfHV, stride, 24, 16, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc31_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full+1, 16, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH, halfHV, stride, 16, 16, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc13_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l4(dst, full+24, halfH+16, halfV, halfHV, stride, 24, 16, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc13_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full, 16, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH+16, halfHV, stride, 16, 16, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc33_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full  , 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full+1, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l4(dst, full+25, halfH+16, halfV, halfHV, stride, 24, 16, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc33_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full+1, 16, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH+16, halfHV, stride, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc21_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, src, 16, stride, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH, halfHV, stride, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc23_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[272];\
    uint8_t halfHV[256];\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, src, 16, stride, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfH+16, halfHV, stride, 16, 16, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc12_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfV, halfHV, stride, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc12_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full, 16, 16, 24, 17);\
    OPNAME ## mpeg4_qpel16_v_lowpass(dst, halfH, stride, 16);\
}\
void ff_ ## OPNAME ## qpel16_mc32_old_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    uint8_t halfV[256];\
    uint8_t halfHV[256];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfV, full+1, 16, 24);\
    put ## RND ## mpeg4_qpel16_v_lowpass(halfHV, halfH, 16, 16);\
    OPNAME ## pixels16_l2(dst, halfV, halfHV, stride, 16, 16, 16);\
}\
static void OPNAME ## qpel16_mc32_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[24*17];\
    uint8_t halfH[272];\
    copy_block17(full, src, 24, stride, 17);\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, full, 16, 24, 17);\
    put ## RND ## pixels16_l2(halfH, halfH, full+1, 16, 16, 24, 17);\
    OPNAME ## mpeg4_qpel16_v_lowpass(dst, halfH, stride, 16);\
}\
static void OPNAME ## qpel16_mc22_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t halfH[272];\
    put ## RND ## mpeg4_qpel16_h_lowpass(halfH, src, 16, stride, 17);\
    OPNAME ## mpeg4_qpel16_v_lowpass(dst, halfH, stride, 16);\
}

#define op_avg(a, b) a = (((a)+cm[((b) + 16)>>5]+1)>>1)
#define op_avg_no_rnd(a, b) a = (((a)+cm[((b) + 15)>>5])>>1)
#define op_put(a, b) a = cm[((b) + 16)>>5]
#define op_put_no_rnd(a, b) a = cm[((b) + 15)>>5]

QPEL_MC(0, put_       , _       , op_put)
QPEL_MC(1, put_no_rnd_, _no_rnd_, op_put_no_rnd)
QPEL_MC(0, avg_       , _       , op_avg)
//QPEL_MC(1, avg_no_rnd , _       , op_avg)
#undef op_avg
#undef op_avg_no_rnd
#undef op_put
#undef op_put_no_rnd

#ifdef JZ4740_MXU_OPT
static void wmv2_mspel8_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){

  uint32_t  j;
  uint32_t src_aln0,src_aln1,src_rs0,src_rs1,src_rs2,src_rs3;
  int mul_ct0,mul_ct1;
  int ct0;
  mul_ct0=0x09090909;   //9 9 9 9
  mul_ct1=0x01010101;   //1 1 1 1
  ct0 = 0x00080008;
  src_aln0 = (((uint32_t)src-1) & 0xFFFFFFFC);
  src_aln1 = (((uint32_t)src+1) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - 1) & 3);
  src_rs1 = src_rs0-1;
  src_rs2 = 4-(((int)src + 1) & 3);
  src_rs3 = src_rs2-1;
  S32I2M(xr15,mul_ct0); //xr15:9 9 9 9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr13,ct0);     //xr13:  8   8
  dst-=dstStride;
  for(j=0;j<h;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDD(xr5,src_aln1,0);
   S32LDD(xr6,src_aln1,4);
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[2] src[1] src[0] src[-1]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[3] src[2] src[1] src[0]
   S32ALN(xr7,xr6,xr5,src_rs2);  //xr7:src[4] src[3] src[2] src[1]
   S32ALN(xr8,xr6,xr5,src_rs3);  //xr8:src[5] src[4] src[3] src[2]
   Q8MUL(xr5,xr4,xr15,xr6);   //xr5:9*src[3] 9*src[2] xr6:9*src[1] 9*src[0]
   Q8MAC_SS(xr5,xr3,xr14,xr6);//xr5:9*src[3]-src[2] 9*src[2]-src[1] xr6:9*src[1]-src[0] 9*src[0]-src[-1]
   Q8MUL(xr9,xr7,xr15,xr10);  //xr9:9*src[4] 9*src[3] xr10:9*src[2] 9*src[1]
   Q8MAC_SS(xr9,xr8,xr14,xr10); //xr9:9*src[4]-src[5] 9*src[3]-src[4] xr10:9*src[2]-src[3] 9*src[1]-src[2]
   Q16ADD_AA_WW(xr0,xr5,xr9,xr5); //xr5:dst[3] 9*src[3]-src[2]+9*src[4]-src[5] dst[2] 9*src[2]-src[1]+9*src[3]-src[4]
   Q16ADD_AA_WW(xr0,xr6,xr10,xr6); //xr6:dst[1] 9*src[1]-src[0]+9*src[2]-src[3] dst[0] 9*src[0]-src[-1]+9*src[1]-src[2]
   Q16ACC_SS(xr5,xr13,xr0,xr6); //
   Q16SAR(xr5,xr5,xr6,xr6,4);
   Q16SAT(xr5,xr5,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[6] src[5] src[4] src[3]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[7] src[6] src[5] src[4]

   S32LDI(xr1,src_aln1,4);
   S32LDD(xr2,src_aln1,4);
   S32ALN(xr7,xr2,xr1,src_rs2);  //xr7:src[8] src[7] src[6] src[5]
   S32ALN(xr8,xr2,xr1,src_rs3);  //xr8:src[9] src[8] src[7] src[6]
   Q8MUL(xr1,xr4,xr15,xr2);   //xr5:9*src[3] 9*src[2] xr6:9*src[1] 9*src[0]
   Q8MAC_SS(xr1,xr3,xr14,xr2);//xr5:9*src[3]-src[2] 9*src[2]-src[1] xr6:9*src[1]-src[0] 9*src[0]-src[-1]

   Q8MUL(xr9,xr7,xr15,xr10);  //xr9:9*src[4] 9*src[3] xr10:9*src[2] 9*src[1]
   Q8MAC_SS(xr9,xr8,xr14,xr10); //xr9:9*src[4]-src[5] 9*src[3]-src[4] xr10:9*src[2]-src[3] 9*src[1]-src[2]
   Q16ADD_AA_WW(xr0,xr1,xr9,xr1); //xr5:dst[3] 9*src[3]-src[2]+9*src[4]-src[5] dst[2] 9*src[2]-src[1]+9*src[3]
                                        //-src[4]

   Q16ADD_AA_WW(xr0,xr2,xr10,xr2);//xr6:dst[1] 9*src[1]-src[0]+9*src[2]-src[3]dst[0] 9*src[0]-src[-1]+9*src[1]-src[2]
   Q16ACC_SS(xr1,xr13,xr0,xr2);
   Q16SAR(xr1,xr1,xr2,xr2,4);
   Q16SAT(xr1,xr1,xr2);      
   S32SDIV(xr5,dst,dstStride,0);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32STD(xr1,dst,4);
   src_aln0+=(srcStride-4);
   src_aln1+=(srcStride-4);
   }
}
#else
static void wmv2_mspel8_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h){
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    int i;

    for(i=0; i<h; i++){
        dst[0]= cm[(9*(src[0] + src[1]) - (src[-1] + src[2]) + 8)>>4];
        dst[1]= cm[(9*(src[1] + src[2]) - (src[ 0] + src[3]) + 8)>>4];
        dst[2]= cm[(9*(src[2] + src[3]) - (src[ 1] + src[4]) + 8)>>4];
        dst[3]= cm[(9*(src[3] + src[4]) - (src[ 2] + src[5]) + 8)>>4];
        dst[4]= cm[(9*(src[4] + src[5]) - (src[ 3] + src[6]) + 8)>>4];
        dst[5]= cm[(9*(src[5] + src[6]) - (src[ 4] + src[7]) + 8)>>4];
        dst[6]= cm[(9*(src[6] + src[7]) - (src[ 5] + src[8]) + 8)>>4];
        dst[7]= cm[(9*(src[7] + src[8]) - (src[ 6] + src[9]) + 8)>>4];
    
        dst+=dstStride;
        src+=srcStride;
    }
}

#endif

#ifdef CONFIG_CAVS_DECODER

#ifndef __LINUX__
void (*ff_cavsdsp_init_funcp)(DSPContext* c, AVCodecContext *avctx) = NULL;
#else
/* AVS specific */
void ff_cavsdsp_init(DSPContext* c, AVCodecContext *avctx);
#endif

void ff_put_cavs_qpel8_mc00_c(uint8_t *dst, uint8_t *src, int stride) {
     put_pixels8_c(dst, src, stride, 8);
}
void ff_avg_cavs_qpel8_mc00_c(uint8_t *dst, uint8_t *src, int stride) {
    avg_pixels8_c(dst, src, stride, 8);
}
void ff_put_cavs_qpel16_mc00_c(uint8_t *dst, uint8_t *src, int stride) {
    put_pixels16_c(dst, src, stride, 16);
}
void ff_avg_cavs_qpel16_mc00_c(uint8_t *dst, uint8_t *src, int stride) {
    avg_pixels16_c(dst, src, stride, 16);
}
#endif /* CONFIG_CAVS_DECODER */

#if defined(CONFIG_VC1_DECODER) || defined(CONFIG_WMV3_DECODER)
#ifndef __LINUX__
 void (*ff_vc1dsp_init_funcp)(DSPContext* c, AVCodecContext *avctx) = NULL;
#else
/* VC-1 specific */
void ff_vc1dsp_init(DSPContext* c, AVCodecContext *avctx);
#endif

void ff_put_vc1_mspel_mc00_c(uint8_t *dst, uint8_t *src, int stride, int rnd) {
    put_pixels8_c(dst, src, stride, 8);
}
#endif /* CONFIG_VC1_DECODER||CONFIG_WMV3_DECODER */

#if defined(CONFIG_H264_ENCODER)
/* H264 specific */
void ff_h264dspenc_init(DSPContext* c, AVCodecContext *avctx);
#endif /* CONFIG_H264_ENCODER */
#ifdef JZ4740_MXU_OPT
 static void wmv2_mspel8_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int w){
  int i,j;
  uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
  uint32_t src_aln0,src_rs0,src_aln,dst_aln,dst_aln1;
  int mul_ct0,mul_ct1,mul_ct2;
  mul_ct0=0x09090909;   //9 9 9 9
  mul_ct1=0x01010101;   //1 1 1 1
  mul_ct2=0x00080008;   //8   8
  src_aln = (((uint32_t)src - srcStride) & 0xFFFFFFFC);
  src_aln0  = src_aln;
  src_rs0 = 4-(((uint32_t)src - srcStride) & 3);
  dst_aln = (uint32_t)dst;
  dst_aln1 = dst_aln;
  S32I2M(xr15,mul_ct0); //xr15:9 9 9 9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr13,mul_ct2); //xr13:  8   8
 for(i=0; i<2; i++){
   dst_aln  -= dstStride;
   for(j=0; j<8; j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,srcStride,0);
   S32LDD(xr6,src_aln0,4);
   S32ALN(xr3,xr2,xr1,src_rs0); //xr3:src[-s+3] src[-s+2] src[-s+1] src[-s]
   S32ALN(xr7,xr6,xr5,src_rs0); //xr7:src[3]    src[2]    src[1]    src[0]

   S32LDIV(xr1,src_aln0,srcStride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,srcStride,0);
   S32LDD(xr6,src_aln0,4);
   S32ALN(xr4,xr2,xr1,src_rs0); //xr4:src[s+3]   src[s+2]   src[s+1]   src[s]
   S32ALN(xr8,xr6,xr5,src_rs0); //xr8:src[2*s+3] src[2*s+2] src[2*s+1] src[2*s]

   Q8MUL(xr1,xr7,xr15,xr2); //xr1:9*src[3]   9*src[2]  xr2:9*src[1]   9*src[0] 
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:9*src[3]-src[-s+3]  9*src[2]-src[-s+2]
                               //xr2:9*src[1]-src[-s+1]  9*src[0]-src[-s  ]
   Q8MUL(xr5,xr4,xr15,xr6);
   Q8MAC_SS(xr5,xr8,xr14,xr6); //xr5:9*src[s+3]-src[2*s+3] 9*src[s+2]-src[2*s+2]
                               //xr6:9*src[s+1]-src[2*s+1] 9*src[s  ]-src[2*s  ]
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); //xr5:9*src[s+3]-src[2*s+3] + 9*src[3]-src[-s+3]
                                  //    9*src[s+2]-src[2*s+2] + 9*src[2]-src[-s+2]
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); //xr6:9*src[s+1]-src[2*s+1] + 9*src[1]-src[-s+1]
                                  //    9*src[s  ]-src[2*s  ] + 9*src[0]-src[-s  ]
   Q16ACC_AA(xr5,xr13,xr0,xr6);
   Q16SAR(xr5,xr5,xr6,xr6,4);  //xr5:d[3] d[2] xr6:d[1] d[0]
   Q16SAT(xr5,xr5,xr6);        //xr5:
   S32SDIV(xr5,dst_aln,dstStride,0x0);
   src_aln0 -= 2 * srcStride;
   }
   src_aln0 = src_aln + 4;
   src_aln += 4;
   dst_aln =  dst_aln1 + 4;
   dst_aln1 += 4;
   }
}

#else
static void wmv2_mspel8_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int w){
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    int i;

    for(i=0; i<w; i++){
        const int src_1= src[ -srcStride];
        const int src0 = src[0          ];
        const int src1 = src[  srcStride];
        const int src2 = src[2*srcStride];
        const int src3 = src[3*srcStride];
        const int src4 = src[4*srcStride];
        const int src5 = src[5*srcStride];
        const int src6 = src[6*srcStride];
        const int src7 = src[7*srcStride];
        const int src8 = src[8*srcStride];
        const int src9 = src[9*srcStride];
        dst[0*dstStride]= cm[(9*(src0 + src1) - (src_1 + src2) + 8)>>4];
        dst[1*dstStride]= cm[(9*(src1 + src2) - (src0  + src3) + 8)>>4];
        dst[2*dstStride]= cm[(9*(src2 + src3) - (src1  + src4) + 8)>>4];
        dst[3*dstStride]= cm[(9*(src3 + src4) - (src2  + src5) + 8)>>4];
        dst[4*dstStride]= cm[(9*(src4 + src5) - (src3  + src6) + 8)>>4];
        dst[5*dstStride]= cm[(9*(src5 + src6) - (src4  + src7) + 8)>>4];
        dst[6*dstStride]= cm[(9*(src6 + src7) - (src5  + src8) + 8)>>4];
        dst[7*dstStride]= cm[(9*(src7 + src8) - (src6  + src9) + 8)>>4];
        src++;
        dst++;
    }
}
#endif
static void put_mspel8_mc00_c (uint8_t *dst, uint8_t *src, int stride){
   put_pixels8_c(dst, src, stride, 8);
}

static void put_mspel8_mc10_c(uint8_t *dst, uint8_t *src, int stride){
    uint8_t half[64];
    wmv2_mspel8_h_lowpass(half, src, 8, stride, 8);
    put_pixels8_l2(dst, src, half, stride, stride, 8, 8);
}

static void put_mspel8_mc20_c(uint8_t *dst, uint8_t *src, int stride){
    wmv2_mspel8_h_lowpass(dst, src, stride, stride, 8);
}

static void put_mspel8_mc30_c(uint8_t *dst, uint8_t *src, int stride){
    uint8_t half[64];
    wmv2_mspel8_h_lowpass(half, src, 8, stride, 8);
    put_pixels8_l2(dst, src+1, half, stride, stride, 8, 8);
}

static void put_mspel8_mc02_c(uint8_t *dst, uint8_t *src, int stride){
    wmv2_mspel8_v_lowpass(dst, src, stride, stride, 8);
}

static void put_mspel8_mc12_c(uint8_t *dst, uint8_t *src, int stride){
    uint8_t halfH[88];
    uint8_t halfV[64];
    uint8_t halfHV[64];
    wmv2_mspel8_h_lowpass(halfH, src-stride, 8, stride, 11);
    wmv2_mspel8_v_lowpass(halfV, src, 8, stride, 8);
    wmv2_mspel8_v_lowpass(halfHV, halfH+8, 8, 8, 8);
    put_pixels8_l2(dst, halfV, halfHV, stride, 8, 8, 8);
}
static void put_mspel8_mc32_c(uint8_t *dst, uint8_t *src, int stride){
    uint8_t halfH[88];
    uint8_t halfV[64];
    uint8_t halfHV[64];
    wmv2_mspel8_h_lowpass(halfH, src-stride, 8, stride, 11);
    wmv2_mspel8_v_lowpass(halfV, src+1, 8, stride, 8);
    wmv2_mspel8_v_lowpass(halfHV, halfH+8, 8, 8, 8);
    put_pixels8_l2(dst, halfV, halfHV, stride, 8, 8, 8);
}
static void put_mspel8_mc22_c(uint8_t *dst, uint8_t *src, int stride){
    uint8_t halfH[88];
    wmv2_mspel8_h_lowpass(halfH, src-stride, 8, stride, 11);
    wmv2_mspel8_v_lowpass(dst, halfH+8, stride, 8, 8);
}
#ifdef JZ4740_MXU_OPT
 static void h263_v_loop_filter_c(uint8_t *src, int stride, int qscale){
  
  if(ENABLE_ANY_H263) {
    int x;
    const int strength= ff_h263_loop_filter_strength[qscale];
    uint32_t s1 = (2*strength) * 0x00010001;
    uint32_t s2 = (strength) * 0x00010001;
    uint8_t *src_data;
    for(x =0;x<2;x++) {
    src_data = src - 2 * stride;
    S32LDD (xr1,src_data,0);         //xr1:p0 (src[-2*s + 3] - src[-2*s])
    S32LDIV(xr2,src_data,stride,0); //xr2:p1  (src[-s + 3] - src[-s])
    S32LDIV(xr3,src_data,stride,0); //xr3:p2  (src[3] - src[0])
    S32LDIV(xr4,src_data,stride,0); //xr4:p3  (src[s + 3] - src[s])

    Q8ADDE_SS(xr5,xr3,xr2,xr6);     // p2-p1
    Q16SLL(xr5,xr5,xr6,xr6,2);      //(p2-p1)*4
    Q8ACCE_SS(xr5,xr1,xr4,xr6);     //(p0-p3)+(p2-p1)*4
    Q16SAR(xr5,xr5,xr6,xr6,3);      //  xr5 xr6  d
   
    S32I2M(xr15,s1);
    S32I2M(xr14,s2);
    //////condition begin///////////
    Q16ADD_AS_WW(xr7,xr5,xr15,xr8);     //xr7: d+2s   xr8:  d-2s
    Q16ADD_AS_WW(xr9,xr6,xr15,xr10);    //xr9: d+2s   xr10: d-2s
    Q16SLR (xr11, xr7, xr9, xr13, 15);    //xr11 xr13: d+2s  sign
    Q16SLR (xr12, xr8, xr10,xr15,15);     //xr12 xr15: d-2s  sign

    D16MADL_AA_WW(xr0,xr7,xr11,xr7);    //xr7:(d+2s)*(d+2s)sign
    D16MADL_AA_WW(xr0,xr9,xr13,xr9);    //xr9:(d+2s)*(d+2s)sign
    D16MADL_SS_WW(xr7,xr8,xr12,xr7);    //xr7:(d-2s)*(d-2s)sign
    D16MADL_SS_WW(xr9,xr10,xr15,xr9);   //xr9:(d-2s)*(d-2s)sign

    Q16ADD_AS_WW(xr11,xr5,xr14,xr12);   //xr11:d+s    xr12:d-s
    Q16ADD_AS_WW(xr13,xr6,xr14,xr15);   //xr13:d+s    xr15:d-s

    Q16SLL (xr8, xr11,xr13,xr10,1);     //xr8: xr10: 2*(d+s)
    Q16SLR (xr11,xr11,xr13,xr13,15);    //xr11 xr13: d+s sign
    D16MADL_SS_WW(xr7,xr8,xr11,xr7);    //xr7: (d+s)sign * 2*(d+s)
    D16MADL_SS_WW(xr9,xr10,xr13,xr9);   //xr9: (d+s)sign * 2*(d+s)

    Q16SLL (xr8,xr12,xr15,xr10,1);
    Q16SLR (xr12,xr12,xr15,xr15,15);
    D16MADL_AA_WW(xr7,xr8,xr12,xr7);    //xr7:d1
    D16MADL_AA_WW(xr9,xr10,xr15,xr8);   //xr8:d1
    //////////condition end///////////////
  
    Q8ADDE_AS(xr5,xr2,xr0,xr6);
    Q16ADD_AA_WW(xr5,xr5,xr7,xr0);     //new p1
    Q16ADD_AA_WW(xr6,xr6,xr8,xr0);     //new p1
    Q8ADDE_AS(xr9,xr3,xr0,xr10);
    Q16ADD_SS_WW(xr9,xr9,xr7,xr0);     //new p2
    Q16ADD_SS_WW(xr10,xr10,xr8,xr0);   //new p2

    D16MAX(xr5,xr5,xr0);      //p1
    D16MAX(xr6,xr6,xr0);      //p1
    D16MAX(xr9,xr9,xr0);      //p2
    D16MAX(xr10,xr10,xr0);    //p2

    Q16SAT(xr5,xr5,xr6);      //p1
    Q16SAT(xr9,xr9,xr10);     //p2
    D16CPS    (xr7,xr7,xr7);
    D16CPS    (xr8,xr8,xr8);
    Q16SAR    (xr7,xr7,xr8,xr8,1);    //xr7 xr8: ad1
    Q8ADDE_SS (xr12,xr1,xr4,xr13);    //(p0-p3)
    Q16SAR(xr12,xr12,xr13,xr13,2);

    Q16ADD_SS_WW(xr11,xr0,xr7,xr0);   //xr11:-ad1
    Q16ADD_SS_WW(xr10,xr0,xr8,xr0);   //xr10:-ad1

    D16MAX(xr12,xr11,xr12);
    D16MAX(xr13,xr10,xr13);
    D16MIN(xr12,xr7,xr12);            //xr12:d2
    D16MIN(xr13,xr8,xr13);            //xr13:d2
    Q8ADDE_AA(xr11,xr1,xr0,xr10);     //xr11 xr10:p0
    Q8ADDE_AA(xr7, xr4,xr0,xr8 );     //xr7  xr8 :p3
    Q16ADD_SS_WW(xr11,xr11,xr12,xr0); //xr11  p0 - d2
    Q16ADD_SS_WW(xr10,xr10,xr13,xr0); //xr10  p0 - d2
    Q16SAT(xr10,xr11,xr10);           //xr10:p0 - d2
    Q16ADD_AA_WW(xr7,xr7,xr12,xr0);
    Q16ADD_AA_WW(xr8,xr8,xr13,xr0);
    Q16SAT(xr8,xr7,xr8);              //xr8: p3 + d2
 
    S32STD(xr8,src_data,0);
    S32SDIV(xr9,src_data,-stride,0);
    S32SDIV(xr5,src_data,-stride,0);
    S32SDIV(xr10,src_data,-stride,0);

    src = src + 4;
   }
 }
}
#else
static void h263_v_loop_filter_c(uint8_t *src, int stride, int qscale){
    if(ENABLE_ANY_H263) {
    int x;
    const int strength= ff_h263_loop_filter_strength[qscale];

    for(x=0; x<8; x++){
        int d1, d2, ad1;
        int p0= src[x-2*stride];
        int p1= src[x-1*stride];
        int p2= src[x+0*stride];
        int p3= src[x+1*stride];
        int d = (p0 - p3 + 4*(p2 - p1)) / 8;

        if     (d<-2*strength) d1= 0;
        else if(d<-  strength) d1=-2*strength - d;
        else if(d<   strength) d1= d;
        else if(d< 2*strength) d1= 2*strength - d;
        else                   d1= 0;

        p1 += d1;
        p2 -= d1;
        if(p1&256) p1= ~(p1>>31);
        if(p2&256) p2= ~(p2>>31);

        src[x-1*stride] = p1;
        src[x+0*stride] = p2;

        ad1= FFABS(d1)>>1;

        d2= av_clip((p0-p3)/4, -ad1, ad1);

        src[x-2*stride] = p0 - d2;
        src[x+  stride] = p3 + d2;
    }
    }
}
#endif
#ifdef JZ4740_MXU_OPT
static void h263_h_loop_filter_c(uint8_t *src, int stride, int qscale){
  if(ENABLE_ANY_H263) {
    int y,t1,t2,t3,t4;
    uint8_t *src_data,*src_q;
    const int strength= ff_h263_loop_filter_strength[qscale];
    uint32_t s1 = (2*strength) * 0x00010001;
    uint32_t s2 = strength * 0x00010001;
    uint32_t src_aln0 = (((uint32_t)src - 2 - stride) & 0xFFFFFFFC);
    uint32_t src_rs0 = 4-(((uint32_t)src - 2) & 3);
    src_data = src;
    for(y=0; y<2; y++){
        S32LDIV(xr1,src_aln0,stride,0);
        S32LDD(xr2,src_aln0,4);
        S32LDIV(xr3,src_aln0,stride,0);
        S32LDD (xr4,src_aln0,4);
        S32ALN(xr1,xr2,xr1,src_rs0);   //xr1:src[1] src[0] src[-1] src[-2]
        S32ALN(xr3,xr4,xr3,src_rs0);   //xr3:src[s+1] src[s] src[s-1] src[s-2]
        S32LDIV(xr2,src_aln0,stride,0);
        S32LDD (xr4,src_aln0,4);
        S32LDIV(xr5,src_aln0,stride,0);
        S32LDD (xr6,src_aln0,4);
        S32ALN(xr2,xr4,xr2,src_rs0);   //xr2:src[2s+1] src[2s] src[2s-1] src[2s-2]
        S32ALN(xr4,xr6,xr5,src_rs0);   //xr4:src[3s+1] src[3s] src[3s-1] src[3s-2]

        S32SFL(xr3,xr3,xr1,xr1,ptn2);  //xr3:src[s+1] src[1] src[s-1] src[-1]
                                       //xr1:src[s]   src[0] src[s-2] src[-2]
        S32SFL(xr4,xr4,xr2,xr2,ptn2);  //xr4:src[3s+1] src[2s+1] src[3s-1] src[2s-1]
                                       //xr2:src[3s]   src[2s]   src[3s-2] src[2s-2]
        S32SFL(xr4,xr4,xr3,xr3,ptn3);  //xr4:p3  src[3s+1] src[2s+1] src[s+1] src[1]
                                       //xr3:p1  src[3s-1]   src[2s-1] src[s-1] src[-1]
        S32SFL(xr2,xr2,xr1,xr1,ptn3);  //xr2:p2  src[3s]     src[2s]     src[s]   src[0]
                                       //xr1:p0  src[3s-2]   src[2s-2]   src[s-2] src[-2]

        Q8ADDE_SS(xr5,xr2,xr3,xr6); //xr5 xr6:p2-p1
        Q16SLL(xr5,xr5,xr6,xr6,2);  //xr5 xr6:(p2-p1)*4
        Q8ACCE_SS(xr5,xr1,xr4,xr6); //xr5 xr6:(p0-p3)+(p2-p1)*4
        Q16SAR(xr5,xr5,xr6,xr6,3);  //xr5 xr6: d
        S32I2M(xr15,s1);
        S32I2M(xr14,s2);
/////////////////////////
       Q16ADD_AS_WW(xr7,xr5,xr15,xr8);     //xr7: d+2s   xr8:  d-2s
       Q16ADD_AS_WW(xr9,xr6,xr15,xr10);    //xr9: d+2s   xr10: d-2s
       Q16SLR (xr11, xr7, xr9, xr13, 15);    //xr11 xr13: d+2s  sign
       Q16SLR (xr12, xr8, xr10,xr15,15);     //xr12 xr15: d-2s  sign

       D16MADL_AA_WW(xr0,xr7,xr11,xr7);    //xr7:(d+2s)*(d+2s)sign
       D16MADL_AA_WW(xr0,xr9,xr13,xr9);    //xr9:(d+2s)*(d+2s)sign
       D16MADL_SS_WW(xr7,xr8,xr12,xr7);    //xr7:(d-2s)*(d-2s)sign
       D16MADL_SS_WW(xr9,xr10,xr15,xr9);   //xr9:(d-2s)*(d-2s)sign


       Q16ADD_AS_WW(xr11,xr5,xr14,xr12);   //xr11:d+s    xr12:d-s
       Q16ADD_AS_WW(xr13,xr6,xr14,xr15);   //xr13:d+s    xr15:d-s

       Q16SLL (xr8, xr11,xr13,xr10,1);     //xr8: xr10: 2*(d+s)
       Q16SLR (xr11,xr11,xr13,xr13,15);    //xr11 xr13: d+s sign
       D16MADL_SS_WW(xr7,xr8,xr11,xr7);    //xr7: (d+s)sign * 2*(d+s)
       D16MADL_SS_WW(xr9,xr10,xr13,xr9);   //xr9: (d+s)sign * 2*(d+s)

       Q16SLL (xr8,xr12,xr15,xr10,1);
       Q16SLR (xr12,xr12,xr15,xr15,15);
       D16MADL_AA_WW(xr7,xr8,xr12,xr7);    //xr7:d1
       D16MADL_AA_WW(xr9,xr10,xr15,xr8);   //xr8:d1
/////////////////////////////////
       Q8ADDE_AS(xr5,xr3,xr0,xr6);
        Q16ADD_AA_WW(xr5,xr5,xr7,xr0);     //new p1
        Q16ADD_AA_WW(xr6,xr6,xr8,xr0);     //new p1
        Q8ADDE_AS(xr9,xr2,xr0,xr10);
        Q16ADD_SS_WW(xr9,xr9,xr7,xr0);     //new p2
        Q16ADD_SS_WW(xr10,xr10,xr8,xr0);   //new p2

        D16MAX(xr5,xr5,xr0);      //p1
        D16MAX(xr6,xr6,xr0);      //p1
        D16MAX(xr9,xr9,xr0);      //p2
        D16MAX(xr10,xr10,xr0);    //p2

        Q16SAT(xr5,xr5,xr6);      //p1
        Q16SAT(xr9,xr9,xr10);     //p2

        D16CPS    (xr7,xr7,xr7);
        D16CPS    (xr8,xr8,xr8);
        Q16SAR    (xr7,xr7,xr8,xr8,1);  //xr7 xr8: ad1
        Q8ADDE_SS (xr12,xr1,xr4,xr13);  //(p0-p3)
        Q16SAR(xr12,xr12,xr13,xr13,2);

        Q16ADD_SS_WW(xr11,xr0,xr7,xr0);  //xr11:-ad1
        Q16ADD_SS_WW(xr10,xr0,xr8,xr0);  //xr10:-ad1

        D16MAX(xr12,xr11,xr12);
        D16MAX(xr13,xr10,xr13);
        D16MIN(xr12,xr7,xr12);          //xr12:d2
        D16MIN(xr13,xr8,xr13);          //xr13:d2
        Q8ADDE_AA(xr11,xr1,xr0,xr10);   //xr11 xr10:p0
        Q8ADDE_AA(xr7, xr4,xr0,xr8 );   //xr7  xr8 :p3
        Q16ADD_SS_WW(xr11,xr11,xr12,xr0); //xr11  p0 - d2
        Q16ADD_SS_WW(xr10,xr10,xr13,xr0); //xr10  p0 - d2
        Q16SAT(xr10,xr11,xr10);         //xr10:p0 - d2
        Q16ADD_AA_WW(xr7,xr7,xr12,xr0);
        Q16ADD_AA_WW(xr8,xr8,xr13,xr0);
        Q16SAT(xr8,xr7,xr8);            //xr8: p3 + d2

 /////////////////////////////////
        S32SFL(xr5,xr5,xr10,xr10,ptn0);
        S32SFL(xr8,xr8,xr9,xr9,ptn0);
        S32SFL(xr8,xr8,xr5,xr5,ptn3);
        S32SFL(xr9,xr9,xr10,xr10,ptn3);
        #ifdef JZ4750_OPT
        S16STD(xr10,src_data,-2,0);
        S16STD(xr10,src_data,0 ,1);         
        src_data += stride;
        S16STD(xr9,src_data,-2,0);
        S16STD(xr9,src_data,0 ,1);
        src_data += stride;
        S16STD(xr5,src_data,-2,0);
        S16STD(xr5,src_data,0 ,1);
        src_data += stride;
        S16STD(xr8,src_data,-2,0);
        S16STD(xr8,src_data,0 ,1);
        src_data += stride;
        #else
        t1 = S32M2I(xr8);
        t2 = S32M2I(xr5);
        t3 = S32M2I(xr9);
        t4 = S32M2I(xr10);
        *((unsigned short *)src_data - 1) = t4;
        *((unsigned short *)src_data + 0) = (t4 >> 16);
        src_data += stride;
        *((unsigned short *)src_data - 1) = t3;
        *((unsigned short *)src_data + 0) = (t3 >> 16);
        src_data += stride;
        *((unsigned short *)src_data - 1) = t2;
        *((unsigned short *)src_data + 0) = (t2 >> 16);
        src_data += stride;
        *((unsigned short *)src_data - 1) = t1;
        *((unsigned short *)src_data + 0) = (t1 >> 16);
        src_data += stride;
        #endif
     }
   }
}
#else

static void h263_h_loop_filter_c(uint8_t *src, int stride, int qscale){
    if(ENABLE_ANY_H263) {
    int y;
    const int strength= ff_h263_loop_filter_strength[qscale];

    for(y=0; y<8; y++){
        int d1, d2, ad1;
        int p0= src[y*stride-2];
        int p1= src[y*stride-1];
        int p2= src[y*stride+0];
        int p3= src[y*stride+1];
        int d = (p0 - p3 + 4*(p2 - p1)) / 8;

        if     (d<-2*strength) d1= 0;
        else if(d<-  strength) d1=-2*strength - d;
        else if(d<   strength) d1= d;
        else if(d< 2*strength) d1= 2*strength - d;
        else                   d1= 0;

        p1 += d1;
        p2 -= d1;
        if(p1&256) p1= ~(p1>>31);
        if(p2&256) p2= ~(p2>>31);

        src[y*stride-1] = p1;
        src[y*stride+0] = p2;

        ad1= FFABS(d1)>>1;

        d2= av_clip((p0-p3)/4, -ad1, ad1);

        src[y*stride-2] = p0 - d2;
        src[y*stride+1] = p3 + d2;
    }
    }
}
#endif
static void h261_loop_filter_c(uint8_t *src, int stride){
    int x,y,xy,yz;
    int temp[64];

    for(x=0; x<8; x++){
        temp[x      ] = 4*src[x           ];
        temp[x + 7*8] = 4*src[x + 7*stride];
    }
    for(y=1; y<7; y++){
        for(x=0; x<8; x++){
            xy = y * stride + x;
            yz = y * 8 + x;
            temp[yz] = src[xy - stride] + 2*src[xy] + src[xy + stride];
        }
    }

    for(y=0; y<8; y++){
        src[  y*stride] = (temp[  y*8] + 2)>>2;
        src[7+y*stride] = (temp[7+y*8] + 2)>>2;
        for(x=1; x<7; x++){
            xy = y * stride + x;
            yz = y * 8 + x;
            src[xy] = (temp[yz-1] + 2*temp[yz] + temp[yz+1] + 8)>>4;
        }
    }
}
//#undef printf
#ifdef JZ4740_MXU_OPT
 static inline int pix_abs16_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
  int s, i;
  s = 0;
  for(i=0;i<h;i++){
  S32LDD(xr1,pix1,0x0);  //xr1: pix1[3] pix1[2] pix1[1] pix1[0]
  S32LDD(xr2,pix1,0x4);  //xr2: pix1[7] pix1[6] pix1[5] pix1[4]
  S32LDD(xr3,pix1,0x8);  //xr3: pix1[11]pix1[10]pix1[9] pix1[8]
  S32LDD(xr4,pix1,0xc);  //xr4: pix1[15]pix1[14]pix1[13]pix1[12]

  S32LDD(xr5,pix2,0x0);  //xr5: pix2[3] pix2[2] pix2[1] pix2[0]
  S32LDD(xr6,pix2,0x4);  //xr6: pix2[7] pix2[6] pix2[5] pix2[4]
  S32LDD(xr7,pix2,0x8);  //xr7: pix2[11]pix2[10]pix2[9] pix2[8]
  S32LDD(xr8,pix2,0xc);  //xr8: pix2[15]pix2[14]pix2[13]pix2[12]

  Q8SAD(xr9,xr1,xr5,xr0);
  Q8SAD(xr0,xr2,xr6,xr9);
  Q8SAD(xr0,xr3,xr7,xr9);
  Q8SAD(xr0,xr4,xr8,xr9);

  s = S32M2I(xr9);
  pix1 += line_size;
  pix2 += line_size;
}
  return s;
}
#else
static inline int pix_abs16_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - pix2[0]);
        s += abs(pix1[1] - pix2[1]);
        s += abs(pix1[2] - pix2[2]);
        s += abs(pix1[3] - pix2[3]);
        s += abs(pix1[4] - pix2[4]);
        s += abs(pix1[5] - pix2[5]);
        s += abs(pix1[6] - pix2[6]);
        s += abs(pix1[7] - pix2[7]);
        s += abs(pix1[8] - pix2[8]);
        s += abs(pix1[9] - pix2[9]);
        s += abs(pix1[10] - pix2[10]);
        s += abs(pix1[11] - pix2[11]);
        s += abs(pix1[12] - pix2[12]);
        s += abs(pix1[13] - pix2[13]);
        s += abs(pix1[14] - pix2[14]);
        s += abs(pix1[15] - pix2[15]);
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}
#endif
static int pix_abs16_x2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg2(pix2[0], pix2[1]));
        s += abs(pix1[1] - avg2(pix2[1], pix2[2]));
        s += abs(pix1[2] - avg2(pix2[2], pix2[3]));
        s += abs(pix1[3] - avg2(pix2[3], pix2[4]));
        s += abs(pix1[4] - avg2(pix2[4], pix2[5]));
        s += abs(pix1[5] - avg2(pix2[5], pix2[6]));
        s += abs(pix1[6] - avg2(pix2[6], pix2[7]));
        s += abs(pix1[7] - avg2(pix2[7], pix2[8]));
        s += abs(pix1[8] - avg2(pix2[8], pix2[9]));
        s += abs(pix1[9] - avg2(pix2[9], pix2[10]));
        s += abs(pix1[10] - avg2(pix2[10], pix2[11]));
        s += abs(pix1[11] - avg2(pix2[11], pix2[12]));
        s += abs(pix1[12] - avg2(pix2[12], pix2[13]));
        s += abs(pix1[13] - avg2(pix2[13], pix2[14]));
        s += abs(pix1[14] - avg2(pix2[14], pix2[15]));
        s += abs(pix1[15] - avg2(pix2[15], pix2[16]));
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}

static int pix_abs16_y2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    uint8_t *pix3 = pix2 + line_size;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg2(pix2[0], pix3[0]));
        s += abs(pix1[1] - avg2(pix2[1], pix3[1]));
        s += abs(pix1[2] - avg2(pix2[2], pix3[2]));
        s += abs(pix1[3] - avg2(pix2[3], pix3[3]));
        s += abs(pix1[4] - avg2(pix2[4], pix3[4]));
        s += abs(pix1[5] - avg2(pix2[5], pix3[5]));
        s += abs(pix1[6] - avg2(pix2[6], pix3[6]));
        s += abs(pix1[7] - avg2(pix2[7], pix3[7]));
        s += abs(pix1[8] - avg2(pix2[8], pix3[8]));
        s += abs(pix1[9] - avg2(pix2[9], pix3[9]));
        s += abs(pix1[10] - avg2(pix2[10], pix3[10]));
        s += abs(pix1[11] - avg2(pix2[11], pix3[11]));
        s += abs(pix1[12] - avg2(pix2[12], pix3[12]));
        s += abs(pix1[13] - avg2(pix2[13], pix3[13]));
        s += abs(pix1[14] - avg2(pix2[14], pix3[14]));
        s += abs(pix1[15] - avg2(pix2[15], pix3[15]));
        pix1 += line_size;
        pix2 += line_size;
        pix3 += line_size;
    }
    return s;
}

static int pix_abs16_xy2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    uint8_t *pix3 = pix2 + line_size;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg4(pix2[0], pix2[1], pix3[0], pix3[1]));
        s += abs(pix1[1] - avg4(pix2[1], pix2[2], pix3[1], pix3[2]));
        s += abs(pix1[2] - avg4(pix2[2], pix2[3], pix3[2], pix3[3]));
        s += abs(pix1[3] - avg4(pix2[3], pix2[4], pix3[3], pix3[4]));
        s += abs(pix1[4] - avg4(pix2[4], pix2[5], pix3[4], pix3[5]));
        s += abs(pix1[5] - avg4(pix2[5], pix2[6], pix3[5], pix3[6]));
        s += abs(pix1[6] - avg4(pix2[6], pix2[7], pix3[6], pix3[7]));
        s += abs(pix1[7] - avg4(pix2[7], pix2[8], pix3[7], pix3[8]));
        s += abs(pix1[8] - avg4(pix2[8], pix2[9], pix3[8], pix3[9]));
        s += abs(pix1[9] - avg4(pix2[9], pix2[10], pix3[9], pix3[10]));
        s += abs(pix1[10] - avg4(pix2[10], pix2[11], pix3[10], pix3[11]));
        s += abs(pix1[11] - avg4(pix2[11], pix2[12], pix3[11], pix3[12]));
        s += abs(pix1[12] - avg4(pix2[12], pix2[13], pix3[12], pix3[13]));
        s += abs(pix1[13] - avg4(pix2[13], pix2[14], pix3[13], pix3[14]));
        s += abs(pix1[14] - avg4(pix2[14], pix2[15], pix3[14], pix3[15]));
        s += abs(pix1[15] - avg4(pix2[15], pix2[16], pix3[15], pix3[16]));
        pix1 += line_size;
        pix2 += line_size;
        pix3 += line_size;
    }
    return s;
}

static inline int pix_abs8_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - pix2[0]);
        s += abs(pix1[1] - pix2[1]);
        s += abs(pix1[2] - pix2[2]);
        s += abs(pix1[3] - pix2[3]);
        s += abs(pix1[4] - pix2[4]);
        s += abs(pix1[5] - pix2[5]);
        s += abs(pix1[6] - pix2[6]);
        s += abs(pix1[7] - pix2[7]);
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}

static int pix_abs8_x2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg2(pix2[0], pix2[1]));
        s += abs(pix1[1] - avg2(pix2[1], pix2[2]));
        s += abs(pix1[2] - avg2(pix2[2], pix2[3]));
        s += abs(pix1[3] - avg2(pix2[3], pix2[4]));
        s += abs(pix1[4] - avg2(pix2[4], pix2[5]));
        s += abs(pix1[5] - avg2(pix2[5], pix2[6]));
        s += abs(pix1[6] - avg2(pix2[6], pix2[7]));
        s += abs(pix1[7] - avg2(pix2[7], pix2[8]));
        pix1 += line_size;
        pix2 += line_size;
    }
    return s;
}

static int pix_abs8_y2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    uint8_t *pix3 = pix2 + line_size;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg2(pix2[0], pix3[0]));
        s += abs(pix1[1] - avg2(pix2[1], pix3[1]));
        s += abs(pix1[2] - avg2(pix2[2], pix3[2]));
        s += abs(pix1[3] - avg2(pix2[3], pix3[3]));
        s += abs(pix1[4] - avg2(pix2[4], pix3[4]));
        s += abs(pix1[5] - avg2(pix2[5], pix3[5]));
        s += abs(pix1[6] - avg2(pix2[6], pix3[6]));
        s += abs(pix1[7] - avg2(pix2[7], pix3[7]));
        pix1 += line_size;
        pix2 += line_size;
        pix3 += line_size;
    }
    return s;
}

static int pix_abs8_xy2_c(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)
{
    int s, i;
    uint8_t *pix3 = pix2 + line_size;

    s = 0;
    for(i=0;i<h;i++) {
        s += abs(pix1[0] - avg4(pix2[0], pix2[1], pix3[0], pix3[1]));
        s += abs(pix1[1] - avg4(pix2[1], pix2[2], pix3[1], pix3[2]));
        s += abs(pix1[2] - avg4(pix2[2], pix2[3], pix3[2], pix3[3]));
        s += abs(pix1[3] - avg4(pix2[3], pix2[4], pix3[3], pix3[4]));
        s += abs(pix1[4] - avg4(pix2[4], pix2[5], pix3[4], pix3[5]));
        s += abs(pix1[5] - avg4(pix2[5], pix2[6], pix3[5], pix3[6]));
        s += abs(pix1[6] - avg4(pix2[6], pix2[7], pix3[6], pix3[7]));
        s += abs(pix1[7] - avg4(pix2[7], pix2[8], pix3[7], pix3[8]));
        pix1 += line_size;
        pix2 += line_size;
        pix3 += line_size;
    }
    return s;
}

static int nsse16_c(void *v, uint8_t *s1, uint8_t *s2, int stride, int h){
    MpegEncContext *c = v;
    int score1=0;
    int score2=0;
    int x,y;

    for(y=0; y<h; y++){
        for(x=0; x<16; x++){
            score1+= (s1[x  ] - s2[x ])*(s1[x  ] - s2[x ]);
        }
        if(y+1<h){
            for(x=0; x<15; x++){
                score2+= FFABS(  s1[x  ] - s1[x  +stride]
                             - s1[x+1] + s1[x+1+stride])
                        -FFABS(  s2[x  ] - s2[x  +stride]
                             - s2[x+1] + s2[x+1+stride]);
            }
        }
        s1+= stride;
        s2+= stride;
    }

    if(c) return score1 + FFABS(score2)*c->avctx->nsse_weight;
    else  return score1 + FFABS(score2)*8;
}

static int nsse8_c(void *v, uint8_t *s1, uint8_t *s2, int stride, int h){
    MpegEncContext *c = v;
    int score1=0;
    int score2=0;
    int x,y;

    for(y=0; y<h; y++){
        for(x=0; x<8; x++){
            score1+= (s1[x  ] - s2[x ])*(s1[x  ] - s2[x ]);
        }
        if(y+1<h){
            for(x=0; x<7; x++){
                score2+= FFABS(  s1[x  ] - s1[x  +stride]
                             - s1[x+1] + s1[x+1+stride])
                        -FFABS(  s2[x  ] - s2[x  +stride]
                             - s2[x+1] + s2[x+1+stride]);
            }
        }
        s1+= stride;
        s2+= stride;
    }

    if(c) return score1 + FFABS(score2)*c->avctx->nsse_weight;
    else  return score1 + FFABS(score2)*8;
}

static int try_8x8basis_c(int16_t rem[64], int16_t weight[64], int16_t basis[64], int scale){
    int i;
    unsigned int sum=0;

    for(i=0; i<8*8; i++){
        int b= rem[i] + ((basis[i]*scale + (1<<(BASIS_SHIFT - RECON_SHIFT-1)))>>(BASIS_SHIFT - RECON_SHIFT));
        int w= weight[i];
        b>>= RECON_SHIFT;
        assert(-512<b && b<512);

        sum += (w*b)*(w*b)>>4;
    }
    return sum>>2;
}

static void add_8x8basis_c(int16_t rem[64], int16_t basis[64], int scale){
    int i;

    for(i=0; i<8*8; i++){
        rem[i] += (basis[i]*scale + (1<<(BASIS_SHIFT - RECON_SHIFT-1)))>>(BASIS_SHIFT - RECON_SHIFT);
    }
}

/**
 * permutes an 8x8 block.
 * @param block the block which will be permuted according to the given permutation vector
 * @param permutation the permutation vector
 * @param last the last non zero coefficient in scantable order, used to speed the permutation up
 * @param scantable the used scantable, this is only used to speed the permutation up, the block is not
 *                  (inverse) permutated to scantable order!
 */
void ff_block_permute(DCTELEM *block, uint8_t *permutation, const uint8_t *scantable, int last)
{
    int i;
    DCTELEM temp[64];

    if(last<=0) return;
    //if(permutation[1]==1) return; //FIXME it is ok but not clean and might fail for some permutations

    for(i=0; i<=last; i++){
        const int j= scantable[i];
        temp[j]= block[j];
        block[j]=0;
    }

    for(i=0; i<=last; i++){
        const int j= scantable[i];
        const int perm_j= permutation[j];
        block[perm_j]= temp[j];
    }
}

static int zero_cmp(void *s, uint8_t *a, uint8_t *b, int stride, int h){
    return 0;
}

void ff_set_cmp(DSPContext* c, me_cmp_func *cmp, int type){
    int i;

    memset(cmp, 0, sizeof(void*)*5);

    for(i=0; i<5; i++){
        switch(type&0xFF){
        case FF_CMP_SAD:
            cmp[i]= c->sad[i];
            break;
        case FF_CMP_SATD:
            cmp[i]= c->hadamard8_diff[i];
            break;
        case FF_CMP_SSE:
            cmp[i]= c->sse[i];
            break;
        case FF_CMP_DCT:
            cmp[i]= c->dct_sad[i];
            break;
        case FF_CMP_DCT264:
            cmp[i]= c->dct264_sad[i];
            break;
        case FF_CMP_DCTMAX:
            cmp[i]= c->dct_max[i];
            break;
        case FF_CMP_PSNR:
            cmp[i]= c->quant_psnr[i];
            break;
        case FF_CMP_BIT:
            cmp[i]= c->bit[i];
            break;
        case FF_CMP_RD:
            cmp[i]= c->rd[i];
            break;
        case FF_CMP_VSAD:
            cmp[i]= c->vsad[i];
            break;
        case FF_CMP_VSSE:
            cmp[i]= c->vsse[i];
            break;
        case FF_CMP_ZERO:
            cmp[i]= zero_cmp;
            break;
        case FF_CMP_NSSE:
            cmp[i]= c->nsse[i];
            break;
#ifdef CONFIG_SNOW_ENCODER
        case FF_CMP_W53:
            cmp[i]= c->w53[i];
            break;
        case FF_CMP_W97:
            cmp[i]= c->w97[i];
            break;
#endif
        default:
            av_log(NULL, AV_LOG_ERROR,"internal error in cmp function selection\n");
        }
    }
}

/**
 * memset(blocks, 0, sizeof(DCTELEM)*6*64)
 */
static void clear_blocks_c(DCTELEM *blocks)
{
    memset(blocks, 0, sizeof(DCTELEM)*6*64);
}

static void add_bytes_c(uint8_t *dst, uint8_t *src, int w){
    int i;
    for(i=0; i+7<w; i+=8){
        dst[i+0] += src[i+0];
        dst[i+1] += src[i+1];
        dst[i+2] += src[i+2];
        dst[i+3] += src[i+3];
        dst[i+4] += src[i+4];
        dst[i+5] += src[i+5];
        dst[i+6] += src[i+6];
        dst[i+7] += src[i+7];
    }
    for(; i<w; i++)
        dst[i+0] += src[i+0];
}

static void diff_bytes_c(uint8_t *dst, uint8_t *src1, uint8_t *src2, int w){
    int i;
    for(i=0; i+7<w; i+=8){
        dst[i+0] = src1[i+0]-src2[i+0];
        dst[i+1] = src1[i+1]-src2[i+1];
        dst[i+2] = src1[i+2]-src2[i+2];
        dst[i+3] = src1[i+3]-src2[i+3];
        dst[i+4] = src1[i+4]-src2[i+4];
        dst[i+5] = src1[i+5]-src2[i+5];
        dst[i+6] = src1[i+6]-src2[i+6];
        dst[i+7] = src1[i+7]-src2[i+7];
    }
    for(; i<w; i++)
        dst[i+0] = src1[i+0]-src2[i+0];
}

static void sub_hfyu_median_prediction_c(uint8_t *dst, uint8_t *src1, uint8_t *src2, int w, int *left, int *left_top){
    int i;
    uint8_t l, lt;

    l= *left;
    lt= *left_top;

    for(i=0; i<w; i++){
        const int pred= mid_pred(l, src1[i], (l + src1[i] - lt)&0xFF);
        lt= src1[i];
        l= src2[i];
        dst[i]= l - pred;
    }

    *left= l;
    *left_top= lt;
}

#define BUTTERFLY2(o1,o2,i1,i2) \
o1= (i1)+(i2);\
o2= (i1)-(i2);

#define BUTTERFLY1(x,y) \
{\
    int a,b;\
    a= x;\
    b= y;\
    x= a+b;\
    y= a-b;\
}

#define BUTTERFLYA(x,y) (FFABS((x)+(y)) + FFABS((x)-(y)))

static int hadamard8_diff8x8_c(/*MpegEncContext*/ void *s, uint8_t *dst, uint8_t *src, int stride, int h){
    int i;
    int temp[64];
    int sum=0;

    assert(h==8);

    for(i=0; i<8; i++){
        //FIXME try pointer walks
        BUTTERFLY2(temp[8*i+0], temp[8*i+1], src[stride*i+0]-dst[stride*i+0],src[stride*i+1]-dst[stride*i+1]);
        BUTTERFLY2(temp[8*i+2], temp[8*i+3], src[stride*i+2]-dst[stride*i+2],src[stride*i+3]-dst[stride*i+3]);
        BUTTERFLY2(temp[8*i+4], temp[8*i+5], src[stride*i+4]-dst[stride*i+4],src[stride*i+5]-dst[stride*i+5]);
        BUTTERFLY2(temp[8*i+6], temp[8*i+7], src[stride*i+6]-dst[stride*i+6],src[stride*i+7]-dst[stride*i+7]);

        BUTTERFLY1(temp[8*i+0], temp[8*i+2]);
        BUTTERFLY1(temp[8*i+1], temp[8*i+3]);
        BUTTERFLY1(temp[8*i+4], temp[8*i+6]);
        BUTTERFLY1(temp[8*i+5], temp[8*i+7]);

        BUTTERFLY1(temp[8*i+0], temp[8*i+4]);
        BUTTERFLY1(temp[8*i+1], temp[8*i+5]);
        BUTTERFLY1(temp[8*i+2], temp[8*i+6]);
        BUTTERFLY1(temp[8*i+3], temp[8*i+7]);
    }

    for(i=0; i<8; i++){
        BUTTERFLY1(temp[8*0+i], temp[8*1+i]);
        BUTTERFLY1(temp[8*2+i], temp[8*3+i]);
        BUTTERFLY1(temp[8*4+i], temp[8*5+i]);
        BUTTERFLY1(temp[8*6+i], temp[8*7+i]);

        BUTTERFLY1(temp[8*0+i], temp[8*2+i]);
        BUTTERFLY1(temp[8*1+i], temp[8*3+i]);
        BUTTERFLY1(temp[8*4+i], temp[8*6+i]);
        BUTTERFLY1(temp[8*5+i], temp[8*7+i]);

        sum +=
             BUTTERFLYA(temp[8*0+i], temp[8*4+i])
            +BUTTERFLYA(temp[8*1+i], temp[8*5+i])
            +BUTTERFLYA(temp[8*2+i], temp[8*6+i])
            +BUTTERFLYA(temp[8*3+i], temp[8*7+i]);
    }
#if 0
static int maxi=0;
if(sum>maxi){
    maxi=sum;
    printf("MAX:%d\n", maxi);
}
#endif
    return sum;
}

static int hadamard8_intra8x8_c(/*MpegEncContext*/ void *s, uint8_t *src, uint8_t *dummy, int stride, int h){
    int i;
    int temp[64];
    int sum=0;

    assert(h==8);

    for(i=0; i<8; i++){
        //FIXME try pointer walks
        
        BUTTERFLY2(temp[8*i+0], temp[8*i+1], src[stride*i+0],src[stride*i+1]);
        BUTTERFLY2(temp[8*i+2], temp[8*i+3], src[stride*i+2],src[stride*i+3]);
        BUTTERFLY2(temp[8*i+4], temp[8*i+5], src[stride*i+4],src[stride*i+5]);
        BUTTERFLY2(temp[8*i+6], temp[8*i+7], src[stride*i+6],src[stride*i+7]);

        BUTTERFLY1(temp[8*i+0], temp[8*i+2]);
        BUTTERFLY1(temp[8*i+1], temp[8*i+3]);
        BUTTERFLY1(temp[8*i+4], temp[8*i+6]);
        BUTTERFLY1(temp[8*i+5], temp[8*i+7]);

        BUTTERFLY1(temp[8*i+0], temp[8*i+4]);
        BUTTERFLY1(temp[8*i+1], temp[8*i+5]);
        BUTTERFLY1(temp[8*i+2], temp[8*i+6]);
        BUTTERFLY1(temp[8*i+3], temp[8*i+7]);
    }

    for(i=0; i<8; i++){
        BUTTERFLY1(temp[8*0+i], temp[8*1+i]);
        BUTTERFLY1(temp[8*2+i], temp[8*3+i]);
        BUTTERFLY1(temp[8*4+i], temp[8*5+i]);
        BUTTERFLY1(temp[8*6+i], temp[8*7+i]);

        BUTTERFLY1(temp[8*0+i], temp[8*2+i]);
        BUTTERFLY1(temp[8*1+i], temp[8*3+i]);
        BUTTERFLY1(temp[8*4+i], temp[8*6+i]);
        BUTTERFLY1(temp[8*5+i], temp[8*7+i]);

        sum +=
             BUTTERFLYA(temp[8*0+i], temp[8*4+i])
            +BUTTERFLYA(temp[8*1+i], temp[8*5+i])
            +BUTTERFLYA(temp[8*2+i], temp[8*6+i])
            +BUTTERFLYA(temp[8*3+i], temp[8*7+i]);
    }

    sum -= FFABS(temp[8*0] + temp[8*4]); // -mean

    return sum;
}

static int dct_sad8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    DECLARE_ALIGNED_16(uint64_t, aligned_temp[sizeof(DCTELEM)*64/8]);
    DCTELEM * const temp= (DCTELEM*)aligned_temp;
    
    assert(h==8);

    s->dsp.diff_pixels(temp, src1, src2, stride);
    s->dsp.fdct(temp);
    return s->dsp.sum_abs_dctelem(temp);
}

#ifdef CONFIG_GPL
#define DCT8_1D {\
    const int s07 = SRC(0) + SRC(7);\
    const int s16 = SRC(1) + SRC(6);\
    const int s25 = SRC(2) + SRC(5);\
    const int s34 = SRC(3) + SRC(4);\
    const int a0 = s07 + s34;\
    const int a1 = s16 + s25;\
    const int a2 = s07 - s34;\
    const int a3 = s16 - s25;\
    const int d07 = SRC(0) - SRC(7);\
    const int d16 = SRC(1) - SRC(6);\
    const int d25 = SRC(2) - SRC(5);\
    const int d34 = SRC(3) - SRC(4);\
    const int a4 = d16 + d25 + (d07 + (d07>>1));\
    const int a5 = d07 - d34 - (d25 + (d25>>1));\
    const int a6 = d07 + d34 - (d16 + (d16>>1));\
    const int a7 = d16 - d25 + (d34 + (d34>>1));\
    DST(0,  a0 + a1     ) ;\
    DST(1,  a4 + (a7>>2)) ;\
    DST(2,  a2 + (a3>>1)) ;\
    DST(3,  a5 + (a6>>2)) ;\
    DST(4,  a0 - a1     ) ;\
    DST(5,  a6 - (a5>>2)) ;\
    DST(6, (a2>>1) - a3 ) ;\
    DST(7, (a4>>2) - a7 ) ;\
}

static int dct264_sad8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    DCTELEM dct[8][8];
    int i;
    int sum=0;

    s->dsp.diff_pixels(dct[0], src1, src2, stride);

#define SRC(x) dct[i][x]
#define DST(x,v) dct[i][x]= v
    for( i = 0; i < 8; i++ )
        DCT8_1D
#undef SRC
#undef DST

#define SRC(x) dct[x][i]
#define DST(x,v) sum += FFABS(v)
    for( i = 0; i < 8; i++ )
        DCT8_1D
#undef SRC
#undef DST
    return sum;
}
#endif

static int dct_max8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    DECLARE_ALIGNED_8(uint64_t, aligned_temp[sizeof(DCTELEM)*64/8]);
    DCTELEM * const temp= (DCTELEM*)aligned_temp;
    int sum=0, i;

    assert(h==8);

    s->dsp.diff_pixels(temp, src1, src2, stride);
    s->dsp.fdct(temp);

    for(i=0; i<64; i++)
    {
    sum= FFMAX(sum, FFABS(temp[i]));
    }
    return sum;
}

static int quant_psnr8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    DECLARE_ALIGNED_8 (uint64_t, aligned_temp[sizeof(DCTELEM)*64*2/8]);
    DCTELEM * const temp= (DCTELEM*)aligned_temp;
    DCTELEM * const bak = ((DCTELEM*)aligned_temp)+64;
    int sum=0, i;

    assert(h==8);
    s->mb_intra=0;

    s->dsp.diff_pixels(temp, src1, src2, stride);

    memcpy(bak, temp, 64*sizeof(DCTELEM));

    s->block_last_index[0/*FIXME*/]= s->fast_dct_quantize(s, temp, 0/*FIXME*/, s->qscale, &i);
    s->dct_unquantize_inter(s, temp, 0, s->qscale);
    simple_idct(temp); //FIXME

    for(i=0; i<64; i++)
    {
    sum+= (temp[i]-bak[i])*(temp[i]-bak[i]);
    }
    return sum;
}

static int rd8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    const uint8_t *scantable= s->intra_scantable.permutated;
    DECLARE_ALIGNED_8 (uint64_t, aligned_temp[sizeof(DCTELEM)*64/8]);
    DECLARE_ALIGNED_8 (uint64_t, aligned_bak[stride]);
    DCTELEM * const temp= (DCTELEM*)aligned_temp;
    uint8_t * const bak= (uint8_t*)aligned_bak;
    int i, last, run, bits, level, distoration, start_i;
    const int esc_length= s->ac_esc_length;
    uint8_t * length;
    uint8_t * last_length;

    assert(h==8);

    for(i=0; i<8; i++){
        
        ((uint32_t*)(bak + i*stride))[0]= ((uint32_t*)(src2 + i*stride))[0];
        ((uint32_t*)(bak + i*stride))[1]= ((uint32_t*)(src2 + i*stride))[1];
    }

    s->dsp.diff_pixels(temp, src1, src2, stride);

    s->block_last_index[0/*FIXME*/]= last= s->fast_dct_quantize(s, temp, 0/*FIXME*/, s->qscale, &i);

    bits=0;

    if (s->mb_intra) {
        start_i = 1;
        length     = s->intra_ac_vlc_length;
        last_length= s->intra_ac_vlc_last_length;
        bits+= s->luma_dc_vlc_length[temp[0] + 256]; //FIXME chroma
    } else {
        start_i = 0;
        length     = s->inter_ac_vlc_length;
        last_length= s->inter_ac_vlc_last_length;
    }

    if(last>=start_i){
        run=0;
        for(i=start_i; i<last; i++){
            int j= scantable[i];
            level= temp[j];

            if(level){
                level+=64;
                if((level&(~127)) == 0){
                    bits+= length[UNI_AC_ENC_INDEX(run, level)];
                }else
                    bits+= esc_length;
                run=0;
            }else
                run++;
        }
        i= scantable[last];

        level= temp[i] + 64;

        assert(level - 64);

        if((level&(~127)) == 0){
            bits+= last_length[UNI_AC_ENC_INDEX(run, level)];
        }else
            bits+= esc_length;

    }

    if(last>=0){
        if(s->mb_intra)
            s->dct_unquantize_intra(s, temp, 0, s->qscale);
        else
            s->dct_unquantize_inter(s, temp, 0, s->qscale);
    }

    s->dsp.idct_add(bak, stride, temp);

    distoration= s->dsp.sse[1](NULL, bak, src1, stride, 8);

    return distoration + ((bits*s->qscale*s->qscale*109 + 64)>>7);
}

static int bit8x8_c(/*MpegEncContext*/ void *c, uint8_t *src1, uint8_t *src2, int stride, int h){
    MpegEncContext * const s= (MpegEncContext *)c;
    const uint8_t *scantable= s->intra_scantable.permutated;
    DECLARE_ALIGNED_8 (uint64_t, aligned_temp[sizeof(DCTELEM)*64/8]);
    DCTELEM * const temp= (DCTELEM*)aligned_temp;
    int i, last, run, bits, level, start_i;
    const int esc_length= s->ac_esc_length;
    uint8_t * length;
    uint8_t * last_length;

    assert(h==8);

    s->dsp.diff_pixels(temp, src1, src2, stride);

    s->block_last_index[0/*FIXME*/]= last= s->fast_dct_quantize(s, temp, 0/*FIXME*/, s->qscale, &i);

    bits=0;

    if (s->mb_intra) {
        start_i = 1;
        length     = s->intra_ac_vlc_length;
        last_length= s->intra_ac_vlc_last_length;
        bits+= s->luma_dc_vlc_length[temp[0] + 256]; //FIXME chroma
    } else {
        start_i = 0;
        length     = s->inter_ac_vlc_length;
        last_length= s->inter_ac_vlc_last_length;
    }

    if(last>=start_i){
        run=0;
        for(i=start_i; i<last; i++){
            int j= scantable[i];
            level= temp[j];

            if(level){
                level+=64;
                if((level&(~127)) == 0){
                    bits+= length[UNI_AC_ENC_INDEX(run, level)];
                }else
                    bits+= esc_length;
                run=0;
            }else
                run++;
        }
        i= scantable[last];

        level= temp[i] + 64;

        assert(level - 64);

        if((level&(~127)) == 0){
            bits+= last_length[UNI_AC_ENC_INDEX(run, level)];
        }else
            bits+= esc_length;
    }

    return bits;
}

static int vsad_intra16_c(/*MpegEncContext*/ void *c, uint8_t *s, uint8_t *dummy, int stride, int h){
    int score=0;
    int x,y;

    for(y=1; y<h; y++){
        for(x=0; x<16; x+=4){
          
  score+= FFABS(s[x  ] - s[x  +stride]) + FFABS(s[x+1] - s[x+1+stride])
                   +FFABS(s[x+2] - s[x+2+stride]) + FFABS(s[x+3] - s[x+3+stride]);
        }
        s+= stride;
    }

    return score;
}

static int vsad16_c(/*MpegEncContext*/ void *c, uint8_t *s1, uint8_t *s2, int stride, int h){
    int score=0;
    int x,y;

    for(y=1; y<h; y++){
        for(x=0; x<16; x++){
        score+= FFABS(s1[x  ] - s2[x ] - s1[x  +stride] + s2[x +stride]);
        }
        s1+= stride;
        s2+= stride;
    }

    return score;
}

#define SQ(a) ((a)*(a))
static int vsse_intra16_c(/*MpegEncContext*/ void *c, uint8_t *s, uint8_t *dummy, int stride, int h){
    int score=0;
    int x,y;

    for(y=1; y<h; y++){
        for(x=0; x<16; x+=4){
        score+= SQ(s[x  ] - s[x  +stride]) + SQ(s[x+1] - s[x+1+stride])
                   +SQ(s[x+2] - s[x+2+stride]) + SQ(s[x+3] - s[x+3+stride]);
        }
        s+= stride;
    }

    return score;
}

static int vsse16_c(/*MpegEncContext*/ void *c, uint8_t *s1, uint8_t *s2, int stride, int h){
    int score=0;
    int x,y;

    for(y=1; y<h; y++){
        for(x=0; x<16; x++){
        score+= SQ(s1[x  ] - s2[x ] - s1[x  +stride] + s2[x +stride]);
        }
        s1+= stride;
        s2+= stride;
    }

    return score;
}

static int ssd_int8_vs_int16_c(const int8_t *pix1, const int16_t *pix2,
                               int size){
    int score=0;
    int i;
    for(i=0; i<size; i++)
    score += (pix1[i]-pix2[i])*(pix1[i]-pix2[i]);
    return score;
}

WARPER8_16_SQ(hadamard8_diff8x8_c, hadamard8_diff16_c)
WARPER8_16_SQ(hadamard8_intra8x8_c, hadamard8_intra16_c)
WARPER8_16_SQ(dct_sad8x8_c, dct_sad16_c)
#ifdef CONFIG_GPL
WARPER8_16_SQ(dct264_sad8x8_c, dct264_sad16_c)
#endif
WARPER8_16_SQ(dct_max8x8_c, dct_max16_c)
WARPER8_16_SQ(quant_psnr8x8_c, quant_psnr16_c)
WARPER8_16_SQ(rd8x8_c, rd16_c)
WARPER8_16_SQ(bit8x8_c, bit16_c)

static void vector_fmul_c(float *dst, const float *src, int len){
    int i;
    for(i=0; i<len; i++)
    dst[i] *= src[i];
}

static void vector_fmul_reverse_c(float *dst, const float *src0, const float *src1, int len){
    int i;
    src1 += len-1;
    for(i=0; i<len; i++)
        dst[i] = src0[i] * src1[-i];
}

void ff_vector_fmul_add_add_c(float *dst, const float *src0, const float *src1, const float *src2, int src3, int len, int step){
    int i;
    for(i=0; i<len; i++)
     dst[i*step] = src0[i] * src1[i] + src2[i] + src3;
}

void ff_float_to_int16_c(int16_t *dst, const float *src, int len){
    int i;
    for(i=0; i<len; i++) {
        int_fast32_t tmp = ((int32_t*)src)[i];
        if(tmp & 0xf0000){
            tmp = (0x43c0ffff - tmp)>>31;
            // is this faster on some gcc/cpu combinations?
//          if(tmp > 0x43c0ffff) tmp = 0xFFFF;
//          else                 tmp = 0;
        }
        dst[i] = tmp - 0x8000;
    }
}

/* XXX: those functions should be suppressed ASAP when all IDCTs are
 converted */
static void ff_jref_idct_put(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct (block);
    put_pixels_clamped_c(block, dest, line_size);
}
static void ff_jref_idct_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct (block);
    add_pixels_clamped_c(block, dest, line_size);
}

static void ff_jref_idct4_put(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct4 (block);
    put_pixels_clamped4_c(block, dest, line_size);
}
static void ff_jref_idct4_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct4 (block);
    add_pixels_clamped4_c(block, dest, line_size);
}

static void ff_jref_idct2_put(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct2 (block);
    put_pixels_clamped2_c(block, dest, line_size);
}
static void ff_jref_idct2_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    j_rev_dct2 (block);
    add_pixels_clamped2_c(block, dest, line_size);
}

static void ff_jref_idct1_put(uint8_t *dest, int line_size, DCTELEM *block)
{
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    dest[0] = cm[(block[0] + 4)>>3];
}
static void ff_jref_idct1_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    dest[0] = cm[dest[0] + ((block[0] + 4)>>3)];
}

static void just_return(void *mem av_unused, int stride av_unused, int h av_unused) { return; }

/* init static data */
void dsputil_static_init(void)
{
    int i;
    for(i=0;i<256;i++) ff_cropTbl[i + MAX_NEG_CROP] = i;
    for(i=0;i<MAX_NEG_CROP;i++) {
        ff_cropTbl[i] = 0;
        ff_cropTbl[i + MAX_NEG_CROP + 256] = 255;
    }

    for(i=0;i<512;i++) {
        ff_squareTbl[i] = (i - 256)* (i - 256);
    }

    for(i=0; i<64; i++) inv_zigzag_direct16[ff_zigzag_direct[i]]= i+1;
}

int ff_check_alignment(void){
    static int did_fail=0;
    DECLARE_ALIGNED_16(int, aligned);

    if((long)&aligned & 15){
        if(!did_fail){
#if defined(HAVE_MMX) || defined(HAVE_ALTIVEC)
            av_log(NULL, AV_LOG_ERROR,
                "Compiler did not align stack variables. Libavcodec has been miscompiled\n"
                "and may be very slow or crash. This is not a bug in libavcodec,\n"
                "but in the compiler. You may try recompiling using gcc >= 4.2.\n"
                "Do not report crashes to FFmpeg developers.\n");
#endif
            did_fail=1;
        }
        return -1;
    }
    return 0;
}

#ifndef __LINUX__
#ifdef JZ4740_MXU_OPT
void ff_h264_lowres_idct_add_c(uint8_t *dst, int stride, DCTELEM *block){
    //idct_internal(dst, block, stride, 8, 3, 1);
    int i;
    DCTELEM *blk = block;

    block[0] += 1<<(3-1);
/*
    for(i=0; i<4; i++){
        const int z0=  blk[0]     +  blk[2];
        const int z1=  blk[0]     -  blk[2];
        const int z2= (blk[1]>>1) -  blk[3];
        const int z3=  blk[1]     + (blk[3]>>1);

        blk[0]= z0 + z3;
        blk[1]= z1 + z2;
        blk[2]= z1 - z2;
        blk[3]= z0 - z3;
        blk += 8;
    }
*/
// Loop 0,1
// load data blk[0~7]
    S32LDD(xr1, blk, 0x00);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x04);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x10);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x14);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x00);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x04);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x10);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x14);    //  xr4 (Nblk3, Nblk2)

// Loop 2,3
// load data blk[0~7]
    S32LDD(xr1, blk, 0x20);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x24);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x30);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x34);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x20);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x24);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x30);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x34);    //  xr4 (Nblk3, Nblk2)
//
/*
    for(i=0; i<4; i++){
        const int z0=  block[i + 8*0]     +  block[i + 8*2];
        const int z1=  block[i + 8*0]     -  block[i + 8*2];
        const int z2= (block[i + 8*1]>>1) -  block[i + 8*3];
        const int z3=  block[i + 8*1]     + (block[i + 8*3]>>1);

        dst[i + 0*stride]= cm[ dst[i + 0*stride] + ((z0 + z3) >> 3) ];
        dst[i + 1*stride]= cm[ dst[i + 1*stride] + ((z1 + z2) >> 3) ];
        dst[i + 2*stride]= cm[ dst[i + 2*stride] + ((z1 - z2) >> 3) ];
        dst[i + 3*stride]= cm[ dst[i + 3*stride] + ((z0 - z3) >> 3) ];
    }
*/
// Loop 0,1
// load data and do shift op
    S32LDD(xr10, blk, 0x00);    //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x10);    //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x20);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x30);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr11, xr6, xr9, xr12);// xr11 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                       // xr12 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr13, xr7, xr8, xr14);  //xr13 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                         //xr14 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr11,xr11,xr12,xr12, 3);
    Q16SAR(xr13,xr13,xr14,xr14, 3);

// Loop 2,3
// load data and do shift op
    S32LDD(xr10, blk, 0x04);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x14);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x24);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x34);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr1, xr6, xr9, xr2);// xr1 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr2 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr3, xr7, xr8, xr4);//xr3 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                     //xr4 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr1,xr1,xr2,xr2, 3);
    Q16SAR(xr3,xr3,xr4,xr4, 3);

// load dst data and do add op
// row 0
    S32LDD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr1, xr5, xr0, xr11);  // xr11 (b1, b0) xr1 (b3, b2)
    Q16SAT(xr5, xr1, xr11);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 1
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr3, xr5, xr0, xr13);  // xr13 (b1, b0) xr3 (b3, b2)
    Q16SAT(xr5, xr3, xr13);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 2
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr4, xr5, xr0, xr14);  // xr14 (b1, b0) xr4 (b3, b2)
    Q16SAT(xr5, xr4, xr14);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 3
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr2, xr5, xr0, xr12);  // xr12 (b1, b0) xr2 (b3, b2)
    Q16SAT(xr5, xr2, xr12);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
}
#else
void ff_h264_lowres_idct_add_c(uint8_t *dst, int stride, DCTELEM *block){
    idct_internal(dst, block, stride, 8, 3, 1);
}
#endif

#ifdef JZ4740_MXU_OPT
void ff_h264_lowres_idct_put_c(uint8_t *dst, int stride, DCTELEM *block){
//    idct_internal(dst, block, stride, 8, 3, 0);
    int i;
    DCTELEM *blk = block;

    block[0] += 1<<(3-1);

// Loop 0,1
// load data blk[0~7]
    S32LDD(xr1, blk, 0x00);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x04);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x10);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x14);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x00);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x04);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x10);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x14);    //  xr4 (Nblk3, Nblk2)

// Loop 2,3
// load data blk[0~7]
    S32LDD(xr1, blk, 0x20);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x24);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x30);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x34);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x20);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x24);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x30);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x34);    //  xr4 (Nblk3, Nblk2)
//
/*
    for(i=0; i<4; i++){
        const int z0=  block[i + 8*0]     +  block[i + 8*2];
        const int z1=  block[i + 8*0]     -  block[i + 8*2];
        const int z2= (block[i + 8*1]>>1) -  block[i + 8*3];
        const int z3=  block[i + 8*1]     + (block[i + 8*3]>>1);

        dst[i + 0*stride]= cm[(z0 + z3) >> 3 ];
        dst[i + 1*stride]= cm[(z1 + z2) >> 3 ];
        dst[i + 2*stride]= cm[(z1 - z2) >> 3 ];
        dst[i + 3*stride]= cm[(z0 - z3) >> 3 ];
    }
*/
// Loop 0,1
// load data and do shift op
    S32LDD(xr10, blk, 0x00);    //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x10);    //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x20);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x30);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr11, xr6, xr9, xr12);// xr11 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                       // xr12 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr13, xr7, xr8, xr14);  //xr13 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                         //xr14 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr11,xr11,xr12,xr12, 3);
    Q16SAR(xr13,xr13,xr14,xr14, 3);

// Loop 2,3
// load data and do shift op
    S32LDD(xr10, blk, 0x04);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x14);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x24);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x34);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr1, xr6, xr9, xr2);// xr1 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr2 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr3, xr7, xr8, xr4);//xr3 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                     //xr4 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr1,xr1,xr2,xr2, 3);
    Q16SAR(xr3,xr3,xr4,xr4, 3);

// load dst data and do add op
// row 0
    Q16SAT(xr5, xr1, xr11);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 1
    Q16SAT(xr5, xr3, xr13);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
// row 2
    Q16SAT(xr5, xr4, xr14);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
// row 3
    Q16SAT(xr5, xr2, xr12);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
}
#else
void ff_h264_lowres_idct_put_c(uint8_t *dst, int stride, DCTELEM *block){
    idct_internal(dst, block, stride, 8, 3, 0);
}
#endif

#endif //#ifdef USE_16M_SDRAM

#ifndef __LINUX__
void (*h264dsputil_init_funcp)(DSPContext* c, AVCodecContext *avctx) = NULL;
void (*ff_vp3_idct_put_c_funcp)(uint8_t *dest/*align 8*/, int line_size, DCTELEM *block/*align 16*/) = NULL;
void (*ff_vp3_idct_add_c_funcp)(uint8_t *dest/*align 8*/, int line_size, DCTELEM *block/*align 16*/) = NULL;
void (*ff_vp3_idct_c_funcp)(DCTELEM *block/* align 16*/) = NULL;
#else
extern void h264dsputil_init(DSPContext* c, AVCodecContext *avctx);
#endif

void dsputil_init(DSPContext* c, AVCodecContext *avctx)
{
    int i;

    ff_check_alignment();

#ifdef CONFIG_ENCODERS
    if(avctx->dct_algo==FF_DCT_FASTINT) {
        c->fdct = fdct_ifast;
        c->fdct248 = fdct_ifast248;
    }
    else if(avctx->dct_algo==FF_DCT_FAAN) {
        c->fdct = ff_faandct;
        c->fdct248 = ff_faandct248;
    }
    else {
        c->fdct = ff_jpeg_fdct_islow; //slow/accurate/default
        c->fdct248 = ff_fdct248_islow;
    }
#endif //CONFIG_ENCODERS

    if(avctx->lowres==1){
        if(avctx->idct_algo==FF_IDCT_INT || avctx->idct_algo==FF_IDCT_AUTO || !ENABLE_H264_DECODER){
            c->idct_put= ff_jref_idct4_put;
            c->idct_add= ff_jref_idct4_add;
        }else{
            c->idct_put= ff_h264_lowres_idct_put_c;
            c->idct_add= ff_h264_lowres_idct_add_c;
        }
        c->idct    = j_rev_dct4;
        c->idct_permutation_type= FF_NO_IDCT_PERM;
    }else if(avctx->lowres==2){
        c->idct_put= ff_jref_idct2_put;
        c->idct_add= ff_jref_idct2_add;
        c->idct    = j_rev_dct2;
        c->idct_permutation_type= FF_NO_IDCT_PERM;
    }else if(avctx->lowres==3){
        c->idct_put= ff_jref_idct1_put;
        c->idct_add= ff_jref_idct1_add;
        c->idct    = j_rev_dct1;
        c->idct_permutation_type= FF_NO_IDCT_PERM;
    }else{
        if(avctx->idct_algo==FF_IDCT_INT){
            c->idct_put= ff_jref_idct_put;
            c->idct_add= ff_jref_idct_add;
            c->idct    = j_rev_dct;
            c->idct_permutation_type= FF_LIBMPEG2_IDCT_PERM;
        }else if((ENABLE_VP3_DECODER || ENABLE_VP5_DECODER || ENABLE_VP6_DECODER || ENABLE_THEORA_DECODER ) &&
                avctx->idct_algo==FF_IDCT_VP3){
#ifndef __LINUX__
            c->idct_put= ff_vp3_idct_put_c_funcp;
            c->idct_add= ff_vp3_idct_add_c_funcp;
            c->idct    = ff_vp3_idct_c_funcp;
#else
            c->idct_put= ff_vp3_idct_put_c;
            c->idct_add= ff_vp3_idct_add_c;
            c->idct    = ff_vp3_idct_c;
#endif
            c->idct_permutation_type= FF_NO_IDCT_PERM;
        }else{ //accurate/default
            c->idct_put= simple_idct_put;
            c->idct_add= simple_idct_add;
            c->idct    = simple_idct;
            c->idct_permutation_type= FF_NO_IDCT_PERM;
        }
    }

#define dspfunc(PFX, IDX, NUM) \
    c->PFX ## _pixels_tab[IDX][ 0] = PFX ## NUM ## _mc00_c; \
    c->PFX ## _pixels_tab[IDX][ 1] = PFX ## NUM ## _mc10_c; \
    c->PFX ## _pixels_tab[IDX][ 2] = PFX ## NUM ## _mc20_c; \
    c->PFX ## _pixels_tab[IDX][ 3] = PFX ## NUM ## _mc30_c; \
    c->PFX ## _pixels_tab[IDX][ 4] = PFX ## NUM ## _mc01_c; \
    c->PFX ## _pixels_tab[IDX][ 5] = PFX ## NUM ## _mc11_c; \
    c->PFX ## _pixels_tab[IDX][ 6] = PFX ## NUM ## _mc21_c; \
    c->PFX ## _pixels_tab[IDX][ 7] = PFX ## NUM ## _mc31_c; \
    c->PFX ## _pixels_tab[IDX][ 8] = PFX ## NUM ## _mc02_c; \
    c->PFX ## _pixels_tab[IDX][ 9] = PFX ## NUM ## _mc12_c; \
    c->PFX ## _pixels_tab[IDX][10] = PFX ## NUM ## _mc22_c; \
    c->PFX ## _pixels_tab[IDX][11] = PFX ## NUM ## _mc32_c; \
    c->PFX ## _pixels_tab[IDX][12] = PFX ## NUM ## _mc03_c; \
    c->PFX ## _pixels_tab[IDX][13] = PFX ## NUM ## _mc13_c; \
    c->PFX ## _pixels_tab[IDX][14] = PFX ## NUM ## _mc23_c; \
    c->PFX ## _pixels_tab[IDX][15] = PFX ## NUM ## _mc33_c

    dspfunc(put_qpel, 0, 16);
    dspfunc(put_no_rnd_qpel, 0, 16);

    dspfunc(avg_qpel, 0, 16);
    /* dspfunc(avg_no_rnd_qpel, 0, 16); */

    dspfunc(put_qpel, 1, 8);
    dspfunc(put_no_rnd_qpel, 1, 8);

    dspfunc(avg_qpel, 1, 8);
    /* dspfunc(avg_no_rnd_qpel, 1, 8); */

#ifndef __LINUX__
    printf ("\n+++++++++ h264dsputil_init_funcp = 0x%08x +++++++++\n", (unsigned int)h264dsputil_init_funcp);
    if (h264dsputil_init_funcp != NULL)
      h264dsputil_init_funcp (c, avctx);
#else
    h264dsputil_init (c, avctx);
#endif
    c->get_pixels = get_pixels_c;
    c->diff_pixels = diff_pixels_c;
    c->put_pixels_clamped = put_pixels_clamped_c;
    c->put_signed_pixels_clamped = put_signed_pixels_clamped_c;
    c->add_pixels_clamped = add_pixels_clamped_c;
    c->add_pixels8 = add_pixels8_c;
    c->add_pixels4 = add_pixels4_c;
    c->sum_abs_dctelem = sum_abs_dctelem_c;
    c->gmc1 = gmc1_c;
    c->gmc = ff_gmc_c;
    c->clear_blocks = clear_blocks_c;
    c->pix_sum = pix_sum_c;
    c->pix_norm1 = pix_norm1_c;

    /* TODO [0] 16  [1] 8 */
    c->pix_abs[0][0] = pix_abs16_c;
    c->pix_abs[0][1] = pix_abs16_x2_c;
    c->pix_abs[0][2] = pix_abs16_y2_c;
    c->pix_abs[0][3] = pix_abs16_xy2_c;
    c->pix_abs[1][0] = pix_abs8_c;
    c->pix_abs[1][1] = pix_abs8_x2_c;
    c->pix_abs[1][2] = pix_abs8_y2_c;
    c->pix_abs[1][3] = pix_abs8_xy2_c;

#define dspfunc(PFX, IDX, NUM) \
    c->PFX ## _pixels_tab[IDX][0] = PFX ## _pixels ## NUM ## _c;     \
    c->PFX ## _pixels_tab[IDX][1] = PFX ## _pixels ## NUM ## _x2_c;  \
    c->PFX ## _pixels_tab[IDX][2] = PFX ## _pixels ## NUM ## _y2_c;  \
    c->PFX ## _pixels_tab[IDX][3] = PFX ## _pixels ## NUM ## _xy2_c

    dspfunc(put, 0, 16);
    dspfunc(put_no_rnd, 0, 16);
    dspfunc(put, 1, 8);
    dspfunc(put_no_rnd, 1, 8);
    dspfunc(put, 2, 4);
    dspfunc(put, 3, 2);

    dspfunc(avg, 0, 16);
    dspfunc(avg_no_rnd, 0, 16);
    dspfunc(avg, 1, 8);
    dspfunc(avg_no_rnd, 1, 8);
    dspfunc(avg, 2, 4);
    dspfunc(avg, 3, 2);
#undef dspfunc

    c->put_no_rnd_pixels_l2[0]= put_no_rnd_pixels16_l2_c;
    c->put_no_rnd_pixels_l2[1]= put_no_rnd_pixels8_l2_c;

    c->put_tpel_pixels_tab[ 0] = put_tpel_pixels_mc00_c;
    c->put_tpel_pixels_tab[ 1] = put_tpel_pixels_mc10_c;
    c->put_tpel_pixels_tab[ 2] = put_tpel_pixels_mc20_c;
    c->put_tpel_pixels_tab[ 4] = put_tpel_pixels_mc01_c;
    c->put_tpel_pixels_tab[ 5] = put_tpel_pixels_mc11_c;
    c->put_tpel_pixels_tab[ 6] = put_tpel_pixels_mc21_c;
    c->put_tpel_pixels_tab[ 8] = put_tpel_pixels_mc02_c;
    c->put_tpel_pixels_tab[ 9] = put_tpel_pixels_mc12_c;
    c->put_tpel_pixels_tab[10] = put_tpel_pixels_mc22_c;

    c->avg_tpel_pixels_tab[ 0] = avg_tpel_pixels_mc00_c;
    c->avg_tpel_pixels_tab[ 1] = avg_tpel_pixels_mc10_c;
    c->avg_tpel_pixels_tab[ 2] = avg_tpel_pixels_mc20_c;
    c->avg_tpel_pixels_tab[ 4] = avg_tpel_pixels_mc01_c;
    c->avg_tpel_pixels_tab[ 5] = avg_tpel_pixels_mc11_c;
    c->avg_tpel_pixels_tab[ 6] = avg_tpel_pixels_mc21_c;
    c->avg_tpel_pixels_tab[ 8] = avg_tpel_pixels_mc02_c;
    c->avg_tpel_pixels_tab[ 9] = avg_tpel_pixels_mc12_c;
    c->avg_tpel_pixels_tab[10] = avg_tpel_pixels_mc22_c;

#ifdef CONFIG_CAVS_DECODER
#ifndef __LINUX__
    if (ff_cavsdsp_init_funcp != NULL)
      ff_cavsdsp_init_funcp(c,avctx);
#else
    ff_cavsdsp_init(c,avctx);
#endif
#endif
#if defined(CONFIG_VC1_DECODER) || defined(CONFIG_WMV3_DECODER)
#ifndef __LINUX__
    if (ff_vc1dsp_init_funcp != NULL)
       ff_vc1dsp_init_funcp(c,avctx);
#else
    ff_vc1dsp_init(c,avctx);
#endif
#endif
#if defined(CONFIG_H264_ENCODER)
    ff_h264dspenc_init(c,avctx);
#endif

    c->put_mspel_pixels_tab[0]= put_mspel8_mc00_c;
    c->put_mspel_pixels_tab[1]= put_mspel8_mc10_c;
    c->put_mspel_pixels_tab[2]= put_mspel8_mc20_c;
    c->put_mspel_pixels_tab[3]= put_mspel8_mc30_c;
    c->put_mspel_pixels_tab[4]= put_mspel8_mc02_c;
    c->put_mspel_pixels_tab[5]= put_mspel8_mc12_c;
    c->put_mspel_pixels_tab[6]= put_mspel8_mc22_c;
    c->put_mspel_pixels_tab[7]= put_mspel8_mc32_c;

#define SET_CMP_FUNC(name) \
    c->name[0]= name ## 16_c;\
    c->name[1]= name ## 8x8_c;

    SET_CMP_FUNC(hadamard8_diff)
    c->hadamard8_diff[4]= hadamard8_intra16_c;
    SET_CMP_FUNC(dct_sad)
    SET_CMP_FUNC(dct_max)
#ifdef CONFIG_GPL
    SET_CMP_FUNC(dct264_sad)
#endif
    c->sad[0]= pix_abs16_c;
    c->sad[1]= pix_abs8_c;
    c->sse[0]= sse16_c;
    c->sse[1]= sse8_c;
    c->sse[2]= sse4_c;
    SET_CMP_FUNC(quant_psnr)
    SET_CMP_FUNC(rd)
    SET_CMP_FUNC(bit)
    c->vsad[0]= vsad16_c;
    c->vsad[4]= vsad_intra16_c;
    c->vsse[0]= vsse16_c;
    c->vsse[4]= vsse_intra16_c;
    c->nsse[0]= nsse16_c;
    c->nsse[1]= nsse8_c;
#ifdef CONFIG_SNOW_ENCODER
    c->w53[0]= w53_16_c;
    c->w53[1]= w53_8_c;
    c->w97[0]= w97_16_c;
    c->w97[1]= w97_8_c;
#endif

    c->ssd_int8_vs_int16 = ssd_int8_vs_int16_c;

    c->add_bytes= add_bytes_c;
    c->diff_bytes= diff_bytes_c;
    c->sub_hfyu_median_prediction= sub_hfyu_median_prediction_c;
    c->bswap_buf= bswap_buf;

    if (ENABLE_ANY_H263) {
        c->h263_h_loop_filter= h263_h_loop_filter_c;
        c->h263_v_loop_filter= h263_v_loop_filter_c;
    }

    c->h261_loop_filter= h261_loop_filter_c;

    c->try_8x8basis= try_8x8basis_c;
    c->add_8x8basis= add_8x8basis_c;

#ifdef CONFIG_SNOW_DECODER
    c->vertical_compose97i = ff_snow_vertical_compose97i;
    c->horizontal_compose97i = ff_snow_horizontal_compose97i;
    c->inner_add_yblock = ff_snow_inner_add_yblock;
#endif

#ifdef CONFIG_VORBIS_DECODER
#ifdef __LINUX__
    c->vorbis_inverse_coupling = vorbis_inverse_coupling;
#endif
#endif
#ifdef CONFIG_FLAC_ENCODER
    c->flac_compute_autocorr = ff_flac_compute_autocorr;
#endif
    c->vector_fmul = vector_fmul_c;
    c->vector_fmul_reverse = vector_fmul_reverse_c;
    c->vector_fmul_add_add = ff_vector_fmul_add_add_c;
    c->float_to_int16 = ff_float_to_int16_c;

    c->shrink[0]= ff_img_copy_plane;
    c->shrink[1]= ff_shrink22;
    c->shrink[2]= ff_shrink44;
    c->shrink[3]= ff_shrink88;

    c->prefetch= just_return;

    memset(c->put_2tap_qpel_pixels_tab, 0, sizeof(c->put_2tap_qpel_pixels_tab));
    memset(c->avg_2tap_qpel_pixels_tab, 0, sizeof(c->avg_2tap_qpel_pixels_tab));

    if (ENABLE_MMX)      dsputil_init_mmx   (c, avctx);
    if (ENABLE_ARMV4L)   dsputil_init_armv4l(c, avctx);
    if (ENABLE_MLIB)     dsputil_init_mlib  (c, avctx);
    if (ENABLE_VIS)      dsputil_init_vis   (c, avctx);
    if (ENABLE_ALPHA)    dsputil_init_alpha (c, avctx);
    if (ENABLE_POWERPC)  dsputil_init_ppc   (c, avctx);
    if (ENABLE_MMI)      dsputil_init_mmi   (c, avctx);
    if (ENABLE_SH4)      dsputil_init_sh4   (c, avctx);
    if (ENABLE_BFIN)     dsputil_init_bfin  (c, avctx);

    for(i=0; i<64; i++){
        if(!c->put_2tap_qpel_pixels_tab[0][i])
            c->put_2tap_qpel_pixels_tab[0][i]= c->put_h264_qpel_pixels_tab[0][i];
        if(!c->avg_2tap_qpel_pixels_tab[0][i])
            c->avg_2tap_qpel_pixels_tab[0][i]= c->avg_h264_qpel_pixels_tab[0][i];
    }

    switch(c->idct_permutation_type){
    case FF_NO_IDCT_PERM:
        for(i=0; i<64; i++)
            c->idct_permutation[i]= i;
        break;
    case FF_LIBMPEG2_IDCT_PERM:
        for(i=0; i<64; i++)
            c->idct_permutation[i]= (i & 0x38) | ((i & 6) >> 1) | ((i & 1) << 2);
        break;
    case FF_SIMPLE_IDCT_PERM:
        for(i=0; i<64; i++)
            c->idct_permutation[i]= simple_mmx_permutation[i];
        break;
    case FF_TRANSPOSE_IDCT_PERM:
        for(i=0; i<64; i++)
            c->idct_permutation[i]= ((i&7)<<3) | (i>>3);
        break;
    case FF_PARTTRANS_IDCT_PERM:
        for(i=0; i<64; i++)
            c->idct_permutation[i]= (i&0x24) | ((i&3)<<3) | ((i>>3)&3);
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Internal error, IDCT permutation not set\n");
    }
}

