/*
 * WMA compatible decoder
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
 * @file wmadec.c
 * WMA compatible decoder.
 * This decoder handles Microsoft Windows Media Audio data, versions 1 & 2.
 * WMA v1 is identified by audio format 0x160 in Microsoft media files
 * (ASF/AVI/WAV). WMA v2 is identified by audio format 0x161.
 *
 * To use this decoder, a calling application must supply the extra data
 * bytes provided with the WMA data. These are the extra, codec-specific
 * bytes at the end of a WAVEFORMATEX data structure. Transmit these bytes
 * to the decoder using the extradata[_size] fields in AVCodecContext. There
 * should be 4 extra bytes for v1 data and 6 extra bytes for v2 data.
 */

#include "avcodec.h"
#include "wmafixed.h"
#include "fixed.h"

#undef NDEBUG
#include <assert.h>

#define EXPVLCBITS 8
#define EXPMAX ((19+EXPVLCBITS-1)/EXPVLCBITS)

#define HGAINVLCBITS 9
#define HGAINMAX ((13+HGAINVLCBITS-1)/HGAINVLCBITS)

int ff_mdct_init_fixed(MDCTContextFix *s, int nbits, int inverse);
void ff_imdct_calc_fixed(MDCTContextFix *s, int *outputfix,
                   const int *input, FFTSample *tmp);

#undef printf
//#undef JZ47_OPT
#undef tprint
//#define TRACE
//#define tprintf(p,...) printf(__VA_ARGS__)

#ifdef JZ47_OPT
int tablefix_10exp1div16[64] = {
/*00*/ 256,
/*01*/ 295,
/*02*/ 341,
/*03*/ 394,
/*04*/ 455,
/*05*/ 525,
/*06*/ 607,
/*07*/ 701,
/*08*/ 809,
/*09*/ 934,
/*10*/ 1079,
/*11*/ 1246,
/*12*/ 1439,
/*13*/ 1662,
/*14*/ 1919,
/*15*/ 2216,
/*16*/ 2560,
/*17*/ 2956,
/*18*/ 3413,
/*19*/ 3942,
/*20*/ 4552,
/*21*/ 5257,
/*22*/ 6070,
/*23*/ 7010,
/*24*/ 8095,
/*25*/ 9348,
/*26*/ 10795,
/*27*/ 12466,
/*28*/ 14395,
/*29*/ 16624,
/*30*/ 19197,
/*31*/ 22168,
/*32*/ 25600,
/*33*/ 29562,
/*34*/ 34138,
/*35*/ 39422,
/*36*/ 45523,
/*37*/ 52570,
/*38*/ 60707,
/*39*/ 70103,
/*40*/ 80954,
/*41*/ 93484,
/*42*/ 107954,
/*43*/ 124663,
/*44*/ 143959,
/*45*/ 166241,
/*46*/ 191972,
/*47*/ 221686,
/*48*/ 256000,
/*49*/ 295624,
/*50*/ 341381,
/*51*/ 394221,
/*52*/ 455239,
/*53*/ 525702,
/*54*/ 607071,
/*55*/ 701035,
/*56*/ 809543,
/*57*/ 934845,
/*58*/ 1079543,
/*59*/ 1246636,
/*60*/ 1439593,
/*61*/ 1662416,
/*62*/ 1919729,
/*63*/ 2216868,
};

int table_fixpow10[5][16] = {
/*0, 00, 1.000000, 1.000000*/ 0x00000100,
/*0, 01, 1.000035, 1.000000*/ 0x00000100,
/*0, 02, 1.000070, 1.000000*/ 0x00000100,
/*0, 03, 1.000105, 1.000000*/ 0x00000100,
/*0, 04, 1.000141, 1.000000*/ 0x00000100,
/*0, 05, 1.000176, 1.000000*/ 0x00000100,
/*0, 06, 1.000211, 1.000000*/ 0x00000100,
/*0, 07, 1.000246, 1.000000*/ 0x00000100,
/*0, 08, 1.000281, 1.000000*/ 0x00000100,
/*0, 09, 1.000316, 1.000000*/ 0x00000100,
/*0, 10, 1.000351, 1.000000*/ 0x00000100,
/*0, 11, 1.000387, 1.000000*/ 0x00000100,
/*0, 12, 1.000422, 1.000000*/ 0x00000100,
/*0, 13, 1.000457, 1.000000*/ 0x00000100,
/*0, 14, 1.000492, 1.000000*/ 0x00000100,
/*0, 15, 1.000527, 1.000000*/ 0x00000100,
/*1, 00, 1.000000, 1.000000*/ 0x00000100,
/*1, 01, 1.000562, 1.000000*/ 0x00000100,
/*1, 02, 1.001125, 1.000000*/ 0x00000100,
/*1, 03, 1.001688, 1.000000*/ 0x00000100,
/*1, 04, 1.002251, 1.000000*/ 0x00000100,
/*1, 05, 1.002815, 1.000000*/ 0x00000100,
/*1, 06, 1.003379, 1.000000*/ 0x00000100,
/*1, 07, 1.003943, 1.003906*/ 0x00000101,
/*1, 08, 1.004507, 1.003906*/ 0x00000101,
/*1, 09, 1.005072, 1.003906*/ 0x00000101,
/*1, 10, 1.005637, 1.003906*/ 0x00000101,
/*1, 11, 1.006203, 1.003906*/ 0x00000101,
/*1, 12, 1.006769, 1.003906*/ 0x00000101,
/*1, 13, 1.007335, 1.003906*/ 0x00000101,
/*1, 14, 1.007901, 1.007812*/ 0x00000102,
/*1, 15, 1.008468, 1.007812*/ 0x00000102,
/*2, 00, 1.000000, 1.000000*/ 0x00000100,
/*2, 01, 1.009035, 1.007812*/ 0x00000102,
/*2, 02, 1.018152, 1.015625*/ 0x00000104,
/*2, 03, 1.027351, 1.027344*/ 0x00000107,
/*2, 04, 1.036633, 1.035156*/ 0x00000109,
/*2, 05, 1.045999, 1.042969*/ 0x0000010b,
/*2, 06, 1.055450, 1.054688*/ 0x0000010e,
/*2, 07, 1.064986, 1.062500*/ 0x00000110,
/*2, 08, 1.074608, 1.074219*/ 0x00000113,
/*2, 09, 1.084317, 1.082031*/ 0x00000115,
/*2, 10, 1.094114, 1.093750*/ 0x00000118,
/*2, 11, 1.103999, 1.101562*/ 0x0000011a,
/*2, 12, 1.113974, 1.113281*/ 0x0000011d,
/*2, 13, 1.124039, 1.121094*/ 0x0000011f,
/*2, 14, 1.134194, 1.132812*/ 0x00000122,
/*2, 15, 1.144442, 1.140625*/ 0x00000124,
/*3, 00, 1.000000, 1.000000*/ 0x00000100,
/*3, 01, 1.154782, 1.152344*/ 0x00000127,
/*3, 02, 1.333521, 1.332031*/ 0x00000155,
/*3, 03, 1.539927, 1.539062*/ 0x0000018a,
/*3, 04, 1.778279, 1.777344*/ 0x000001c7,
/*3, 05, 2.053525, 2.050781*/ 0x0000020d,
/*3, 06, 2.371374, 2.371094*/ 0x0000025f,
/*3, 07, 2.738420, 2.738281*/ 0x000002bd,
/*3, 08, 3.162278, 3.160156*/ 0x00000329,
/*3, 09, 3.651741, 3.648438*/ 0x000003a6,
/*3, 10, 4.216965, 4.214844*/ 0x00000437,
/*3, 11, 4.869675, 4.867188*/ 0x000004de,
/*3, 12, 5.623413, 5.621094*/ 0x0000059f,
/*3, 13, 6.493816, 6.492188*/ 0x0000067e,
/*3, 14, 7.498942, 7.496094*/ 0x0000077f,
/*3, 15, 8.659643, 8.656250*/ 0x000008a8,
/*4, 00, 1.000000, 1.000000*/ 0x00000100,
/*4, 01, 10.000000, 10.000000*/ 0x00000a00,
/*4, 02, 100.000000, 100.000000*/ 0x00006400,
/*4, 03, 1000.000000, 1000.000000*/ 0x0003e800,
/*4, 04, 10000.000000, 10000.000000*/ 0x00271000,
/*4, 05, 100000.000000, 100000.000000*/ 0x0186a000,
/*4, 06, 1000000.000000, 1000000.000000*/ 0x0f424000,
};

