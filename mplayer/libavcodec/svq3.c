/*
 * Copyright (c) 2003 The FFmpeg Project.
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

/*
 * How to use this decoder:
 * SVQ3 data is transported within Apple Quicktime files. Quicktime files
 * have stsd atoms to describe media trak properties. A stsd atom for a
 * video trak contains 1 or more ImageDescription atoms. These atoms begin
 * with the 4-byte length of the atom followed by the codec fourcc. Some
 * decoders need information in this atom to operate correctly. Such
 * is the case with SVQ3. In order to get the best use out of this decoder,
 * the calling app must make the SVQ3 ImageDescription atom available
 * via the AVCodecContext's extradata[_size] field:
 *
 * AVCodecContext.extradata = pointer to ImageDescription, first characters
 * are expected to be 'S', 'V', 'Q', and '3', NOT the 4-byte atom length
 * AVCodecContext.extradata_size = size of ImageDescription atom memory
 * buffer (which will be the same as the ImageDescription atom size field
 * from the QT file, minus 4 bytes since the length is missing)
 *
 * You will know you have these parameters passed correctly when the decoder
 * correctly decodes this file:
 *  ftp://ftp.mplayerhq.hu/MPlayer/samples/V-codecs/SVQ3/Vertical400kbit.sorenson3.mov
 */

/**
 * @file svq3.c
 * svq3 decoder.
 */
#include "dsputil.h"
#include "avcodec.h"
#include "mpegvideo.h"
#include "mtvh264.h"
#include "h264data.h"
#include "h264_parser.h"
#include "golomb.h"

//#undef NDEBUG
#include <assert.h>

#ifdef JZC_CRC_VER
extern short crc_code;
extern short mpFrame;
#endif

#define FULLPEL_MODE  1
#define HALFPEL_MODE  2
#define THIRDPEL_MODE 3
#define PREDICT_MODE  4

/* dual scan (from some older h264 draft)
 o-->o-->o   o
         |  /|
 o   o   o / o
 | / |   |/  |
 o   o   o   o
   /
 o-->o-->o-->o
*/
static const uint8_t svq3_scan[16]={
 0+0*4, 1+0*4, 2+0*4, 2+1*4,
 2+2*4, 3+0*4, 3+1*4, 3+2*4,
 0+1*4, 0+2*4, 1+1*4, 1+2*4,
 0+3*4, 1+3*4, 2+3*4, 3+3*4,
};

static const uint8_t svq3_pred_0[25][2] = {
  { 0, 0 },
  { 1, 0 }, { 0, 1 },
  { 0, 2 }, { 1, 1 }, { 2, 0 },
  { 3, 0 }, { 2, 1 }, { 1, 2 }, { 0, 3 },
  { 0, 4 }, { 1, 3 }, { 2, 2 }, { 3, 1 }, { 4, 0 },
  { 4, 1 }, { 3, 2 }, { 2, 3 }, { 1, 4 },
  { 2, 4 }, { 3, 3 }, { 4, 2 },
  { 4, 3 }, { 3, 4 },
  { 4, 4 }
};

static const int8_t svq3_pred_1[6][6][5] = {
  { { 2,-1,-1,-1,-1 }, { 2, 1,-1,-1,-1 }, { 1, 2,-1,-1,-1 },
    { 2, 1,-1,-1,-1 }, { 1, 2,-1,-1,-1 }, { 1, 2,-1,-1,-1 } },
  { { 0, 2,-1,-1,-1 }, { 0, 2, 1, 4, 3 }, { 0, 1, 2, 4, 3 },
    { 0, 2, 1, 4, 3 }, { 2, 0, 1, 3, 4 }, { 0, 4, 2, 1, 3 } },
  { { 2, 0,-1,-1,-1 }, { 2, 1, 0, 4, 3 }, { 1, 2, 4, 0, 3 },
    { 2, 1, 0, 4, 3 }, { 2, 1, 4, 3, 0 }, { 1, 2, 4, 0, 3 } },
  { { 2, 0,-1,-1,-1 }, { 2, 0, 1, 4, 3 }, { 1, 2, 0, 4, 3 },
    { 2, 1, 0, 4, 3 }, { 2, 1, 3, 4, 0 }, { 2, 4, 1, 0, 3 } },
  { { 0, 2,-1,-1,-1 }, { 0, 2, 1, 3, 4 }, { 1, 2, 3, 0, 4 },
    { 2, 0, 1, 3, 4 }, { 2, 1, 3, 0, 4 }, { 2, 0, 4, 3, 1 } },
  { { 0, 2,-1,-1,-1 }, { 0, 2, 4, 1, 3 }, { 1, 4, 2, 0, 3 },
    { 4, 2, 0, 1, 3 }, { 2, 0, 1, 4, 3 }, { 4, 2, 1, 0, 3 } },
};

static const struct { uint8_t run; uint8_t level; } svq3_dct_tables[2][16] = {
  { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 0, 2 }, { 3, 1 }, { 4, 1 }, { 5, 1 },
    { 0, 3 }, { 1, 2 }, { 2, 2 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 }, { 0, 4 } },
  { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 0, 2 }, { 2, 1 }, { 0, 3 }, { 0, 4 }, { 0, 5 },
    { 3, 1 }, { 4, 1 }, { 1, 2 }, { 1, 3 }, { 0, 6 }, { 0, 7 }, { 0, 8 }, { 0, 9 } }
};

static const uint32_t svq3_dequant_coeff[32] = {
   3881,  4351,  4890,  5481,  6154,  6914,  7761,  8718,
   9781, 10987, 12339, 13828, 15523, 17435, 19561, 21873,
  24552, 27656, 30847, 34870, 38807, 43747, 49103, 54683,
  61694, 68745, 77615, 89113,100253,109366,126635,141533
};

//move from h264.c
#define DELAYED_PIC_REF 4

static VLC coeff_token_vlc[4];
static VLC chroma_dc_coeff_token_vlc;

static VLC total_zeros_vlc[15];
static VLC chroma_dc_total_zeros_vlc[3];

static VLC run_vlc[6];
static VLC run7_vlc;

static void svq3_luma_dc_dequant_idct_c(DCTELEM *block, int qp);
static void svq3_add_idct_c(uint8_t *dst, DCTELEM *block, int stride, int qp, int dc);
static void filter_mb( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize);
static void filter_mb_fast( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize);

static av_always_inline uint32_t pack16to32(int a, int b){
#ifdef WORDS_BIGENDIAN
   return (b&0xFFFF) + (a<<16);
#else
   return (a&0xFFFF) + (b<<16);
#endif
}


extern const uint8_t ff_rem6[52];
extern const uint8_t ff_div6[52];
/**
 * fill a rectangle.
 * @param h height of the rectangle, should be a constant
 * @param w width of the rectangle, should be a constant
 * @param size the size of val (1 or 4), should be a constant
 */
static av_always_inline void fill_rectangle(void *vp, int w, int h, int stride, uint32_t val, int size){
    uint8_t *p= (uint8_t*)vp;
    assert(size==1 || size==4);
    assert(w<=4);

    w      *= size;
    stride *= size;

    assert((((long)vp)&(FFMIN(w, STRIDE_ALIGN)-1)) == 0);
    assert((stride&(w-1))==0);
    if(w==2){
        const uint16_t v= size==4 ? val : val*0x0101;
        *(uint16_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint16_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint16_t*)(p + 2*stride)= v;
        *(uint16_t*)(p + 3*stride)= v;
    }else if(w==4){
        const uint32_t v= size==4 ? val : val*0x01010101;
        *(uint32_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint32_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint32_t*)(p + 2*stride)= v;
        *(uint32_t*)(p + 3*stride)= v;
    }else if(w==8){
    //gcc can't optimize 64bit math on x86_32
#if defined(ARCH_X86_64) || (defined(MP_WORDSIZE) && MP_WORDSIZE >= 64)
        const uint64_t v= val*0x0100000001ULL;
        *(uint64_t*)(p + 0*stride)= v;
        if(h==1) return;
        *(uint64_t*)(p + 1*stride)= v;
        if(h==2) return;
        *(uint64_t*)(p + 2*stride)= v;
        *(uint64_t*)(p + 3*stride)= v;
    }else if(w==16){
        const uint64_t v= val*0x0100000001ULL;
        *(uint64_t*)(p + 0+0*stride)= v;
        *(uint64_t*)(p + 8+0*stride)= v;
        *(uint64_t*)(p + 0+1*stride)= v;
        *(uint64_t*)(p + 8+1*stride)= v;
        if(h==2) return;
        *(uint64_t*)(p + 0+2*stride)= v;
        *(uint64_t*)(p + 8+2*stride)= v;
        *(uint64_t*)(p + 0+3*stride)= v;
        *(uint64_t*)(p + 8+3*stride)= v;
#else
        *(uint32_t*)(p + 0+0*stride)= val;
        *(uint32_t*)(p + 4+0*stride)= val;
        if(h==1) return;
        *(uint32_t*)(p + 0+1*stride)= val;
        *(uint32_t*)(p + 4+1*stride)= val;
        if(h==2) return;
        *(uint32_t*)(p + 0+2*stride)= val;
        *(uint32_t*)(p + 4+2*stride)= val;
        *(uint32_t*)(p + 0+3*stride)= val;
        *(uint32_t*)(p + 4+3*stride)= val;
    }else if(w==16){
        *(uint32_t*)(p + 0+0*stride)= val;
        *(uint32_t*)(p + 4+0*stride)= val;
        *(uint32_t*)(p + 8+0*stride)= val;
        *(uint32_t*)(p +12+0*stride)= val;
        *(uint32_t*)(p + 0+1*stride)= val;
        *(uint32_t*)(p + 4+1*stride)= val;
        *(uint32_t*)(p + 8+1*stride)= val;
        *(uint32_t*)(p +12+1*stride)= val;
        if(h==2) return;
        *(uint32_t*)(p + 0+2*stride)= val;
        *(uint32_t*)(p + 4+2*stride)= val;
        *(uint32_t*)(p + 8+2*stride)= val;
        *(uint32_t*)(p +12+2*stride)= val;
        *(uint32_t*)(p + 0+3*stride)= val;
        *(uint32_t*)(p + 4+3*stride)= val;
        *(uint32_t*)(p + 8+3*stride)= val;
        *(uint32_t*)(p +12+3*stride)= val;
#endif
    }else
        assert(0);
    assert(h==4);
}

