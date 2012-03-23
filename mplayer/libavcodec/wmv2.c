/*
 * Copyright (c) 2002 The FFmpeg Project.
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
 * @file wmv2.c
 * wmv2 codec.
 */

#include "simple_idct.h"
#include "intrax8.h"

#define SKIP_TYPE_NONE 0
#define SKIP_TYPE_MPEG 1
#define SKIP_TYPE_ROW  2
#define SKIP_TYPE_COL  3

typedef struct Wmv2Context{
    MpegEncContext s;
    IntraX8Context x8;
    int j_type_bit;
    int j_type;
    int abt_flag;
    int abt_type;
    int abt_type_table[6];
    int per_mb_abt;
    int per_block_abt;
    int mspel_bit;
    int cbp_table_index;
    int top_left_mv_flag;
    int per_mb_rl_bit;
    int skip_type;
    int hshift;

    ScanTable abt_scantable[2];
    DECLARE_ALIGNED_8(DCTELEM, abt_block2[6][64]);
}Wmv2Context;

static void wmv2_common_init(Wmv2Context * w){
    MpegEncContext * const s= &w->s;

    ff_init_scantable(s->dsp.idct_permutation, &w->abt_scantable[0], wmv2_scantableA);
    ff_init_scantable(s->dsp.idct_permutation, &w->abt_scantable[1], wmv2_scantableB);
}

#ifdef CONFIG_WMV2_ENCODER

static int encode_ext_header(Wmv2Context *w){
    MpegEncContext * const s= &w->s;
    PutBitContext pb;
    int code;

    init_put_bits(&pb, s->avctx->extradata, s->avctx->extradata_size);

    put_bits(&pb, 5, s->avctx->time_base.den / s->avctx->time_base.num); //yes 29.97 -> 29
    put_bits(&pb, 11, FFMIN(s->bit_rate/1024, 2047));

    put_bits(&pb, 1, w->mspel_bit=1);
    put_bits(&pb, 1, s->loop_filter);
    put_bits(&pb, 1, w->abt_flag=1);
    put_bits(&pb, 1, w->j_type_bit=1);
    put_bits(&pb, 1, w->top_left_mv_flag=0);
    put_bits(&pb, 1, w->per_mb_rl_bit=1);
    put_bits(&pb, 3, code=1);

    flush_put_bits(&pb);

    s->slice_height = s->mb_height / code;

    return 0;
}

static int wmv2_encode_init(AVCodecContext *avctx){
    Wmv2Context * const w= avctx->priv_data;

    if(MPV_encode_init(avctx) < 0)
        return -1;

    wmv2_common_init(w);

    avctx->extradata_size= 4;
    avctx->extradata= av_mallocz(avctx->extradata_size + 10);
    encode_ext_header(w);

    return 0;
}

#if 0 /* unused, remove? */
static int wmv2_encode_end(AVCodecContext *avctx){

    if(MPV_encode_end(avctx) < 0)
        return -1;

    avctx->extradata_size= 0;
    av_freep(&avctx->extradata);

    return 0;
}
#endif

int ff_wmv2_encode_picture_header(MpegEncContext * s, int picture_number)
{
    Wmv2Context * const w= (Wmv2Context*)s;

    put_bits(&s->pb, 1, s->pict_type - 1);
    if(s->pict_type == I_TYPE){
        put_bits(&s->pb, 7, 0);
    }
    put_bits(&s->pb, 5, s->qscale);

    s->dc_table_index = 1;
    s->mv_table_index = 1; /* only if P frame */
//    s->use_skip_mb_code = 1; /* only if P frame */
    s->per_mb_rl_table = 0;
    s->mspel= 0;
    w->per_mb_abt=0;
    w->abt_type=0;
    w->j_type=0;

    assert(s->flipflop_rounding);

    if (s->pict_type == I_TYPE) {
        assert(s->no_rounding==1);
        if(w->j_type_bit) put_bits(&s->pb, 1, w->j_type);

        if(w->per_mb_rl_bit) put_bits(&s->pb, 1, s->per_mb_rl_table);

        if(!s->per_mb_rl_table){
            ff_code012(&s->pb, s->rl_chroma_table_index);
            ff_code012(&s->pb, s->rl_table_index);
        }

        put_bits(&s->pb, 1, s->dc_table_index);

        s->inter_intra_pred= 0;
    }else{
        int cbp_index;

        put_bits(&s->pb, 2, SKIP_TYPE_NONE);

        ff_code012(&s->pb, cbp_index=0);
        if(s->qscale <= 10){
            int map[3]= {0,2,1};
            w->cbp_table_index= map[cbp_index];
        }else if(s->qscale <= 20){
            int map[3]= {1,0,2};
            w->cbp_table_index= map[cbp_index];
        }else{
            int map[3]= {2,1,0};
            w->cbp_table_index= map[cbp_index];
        }

        if(w->mspel_bit) put_bits(&s->pb, 1, s->mspel);

        if(w->abt_flag){
            put_bits(&s->pb, 1, w->per_mb_abt^1);
            if(!w->per_mb_abt){
                ff_code012(&s->pb, w->abt_type);
            }
        }

        if(w->per_mb_rl_bit) put_bits(&s->pb, 1, s->per_mb_rl_table);

        if(!s->per_mb_rl_table){
            ff_code012(&s->pb, s->rl_table_index);
            s->rl_chroma_table_index = s->rl_table_index;
        }
        put_bits(&s->pb, 1, s->dc_table_index);
        put_bits(&s->pb, 1, s->mv_table_index);

        s->inter_intra_pred= 0;//(s->width*s->height < 320*240 && s->bit_rate<=II_BITRATE);
    }
    s->esc3_level_length= 0;
    s->esc3_run_length= 0;

    return 0;
}

/* Nearly identical to wmv1 but that is just because we do not use the
 * useless M$ crap features. It is duplicated here in case someone wants
 * to add support for these crap features. */
void ff_wmv2_encode_mb(MpegEncContext * s,
                       DCTELEM block[6][64],
                       int motion_x, int motion_y)
{
    Wmv2Context * const w= (Wmv2Context*)s;
    int cbp, coded_cbp, i;
    int pred_x, pred_y;
    uint8_t *coded_block;

    handle_slices(s);

    if (!s->mb_intra) {
        /* compute cbp */
        cbp = 0;
        for (i = 0; i < 6; i++) {
            if (s->block_last_index[i] >= 0)
                cbp |= 1 << (5 - i);
        }

        put_bits(&s->pb,
                 wmv2_inter_table[w->cbp_table_index][cbp + 64][1],
                 wmv2_inter_table[w->cbp_table_index][cbp + 64][0]);

        /* motion vector */
        h263_pred_motion(s, 0, 0, &pred_x, &pred_y);
        msmpeg4_encode_motion(s, motion_x - pred_x,
                              motion_y - pred_y);
    } else {
        /* compute cbp */
        cbp = 0;
        coded_cbp = 0;
        for (i = 0; i < 6; i++) {
            int val, pred;
            val = (s->block_last_index[i] >= 1);
            cbp |= val << (5 - i);
            if (i < 4) {
                /* predict value for close blocks only for luma */
                pred = coded_block_pred(s, i, &coded_block);
                *coded_block = val;
                val = val ^ pred;
            }
            coded_cbp |= val << (5 - i);
        }
#if 0
        if (coded_cbp)
            printf("cbp=%x %x\n", cbp, coded_cbp);
#endif

        if (s->pict_type == I_TYPE) {
            put_bits(&s->pb,
                     ff_msmp4_mb_i_table[coded_cbp][1], ff_msmp4_mb_i_table[coded_cbp][0]);
        } else {
            put_bits(&s->pb,
                     wmv2_inter_table[w->cbp_table_index][cbp][1],
                     wmv2_inter_table[w->cbp_table_index][cbp][0]);
        }
        put_bits(&s->pb, 1, 0);         /* no AC prediction yet */
        if(s->inter_intra_pred){
            s->h263_aic_dir=0;
            put_bits(&s->pb, table_inter_intra[s->h263_aic_dir][1], table_inter_intra[s->h263_aic_dir][0]);
        }
    }

    for (i = 0; i < 6; i++) {
        ff_msmpeg4_encode_block(s, block[i], i);
    }
}
#endif //CONFIG_WMV2_ENCODER