int fixedpow10 (int exp)
{
  int powexp = exp;
  int value = (1 << FRAC_BIT);
  int idx;

  idx = 0;

  while (powexp)
  {
    value = FIX_MULT(value, table_fixpow10[idx][powexp & 0xf]);
    powexp >>= 4;
    idx++;
  }

  return value;
}

int sqrtint (int a)
{
  int aexp, i, value, sval, eval;

  if (a <= 0)
    return 0;

  a = a << FRAC_BIT;

  // get 2 exp
  for (i = 0; i < 31; i++)
    if ((a >> (i + 1)) == 0) 
      break;

  aexp = i / 2;
  sval = (1 << aexp);
  eval = (a >> aexp);

  
  while (eval - sval > 1)
  {
    value = (eval + sval) / 2;
    if ((value * value) < value || (value * value) > a)
      eval = value;
    else
      sval = value;
  }

  if ((a - sval * sval) < (eval * eval - a))
    return sval;  
  else
    return eval;
}

#endif

static void wma_lsp_to_curve_init(WMACodecContext *s, int frame_len);

#ifdef TRACE
static void dump_shorts(WMACodecContext *s, const char *name, const short *tab, int n)
{
    int i;

    tprintf(s->avctx, "%s[%d]:\n", name, n);
    for(i=0;i<n;i++) {
        if ((i & 7) == 0)
            tprintf(s->avctx, "%4d: ", i);
        tprintf(s->avctx, " %5d.0", tab[i]);
        if ((i & 7) == 7)
            tprintf(s->avctx, "\n");
    }
}

static void dump_floats(WMACodecContext *s, const char *name, int prec, const float *tab, int n)
{
    int i;

    tprintf(s->avctx, "%s[%d]:\n", name, n);
    for(i=0;i<n;i++) {
        if ((i & 7) == 0)
            tprintf(s->avctx, "%4d: ", i);
        tprintf(s->avctx, " %8.*f", prec, tab[i]);
        if ((i & 7) == 7)
            tprintf(s->avctx, "\n");
    }
    if ((i & 7) != 0)
        tprintf(s->avctx, "\n");
}
#endif

static int wma_decode_init(AVCodecContext * avctx)
{
    WMACodecContext *s = avctx->priv_data;
    int i, flags1, flags2;
    uint8_t *extradata;

    s->avctx = avctx;

    /* extract flag infos */
    flags1 = 0;
    flags2 = 0;
    extradata = avctx->extradata;
    if (avctx->codec->id == CODEC_ID_WMAV1 && avctx->extradata_size >= 4) {
        flags1 = AV_RL16(extradata);
        flags2 = AV_RL16(extradata+2);
    } else if (avctx->codec->id == CODEC_ID_WMAV2 && avctx->extradata_size >= 6) {
        flags1 = AV_RL32(extradata);
        flags2 = AV_RL16(extradata+4);
    }
// for(i=0; i<avctx->extradata_size; i++)
//     av_log(NULL, AV_LOG_ERROR, "%02X ", extradata[i]);

    s->use_exp_vlc = flags2 & 0x0001;
    s->use_bit_reservoir = flags2 & 0x0002;
    s->use_variable_block_len = flags2 & 0x0004;

    if(ff_wma_init(avctx, flags2)<0)
        return -1;

    /* init MDCT */
    for(i = 0; i < s->nb_block_sizes; i++)
        ff_mdct_init_fixed(&s->mdct_ctx[i], s->frame_len_bits - i + 1, 1);

    if (s->use_noise_coding) {
        init_vlc(&s->hgain_vlc, HGAINVLCBITS, sizeof(ff_wma_hgain_huffbits),
                 ff_wma_hgain_huffbits, 1, 1,
                 ff_wma_hgain_huffcodes, 2, 2, 0);
    }

    if (s->use_exp_vlc) {
        init_vlc(&s->exp_vlc, EXPVLCBITS, sizeof(ff_wma_scale_huffbits), //FIXME move out of context
                 ff_wma_scale_huffbits, 1, 1,
                 ff_wma_scale_huffcodes, 4, 4, 0);
    } else {
        wma_lsp_to_curve_init(s, s->frame_len);
    }

    return 0;
}

