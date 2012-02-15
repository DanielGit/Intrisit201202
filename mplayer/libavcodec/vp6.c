/**
 * @file vp6.c
 * VP6 compatible video decoder
 *
 * Copyright (C) 2006  Aurelien Jacobs <aurel@gnuage.org>
 *
 * The VP6F decoder accepts an optional 1 byte extradata. It is composed of:
 *  - upper 4bits: difference between encoded width and visible width
 *  - lower 4bits: difference between encoded height and visible height
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

#ifdef __LINUX__
#include <stdlib.h>
#else
#include <mplaylib.h>
#endif

#include "avcodec.h"
#include "dsputil.h"
#include "bitstream.h"
#include "mpegvideo.h"

#include "vp56.h"
#include "vp56data.h"
#include "vp6data.h"
#ifdef JZ4750_OPT
#include "jzmedia.h"
#endif

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

static int vp6_parse_header(vp56_context_t *s, uint8_t *buf, int buf_size,
                            int *golden_frame)
{
    vp56_range_coder_t *c = &s->c;
    int parse_filter_info = 0;
    int coeff_offset = 0;
    int vrt_shift = 0;
    int sub_version;
    int rows, cols;
    int res = 1;
    int separated_coeff = buf[0] & 1;
    s->framep[VP56_FRAME_CURRENT]->key_frame = !(buf[0] & 0x80);
    vp56_init_dequant(s, (buf[0] >> 1) & 0x3F);

    if (s->framep[VP56_FRAME_CURRENT]->key_frame) {
        sub_version = buf[1] >> 3;
        if (sub_version > 8)
            return 0;
        s->filter_header = buf[1] & 0x06;
        if (buf[1] & 1) {
            av_log(s->avctx, AV_LOG_ERROR, "interlacing not supported\n");
            return 0;
        }
        if (separated_coeff || !s->filter_header) {
            coeff_offset = AV_RB16(buf+2) - 2;
            buf += 2;
            buf_size -= 2;
        }

        rows = buf[2];  /* number of stored macroblock rows */
        cols = buf[3];  /* number of stored macroblock cols */
        /* buf[4] is number of displayed macroblock rows */
        /* buf[5] is number of displayed macroblock cols */

        if (16*cols != s->avctx->coded_width ||
            16*rows != s->avctx->coded_height) {
            avcodec_set_dimensions(s->avctx, 16*cols, 16*rows);
            if (s->avctx->extradata_size == 1) {
                s->avctx->width  -= s->avctx->extradata[0] >> 4;
                s->avctx->height -= s->avctx->extradata[0] & 0x0F;
            }
            res = 2;
        }

        vp56_init_range_decoder(c, buf+6, buf_size-6);
        vp56_rac_gets(c, 2);

        parse_filter_info = s->filter_header;
        if (sub_version < 8)
            vrt_shift = 5;
        s->sub_version = sub_version;
    } else {
        if (!s->sub_version)
            return 0;

        if (separated_coeff || !s->filter_header) {
            coeff_offset = AV_RB16(buf+1) - 2;
            buf += 2;
            buf_size -= 2;
        }
        vp56_init_range_decoder(c, buf+1, buf_size-1);

        *golden_frame = vp56_rac_get(c);
        if (s->filter_header) {
            s->deblock_filtering = vp56_rac_get(c);
            if (s->deblock_filtering)
                vp56_rac_get(c);
            if (s->sub_version > 7)
                parse_filter_info = vp56_rac_get(c);
        }
    }

    if (parse_filter_info) {
        if (vp56_rac_get(c)) {
            s->filter_mode = 2;
            s->sample_variance_threshold = vp56_rac_gets(c, 5) << vrt_shift;
            s->max_vector_length = 2 << vp56_rac_gets(c, 3);
        } else if (vp56_rac_get(c)) {
            s->filter_mode = 1;
        } else {
            s->filter_mode = 0;
        }
        if (s->sub_version > 7)
            s->filter_selection = vp56_rac_gets(c, 4);
        else
            s->filter_selection = 16;
    }

    if (vp56_rac_get(c))
        av_log(s->avctx, AV_LOG_WARNING,
               "alternative entropy decoding not supported\n");

    if (coeff_offset) {
        vp56_init_range_decoder(&s->cc, buf+coeff_offset,
                                buf_size-coeff_offset);
        s->ccp = &s->cc;
    } else {
        s->ccp = &s->c;
    }

    return res;
}

static void vp6_coeff_order_table_init(vp56_context_t *s)
{
    int i, pos, idx = 1;

    s->modelp->coeff_index_to_pos[0] = 0;
    for (i=0; i<16; i++)
        for (pos=1; pos<64; pos++)
            if (s->modelp->coeff_reorder[pos] == i)
                s->modelp->coeff_index_to_pos[idx++] = pos;
}

static void vp6_default_models_init(vp56_context_t *s)
{
    vp56_model_t *model = s->modelp;

    model->vector_dct[0] = 0xA2;
    model->vector_dct[1] = 0xA4;
    model->vector_sig[0] = 0x80;
    model->vector_sig[1] = 0x80;

    memcpy(model->mb_types_stats, vp56_def_mb_types_stats, sizeof(model->mb_types_stats));
    memcpy(model->vector_fdv, vp6_def_fdv_vector_model, sizeof(model->vector_fdv));
    memcpy(model->vector_pdv, vp6_def_pdv_vector_model, sizeof(model->vector_pdv));
    memcpy(model->coeff_runv, vp6_def_runv_coeff_model, sizeof(model->coeff_runv));
    memcpy(model->coeff_reorder, vp6_def_coeff_reorder, sizeof(model->coeff_reorder));

    vp6_coeff_order_table_init(s);
}