static void parse_mb_skip(Wmv2Context * w){
    int mb_x, mb_y;
    MpegEncContext * const s= &w->s;
    uint32_t * const mb_type= s->current_picture_ptr->mb_type;

    w->skip_type= get_bits(&s->gb, 2);
    switch(w->skip_type){
    case SKIP_TYPE_NONE:
        for(mb_y=0; mb_y<s->mb_height; mb_y++){
            for(mb_x=0; mb_x<s->mb_width; mb_x++){
                mb_type[mb_y*s->mb_stride + mb_x]= MB_TYPE_16x16 | MB_TYPE_L0;
            }
        }
        break;
    case SKIP_TYPE_MPEG:
        for(mb_y=0; mb_y<s->mb_height; mb_y++){
            for(mb_x=0; mb_x<s->mb_width; mb_x++){
                mb_type[mb_y*s->mb_stride + mb_x]= (get_bits1(&s->gb) ? MB_TYPE_SKIP : 0) | MB_TYPE_16x16 | MB_TYPE_L0;
            }
        }
        break;
    case SKIP_TYPE_ROW:
        for(mb_y=0; mb_y<s->mb_height; mb_y++){
            if(get_bits1(&s->gb)){
                for(mb_x=0; mb_x<s->mb_width; mb_x++){
                    mb_type[mb_y*s->mb_stride + mb_x]=  MB_TYPE_SKIP | MB_TYPE_16x16 | MB_TYPE_L0;
                }
            }else{
                for(mb_x=0; mb_x<s->mb_width; mb_x++){
                    mb_type[mb_y*s->mb_stride + mb_x]= (get_bits1(&s->gb) ? MB_TYPE_SKIP : 0) | MB_TYPE_16x16 | MB_TYPE_L0;
                }
            }
        }
        break;
    case SKIP_TYPE_COL:
        for(mb_x=0; mb_x<s->mb_width; mb_x++){
            if(get_bits1(&s->gb)){
                for(mb_y=0; mb_y<s->mb_height; mb_y++){
                    mb_type[mb_y*s->mb_stride + mb_x]=  MB_TYPE_SKIP | MB_TYPE_16x16 | MB_TYPE_L0;
                }
            }else{
                for(mb_y=0; mb_y<s->mb_height; mb_y++){
                    mb_type[mb_y*s->mb_stride + mb_x]= (get_bits1(&s->gb) ? MB_TYPE_SKIP : 0) | MB_TYPE_16x16 | MB_TYPE_L0;
                }
            }
        }
        break;
    }
}

static int decode_ext_header(Wmv2Context *w){
    MpegEncContext * const s= &w->s;
    GetBitContext gb;
    int fps;
    int code;

    if(s->avctx->extradata_size<4) return -1;

    init_get_bits(&gb, s->avctx->extradata, s->avctx->extradata_size*8);

    fps                = get_bits(&gb, 5);
    s->bit_rate        = get_bits(&gb, 11)*1024;
    w->mspel_bit       = get_bits1(&gb);
    s->loop_filter     = get_bits1(&gb);
    w->abt_flag        = get_bits1(&gb);
    w->j_type_bit      = get_bits1(&gb);
    w->top_left_mv_flag= get_bits1(&gb);
    w->per_mb_rl_bit   = get_bits1(&gb);
    code               = get_bits(&gb, 3);

    if(code==0) return -1;

    s->slice_height = s->mb_height / code;

    if(s->avctx->debug&FF_DEBUG_PICT_INFO){
        av_log(s->avctx, AV_LOG_DEBUG, "fps:%d, br:%d, qpbit:%d, abt_flag:%d, j_type_bit:%d, tl_mv_flag:%d, mbrl_bit:%d, code:%d, loop_filter:%d, slices:%d\n",
        fps, s->bit_rate, w->mspel_bit, w->abt_flag, w->j_type_bit, w->top_left_mv_flag, w->per_mb_rl_bit, code, s->loop_filter,
        code);
    }
    return 0;
}

int ff_wmv2_decode_picture_header(MpegEncContext * s)
{
    Wmv2Context * const w= (Wmv2Context*)s;
    int code;

#if 0
{
int i;
for(i=0; i<s->gb.size*8; i++)
    printf("%d", get_bits1(&s->gb));
//    get_bits1(&s->gb);
printf("END\n");
return -1;
}
#endif
    if(s->picture_number==0)
        decode_ext_header(w);

    s->pict_type = get_bits1(&s->gb) + 1;
    if(s->pict_type == I_TYPE){
        code = get_bits(&s->gb, 7);
        av_log(s->avctx, AV_LOG_DEBUG, "I7:%X/\n", code);
    }
    s->chroma_qscale= s->qscale = get_bits(&s->gb, 5);
    if(s->qscale < 0)
       return -1;

    return 0;
}

int ff_wmv2_decode_secondary_picture_header(MpegEncContext * s)
{
    Wmv2Context * const w= (Wmv2Context*)s;

    if (s->pict_type == I_TYPE) {
        if(w->j_type_bit) w->j_type= get_bits1(&s->gb);
        else              w->j_type= 0; //FIXME check

        if(!w->j_type){
            if(w->per_mb_rl_bit) s->per_mb_rl_table= get_bits1(&s->gb);
            else                 s->per_mb_rl_table= 0;

            if(!s->per_mb_rl_table){
                s->rl_chroma_table_index = decode012(&s->gb);
                s->rl_table_index = decode012(&s->gb);
            }

            s->dc_table_index = get_bits1(&s->gb);
        }
        s->inter_intra_pred= 0;
        s->no_rounding = 1;
        if(s->avctx->debug&FF_DEBUG_PICT_INFO){
            av_log(s->avctx, AV_LOG_DEBUG, "qscale:%d rlc:%d rl:%d dc:%d mbrl:%d j_type:%d \n",
                s->qscale,
                s->rl_chroma_table_index,
                s->rl_table_index,
                s->dc_table_index,
                s->per_mb_rl_table,
                w->j_type);
        }
    }else{
        int cbp_index;
        w->j_type=0;

        parse_mb_skip(w);
        cbp_index= decode012(&s->gb);
        if(s->qscale <= 10){
            int map[3]= {0,2,1};
            w->cbp_table_index= map[cbp_index];
        }else if(s->qscale <= 20){
            int map[3]= {1,0,2};
            w->cbp_table_index= map[cbp_index];
        }else{
            int map[3]= {2,1,0};
            w->cbp_table_index= map[cbp_index];
        }

        if(w->mspel_bit) s->mspel= get_bits1(&s->gb);
        else             s->mspel= 0; //FIXME check

        if(w->abt_flag){
            w->per_mb_abt= get_bits1(&s->gb)^1;
            if(!w->per_mb_abt){
                w->abt_type= decode012(&s->gb);
            }
        }

        if(w->per_mb_rl_bit) s->per_mb_rl_table= get_bits1(&s->gb);
        else                 s->per_mb_rl_table= 0;

        if(!s->per_mb_rl_table){
            s->rl_table_index = decode012(&s->gb);
            s->rl_chroma_table_index = s->rl_table_index;
        }

        s->dc_table_index = get_bits1(&s->gb);
        s->mv_table_index = get_bits1(&s->gb);

        s->inter_intra_pred= 0;//(s->width*s->height < 320*240 && s->bit_rate<=II_BITRATE);
        s->no_rounding ^= 1;

        if(s->avctx->debug&FF_DEBUG_PICT_INFO){
            av_log(s->avctx, AV_LOG_DEBUG, "rl:%d rlc:%d dc:%d mv:%d mbrl:%d qp:%d mspel:%d per_mb_abt:%d abt_type:%d cbp:%d ii:%d\n",
                s->rl_table_index,
                s->rl_chroma_table_index,
                s->dc_table_index,
                s->mv_table_index,
                s->per_mb_rl_table,
                s->qscale,
                s->mspel,
                w->per_mb_abt,
                w->abt_type,
                w->cbp_table_index,
                s->inter_intra_pred);
        }
    }
    s->esc3_level_length= 0;
    s->esc3_run_length= 0;

s->picture_number++; //FIXME ?


//    if(w->j_type)
//        return wmv2_decode_j_picture(w); //FIXME

    if(w->j_type){
        ff_intrax8_decode_picture(&w->x8, 2*s->qscale, (s->qscale-1)|1 );
        return 1;
    }

    return 0;
}