static void fill_caches(H264Context *h, int mb_type, int for_deblock){
    MpegEncContext * const s = &h->s;
    const int mb_xy= s->mb_x + s->mb_y*s->mb_stride;
    int topleft_xy, top_xy, topright_xy, left_xy[2];
    int topleft_type, top_type, topright_type, left_type[2];
    int left_block[8];
    int i;

    //FIXME deblocking could skip the intra and nnz parts.
    if(for_deblock && (h->slice_num == 1 || h->slice_table[mb_xy] == h->slice_table[mb_xy-s->mb_stride]) && !FRAME_MBAFF)
        return;

    //wow what a mess, why didn't they simplify the interlacing&intra stuff, i can't imagine that these complex rules are worth it

    top_xy     = mb_xy  - (s->mb_stride << FIELD_PICTURE);
    topleft_xy = top_xy - 1;
    topright_xy= top_xy + 1;
    left_xy[1] = left_xy[0] = mb_xy-1;
    left_block[0]= 0;
    left_block[1]= 1;
    left_block[2]= 2;
    left_block[3]= 3;
    left_block[4]= 7;
    left_block[5]= 10;
    left_block[6]= 8;
    left_block[7]= 11;
    if(FRAME_MBAFF){
        const int pair_xy          = s->mb_x     + (s->mb_y & ~1)*s->mb_stride;
        const int top_pair_xy      = pair_xy     - s->mb_stride;
        const int topleft_pair_xy  = top_pair_xy - 1;
        const int topright_pair_xy = top_pair_xy + 1;
        const int topleft_mb_frame_flag  = !IS_INTERLACED(s->current_picture.mb_type[topleft_pair_xy]);
        const int top_mb_frame_flag      = !IS_INTERLACED(s->current_picture.mb_type[top_pair_xy]);
        const int topright_mb_frame_flag = !IS_INTERLACED(s->current_picture.mb_type[topright_pair_xy]);
        const int left_mb_frame_flag = !IS_INTERLACED(s->current_picture.mb_type[pair_xy-1]);
        const int curr_mb_frame_flag = !IS_INTERLACED(mb_type);
        const int bottom = (s->mb_y & 1);
        tprintf(s->avctx, "fill_caches: curr_mb_frame_flag:%d, left_mb_frame_flag:%d, topleft_mb_frame_flag:%d, top_mb_frame_flag:%d, topright_mb_frame_flag:%d\n", curr_mb_frame_flag, left_mb_frame_flag, topleft_mb_frame_flag, top_mb_frame_flag, topright_mb_frame_flag);
        if (bottom
                ? !curr_mb_frame_flag // bottom macroblock
                : (!curr_mb_frame_flag && !top_mb_frame_flag) // top macroblock
                ) {
            top_xy -= s->mb_stride;
        }
        if (bottom
                ? !curr_mb_frame_flag // bottom macroblock
                : (!curr_mb_frame_flag && !topleft_mb_frame_flag) // top macroblock
                ) {
            topleft_xy -= s->mb_stride;
        }
        if (bottom
                ? !curr_mb_frame_flag // bottom macroblock
                : (!curr_mb_frame_flag && !topright_mb_frame_flag) // top macroblock
                ) {
            topright_xy -= s->mb_stride;
        }
        if (left_mb_frame_flag != curr_mb_frame_flag) {
            left_xy[1] = left_xy[0] = pair_xy - 1;
            if (curr_mb_frame_flag) {
                if (bottom) {
                    left_block[0]= 2;
                    left_block[1]= 2;
                    left_block[2]= 3;
                    left_block[3]= 3;
                    left_block[4]= 8;
                    left_block[5]= 11;
                    left_block[6]= 8;
                    left_block[7]= 11;
                } else {
                    left_block[0]= 0;
                    left_block[1]= 0;
                    left_block[2]= 1;
                    left_block[3]= 1;
                    left_block[4]= 7;
                    left_block[5]= 10;
                    left_block[6]= 7;
                    left_block[7]= 10;
                }
            } else {
                left_xy[1] += s->mb_stride;
                //left_block[0]= 0;
                left_block[1]= 2;
                left_block[2]= 0;
                left_block[3]= 2;
                //left_block[4]= 7;
                left_block[5]= 10;
                left_block[6]= 7;
                left_block[7]= 10;
            }
        }
    }

    h->top_mb_xy = top_xy;
    h->left_mb_xy[0] = left_xy[0];
    h->left_mb_xy[1] = left_xy[1];
    if(for_deblock){
        topleft_type = 0;
        topright_type = 0;
        top_type     = h->slice_table[top_xy     ] < 255 ? s->current_picture.mb_type[top_xy]     : 0;
        left_type[0] = h->slice_table[left_xy[0] ] < 255 ? s->current_picture.mb_type[left_xy[0]] : 0;
        left_type[1] = h->slice_table[left_xy[1] ] < 255 ? s->current_picture.mb_type[left_xy[1]] : 0;

        if(FRAME_MBAFF && !IS_INTRA(mb_type)){
            int list;
            int v = *(uint16_t*)&h->non_zero_count[mb_xy][14];
            for(i=0; i<16; i++)
                h->non_zero_count_cache[scan8[i]] = (v>>i)&1;
            for(list=0; list<h->list_count; list++){
                if(USES_LIST(mb_type,list)){
                    uint32_t *src = (uint32_t*)s->current_picture.motion_val[list][h->mb2b_xy[mb_xy]];
                    uint32_t *dst = (uint32_t*)h->mv_cache[list][scan8[0]];
                    int8_t *ref = &s->current_picture.ref_index[list][h->mb2b8_xy[mb_xy]];
                    for(i=0; i<4; i++, dst+=8, src+=h->b_stride){
                        dst[0] = src[0];
                        dst[1] = src[1];
                        dst[2] = src[2];
                        dst[3] = src[3];
                    }
                    *(uint32_t*)&h->ref_cache[list][scan8[ 0]] =
                    *(uint32_t*)&h->ref_cache[list][scan8[ 2]] = pack16to32(ref[0],ref[1])*0x0101;
                    ref += h->b8_stride;
                    *(uint32_t*)&h->ref_cache[list][scan8[ 8]] =
                    *(uint32_t*)&h->ref_cache[list][scan8[10]] = pack16to32(ref[0],ref[1])*0x0101;
                }else{
                    fill_rectangle(&h-> mv_cache[list][scan8[ 0]], 4, 4, 8, 0, 4);
                    fill_rectangle(&h->ref_cache[list][scan8[ 0]], 4, 4, 8, (uint8_t)LIST_NOT_USED, 1);
                }
            }
        }
    }else{
        topleft_type = h->slice_table[topleft_xy ] == h->slice_num ? s->current_picture.mb_type[topleft_xy] : 0;
        top_type     = h->slice_table[top_xy     ] == h->slice_num ? s->current_picture.mb_type[top_xy]     : 0;
        topright_type= h->slice_table[topright_xy] == h->slice_num ? s->current_picture.mb_type[topright_xy]: 0;
        left_type[0] = h->slice_table[left_xy[0] ] == h->slice_num ? s->current_picture.mb_type[left_xy[0]] : 0;
        left_type[1] = h->slice_table[left_xy[1] ] == h->slice_num ? s->current_picture.mb_type[left_xy[1]] : 0;
    }

    if(IS_INTRA(mb_type)){
        h->topleft_samples_available=
        h->top_samples_available=
        h->left_samples_available= 0xFFFF;
        h->topright_samples_available= 0xEEEA;

        if(!IS_INTRA(top_type) && (top_type==0 || h->pps.constrained_intra_pred)){
            h->topleft_samples_available= 0xB3FF;
            h->top_samples_available= 0x33FF;
            h->topright_samples_available= 0x26EA;
        }
        for(i=0; i<2; i++){
            if(!IS_INTRA(left_type[i]) && (left_type[i]==0 || h->pps.constrained_intra_pred)){
                h->topleft_samples_available&= 0xDF5F;
                h->left_samples_available&= 0x5F5F;
            }
        }

        if(!IS_INTRA(topleft_type) && (topleft_type==0 || h->pps.constrained_intra_pred))
            h->topleft_samples_available&= 0x7FFF;

        if(!IS_INTRA(topright_type) && (topright_type==0 || h->pps.constrained_intra_pred))
            h->topright_samples_available&= 0xFBFF;

        if(IS_INTRA4x4(mb_type)){
            if(IS_INTRA4x4(top_type)){
                h->intra4x4_pred_mode_cache[4+8*0]= h->intra4x4_pred_mode[top_xy][4];
                h->intra4x4_pred_mode_cache[5+8*0]= h->intra4x4_pred_mode[top_xy][5];
                h->intra4x4_pred_mode_cache[6+8*0]= h->intra4x4_pred_mode[top_xy][6];
                h->intra4x4_pred_mode_cache[7+8*0]= h->intra4x4_pred_mode[top_xy][3];
            }else{
                int pred;
                if(!top_type || (IS_INTER(top_type) && h->pps.constrained_intra_pred))
                    pred= -1;
                else{
                    pred= 2;
                }
                h->intra4x4_pred_mode_cache[4+8*0]=
                h->intra4x4_pred_mode_cache[5+8*0]=
                h->intra4x4_pred_mode_cache[6+8*0]=
                h->intra4x4_pred_mode_cache[7+8*0]= pred;
            }
            for(i=0; i<2; i++){
                if(IS_INTRA4x4(left_type[i])){
                    h->intra4x4_pred_mode_cache[3+8*1 + 2*8*i]= h->intra4x4_pred_mode[left_xy[i]][left_block[0+2*i]];
                    h->intra4x4_pred_mode_cache[3+8*2 + 2*8*i]= h->intra4x4_pred_mode[left_xy[i]][left_block[1+2*i]];
                }else{
                    int pred;
                    if(!left_type[i] || (IS_INTER(left_type[i]) && h->pps.constrained_intra_pred))
                        pred= -1;
                    else{
                        pred= 2;
                    }
                    h->intra4x4_pred_mode_cache[3+8*1 + 2*8*i]=
                    h->intra4x4_pred_mode_cache[3+8*2 + 2*8*i]= pred;
                }
            }
        }
    }


/*
0 . T T. T T T T
1 L . .L . . . .
2 L . .L . . . .
3 . T TL . . . .
4 L . .L . . . .
5 L . .. . . . .
*/
//FIXME constraint_intra_pred & partitioning & nnz (lets hope this is just a typo in the spec)
    if(top_type){
        h->non_zero_count_cache[4+8*0]= h->non_zero_count[top_xy][4];
        h->non_zero_count_cache[5+8*0]= h->non_zero_count[top_xy][5];
        h->non_zero_count_cache[6+8*0]= h->non_zero_count[top_xy][6];
        h->non_zero_count_cache[7+8*0]= h->non_zero_count[top_xy][3];

        h->non_zero_count_cache[1+8*0]= h->non_zero_count[top_xy][9];
        h->non_zero_count_cache[2+8*0]= h->non_zero_count[top_xy][8];

        h->non_zero_count_cache[1+8*3]= h->non_zero_count[top_xy][12];
        h->non_zero_count_cache[2+8*3]= h->non_zero_count[top_xy][11];

    }else{
        h->non_zero_count_cache[4+8*0]=
        h->non_zero_count_cache[5+8*0]=
        h->non_zero_count_cache[6+8*0]=
        h->non_zero_count_cache[7+8*0]=

        h->non_zero_count_cache[1+8*0]=
        h->non_zero_count_cache[2+8*0]=

        h->non_zero_count_cache[1+8*3]=
        h->non_zero_count_cache[2+8*3]= h->pps.cabac && !IS_INTRA(mb_type) ? 0 : 64;

    }

    for (i=0; i<2; i++) {
        if(left_type[i]){
            h->non_zero_count_cache[3+8*1 + 2*8*i]= h->non_zero_count[left_xy[i]][left_block[0+2*i]];
            h->non_zero_count_cache[3+8*2 + 2*8*i]= h->non_zero_count[left_xy[i]][left_block[1+2*i]];
            h->non_zero_count_cache[0+8*1 +   8*i]= h->non_zero_count[left_xy[i]][left_block[4+2*i]];
            h->non_zero_count_cache[0+8*4 +   8*i]= h->non_zero_count[left_xy[i]][left_block[5+2*i]];
        }else{
            h->non_zero_count_cache[3+8*1 + 2*8*i]=
            h->non_zero_count_cache[3+8*2 + 2*8*i]=
            h->non_zero_count_cache[0+8*1 +   8*i]=
            h->non_zero_count_cache[0+8*4 +   8*i]= h->pps.cabac && !IS_INTRA(mb_type) ? 0 : 64;
        }
    }

    if( h->pps.cabac ) {
        // top_cbp
        if(top_type) {
            h->top_cbp = h->cbp_table[top_xy];
        } else if(IS_INTRA(mb_type)) {
            h->top_cbp = 0x1C0;
        } else {
            h->top_cbp = 0;
        }
        // left_cbp
        if (left_type[0]) {
            h->left_cbp = h->cbp_table[left_xy[0]] & 0x1f0;
        } else if(IS_INTRA(mb_type)) {
            h->left_cbp = 0x1C0;
        } else {
            h->left_cbp = 0;
        }
        if (left_type[0]) {
            h->left_cbp |= ((h->cbp_table[left_xy[0]]>>((left_block[0]&(~1))+1))&0x1) << 1;
        }
        if (left_type[1]) {
            h->left_cbp |= ((h->cbp_table[left_xy[1]]>>((left_block[2]&(~1))+1))&0x1) << 3;
        }
    }

#if 1
    if(IS_INTER(mb_type) || IS_DIRECT(mb_type)){
        int list;
        for(list=0; list<h->list_count; list++){
            if(!USES_LIST(mb_type, list) && !IS_DIRECT(mb_type) && !h->deblocking_filter){
                /*if(!h->mv_cache_clean[list]){
                    memset(h->mv_cache [list],  0, 8*5*2*sizeof(int16_t)); //FIXME clean only input? clean at all?
                    memset(h->ref_cache[list], PART_NOT_AVAILABLE, 8*5*sizeof(int8_t));
                    h->mv_cache_clean[list]= 1;
                }*/
                continue;
            }
            h->mv_cache_clean[list]= 0;

            if(USES_LIST(top_type, list)){
                const int b_xy= h->mb2b_xy[top_xy] + 3*h->b_stride;
                const int b8_xy= h->mb2b8_xy[top_xy] + h->b8_stride;
                *(uint32_t*)h->mv_cache[list][scan8[0] + 0 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + 0];
                *(uint32_t*)h->mv_cache[list][scan8[0] + 1 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + 1];
                *(uint32_t*)h->mv_cache[list][scan8[0] + 2 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + 2];
                *(uint32_t*)h->mv_cache[list][scan8[0] + 3 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + 3];
                h->ref_cache[list][scan8[0] + 0 - 1*8]=
                h->ref_cache[list][scan8[0] + 1 - 1*8]= s->current_picture.ref_index[list][b8_xy + 0];
                h->ref_cache[list][scan8[0] + 2 - 1*8]=
                h->ref_cache[list][scan8[0] + 3 - 1*8]= s->current_picture.ref_index[list][b8_xy + 1];
            }else{
                *(uint32_t*)h->mv_cache [list][scan8[0] + 0 - 1*8]=
                *(uint32_t*)h->mv_cache [list][scan8[0] + 1 - 1*8]=
                *(uint32_t*)h->mv_cache [list][scan8[0] + 2 - 1*8]=
                *(uint32_t*)h->mv_cache [list][scan8[0] + 3 - 1*8]= 0;
                *(uint32_t*)&h->ref_cache[list][scan8[0] + 0 - 1*8]= ((top_type ? LIST_NOT_USED : PART_NOT_AVAILABLE)&0xFF)*0x01010101;
            }

            for(i=0; i<2; i++){
                int cache_idx = scan8[0] - 1 + i*2*8;
                if(USES_LIST(left_type[i], list)){
                    const int b_xy= h->mb2b_xy[left_xy[i]] + 3;
                    const int b8_xy= h->mb2b8_xy[left_xy[i]] + 1;
                    *(uint32_t*)h->mv_cache[list][cache_idx  ]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + h->b_stride*left_block[0+i*2]];
                    *(uint32_t*)h->mv_cache[list][cache_idx+8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy + h->b_stride*left_block[1+i*2]];
                    h->ref_cache[list][cache_idx  ]= s->current_picture.ref_index[list][b8_xy + h->b8_stride*(left_block[0+i*2]>>1)];
                    h->ref_cache[list][cache_idx+8]= s->current_picture.ref_index[list][b8_xy + h->b8_stride*(left_block[1+i*2]>>1)];
                }else{
                    *(uint32_t*)h->mv_cache [list][cache_idx  ]=
                    *(uint32_t*)h->mv_cache [list][cache_idx+8]= 0;
                    h->ref_cache[list][cache_idx  ]=
                    h->ref_cache[list][cache_idx+8]= left_type[i] ? LIST_NOT_USED : PART_NOT_AVAILABLE;
                }
            }

            if((for_deblock || (IS_DIRECT(mb_type) && !h->direct_spatial_mv_pred)) && !FRAME_MBAFF)
                continue;

            if(USES_LIST(topleft_type, list)){
                const int b_xy = h->mb2b_xy[topleft_xy] + 3 + 3*h->b_stride;
                const int b8_xy= h->mb2b8_xy[topleft_xy] + 1 + h->b8_stride;
                *(uint32_t*)h->mv_cache[list][scan8[0] - 1 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy];
                h->ref_cache[list][scan8[0] - 1 - 1*8]= s->current_picture.ref_index[list][b8_xy];
            }else{
                *(uint32_t*)h->mv_cache[list][scan8[0] - 1 - 1*8]= 0;
                h->ref_cache[list][scan8[0] - 1 - 1*8]= topleft_type ? LIST_NOT_USED : PART_NOT_AVAILABLE;
            }

            if(USES_LIST(topright_type, list)){
                const int b_xy= h->mb2b_xy[topright_xy] + 3*h->b_stride;
                const int b8_xy= h->mb2b8_xy[topright_xy] + h->b8_stride;
                *(uint32_t*)h->mv_cache[list][scan8[0] + 4 - 1*8]= *(uint32_t*)s->current_picture.motion_val[list][b_xy];
                h->ref_cache[list][scan8[0] + 4 - 1*8]= s->current_picture.ref_index[list][b8_xy];
            }else{
                *(uint32_t*)h->mv_cache [list][scan8[0] + 4 - 1*8]= 0;
                h->ref_cache[list][scan8[0] + 4 - 1*8]= topright_type ? LIST_NOT_USED : PART_NOT_AVAILABLE;
            }

            if((IS_SKIP(mb_type) || IS_DIRECT(mb_type)) && !FRAME_MBAFF)
                continue;

            h->ref_cache[list][scan8[5 ]+1] =
            h->ref_cache[list][scan8[7 ]+1] =
            h->ref_cache[list][scan8[13]+1] =  //FIXME remove past 3 (init somewhere else)
            h->ref_cache[list][scan8[4 ]] =
            h->ref_cache[list][scan8[12]] = PART_NOT_AVAILABLE;
            *(uint32_t*)h->mv_cache [list][scan8[5 ]+1]=
            *(uint32_t*)h->mv_cache [list][scan8[7 ]+1]=
            *(uint32_t*)h->mv_cache [list][scan8[13]+1]= //FIXME remove past 3 (init somewhere else)
            *(uint32_t*)h->mv_cache [list][scan8[4 ]]=
            *(uint32_t*)h->mv_cache [list][scan8[12]]= 0;

            if( h->pps.cabac ) {
                /* XXX beurk, Load mvd */
                if(USES_LIST(top_type, list)){
                    const int b_xy= h->mb2b_xy[top_xy] + 3*h->b_stride;
                    *(uint32_t*)h->mvd_cache[list][scan8[0] + 0 - 1*8]= *(uint32_t*)h->mvd_table[list][b_xy + 0];
                    *(uint32_t*)h->mvd_cache[list][scan8[0] + 1 - 1*8]= *(uint32_t*)h->mvd_table[list][b_xy + 1];
                    *(uint32_t*)h->mvd_cache[list][scan8[0] + 2 - 1*8]= *(uint32_t*)h->mvd_table[list][b_xy + 2];
                    *(uint32_t*)h->mvd_cache[list][scan8[0] + 3 - 1*8]= *(uint32_t*)h->mvd_table[list][b_xy + 3];
                }else{
                    *(uint32_t*)h->mvd_cache [list][scan8[0] + 0 - 1*8]=
                    *(uint32_t*)h->mvd_cache [list][scan8[0] + 1 - 1*8]=
                    *(uint32_t*)h->mvd_cache [list][scan8[0] + 2 - 1*8]=
                    *(uint32_t*)h->mvd_cache [list][scan8[0] + 3 - 1*8]= 0;
                }
                if(USES_LIST(left_type[0], list)){
                    const int b_xy= h->mb2b_xy[left_xy[0]] + 3;
                    *(uint32_t*)h->mvd_cache[list][scan8[0] - 1 + 0*8]= *(uint32_t*)h->mvd_table[list][b_xy + h->b_stride*left_block[0]];
                    *(uint32_t*)h->mvd_cache[list][scan8[0] - 1 + 1*8]= *(uint32_t*)h->mvd_table[list][b_xy + h->b_stride*left_block[1]];
                }else{
                    *(uint32_t*)h->mvd_cache [list][scan8[0] - 1 + 0*8]=
                    *(uint32_t*)h->mvd_cache [list][scan8[0] - 1 + 1*8]= 0;
                }
                if(USES_LIST(left_type[1], list)){
                    const int b_xy= h->mb2b_xy[left_xy[1]] + 3;
                    *(uint32_t*)h->mvd_cache[list][scan8[0] - 1 + 2*8]= *(uint32_t*)h->mvd_table[list][b_xy + h->b_stride*left_block[2]];
                    *(uint32_t*)h->mvd_cache[list][scan8[0] - 1 + 3*8]= *(uint32_t*)h->mvd_table[list][b_xy + h->b_stride*left_block[3]];
                }else{
                    *(uint32_t*)h->mvd_cache [list][scan8[0] - 1 + 2*8]=
                    *(uint32_t*)h->mvd_cache [list][scan8[0] - 1 + 3*8]= 0;
                }
                *(uint32_t*)h->mvd_cache [list][scan8[5 ]+1]=
                *(uint32_t*)h->mvd_cache [list][scan8[7 ]+1]=
                *(uint32_t*)h->mvd_cache [list][scan8[13]+1]= //FIXME remove past 3 (init somewhere else)
                *(uint32_t*)h->mvd_cache [list][scan8[4 ]]=
                *(uint32_t*)h->mvd_cache [list][scan8[12]]= 0;

                if(h->slice_type == B_TYPE){
                    fill_rectangle(&h->direct_cache[scan8[0]], 4, 4, 8, 0, 1);

                    if(IS_DIRECT(top_type)){
                        *(uint32_t*)&h->direct_cache[scan8[0] - 1*8]= 0x01010101;
                    }else if(IS_8X8(top_type)){
                        int b8_xy = h->mb2b8_xy[top_xy] + h->b8_stride;
                        h->direct_cache[scan8[0] + 0 - 1*8]= h->direct_table[b8_xy];
                        h->direct_cache[scan8[0] + 2 - 1*8]= h->direct_table[b8_xy + 1];
                    }else{
                        *(uint32_t*)&h->direct_cache[scan8[0] - 1*8]= 0;
                    }

                    if(IS_DIRECT(left_type[0]))
                        h->direct_cache[scan8[0] - 1 + 0*8]= 1;
                    else if(IS_8X8(left_type[0]))
                        h->direct_cache[scan8[0] - 1 + 0*8]= h->direct_table[h->mb2b8_xy[left_xy[0]] + 1 + h->b8_stride*(left_block[0]>>1)];
                    else
                        h->direct_cache[scan8[0] - 1 + 0*8]= 0;

                    if(IS_DIRECT(left_type[1]))
                        h->direct_cache[scan8[0] - 1 + 2*8]= 1;
                    else if(IS_8X8(left_type[1]))
                        h->direct_cache[scan8[0] - 1 + 2*8]= h->direct_table[h->mb2b8_xy[left_xy[1]] + 1 + h->b8_stride*(left_block[2]>>1)];
                    else
                        h->direct_cache[scan8[0] - 1 + 2*8]= 0;
                }
            }

            if(FRAME_MBAFF){
#define MAP_MVS\
                    MAP_F2F(scan8[0] - 1 - 1*8, topleft_type)\
                    MAP_F2F(scan8[0] + 0 - 1*8, top_type)\
                    MAP_F2F(scan8[0] + 1 - 1*8, top_type)\
                    MAP_F2F(scan8[0] + 2 - 1*8, top_type)\
                    MAP_F2F(scan8[0] + 3 - 1*8, top_type)\
                    MAP_F2F(scan8[0] + 4 - 1*8, topright_type)\
                    MAP_F2F(scan8[0] - 1 + 0*8, left_type[0])\
                    MAP_F2F(scan8[0] - 1 + 1*8, left_type[0])\
                    MAP_F2F(scan8[0] - 1 + 2*8, left_type[1])\
                    MAP_F2F(scan8[0] - 1 + 3*8, left_type[1])
                if(MB_FIELD){
#define MAP_F2F(idx, mb_type)\
                    if(!IS_INTERLACED(mb_type) && h->ref_cache[list][idx] >= 0){\
                        h->ref_cache[list][idx] <<= 1;\
                        h->mv_cache[list][idx][1] /= 2;\
                        h->mvd_cache[list][idx][1] /= 2;\
                    }
                    MAP_MVS
#undef MAP_F2F
                }else{
#define MAP_F2F(idx, mb_type)\
                    if(IS_INTERLACED(mb_type) && h->ref_cache[list][idx] >= 0){\
                        h->ref_cache[list][idx] >>= 1;\
                        h->mv_cache[list][idx][1] <<= 1;\
                        h->mvd_cache[list][idx][1] <<= 1;\
                    }
                    MAP_MVS
#undef MAP_F2F
                }
            }
        }
    }
#endif

    h->neighbor_transform_size= !!IS_8x8DCT(top_type) + !!IS_8x8DCT(left_type[0]);
}

static inline void write_back_intra_pred_mode(H264Context *h){
    MpegEncContext * const s = &h->s;
    const int mb_xy= s->mb_x + s->mb_y*s->mb_stride;

    h->intra4x4_pred_mode[mb_xy][0]= h->intra4x4_pred_mode_cache[7+8*1];
    h->intra4x4_pred_mode[mb_xy][1]= h->intra4x4_pred_mode_cache[7+8*2];
    h->intra4x4_pred_mode[mb_xy][2]= h->intra4x4_pred_mode_cache[7+8*3];
    h->intra4x4_pred_mode[mb_xy][3]= h->intra4x4_pred_mode_cache[7+8*4];
    h->intra4x4_pred_mode[mb_xy][4]= h->intra4x4_pred_mode_cache[4+8*4];
    h->intra4x4_pred_mode[mb_xy][5]= h->intra4x4_pred_mode_cache[5+8*4];
    h->intra4x4_pred_mode[mb_xy][6]= h->intra4x4_pred_mode_cache[6+8*4];
}