static void vp6_parse_vector_models(vp56_context_t *s)
{
    vp56_range_coder_t *c = &s->c;
    vp56_model_t *model = s->modelp;
    int comp, node;

    for (comp=0; comp<2; comp++) {
        if (vp56_rac_get_prob(c, vp6_sig_dct_pct[comp][0]))
            model->vector_dct[comp] = vp56_rac_gets_nn(c, 7);
        if (vp56_rac_get_prob(c, vp6_sig_dct_pct[comp][1]))
            model->vector_sig[comp] = vp56_rac_gets_nn(c, 7);
    }

    for (comp=0; comp<2; comp++)
        for (node=0; node<7; node++)
            if (vp56_rac_get_prob(c, vp6_pdv_pct[comp][node]))
                model->vector_pdv[comp][node] = vp56_rac_gets_nn(c, 7);

    for (comp=0; comp<2; comp++)
        for (node=0; node<8; node++)
            if (vp56_rac_get_prob(c, vp6_fdv_pct[comp][node]))
                model->vector_fdv[comp][node] = vp56_rac_gets_nn(c, 7);
}

static void vp6_parse_coeff_models(vp56_context_t *s)
{
    vp56_range_coder_t *c = &s->c;
    vp56_model_t *model = s->modelp;
    int def_prob[11];
    int node, cg, ctx, pos;
    int ct;    /* code type */
    int pt;    /* plane type (0 for Y, 1 for U or V) */

    memset(def_prob, 0x80, sizeof(def_prob));

    for (pt=0; pt<2; pt++)
        for (node=0; node<11; node++)
            if (vp56_rac_get_prob(c, vp6_dccv_pct[pt][node])) {
                def_prob[node] = vp56_rac_gets_nn(c, 7);
                model->coeff_dccv[pt][node] = def_prob[node];
            } else if (s->framep[VP56_FRAME_CURRENT]->key_frame) {
                model->coeff_dccv[pt][node] = def_prob[node];
            }

    if (vp56_rac_get(c)) {
        for (pos=1; pos<64; pos++)
            if (vp56_rac_get_prob(c, vp6_coeff_reorder_pct[pos]))
                model->coeff_reorder[pos] = vp56_rac_gets(c, 4);
        vp6_coeff_order_table_init(s);
    }

    for (cg=0; cg<2; cg++)
        for (node=0; node<14; node++)
            if (vp56_rac_get_prob(c, vp6_runv_pct[cg][node]))
                model->coeff_runv[cg][node] = vp56_rac_gets_nn(c, 7);

    for (ct=0; ct<3; ct++)
        for (pt=0; pt<2; pt++)
            for (cg=0; cg<6; cg++)
                for (node=0; node<11; node++)
                    if (vp56_rac_get_prob(c, vp6_ract_pct[ct][pt][cg][node])) {
                        def_prob[node] = vp56_rac_gets_nn(c, 7);
                        model->coeff_ract[pt][ct][cg][node] = def_prob[node];
                    } else if (s->framep[VP56_FRAME_CURRENT]->key_frame) {
                        model->coeff_ract[pt][ct][cg][node] = def_prob[node];
                    }

    /* coeff_dcct is a linear combination of coeff_dccv */
    for (pt=0; pt<2; pt++)
        for (ctx=0; ctx<3; ctx++)
            for (node=0; node<5; node++)
                model->coeff_dcct[pt][ctx][node] = av_clip(((model->coeff_dccv[pt][node] * vp6_dccv_lc[ctx][node][0] + 128) >> 8) + vp6_dccv_lc[ctx][node][1], 1, 255);
}

static void vp6_parse_vector_adjustment(vp56_context_t *s, vp56_mv_t *vect)
{
    vp56_range_coder_t *c = &s->c;
    vp56_model_t *model = s->modelp;
    int comp;

    *vect = (vp56_mv_t) {0,0};
    if (s->vector_candidate_pos < 2)
        *vect = s->vector_candidate[0];

    for (comp=0; comp<2; comp++) {
        int i, delta = 0;

        if (vp56_rac_get_prob(c, model->vector_dct[comp])) {
            static const uint8_t prob_order[] = {0, 1, 2, 7, 6, 5, 4};
            for (i=0; i<sizeof(prob_order); i++) {
                int j = prob_order[i];
                delta |= vp56_rac_get_prob(c, model->vector_fdv[comp][j])<<j;
            }
            if (delta & 0xF0)
                delta |= vp56_rac_get_prob(c, model->vector_fdv[comp][3])<<3;
            else
                delta |= 8;
        } else {
            delta = vp56_rac_get_tree(c, vp56_pva_tree,
                                      model->vector_pdv[comp]);
        }

        if (delta && vp56_rac_get_prob(c, model->vector_sig[comp]))
            delta = -delta;

        if (!comp)
            vect->x += delta;
        else
            vect->y += delta;
    }
}