static inline int wmv2_decode_motion(Wmv2Context *w, int *mx_ptr, int *my_ptr){
    MpegEncContext * const s= &w->s;
    int ret;

    ret= msmpeg4_decode_motion(s, mx_ptr, my_ptr);

    if(ret<0) return -1;

    if((((*mx_ptr)|(*my_ptr)) & 1) && s->mspel)
        w->hshift= get_bits1(&s->gb);
    else
        w->hshift= 0;

//printf("%d %d  ", *mx_ptr, *my_ptr);

    return 0;
}

static int16_t *wmv2_pred_motion(Wmv2Context *w, int *px, int *py){
    MpegEncContext * const s= &w->s;
    int xy, wrap, diff, type;
    int16_t *A, *B, *C, *mot_val;

    wrap = s->b8_stride;
    xy = s->block_index[0];

    mot_val = s->current_picture.motion_val[0][xy];

    A = s->current_picture.motion_val[0][xy - 1];
    B = s->current_picture.motion_val[0][xy - wrap];
    C = s->current_picture.motion_val[0][xy + 2 - wrap];

    if(s->mb_x && !s->first_slice_line && !s->mspel && w->top_left_mv_flag)
        diff= FFMAX(FFABS(A[0] - B[0]), FFABS(A[1] - B[1]));
    else
        diff=0;

    if(diff >= 8)
        type= get_bits1(&s->gb);
    else
        type= 2;

    if(type == 0){
        *px= A[0];
        *py= A[1];
    }else if(type == 1){
        *px= B[0];
        *py= B[1];
    }else{
        /* special case for first (slice) line */
        if (s->first_slice_line) {
            *px = A[0];
            *py = A[1];
        } else {
            *px = mid_pred(A[0], B[0], C[0]);
            *py = mid_pred(A[1], B[1], C[1]);
        }
    }

    return mot_val;
}

static inline int wmv2_decode_inter_block(Wmv2Context *w, DCTELEM *block, int n, int cbp){
    MpegEncContext * const s= &w->s;
    static const int sub_cbp_table[3]= {2,3,1};
    int sub_cbp;

    if(!cbp){
        s->block_last_index[n] = -1;

        return 0;
    }

    if(w->per_block_abt)
        w->abt_type= decode012(&s->gb);
#if 0
    if(w->per_block_abt)
        printf("B%d", w->abt_type);
#endif
    w->abt_type_table[n]= w->abt_type;

    if(w->abt_type){
//        const uint8_t *scantable= w->abt_scantable[w->abt_type-1].permutated;
        const uint8_t *scantable= w->abt_scantable[w->abt_type-1].scantable;
//        const uint8_t *scantable= w->abt_type-1 ? w->abt_scantable[1].permutated : w->abt_scantable[0].scantable;

        sub_cbp= sub_cbp_table[ decode012(&s->gb) ];
//        printf("S%d", sub_cbp);

        if(sub_cbp&1){
            if (msmpeg4_decode_block(s, block, n, 1, scantable) < 0)
                return -1;
        }

        if(sub_cbp&2){
            if (msmpeg4_decode_block(s, w->abt_block2[n], n, 1, scantable) < 0)
                return -1;
        }
        s->block_last_index[n] = 63;

        return 0;
    }else{
        return msmpeg4_decode_block(s, block, n, 1, s->inter_scantable.permutated);
    }
}

static void wmv2_add_block(Wmv2Context *w, DCTELEM *block1, uint8_t *dst, int stride, int n){
    MpegEncContext * const s= &w->s;

  if (s->block_last_index[n] >= 0) {
    switch(w->abt_type_table[n]){
    case 0:
        s->dsp.idct_add (dst, stride, block1);
        break;
    case 1:
        simple_idct84_add(dst           , stride, block1);
        simple_idct84_add(dst + 4*stride, stride, w->abt_block2[n]);
        memset(w->abt_block2[n], 0, 64*sizeof(DCTELEM));
        break;
    case 2:
        simple_idct48_add(dst           , stride, block1);
        simple_idct48_add(dst + 4       , stride, w->abt_block2[n]);
        memset(w->abt_block2[n], 0, 64*sizeof(DCTELEM));
        break;
    default:
        av_log(s->avctx, AV_LOG_ERROR, "internal error in WMV2 abt\n");
    }
  }
}

void ff_wmv2_add_mb(MpegEncContext *s, DCTELEM block1[6][64], uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr){
    Wmv2Context * const w= (Wmv2Context*)s;

    wmv2_add_block(w, block1[0], dest_y                    , s->linesize, 0);
    wmv2_add_block(w, block1[1], dest_y + 8                , s->linesize, 1);
    wmv2_add_block(w, block1[2], dest_y +     8*s->linesize, s->linesize, 2);
    wmv2_add_block(w, block1[3], dest_y + 8 + 8*s->linesize, s->linesize, 3);

    if(s->flags&CODEC_FLAG_GRAY) return;

    wmv2_add_block(w, block1[4], dest_cb                   , s->uvlinesize, 4);
    wmv2_add_block(w, block1[5], dest_cr                   , s->uvlinesize, 5);
}