/**
 * compute x^-0.25 with an exponent and mantissa table. We use linear
 * interpolation to reduce the mantissa table size at a small speed
 * expense (linear interpolation approximately doubles the number of
 * bits of precision).
 */
static inline float pow_m1_4(WMACodecContext *s, float x)
{
#if 0   // Daniel
    union {
        float f;
        unsigned int v;
    } u, t;
    unsigned int e, m;
    float a, b;

    u.f = x;
    e = u.v >> 23;
    m = (u.v >> (23 - LSP_POW_BITS)) & ((1 << LSP_POW_BITS) - 1);
    /* build interpolation scale: 1 <= t < 2. */
    t.v = ((u.v << LSP_POW_BITS) & ((1 << 23) - 1)) | (127 << 23);
    a = s->lsp_pow_m_table1[m];
    b = s->lsp_pow_m_table2[m];
    return s->lsp_pow_e_table[e] * (a + b * t.f);
#endif
}

static void wma_lsp_to_curve_init(WMACodecContext *s, int frame_len)
{
    float wdel, a, b;
    int i, e, m;

#if 0 // Daniel
    wdel = M_PI / frame_len;
    for(i=0;i<frame_len;i++)
        s->lsp_cos_table[i] = 2.0f * cos(wdel * i);

    /* tables for x^-0.25 computation */
    for(i=0;i<256;i++) {
        e = i - 126;
        s->lsp_pow_e_table[i] = pow(2.0, e * -0.25);
    }

    /* NOTE: these two tables are needed to avoid two operations in
       pow_m1_4 */
    b = 1.0;
    for(i=(1 << LSP_POW_BITS) - 1;i>=0;i--) {
        m = (1 << LSP_POW_BITS) + i;
        a = (float)m * (0.5 / (1 << LSP_POW_BITS));
        a = pow(a, -0.25);
        s->lsp_pow_m_table1[i] = 2 * a - b;
        s->lsp_pow_m_table2[i] = b - a;
        b = a;
    }
#if 0
    for(i=1;i<20;i++) {
        float v, r1, r2;
        v = 5.0 / i;
        r1 = pow_m1_4(s, v);
        r2 = pow(v,-0.25);
        printf("%f^-0.25=%f e=%f\n", v, r1, r2 - r1);
    }
#endif
#endif
}

/**
 * NOTE: We use the same code as Vorbis here
 * @todo optimize it further with SSE/3Dnow
 */
static void wma_lsp_to_curve(WMACodecContext *s,
                             float *out, float *val_max_ptr,
                             int n, float *lsp)
{
    int i, j;
    float p, q, w, v, val_max;

#if 0 // Daniel
    val_max = 0;
    for(i=0;i<n;i++) {
        p = 0.5f;
        q = 0.5f;
        w = s->lsp_cos_table[i];
        for(j=1;j<NB_LSP_COEFS;j+=2){
            q *= w - lsp[j - 1];
            p *= w - lsp[j];
        }
        p *= p * (2.0f - w);
        q *= q * (2.0f + w);
        v = p + q;
        v = pow_m1_4(s, v);
        if (v > val_max)
            val_max = v;
        out[i] = v;
    }
    *val_max_ptr = val_max;
#endif
}

/**
 * decode exponents coded with LSP coefficients (same idea as Vorbis)
 */
static void decode_exp_lsp(WMACodecContext *s, int ch)
{
    float lsp_coefs[NB_LSP_COEFS];
    int val, i;

    for(i = 0; i < NB_LSP_COEFS; i++) {
        if (i == 0 || i >= 8)
            val = get_bits(&s->gb, 3);
        else
            val = get_bits(&s->gb, 4);
        lsp_coefs[i] = ff_wma_lsp_codebook[i][val];
    }

#if 0 // daniel  temporary compiling error
    wma_lsp_to_curve(s, s->exponents[ch], &s->max_exponent[ch],
                     s->block_len, lsp_coefs);
#endif
}

/**
 * decode exponents coded with VLC codes
 */
static int decode_exp_vlc(WMACodecContext *s, int ch)
{
    int last_exp, n, code;
    const uint16_t *ptr, *band_ptr;
    int v, *q, max_scale, *q_end;

    band_ptr = s->exponent_bands[s->frame_len_bits - s->block_len_bits];
    ptr = band_ptr;
    q = s->exponents_fix[ch];
    q_end = q + s->block_len;
    max_scale = 0;
    if (s->version == 1) {
        last_exp = get_bits(&s->gb, 5) + 10;
        /* XXX: use a table */
        if (last_exp < 64)
          v = tablefix_10exp1div16[last_exp];
        else
        {
          v = (int)(pow(10, last_exp * (1.0 / 16.0)) * (1 << FRAC_BIT));
          printf (" ++++ found error last_exp = %d +++\n", last_exp);
        }
        max_scale = v;
        n = *ptr++;
        do {
            *q++ = v;
        } while (--n);
    }else
        last_exp = 36;

    while (q < q_end) {
        code = get_vlc2(&s->gb, s->exp_vlc.table, EXPVLCBITS, EXPMAX);
        if (code < 0)
            return -1;
        /* NOTE: this offset is the same as MPEG4 AAC ! */
        last_exp += code - 60;
        /* XXX: use a table */
        if (last_exp < 64)
          v = tablefix_10exp1div16[last_exp];
        else
        {
          v = (int)(pow(10, last_exp * (1.0 / 16.0)) * (1 << FRAC_BIT));
          printf (" ++++ found error last_exp = %d +++\n", last_exp);
        }
        if (v > max_scale)
            max_scale = v;
        n = *ptr++;
        do {
            *q++ = v;
        } while (--n);
    }
    s->max_exponent_fix[ch] = max_scale;
    return 0;
}