static void vp6_parse_coeff(vp56_context_t *s)
{
    vp56_range_coder_t *c = s->ccp;
    vp56_model_t *model = s->modelp;
    uint8_t *permute = s->scantable.permutated;
    uint8_t *model1, *model2, *model3;
    int coeff, sign, coeff_idx;
    int b, i, cg, idx, ctx;
    int pt = 0;    /* plane type (0 for Y, 1 for U or V) */

    for (b=0; b<6; b++) {
        int ct = 1;    /* code type */
        int run = 1;

        if (b > 3) pt = 1;

        ctx = s->left_block[vp56_b6to4[b]].not_null_dc
              + s->above_blocks[s->above_block_idx[b]].not_null_dc;
        model1 = model->coeff_dccv[pt];
        model2 = model->coeff_dcct[pt][ctx];

        for (coeff_idx=0; coeff_idx<64; ) {
            if ((coeff_idx>1 && ct==0) || vp56_rac_get_prob(c, model2[0])) {
                /* parse a coeff */
                if (vp56_rac_get_prob(c, model2[2])) {
                    if (vp56_rac_get_prob(c, model2[3])) {
                        idx = vp56_rac_get_tree(c, vp56_pc_tree, model1);
                        coeff = vp56_coeff_bias[idx];
                        for (i=vp56_coeff_bit_length[idx]; i>=0; i--)
                            coeff += vp56_rac_get_prob(c, vp56_coeff_parse_table[idx][i]) << i;
                    } else {
                        if (vp56_rac_get_prob(c, model2[4]))
                            coeff = 3 + vp56_rac_get_prob(c, model1[5]);
                        else
                            coeff = 2;
                    }
                    ct = 2;
                } else {
                    ct = 1;
                    coeff = 1;
                }
                sign = vp56_rac_get(c);
                coeff = (coeff ^ -sign) + sign;
                if (coeff_idx)
                    coeff *= s->dequant_ac;
                idx = model->coeff_index_to_pos[coeff_idx];
                s->block_coeff[b][permute[idx]] = coeff;
                run = 1;
            } else {
                /* parse a run */
                ct = 0;
                if (coeff_idx > 0) {
                    if (!vp56_rac_get_prob(c, model2[1]))
                        break;

                    model3 = model->coeff_runv[coeff_idx >= 6];
                    run = vp56_rac_get_tree(c, vp6_pcr_tree, model3);
                    if (!run)
                        for (run=9, i=0; i<6; i++)
                            run += vp56_rac_get_prob(c, model3[i+8]) << i;
                }
            }

            cg = vp6_coeff_groups[coeff_idx+=run];
            model1 = model2 = model->coeff_ract[pt][ct][cg];
        }

        s->left_block[vp56_b6to4[b]].not_null_dc =
        s->above_blocks[s->above_block_idx[b]].not_null_dc = !!s->block_coeff[b][0];
    }
}

static int vp6_adjust(int v, int t)
{
    int V = v, s = v >> 31;
    V ^= s;
    V -= s;
    if (V-t-1 >= (unsigned)(t-1))
        return v;
    V = 2*t - V;
    V += s;
    V ^= s;
    return V;
}