void ff_mspel_motion(MpegEncContext *s,
                               uint8_t *dest_y, uint8_t *dest_cb, uint8_t *dest_cr,
                               uint8_t **ref_picture, op_pixels_func (*pix_op)[4],
                               int motion_x, int motion_y, int h)
{
    Wmv2Context * const w= (Wmv2Context*)s;
    uint8_t *ptr;
    int dxy, offset, mx, my, src_x, src_y, v_edge_pos, linesize, uvlinesize;
    int emu=0;

    dxy = ((motion_y & 1) << 1) | (motion_x & 1);
    dxy = (2 * dxy)  + w->hshift;
    src_x = (s->mb_x * 16) + (motion_x >> 1);
    src_y = (s->mb_y * 16) + (motion_y >> 1);

    /* WARNING: do no forget half pels */
    v_edge_pos = s->v_edge_pos;
    src_x = av_clip(src_x, -16, s->width);
    src_y = av_clip(src_y, -16, s->height);

    if(src_x<=-16 || src_x >= s->width)
        dxy &= ~3;
    if(src_y<=-16 || src_y >= s->height)
        dxy &= ~4;

    linesize   = s->linesize;
    uvlinesize = s->uvlinesize;
    ptr = ref_picture[0] + (src_y * linesize) + src_x;

    if(s->flags&CODEC_FLAG_EMU_EDGE){
        if(src_x<1 || src_y<1 || src_x + 17  >= s->h_edge_pos
                              || src_y + h+1 >= v_edge_pos){
            ff_emulated_edge_mc(s->edge_emu_buffer, ptr - 1 - s->linesize, s->linesize, 19, 19,
                             src_x-1, src_y-1, s->h_edge_pos, s->v_edge_pos);
            ptr= s->edge_emu_buffer + 1 + s->linesize;
            emu=1;
        }
    }

    s->dsp.put_mspel_pixels_tab[dxy](dest_y             , ptr             , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8           , ptr+8           , linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y  +8*linesize, ptr  +8*linesize, linesize);
    s->dsp.put_mspel_pixels_tab[dxy](dest_y+8+8*linesize, ptr+8+8*linesize, linesize);

    if(s->flags&CODEC_FLAG_GRAY) return;

    if (s->out_format == FMT_H263) {
        dxy = 0;
        if ((motion_x & 3) != 0)
            dxy |= 1;
        if ((motion_y & 3) != 0)
            dxy |= 2;
        mx = motion_x >> 2;
        my = motion_y >> 2;
    } else {
        mx = motion_x /2;
        my = motion_y /2;
        dxy = ((my & 1) << 1) | (mx & 1);
        mx >>= 1;
        my >>= 1;
    }

    src_x = s->mb_x * 8   + mx;
    src_y = s->mb_y * 8   + my;
    src_x = av_clip(src_x, -8, s->width >> 1);
    if (src_x == (s->width >> 1))
        dxy &= ~1;
    src_y = av_clip(src_y, -8, s->height >> 1);
    if (src_y == (s->height >> 1))
        dxy &= ~2;
    offset = (src_y * uvlinesize) + src_x;
    ptr = ref_picture[1] + offset;
    if(emu){
        ff_emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize, 9, 9,
                         src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    pix_op[1][dxy](dest_cb, ptr, uvlinesize, h >> 1);

    ptr = ref_picture[2] + offset;
    if(emu){
        ff_emulated_edge_mc(s->edge_emu_buffer, ptr, s->uvlinesize, 9, 9,
                         src_x, src_y, s->h_edge_pos>>1, s->v_edge_pos>>1);
        ptr= s->edge_emu_buffer;
    }
    pix_op[1][dxy](dest_cr, ptr, uvlinesize, h >> 1);
}


static int wmv2_decode_mb(MpegEncContext *s, DCTELEM block[6][64])
{
    Wmv2Context * const w= (Wmv2Context*)s;
    int cbp, code, i;
    uint8_t *coded_val;

    if(w->j_type) return 0;

    if (s->pict_type == P_TYPE) {
        if(IS_SKIP(s->current_picture.mb_type[s->mb_y * s->mb_stride + s->mb_x])){
            /* skip mb */
            s->mb_intra = 0;
            for(i=0;i<6;i++)
                s->block_last_index[i] = -1;
            s->mv_dir = MV_DIR_FORWARD;
            s->mv_type = MV_TYPE_16X16;
            s->mv[0][0][0] = 0;
            s->mv[0][0][1] = 0;
            s->mb_skipped = 1;
            w->hshift=0;
            return 0;
        }

        code = get_vlc2(&s->gb, mb_non_intra_vlc[w->cbp_table_index].table, MB_NON_INTRA_VLC_BITS, 3);
        if (code < 0)
            return -1;
        s->mb_intra = (~code & 0x40) >> 6;

        cbp = code & 0x3f;
    } else {
        s->mb_intra = 1;
        code = get_vlc2(&s->gb, ff_msmp4_mb_i_vlc.table, MB_INTRA_VLC_BITS, 2);
        if (code < 0){
            av_log(s->avctx, AV_LOG_ERROR, "II-cbp illegal at %d %d\n", s->mb_x, s->mb_y);
            return -1;
        }
        /* predict coded block pattern */
        cbp = 0;
        for(i=0;i<6;i++) {
            int val = ((code >> (5 - i)) & 1);
            if (i < 4) {
                int pred = coded_block_pred(s, i, &coded_val);
                val = val ^ pred;
                *coded_val = val;
            }
            cbp |= val << (5 - i);
        }
    }

    if (!s->mb_intra) {
        int mx, my;
//printf("P at %d %d\n", s->mb_x, s->mb_y);
        wmv2_pred_motion(w, &mx, &my);

        if(cbp){
            s->dsp.clear_blocks(s->block[0]);
            if(s->per_mb_rl_table){
                s->rl_table_index = decode012(&s->gb);
                s->rl_chroma_table_index = s->rl_table_index;
            }

            if(w->abt_flag && w->per_mb_abt){
                w->per_block_abt= get_bits1(&s->gb);
                if(!w->per_block_abt)
                    w->abt_type= decode012(&s->gb);
            }else
                w->per_block_abt=0;
        }

        if (wmv2_decode_motion(w, &mx, &my) < 0)
            return -1;

        s->mv_dir = MV_DIR_FORWARD;
        s->mv_type = MV_TYPE_16X16;
        s->mv[0][0][0] = mx;
        s->mv[0][0][1] = my;

        for (i = 0; i < 6; i++) {
            if (wmv2_decode_inter_block(w, block[i], i, (cbp >> (5 - i)) & 1) < 0)
            {
                av_log(s->avctx, AV_LOG_ERROR, "\nerror while decoding inter block: %d x %d (%d)\n", s->mb_x, s->mb_y, i);
                return -1;
            }
        }
    } else {
//if(s->pict_type==P_TYPE)
//   printf("%d%d ", s->inter_intra_pred, cbp);
//printf("I at %d %d %d %06X\n", s->mb_x, s->mb_y, ((cbp&3)? 1 : 0) +((cbp&0x3C)? 2 : 0), show_bits(&s->gb, 24));
        s->ac_pred = get_bits1(&s->gb);
        if(s->inter_intra_pred){
            s->h263_aic_dir= get_vlc2(&s->gb, inter_intra_vlc.table, INTER_INTRA_VLC_BITS, 1);
//            printf("%d%d %d %d/", s->ac_pred, s->h263_aic_dir, s->mb_x, s->mb_y);
        }
        if(s->per_mb_rl_table && cbp){
            s->rl_table_index = decode012(&s->gb);
            s->rl_chroma_table_index = s->rl_table_index;
        }

        s->dsp.clear_blocks(s->block[0]);
        for (i = 0; i < 6; i++) {
            if (msmpeg4_decode_block(s, block[i], i, (cbp >> (5 - i)) & 1, NULL) < 0)
            {
                av_log(s->avctx, AV_LOG_ERROR, "\nerror while decoding intra block: %d x %d (%d)\n", s->mb_x, s->mb_y, i);
                return -1;
            }
        }
    }

    return 0;
}

static int wmv2_decode_init(AVCodecContext *avctx){
    Wmv2Context * const w= avctx->priv_data;

    if(avctx->idct_algo==FF_IDCT_AUTO){
        avctx->idct_algo=FF_IDCT_WMV2;
    }
    
    if(ff_h263_decode_init(avctx) < 0)
        return -1;

    wmv2_common_init(w);
    
    ff_intrax8_common_init(&w->x8,&w->s);

    return 0;
}

static int wmv2_decode_end(AVCodecContext *avctx)
{
    Wmv2Context *w = avctx->priv_data;

    ff_intrax8_common_end(&w->x8);
    return ff_h263_decode_end(avctx);
}

#ifdef CONFIG_WMV2_DECODER
AVCodec wmv2_decoder = {
    "wmv2",
    CODEC_TYPE_VIDEO,
    CODEC_ID_WMV2,
    sizeof(Wmv2Context),
    wmv2_decode_init,
    NULL,
    wmv2_decode_end,
    ff_h263_decode_frame,
    CODEC_CAP_DRAW_HORIZ_BAND | CODEC_CAP_DR1,
};
#endif

#ifdef CONFIG_WMV2_ENCODER
AVCodec wmv2_encoder = {
    "wmv2",
    CODEC_TYPE_VIDEO,
    CODEC_ID_WMV2,
    sizeof(Wmv2Context),
    wmv2_encode_init,
    MPV_encode_picture,
    MPV_encode_end,
    .pix_fmts= (enum PixelFormat[]){PIX_FMT_YUV420P, -1},
};
#endif


#ifndef __LINUX__

#include "avcodec.h"
#include "dsputil.h"
#include "simple_idct.h"

//#ifdef JZ4740_MXU_OPT
//#include "jzmedia.h"
//Q15 Constant Data
//#define  WM44  0x5A825A82   //(W4,W4)
//#define  WM26  0x764230FC   //(W2,W6)
//#define  WM13  0x7D8A6A6E   //(W1,W3)
//#define  WM57  0x471D18F9   //(W5,W7)

#if 0
#define W1 2841 /* 2048*sqrt (2)*cos (1*pi/16) */
#define W2 2676 /* 2048*sqrt (2)*cos (2*pi/16) */
#define W3 2408 /* 2048*sqrt (2)*cos (3*pi/16) */
#define W4 2048 /* 2048*sqrt (2)*cos (4*pi/16) */
#define W5 1609 /* 2048*sqrt (2)*cos (5*pi/16) */
#define W6 1108 /* 2048*sqrt (2)*cos (6*pi/16) */
#define W7 565  /* 2048*sqrt (2)*cos (7*pi/16) */
#define ROW_SHIFT 8
#define COL_SHIFT 17
#else
#define W1  22725  //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W2  21407  //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W3  19266  //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W4  16383  //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W5  12873  //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W6  8867   //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define W7  4520   //cos(i*M_PI/16)*sqrt(2)*(1<<14) + 0.5
#define ROW_SHIFT 11
#define COL_SHIFT 20 // 6
#endif

#if defined(ARCH_POWERPC_405)
/* signed 16x16 -> 32 multiply add accumulate */
#define MAC16(rt, ra, rb) \
    asm ("maclhw %0, %2, %3" : "=r" (rt) : "0" (rt), "r" (ra), "r" (rb));

/* signed 16x16 -> 32 multiply */
#define MUL16(rt, ra, rb) \
    asm ("mullhw %0, %1, %2" : "=r" (rt) : "r" (ra), "r" (rb));

#else

/* signed 16x16 -> 32 multiply add accumulate */
#define MAC16(rt, ra, rb) rt += (ra) * (rb)

/* signed 16x16 -> 32 multiply */
#define MUL16(rt, ra, rb) rt = (ra) * (rb)

#endif

static inline void idctRowCondDC (DCTELEM * row)
{
        int a0, a1, a2, a3, b0, b1, b2, b3;
#ifdef HAVE_FAST_64BIT
        uint64_t temp;
#else
        uint32_t temp;
#endif

#ifdef HAVE_FAST_64BIT
#ifdef WORDS_BIGENDIAN
#define ROW0_MASK 0xffff000000000000LL
#else
#define ROW0_MASK 0xffffLL
#endif
        if(sizeof(DCTELEM)==2){
            if ( ((((uint64_t *)row)[0] & ~ROW0_MASK) |
                  ((uint64_t *)row)[1]) == 0) {
                temp = (row[0] << 3) & 0xffff;
                temp += temp << 16;
                temp += temp << 32;
                ((uint64_t *)row)[0] = temp;
                ((uint64_t *)row)[1] = temp;
                return;
            }
        }else{
            if (!(row[1]|row[2]|row[3]|row[4]|row[5]|row[6]|row[7])) {
                row[0]=row[1]=row[2]=row[3]=row[4]=row[5]=row[6]=row[7]= row[0] << 3;
                return;
            }
        }
#else
        if(sizeof(DCTELEM)==2){
            if (!(((uint32_t*)row)[1] |
                  ((uint32_t*)row)[2] |
                  ((uint32_t*)row)[3] |
                  row[1])) {
                temp = (row[0] << 3) & 0xffff;
                temp += temp << 16;
                ((uint32_t*)row)[0]=((uint32_t*)row)[1] =
                ((uint32_t*)row)[2]=((uint32_t*)row)[3] = temp;
                return;
            }
        }else{
            if (!(row[1]|row[2]|row[3]|row[4]|row[5]|row[6]|row[7])) {
                row[0]=row[1]=row[2]=row[3]=row[4]=row[5]=row[6]=row[7]= row[0] << 3;
                return;
            }
        }
#endif

        a0 = (W4 * row[0]) + (1 << (ROW_SHIFT - 1));
        a1 = a0;
        a2 = a0;
        a3 = a0;

        /* no need to optimize : gcc does it */
        a0 += W2 * row[2];
        a1 += W6 * row[2];
        a2 -= W6 * row[2];
        a3 -= W2 * row[2];

        MUL16(b0, W1, row[1]);
        MAC16(b0, W3, row[3]);
        MUL16(b1, W3, row[1]);
        MAC16(b1, -W7, row[3]);
        MUL16(b2, W5, row[1]);
        MAC16(b2, -W1, row[3]);
        MUL16(b3, W7, row[1]);
        MAC16(b3, -W5, row[3]);

#ifdef HAVE_FAST_64BIT
        temp = ((uint64_t*)row)[1];
#else
        temp = ((uint32_t*)row)[2] | ((uint32_t*)row)[3];
#endif
        if (temp != 0) {
            a0 += W4*row[4] + W6*row[6];
            a1 += - W4*row[4] - W2*row[6];
            a2 += - W4*row[4] + W2*row[6];
            a3 += W4*row[4] - W6*row[6];

            MAC16(b0, W5, row[5]);
            MAC16(b0, W7, row[7]);

            MAC16(b1, -W1, row[5]);
            MAC16(b1, -W5, row[7]);

            MAC16(b2, W7, row[5]);
            MAC16(b2, W3, row[7]);

            MAC16(b3, W3, row[5]);
            MAC16(b3, -W1, row[7]);
        }

        row[0] = (a0 + b0) >> ROW_SHIFT;
        row[7] = (a0 - b0) >> ROW_SHIFT;
        row[1] = (a1 + b1) >> ROW_SHIFT;
        row[6] = (a1 - b1) >> ROW_SHIFT;
        row[2] = (a2 + b2) >> ROW_SHIFT;
        row[5] = (a2 - b2) >> ROW_SHIFT;
        row[3] = (a3 + b3) >> ROW_SHIFT;
        row[4] = (a3 - b3) >> ROW_SHIFT;
}

static inline void idctSparseColAdd (uint8_t *dest, int line_size,
                                     DCTELEM * col)
{
        int a0, a1, a2, a3, b0, b1, b2, b3;
        uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

        /* XXX: I did that only to give same values as previous code */
        a0 = W4 * (col[8*0] + ((1<<(COL_SHIFT-1))/W4));
        a1 = a0;
        a2 = a0;
        a3 = a0;

        a0 +=  + W2*col[8*2];
        a1 +=  + W6*col[8*2];
        a2 +=  - W6*col[8*2];
        a3 +=  - W2*col[8*2];

        MUL16(b0, W1, col[8*1]);
        MUL16(b1, W3, col[8*1]);
        MUL16(b2, W5, col[8*1]);
        MUL16(b3, W7, col[8*1]);

        MAC16(b0, + W3, col[8*3]);
        MAC16(b1, - W7, col[8*3]);
        MAC16(b2, - W1, col[8*3]);
        MAC16(b3, - W5, col[8*3]);

        if(col[8*4]){
            a0 += + W4*col[8*4];
            a1 += - W4*col[8*4];
            a2 += - W4*col[8*4];
            a3 += + W4*col[8*4];
        }

        if (col[8*5]) {
            MAC16(b0, + W5, col[8*5]);
            MAC16(b1, - W1, col[8*5]);
            MAC16(b2, + W7, col[8*5]);
            MAC16(b3, + W3, col[8*5]);
        }

        if(col[8*6]){
            a0 += + W6*col[8*6];
            a1 += - W2*col[8*6];
            a2 += + W2*col[8*6];
            a3 += - W6*col[8*6];
        }

        if (col[8*7]) {
            MAC16(b0, + W7, col[8*7]);
            MAC16(b1, - W5, col[8*7]);
            MAC16(b2, + W3, col[8*7]);
            MAC16(b3, - W1, col[8*7]);
        }

        dest[0] = cm[dest[0] + ((a0 + b0) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a1 + b1) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a2 + b2) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a3 + b3) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a3 - b3) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a2 - b2) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a1 - b1) >> COL_SHIFT)];
        dest += line_size;
        dest[0] = cm[dest[0] + ((a0 - b0) >> COL_SHIFT)];
}