/**
 * Apply MDCT window and add into output.
 *
 * We ensure that when the windows overlap their squared sum
 * is always 1 (MDCT reconstruction rule).
 */
static void wma_window(WMACodecContext *s, int *outfix)
{
    int   *infix = s->outputfix;
    int block_len, bsize, n, i;

    /* left part */
    if (s->block_len_bits <= s->prev_block_len_bits) {
        block_len = s->block_len;
        bsize = s->frame_len_bits - s->block_len_bits;

#ifdef JZ47_OPT
        for (i = 0; i < block_len; i++)
          outfix[i] = FIX_MULT(infix[i], s->windowsfix[bsize][i])  + outfix[i];
#else
        s->dsp.vector_fmul_add_add(out, in, s->windows[bsize],
                                   out, 0, block_len, 1);
#endif
    } else {
        block_len = 1 << s->prev_block_len_bits;
        n = (s->block_len - block_len) / 2;
        bsize = s->frame_len_bits - s->prev_block_len_bits;

#ifdef JZ47_OPT
        for (i = 0; i < block_len; i++)
          outfix[i+n] = FIX_MULT(infix[i+n], s->windowsfix[bsize][i])  + outfix[i+n];
#else
        s->dsp.vector_fmul_add_add(out+n, in+n, s->windows[bsize],
                                   out+n, 0, block_len, 1);
#endif

#ifdef JZ47_OPT
        memcpy(outfix+n+block_len, infix+n+block_len, n*sizeof(int));
#else
        memcpy(out+n+block_len, in+n+block_len, n*sizeof(float));
#endif
    }

    outfix += s->block_len;
    infix += s->block_len;

    /* right part */
    if (s->block_len_bits <= s->next_block_len_bits) {
        block_len = s->block_len;
        bsize = s->frame_len_bits - s->block_len_bits;

#ifdef JZ47_OPT
        for (i = 0; i < block_len; i++) 
        {  
          outfix[i] = FIX_MULT(infix[i], s->windowsfix[bsize][block_len - 1 - i]);
        }
#else
        s->dsp.vector_fmul_reverse(out, in, s->windows[bsize], block_len);
#endif

    } else {
        block_len = 1 << s->next_block_len_bits;
        n = (s->block_len - block_len) / 2;
        bsize = s->frame_len_bits - s->next_block_len_bits;

        memcpy(outfix, infix, n*sizeof(int));

        for (i = 0; i < block_len; i++) 
          outfix[n + i] = FIX_MULT(infix[i + n], s->windowsfix[bsize][block_len - 1 - i]);

        memset(outfix+n+block_len, 0, n*sizeof(int));
    }
}


/* butter fly op */
#define BF(pre, pim, qre, qim, pre1, pim1, qre1, qim1) \
{\
  int ax, ay, bx, by;\
  bx=pre1;\
  by=pim1;\
  ax=qre1;\
  ay=qim1;\
  pre = (bx + ax);\
  pim = (by + ay);\
  qre = (bx - ax);\
  qim = (by - ay);\
}

#define MUL16(a,b) ((a) * (b))

#define CMUL(pre, pim, are, aim, bre, bim) \
{\
   pre = (FIX_MULT(are, bre) - FIX_MULT(aim, bim));\
   pim = (FIX_MULT(are, bim) + FIX_MULT(bre, aim));\
}

/**
 * Do a complex FFT with the parameters defined in ff_fft_init(). The
 * input data must be permuted before with s->revtab table. No
 * 1.0/sqrt(n) normalization is done.
 */
void ff_fft_calc_fixed(FFTContextFix *s, FFTComplexFix *z)
{
    int ln = s->nbits;
    int j, np, np2;
    int nblocks, nloops;
    register FFTComplexFix *p, *q;
    FFTComplexFix *exptab = s->exptab;
    int l;
    int tmp_refix, tmp_imfix;

    np = 1 << ln;

    /* pass 0 */

    p=&z[0];
    j=(np >> 1);
    do {
        BF(p[0].refix, p[0].imfix, p[1].refix, p[1].imfix,
           p[0].refix, p[0].imfix, p[1].refix, p[1].imfix);
        p+=2;
    } while (--j != 0);

    /* pass 1 */


    p=&z[0];
    j=np >> 2;
    if (s->inverse) {
        do {
            BF(p[0].refix, p[0].imfix, p[2].refix, p[2].imfix,
               p[0].refix, p[0].imfix, p[2].refix, p[2].imfix);
            BF(p[1].refix, p[1].imfix, p[3].refix, p[3].imfix,
               p[1].refix, p[1].imfix, -p[3].imfix, p[3].refix);
            p+=4;
        } while (--j != 0);
    } else {
        do {
            BF(p[0].refix, p[0].imfix, p[2].refix, p[2].imfix,
               p[0].refix, p[0].imfix, p[2].refix, p[2].imfix);
            BF(p[1].refix, p[1].imfix, p[3].refix, p[3].imfix,
               p[1].refix, p[1].imfix, p[3].imfix, -p[3].refix);
            p+=4;
        } while (--j != 0);
    }
    /* pass 2 .. ln-1 */

    nblocks = np >> 3;
    nloops = 1 << 2;
    np2 = np >> 1;
    do {
        p = z;
        q = z + nloops;
        for (j = 0; j < nblocks; ++j) {
            BF(p->refix, p->imfix, q->refix, q->imfix,
               p->refix, p->imfix, q->refix, q->imfix);

            p++;
            q++;
            for(l = nblocks; l < np2; l += nblocks) {
                CMUL(tmp_refix, tmp_imfix, exptab[l].refix, exptab[l].imfix, q->refix, q->imfix);
                BF(p->refix, p->imfix, q->refix, q->imfix,
                   p->refix, p->imfix, tmp_refix, tmp_imfix);
                p++;
                q++;
            }

            p += nloops;
            q += nloops;
        }
        nblocks = nblocks >> 1;
        nloops = nloops << 1;
    } while (nblocks != 0);
}