/**
 * checks if the top & left blocks are available if needed & changes the dc mode so it only uses the available blocks.
 */
static inline int check_intra4x4_pred_mode(H264Context *h){
    MpegEncContext * const s = &h->s;
    static const int8_t top [12]= {-1, 0,LEFT_DC_PRED,-1,-1,-1,-1,-1, 0};
    static const int8_t left[12]= { 0,-1, TOP_DC_PRED, 0,-1,-1,-1, 0,-1,DC_128_PRED};
    int i;

    if(!(h->top_samples_available&0x8000)){
        for(i=0; i<4; i++){
            int status= top[ h->intra4x4_pred_mode_cache[scan8[0] + i] ];
            if(status<0){
                av_log(h->s.avctx, AV_LOG_ERROR, "top block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                return -1;
            } else if(status){
                h->intra4x4_pred_mode_cache[scan8[0] + i]= status;
            }
        }
    }

    if(!(h->left_samples_available&0x8000)){
        for(i=0; i<4; i++){
            int status= left[ h->intra4x4_pred_mode_cache[scan8[0] + 8*i] ];
            if(status<0){
                av_log(h->s.avctx, AV_LOG_ERROR, "left block unavailable for requested intra4x4 mode %d at %d %d\n", status, s->mb_x, s->mb_y);
                return -1;
            } else if(status){
                h->intra4x4_pred_mode_cache[scan8[0] + 8*i]= status;
            }
        }
    }

    return 0;
} //FIXME cleanup like next

/**
 * checks if the top & left blocks are available if needed & changes the dc mode so it only uses the available blocks.
 */
static inline int check_intra_pred_mode(H264Context *h, int mode){
    MpegEncContext * const s = &h->s;
    static const int8_t top [7]= {LEFT_DC_PRED8x8, 1,-1,-1};
    static const int8_t left[7]= { TOP_DC_PRED8x8,-1, 2,-1,DC_128_PRED8x8};

    if(mode > 6U) {
        av_log(h->s.avctx, AV_LOG_ERROR, "out of range intra chroma pred mode at %d %d\n", s->mb_x, s->mb_y);
        return -1;
    }

    if(!(h->top_samples_available&0x8000)){
        mode= top[ mode ];
        if(mode<0){
            av_log(h->s.avctx, AV_LOG_ERROR, "top block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    if(!(h->left_samples_available&0x8000)){
        mode= left[ mode ];
        if(mode<0){
            av_log(h->s.avctx, AV_LOG_ERROR, "left block unavailable for requested intra mode at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
    }

    return mode;
}

static inline int fetch_diagonal_mv(H264Context *h, const int16_t **C, int i, int list, int part_width){
    const int topright_ref= h->ref_cache[list][ i - 8 + part_width ];
    MpegEncContext *s = &h->s;

    /* there is no consistent mapping of mvs to neighboring locations that will
     * make mbaff happy, so we can't move all this logic to fill_caches */
    if(FRAME_MBAFF){
        const uint32_t *mb_types = s->current_picture_ptr->mb_type;
        const int16_t *mv;
        *(uint32_t*)h->mv_cache[list][scan8[0]-2] = 0;
        *C = h->mv_cache[list][scan8[0]-2];

        if(!MB_FIELD
           && (s->mb_y&1) && i < scan8[0]+8 && topright_ref != PART_NOT_AVAILABLE){
            int topright_xy = s->mb_x + (s->mb_y-1)*s->mb_stride + (i == scan8[0]+3);
            if(IS_INTERLACED(mb_types[topright_xy])){
#define SET_DIAG_MV(MV_OP, REF_OP, X4, Y4)\
                const int x4 = X4, y4 = Y4;\
                const int mb_type = mb_types[(x4>>2)+(y4>>2)*s->mb_stride];\
                if(!USES_LIST(mb_type,list) && !IS_8X8(mb_type))\
                    return LIST_NOT_USED;\
                mv = s->current_picture_ptr->motion_val[list][x4 + y4*h->b_stride];\
                h->mv_cache[list][scan8[0]-2][0] = mv[0];\
                h->mv_cache[list][scan8[0]-2][1] = mv[1] MV_OP;\
                return s->current_picture_ptr->ref_index[list][(x4>>1) + (y4>>1)*h->b8_stride] REF_OP;

                SET_DIAG_MV(*2, >>1, s->mb_x*4+(i&7)-4+part_width, s->mb_y*4-1);
            }
        }
        if(topright_ref == PART_NOT_AVAILABLE
           && ((s->mb_y&1) || i >= scan8[0]+8) && (i&7)==4
           && h->ref_cache[list][scan8[0]-1] != PART_NOT_AVAILABLE){
            if(!MB_FIELD
               && IS_INTERLACED(mb_types[h->left_mb_xy[0]])){
                SET_DIAG_MV(*2, >>1, s->mb_x*4-1, (s->mb_y|1)*4+(s->mb_y&1)*2+(i>>4)-1);
            }
            if(MB_FIELD
               && !IS_INTERLACED(mb_types[h->left_mb_xy[0]])
               && i >= scan8[0]+8){
                // leftshift will turn LIST_NOT_USED into PART_NOT_AVAILABLE, but that's ok.
                SET_DIAG_MV(>>1, <<1, s->mb_x*4-1, (s->mb_y&~1)*4 - 1 + ((i-scan8[0])>>3)*2);
            }
        }
#undef SET_DIAG_MV
    }

    if(topright_ref != PART_NOT_AVAILABLE){
        *C= h->mv_cache[list][ i - 8 + part_width ];
        return topright_ref;
    }else{
        tprintf(s->avctx, "topright MV not available\n");

        *C= h->mv_cache[list][ i - 8 - 1 ];
        return h->ref_cache[list][ i - 8 - 1 ];
    }
}

/**
 * gets the predicted MV.
 * @param n the block index
 * @param part_width the width of the partition (4, 8,16) -> (1, 2, 4)
 * @param mx the x component of the predicted motion vector
 * @param my the y component of the predicted motion vector
 */
static inline void pred_motion(H264Context * const h, int n, int part_width, int list, int ref, int * const mx, int * const my){
    const int index8= scan8[n];
    const int top_ref=      h->ref_cache[list][ index8 - 8 ];
    const int left_ref=     h->ref_cache[list][ index8 - 1 ];
    const int16_t * const A= h->mv_cache[list][ index8 - 1 ];
    const int16_t * const B= h->mv_cache[list][ index8 - 8 ];
    const int16_t * C;
    int diagonal_ref, match_count;

    assert(part_width==1 || part_width==2 || part_width==4);

/* mv_cache
  B . . A T T T T
  U . . L . . , .
  U . . L . . . .
  U . . L . . , .
  . . . L . . . .
*/

    diagonal_ref= fetch_diagonal_mv(h, &C, index8, list, part_width);
    match_count= (diagonal_ref==ref) + (top_ref==ref) + (left_ref==ref);
    tprintf(h->s.avctx, "pred_motion match_count=%d\n", match_count);
    if(match_count > 1){ //most common
        *mx= mid_pred(A[0], B[0], C[0]);
        *my= mid_pred(A[1], B[1], C[1]);
    }else if(match_count==1){
        if(left_ref==ref){
            *mx= A[0];
            *my= A[1];
        }else if(top_ref==ref){
            *mx= B[0];
            *my= B[1];
        }else{
            *mx= C[0];
            *my= C[1];
        }
    }else{
        if(top_ref == PART_NOT_AVAILABLE && diagonal_ref == PART_NOT_AVAILABLE && left_ref != PART_NOT_AVAILABLE){
            *mx= A[0];
            *my= A[1];
        }else{
            *mx= mid_pred(A[0], B[0], C[0]);
            *my= mid_pred(A[1], B[1], C[1]);
        }
    }

    tprintf(h->s.avctx, "pred_motion (%2d %2d %2d) (%2d %2d %2d) (%2d %2d %2d) -> (%2d %2d %2d) at %2d %2d %d list %d\n", top_ref, B[0], B[1],                    diagonal_ref, C[0], C[1], left_ref, A[0], A[1], ref, *mx, *my, h->s.mb_x, h->s.mb_y, n, list);
}

/**
 * idct tranforms the 16 dc values and dequantize them.
 * @param qp quantization parameter
 */
static void h264_luma_dc_dequant_idct_c(DCTELEM *block, int qp, int qmul){
#define stride 16
    int i;
    int temp[16]; //FIXME check if this is a good idea
    static const int x_offset[4]={0, 1*stride, 4* stride,  5*stride};
    static const int y_offset[4]={0, 2*stride, 8* stride, 10*stride};

//memset(block, 64, 2*256);
//return;
    for(i=0; i<4; i++){
        const int offset= y_offset[i];
        const int z0= block[offset+stride*0] + block[offset+stride*4];
        const int z1= block[offset+stride*0] - block[offset+stride*4];
        const int z2= block[offset+stride*1] - block[offset+stride*5];
        const int z3= block[offset+stride*1] + block[offset+stride*5];

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z1+z2;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z0-z3;
    }

    for(i=0; i<4; i++){
        const int offset= x_offset[i];
        const int z0= temp[4*0+i] + temp[4*2+i];
        const int z1= temp[4*0+i] - temp[4*2+i];
        const int z2= temp[4*1+i] - temp[4*3+i];
        const int z3= temp[4*1+i] + temp[4*3+i];

        block[stride*0 +offset]= ((((z0 + z3)*qmul + 128 ) >> 8)); //FIXME think about merging this into decode_resdual
        block[stride*2 +offset]= ((((z1 + z2)*qmul + 128 ) >> 8));
        block[stride*8 +offset]= ((((z1 - z2)*qmul + 128 ) >> 8));
        block[stride*10+offset]= ((((z0 - z3)*qmul + 128 ) >> 8));
    }
}

#undef xStride
#undef stride

static void chroma_dc_dequant_idct_c(DCTELEM *block, int qp, int qmul){
    const int stride= 16*2;
    const int xStride= 16;
    int a,b,c,d,e;

    a= block[stride*0 + xStride*0];
    b= block[stride*0 + xStride*1];
    c= block[stride*1 + xStride*0];
    d= block[stride*1 + xStride*1];

    e= a-b;
    a= a+b;
    b= c-d;
    c= c+d;

    block[stride*0 + xStride*0]= ((a+c)*qmul) >> 7;
    block[stride*0 + xStride*1]= ((e+b)*qmul) >> 7;
    block[stride*1 + xStride*0]= ((a-c)*qmul) >> 7;
    block[stride*1 + xStride*1]= ((e-b)*qmul) >> 7;
}

/**
 * gets the chroma qp.
 */
static inline int get_chroma_qp(H264Context *h, int t, int qscale){
    return h->pps.chroma_qp_table[t][qscale & 0xff];
}

static void decode_init_vlc(void){
    static int done = 0;

    if (!done) {
        int i;
        done = 1;

        init_vlc(&chroma_dc_coeff_token_vlc, CHROMA_DC_COEFF_TOKEN_VLC_BITS, 4*5,
                 &chroma_dc_coeff_token_len [0], 1, 1,
                 &chroma_dc_coeff_token_bits[0], 1, 1, 1);

        for(i=0; i<4; i++){
            init_vlc(&coeff_token_vlc[i], COEFF_TOKEN_VLC_BITS, 4*17,
                     &coeff_token_len [i][0], 1, 1,
                     &coeff_token_bits[i][0], 1, 1, 1);
        }

        for(i=0; i<3; i++){
            init_vlc(&chroma_dc_total_zeros_vlc[i], CHROMA_DC_TOTAL_ZEROS_VLC_BITS, 4,
                     &chroma_dc_total_zeros_len [i][0], 1, 1,
                     &chroma_dc_total_zeros_bits[i][0], 1, 1, 1);
        }
        for(i=0; i<15; i++){
            init_vlc(&total_zeros_vlc[i], TOTAL_ZEROS_VLC_BITS, 16,
                     &total_zeros_len [i][0], 1, 1,
                     &total_zeros_bits[i][0], 1, 1, 1);
        }

        for(i=0; i<6; i++){
            init_vlc(&run_vlc[i], RUN_VLC_BITS, 7,
                     &run_len [i][0], 1, 1,
                     &run_bits[i][0], 1, 1, 1);
        }
        init_vlc(&run7_vlc, RUN7_VLC_BITS, 16,
                 &run_len [6][0], 1, 1,
                 &run_bits[6][0], 1, 1, 1);
    }
}

static void free_tables(H264Context *h){
    int i;
    H264Context *hx;
    av_freep(&h->intra4x4_pred_mode);
    av_freep(&h->chroma_pred_mode_table);
    av_freep(&h->cbp_table);
    av_freep(&h->mvd_table[0]);
    av_freep(&h->mvd_table[1]);
    av_freep(&h->direct_table);
    av_freep(&h->non_zero_count);
    av_freep(&h->slice_table_base);
    h->slice_table= NULL;

    av_freep(&h->mb2b_xy);
    av_freep(&h->mb2b8_xy);

    for(i = 0; i < MAX_SPS_COUNT; i++)
        av_freep(h->sps_buffers + i);

    for(i = 0; i < MAX_PPS_COUNT; i++)
        av_freep(h->pps_buffers + i);

    for(i = 0; i < h->s.avctx->thread_count; i++) {
        hx = h->thread_context[i];
        if(!hx) continue;
        av_freep(&hx->top_borders[1]);
        av_freep(&hx->top_borders[0]);
        av_freep(&hx->s.obmc_scratchpad);
        av_freep(&hx->s.allocated_edge_emu_buffer);
    }
}

static void init_dequant8_coeff_table(H264Context *h){
    int i,q,x;
    const int transpose = (h->s.dsp.h264_idct8_add != ff_h264_idct8_add_c); //FIXME ugly
    h->dequant8_coeff[0] = h->dequant8_buffer[0];
    h->dequant8_coeff[1] = h->dequant8_buffer[1];

    for(i=0; i<2; i++ ){
        if(i && !memcmp(h->pps.scaling_matrix8[0], h->pps.scaling_matrix8[1], 64*sizeof(uint8_t))){
            h->dequant8_coeff[1] = h->dequant8_buffer[0];
            break;
        }

        for(q=0; q<52; q++){
            int shift = ff_div6[q];
            int idx = ff_rem6[q];
            for(x=0; x<64; x++)
                h->dequant8_coeff[i][q][transpose ? (x>>3)|((x&7)<<3) : x] =
                    ((uint32_t)dequant8_coeff_init[idx][ dequant8_coeff_init_scan[((x>>1)&12) | (x&3)] ] *
                    h->pps.scaling_matrix8[i][x]) << shift;
        }
    }
}

static void init_dequant4_coeff_table(H264Context *h){
    int i,j,q,x;
    const int transpose = (h->s.dsp.h264_idct_add != ff_h264_idct_add_c); //FIXME ugly
    for(i=0; i<6; i++ ){
        h->dequant4_coeff[i] = h->dequant4_buffer[i];
        for(j=0; j<i; j++){
            if(!memcmp(h->pps.scaling_matrix4[j], h->pps.scaling_matrix4[i], 16*sizeof(uint8_t))){
                h->dequant4_coeff[i] = h->dequant4_buffer[j];
                break;
            }
        }
        if(j<i)
            continue;

        for(q=0; q<52; q++){
            int shift = ff_div6[q] + 2;
            int idx = ff_rem6[q];
            for(x=0; x<16; x++)
                h->dequant4_coeff[i][q][transpose ? (x>>2)|((x<<2)&0xF) : x] =
                    ((uint32_t)dequant4_coeff_init[idx][(x&1) + ((x>>2)&1)] *
                    h->pps.scaling_matrix4[i][x]) << shift;
        }
    }
}

static void init_dequant_tables(H264Context *h){
    int i,x;
    init_dequant4_coeff_table(h);
    if(h->pps.transform_8x8_mode)
        init_dequant8_coeff_table(h);
    if(h->sps.transform_bypass){
        for(i=0; i<6; i++)
            for(x=0; x<16; x++)
                h->dequant4_coeff[i][0][x] = 1<<6;
        if(h->pps.transform_8x8_mode)
            for(i=0; i<2; i++)
                for(x=0; x<64; x++)
                    h->dequant8_coeff[i][0][x] = 1<<6;
    }
}


/**
 * allocates tables.
 * needs width/height
 */
static int alloc_tables(H264Context *h){
    MpegEncContext * const s = &h->s;
    const int big_mb_num= s->mb_stride * (s->mb_height+1);
    int x,y;

    CHECKED_ALLOCZ(h->intra4x4_pred_mode, big_mb_num * 8  * sizeof(uint8_t))

    CHECKED_ALLOCZ(h->non_zero_count    , big_mb_num * 16 * sizeof(uint8_t))
    CHECKED_ALLOCZ(h->slice_table_base  , (big_mb_num+s->mb_stride) * sizeof(uint8_t))
    CHECKED_ALLOCZ(h->cbp_table, big_mb_num * sizeof(uint16_t))

    if( h->pps.cabac ) {
        CHECKED_ALLOCZ(h->chroma_pred_mode_table, big_mb_num * sizeof(uint8_t))
        CHECKED_ALLOCZ(h->mvd_table[0], 32*big_mb_num * sizeof(uint16_t));
        CHECKED_ALLOCZ(h->mvd_table[1], 32*big_mb_num * sizeof(uint16_t));
        CHECKED_ALLOCZ(h->direct_table, 32*big_mb_num * sizeof(uint8_t));
    }

    memset(h->slice_table_base, -1, (big_mb_num+s->mb_stride)  * sizeof(uint8_t));
    h->slice_table= h->slice_table_base + s->mb_stride*2 + 1;

    CHECKED_ALLOCZ(h->mb2b_xy  , big_mb_num * sizeof(uint32_t));
    CHECKED_ALLOCZ(h->mb2b8_xy , big_mb_num * sizeof(uint32_t));
    for(y=0; y<s->mb_height; y++){
        for(x=0; x<s->mb_width; x++){
            const int mb_xy= x + y*s->mb_stride;
            const int b_xy = 4*x + 4*y*h->b_stride;
            const int b8_xy= 2*x + 2*y*h->b8_stride;

            h->mb2b_xy [mb_xy]= b_xy;
            h->mb2b8_xy[mb_xy]= b8_xy;
        }
    }

    s->obmc_scratchpad = NULL;

    if(!h->dequant4_coeff[0])
        init_dequant_tables(h);

    return 0;
fail:
    free_tables(h);
    return -1;
}

/**
 * Init context
 * Allocate buffers which are not shared amongst multiple threads.
 */
static int context_init(H264Context *h){
    MpegEncContext * const s = &h->s;

    CHECKED_ALLOCZ(h->top_borders[0], h->s.mb_width * (16+8+8) * sizeof(uint8_t))
    CHECKED_ALLOCZ(h->top_borders[1], h->s.mb_width * (16+8+8) * sizeof(uint8_t))

    // edge emu needs blocksize + filter length - 1 (=17x17 for halfpel / 21x21 for h264)
    CHECKED_ALLOCZ(s->allocated_edge_emu_buffer,
                   (s->width+64)*2*21*2); //(width + edge + align)*interlaced*MBsize*tolerance
    s->edge_emu_buffer= s->allocated_edge_emu_buffer + (s->width+64)*2*21;
    return 0;
fail:
    return -1; // free_tables will clean up for us
}

static void common_init(H264Context *h){
    MpegEncContext * const s = &h->s;

    s->width = s->avctx->width;
    s->height = s->avctx->height;
    s->codec_id= s->avctx->codec->id;

    mtv_ff_h264_pred_init(&h->hpc, s->codec_id);

    h->dequant_coeff_pps= -1;
    s->unrestricted_mv=1;
    s->decode=1; //FIXME

    memset(h->pps.scaling_matrix4, 16, 6*16*sizeof(uint8_t));
    memset(h->pps.scaling_matrix8, 16, 2*64*sizeof(uint8_t));
}

static int decode_init(AVCodecContext *avctx){
    H264Context *h= avctx->priv_data;
    MpegEncContext * const s = &h->s;

#ifdef JZC_CRC_VER
    crc_code = 0;
    mpFrame = 0;
#endif //JZC_CRC_VER

    MPV_decode_defaults(s);

    s->avctx = avctx;
    common_init(h);

    s->out_format = FMT_H264;
    s->workaround_bugs= avctx->workaround_bugs;

    // set defaults
//    s->decode_mb= ff_h263_decode_mb;
    s->quarter_sample = 1;
    s->low_delay= 1;
    avctx->pix_fmt= PIX_FMT_YUV420P;

    decode_init_vlc();

    if(avctx->extradata_size > 0 && avctx->extradata &&
       *(char *)avctx->extradata == 1){
        h->is_avc = 1;
        h->got_avcC = 0;
    } else {
        h->is_avc = 0;
    }

    h->thread_context[0] = h;
    return 0;
}

static int frame_start(H264Context *h){
    MpegEncContext * const s = &h->s;
    int i;

    if(MPV_frame_start(s, s->avctx) < 0)
        return -1;
    ff_er_frame_start(s);
    /*
     * MPV_frame_start uses pict_type to derive key_frame.
     * This is incorrect for H.264; IDR markings must be used.
     * Zero here; IDR markings per slice in frame or fields are OR'd in later.
     * See decode_nal_units().
     */
    s->current_picture_ptr->key_frame= 0;

    assert(s->linesize && s->uvlinesize);

    for(i=0; i<16; i++){
        h->block_offset[i]= 4*((scan8[i] - scan8[0])&7) + 4*s->linesize*((scan8[i] - scan8[0])>>3);
        h->block_offset[24+i]= 4*((scan8[i] - scan8[0])&7) + 8*s->linesize*((scan8[i] - scan8[0])>>3);
    }
    for(i=0; i<4; i++){
        h->block_offset[16+i]=
        h->block_offset[20+i]= 4*((scan8[i] - scan8[0])&7) + 4*s->uvlinesize*((scan8[i] - scan8[0])>>3);
        h->block_offset[24+16+i]=
        h->block_offset[24+20+i]= 4*((scan8[i] - scan8[0])&7) + 8*s->uvlinesize*((scan8[i] - scan8[0])>>3);
    }

    /* can't be in alloc_tables because linesize isn't known there.
     * FIXME: redo bipred weight to not require extra buffer? */
    for(i = 0; i < s->avctx->thread_count; i++)
        if(!h->thread_context[i]->s.obmc_scratchpad)
            h->thread_context[i]->s.obmc_scratchpad = av_malloc(16*2*s->linesize + 8*2*s->uvlinesize);

    /* some macroblocks will be accessed before they're available */
    if(FRAME_MBAFF || s->avctx->thread_count > 1)
        memset(h->slice_table, -1, (s->mb_height*s->mb_stride-1) * sizeof(uint8_t));

//    s->decode= (s->flags&CODEC_FLAG_PSNR) || !s->encoding || s->current_picture.reference /*|| h->contains_intra*/ || 1;
    return 0;
}

static inline void backup_mb_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize, int simple){
    MpegEncContext * const s = &h->s;
    int i;

    src_y  -=   linesize;
    src_cb -= uvlinesize;
    src_cr -= uvlinesize;

    // There are two lines saved, the line above the the top macroblock of a pair,
    // and the line above the bottom macroblock
    h->left_border[0]= h->top_borders[0][s->mb_x][15];
    for(i=1; i<17; i++){
        h->left_border[i]= src_y[15+i*  linesize];
    }

    *(uint64_t*)(h->top_borders[0][s->mb_x]+0)= *(uint64_t*)(src_y +  16*linesize);
    *(uint64_t*)(h->top_borders[0][s->mb_x]+8)= *(uint64_t*)(src_y +8+16*linesize);

    if(simple || !ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
        h->left_border[17  ]= h->top_borders[0][s->mb_x][16+7];
        h->left_border[17+9]= h->top_borders[0][s->mb_x][24+7];
        for(i=1; i<9; i++){
            h->left_border[i+17  ]= src_cb[7+i*uvlinesize];
            h->left_border[i+17+9]= src_cr[7+i*uvlinesize];
        }
        *(uint64_t*)(h->top_borders[0][s->mb_x]+16)= *(uint64_t*)(src_cb+8*uvlinesize);
        *(uint64_t*)(h->top_borders[0][s->mb_x]+24)= *(uint64_t*)(src_cr+8*uvlinesize);
    }
}

static inline void xchg_mb_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize, int xchg, int simple){
    MpegEncContext * const s = &h->s;
    int temp8, i;
    uint64_t temp64;
    int deblock_left;
    int deblock_top;
    int mb_xy;

    if(h->deblocking_filter == 2) {
        mb_xy = s->mb_x + s->mb_y*s->mb_stride;
        deblock_left = h->slice_table[mb_xy] == h->slice_table[mb_xy - 1];
        deblock_top  = h->slice_table[mb_xy] == h->slice_table[h->top_mb_xy];
    } else {
        deblock_left = (s->mb_x > 0);
        deblock_top =  (s->mb_y > 0);
    }

    src_y  -=   linesize + 1;
    src_cb -= uvlinesize + 1;
    src_cr -= uvlinesize + 1;

#define XCHG(a,b,t,xchg)\
t= a;\
if(xchg)\
    a= b;\
b= t;

    if(deblock_left){
        for(i = !deblock_top; i<17; i++){
            XCHG(h->left_border[i     ], src_y [i*  linesize], temp8, xchg);
        }
    }

    if(deblock_top){
        XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+0), *(uint64_t*)(src_y +1), temp64, xchg);
        XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+8), *(uint64_t*)(src_y +9), temp64, 1);
        if(s->mb_x+1 < s->mb_width){
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x+1]), *(uint64_t*)(src_y +17), temp64, 1);
        }
    }

    if(simple || !ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
        if(deblock_left){
            for(i = !deblock_top; i<9; i++){
                XCHG(h->left_border[i+17  ], src_cb[i*uvlinesize], temp8, xchg);
                XCHG(h->left_border[i+17+9], src_cr[i*uvlinesize], temp8, xchg);
            }
        }
        if(deblock_top){
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+16), *(uint64_t*)(src_cb+1), temp64, 1);
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+24), *(uint64_t*)(src_cr+1), temp64, 1);
        }
    }
}