#ifdef JZ4750_OPT
static void vp6_edge_filter_x(vp56_context_t *s, uint8_t *yuv,
                             int pix_inc, int line_inc, int t)
{
    int i;

    S32I2M(xr15, t);  //xr15: t
    S32SFL(xr0,xr15,xr15,xr15,ptn3); //xr15: t, t
    Q16SLL(xr0,xr0,xr15,xr14,1);     //xr14: 2*t, 2*t
    S32I2M(xr13,0x00040004);
    for (i=0; i<12; i++) {
        S8LDD(xr1, yuv, 0, ptn7);
        S8LDD(xr2, yuv, -1, ptn7);
        S8LDD(xr1, yuv, -2, ptn0);  //xr1: yuv[0], yuv[0], yuv[0], yuv[-2]
        S8LDD(xr2, yuv, 1, ptn0); //xr2: yuv[-1],yuv[-1],yuv[-1],yuv[1]
        S32SFL(xr12,xr1,xr2,xr0,ptn0);//xr12:yuv[0],yuv[0],yuv[-1],yuv[-1]
        Q8ADDE_SS(xr12,xr12,xr0,xr0); //xr12:sign_ext16(yuv[0]),sign_ext16(yuv[-1]), 
        Q8ADDE_SS(xr1,xr1,xr2,xr2);//xr1: yuv[0]-yuv[-1], yuv[0]-yuv[-1]
                                   //xr2: yuv[0]-yuv[-1], yuv[-2]-yuv[1]
        Q16ACCM_AA(xr1,xr2,xr1,xr2);
        //xr1:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1]), (yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])
        //xr2:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1]), (yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])
        Q16ADD_AA_XW(xr1,xr1,xr2,xr2);
        //xr1:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1]),
        //   :(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])
        //xr2:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1]),
        //   :(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])
        Q16ACCM_AA(xr1,xr13,xr13,xr2);
        //xr1:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])+4,
        //   :(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])+4
        //xr2:(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])+4,
        //   :(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[0]-yuv[-1])+(yuv[-2]-yuv[1])+4
        Q16SAR(xr1,xr1,xr0,xr0,3);
        S32SFL(xr0,xr0,xr1,xr1,ptn3);
        Q16SAR(xr2,xr1,xr0,xr0,15); //xr2=xr1>>15, vs=v>>15
        S32XOR(xr3,xr2,xr1);        //xr3=xr2^xr1, V ^= vs
        Q16ADD_SS_WW(xr3,xr3,xr2,xr0);//xr3=xr3-xr2, V -=vs
        D16SLT(xr4,xr3,xr14);       //if V<2t then xr4=1 else xr4=0
        D16SLT(xr5,xr15,xr3);       //if t<V  then xr5=1 else xr5=0
        S32AND(xr4,xr4,xr5);
        Q16ADD_SS_WW(xr3,xr14,xr3,xr0);//xr3=xr3-xr14, V=2*t-V
        S32SFL(xr0,xr0,xr3,xr3,ptn3);  
        Q16ADD_AA_WW(xr3,xr3,xr2,xr0); //xr3=xr3+xr2,  V+=vs
        S32XOR(xr3,xr3,xr2);           //xr3=xr3^xr2,  V^=vs
        D16MOVN(xr1,xr4,xr3);
        S32SFL(xr0, xr1, xr1,xr1, ptn3);//xr1:v,v
        Q16ADD_AS_WW(xr12,xr12,xr1,xr1);//xr12:yuv[0]+v, yuv[-1]+v
                                        //xr1: yuv[0]-v, yuv[-1]-v
        Q16SAT(xr1,xr12,xr1);           //xr1: sat(yuv[0]+v), sat(yuv[-1]+v),
                                        //   : sat(yuv[0]-v), sat(yuv[-1]-v)
        S8STD(xr1,yuv,0,ptn1);
        S8STD(xr1,yuv,-1,ptn2);
        yuv += line_inc;
    }
}
/*
//The following is c example of mux code vp6_edge_filter_x().
static void vp6_edge_filter_x(vp56_context_t *s, uint8_t *yuv,
                             int pix_inc, int line_inc, int t)
{
    int i;
    int32_t V, vs, v, t2;
    uint8_t *yuv_bak;
    yuv_bak = yuv;

    t2 = 2*t;
    for (i=0; i<12; i++) {
        v = (yuv[-2] + 3*(yuv[0]-yuv[-1]) - yuv[1] + 4) >>3;
        V=v;
        vs=v>>31;
        V ^= vs;
        V -= vs;
        if(V < t2&&V>t){   // t<abs(v)<2t ?
         V=t2-V;           //if v>0 then v=t2-v
         V += vs;          //if v<0 then v=abs(v)-t2
         V ^= vs;
         v=V;
        }
        yuv[-1] = av_clip_uint8(yuv[-1] + v);
        yuv[0] = av_clip_uint8(yuv[0] - v);
        yuv += line_inc;
    }
}
*/
#endif
#ifdef JZ4750_OPT
static void vp6_edge_filter_y(vp56_context_t *s, uint8_t *yuv,
                             int pix_inc, int line_inc, int t)
{
    int pix2_inc = 2 * pix_inc;
    int i, v;
    int V, vs;

    for (i=0; i<12; i++) {
        v = (yuv[-pix2_inc] + 3*(yuv[0]-yuv[-pix_inc]) - yuv[pix_inc] + 4) >>3;
        V=v;
        vs=v>>31;
        V ^= vs;
        V -= vs;
        if(V-t-1 < (uint32_t)(t-1)){
         V=2*t-V;
         V += vs;
         V ^= vs;
         v=V;
        }
        yuv[-pix_inc] = av_clip_uint8(yuv[-pix_inc] + v);
        yuv[0] = av_clip_uint8(yuv[0] - v);
        yuv += line_inc;
    }
}
#endif
#ifdef JZ4750_OPT
static int vp6_block_variance(uint8_t *src, int stride)
{
    int sum = 0, square_sum = 0;
    int y, x;
    uint32_t src_aln, src_rst;
    int stride2=2*stride;

    src_aln = ((uint32_t)src)&0xFFFFFFFC;
    src_rst = 4 - (((uint32_t)src)&0x3);
    src_aln -= stride2;
    D32SLL(xr14,xr0,xr0,xr15,1);//xr14, xr15=square_sum
    D32SLL(xr12,xr0,xr0,xr13,1);//xr12, xr13=sum
    for (y=0; y<8; y+=2) {
        S32LDIV(xr1,src_aln, stride2, 0);
        S32LDD(xr2,src_aln, 0x4);
        S32LDD(xr3,src_aln, 0x8);
        S32ALN(xr1,xr2,xr1, src_rst);//xr1=src[3],src[2],src[1],src[0]
        S32ALN(xr2,xr3,xr2, src_rst);//xr2=src[7],src[6],src[5],src[4]
        S32SFL(xr0,xr2,xr1,xr1,ptn1);//xr1=src[6],src[4],src[2],src[0]
        Q8MUL(xr2,xr1,xr1,xr3);      //xr2=src[6]*src[6],src[4]*src[4]
                                     //xr3=src[2]*src[2],src[0]*src[0]
        Q8SAD(xr0,xr1,xr0,xr12);          //xr12+=src[0]+src[2]+src[4]+src[6]
        S32SFL(xr4,xr0,xr2,xr2,ptn3);//xr4:0,src[6]*src[6]
                                     //xr2:0,src[4]*src[4]
        S32SFL(xr5,xr0,xr3,xr3,ptn3);//xr5:0,src[2]*src[2]
                                     //xr3:0,src[0]*src[0]
        D32ASUM_AA(xr14,xr2,xr3,xr15);
        D32ASUM_AA(xr14,xr4,xr5,xr15);
    }
    sum=S32M2I(xr12);
    D32ADD_AA(xr10,xr14,xr15,xr0);
    D32SLL(xr10,xr10,xr0,xr0,4);
    sum=sum*sum;
    square_sum=S32M2I(xr10);
    return (square_sum - sum) >> 8;
}
#else
static int vp6_block_variance(uint8_t *src, int stride)
{
    int sum = 0, square_sum = 0;
    int y, x;

    for (y=0; y<8; y+=2) {
        for (x=0; x<8; x+=2) {
            sum += src[x];
            square_sum += src[x]*src[x];
        }
        src += 2*stride;
    }
    return (16*square_sum - sum*sum) >> 8;
}
#endif
#ifdef JZ4750_OPT
static void vp6_filter_hv4_h(uint8_t *dst, uint8_t *src, int stride,
                           int delta, const int16_t *weights)
{
    int x, y;
    uint32_t src_aln, src_rst;
    src_aln=(uint32_t)(src-1);
    src_rst=4-(src_aln&0x3);
    src_aln=src_aln&0xFFFFFFFC;
    src_aln -= stride;
    S32LDD(xr10,weights,0x0);
    S32LDD(xr11,weights,0x4);
    S32I2M(xr9, 64);                 //xr15:64
    for (y=0; y<8; y++) {
        S32LDIV(xr1, src_aln, stride, 0x0);
        S32LDD(xr2, src_aln, 0x4);
        S32LDD(xr3, src_aln, 0x8);
        S32LDD(xr4, src_aln, 0xc);
        S32ALN(xr13, xr2, xr1, src_rst);//xr13:src[2],src[1],src[0],src[-1]
        S32ALN(xr14, xr3, xr2, src_rst);//xr14:src[6],src[5],src[4],src[3]
        S32ALN(xr15, xr4, xr3, src_rst);//xr15:src[10],src[9],src[8],src[7]
        for (x=0; x<4; x++) {
            S32ALN(xr1, xr14,xr13,4-x);
            S32ALN(xr5, xr15,xr14,4-x);
            S32SFL(xr2, xr0, xr1, xr1, ptn0);//xr2:0,src[2],0,src[1]
                                             //xr1:0,src[0],0,src[-1]
            S32SFL(xr6, xr0, xr5, xr5, ptn0);//xr6:0,src[6],0,src[5]
                                             //xr5:0,src[4],0,src[3]
            D16MUL_WW(xr3, xr1, xr10,xr4);//xr3:src[0]*weights[1]
                                          //xr4:src[-1]*weights[0]
            D16MUL_WW(xr7, xr5, xr10,xr8);//xr7:src[4]*weights[1]
                                          //xr4:src[3]*weights[0]
            D16MAC_AA_WW(xr3,xr2,xr11,xr4);//xr3:src[0]*weights[1]+src[2]*weithts[3]
                   //xr4:src[-1]*weights[0]+src[1]*weights[2]
            D16MAC_AA_WW(xr7,xr6,xr11,xr8);//xr7:src[4]*weights[1]+src[6]*weithts[3]
                   //xr8:src[3]*weights[0]+src[5]*weights[2]
            D32ACC_AA(xr3,xr4,xr9,xr0);//xr3:src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64
            D32ACC_AA(xr7,xr8,xr9,xr0);//xr7:src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64
            D32SAR(xr3,xr3,xr7,xr7,7);
       //xr3:(src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64)>>7
       //xr7:(src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64)>>7
            Q16SAT(xr3,xr7,xr3);
       //xr3_low:sat_8((src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64)>>7)
       //xr3_high:sat_8((src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64)>>7)
            S8STD(xr3,dst+x,0,ptn0);
            S8STD(xr3,dst+x,4,ptn2);
        }
        dst += stride;
    }
}
#else
static void vp6_filter_hv4_h(uint8_t *dst, uint8_t *src, int stride,
                           int delta, const int16_t *weights)
{
    int x, y;

    for (y=0; y<8; y++) {
        for (x=0; x<8; x++) {
            dst[x] = av_clip_uint8((  src[x-delta  ] * weights[0]
                                 + src[x        ] * weights[1]
                                 + src[x+delta  ] * weights[2]
                                 + src[x+2*delta] * weights[3] + 64) >> 7);
        }
        src += stride;
        dst += stride;
    }
}
#endif
#ifdef JZ4750_OPT
static void vp6_filter_hv4_v(uint8_t *dst, uint8_t *src, int stride,
                           int delta, const int16_t *weights)
{
    int x, y;
    uint32_t src_aln, src_rst;
    src_aln=(uint32_t)(src);
    src_rst=4-(src_aln&0x3);
    src_aln=src_aln&0xFFFFFFFC;
    src_aln =src_aln- stride-4;
    S32LDD(xr10,weights,0x0);
    S32LDD(xr11,weights,0x4);
    S32I2M(xr9, 64);                 //xr15:64
    for (y=0; y<8; y++) {
        for (x=0; x<2; x++) {
            S32LDI(xr1, src_aln, 0x4);
            S32LDD(xr2, src_aln, 0x4);
            S32LDIV(xr3, src_aln, stride, 0x0);
            S32LDD(xr4, src_aln, 0x4);
            S32ALN(xr1, xr2, xr1, src_rst);
            S32SFL(xr2, xr0, xr1, xr1, ptn0);//xr2:0,src[3-stride],0,src[2-stride]
                                             //xr1:0,src[1-stride],0,src[0-stride]
            D16MUL_LW(xr13,xr10,xr1,xr12);//xr13:s1*w0,xr12:s0*w0
            D16MUL_LW(xr15,xr10,xr2,xr14);//xr15:s3*w0,xr14:s2*w0
            S32ALN(xr3, xr4, xr3, src_rst);
            S32SFL(xr4, xr0, xr3, xr3, ptn0);//xr4:0,src[3],0,src[2]
                                             //xr3:0,src[1],0,src[0]
            D16MAC_AA_HW(xr13,xr10,xr3,xr12);//xr13:s1'*w1+s1*w0,xr12:s0'*w1+s0*w0
            D16MAC_AA_HW(xr15,xr10,xr4,xr14);//xr15:s3'*w1+s3*w0,xr14:s2'*w1+s2*w0
            S32LDIV(xr5, src_aln, stride, 0x0);
            S32LDD(xr6, src_aln, 0x4);
            S32LDIV(xr7, src_aln, stride, 0x0);
            S32LDD(xr8, src_aln, 0x4);
            S32ALN(xr5, xr6, xr5, src_rst);
            S32SFL(xr6, xr0, xr5, xr5, ptn0);
               //xr6:0,src[3+stride],0,src[2+stride]
               //xr5:0,src[1+stride],0,src[0+stride]
            D16MAC_AA_LW(xr13,xr11,xr5,xr12);
              //xr13:s1"*w2+s1'*w1+s1*w0,xr12:s0"*w2+s0'*w1+s0*w0
            D16MAC_AA_LW(xr15,xr11,xr6,xr14);//xr15:s3"*w2+s3'*w1+s3*w0,xr14:s2"*w2+s2'*w1+s2*w0

            S32ALN(xr7, xr8, xr7, src_rst);
            S32SFL(xr8, xr0, xr7, xr7, ptn0);
               //xr8:0,src[3+2*stride],0,src[2+2*stride]
               //xr7:0,src[1+2*stride],0,src[0+2*stride]
            D16MAC_AA_HW(xr13,xr11,xr7,xr12);
              //xr13:s1^*w3+s1"*w2+s1'*w1+s1*w0,xr12:s0^*w3+s0"*w2+s0'*w1+s0*w0
            D16MAC_AA_HW(xr15,xr11,xr8,xr14);//xr15:s3^*w3+s3"*w2+s3'*w1+s3*w0,xr14:s2^*w3+s2"*w2+s2'*w1+s2*w0

            D32ASUM_AA(xr13,xr9,xr9,xr12);
            D32ASUM_AA(xr15,xr9,xr9,xr14);
            D32SAR(xr13,xr13,xr12,xr12,7);
            D32SAR(xr15,xr15,xr14,xr14,7);
            S32SFL(xr0,xr13,xr12,xr13,ptn3);
            S32SFL(xr0,xr15,xr14,xr15,ptn3);
            Q16SAT(xr3,xr15,xr13);
            S32STDV(xr3,dst,x,2);
        src_aln = src_aln- 3*stride;
        }
        src_aln = src_aln+stride-8;
        dst += stride;
    }
}
#else
static void vp6_filter_hv4_v(uint8_t *dst, uint8_t *src, int stride,
                           int delta, const int16_t *weights)
{
    int x, y;

    for (y=0; y<8; y++) {
        for (x=0; x<8; x++) {
            dst[x] = av_clip_uint8((  src[x-stride  ] * weights[0]
                                 + src[x        ] * weights[1]
                                 + src[x+stride  ] * weights[2]
                                 + src[x+2*stride] * weights[3] + 64) >> 7);
        }
        src += stride;
        dst += stride;
    }
}
#endif
static void vp6_filter_diag2(vp56_context_t *s, uint8_t *dst, uint8_t *src,
                             int stride, int h_weight, int v_weight)
{
    uint8_t *tmp = s->edge_emu_buffer+16;
    s->dsp.put_h264_chroma_pixels_tab[0](tmp, src, stride, 9, h_weight, 0);
    s->dsp.put_h264_chroma_pixels_tab[0](dst, tmp, stride, 8, 0, v_weight);
}
#ifdef JZ4750_OPT
static void vp6_filter_diag4(uint8_t *dst, uint8_t *src, int stride,
                             const int16_t *h_weights,const int16_t *v_weights)
{
    int x, y;
    uint8_t tmp[8*11];
    uint8_t *t = tmp;
    uint32_t src_aln, src_rst;
    src_aln=(uint32_t)(src-1-stride);
    src_rst=4-(src_aln&0x3);
    src_aln=src_aln&0xFFFFFFFC;
    src_aln -= stride;
    S32LDD(xr10,h_weights,0x0);
    S32LDD(xr11,h_weights,0x4);
    S32I2M(xr9, 64);                 //xr15:64

    for (y=0; y<11; y++) {
        S32LDIV(xr1, src_aln, stride, 0x0);
        S32LDD(xr2, src_aln, 0x4);
        S32LDD(xr3, src_aln, 0x8);
        S32LDD(xr4, src_aln, 0xc);
        S32ALN(xr13, xr2, xr1, src_rst);//xr13:src[2],src[1],src[0],src[-1]
        S32ALN(xr14, xr3, xr2, src_rst);//xr14:src[6],src[5],src[4],src[3]
        S32ALN(xr15, xr4, xr3, src_rst);//xr15:src[10],src[9],src[8],src[7]
        for (x=0; x<4; x++) {
            S32ALN(xr1, xr14,xr13,4-x);
            S32ALN(xr5, xr15,xr14,4-x);
            S32SFL(xr2, xr0, xr1, xr1, ptn0);//xr2:0,src[2],0,src[1]
                                             //xr1:0,src[0],0,src[-1]
            S32SFL(xr6, xr0, xr5, xr5, ptn0);//xr6:0,src[6],0,src[5]
                                             //xr5:0,src[4],0,src[3]
            D16MUL_WW(xr3, xr1, xr10,xr4);//xr3:src[0]*weights[1]
                                          //xr4:src[-1]*weights[0]
            D16MUL_WW(xr7, xr5, xr10,xr8);//xr7:src[4]*weights[1]
                                          //xr4:src[3]*weights[0]
            D16MAC_AA_WW(xr3,xr2,xr11,xr4);//xr3:src[0]*weights[1]+src[2]*weithts[3]
                   //xr4:src[-1]*weights[0]+src[1]*weights[2]
            D16MAC_AA_WW(xr7,xr6,xr11,xr8);//xr7:src[4]*weights[1]+src[6]*weithts[3]
                   //xr8:src[3]*weights[0]+src[5]*weights[2]
            D32ACC_AA(xr3,xr4,xr9,xr0);//xr3:src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64
            D32ACC_AA(xr7,xr8,xr9,xr0);//xr7:src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64
            D32SAR(xr3,xr3,xr7,xr7,7);
       //xr3:(src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64)>>7
       //xr7:(src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64)>>7
            Q16SAT(xr3,xr7,xr3);
       //xr3_low:sat_8((src[0]*weights[1]+src[2]*weithts[3]+src[-1]*weights[0]+src[1]*weights[2]+64)>>7)
       //xr3_high:sat_8((src[4]*weights[1]+src[6]*weithts[3]+src[3]*weights[0]+src[5]*weights[2]+64)>>7)
            S8STD(xr3,t+x,0,ptn0);
            S8STD(xr3,t+x,4,ptn2);
        }
        t += 8;
    }

    S32LDD(xr10,v_weights,0x0);
    S32LDD(xr11,v_weights,0x4);
    src_aln = tmp - 4;
    for (y=0; y<8; y++) {
        for (x=0; x<2; x++) {
            S32LDI(xr1, src_aln, 0x4);
            S32LDD(xr3, src_aln, 0x8);
            S32LDD(xr5, src_aln, 0x10);
            S32LDD(xr7, src_aln, 0x18);
            S32SFL(xr2, xr0, xr1, xr1, ptn0);//xr2:0,src[3-stride],0,src[2-stride]
                                             //xr1:0,src[1-stride],0,src[0-stride]
            D16MUL_LW(xr13,xr10,xr1,xr12);//xr13:s1*w0,xr12:s0*w0
            D16MUL_LW(xr15,xr10,xr2,xr14);//xr15:s3*w0,xr14:s2*w0
            S32SFL(xr4, xr0, xr3, xr3, ptn0);//xr4:0,src[3],0,src[2]
                                             //xr3:0,src[1],0,src[0]
            D16MAC_AA_HW(xr13,xr10,xr3,xr12);//xr13:s1'*w1+s1*w0,xr12:s0'*w1+s0*w0
            D16MAC_AA_HW(xr15,xr10,xr4,xr14);//xr15:s3'*w1+s3*w0,xr14:s2'*w1+s2*w0
            S32SFL(xr6, xr0, xr5, xr5, ptn0);
               //xr6:0,src[3+stride],0,src[2+stride]
               //xr5:0,src[1+stride],0,src[0+stride]
            D16MAC_AA_LW(xr13,xr11,xr5,xr12);
              //xr13:s1"*w2+s1'*w1+s1*w0,xr12:s0"*w2+s0'*w1+s0*w0
            D16MAC_AA_LW(xr15,xr11,xr6,xr14);//xr15:s3"*w2+s3'*w1+s3*w0,xr14:s2"*w2+s2'*w1+s2*w0

            S32SFL(xr8, xr0, xr7, xr7, ptn0);
               //xr8:0,src[3+2*stride],0,src[2+2*stride]
               //xr7:0,src[1+2*stride],0,src[0+2*stride]
            D16MAC_AA_HW(xr13,xr11,xr7,xr12);
              //xr13:s1^*w3+s1"*w2+s1'*w1+s1*w0,xr12:s0^*w3+s0"*w2+s0'*w1+s0*w0
            D16MAC_AA_HW(xr15,xr11,xr8,xr14);//xr15:s3^*w3+s3"*w2+s3'*w1+s3*w0,xr14:s2^*w3+s2"*w2+s2'*w1+s2*w0

            D32ASUM_AA(xr13,xr9,xr9,xr12);
            D32ASUM_AA(xr15,xr9,xr9,xr14);
            D32SAR(xr13,xr13,xr12,xr12,7);
            D32SAR(xr15,xr15,xr14,xr14,7);
            S32SFL(xr0,xr13,xr12,xr13,ptn3);
            S32SFL(xr0,xr15,xr14,xr15,ptn3);
            Q16SAT(xr3,xr15,xr13);
            S32STDV(xr3,dst,x,2);
        }
        dst += stride;
    }
}
#else
static void vp6_filter_diag4(uint8_t *dst, uint8_t *src, int stride,
                             const int16_t *h_weights,const int16_t *v_weights)
{
    int x, y;
    int tmp[8*11];
    int *t = tmp;

    src -= stride;

    for (y=0; y<11; y++) {
        for (x=0; x<8; x++) {
            t[x] = av_clip_uint8((  src[x-1] * h_weights[0]
                               + src[x  ] * h_weights[1]
                               + src[x+1] * h_weights[2]
                               + src[x+2] * h_weights[3] + 64) >> 7);
        }
        src += stride;
        t += 8;
    }

    t = tmp + 8;
    for (y=0; y<8; y++) {
        for (x=0; x<8; x++) {
            dst[x] = av_clip_uint8((  t[x-8 ] * v_weights[0]
                                 + t[x   ] * v_weights[1]
                                 + t[x+8 ] * v_weights[2]
                                 + t[x+16] * v_weights[3] + 64) >> 7);
        }
        dst += stride;
        t += 8;
    }
}
#endif