/**
 * The size of the FFT is 2^nbits. If inverse is TRUE, inverse FFT is
 * done
 */
int ff_fft_init_fixed(FFTContextFix *s, int nbits, int inverse)
{
    int i, j, m, n;
    float alpha, c1, s1, s2;

    s->nbits = nbits;
    n = 1 << nbits;

    s->exptab = av_malloc((n / 2) * sizeof(FFTComplexFix));
    if (!s->exptab)
        goto fail;
    s->revtab = av_malloc(n * sizeof(uint16_t));
    if (!s->revtab)
        goto fail;
    s->inverse = inverse;

    s2 = inverse ? 1.0 : -1.0;

    for(i=0;i<(n/2);i++) {
        alpha = 2 * M_PI * (float)i / (float)n;
        c1 = cos(alpha);
        s1 = sin(alpha) * s2;
        s->exptab[i].refix = FIXED(c1);
        s->exptab[i].imfix = FIXED(s1);
    }
    s->exptab1 = NULL;

    /* compute bit reverse table */

    for(i=0;i<n;i++) {
        m=0;
        for(j=0;j<nbits;j++) {
            m |= ((i >> j) & 1) << (nbits-j-1);
        }
        s->revtab[i]=m;
    }
    return 0;
 fail:
    av_freep(&s->revtab);
    av_freep(&s->exptab);
    av_freep(&s->exptab1);
    return -1;
}


void ff_fft_end_fixed(FFTContextFix *s)
{
    av_freep(&s->revtab);
    av_freep(&s->exptab);
    av_freep(&s->exptab1);
}


#undef CMUL

/* complex multiplication: p = a * b */
#define CMUL(pre, pim, are, aim, bre, bim) \
{\
    int _are = (are);\
    int _aim = (aim);\
    int _bre = (bre);\
    int _bim = (bim);\
    (pre) = FIX_MULT(_are , _bre) - FIX_MULT(_aim , _bim);\
    (pim) = FIX_MULT(_are , _bim) + FIX_MULT(_aim , _bre);\
}

/**
 * Compute inverse MDCT of size N = 2^nbits
 * @param output N samples
 * @param input N/2 samples
 * @param tmp N/2 samples
 */
void ff_imdct_calc_fixed(MDCTContextFix *s, int *outputfix,
                   const int *input, FFTSample *tmp)
{
    int k, n8, n4, n2, n, j;
    const uint16_t *revtab = s->fft.revtab;
    const int *tcosfix = s->tcosfix;
    const int *tsinfix = s->tsinfix;
    const int *in1, *in2;
    FFTComplexFix *z = (FFTComplexFix *)tmp;

    n = 1 << s->nbits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;

    /* pre rotation */
    in1 = input;
    in2 = input + n2 - 1;
    for(k = 0; k < n4; k++) {
        j=revtab[k];
        CMUL(z[j].refix, z[j].imfix, *in2, *in1, tcosfix[k], tsinfix[k]);
        in1 += 2;
        in2 -= 2;
    }

    ff_fft_calc_fixed(&s->fft, z);

    /* post rotation + reordering */
    /* XXX: optimize */
    for(k = 0; k < n4; k++) {
        CMUL(z[k].refix, z[k].imfix, z[k].refix, z[k].imfix, tcosfix[k], tsinfix[k]);
    }

    for(k = 0; k < n8; k++) {
        outputfix[2*k] = -z[n8 + k].imfix;
        outputfix[n2-1-2*k] = z[n8 + k].imfix;

        outputfix[2*k+1] = z[n8-1-k].refix;
        outputfix[n2-1-2*k-1] = -z[n8-1-k].refix;

        outputfix[n2 + 2*k]=-z[k+n8].refix;
        outputfix[n-1- 2*k]=-z[k+n8].refix;

        outputfix[n2 + 2*k+1]=z[n8-k-1].imfix;
        outputfix[n-2 - 2 * k] = z[n8-k-1].imfix;
    }
}

/**
 * init MDCT or IMDCT computation.
 */
int ff_mdct_init_fixed(MDCTContextFix *s, int nbits, int inverse)
{
    int n, n4, i;
    float alpha, val;

    memset(s, 0, sizeof(*s));
    n = 1 << nbits;
    s->nbits = nbits;
    s->n = n;
    n4 = n >> 2;
    s->tcosfix = av_malloc(n4 * sizeof(FFTSample));
    if (!s->tcosfix)
        goto fail;
    s->tsinfix = av_malloc(n4 * sizeof(FFTSample));
    if (!s->tsinfix)
        goto fail;

    for(i=0;i<n4;i++) {
        alpha = 2 * M_PI * (i + 1.0 / 8.0) / n;
        val = -cos(alpha);
        s->tcosfix[i] = FIXED(val);
        val = -sin(alpha);
        s->tsinfix[i] = FIXED(val);
    }
    if (ff_fft_init_fixed(&s->fft, s->nbits - 2, inverse) < 0)
        goto fail;
    return 0;
 fail:
    av_freep(&s->tcosfix);
    av_freep(&s->tsinfix);
    return -1;
}

void ff_mdct_end_fixed(MDCTContextFix *s)
{
    av_freep(&s->tcosfix);
    av_freep(&s->tsinfix);
    ff_fft_end_fixed(&s->fft);
}

/**
 * @return 0 if OK. 1 if last block of frame. return -1 if
 * unrecorrable error.
 */