static inline void backup_pair_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize){
    MpegEncContext * const s = &h->s;
    int i;

    src_y  -= 2 *   linesize;
    src_cb -= 2 * uvlinesize;
    src_cr -= 2 * uvlinesize;

    // There are two lines saved, the line above the the top macroblock of a pair,
    // and the line above the bottom macroblock
    h->left_border[0]= h->top_borders[0][s->mb_x][15];
    h->left_border[1]= h->top_borders[1][s->mb_x][15];
    for(i=2; i<34; i++){
        h->left_border[i]= src_y[15+i*  linesize];
    }

    *(uint64_t*)(h->top_borders[0][s->mb_x]+0)= *(uint64_t*)(src_y +  32*linesize);
    *(uint64_t*)(h->top_borders[0][s->mb_x]+8)= *(uint64_t*)(src_y +8+32*linesize);
    *(uint64_t*)(h->top_borders[1][s->mb_x]+0)= *(uint64_t*)(src_y +  33*linesize);
    *(uint64_t*)(h->top_borders[1][s->mb_x]+8)= *(uint64_t*)(src_y +8+33*linesize);

    if(!ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
        h->left_border[34     ]= h->top_borders[0][s->mb_x][16+7];
        h->left_border[34+   1]= h->top_borders[1][s->mb_x][16+7];
        h->left_border[34+18  ]= h->top_borders[0][s->mb_x][24+7];
        h->left_border[34+18+1]= h->top_borders[1][s->mb_x][24+7];
        for(i=2; i<18; i++){
            h->left_border[i+34   ]= src_cb[7+i*uvlinesize];
            h->left_border[i+34+18]= src_cr[7+i*uvlinesize];
        }
        *(uint64_t*)(h->top_borders[0][s->mb_x]+16)= *(uint64_t*)(src_cb+16*uvlinesize);
        *(uint64_t*)(h->top_borders[0][s->mb_x]+24)= *(uint64_t*)(src_cr+16*uvlinesize);
        *(uint64_t*)(h->top_borders[1][s->mb_x]+16)= *(uint64_t*)(src_cb+17*uvlinesize);
        *(uint64_t*)(h->top_borders[1][s->mb_x]+24)= *(uint64_t*)(src_cr+17*uvlinesize);
    }
}

static inline void xchg_pair_border(H264Context *h, uint8_t *src_y, uint8_t *src_cb, uint8_t *src_cr, int linesize, int uvlinesize, int xchg){
    MpegEncContext * const s = &h->s;
    int temp8, i;
    uint64_t temp64;
    int deblock_left = (s->mb_x > 0);
    int deblock_top  = (s->mb_y > 1);

    tprintf(s->avctx, "xchg_pair_border: src_y:%p src_cb:%p src_cr:%p ls:%d uvls:%d\n", src_y, src_cb, src_cr, linesize, uvlinesize);

    src_y  -= 2 *   linesize + 1;
    src_cb -= 2 * uvlinesize + 1;
    src_cr -= 2 * uvlinesize + 1;

#define XCHG(a,b,t,xchg)\
t= a;\
if(xchg)\
    a= b;\
b= t;

    if(deblock_left){
        for(i = (!deblock_top)<<1; i<34; i++){
            XCHG(h->left_border[i     ], src_y [i*  linesize], temp8, xchg);
        }
    }

    if(deblock_top){
        XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+0), *(uint64_t*)(src_y +1), temp64, xchg);
        XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+8), *(uint64_t*)(src_y +9), temp64, 1);
        XCHG(*(uint64_t*)(h->top_borders[1][s->mb_x]+0), *(uint64_t*)(src_y +1 +linesize), temp64, xchg);
        XCHG(*(uint64_t*)(h->top_borders[1][s->mb_x]+8), *(uint64_t*)(src_y +9 +linesize), temp64, 1);
        if(s->mb_x+1 < s->mb_width){
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x+1]), *(uint64_t*)(src_y +17), temp64, 1);
            XCHG(*(uint64_t*)(h->top_borders[1][s->mb_x+1]), *(uint64_t*)(src_y +17 +linesize), temp64, 1);
        }
    }

    if(!ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
        if(deblock_left){
            for(i = (!deblock_top) << 1; i<18; i++){
                XCHG(h->left_border[i+34   ], src_cb[i*uvlinesize], temp8, xchg);
                XCHG(h->left_border[i+34+18], src_cr[i*uvlinesize], temp8, xchg);
            }
        }
        if(deblock_top){
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+16), *(uint64_t*)(src_cb+1), temp64, 1);
            XCHG(*(uint64_t*)(h->top_borders[0][s->mb_x]+24), *(uint64_t*)(src_cr+1), temp64, 1);
            XCHG(*(uint64_t*)(h->top_borders[1][s->mb_x]+16), *(uint64_t*)(src_cb+1 +uvlinesize), temp64, 1);
            XCHG(*(uint64_t*)(h->top_borders[1][s->mb_x]+24), *(uint64_t*)(src_cr+1 +uvlinesize), temp64, 1);
        }
    }
}