/* 8x4 & 4x8 WMV2 IDCT */
#undef CN_SHIFT
#undef C_SHIFT
#undef C_FIX
#undef C1
#undef C2
#define CN_SHIFT 12
#define C_FIX(x) ((int)((x) * 1.414213562 * (1 << CN_SHIFT) + 0.5))
#define C1 C_FIX(0.6532814824)
#define C2 C_FIX(0.2705980501)
#define C3 C_FIX(0.5)
#define C_SHIFT (4+1+12)
static inline void idct4col_add(uint8_t *dest, int line_size, const DCTELEM *col)
{
    int c0, c1, c2, c3, a0, a1, a2, a3;
    const uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    a0 = col[8*0];
    a1 = col[8*1];
    a2 = col[8*2];
    a3 = col[8*3];
    c0 = (a0 + a2)*C3 + (1 << (C_SHIFT - 1));
    c2 = (a0 - a2)*C3 + (1 << (C_SHIFT - 1));
    c1 = a1 * C1 + a3 * C2;
    c3 = a1 * C2 - a3 * C1;
    dest[0] = cm[dest[0] + ((c0 + c1) >> C_SHIFT)];
    dest += line_size;
    dest[0] = cm[dest[0] + ((c2 + c3) >> C_SHIFT)];
    dest += line_size;
    dest[0] = cm[dest[0] + ((c2 - c3) >> C_SHIFT)];
    dest += line_size;
    dest[0] = cm[dest[0] + ((c0 - c1) >> C_SHIFT)];
}