static int wma_decode_block(WMACodecContext *s)
{
    int n, v, a, ch, code, bsize;
    int coef_nb_bits, total_gain;
    int nb_coefs[MAX_CHANNELS];
    float mdct_norm;
    int   mdct_norm_fix;

#ifdef TRACE
    tprintf(s->avctx, "***decode_block: %d:%d\n", s->frame_count - 1, s->block_num);
#endif
    /* compute current block length */
    if (s->use_variable_block_len) {
        n = av_log2(s->nb_block_sizes - 1) + 1;

        if (s->reset_block_lengths) {
            s->reset_block_lengths = 0;
            v = get_bits(&s->gb, n);
            if (v >= s->nb_block_sizes)
                return -1;
            s->prev_block_len_bits = s->frame_len_bits - v;
            v = get_bits(&s->gb, n);
            if (v >= s->nb_block_sizes)
                return -1;
            s->block_len_bits = s->frame_len_bits - v;
        } else {
            /* update block lengths */
            s->prev_block_len_bits = s->block_len_bits;
            s->block_len_bits = s->next_block_len_bits;
        }
        v = get_bits(&s->gb, n);
        if (v >= s->nb_block_sizes)
            return -1;
        s->next_block_len_bits = s->frame_len_bits - v;
    } else {
        /* fixed block len */
        s->next_block_len_bits = s->frame_len_bits;
        s->prev_block_len_bits = s->frame_len_bits;
        s->block_len_bits = s->frame_len_bits;
    }

    /* now check if the block length is coherent with the frame length */
    s->block_len = 1 << s->block_len_bits;
    if ((s->block_pos + s->block_len) > s->frame_len)
        return -1;

    if (s->nb_channels == 2) {
        s->ms_stereo = get_bits1(&s->gb);
    }
    v = 0;
    for(ch = 0; ch < s->nb_channels; ch++) {
        a = get_bits1(&s->gb);
        s->channel_coded[ch] = a;
        v |= a;
    }
    /* if no channel coded, no need to go further */
    /* XXX: fix potential framing problems */
    if (!v)
        goto next;

    bsize = s->frame_len_bits - s->block_len_bits;

    /* read total gain and extract corresponding number of bits for
       coef escape coding */
    total_gain = 1;
    for(;;) {
        a = get_bits(&s->gb, 7);
        total_gain += a;
        if (a != 127)
            break;
    }

    coef_nb_bits= ff_wma_total_gain_to_bits(total_gain);

    /* compute number of coefficients */
    n = s->coefs_end[bsize] - s->coefs_start;
    for(ch = 0; ch < s->nb_channels; ch++)
        nb_coefs[ch] = n;

    /* complex coding */
    if (s->use_noise_coding) {

        for(ch = 0; ch < s->nb_channels; ch++) {
            if (s->channel_coded[ch]) {
                int i, n, a;
                n = s->exponent_high_sizes[bsize];
                for(i=0;i<n;i++) {
                    a = get_bits1(&s->gb);
                    s->high_band_coded[ch][i] = a;
                    /* if noise coding, the coefficients are not transmitted */
                    if (a)
                        nb_coefs[ch] -= s->exponent_high_bands[bsize][i];
                }
            }
        }
        for(ch = 0; ch < s->nb_channels; ch++) {
            if (s->channel_coded[ch]) {
                int i, n, val, code;

                n = s->exponent_high_sizes[bsize];
                val = (int)0x80000000;
                for(i=0;i<n;i++) {
                    if (s->high_band_coded[ch][i]) {
                        if (val == (int)0x80000000) {
                            val = get_bits(&s->gb, 7) - 19;
                        } else {
                            code = get_vlc2(&s->gb, s->hgain_vlc.table, HGAINVLCBITS, HGAINMAX);
                            if (code < 0)
                                return -1;
                            val += code - 18;
                        }
                        s->high_band_values[ch][i] = val;
                    }
                }
            }
        }
    }

    /* exponents can be reused in short blocks. */
    if ((s->block_len_bits == s->frame_len_bits) ||
        get_bits1(&s->gb)) {
        for(ch = 0; ch < s->nb_channels; ch++) {
            if (s->channel_coded[ch]) {
                if (s->use_exp_vlc) {
                    if (decode_exp_vlc(s, ch) < 0)
                        return -1;
                } else {
                    decode_exp_lsp(s, ch);
                }
                s->exponents_bsize[ch] = bsize;
            }
        }
    }

    /* parse spectral coefficients : just RLE encoding */
    for(ch = 0; ch < s->nb_channels; ch++) {
        if (s->channel_coded[ch]) {
            VLC *coef_vlc;
            int level, run, sign, tindex;
            int16_t *ptr, *eptr;
            const uint16_t *level_table, *run_table;

            /* special VLC tables are used for ms stereo because
               there is potentially less energy there */
            tindex = (ch == 1 && s->ms_stereo);
            coef_vlc = &s->coef_vlc[tindex];
            run_table = s->run_table[tindex];
            level_table = s->level_table[tindex];
            /* XXX: optimize */
            ptr = &s->coefs1[ch][0];
            eptr = ptr + nb_coefs[ch];
            memset(ptr, 0, s->block_len * sizeof(int16_t));
            for(;;) {
                code = get_vlc2(&s->gb, coef_vlc->table, VLCBITS, VLCMAX);
                if (code < 0)
                    return -1;
                if (code == 1) {
                    /* EOB */
                    break;
                } else if (code == 0) {
                    /* escape */
                    level = get_bits(&s->gb, coef_nb_bits);
                    /* NOTE: this is rather suboptimal. reading
                       block_len_bits would be better */
                    run = get_bits(&s->gb, s->frame_len_bits);
                } else {
                    /* normal code */
                    run = run_table[code];
                    level = level_table[code];
                }
                sign = get_bits1(&s->gb);
                if (!sign)
                    level = -level;
                ptr += run;
                if (ptr >= eptr)
                {
                    av_log(NULL, AV_LOG_ERROR, "overflow in spectral RLE, ignoring\n");
                    break;
                }
                *ptr++ = level;
                /* NOTE: EOB can be omitted */
                if (ptr >= eptr)
                    break;
            }
        }
        if (s->version == 1 && s->nb_channels >= 2) {
            align_get_bits(&s->gb);
        }
    }

    /* normalize */
    {
        int n4 = s->block_len / 2;
        mdct_norm = 1.0 / (float)n4;
        if (s->version == 1) {
            mdct_norm *= sqrt(n4);
        }
    }
    mdct_norm_fix = (int)(mdct_norm * (1<<(FRAC_BIT*2)));

    /* finally compute the MDCT coefficients */
    for(ch = 0; ch < s->nb_channels; ch++) {
        if (s->channel_coded[ch]) {
            int16_t *coefs1;
            int *coefs_fix, *exponents_fix, mult_fix, mult1_fix, noise_fix;
            int i, j, n, n1, last_high_band, esize;
            int exp_power_fix[HIGH_BAND_MAX_SIZE];

            coefs1 = s->coefs1[ch];
            exponents_fix = s->exponents_fix[ch];
            esize = s->exponents_bsize[ch];
            mult_fix=FIX_DIV(fixedpow10(total_gain * 3277 /*0.05*/), s->max_exponent_fix[ch]);
            mult_fix=FIXF2_MULT(mult_fix, mdct_norm_fix);
            coefs_fix = s->coefs_fix[ch];
            if (s->use_noise_coding) {
                mult1_fix = mult_fix;
                /* very low freqs : noise */
                for(i = 0;i < s->coefs_start; i++) {
                    *coefs_fix++ = FIX_MULT3(s->noise_table_fix[s->noise_index],
                      exponents_fix[i<<bsize>>esize], mult1_fix);
                    s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }

                n1 = s->exponent_high_sizes[bsize];

                /* compute power of high bands */
                exponents_fix = s->exponents_fix[ch] +
                                (s->high_band_start[bsize]<<bsize);
                last_high_band = 0; /* avoid warning */
                for(j=0;j<n1;j++) {
                    n = s->exponent_high_bands[s->frame_len_bits -
                                              s->block_len_bits][j];
                    if (s->high_band_coded[ch][j]) {
                        int e2fix, vfix; 
                        e2fix = 0;
                        for(i = 0;i < n; i++) {
                          vfix = exponents_fix[i<<bsize>>esize]; 
                          e2fix += FIX_MULT(vfix, vfix);
                        }
                        exp_power_fix[j] = e2fix / n;
                        last_high_band = j;
                    }
                    exponents_fix += n<<bsize;
                }

                /* main freqs and high freqs */
                exponents_fix = s->exponents_fix[ch] + (s->coefs_start<<bsize);
                for(j=-1;j<n1;j++) {
                    if (j < 0) {
                        n = s->high_band_start[bsize] -
                            s->coefs_start;
                    } else {
                        n = s->exponent_high_bands[s->frame_len_bits -
                                                  s->block_len_bits][j];
                    }
                    if (j >= 0 && s->high_band_coded[ch][j]) {
                        /* use noise with specified power */
                        mult1_fix = sqrtint (FIX_DIV(exp_power_fix[j],
                                              exp_power_fix[last_high_band]));
                        /* XXX: use a table */
                        mult1_fix = FIX_MULT(mult1_fix, 
                                      fixedpow10(s->high_band_values[ch][j] * 3277 /*0.05*/));
                        mult1_fix = FIX_DIV(mult1_fix, 
                                      FIX_MULT(s->max_exponent_fix[ch], s->noise_mult_fix));
                        mult1_fix = FIXF2_MULT (mult1_fix, mdct_norm_fix);
                        for(i = 0;i < n; i++) {
                            noise_fix = s->noise_table_fix[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                            *coefs_fix++ = FIX_MULT3( noise_fix,
                                  exponents_fix[i<<bsize>>esize],  mult1_fix);
                        }
                        exponents_fix += n<<bsize;
                    } else {
                        /* coded values + small noise */
                        for(i = 0;i < n; i++) {
                            noise_fix = s->noise_table_fix[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                            *coefs_fix++ = FIX_MULT3 ((((*coefs1++) << FRAC_BIT) + noise_fix),
                                             exponents_fix[i<<bsize>>esize],  mult_fix);
                        }
                        exponents_fix += n<<bsize;
                    }
                }

                /* very high freqs : noise */
                n = s->block_len - s->coefs_end[bsize];
                mult1_fix = FIX_MULT (mult_fix,  exponents_fix[((-1<<bsize))>>esize]);
                for(i = 0; i < n; i++) {
                  *coefs_fix++ = FIX_MULT (s->noise_table_fix[s->noise_index], mult1_fix);
                  s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }
            } else {
                /* XXX: optimize more */
                for(i = 0;i < s->coefs_start; i++)
                  *coefs_fix++ = 0; 
                n = nb_coefs[ch];
                for(i = 0;i < n; i++) {
                    *coefs_fix++ = FIX_MULT3 ((coefs1[i] << FRAC_BIT), 
                                     exponents_fix[i<<bsize>>esize], mult_fix);
                }
                n = s->block_len - s->coefs_end[bsize];
                for(i = 0;i < n; i++)
                  *coefs_fix++ = 0;
            }
        }
    }

#ifdef TRACE
    for(ch = 0; ch < s->nb_channels; ch++) {
        if (s->channel_coded[ch]) {
            dump_floats(s, "exponents", 3, s->exponents[ch], s->block_len);
            dump_floats(s, "coefs", 1, s->coefs[ch], s->block_len);
        }
    }
#endif

    if (s->ms_stereo && s->channel_coded[1]) {
        int a, b;
        int i;

        /* nominal case for ms stereo: we do it before mdct */
        /* no need to optimize this case because it should almost
           never happen */
        if (!s->channel_coded[0]) {
            tprintf(s->avctx, "rare ms-stereo case happened\n");
            memset(s->coefs_fix[0], 0, sizeof(int) * s->block_len);
            s->channel_coded[0] = 1;
        }

        for(i = 0; i < s->block_len; i++) {
            a = s->coefs_fix[0][i];
            b = s->coefs_fix[1][i];
            s->coefs_fix[0][i] = a + b;
            s->coefs_fix[1][i] = a - b;
        }
    }

    for(ch = 0; ch < s->nb_channels; ch++) {
        if (s->channel_coded[ch]) {
            int n4, index, n;

            n = s->block_len;
            n4 = s->block_len / 2;
#ifdef JZ47_OPT
            ff_imdct_calc_fixed(&s->mdct_ctx[bsize], s->outputfix, 
                                s->coefs_fix[ch], s->mdct_tmp);
#else
            s->mdct_ctx[bsize].fft.imdct_calc(&s->mdct_ctx[bsize],
                          s->output, s->coefs[ch], s->mdct_tmp);
#endif

            /* multiply by the window and add in the frame */
            index = (s->frame_len / 2) + s->block_pos - n4;
            wma_window(s, &s->frame_outfix[ch][index]);

            /* specific fast case for ms-stereo : add to second
               channel if it is not coded */
            if (s->ms_stereo && !s->channel_coded[1]) {
                wma_window(s, &s->frame_outfix[ch][index]);
            }
        }
    }
 next:
    /* update block number */
    s->block_num++;
    s->block_pos += s->block_len;
    if (s->block_pos >= s->frame_len)
        return 1;
    else
        return 0;
}

/* decode a frame of frame_len samples */
static int wma_decode_frame(WMACodecContext *s, int16_t *samples)
{
    int ret, i, n, ch, incr;
    int16_t *ptr;
    int *iptrfix;

#ifdef TRACE
    tprintf(s->avctx, "***decode_frame: %d size=%d\n", s->frame_count++, s->frame_len);
#endif

    /* read each block */
    s->block_num = 0;
    s->block_pos = 0;
    for(;;) {
        ret = wma_decode_block(s);
        if (ret < 0)
            return -1;
        if (ret)
            break;
    }

    /* convert frame to integer */
    n = s->frame_len;
    incr = s->nb_channels;
    for(ch = 0; ch < s->nb_channels; ch++) {
        ptr = samples + ch;
        iptrfix = s->frame_outfix[ch];

        for(i=0;i<n;i++) {
            *ptr = av_clip_int16((*iptrfix++) >> FRAC_BIT);
            ptr += incr;
        }
        /* prepare for next block */
        memmove(&s->frame_outfix[ch][0], &s->frame_outfix[ch][s->frame_len],
                s->frame_len * sizeof(int));
    }

#ifdef TRACE
    dump_shorts(s, "samples", samples, n * s->nb_channels);
#endif
    return 0;
}

static int wma_decode_superframe(AVCodecContext *avctx,
                                 void *data, int *data_size,
                                 uint8_t *buf, int buf_size)
{
    WMACodecContext *s = avctx->priv_data;
    int nb_frames, bit_offset, i, pos, len;
    uint8_t *q;
    int16_t *samples;

    tprintf(avctx, "***decode_superframe:\n");

    if(buf_size==0){
        s->last_superframe_len = 0;
        return 0;
    }

    samples = data;

    init_get_bits(&s->gb, buf, buf_size*8);

    if (s->use_bit_reservoir) {
        /* read super frame header */
        skip_bits(&s->gb, 4); /* super frame index */
        nb_frames = get_bits(&s->gb, 4) - 1;

        bit_offset = get_bits(&s->gb, s->byte_offset_bits + 3);

        if (s->last_superframe_len > 0) {
            //        printf("skip=%d\n", s->last_bitoffset);
            /* add bit_offset bits to last frame */
            if ((s->last_superframe_len + ((bit_offset + 7) >> 3)) >
                MAX_CODED_SUPERFRAME_SIZE)
                goto fail;
            q = s->last_superframe + s->last_superframe_len;
            len = bit_offset;
            while (len > 7) {
                *q++ = (get_bits)(&s->gb, 8);
                len -= 8;
            }
            if (len > 0) {
                *q++ = (get_bits)(&s->gb, len) << (8 - len);
            }

            /* XXX: bit_offset bits into last frame */
            init_get_bits(&s->gb, s->last_superframe, MAX_CODED_SUPERFRAME_SIZE*8);
            /* skip unused bits */
            if (s->last_bitoffset > 0)
                skip_bits(&s->gb, s->last_bitoffset);
            /* this frame is stored in the last superframe and in the
               current one */
            if (wma_decode_frame(s, samples) < 0)
                goto fail;
            samples += s->nb_channels * s->frame_len;
        }

        /* read each frame starting from bit_offset */
        pos = bit_offset + 4 + 4 + s->byte_offset_bits + 3;
        init_get_bits(&s->gb, buf + (pos >> 3), (MAX_CODED_SUPERFRAME_SIZE - (pos >> 3))*8);
        len = pos & 7;
        if (len > 0)
            skip_bits(&s->gb, len);

        s->reset_block_lengths = 1;
        for(i=0;i<nb_frames;i++) {
            if (wma_decode_frame(s, samples) < 0)
                goto fail;
            samples += s->nb_channels * s->frame_len;
        }

        /* we copy the end of the frame in the last frame buffer */
        pos = get_bits_count(&s->gb) + ((bit_offset + 4 + 4 + s->byte_offset_bits + 3) & ~7);
        s->last_bitoffset = pos & 7;
        pos >>= 3;
        len = buf_size - pos;
        if (len > MAX_CODED_SUPERFRAME_SIZE || len < 0) {
            goto fail;
        }
        s->last_superframe_len = len;
        memcpy(s->last_superframe, buf + pos, len);
    } else {
        /* single frame decode */
        if (wma_decode_frame(s, samples) < 0)
            goto fail;
        samples += s->nb_channels * s->frame_len;
    }

//av_log(NULL, AV_LOG_ERROR, "%d %d %d %d outbytes:%d eaten:%d\n", s->frame_len_bits, s->block_len_bits, s->frame_len, s->block_len,        (int8_t *)samples - (int8_t *)data, s->block_align);

    *data_size = (int8_t *)samples - (int8_t *)data;
    return s->block_align;
 fail:
    /* when error, we reset the bit reservoir */
    s->last_superframe_len = 0;
    return -1;
}

AVCodec wmav1_decoder =
{
    "wmav1",
    CODEC_TYPE_AUDIO,
    CODEC_ID_WMAV1,
    sizeof(WMACodecContext),
    wma_decode_init,
    NULL,
    ff_wma_end,
    wma_decode_superframe,
};

AVCodec wmav2_decoder =
{
    "wmav2",
    CODEC_TYPE_AUDIO,
    CODEC_ID_WMAV2,
    sizeof(WMACodecContext),
    wma_decode_init,
    NULL,
    ff_wma_end,
    wma_decode_superframe,
};