static void vp6_filter(vp56_context_t *s, uint8_t *dst, uint8_t *src,
                       int offset1, int offset2, int stride,
                       vp56_mv_t mv, int mask, int select, int luma)
{
    int filter4 = 0;
    int x8 = mv.x & mask;
    int y8 = mv.y & mask;

    if (luma) {
        x8 *= 2;
        y8 *= 2;
        filter4 = s->filter_mode;
        if (filter4 == 2) {
            if (s->max_vector_length &&
                (FFABS(mv.x) > s->max_vector_length ||
                 FFABS(mv.y) > s->max_vector_length)) {
                filter4 = 0;
            } else if (s->sample_variance_threshold
                       && (vp6_block_variance(src+offset1, stride)
                           < s->sample_variance_threshold)) {
                filter4 = 0;
            }
        }
    }

    if ((y8 && (offset2-offset1)*s->flip<0) || (!y8 && offset1 > offset2)) {
        offset1 = offset2;
    }

    if (filter4) {
        if (!y8) {                      /* left or right combine */
            vp6_filter_hv4_h(dst, src+offset1, stride, 1,
                           vp6_block_copy_filter[select][x8]);
        } else if (!x8) {               /* above or below combine */
            vp6_filter_hv4_v(dst, src+offset1, stride, stride,
                           vp6_block_copy_filter[select][y8]);
        } else {
            vp6_filter_diag4(dst, src+offset1 + ((mv.x^mv.y)>>31), stride,
                             vp6_block_copy_filter[select][x8],
                             vp6_block_copy_filter[select][y8]);
        }
    } else {
        if (!x8 || !y8) {
            s->dsp.put_h264_chroma_pixels_tab[0](dst, src+offset1, stride, 8, x8, y8);
        } else {
            vp6_filter_diag2(s, dst, src+offset1 + ((mv.x^mv.y)>>31), stride, x8, y8);
        }
    }
}