#define RN_SHIFT 15
#define R_FIX(x) ((int)((x) * 1.414213562 * (1 << RN_SHIFT) + 0.5))
#define R1 R_FIX(0.6532814824)
#define R2 R_FIX(0.2705980501)
#define R3 R_FIX(0.5)
#define R_SHIFT 11
static inline void idct4row(DCTELEM *row)
{
    int c0, c1, c2, c3, a0, a1, a2, a3;
    //const uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    a0 = row[0];
    a1 = row[1];
    a2 = row[2];
    a3 = row[3];
    c0 = (a0 + a2)*R3 + (1 << (R_SHIFT - 1));
    c2 = (a0 - a2)*R3 + (1 << (R_SHIFT - 1));
    c1 = a1 * R1 + a3 * R2;
    c3 = a1 * R2 - a3 * R1;
    row[0]= (c0 + c1) >> R_SHIFT;
    row[1]= (c2 + c3) >> R_SHIFT;
    row[2]= (c2 - c3) >> R_SHIFT;
    row[3]= (c0 - c1) >> R_SHIFT;
}
/* 8x4 & 4x8 WMV2 IDCT */
#undef C_FIX
#undef C1
#undef C2
#undef C3
#define C_FIX(x) ((int)((x) * /*1.414213562 */ (1 << 15) + 0.5))
#define C1 C_FIX(0.6532814824)
#define C2 C_FIX(0.2705980501)
#define C3 C_FIX(0.5)
#define  wxr5  0x5A827642
#define  wxr6  0x5A8230FC
#define  wxr7  0x7D8A6A6E
#define  wxr9  0x6A6E18F9
#define  wxr10 0x471D7D8A
#define  wxr8  0x18F9471D
#ifdef JZ4740_MXU_OPT
 void simple_idct84_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    int i;
    DCTELEM *blk;

    /* IDCT4 and store */
    S32I2M(xr11, ((C1 << 16) | (C2 & 0xffff)));            // xr11  (R1, R2)
    S32I2M(xr12, (1 << 14));                       // xr11  (R1, R2)
    S32I2M(xr13, (C3)) ;                   // xr13 (R3)
  for(i=0; i<4; i++)
  {
  DCTELEM *col;
  col = block + i*2;
  S32LDD(xr2,  col, 0x10);          //  xr2  (nx1, x1)
  S32LDD(xr4,  col, 0x30);          //  xr4  (nx3, x3)
  S32LDD(xr1,  col, 0x00);          //  xr1  (nx0, x0)
  S32LDD(xr3,  col, 0x20);          //  xr3  (nx2, x2)
  D16MUL_HW    (xr5, xr11, xr2, xr6);    // xr5 (na1*C1)   xr6 (a1*C1)
  D16MAC_AA_LW (xr5, xr11, xr4, xr6);    // xr5.nc1 (na1*C1 + na3*C2)    xr6.c1 (a1*C1 + a3*C2)

  D16MUL_LW    (xr7, xr11, xr2, xr8);    // xr7 (na1*C2)   xr8 (a1*C2)
  D16MAC_SS_HW (xr7, xr11, xr4, xr8);    // xr7.nc3 (na1*C2 - na3*C1)    xr8.c3 (a1*C2 - a3*C1)

  D16MUL_LW    (xr9,  xr13, xr1, xr10);  // xr9  (C3*na0)   xr10 (C3*a0)
  D16MUL_LW    (xr14, xr13, xr3, xr15);  // xr14 (C3*na2)   xr15 (C3*a2)

  D32ADD_AS    (xr1, xr9,  xr14, xr2);   // xr1.nc0 (na0 + na2)*C3   xr2.nc2 (na0-na2)*C3
  D32ADD_AS    (xr3, xr10, xr15, xr4);   // xr3.c0  (a0 + a2)*C3      xr4.c2   (a0-a2)*C3

  D32ADD_AS (xr9, xr3,  xr6, xr10);      // xr9: (c0 + c1)       xr10: (c0 - c1)
  D32ACC_AS (xr9, xr12, xr0, xr10);      // xr9: (c0 + c1)       xr10: (c0 - c1)

  D32ADD_AS (xr5, xr1,  xr5, xr6);       // xr5: (nc0 + nc1)     xr6:  (nc0 - nc1)
  D32ACC_AS (xr5, xr12, xr0, xr6);       // xr5: (nc0 + nc1)     xr6:  (nc0 - nc1)

  D32ADD_AS (xr14, xr4, xr8, xr15);      // xr14: (c2 + c3)        xr15: (c2 - c3)
  D32ACC_AS (xr14, xr12,xr0, xr15);      // xr14: (c2 + c3)        xr15: (c2 - c3)

  D32ADD_AS (xr7, xr2, xr7, xr8);    // xr7:  (nc2 + nc3)   xr8:  (nc2 - nc3)
  D32ACC_AS (xr7, xr12,xr0, xr8);    // xr7:  (nc2 + nc3)   xr8:  (nc2 - nc3)

  D32SARL(xr5, xr5, xr9, 15);           // xr5 (nc0+nc1, c0+c1)
  S32STD (xr5, col, 0x0);

  D32SARL(xr7, xr7,xr14, 15);           // xr7 (nc2+nc3, c2+c3)
  S32STD (xr7, col, 0x10);

  D32SARL(xr8, xr8, xr15, 15);          // xr8  (nc2-nc3, c2-c3)
  S32STD (xr8, col, 0x20);

  D32SARL(xr6, xr6, xr10, 15);          // xr6  (nc0-nc1, c0-c1)
  S32STD (xr6, col, 0x30);

  }

  S32I2M(xr5,wxr5) ;                 // xr5 (W4, W2)
  S32I2M(xr6,wxr6) ;                 // xr6 (W4, W6)
  S32I2M(xr7,wxr7) ;                 // xr7 (W1, W3)
  S32I2M(xr8,wxr8) ;                 // xr8 (W7, W5)
  S32I2M(xr9,wxr9) ;                 // xr9 (W3, W7)
  S32I2M(xr10,wxr10) ;               // xr10(W5,W1)

   for (i = 0; i < 4; i++)   /* idct rows */
   {
        blk = block + (i << 3);

        S32LDD(xr1, blk, 0);              //  xr1 (x1, x0)
        S32LDD(xr2, blk, 0x4);            //  xr2 (x3, x2)
        S32LDD(xr3, blk, 0x8);            //  xr3 (x5, x4)
        S32LDD(xr4, blk, 0xc);            //  xr4 (x7, x6)

        S32SFL(xr1,xr1,xr2,xr2, ptn3);  // xr1(x1, x3)  xr2(x0, x2)
        S32SFL(xr3,xr3,xr4,xr4, ptn3);  // xr3(x5, x7)  xr4(x4, x6)

        D16MUL_WW(xr11, xr2, xr5, xr12);            //xr11(w4*x0)  xr12(w2*x2)
        D16MAC_AA_WW(xr11,xr4,xr6,xr12);            //xr11(w4*x0+w4*x4) xr12:(w2*x2+w6*x6)

        D16MUL_WW(xr13, xr2, xr6, xr14);            //xr13(w4*x0)  xr14(w6*x2)
        D16MAC_SS_WW(xr13,xr4,xr5,xr14);            //xr13(w4*x0-w4*x4) xr14:(w6*x2+w6*x6)

        D16MUL_HW(xr2, xr1, xr7, xr4);              //xr2:w1*x1    xr4:w3*x1
        D16MAC_AS_LW(xr2,xr1,xr9,xr4);              //xr2:w1*x1 + w3*x3 xr4:w3*x1-w7*x3
        D16MAC_AS_HW(xr2,xr3,xr10,xr4);             //
        D16MAC_AS_LW(xr2,xr3,xr8,xr4);              
        D16MACF_AA_WW(xr2, xr0, xr0, xr4);   //
        D16MACF_AA_WW(xr11, xr0, xr0, xr13); //
        D16MACF_AA_WW(xr12, xr0, xr0, xr14); //

        D16MUL_HW(xr13, xr1, xr8, xr14);            //w7*(x4+x5) store in xr14 ,w3*(x6+x7) store in xr15
        D16MAC_SS_LW(xr13,xr1,xr10,xr14);           
        D16MAC_AA_HW(xr13,xr3,xr9,xr14);            
        D16MAC_SA_LW(xr13,xr3,xr7,xr14);            
        D16MACF_AA_WW(xr14, xr0, xr0, xr13); 

        Q16ADD_AS_WW(xr11,xr11,xr12,xr12);   //

        Q16ADD_AS_WW(xr11, xr11, xr2, xr2);  // xr11 ((a0 + b0), (a1 + b1))  // xr2  ((a0 - b0), (a1 - b1))
        Q16ADD_AS_XW(xr12, xr12, xr14, xr14);// xr12 ((a2 + b2), (a3 + b3))  // xr14 ((a2 - b2), (a3 - b3))
        S32SFL(xr11,xr11,xr12,xr12, ptn3);   // xr11 ((a0 + b0), (a2 + b2))  // xr12 ((a1 + b1), (a3 + b3))
        S32SFL(xr12,xr12,xr11,xr11, ptn3);         // xr11 ((a3 + b3), (a2 + b2))

        S32LDD(xr3, dest, 0);
        Q8ACCE_AA(xr11, xr3, xr0, xr12);
        Q16SAT(xr3,xr11, xr12);
        S32STD(xr3, dest, 0x0);

        S32LDD(xr3, dest, 4);
        Q8ACCE_AA(xr2, xr3, xr0, xr14);
        Q16SAT(xr3,xr2, xr14);
        S32STD(xr3, dest, 0x4);

        dest += line_size;
  }
}