static av_always_inline void hl_decode_mb_internal(H264Context *h, int simple){
    MpegEncContext * const s = &h->s;
    const int mb_x= s->mb_x;
    const int mb_y= s->mb_y;

    const int mb_xy= mb_x + mb_y*s->mb_stride;
    const int mb_type= s->current_picture.mb_type[mb_xy];
    uint8_t  *dest_y, *dest_cb, *dest_cr;
    int linesize, uvlinesize /*dct_offset*/;
    int i;
    int *block_offset = &h->block_offset[0];
    const unsigned int bottom = mb_y & 1;
    const int transform_bypass = (s->qscale == 0 && h->sps.transform_bypass);
    void (*idct_add)(uint8_t *dst, DCTELEM *block, int stride);
    void (*idct_dc_add)(uint8_t *dst, DCTELEM *block, int stride);

    dest_y  = s->current_picture.data[0] + (mb_y * 16* s->linesize  ) + mb_x * 16;
    dest_cb = s->current_picture.data[1] + (mb_y * 8 * s->uvlinesize) + mb_x * 8;
    dest_cr = s->current_picture.data[2] + (mb_y * 8 * s->uvlinesize) + mb_x * 8;

    s->dsp.prefetch(dest_y + (s->mb_x&3)*4*s->linesize + 64, s->linesize, 4);
    s->dsp.prefetch(dest_cb + (s->mb_x&7)*s->uvlinesize + 64, dest_cr - dest_cb, 2);

    if (!simple && MB_FIELD) {
        linesize   = h->mb_linesize   = s->linesize * 2;
        uvlinesize = h->mb_uvlinesize = s->uvlinesize * 2;
        block_offset = &h->block_offset[24];
        if(mb_y&1){ //FIXME move out of this func?
            dest_y -= s->linesize*15;
            dest_cb-= s->uvlinesize*7;
            dest_cr-= s->uvlinesize*7;
        }
        if(FRAME_MBAFF) {
            int list;
            for(list=0; list<h->list_count; list++){
                if(!USES_LIST(mb_type, list))
                    continue;
                if(IS_16X16(mb_type)){
                    int8_t *ref = &h->ref_cache[list][scan8[0]];
                    fill_rectangle(ref, 4, 4, 8, 16+*ref^(s->mb_y&1), 1);
                }else{
                    for(i=0; i<16; i+=4){
                        //FIXME can refs be smaller than 8x8 when !direct_8x8_inference ?
                        int ref = h->ref_cache[list][scan8[i]];
                        if(ref >= 0)
                            fill_rectangle(&h->ref_cache[list][scan8[i]], 2, 2, 8, 16+ref^(s->mb_y&1), 1);
                    }
                }
            }
        }
    } else {
        linesize   = h->mb_linesize   = s->linesize;
        uvlinesize = h->mb_uvlinesize = s->uvlinesize;
//        dct_offset = s->linesize * 16;
    }

    if(transform_bypass){
        idct_dc_add =
        idct_add = IS_8x8DCT(mb_type) ? s->dsp.add_pixels8 : s->dsp.add_pixels4;
    }else if(IS_8x8DCT(mb_type)){
        idct_dc_add = s->dsp.h264_idct8_dc_add;
        idct_add = s->dsp.h264_idct8_add;
    }else{
        idct_dc_add = s->dsp.h264_idct_dc_add;
        idct_add = s->dsp.h264_idct_add;
    }

    if(!simple && FRAME_MBAFF && h->deblocking_filter && IS_INTRA(mb_type)
       && (!bottom || !IS_INTRA(s->current_picture.mb_type[mb_xy-s->mb_stride]))){
        int mbt_y = mb_y&~1;
        uint8_t *top_y  = s->current_picture.data[0] + (mbt_y * 16* s->linesize  ) + mb_x * 16;
        uint8_t *top_cb = s->current_picture.data[1] + (mbt_y * 8 * s->uvlinesize) + mb_x * 8;
        uint8_t *top_cr = s->current_picture.data[2] + (mbt_y * 8 * s->uvlinesize) + mb_x * 8;
        xchg_pair_border(h, top_y, top_cb, top_cr, s->linesize, s->uvlinesize, 1);
    }

    if (!simple && IS_INTRA_PCM(mb_type)) {
        unsigned int x, y;

        // The pixels are stored in h->mb array in the same order as levels,
        // copy them in output in the correct order.
        for(i=0; i<16; i++) {
            for (y=0; y<4; y++) {
                for (x=0; x<4; x++) {
                    *(dest_y + block_offset[i] + y*linesize + x) = h->mb[i*16+y*4+x];
                }
            }
        }
        for(i=16; i<16+4; i++) {
            for (y=0; y<4; y++) {
                for (x=0; x<4; x++) {
                    *(dest_cb + block_offset[i] + y*uvlinesize + x) = h->mb[i*16+y*4+x];
                }
            }
        }
        for(i=20; i<20+4; i++) {
            for (y=0; y<4; y++) {
                for (x=0; x<4; x++) {
                    *(dest_cr + block_offset[i] + y*uvlinesize + x) = h->mb[i*16+y*4+x];
                }
            }
        }
    } else {
        if(IS_INTRA(mb_type)){
            if(h->deblocking_filter && (simple || !FRAME_MBAFF))
                xchg_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, 1, simple);

            if(simple || !ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
                h->hpc.pred8x8[ h->chroma_pred_mode ](dest_cb, uvlinesize);
                h->hpc.pred8x8[ h->chroma_pred_mode ](dest_cr, uvlinesize);
            }

            if(IS_INTRA4x4(mb_type)){
                if(simple || !s->encoding){
                    if(IS_8x8DCT(mb_type)){
                        for(i=0; i<16; i+=4){
                            uint8_t * const ptr= dest_y + block_offset[i];
                            const int dir= h->intra4x4_pred_mode_cache[ scan8[i] ];
                            const int nnz = h->non_zero_count_cache[ scan8[i] ];
                            h->hpc.pred8x8l[ dir ](ptr, (h->topleft_samples_available<<i)&0x8000,
                                                   (h->topright_samples_available<<i)&0x4000, linesize);
                            if(nnz){
                                if(nnz == 1 && h->mb[i*16])
                                    idct_dc_add(ptr, h->mb + i*16, linesize);
                                else
                                    idct_add(ptr, h->mb + i*16, linesize);
                            }
                        }
                    }else
                    for(i=0; i<16; i++){
                        uint8_t * const ptr= dest_y + block_offset[i];
                        uint8_t *topright;
                        const int dir= h->intra4x4_pred_mode_cache[ scan8[i] ];
                        int nnz, tr;

                        if(dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED){
                            const int topright_avail= (h->topright_samples_available<<i)&0x8000;
                            assert(mb_y || linesize <= block_offset[i]);
                            if(!topright_avail){
                                tr= ptr[3 - linesize]*0x01010101;
                                topright= (uint8_t*) &tr;
                            }else
                                topright= ptr + 4 - linesize;
                        }else
                            topright= NULL;

                        h->hpc.pred4x4[ dir ](ptr, topright, linesize);
                        nnz = h->non_zero_count_cache[ scan8[i] ];
                        if(nnz){
			  svq3_add_idct_c(ptr, h->mb + i*16, linesize, s->qscale, 0);
                        }
                    }
                }
            }else{
                h->hpc.pred16x16[ h->intra16x16_pred_mode ](dest_y , linesize);
		svq3_luma_dc_dequant_idct_c(h->mb, s->qscale);
            }
            if(h->deblocking_filter && (simple || !FRAME_MBAFF))
                xchg_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, 0, simple);
        }

        if(!IS_INTRA4x4(mb_type)){
	  for(i=0; i<16; i++){
	    if(h->non_zero_count_cache[ scan8[i] ] || h->mb[i*16]){ //FIXME benchmark weird rule, & below
	      uint8_t * const ptr= dest_y + block_offset[i];
	      svq3_add_idct_c(ptr, h->mb + i*16, linesize, s->qscale, IS_INTRA(mb_type) ? 1 : 0);
	    }
	  }
	}

        if(simple || !ENABLE_GRAY || !(s->flags&CODEC_FLAG_GRAY)){
            uint8_t *dest[2] = {dest_cb, dest_cr};
            if(transform_bypass){
                idct_add = idct_dc_add = s->dsp.add_pixels4;
            }else{
                idct_add = s->dsp.h264_idct_add;
                idct_dc_add = s->dsp.h264_idct_dc_add;
                chroma_dc_dequant_idct_c(h->mb + 16*16, h->chroma_qp[0], h->dequant4_coeff[IS_INTRA(mb_type) ? 1:4][h->chroma_qp[0]][0]);
                chroma_dc_dequant_idct_c(h->mb + 16*16+4*16, h->chroma_qp[1], h->dequant4_coeff[IS_INTRA(mb_type) ? 2:5][h->chroma_qp[1]][0]);
            }
	    for(i=16; i<16+8; i++){
	      if(h->non_zero_count_cache[ scan8[i] ] || h->mb[i*16]){
		uint8_t * const ptr= dest[(i&4)>>2] + block_offset[i];
		svq3_add_idct_c(ptr, h->mb + i*16, uvlinesize, chroma_qp[s->qscale + 12] - 12, 2);
	      }
	    }
        }
    }
    if(h->deblocking_filter) {
        if (!simple && FRAME_MBAFF) {
            //FIXME try deblocking one mb at a time?
            // the reduction in load/storing mvs and such might outweigh the extra backup/xchg_border
            const int mb_y = s->mb_y - 1;
            uint8_t  *pair_dest_y, *pair_dest_cb, *pair_dest_cr;
            const int mb_xy= mb_x + mb_y*s->mb_stride;
            const int mb_type_top   = s->current_picture.mb_type[mb_xy];
            const int mb_type_bottom= s->current_picture.mb_type[mb_xy+s->mb_stride];
            if (!bottom) return;
            pair_dest_y  = s->current_picture.data[0] + (mb_y * 16* s->linesize  ) + mb_x * 16;
            pair_dest_cb = s->current_picture.data[1] + (mb_y * 8 * s->uvlinesize) + mb_x * 8;
            pair_dest_cr = s->current_picture.data[2] + (mb_y * 8 * s->uvlinesize) + mb_x * 8;

            if(IS_INTRA(mb_type_top | mb_type_bottom))
                xchg_pair_border(h, pair_dest_y, pair_dest_cb, pair_dest_cr, s->linesize, s->uvlinesize, 0);

            backup_pair_border(h, pair_dest_y, pair_dest_cb, pair_dest_cr, s->linesize, s->uvlinesize);
            // deblock a pair
            // top
            s->mb_y--;
            tprintf(h->s.avctx, "call mbaff filter_mb mb_x:%d mb_y:%d pair_dest_y = %p, dest_y = %p\n", mb_x, mb_y, pair_dest_y, dest_y);
            fill_caches(h, mb_type_top, 1); //FIXME don't fill stuff which isn't used by filter_mb
            h->chroma_qp[0] = get_chroma_qp(h, 0, s->current_picture.qscale_table[mb_xy]);
            h->chroma_qp[1] = get_chroma_qp(h, 1, s->current_picture.qscale_table[mb_xy]);
            filter_mb(h, mb_x, mb_y, pair_dest_y, pair_dest_cb, pair_dest_cr, linesize, uvlinesize);
            // bottom
            s->mb_y++;
            tprintf(h->s.avctx, "call mbaff filter_mb\n");
            fill_caches(h, mb_type_bottom, 1); //FIXME don't fill stuff which isn't used by filter_mb
            h->chroma_qp[0] = get_chroma_qp(h, 0, s->current_picture.qscale_table[mb_xy+s->mb_stride]);
            h->chroma_qp[1] = get_chroma_qp(h, 1, s->current_picture.qscale_table[mb_xy+s->mb_stride]);
            filter_mb(h, mb_x, mb_y+1, dest_y, dest_cb, dest_cr, linesize, uvlinesize);
        } else {
            tprintf(h->s.avctx, "call filter_mb\n");
            backup_mb_border(h, dest_y, dest_cb, dest_cr, linesize, uvlinesize, simple);
            fill_caches(h, mb_type, 1); //FIXME don't fill stuff which isn't used by filter_mb
            filter_mb_fast(h, mb_x, mb_y, dest_y, dest_cb, dest_cr, linesize, uvlinesize);
        }
    }
}

static void hl_decode_mb(H264Context *h){
    MpegEncContext * const s = &h->s;
    const int mb_x= s->mb_x;
    const int mb_y= s->mb_y;
    const int mb_xy= mb_x + mb_y*s->mb_stride;
    const int mb_type= s->current_picture.mb_type[mb_xy];

    if(!s->decode)
        return;

    hl_decode_mb_internal(h, 0);
}

static void filter_mb_edgev( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp ) {
    int i, d;
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table+52)[index_a];
    const int beta  = (beta_table+52)[qp + h->slice_beta_offset];

    if( bS[0] < 4 ) {
        int8_t tc[4];
        for(i=0; i<4; i++)
            tc[i] = bS[i] ? (tc0_table+52)[index_a][bS[i] - 1] : -1;
        h->s.dsp.h264_h_loop_filter_luma(pix, stride, alpha, beta, tc);
    } else {
        /* 16px edge length, because bS=4 is triggered by being at
         * the edge of an intra MB, so all 4 bS are the same */
            for( d = 0; d < 16; d++ ) {
                const int p0 = pix[-1];
                const int p1 = pix[-2];
                const int p2 = pix[-3];

                const int q0 = pix[0];
                const int q1 = pix[1];
                const int q2 = pix[2];

                if( FFABS( p0 - q0 ) < alpha &&
                    FFABS( p1 - p0 ) < beta &&
                    FFABS( q1 - q0 ) < beta ) {

                    if(FFABS( p0 - q0 ) < (( alpha >> 2 ) + 2 )){
                        if( FFABS( p2 - p0 ) < beta)
                        {
                            const int p3 = pix[-4];
                            /* p0', p1', p2' */
                            pix[-1] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3;
                            pix[-2] = ( p2 + p1 + p0 + q0 + 2 ) >> 2;
                            pix[-3] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3;
                        } else {
                            /* p0' */
                            pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                        }
                        if( FFABS( q2 - q0 ) < beta)
                        {
                            const int q3 = pix[3];
                            /* q0', q1', q2' */
                            pix[0] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3;
                            pix[1] = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
                            pix[2] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3;
                        } else {
                            /* q0' */
                            pix[0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                        }
                    }else{
                        /* p0', q0' */
                        pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                        pix[ 0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                    }
                    tprintf(h->s.avctx, "filter_mb_edgev i:%d d:%d\n# bS:4 -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, d, p2, p1, p0, q0, q1, q2, pix[-2], pix[-1], pix[0], pix[1]);
                }
                pix += stride;
            }
    }
}

static void filter_mb_edgecv( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp ) {
    int i;
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table+52)[index_a];
    const int beta  = (beta_table+52)[qp + h->slice_beta_offset];

    if( bS[0] < 4 ) {
        int8_t tc[4];
        for(i=0; i<4; i++)
            tc[i] = bS[i] ? (tc0_table+52)[index_a][bS[i] - 1] + 1 : 0;
        h->s.dsp.h264_h_loop_filter_chroma(pix, stride, alpha, beta, tc);
    } else {
        h->s.dsp.h264_h_loop_filter_chroma_intra(pix, stride, alpha, beta);
    }
}

static void filter_mb_mbaff_edgev( H264Context *h, uint8_t *pix, int stride, int16_t bS[8], int qp[2] ) {
    int i;
    for( i = 0; i < 16; i++, pix += stride) {
        int index_a;
        int alpha;
        int beta;

        int qp_index;
        int bS_index = (i >> 1);
        if (!MB_FIELD) {
            bS_index &= ~1;
            bS_index |= (i & 1);
        }

        if( bS[bS_index] == 0 ) {
            continue;
        }

        qp_index = MB_FIELD ? (i >> 3) : (i & 1);
        index_a = qp[qp_index] + h->slice_alpha_c0_offset;
        alpha = (alpha_table+52)[index_a];
        beta  = (beta_table+52)[qp[qp_index] + h->slice_beta_offset];

        if( bS[bS_index] < 4 ) {
            const int tc0 = (tc0_table+52)[index_a][bS[bS_index] - 1];
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int p2 = pix[-3];
            const int q0 = pix[0];
            const int q1 = pix[1];
            const int q2 = pix[2];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {
                int tc = tc0;
                int i_delta;

                if( FFABS( p2 - p0 ) < beta ) {
                    pix[-2] = p1 + av_clip( ( p2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( p1 << 1 ) ) >> 1, -tc0, tc0 );
                    tc++;
                }
                if( FFABS( q2 - q0 ) < beta ) {
                    pix[1] = q1 + av_clip( ( q2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( q1 << 1 ) ) >> 1, -tc0, tc0 );
                    tc++;
                }

                i_delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );
                pix[-1] = av_clip_uint8( p0 + i_delta );    /* p0' */
                pix[0]  = av_clip_uint8( q0 - i_delta );    /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgev i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d, tc:%d\n# bS:%d -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, tc, bS[bS_index], pix[-3], p1, p0, q0, q1, pix[2], p1, pix[-1], pix[0], q1);
            }
        }else{
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int p2 = pix[-3];

            const int q0 = pix[0];
            const int q1 = pix[1];
            const int q2 = pix[2];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {

                if(FFABS( p0 - q0 ) < (( alpha >> 2 ) + 2 )){
                    if( FFABS( p2 - p0 ) < beta)
                    {
                        const int p3 = pix[-4];
                        /* p0', p1', p2' */
                        pix[-1] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3;
                        pix[-2] = ( p2 + p1 + p0 + q0 + 2 ) >> 2;
                        pix[-3] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3;
                    } else {
                        /* p0' */
                        pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                    }
                    if( FFABS( q2 - q0 ) < beta)
                    {
                        const int q3 = pix[3];
                        /* q0', q1', q2' */
                        pix[0] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3;
                        pix[1] = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
                        pix[2] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3;
                    } else {
                        /* q0' */
                        pix[0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                    }
                }else{
                    /* p0', q0' */
                    pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                    pix[ 0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                }
                tprintf(h->s.avctx, "filter_mb_mbaff_edgev i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d\n# bS:4 -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, p2, p1, p0, q0, q1, q2, pix[-3], pix[-2], pix[-1], pix[0], pix[1], pix[2]);
            }
        }
    }
}
static void filter_mb_mbaff_edgecv( H264Context *h, uint8_t *pix, int stride, int16_t bS[8], int qp[2] ) {
    int i;
    for( i = 0; i < 8; i++, pix += stride) {
        int index_a;
        int alpha;
        int beta;

        int qp_index;
        int bS_index = i;

        if( bS[bS_index] == 0 ) {
            continue;
        }

        qp_index = MB_FIELD ? (i >> 2) : (i & 1);
        index_a = qp[qp_index] + h->slice_alpha_c0_offset;
        alpha = (alpha_table+52)[index_a];
        beta  = (beta_table+52)[qp[qp_index] + h->slice_beta_offset];

        if( bS[bS_index] < 4 ) {
            const int tc = (tc0_table+52)[index_a][bS[bS_index] - 1] + 1;
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int q0 = pix[0];
            const int q1 = pix[1];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {
                const int i_delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );

                pix[-1] = av_clip_uint8( p0 + i_delta );    /* p0' */
                pix[0]  = av_clip_uint8( q0 - i_delta );    /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgecv i:%d, qp:%d, indexA:%d, alpha:%d, beta:%d, tc:%d\n# bS:%d -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, qp[qp_index], index_a, alpha, beta, tc, bS[bS_index], pix[-3], p1, p0, q0, q1, pix[2], p1, pix[-1], pix[0], q1);
            }
        }else{
            const int p0 = pix[-1];
            const int p1 = pix[-2];
            const int q0 = pix[0];
            const int q1 = pix[1];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {

                pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;   /* p0' */
                pix[0]  = ( 2*q1 + q0 + p1 + 2 ) >> 2;   /* q0' */
                tprintf(h->s.avctx, "filter_mb_mbaff_edgecv i:%d\n# bS:4 -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x, %02x, %02x]\n", i, pix[-3], p1, p0, q0, q1, pix[2], pix[-3], pix[-2], pix[-1], pix[0], pix[1], pix[2]);
            }
        }
    }
}

static void filter_mb_edgeh( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp ) {
    int i, d;
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table+52)[index_a];
    const int beta  = (beta_table+52)[qp + h->slice_beta_offset];
    const int pix_next  = stride;

    if( bS[0] < 4 ) {
        int8_t tc[4];
        for(i=0; i<4; i++)
            tc[i] = bS[i] ? (tc0_table+52)[index_a][bS[i] - 1] : -1;
        h->s.dsp.h264_v_loop_filter_luma(pix, stride, alpha, beta, tc);
    } else {
        /* 16px edge length, see filter_mb_edgev */
            for( d = 0; d < 16; d++ ) {
                const int p0 = pix[-1*pix_next];
                const int p1 = pix[-2*pix_next];
                const int p2 = pix[-3*pix_next];
                const int q0 = pix[0];
                const int q1 = pix[1*pix_next];
                const int q2 = pix[2*pix_next];

                if( FFABS( p0 - q0 ) < alpha &&
                    FFABS( p1 - p0 ) < beta &&
                    FFABS( q1 - q0 ) < beta ) {

                    const int p3 = pix[-4*pix_next];
                    const int q3 = pix[ 3*pix_next];

                    if(FFABS( p0 - q0 ) < (( alpha >> 2 ) + 2 )){
                        if( FFABS( p2 - p0 ) < beta) {
                            /* p0', p1', p2' */
                            pix[-1*pix_next] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3;
                            pix[-2*pix_next] = ( p2 + p1 + p0 + q0 + 2 ) >> 2;
                            pix[-3*pix_next] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3;
                        } else {
                            /* p0' */
                            pix[-1*pix_next] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                        }
                        if( FFABS( q2 - q0 ) < beta) {
                            /* q0', q1', q2' */
                            pix[0*pix_next] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3;
                            pix[1*pix_next] = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
                            pix[2*pix_next] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3;
                        } else {
                            /* q0' */
                            pix[0*pix_next] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                        }
                    }else{
                        /* p0', q0' */
                        pix[-1*pix_next] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
                        pix[ 0*pix_next] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
                    }
                    tprintf(h->s.avctx, "filter_mb_edgeh i:%d d:%d, qp:%d, indexA:%d, alpha:%d, beta:%d\n# bS:%d -> [%02x, %02x, %02x, %02x, %02x, %02x] =>[%02x, %02x, %02x, %02x]\n", i, d, qp, index_a, alpha, beta, bS[i], p2, p1, p0, q0, q1, q2, pix[-2*pix_next], pix[-pix_next], pix[0], pix[pix_next]);
                }
                pix++;
            }
    }
}

static void filter_mb_edgech( H264Context *h, uint8_t *pix, int stride, int16_t bS[4], int qp ) {
    int i;
    const int index_a = qp + h->slice_alpha_c0_offset;
    const int alpha = (alpha_table+52)[index_a];
    const int beta  = (beta_table+52)[qp + h->slice_beta_offset];

    if( bS[0] < 4 ) {
        int8_t tc[4];
        for(i=0; i<4; i++)
            tc[i] = bS[i] ? (tc0_table+52)[index_a][bS[i] - 1] + 1 : 0;
        h->s.dsp.h264_v_loop_filter_chroma(pix, stride, alpha, beta, tc);
    } else {
        h->s.dsp.h264_v_loop_filter_chroma_intra(pix, stride, alpha, beta);
    }
}

static void filter_mb_fast( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize) {
    MpegEncContext * const s = &h->s;
    int mb_xy, mb_type;
    int qp, qp0, qp1, qpc, qpc0, qpc1, qp_thresh;

    mb_xy = mb_x + mb_y*s->mb_stride;

    if(mb_x==0 || mb_y==0 || !s->dsp.h264_loop_filter_strength || h->pps.chroma_qp_diff ||
       (h->deblocking_filter == 2 && (h->slice_table[mb_xy] != h->slice_table[h->top_mb_xy] ||
                                      h->slice_table[mb_xy] != h->slice_table[mb_xy - 1]))) {
        filter_mb(h, mb_x, mb_y, img_y, img_cb, img_cr, linesize, uvlinesize);
        return;
    }
    assert(!FRAME_MBAFF);

    mb_type = s->current_picture.mb_type[mb_xy];
    qp = s->current_picture.qscale_table[mb_xy];
    qp0 = s->current_picture.qscale_table[mb_xy-1];
    qp1 = s->current_picture.qscale_table[h->top_mb_xy];
    qpc = get_chroma_qp( h, 0, qp );
    qpc0 = get_chroma_qp( h, 0, qp0 );
    qpc1 = get_chroma_qp( h, 0, qp1 );
    qp0 = (qp + qp0 + 1) >> 1;
    qp1 = (qp + qp1 + 1) >> 1;
    qpc0 = (qpc + qpc0 + 1) >> 1;
    qpc1 = (qpc + qpc1 + 1) >> 1;
    qp_thresh = 15 - h->slice_alpha_c0_offset;
    if(qp <= qp_thresh && qp0 <= qp_thresh && qp1 <= qp_thresh &&
       qpc <= qp_thresh && qpc0 <= qp_thresh && qpc1 <= qp_thresh)
        return;

    if( IS_INTRA(mb_type) ) {
        int16_t bS4[4] = {4,4,4,4};
        int16_t bS3[4] = {3,3,3,3};
        if( IS_8x8DCT(mb_type) ) {
            filter_mb_edgev( h, &img_y[4*0], linesize, bS4, qp0 );
            filter_mb_edgev( h, &img_y[4*2], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4*0*linesize], linesize, bS4, qp1 );
            filter_mb_edgeh( h, &img_y[4*2*linesize], linesize, bS3, qp );
        } else {
            filter_mb_edgev( h, &img_y[4*0], linesize, bS4, qp0 );
            filter_mb_edgev( h, &img_y[4*1], linesize, bS3, qp );
            filter_mb_edgev( h, &img_y[4*2], linesize, bS3, qp );
            filter_mb_edgev( h, &img_y[4*3], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4*0*linesize], linesize, bS4, qp1 );
            filter_mb_edgeh( h, &img_y[4*1*linesize], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4*2*linesize], linesize, bS3, qp );
            filter_mb_edgeh( h, &img_y[4*3*linesize], linesize, bS3, qp );
        }
        filter_mb_edgecv( h, &img_cb[2*0], uvlinesize, bS4, qpc0 );
        filter_mb_edgecv( h, &img_cb[2*2], uvlinesize, bS3, qpc );
        filter_mb_edgecv( h, &img_cr[2*0], uvlinesize, bS4, qpc0 );
        filter_mb_edgecv( h, &img_cr[2*2], uvlinesize, bS3, qpc );
        filter_mb_edgech( h, &img_cb[2*0*uvlinesize], uvlinesize, bS4, qpc1 );
        filter_mb_edgech( h, &img_cb[2*2*uvlinesize], uvlinesize, bS3, qpc );
        filter_mb_edgech( h, &img_cr[2*0*uvlinesize], uvlinesize, bS4, qpc1 );
        filter_mb_edgech( h, &img_cr[2*2*uvlinesize], uvlinesize, bS3, qpc );
        return;
    } else {
        DECLARE_ALIGNED_8(int16_t, bS[2][4][4]);
        uint64_t (*bSv)[4] = (uint64_t(*)[4])bS;
        int edges;
        if( IS_8x8DCT(mb_type) && (h->cbp&7) == 7 ) {
            edges = 4;
            bSv[0][0] = bSv[0][2] = bSv[1][0] = bSv[1][2] = 0x0002000200020002ULL;
        } else {
            int mask_edge1 = (mb_type & (MB_TYPE_16x16 | MB_TYPE_8x16)) ? 3 :
                             (mb_type & MB_TYPE_16x8) ? 1 : 0;
            int mask_edge0 = (mb_type & (MB_TYPE_16x16 | MB_TYPE_8x16))
                             && (s->current_picture.mb_type[mb_xy-1] & (MB_TYPE_16x16 | MB_TYPE_8x16))
                             ? 3 : 0;
            int step = IS_8x8DCT(mb_type) ? 2 : 1;
            edges = (mb_type & MB_TYPE_16x16) && !(h->cbp & 15) ? 1 : 4;
            s->dsp.h264_loop_filter_strength( bS, h->non_zero_count_cache, h->ref_cache, h->mv_cache,
                                              (h->slice_type == B_TYPE), edges, step, mask_edge0, mask_edge1 );
        }
        if( IS_INTRA(s->current_picture.mb_type[mb_xy-1]) )
            bSv[0][0] = 0x0004000400040004ULL;
        if( IS_INTRA(s->current_picture.mb_type[h->top_mb_xy]) )
            bSv[1][0] = 0x0004000400040004ULL;

#define FILTER(hv,dir,edge)\
        if(bSv[dir][edge]) {\
            filter_mb_edge##hv( h, &img_y[4*edge*(dir?linesize:1)], linesize, bS[dir][edge], edge ? qp : qp##dir );\
            if(!(edge&1)) {\
                filter_mb_edgec##hv( h, &img_cb[2*edge*(dir?uvlinesize:1)], uvlinesize, bS[dir][edge], edge ? qpc : qpc##dir );\
                filter_mb_edgec##hv( h, &img_cr[2*edge*(dir?uvlinesize:1)], uvlinesize, bS[dir][edge], edge ? qpc : qpc##dir );\
            }\
        }
        if( edges == 1 ) {
            FILTER(v,0,0);
            FILTER(h,1,0);
        } else if( IS_8x8DCT(mb_type) ) {
            FILTER(v,0,0);
            FILTER(v,0,2);
            FILTER(h,1,0);
            FILTER(h,1,2);
        } else {
            FILTER(v,0,0);
            FILTER(v,0,1);
            FILTER(v,0,2);
            FILTER(v,0,3);
            FILTER(h,1,0);
            FILTER(h,1,1);
            FILTER(h,1,2);
            FILTER(h,1,3);
        }
#undef FILTER
    }
}