static int vp6_decode_init(AVCodecContext *avctx)
{
    vp56_context_t *s = avctx->priv_data;

    vp56_init(avctx, avctx->codec->id == CODEC_ID_VP6,
                     avctx->codec->id == CODEC_ID_VP6A);
    s->vp56_coord_div = vp6_coord_div;
    s->parse_vector_adjustment = vp6_parse_vector_adjustment;
    s->adjust = vp6_adjust;
#ifdef JZ4750_OPT
    s->edge_filter_x = vp6_edge_filter_x;
    s->edge_filter_y = vp6_edge_filter_y;
#endif
    s->filter = vp6_filter;
    s->parse_coeff = vp6_parse_coeff;
    s->default_models_init = vp6_default_models_init;
    s->parse_vector_models = vp6_parse_vector_models;
    s->parse_coeff_models = vp6_parse_coeff_models;
    s->parse_header = vp6_parse_header;

    return 0;
}

AVCodec vp6_decoder = {
    "vp6",
    CODEC_TYPE_VIDEO,
    CODEC_ID_VP6,
    sizeof(vp56_context_t),
    vp6_decode_init,
    NULL,
    vp56_free,
    vp56_decode_frame,
#ifndef USE_IPU_THROUGH_MODE
    CODEC_CAP_DR1,
#endif
};

/* flash version, not flipped upside-down */
AVCodec vp6f_decoder = {
    "vp6f",
    CODEC_TYPE_VIDEO,
    CODEC_ID_VP6F,
    sizeof(vp56_context_t),
    vp6_decode_init,
    NULL,
    vp56_free,
    vp56_decode_frame,
#ifndef USE_IPU_THROUGH_MODE
    CODEC_CAP_DR1,
#endif
};

/* flash version, not flipped upside-down, with alpha channel */
AVCodec vp6a_decoder = {
    "vp6a",
    CODEC_TYPE_VIDEO,
    CODEC_ID_VP6A,
    sizeof(vp56_context_t),
    vp6_decode_init,
    NULL,
    vp56_free,
    vp56_decode_frame,
#ifndef USE_IPU_THROUGH_MODE
    CODEC_CAP_DR1,
#endif
};