#else
void simple_idct84_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    int i;
   
    /* IDCT8 on each line */
    for(i=0; i<4; i++) {
        idctRowCondDC(block + i*8);
    }

    /* IDCT4 and store */
    for(i=0;i<8;i++) {
        idct4col_add(dest + i, line_size, block + i);
    }
}
#endif

#undef R_FIX(x)
#undef R1
#undef R2
#undef R3
#define R_FIX(x) ((int)((x) /* 1.414213562*/ * (1 << 15) + 0.5))
#define R1 R_FIX(0.6532814824)
#define R2 R_FIX(0.2705980501)
#define R3 R_FIX(0.5)

#ifdef JZ4740_MXU_OPT

void simple_idct48_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    int i;
        DCTELEM *row;
    /* IDCT8 and store */
        S32I2M(xr5,wxr5) ;                 // xr5 (W4, W2)
        S32I2M(xr6,wxr6) ;                 // xr6 (W4, W6)
        S32I2M(xr7,wxr7) ;                 // xr7 (W1, W3)
        S32I2M(xr8,wxr8) ;                 // xr8 (W7, W5)
        S32I2M(xr9,wxr9) ;                 // xr9 (W3, W7)
        S32I2M(xr10,wxr10) ;               // xr10(W5, W1)

        for(i=0; i<2; i++){
        DCTELEM *blk;
        blk = block + 2*i;
        S32LDD(xr1, blk, 0x00);  //  xr1 (x4, x0)
        S32LDD(xr2, blk, 0x10);  //  xr2 (x7, x3)
        S32LDD(xr3, blk, 0x20);  //  xr3 (x6, x1)
        S32LDD(xr4, blk, 0x30);  //  xr4 (x5, x2)

        S32LDD(xr11, blk, 0x40);         //  xr1 (x4, x0)
        S32LDD(xr12, blk, 0x50);         //  xr2 (x7, x3)
        S32LDD(xr13, blk, 0x60);         //  xr3 (x6, x1)
        S32LDD(xr14, blk, 0x70);         //  xr4 (x5, x2)

        S32I2M(xr5,wxr5);
        D16MUL_HW(xr2, xr5, xr1, xr9);    //xr2(h0 * W4) xr9(x0 * W4)
        D16MAC_AA_HW(xr2,xr5,xr11,xr9);   //xr2(h0 * W4 + h4 * W4)    //xr9(x0 * W4 + x4 * W4)
        D16MACF_AA_WW(xr2, xr0, xr0, xr9);  //xr2((h0*W4 + h4*W4), (x0*W4 + x4*W4))
        D16MUL_LW(xr10, xr5, xr3, xr15);          // xr10(h2 * W2) xr15(x2 * W2)
        D16MAC_AA_LW(xr10,xr6,xr13,xr15);         // xr10(h2 * W2 + h6 * W6)    // xr15(x2 * W2 + x6 * W6)
        D16MACF_AA_WW(xr10, xr0, xr0, xr15);      // xr10((h2*W2 + h6*W6), (x2*W2 + x6*W6))
        Q16ADD_AS_WW(xr2,xr2,xr10,xr9);   //xr2(h0*W4 + h4*W4 + h2*W2 + h6*W6) (x0*W4 + x4*W4 + x2*W2 + x6*W6)
                                          //xr9(h0*W4 + h4*W4 - h2*W2 - h6*W6) (x0*W4 + x4*W4 - x2*W2 - x6*W6)
        D16MUL_HW(xr10, xr5, xr1, xr15);          // xr10 (h0 * W4) xr15 (x0 * W4)
        D16MAC_SS_HW(xr10,xr5,xr11,xr15);         // xr10 (h0 * W4 - h4 * W4)
                                                  // xr15 (x0 * W4 - x4 * W4)
        D16MACF_AA_WW(xr10, xr0, xr0, xr15);      // xr10 ((h0*W4 - h4*W4), (x0*W4 - x4*W4))
        D16MUL_LW(xr11, xr6, xr3, xr1);           // xr11 (h2 * W6) xr1  (x2 * W6)
        D16MAC_SS_LW(xr11,xr5,xr13,xr1);          // xr11 (h2 * W6 - h6 * W2)
                                                  // xr1  (x2 * W6 - x6 * W2)
        D16MACF_AA_WW(xr11, xr0, xr0, xr1);       // xr11 ((h2*W6 - h6*W2), (x2*W6 - x6*W2))
        Q16ADD_AS_WW(xr10,xr10,xr11,xr15); 
        S32LDD(xr1, blk, 0x10);                    //  xr1 (x4, x0)
        D16MUL_HW(xr11, xr7, xr1, xr13);           //
        D16MAC_AA_LW(xr11,xr7,xr4,xr13);           //
        D16MAC_AA_LW(xr11,xr8,xr12,xr13);          //
        D16MAC_AA_HW(xr11,xr8,xr14,xr13);          //
        D16MACF_AA_WW(xr11, xr0, xr0, xr13);       //


        D16MUL_LW(xr3, xr7, xr1, xr13);           
        D16MAC_SS_HW(xr3,xr8,xr4,xr13);           
        D16MAC_SS_HW(xr3,xr7,xr12,xr13);          
        D16MAC_SS_LW(xr3,xr8,xr14,xr13);          
        D16MACF_AA_WW(xr3, xr0, xr0, xr13);       


        D16MUL_LW(xr5, xr8, xr1, xr13);           
        D16MAC_SS_HW(xr5,xr7,xr4,xr13);           
        D16MAC_AA_HW(xr5,xr8,xr12,xr13);          
        D16MAC_AA_LW(xr5,xr7,xr14,xr13);          
        D16MACF_AA_WW(xr5, xr0, xr0, xr13); 
        D16MUL_HW(xr1, xr8, xr1, xr13);           
        D16MAC_SS_LW(xr1,xr8,xr4,xr13);           
        D16MAC_AA_LW(xr1,xr7,xr12,xr13);                 
        D16MAC_SS_HW(xr1,xr7,xr14,xr13);                
        D16MACF_AA_WW(xr1, xr0, xr0, xr13);  

        Q16ADD_AS_WW(xr2,xr2,xr11,xr11);         //
        Q16ADD_AS_WW(xr10,xr10,xr3,xr3);         //
        Q16ADD_AS_WW(xr15,xr15,xr5,xr5);         //

        Q16ADD_AS_WW(xr9,xr9,xr1,xr1);          //


        S32STD(xr2, blk, 0x00);
        S32STD(xr3, blk, 0x60);
        S32STD(xr10, blk, 0x10);
        S32STD(xr11, blk, 0x70);
        S32STD(xr1, blk, 0x40);
        S32STD(xr9, blk, 0x30);
        S32STD(xr15, blk, 0x20);
        S32STD(xr5, blk, 0x50);

    }

        S32I2M(xr11, ((R1 << 16) | (R2 & 0xffff)));                // xr11  (R1, R2)
        S32I2M(xr12, (1 << 14));                   // xr11  (R1, R2)
        S32I2M(xr13, (R3)) ;               // xr13 (R3)

        dest -= line_size;
        row = block;
        i = (unsigned int)row + 8 * 8 * 2;
    /* IDCT4 on each line */
    while ((unsigned int)row < i) {
    S32LDD(xr1,  row, 0x00);     //  xr1 (x1, x0)
    S32LDD(xr2,  row, 0x4);      //  xr2 (x3, x2)
    S32LDD(xr14, row, 0x10);     //  xr14 (x1, x0)
    S32LDD(xr15, row, 0x14);           //  xr15 (x3, x2)
    S32SFL(xr1, xr1, xr2, xr2, ptn3);  // xr1 (a1.xr1, a3.xr3) xr2 (a0.x0, a2.x2)

    D16MUL_HW    (xr3, xr1, xr11, xr4);    // xr3 (a1*R1)   xr4 (a1*R2)
    D16MAC_SA_LW (xr4, xr1, xr11, xr3);    // xr4.c3 (a1*R2 - a3*R1)   xr3.c1 (a1*R1 + a3*R2)
    D16MUL_LW(xr5, xr13, xr2, xr6);    // xr5 (R3*a0)   xr6 (R3*a2)
    D32ADD_AS(xr5, xr5, xr6, xr6);     // xr5.c0 (a0 + a2) *R3   xr6.c2 (a0-a2) *R3

    D32ADD_AS (xr9, xr5, xr3, xr10);   // xr9: (c0 + c1)   xr10: (c0 - c1)
    D32ADD_AS (xr7, xr6, xr4, xr8);    // xr7: (c2 + c3)   xr8: (c2 - c3)

    D32ACC_AS (xr7, xr12, xr0, xr9);   // xr7: (c2 + c3)   xr9: (c0 + c1)
    D32ACC_AS (xr10,xr12, xr0, xr8);   // xr10: (c0 - c1)   xr8: (c2 - c3)
    S32LDIV(xr1, dest, line_size, 0x00);          //  xr1 (dest3,dest2,dest1,dest0)

    D32SARL(xr7, xr7, xr9, 15);        // xr7 (c2+c3, c0+c1)
    D32SARL(xr8, xr10,xr8, 15);        // xr8 (c0-c1, c2-c3)

    Q8ACCE_AA(xr8, xr1, xr0, xr7);

//
    S32SFL(xr14, xr14, xr15, xr15, ptn3);  // xr14 (a1.xr1, a3.xr3) xr15 (a0.x0, a2.x2)

    D16MUL_HW    (xr3, xr14, xr11, xr4);    // xr3 (a1*R1)   xr4 (a1*R2)
    D16MAC_SA_LW (xr4, xr14, xr11, xr3);    // xr4.c3 (a1*R2 - a3*R1)   xr3.c1 (a1*R1 + a3*R2)

    D16MUL_LW(xr5, xr13, xr15, xr6);     // xr5 (R3*a0)   xr6 (R3*a2)
    D32ADD_AS(xr5, xr5, xr6, xr6);       // xr5.c0 (a0 + a2) *R3   xr6.c2 (a0-a2) *R3

    D32ADD_AS (xr1, xr5, xr3, xr2);      // xr1: (c0 + c1)   xr2: (c0 - c1)
    D32ADD_AS (xr3, xr6, xr4, xr4);      // xr3: (c2 + c3)   xr4: (c2 - c3)

    D32ACC_AS (xr3, xr12, xr0, xr1);     // xr3: (c2 + c3)   xr1: (c0 + c1)
    D32ACC_AS (xr2,xr12, xr0, xr4);      // xr2: (c0 - c1)   xr4: (c2 - c3)

    S32LDDV(xr5, dest, line_size, 0x00); //  xr1 (dest3,dest2,dest1,dest0)
    D32SARL(xr3, xr3, xr1, 15);          // xr3 (c2+c3, c0+c1)
    D32SARL(xr4, xr2,xr4, 15);           // xr4 (c0-c1, c2-c3)
    Q8ACCE_AA(xr4, xr5, xr0, xr3);

// store
    Q16SAT(xr8, xr8, xr7);
    Q16SAT(xr4, xr4, xr3);
    S32STD(xr8, dest, 0);
    S32SDIV(xr4, dest, line_size, 0);
    row += 16;
    }
}

#else
void simple_idct48_add(uint8_t *dest, int line_size, DCTELEM *block)
{
    int i;

    /* IDCT4 on each line */
    for(i=0; i<8; i++) {
        idct4row(block + i*8);
    }

    /* IDCT8 and store */
    for(i=0; i<4; i++){
        idctSparseColAdd(dest + i, line_size, block + i);
    }
}
#endif
#endif