static void filter_mb( H264Context *h, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize) {
    MpegEncContext * const s = &h->s;
    const int mb_xy= mb_x + mb_y*s->mb_stride;
    const int mb_type = s->current_picture.mb_type[mb_xy];
    const int mvy_limit = IS_INTERLACED(mb_type) ? 2 : 4;
    int first_vertical_edge_done = 0;
    int dir;
    /* FIXME: A given frame may occupy more than one position in
     * the reference list. So ref2frm should be populated with
     * frame numbers, not indices. */
    static const int ref2frm[34] = {-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                                    16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

    //for sufficiently low qp, filtering wouldn't do anything
    //this is a conservative estimate: could also check beta_offset and more accurate chroma_qp
    if(!FRAME_MBAFF){
        int qp_thresh = 15 - h->slice_alpha_c0_offset - FFMAX(0, FFMAX(h->pps.chroma_qp_index_offset[0], h->pps.chroma_qp_index_offset[1]));
        int qp = s->current_picture.qscale_table[mb_xy];
        if(qp <= qp_thresh
           && (mb_x == 0 || ((qp + s->current_picture.qscale_table[mb_xy-1] + 1)>>1) <= qp_thresh)
           && (mb_y == 0 || ((qp + s->current_picture.qscale_table[h->top_mb_xy] + 1)>>1) <= qp_thresh)){
            return;
        }
    }

    if (FRAME_MBAFF
            // left mb is in picture
            && h->slice_table[mb_xy-1] != 255
            // and current and left pair do not have the same interlaced type
            && (IS_INTERLACED(mb_type) != IS_INTERLACED(s->current_picture.mb_type[mb_xy-1]))
            // and left mb is in the same slice if deblocking_filter == 2
            && (h->deblocking_filter!=2 || h->slice_table[mb_xy-1] == h->slice_table[mb_xy])) {
        /* First vertical edge is different in MBAFF frames
         * There are 8 different bS to compute and 2 different Qp
         */
        const int pair_xy = mb_x + (mb_y&~1)*s->mb_stride;
        const int left_mb_xy[2] = { pair_xy-1, pair_xy-1+s->mb_stride };
        int16_t bS[8];
        int qp[2];
        int bqp[2];
        int rqp[2];
        int mb_qp, mbn0_qp, mbn1_qp;
        int i;
        first_vertical_edge_done = 1;

        if( IS_INTRA(mb_type) )
            bS[0] = bS[1] = bS[2] = bS[3] = bS[4] = bS[5] = bS[6] = bS[7] = 4;
        else {
            for( i = 0; i < 8; i++ ) {
                int mbn_xy = MB_FIELD ? left_mb_xy[i>>2] : left_mb_xy[i&1];

                if( IS_INTRA( s->current_picture.mb_type[mbn_xy] ) )
                    bS[i] = 4;
                else if( h->non_zero_count_cache[12+8*(i>>1)] != 0 ||
                         /* FIXME: with 8x8dct + cavlc, should check cbp instead of nnz */
                         h->non_zero_count[mbn_xy][MB_FIELD ? i&3 : (i>>2)+(mb_y&1)*2] )
                    bS[i] = 2;
                else
                    bS[i] = 1;
            }
        }

        mb_qp = s->current_picture.qscale_table[mb_xy];
        mbn0_qp = s->current_picture.qscale_table[left_mb_xy[0]];
        mbn1_qp = s->current_picture.qscale_table[left_mb_xy[1]];
        qp[0] = ( mb_qp + mbn0_qp + 1 ) >> 1;
        bqp[0] = ( get_chroma_qp( h, 0, mb_qp ) +
                   get_chroma_qp( h, 0, mbn0_qp ) + 1 ) >> 1;
        rqp[0] = ( get_chroma_qp( h, 1, mb_qp ) +
                   get_chroma_qp( h, 1, mbn0_qp ) + 1 ) >> 1;
        qp[1] = ( mb_qp + mbn1_qp + 1 ) >> 1;
        bqp[1] = ( get_chroma_qp( h, 0, mb_qp ) +
                   get_chroma_qp( h, 0, mbn1_qp ) + 1 ) >> 1;
        rqp[1] = ( get_chroma_qp( h, 1, mb_qp ) +
                   get_chroma_qp( h, 1, mbn1_qp ) + 1 ) >> 1;

        /* Filter edge */
        tprintf(s->avctx, "filter mb:%d/%d MBAFF, QPy:%d/%d, QPb:%d/%d QPr:%d/%d ls:%d uvls:%d", mb_x, mb_y, qp[0], qp[1], bqp[0], bqp[1], rqp[0], rqp[1], linesize, uvlinesize);
        { int i; for (i = 0; i < 8; i++) tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); tprintf(s->avctx, "\n"); }
        filter_mb_mbaff_edgev ( h, &img_y [0], linesize,   bS, qp );
        filter_mb_mbaff_edgecv( h, &img_cb[0], uvlinesize, bS, bqp );
        filter_mb_mbaff_edgecv( h, &img_cr[0], uvlinesize, bS, rqp );
    }
    /* dir : 0 -> vertical edge, 1 -> horizontal edge */
    for( dir = 0; dir < 2; dir++ )
    {
        int edge;
        const int mbm_xy = dir == 0 ? mb_xy -1 : h->top_mb_xy;
        const int mbm_type = s->current_picture.mb_type[mbm_xy];
        int start = h->slice_table[mbm_xy] == 255 ? 1 : 0;

        const int edges = (mb_type & (MB_TYPE_16x16|MB_TYPE_SKIP))
                                  == (MB_TYPE_16x16|MB_TYPE_SKIP) ? 1 : 4;
        // how often to recheck mv-based bS when iterating between edges
        const int mask_edge = (mb_type & (MB_TYPE_16x16 | (MB_TYPE_16x8 << dir))) ? 3 :
                              (mb_type & (MB_TYPE_8x16 >> dir)) ? 1 : 0;
        // how often to recheck mv-based bS when iterating along each edge
        const int mask_par0 = mb_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir));

        if (first_vertical_edge_done) {
            start = 1;
            first_vertical_edge_done = 0;
        }

        if (h->deblocking_filter==2 && h->slice_table[mbm_xy] != h->slice_table[mb_xy])
            start = 1;

        if (FRAME_MBAFF && (dir == 1) && ((mb_y&1) == 0) && start == 0
            && !IS_INTERLACED(mb_type)
            && IS_INTERLACED(mbm_type)
            ) {
            // This is a special case in the norm where the filtering must
            // be done twice (one each of the field) even if we are in a
            // frame macroblock.
            //
            static const int nnz_idx[4] = {4,5,6,3};
            unsigned int tmp_linesize   = 2 *   linesize;
            unsigned int tmp_uvlinesize = 2 * uvlinesize;
            int mbn_xy = mb_xy - 2 * s->mb_stride;
            int qp;
            int i, j;
            int16_t bS[4];

            for(j=0; j<2; j++, mbn_xy += s->mb_stride){
                if( IS_INTRA(mb_type) ||
                    IS_INTRA(s->current_picture.mb_type[mbn_xy]) ) {
                    bS[0] = bS[1] = bS[2] = bS[3] = 3;
                } else {
                    const uint8_t *mbn_nnz = h->non_zero_count[mbn_xy];
                    for( i = 0; i < 4; i++ ) {
                        if( h->non_zero_count_cache[scan8[0]+i] != 0 ||
                            mbn_nnz[nnz_idx[i]] != 0 )
                            bS[i] = 2;
                        else
                            bS[i] = 1;
                    }
                }
                // Do not use s->qscale as luma quantizer because it has not the same
                // value in IPCM macroblocks.
                qp = ( s->current_picture.qscale_table[mb_xy] + s->current_picture.qscale_table[mbn_xy] + 1 ) >> 1;
                tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d ls:%d uvls:%d", mb_x, mb_y, dir, edge, qp, tmp_linesize, tmp_uvlinesize);
                { int i; for (i = 0; i < 4; i++) tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); tprintf(s->avctx, "\n"); }
                filter_mb_edgeh( h, &img_y[j*linesize], tmp_linesize, bS, qp );
                filter_mb_edgech( h, &img_cb[j*uvlinesize], tmp_uvlinesize, bS,
                                  ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                filter_mb_edgech( h, &img_cr[j*uvlinesize], tmp_uvlinesize, bS,
                                  ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
            }

            start = 1;
        }

        /* Calculate bS */
        for( edge = start; edge < edges; edge++ ) {
            /* mbn_xy: neighbor macroblock */
            const int mbn_xy = edge > 0 ? mb_xy : mbm_xy;
            const int mbn_type = s->current_picture.mb_type[mbn_xy];
            int16_t bS[4];
            int qp;

            if( (edge&1) && IS_8x8DCT(mb_type) )
                continue;

            if( IS_INTRA(mb_type) ||
                IS_INTRA(mbn_type) ) {
                int value;
                if (edge == 0) {
                    if (   (!IS_INTERLACED(mb_type) && !IS_INTERLACED(mbm_type))
                        || ((FRAME_MBAFF || (s->picture_structure != PICT_FRAME)) && (dir == 0))
                    ) {
                        value = 4;
                    } else {
                        value = 3;
                    }
                } else {
                    value = 3;
                }
                bS[0] = bS[1] = bS[2] = bS[3] = value;
            } else {
                int i, l;
                int mv_done;

                if( edge & mask_edge ) {
                    bS[0] = bS[1] = bS[2] = bS[3] = 0;
                    mv_done = 1;
                }
                else if( FRAME_MBAFF && IS_INTERLACED(mb_type ^ mbn_type)) {
                    bS[0] = bS[1] = bS[2] = bS[3] = 1;
                    mv_done = 1;
                }
                else if( mask_par0 && (edge || (mbn_type & (MB_TYPE_16x16 | (MB_TYPE_8x16 >> dir)))) ) {
                    int b_idx= 8 + 4 + edge * (dir ? 8:1);
                    int bn_idx= b_idx - (dir ? 8:1);
                    int v = 0;
                    for( l = 0; !v && l < 1 + (h->slice_type == B_TYPE); l++ ) {
                        v |= ref2frm[h->ref_cache[l][b_idx]+2] != ref2frm[h->ref_cache[l][bn_idx]+2] ||
                             FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                             FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit;
                    }
                    bS[0] = bS[1] = bS[2] = bS[3] = v;
                    mv_done = 1;
                }
                else
                    mv_done = 0;

                for( i = 0; i < 4; i++ ) {
                    int x = dir == 0 ? edge : i;
                    int y = dir == 0 ? i    : edge;
                    int b_idx= 8 + 4 + x + 8*y;
                    int bn_idx= b_idx - (dir ? 8:1);

                    if( h->non_zero_count_cache[b_idx] != 0 ||
                        h->non_zero_count_cache[bn_idx] != 0 ) {
                        bS[i] = 2;
                    }
                    else if(!mv_done)
                    {
                        bS[i] = 0;
                        for( l = 0; l < 1 + (h->slice_type == B_TYPE); l++ ) {
                            if( ref2frm[h->ref_cache[l][b_idx]+2] != ref2frm[h->ref_cache[l][bn_idx]+2] ||
                                FFABS( h->mv_cache[l][b_idx][0] - h->mv_cache[l][bn_idx][0] ) >= 4 ||
                                FFABS( h->mv_cache[l][b_idx][1] - h->mv_cache[l][bn_idx][1] ) >= mvy_limit ) {
                                bS[i] = 1;
                                break;
                            }
                        }
                    }
                }

                if(bS[0]+bS[1]+bS[2]+bS[3] == 0)
                    continue;
            }

            /* Filter edge */
            // Do not use s->qscale as luma quantizer because it has not the same
            // value in IPCM macroblocks.
            qp = ( s->current_picture.qscale_table[mb_xy] + s->current_picture.qscale_table[mbn_xy] + 1 ) >> 1;
            //tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d, QPc:%d, QPcn:%d\n", mb_x, mb_y, dir, edge, qp, h->chroma_qp, s->current_picture.qscale_table[mbn_xy]);
            tprintf(s->avctx, "filter mb:%d/%d dir:%d edge:%d, QPy:%d ls:%d uvls:%d", mb_x, mb_y, dir, edge, qp, linesize, uvlinesize);
            { int i; for (i = 0; i < 4; i++) tprintf(s->avctx, " bS[%d]:%d", i, bS[i]); tprintf(s->avctx, "\n"); }
            if( dir == 0 ) {
                filter_mb_edgev( h, &img_y[4*edge], linesize, bS, qp );
                if( (edge&1) == 0 ) {
                    filter_mb_edgecv( h, &img_cb[2*edge], uvlinesize, bS,
                                      ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                    filter_mb_edgecv( h, &img_cr[2*edge], uvlinesize, bS,
                                      ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                }
            } else {
                filter_mb_edgeh( h, &img_y[4*edge*linesize], linesize, bS, qp );
                if( (edge&1) == 0 ) {
                    filter_mb_edgech( h, &img_cb[2*edge*uvlinesize], uvlinesize, bS,
                                      ( h->chroma_qp[0] + get_chroma_qp( h, 0, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                    filter_mb_edgech( h, &img_cr[2*edge*uvlinesize], uvlinesize, bS,
                                      ( h->chroma_qp[1] + get_chroma_qp( h, 1, s->current_picture.qscale_table[mbn_xy] ) + 1 ) >> 1);
                }
            }
        }
    }
}

static int decode_end(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    MpegEncContext *s = &h->s;

    av_freep(&h->rbsp_buffer[0]);
    av_freep(&h->rbsp_buffer[1]);
    free_tables(h); //FIXME cleanup init stuff perhaps
    MPV_common_end(s);

//    memset(h, 0, sizeof(H264Context));

    return 0;
}

static void svq3_luma_dc_dequant_idct_c(DCTELEM *block, int qp){
    const int qmul= svq3_dequant_coeff[qp];
#define stride 16
    int i;
    int temp[16];
    static const int x_offset[4]={0, 1*stride, 4* stride,  5*stride};
    static const int y_offset[4]={0, 2*stride, 8* stride, 10*stride};

    for(i=0; i<4; i++){
        const int offset= y_offset[i];
        const int z0= 13*(block[offset+stride*0] +    block[offset+stride*4]);
        const int z1= 13*(block[offset+stride*0] -    block[offset+stride*4]);
        const int z2=  7* block[offset+stride*1] - 17*block[offset+stride*5];
        const int z3= 17* block[offset+stride*1] +  7*block[offset+stride*5];

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z1+z2;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z0-z3;
    }

    for(i=0; i<4; i++){
        const int offset= x_offset[i];
        const int z0= 13*(temp[4*0+i] +    temp[4*2+i]);
        const int z1= 13*(temp[4*0+i] -    temp[4*2+i]);
        const int z2=  7* temp[4*1+i] - 17*temp[4*3+i];
        const int z3= 17* temp[4*1+i] +  7*temp[4*3+i];

        block[stride*0 +offset]= ((z0 + z3)*qmul + 0x80000)>>20;
        block[stride*2 +offset]= ((z1 + z2)*qmul + 0x80000)>>20;
        block[stride*8 +offset]= ((z1 - z2)*qmul + 0x80000)>>20;
        block[stride*10+offset]= ((z0 - z3)*qmul + 0x80000)>>20;
    }
}
#undef stride

static void svq3_add_idct_c (uint8_t *dst, DCTELEM *block, int stride, int qp, int dc){
    const int qmul= svq3_dequant_coeff[qp];
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    if (dc) {
        dc = 13*13*((dc == 1) ? 1538*block[0] : ((qmul*(block[0] >> 3)) / 2));
        block[0] = 0;
    }

    for (i=0; i < 4; i++) {
        const int z0= 13*(block[0 + 4*i] +    block[2 + 4*i]);
        const int z1= 13*(block[0 + 4*i] -    block[2 + 4*i]);
        const int z2=  7* block[1 + 4*i] - 17*block[3 + 4*i];
        const int z3= 17* block[1 + 4*i] +  7*block[3 + 4*i];

        block[0 + 4*i]= z0 + z3;
        block[1 + 4*i]= z1 + z2;
        block[2 + 4*i]= z1 - z2;
        block[3 + 4*i]= z0 - z3;
    }

    for (i=0; i < 4; i++) {
        const int z0= 13*(block[i + 4*0] +    block[i + 4*2]);
        const int z1= 13*(block[i + 4*0] -    block[i + 4*2]);
        const int z2=  7* block[i + 4*1] - 17*block[i + 4*3];
        const int z3= 17* block[i + 4*1] +  7*block[i + 4*3];
        const int rr= (dc + 0x80000);

        dst[i + stride*0]= cm[ dst[i + stride*0] + (((z0 + z3)*qmul + rr) >> 20) ];
        dst[i + stride*1]= cm[ dst[i + stride*1] + (((z1 + z2)*qmul + rr) >> 20) ];
        dst[i + stride*2]= cm[ dst[i + stride*2] + (((z1 - z2)*qmul + rr) >> 20) ];
        dst[i + stride*3]= cm[ dst[i + stride*3] + (((z0 - z3)*qmul + rr) >> 20) ];
    }
}

static inline int svq3_decode_block (GetBitContext *gb, DCTELEM *block,
                                     int index, const int type) {

  static const uint8_t *const scan_patterns[4] =
  { luma_dc_zigzag_scan, zigzag_scan, svq3_scan, chroma_dc_scan };

  int run, level, sign, vlc, limit;
  const int intra = (3 * type) >> 2;
  const uint8_t *const scan = scan_patterns[type];

  for (limit=(16 >> intra); index < 16; index=limit, limit+=8) {
    for (; (vlc = svq3_get_ue_golomb (gb)) != 0; index++) {

      if (vlc == INVALID_VLC)
        return -1;

      sign = (vlc & 0x1) - 1;
      vlc  = (vlc + 1) >> 1;

      if (type == 3) {
        if (vlc < 3) {
          run   = 0;
          level = vlc;
        } else if (vlc < 4) {
          run   = 1;
          level = 1;
        } else {
          run   = (vlc & 0x3);
          level = ((vlc + 9) >> 2) - run;
        }
      } else {
        if (vlc < 16) {
          run   = svq3_dct_tables[intra][vlc].run;
          level = svq3_dct_tables[intra][vlc].level;
        } else if (intra) {
          run   = (vlc & 0x7);
          level = (vlc >> 3) + ((run == 0) ? 8 : ((run < 2) ? 2 : ((run < 5) ? 0 : -1)));
        } else {
          run   = (vlc & 0xF);
          level = (vlc >> 4) + ((run == 0) ? 4 : ((run < 3) ? 2 : ((run < 10) ? 1 : 0)));
        }
      }

      if ((index += run) >= limit)
        return -1;

      block[scan[index]] = (level ^ sign) - sign;
    }

    if (type != 2) {
      break;
    }
  }

  return 0;
}

static inline void svq3_mc_dir_part (MpegEncContext *s,
                                     int x, int y, int width, int height,
                                     int mx, int my, int dxy,
                                     int thirdpel, int dir, int avg) {

  const Picture *pic = (dir == 0) ? &s->last_picture : &s->next_picture;
  uint8_t *src, *dest;
  int i, emu = 0;
  int blocksize= 2 - (width>>3); //16->0, 8->1, 4->2

  mx += x;
  my += y;

  if (mx < 0 || mx >= (s->h_edge_pos - width  - 1) ||
      my < 0 || my >= (s->v_edge_pos - height - 1)) {

    if ((s->flags & CODEC_FLAG_EMU_EDGE)) {
      emu = 1;
    }

    mx = av_clip (mx, -16, (s->h_edge_pos - width  + 15));
    my = av_clip (my, -16, (s->v_edge_pos - height + 15));
  }

  /* form component predictions */
  dest = s->current_picture.data[0] + x + y*s->linesize;
  src  = pic->data[0] + mx + my*s->linesize;

  if (emu) {
    ff_emulated_edge_mc (s->edge_emu_buffer, src, s->linesize, (width + 1), (height + 1),
                         mx, my, s->h_edge_pos, s->v_edge_pos);
    src = s->edge_emu_buffer;
  }
  if(thirdpel)
    (avg ? s->dsp.avg_tpel_pixels_tab : s->dsp.put_tpel_pixels_tab)[dxy](dest, src, s->linesize, width, height);
  else
    (avg ? s->dsp.avg_pixels_tab : s->dsp.put_pixels_tab)[blocksize][dxy](dest, src, s->linesize, height);

  if (!(s->flags & CODEC_FLAG_GRAY)) {
    mx     = (mx + (mx < (int) x)) >> 1;
    my     = (my + (my < (int) y)) >> 1;
    width  = (width  >> 1);
    height = (height >> 1);
    blocksize++;

    for (i=1; i < 3; i++) {
      dest = s->current_picture.data[i] + (x >> 1) + (y >> 1)*s->uvlinesize;
      src  = pic->data[i] + mx + my*s->uvlinesize;

      if (emu) {
        ff_emulated_edge_mc (s->edge_emu_buffer, src, s->uvlinesize, (width + 1), (height + 1),
                             mx, my, (s->h_edge_pos >> 1), (s->v_edge_pos >> 1));
        src = s->edge_emu_buffer;
      }
      if(thirdpel)
        (avg ? s->dsp.avg_tpel_pixels_tab : s->dsp.put_tpel_pixels_tab)[dxy](dest, src, s->uvlinesize, width, height);
      else
        (avg ? s->dsp.avg_pixels_tab : s->dsp.put_pixels_tab)[blocksize][dxy](dest, src, s->uvlinesize, height);
    }
  }
}

static inline int svq3_mc_dir (H264Context *h, int size, int mode, int dir, int avg) {

  int i, j, k, mx, my, dx, dy, x, y;
  MpegEncContext *const s = (MpegEncContext *) h;
  const int part_width  = ((size & 5) == 4) ? 4 : 16 >> (size & 1);
  const int part_height = 16 >> ((unsigned) (size + 1) / 3);
  const int extra_width = (mode == PREDICT_MODE) ? -16*6 : 0;
  const int h_edge_pos  = 6*(s->h_edge_pos - part_width ) - extra_width;
  const int v_edge_pos  = 6*(s->v_edge_pos - part_height) - extra_width;

  for (i=0; i < 16; i+=part_height) {
    for (j=0; j < 16; j+=part_width) {
      const int b_xy = (4*s->mb_x+(j>>2)) + (4*s->mb_y+(i>>2))*h->b_stride;
      int dxy;
      x = 16*s->mb_x + j;
      y = 16*s->mb_y + i;
      k = ((j>>2)&1) + ((i>>1)&2) + ((j>>1)&4) + (i&8);

      if (mode != PREDICT_MODE) {
        pred_motion (h, k, (part_width >> 2), dir, 1, &mx, &my);
      } else {
        mx = s->next_picture.motion_val[0][b_xy][0]<<1;
        my = s->next_picture.motion_val[0][b_xy][1]<<1;

        if (dir == 0) {
          mx = ((mx * h->frame_num_offset) / h->prev_frame_num_offset + 1)>>1;
          my = ((my * h->frame_num_offset) / h->prev_frame_num_offset + 1)>>1;
        } else {
          mx = ((mx * (h->frame_num_offset - h->prev_frame_num_offset)) / h->prev_frame_num_offset + 1)>>1;
          my = ((my * (h->frame_num_offset - h->prev_frame_num_offset)) / h->prev_frame_num_offset + 1)>>1;
        }
      }

      /* clip motion vector prediction to frame border */
      mx = av_clip (mx, extra_width - 6*x, h_edge_pos - 6*x);
      my = av_clip (my, extra_width - 6*y, v_edge_pos - 6*y);

      /* get (optional) motion vector differential */
      if (mode == PREDICT_MODE) {
        dx = dy = 0;
      } else {
        dy = svq3_get_se_golomb (&s->gb);
        dx = svq3_get_se_golomb (&s->gb);

        if (dx == INVALID_VLC || dy == INVALID_VLC) {
          av_log(h->s.avctx, AV_LOG_ERROR, "invalid MV vlc\n");
          return -1;
        }
      }

      /* compute motion vector */
      if (mode == THIRDPEL_MODE) {
        int fx, fy;
        mx = ((mx + 1)>>1) + dx;
        my = ((my + 1)>>1) + dy;
        fx= ((unsigned)(mx + 0x3000))/3 - 0x1000;
        fy= ((unsigned)(my + 0x3000))/3 - 0x1000;
        dxy= (mx - 3*fx) + 4*(my - 3*fy);

        svq3_mc_dir_part (s, x, y, part_width, part_height, fx, fy, dxy, 1, dir, avg);
        mx += mx;
        my += my;
      } else if (mode == HALFPEL_MODE || mode == PREDICT_MODE) {
        mx = ((unsigned)(mx + 1 + 0x3000))/3 + dx - 0x1000;
        my = ((unsigned)(my + 1 + 0x3000))/3 + dy - 0x1000;
        dxy= (mx&1) + 2*(my&1);

        svq3_mc_dir_part (s, x, y, part_width, part_height, mx>>1, my>>1, dxy, 0, dir, avg);
        mx *= 3;
        my *= 3;
      } else {
        mx = ((unsigned)(mx + 3 + 0x6000))/6 + dx - 0x1000;
        my = ((unsigned)(my + 3 + 0x6000))/6 + dy - 0x1000;

        svq3_mc_dir_part (s, x, y, part_width, part_height, mx, my, 0, 0, dir, avg);
        mx *= 6;
        my *= 6;
      }

      /* update mv_cache */
      if (mode != PREDICT_MODE) {
        int32_t mv = pack16to32(mx,my);

        if (part_height == 8 && i < 8) {
          *(int32_t *) h->mv_cache[dir][scan8[k] + 1*8] = mv;

          if (part_width == 8 && j < 8) {
            *(int32_t *) h->mv_cache[dir][scan8[k] + 1 + 1*8] = mv;
          }
        }
        if (part_width == 8 && j < 8) {
          *(int32_t *) h->mv_cache[dir][scan8[k] + 1] = mv;
        }
        if (part_width == 4 || part_height == 4) {
          *(int32_t *) h->mv_cache[dir][scan8[k]] = mv;
        }
      }

      /* write back motion vectors */
      fill_rectangle(s->current_picture.motion_val[dir][b_xy], part_width>>2, part_height>>2, h->b_stride, pack16to32(mx,my), 4);
    }
  }

  return 0;
}

static int svq3_decode_mb (H264Context *h, unsigned int mb_type) {
  int i, j, k, m, dir, mode;
  int cbp = 0;
  uint32_t vlc;
  int8_t *top, *left;
  MpegEncContext *const s = (MpegEncContext *) h;
  const int mb_xy = s->mb_x + s->mb_y*s->mb_stride;
  const int b_xy = 4*s->mb_x + 4*s->mb_y*h->b_stride;

  h->top_samples_available        = (s->mb_y == 0) ? 0x33FF : 0xFFFF;
  h->left_samples_available        = (s->mb_x == 0) ? 0x5F5F : 0xFFFF;
  h->topright_samples_available        = 0xFFFF;

  if (mb_type == 0) {           /* SKIP */
    if (s->pict_type == P_TYPE || s->next_picture.mb_type[mb_xy] == -1) {
      svq3_mc_dir_part (s, 16*s->mb_x, 16*s->mb_y, 16, 16, 0, 0, 0, 0, 0, 0);

      if (s->pict_type == B_TYPE) {
        svq3_mc_dir_part (s, 16*s->mb_x, 16*s->mb_y, 16, 16, 0, 0, 0, 0, 1, 1);
      }

      mb_type = MB_TYPE_SKIP;
    } else {
      mb_type= FFMIN(s->next_picture.mb_type[mb_xy], 6);
      if(svq3_mc_dir (h, mb_type, PREDICT_MODE, 0, 0) < 0)
        return -1;
      if(svq3_mc_dir (h, mb_type, PREDICT_MODE, 1, 1) < 0)
        return -1;

      mb_type = MB_TYPE_16x16;
    }
  } else if (mb_type < 8) {     /* INTER */
    if (h->thirdpel_flag && h->halfpel_flag == !get_bits1 (&s->gb)) {
      mode = THIRDPEL_MODE;
    } else if (h->halfpel_flag && h->thirdpel_flag == !get_bits1 (&s->gb)) {
      mode = HALFPEL_MODE;
    } else {
      mode = FULLPEL_MODE;
    }

    /* fill caches */
    /* note ref_cache should contain here:
        ????????
        ???11111
        N??11111
        N??11111
        N??11111
    */

    for (m=0; m < 2; m++) {
      if (s->mb_x > 0 && h->intra4x4_pred_mode[mb_xy - 1][0] != -1) {
        for (i=0; i < 4; i++) {
          *(uint32_t *) h->mv_cache[m][scan8[0] - 1 + i*8] = *(uint32_t *) s->current_picture.motion_val[m][b_xy - 1 + i*h->b_stride];
        }
      } else {
        for (i=0; i < 4; i++) {
          *(uint32_t *) h->mv_cache[m][scan8[0] - 1 + i*8] = 0;
        }
      }
      if (s->mb_y > 0) {
        memcpy (h->mv_cache[m][scan8[0] - 1*8], s->current_picture.motion_val[m][b_xy - h->b_stride], 4*2*sizeof(int16_t));
        memset (&h->ref_cache[m][scan8[0] - 1*8], (h->intra4x4_pred_mode[mb_xy - s->mb_stride][4] == -1) ? PART_NOT_AVAILABLE : 1, 4);

        if (s->mb_x < (s->mb_width - 1)) {
          *(uint32_t *) h->mv_cache[m][scan8[0] + 4 - 1*8] = *(uint32_t *) s->current_picture.motion_val[m][b_xy - h->b_stride + 4];
          h->ref_cache[m][scan8[0] + 4 - 1*8] =
                  (h->intra4x4_pred_mode[mb_xy - s->mb_stride + 1][0] == -1 ||
                   h->intra4x4_pred_mode[mb_xy - s->mb_stride][4] == -1) ? PART_NOT_AVAILABLE : 1;
        }else
          h->ref_cache[m][scan8[0] + 4 - 1*8] = PART_NOT_AVAILABLE;
        if (s->mb_x > 0) {
          *(uint32_t *) h->mv_cache[m][scan8[0] - 1 - 1*8] = *(uint32_t *) s->current_picture.motion_val[m][b_xy - h->b_stride - 1];
          h->ref_cache[m][scan8[0] - 1 - 1*8] = (h->intra4x4_pred_mode[mb_xy - s->mb_stride - 1][3] == -1) ? PART_NOT_AVAILABLE : 1;
        }else
          h->ref_cache[m][scan8[0] - 1 - 1*8] = PART_NOT_AVAILABLE;
      }else
        memset (&h->ref_cache[m][scan8[0] - 1*8 - 1], PART_NOT_AVAILABLE, 8);

      if (s->pict_type != B_TYPE)
        break;
    }

    /* decode motion vector(s) and form prediction(s) */
    if (s->pict_type == P_TYPE) {
      if(svq3_mc_dir (h, (mb_type - 1), mode, 0, 0) < 0)
        return -1;
    } else {        /* B_TYPE */
      if (mb_type != 2) {
        if(svq3_mc_dir (h, 0, mode, 0, 0) < 0)
          return -1;
      } else {
        for (i=0; i < 4; i++) {
          memset (s->current_picture.motion_val[0][b_xy + i*h->b_stride], 0, 4*2*sizeof(int16_t));
        }
      }
      if (mb_type != 1) {
        if(svq3_mc_dir (h, 0, mode, 1, (mb_type == 3)) < 0)
          return -1;
      } else {
        for (i=0; i < 4; i++) {
          memset (s->current_picture.motion_val[1][b_xy + i*h->b_stride], 0, 4*2*sizeof(int16_t));
        }
      }
    }

    mb_type = MB_TYPE_16x16;
  } else if (mb_type == 8 || mb_type == 33) {   /* INTRA4x4 */
    memset (h->intra4x4_pred_mode_cache, -1, 8*5*sizeof(int8_t));

    if (mb_type == 8) {
      if (s->mb_x > 0) {
        for (i=0; i < 4; i++) {
          h->intra4x4_pred_mode_cache[scan8[0] - 1 + i*8] = h->intra4x4_pred_mode[mb_xy - 1][i];
        }
        if (h->intra4x4_pred_mode_cache[scan8[0] - 1] == -1) {
          h->left_samples_available = 0x5F5F;
        }
      }
      if (s->mb_y > 0) {
        h->intra4x4_pred_mode_cache[4+8*0] = h->intra4x4_pred_mode[mb_xy - s->mb_stride][4];
        h->intra4x4_pred_mode_cache[5+8*0] = h->intra4x4_pred_mode[mb_xy - s->mb_stride][5];
        h->intra4x4_pred_mode_cache[6+8*0] = h->intra4x4_pred_mode[mb_xy - s->mb_stride][6];
        h->intra4x4_pred_mode_cache[7+8*0] = h->intra4x4_pred_mode[mb_xy - s->mb_stride][3];

        if (h->intra4x4_pred_mode_cache[4+8*0] == -1) {
          h->top_samples_available = 0x33FF;
        }
      }

      /* decode prediction codes for luma blocks */
      for (i=0; i < 16; i+=2) {
        vlc = svq3_get_ue_golomb (&s->gb);

        if (vlc >= 25){
          av_log(h->s.avctx, AV_LOG_ERROR, "luma prediction:%d\n", vlc);
          return -1;
        }

        left    = &h->intra4x4_pred_mode_cache[scan8[i] - 1];
        top     = &h->intra4x4_pred_mode_cache[scan8[i] - 8];

        left[1] = svq3_pred_1[top[0] + 1][left[0] + 1][svq3_pred_0[vlc][0]];
        left[2] = svq3_pred_1[top[1] + 1][left[1] + 1][svq3_pred_0[vlc][1]];

        if (left[1] == -1 || left[2] == -1){
          av_log(h->s.avctx, AV_LOG_ERROR, "weird prediction\n");
          return -1;
        }
      }
    } else {    /* mb_type == 33, DC_128_PRED block type */
      for (i=0; i < 4; i++) {
        memset (&h->intra4x4_pred_mode_cache[scan8[0] + 8*i], DC_PRED, 4);
      }
    }

    write_back_intra_pred_mode (h);

    if (mb_type == 8) {
      check_intra4x4_pred_mode (h);

      h->top_samples_available  = (s->mb_y == 0) ? 0x33FF : 0xFFFF;
      h->left_samples_available = (s->mb_x == 0) ? 0x5F5F : 0xFFFF;
    } else {
      for (i=0; i < 4; i++) {
        memset (&h->intra4x4_pred_mode_cache[scan8[0] + 8*i], DC_128_PRED, 4);
      }

      h->top_samples_available  = 0x33FF;
      h->left_samples_available = 0x5F5F;
    }

    mb_type = MB_TYPE_INTRA4x4;
  } else {                      /* INTRA16x16 */
    dir = i_mb_type_info[mb_type - 8].pred_mode;
    dir = (dir >> 1) ^ 3*(dir & 1) ^ 1;

    if ((h->intra16x16_pred_mode = check_intra_pred_mode (h, dir)) == -1){
      av_log(h->s.avctx, AV_LOG_ERROR, "check_intra_pred_mode = -1\n");
      return -1;
    }

    cbp = i_mb_type_info[mb_type - 8].cbp;
    mb_type = MB_TYPE_INTRA16x16;
  }

  if (!IS_INTER(mb_type) && s->pict_type != I_TYPE) {
    for (i=0; i < 4; i++) {
      memset (s->current_picture.motion_val[0][b_xy + i*h->b_stride], 0, 4*2*sizeof(int16_t));
    }
    if (s->pict_type == B_TYPE) {
      for (i=0; i < 4; i++) {
        memset (s->current_picture.motion_val[1][b_xy + i*h->b_stride], 0, 4*2*sizeof(int16_t));
      }
    }
  }
  if (!IS_INTRA4x4(mb_type)) {
    memset (h->intra4x4_pred_mode[mb_xy], DC_PRED, 8);
  }
  if (!IS_SKIP(mb_type) || s->pict_type == B_TYPE) {
    memset (h->non_zero_count_cache + 8, 0, 4*9*sizeof(uint8_t));
    s->dsp.clear_blocks(h->mb);
  }

  if (!IS_INTRA16x16(mb_type) && (!IS_SKIP(mb_type) || s->pict_type == B_TYPE)) {
    if ((vlc = svq3_get_ue_golomb (&s->gb)) >= 48){
      av_log(h->s.avctx, AV_LOG_ERROR, "cbp_vlc=%d\n", vlc);
      return -1;
    }

    cbp = IS_INTRA(mb_type) ? golomb_to_intra4x4_cbp[vlc] : golomb_to_inter_cbp[vlc];
  }
  if (IS_INTRA16x16(mb_type) || (s->pict_type != I_TYPE && s->adaptive_quant && cbp)) {
    s->qscale += svq3_get_se_golomb (&s->gb);

    if (s->qscale > 31){
      av_log(h->s.avctx, AV_LOG_ERROR, "qscale:%d\n", s->qscale);
      return -1;
    }
  }
  if (IS_INTRA16x16(mb_type)) {
    if (svq3_decode_block (&s->gb, h->mb, 0, 0)){
      av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding intra luma dc\n");
      return -1;
    }
  }

  if (cbp) {
    const int index = IS_INTRA16x16(mb_type) ? 1 : 0;
    const int type = ((s->qscale < 24 && IS_INTRA4x4(mb_type)) ? 2 : 1);

    for (i=0; i < 4; i++) {
      if ((cbp & (1 << i))) {
        for (j=0; j < 4; j++) {
          k = index ? ((j&1) + 2*(i&1) + 2*(j&2) + 4*(i&2)) : (4*i + j);
          h->non_zero_count_cache[ scan8[k] ] = 1;

          if (svq3_decode_block (&s->gb, &h->mb[16*k], index, type)){
            av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding block\n");
            return -1;
          }
        }
      }
    }

    if ((cbp & 0x30)) {
      for (i=0; i < 2; ++i) {
        if (svq3_decode_block (&s->gb, &h->mb[16*(16 + 4*i)], 0, 3)){
          av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding chroma dc block\n");
          return -1;
        }
      }

      if ((cbp & 0x20)) {
        for (i=0; i < 8; i++) {
          h->non_zero_count_cache[ scan8[16+i] ] = 1;

          if (svq3_decode_block (&s->gb, &h->mb[16*(16 + i)], 1, 1)){
            av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding chroma ac block\n");
            return -1;
          }
        }
      }
    }
  }

  s->current_picture.mb_type[mb_xy] = mb_type;

  if (IS_INTRA(mb_type)) {
    h->chroma_pred_mode = check_intra_pred_mode (h, DC_PRED8x8);
  }

  return 0;
}

static int svq3_decode_slice_header (H264Context *h) {
  MpegEncContext *const s = (MpegEncContext *) h;
  const int mb_xy = s->mb_x + s->mb_y*s->mb_stride;
  int i, header;

  header = get_bits (&s->gb, 8);

  if (((header & 0x9F) != 1 && (header & 0x9F) != 2) || (header & 0x60) == 0) {
    /* TODO: what? */
    av_log(h->s.avctx, AV_LOG_ERROR, "unsupported slice header (%02X)\n", header);
    return -1;
  } else {
    int length = (header >> 5) & 3;

    h->next_slice_index = get_bits_count(&s->gb) + 8*show_bits (&s->gb, 8*length) + 8*length;

    if (h->next_slice_index > s->gb.size_in_bits){
      av_log(h->s.avctx, AV_LOG_ERROR, "slice after bitstream end\n");
      return -1;
    }

    s->gb.size_in_bits = h->next_slice_index - 8*(length - 1);
    skip_bits(&s->gb, 8);

    if (length > 0) {
      memcpy ((uint8_t *) &s->gb.buffer[get_bits_count(&s->gb) >> 3],
             &s->gb.buffer[s->gb.size_in_bits >> 3], (length - 1));
    }
  }

  if ((i = svq3_get_ue_golomb (&s->gb)) == INVALID_VLC || i >= 3){
    av_log(h->s.avctx, AV_LOG_ERROR, "illegal slice type %d \n", i);
    return -1;
  }

  h->slice_type = golomb_to_pict_type[i];

  if ((header & 0x9F) == 2) {
    i = (s->mb_num < 64) ? 6 : (1 + av_log2 (s->mb_num - 1));
    s->mb_skip_run = get_bits (&s->gb, i) - (s->mb_x + (s->mb_y * s->mb_width));
  } else {
    skip_bits1 (&s->gb);
    s->mb_skip_run = 0;
  }

  h->slice_num = get_bits (&s->gb, 8);
  s->qscale = get_bits (&s->gb, 5);
  s->adaptive_quant = get_bits1 (&s->gb);

  /* unknown fields */
  skip_bits1 (&s->gb);

  if (h->unknown_svq3_flag) {
    skip_bits1 (&s->gb);
  }

  skip_bits1 (&s->gb);
  skip_bits (&s->gb, 2);

  while (get_bits1 (&s->gb)) {
    skip_bits (&s->gb, 8);
  }

  /* reset intra predictors and invalidate motion vector references */
  if (s->mb_x > 0) {
    memset (h->intra4x4_pred_mode[mb_xy - 1], -1, 4*sizeof(int8_t));
    memset (h->intra4x4_pred_mode[mb_xy - s->mb_x], -1, 8*sizeof(int8_t)*s->mb_x);
  }
  if (s->mb_y > 0) {
    memset (h->intra4x4_pred_mode[mb_xy - s->mb_stride], -1, 8*sizeof(int8_t)*(s->mb_width - s->mb_x));

    if (s->mb_x > 0) {
      h->intra4x4_pred_mode[mb_xy - s->mb_stride - 1][3] = -1;
    }
  }

  return 0;
}

static int svq3_decode_frame (AVCodecContext *avctx,
                              void *data, int *data_size,
                              uint8_t *buf, int buf_size) {
  MpegEncContext *const s = avctx->priv_data;
  H264Context *const h = avctx->priv_data;
  int m, mb_type;
  unsigned char *extradata;
  unsigned int size;

  s->flags = avctx->flags;
  s->flags2 = avctx->flags2;
  s->unrestricted_mv = 1;

  if (!s->context_initialized) {
    s->width = avctx->width;
    s->height = avctx->height;
    h->halfpel_flag = 1;
    h->thirdpel_flag = 1;
    h->unknown_svq3_flag = 0;
    h->chroma_qp[0] = h->chroma_qp[1] = 4;

    if (MPV_common_init (s) < 0)
      return -1;

    h->b_stride = 4*s->mb_width;

    alloc_tables (h);

    /* prowl for the "SEQH" marker in the extradata */
    extradata = (unsigned char *)avctx->extradata;
    for (m = 0; m < avctx->extradata_size; m++) {
      if (!memcmp (extradata, "SEQH", 4))
        break;
      extradata++;
    }

    /* if a match was found, parse the extra data */
    if (extradata && !memcmp (extradata, "SEQH", 4)) {

      GetBitContext gb;

      size = AV_RB32(&extradata[4]);
      init_get_bits (&gb, extradata + 8, size*8);

      /* 'frame size code' and optional 'width, height' */
      if (get_bits (&gb, 3) == 7) {
        skip_bits (&gb, 12);
        skip_bits (&gb, 12);
      }

      h->halfpel_flag = get_bits1 (&gb);
      h->thirdpel_flag = get_bits1 (&gb);

      /* unknown fields */
      skip_bits1 (&gb);
      skip_bits1 (&gb);
      skip_bits1 (&gb);
      skip_bits1 (&gb);

      s->low_delay = get_bits1 (&gb);

      /* unknown field */
      skip_bits1 (&gb);

      while (get_bits1 (&gb)) {
        skip_bits (&gb, 8);
      }

      h->unknown_svq3_flag = get_bits1 (&gb);
      avctx->has_b_frames = !s->low_delay;
    }
  }

  /* special case for last picture */
  if (buf_size == 0) {
    if (s->next_picture_ptr && !s->low_delay) {
      *(AVFrame *) data = *(AVFrame *) &s->next_picture;
      *data_size = sizeof(AVFrame);
    }
    return 0;
  }

  init_get_bits (&s->gb, buf, 8*buf_size);

  s->mb_x = s->mb_y = 0;

  if (svq3_decode_slice_header (h))
    return -1;

  s->pict_type = h->slice_type;
  s->picture_number = h->slice_num;

  if(avctx->debug&FF_DEBUG_PICT_INFO){
      av_log(h->s.avctx, AV_LOG_DEBUG, "%c hpel:%d, tpel:%d aqp:%d qp:%d\n",
      av_get_pict_type_char(s->pict_type), h->halfpel_flag, h->thirdpel_flag,
      s->adaptive_quant, s->qscale
      );
  }

  /* for hurry_up==5 */
  s->current_picture.pict_type = s->pict_type;
  s->current_picture.key_frame = (s->pict_type == I_TYPE);

  /* Skip B-frames if we do not have reference frames. */
  if (s->last_picture_ptr == NULL && s->pict_type == B_TYPE) return 0;
  /* Skip B-frames if we are in a hurry. */
  if (avctx->hurry_up && s->pict_type == B_TYPE) return 0;
  /* Skip everything if we are in a hurry >= 5. */
  if (avctx->hurry_up >= 5) return 0;
  if(  (avctx->skip_frame >= AVDISCARD_NONREF && s->pict_type==B_TYPE)
     ||(avctx->skip_frame >= AVDISCARD_NONKEY && s->pict_type!=I_TYPE)
     || avctx->skip_frame >= AVDISCARD_ALL)
      return 0;

  if (s->next_p_frame_damaged) {
    if (s->pict_type == B_TYPE)
      return 0;
    else
      s->next_p_frame_damaged = 0;
  }

  if (frame_start (h) < 0)
    return -1;

  if (s->pict_type == B_TYPE) {
    h->frame_num_offset = (h->slice_num - h->prev_frame_num);

    if (h->frame_num_offset < 0) {
      h->frame_num_offset += 256;
    }
    if (h->frame_num_offset == 0 || h->frame_num_offset >= h->prev_frame_num_offset) {
      av_log(h->s.avctx, AV_LOG_ERROR, "error in B-frame picture id\n");
      return -1;
    }
  } else {
    h->prev_frame_num = h->frame_num;
    h->frame_num = h->slice_num;
    h->prev_frame_num_offset = (h->frame_num - h->prev_frame_num);

    if (h->prev_frame_num_offset < 0) {
      h->prev_frame_num_offset += 256;
    }
  }

  for(m=0; m<2; m++){
    int i;
    for(i=0; i<4; i++){
      int j;
      for(j=-1; j<4; j++)
        h->ref_cache[m][scan8[0] + 8*i + j]= 1;
      if(i<3)
        h->ref_cache[m][scan8[0] + 8*i + j]= PART_NOT_AVAILABLE;
    }
  }

  for (s->mb_y=0; s->mb_y < s->mb_height; s->mb_y++) {
    for (s->mb_x=0; s->mb_x < s->mb_width; s->mb_x++) {

      if ( (get_bits_count(&s->gb) + 7) >= s->gb.size_in_bits &&
          ((get_bits_count(&s->gb) & 7) == 0 || show_bits (&s->gb, (-get_bits_count(&s->gb) & 7)) == 0)) {

        skip_bits(&s->gb, h->next_slice_index - get_bits_count(&s->gb));
        s->gb.size_in_bits = 8*buf_size;

        if (svq3_decode_slice_header (h))
          return -1;

        /* TODO: support s->mb_skip_run */
      }

      mb_type = svq3_get_ue_golomb (&s->gb);

      if (s->pict_type == I_TYPE) {
        mb_type += 8;
      } else if (s->pict_type == B_TYPE && mb_type >= 4) {
        mb_type += 4;
      }
      if (mb_type > 33 || svq3_decode_mb (h, mb_type)) {
        av_log(h->s.avctx, AV_LOG_ERROR, "error while decoding MB %d %d\n", s->mb_x, s->mb_y);
        return -1;
      }

      if (mb_type != 0) {
        hl_decode_mb (h);
      }

      if (s->pict_type != B_TYPE && !s->low_delay) {
        s->current_picture.mb_type[s->mb_x + s->mb_y*s->mb_stride] =
                        (s->pict_type == P_TYPE && mb_type < 8) ? (mb_type - 1) : -1;
      }
    }

    ff_draw_horiz_band(s, 16*s->mb_y, 16);
  }

#ifdef JZC_CRC_VER
    {
      int crc_i;
      for(crc_i=0;crc_i<avctx->height;crc_i++)
	crc_code = crc(s->current_picture.data[0]+crc_i*s->linesize, 
		       avctx->width, crc_code);
      for(crc_i=0;crc_i<(avctx->height>>1);crc_i++){
	crc_code = crc(s->current_picture.data[1]+crc_i*s->uvlinesize, 
		       (avctx->width>>1), crc_code);
	crc_code = crc(s->current_picture.data[2]+crc_i*s->uvlinesize, 
		       (avctx->width>>1), crc_code);
      }
      mp_msg(NULL,NULL,"frame: %d, crc_code: 0x%x\n", mpFrame, crc_code);
      mpFrame ++;
    }
#endif //JZC_CRC_VER

  MPV_frame_end(s);

  if (s->pict_type == B_TYPE || s->low_delay) {
    *(AVFrame *) data = *(AVFrame *) &s->current_picture;
  } else {
    *(AVFrame *) data = *(AVFrame *) &s->last_picture;
  }

  avctx->frame_number = s->picture_number - 1;

  /* Do not output the last pic after seeking. */
  if (s->last_picture_ptr || s->low_delay) {
    *data_size = sizeof(AVFrame);
  }

  return buf_size;
}


AVCodec svq3_decoder = {
    "svq3",
    CODEC_TYPE_VIDEO,
    CODEC_ID_SVQ3,
    sizeof(H264Context),
    decode_init,
    NULL,
    decode_end,
    svq3_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1 | CODEC_CAP_DELAY,
};

