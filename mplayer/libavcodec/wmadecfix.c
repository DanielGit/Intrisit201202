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
#include "fixedpoint.h"

#undef NDEBUG
#include <assert.h>

#define EXPVLCBITS 8
#define EXPMAX ((19+EXPVLCBITS-1)/EXPVLCBITS)

#define HGAINVLCBITS 9
#define HGAINMAX ((13+HGAINVLCBITS-1)/HGAINVLCBITS)

int ff_mdct_init_fixed(MDCTContextFix *s, int nbits, int inverse);
void ff_imdct_calc_fixed(MDCTContextFix *s, int *outputfix,
                   const int *input, FFTSample *tmp);

//The following variable is set in mplayer.c, which is added by jyu 2009-4-24
extern int jz_cpu_type_id;
//#undef JZ47_OPT
#undef tprint
//#define TRACE
//#define tprintf(p,...) printf(__VA_ARGS__)

#ifdef JZ47_OPT
int tablefix16_10exp1div16[64] = {
/*00*/ 65536,
/*01*/ 75679,
/*02*/ 87393,
/*03*/ 100920,
/*04*/ 116541,
/*05*/ 134579,
/*06*/ 155410,
/*07*/ 179465,
/*08*/ 207243,
/*09*/ 239320,
/*10*/ 276363,
/*11*/ 319139,
/*12*/ 368536,
/*13*/ 425578,
/*14*/ 491450,
/*15*/ 567518,
/*16*/ 655360,
/*17*/ 756797,
/*18*/ 873936,
/*19*/ 1009206,
/*20*/ 1165413,
/*21*/ 1345798,
/*22*/ 1554103,
/*23*/ 1794650,
/*24*/ 2072430,
/*25*/ 2393205,
/*26*/ 2763630,
/*27*/ 3191390,
/*28*/ 3685360,
/*29*/ 4255787,
/*30*/ 4914506,
/*31*/ 5675183,
/*32*/ 6553600,
/*33*/ 7567979,
/*34*/ 8739366,
/*35*/ 10092062,
/*36*/ 11654131,
/*37*/ 13457981,
/*38*/ 15541034,
/*39*/ 17946506,
/*40*/ 20724302,
/*41*/ 23932051,
/*42*/ 27636302,
/*43*/ 31913903,
/*44*/ 36853601,
/*45*/ 42557874,
/*46*/ 49145066,
/*47*/ 56751837,
/*48*/ 65536000,
/*49*/ 75679792,
/*50*/ 87393660,
/*51*/ 100920624,
/*52*/ 116541319,
/*53*/ 134579816,
/*54*/ 155410347,
/*55*/ 179465069,
/*56*/ 207243028,
/*57*/ 239320516,
/*58*/ 276363020,
/*59*/ 319139037,
/*60*/ 368536010,
/*61*/ 425578746,
/*62*/ 491450669,
/*63*/ 567518378,
};
long long pow_table[] =
    {
        0x10000LL,0x11f3dLL,0x14249LL,0x1699cLL,0x195bcLL,0x1c73dLL,0x1fec9LL,0x23d1dLL,0x2830bLL,0x2d182LL,
        0x3298bLL,0x38c53LL,0x3fb28LL,0x47783LL,0x5030aLL,0x59f98LL,0x64f40LL,0x71457LL,0x7f17bLL,0x8e99aLL,
        0xa0000LL,0xb385eLL,0xc96d9LL,0xe2019LL,0xfd954LL,0x11c865LL,0x13f3dfLL,0x166320LL,0x191e6eLL,0x1c2f10LL,
        0x1f9f6eLL,0x237b39LL,0x27cf8bLL,0x2cab1aLL,0x321e65LL,0x383bf0LL,0x3f1882LL,0x46cb6aLL,0x4f6eceLL,0x592006LL,
        0x640000LL,0x7033acLL,0x7de47eLL,0x8d40f6LL,0x9e7d44LL,0xb1d3f4LL,0xc786b7LL,0xdfdf43LL,0xfb304bLL,0x119d69aLL,
        0x13c3a4eLL,0x162d03aLL,0x18e1b70LL,0x1beaf00LL,0x1f52feeLL,0x2325760LL,0x276f514LL,0x2c3f220LL,0x31a5408LL,
        0x37b403cLL,0x3e80000LL,0x46204b8LL,0x4eaece8LL,0x58489a0LL,0x630e4a8LL,0x6f24788LL,0x7cb4328LL,0x8beb8a0LL,
        0x9cfe2f0LL,0xb026200LL,0xc5a4710LL,0xddc2240LL,0xf8d1260LL,0x1172d600LL,0x1393df60LL,0x15f769c0LL,0x18a592c0LL,
        0x1ba77540LL,0x1f074840LL,0x22d08280LL,0x27100000LL,0x2bd42f40LL,0x312d4100LL,0x372d6000LL,0x3de8ee80LL,
        0x4576cb80LL,0x4df09f80LL,0x57733600LL,0x621edd80LL,0x6e17d480LL,0x7b86c700LL,0x8a995700LL,0x9b82b800LL,
        0xae7c5c00LL,0xc3c6b900LL,0xdbaa2200LL,0xf677bc00LL,0x1148a9400LL,0x13648d200LL,0x15c251800LL,0x186a00000LL,
        0x1b649d800LL,0x1ebc48a00LL,0x227c5c000LL,0x26b195000LL,0x2b6a3f000LL,0x30b663c00LL,0x36a801c00LL,0x3d534a400LL,
        0x44cee4800LL,0x4d343c800LL,0x569fd6000LL,0x6131b2800LL,0x6d0db9800LL,0x7a5c33800LL,0x894a55000LL,0x9a0ad6000LL,
        0xacd69d000LL,0xc1ed84000LL,0xd9972f000LL,0xf42400000LL,0x111ee28000LL,0x1335ad6000LL,0x158db98000LL,0x182efd4000LL,
        0x1b22676000LL,0x1e71fe6000LL,0x2229014000LL,0x26540e8000LL,0x2b014f0000LL,0x3040a5c000LL,0x3623e60000LL,0x3cbf0fc000LL,
        0x4428940000LL,0x4c79a08000LL,0x55ce758000LL,0x6046c58000LL,0x6c06220000LL,0x7934728000LL,0x87fe7d0000LL,0x9896800000LL,
        0xab34d90000LL,0xc018c60000LL,0xd7893f0000LL,0xf1d5e40000LL,0x10f580a0000LL,0x13073f00000LL,0x1559a0c0000LL,0x17f48900000LL,
        0x1ae0d160000LL,0x1e286780000LL,0x21d66fc0000LL,0x25f769c0000LL,0x2a995c80000LL,0x2fcc0440000LL,0x35a10940000LL,
        0x3c2c3b80000LL,0x4383d500000LL,0x4bc0c780000LL,0x54ff0e80000LL,0x5f5e1000000LL,0x6b010780000LL,0x780f7c00000LL,
        0x86b5c800000LL,0x9725ae00000LL,0xa9970600000LL,0xbe487500000LL,0xd5804700000LL,0xef8d5a00000LL,0x10cc82e00000LL,
        0x12d940c00000LL,0x152605c00000LL,0x17baa2200000LL,0x1a9fd9c00000LL,0x1ddf82a00000LL,0x2184a5c00000LL,0x259ba5400000LL,
        0x2a3265400000LL,0x2f587cc00000LL,0x351f69000000LL,0x3b9aca000000LL,0x42e0a4800000LL,0x4b09ad800000LL,
        0x54319d000000LL,0x5e778d000000LL,0x69fe64000000LL,0x76ed49800000LL,0x85702c000000LL,0x95b858000000LL,
        0xa7fd1c000000LL,0xbc7c87000000LL,0xd37c3a000000LL,0xed4a55000000LL,0x10a3e82000000LL,0x12abb1a000000LL,
        0x14f2e7a000000LL,0x1781474000000LL,0x1a5f7f4000000LL,0x1d974de000000LL,0x2133a18000000LL
    };

static int ff_wma_lspfix_codebook[NB_LSP_COEFS][16] = {
{0x1fcc2,0x1fabd,0x1f8c7,0x1f66d,0x1f34c,0x1eef1,0x1e83e,0x1dca6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
{0x1f8fc,0x1f5f9,0x1f328,0x1f025,0x1ecd8,0x1e8fc,0x1e46f,0x1df1b,0x1d87c,0x1d047,0x1c6b5,0x1bb8f,0x1add8,0x19c0e,0x18220,0x154ca},
{0x1e6ae,0x1dd65,0x1d58e,0x1cd3b,0x1c439,0x1ba69,0x1af5e,0x1a32c,0x195c4,0x18498,0x16fd2,0x156ea,0x13de4,0x11f63,0xf7ae,0xbd90},
{0x1c4fa,0x1ada0,0x19976,0x1891d,0x17986,0x1697f,0x15858,0x145fd,0x1316b,0x11900,0xfcfa,0xdf55,0xbe63,0x9902,0x6e83,0x2e05},
{0x16f2d,0x15205,0x135f3,0x11b14,0x10170,0xe743,0xcdec,0xb504,0x9ab2,0x7f86,0x6296,0x4565,0x24e2,0x90,0xffffd52f,0xffffa172},
{0xffbc,0xd786,0xb521,0x943e,0x7876,0x5ea3,0x44ad,0x2bf0,0x1274,0xfffff829,0xfffe9981,0xffffbfab,0xffffa0bb,0xffff7d3f,0xffff59e3,0xffff3269},
{0x43e1,0x102a,0xffffe94a,0xffffc9fa,0xffffb076,0xffff9a6b,0xffff871c,0xffff7555,0xffff62b4,0xffff4f81,0xffff3bf4,0xffff25f7,0xffff0c0f,0xfffeef53,0xfffecb7e,0xfffe9fb3},
{0xffff75ea,0xffff4325,0xffff1da2,0xfffefd23,0xfffeddb9,0xfffebb51,0xfffe945f,0xfffe6131,0xfffee5fe,0xfffed5ba,0xfffec442,0xfffeb224,0xfffe9f95,0xfffe880e,0xfffe6c7a,0xfffe54c1},
{0xffff9d2e,0xffff709e,0xffff5489,0xffff3d5e,0xffff295b,0xffff1761,0xffff06a2,0xfffef68a,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
{0xfffe7045,0xfffe572f,0xfffe45ea,0xfffe38af,0xfffe2d8f,0xfffe2347,0xfffe18df,0xfffe0d42,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
};

int sqrtint (int frac)
{
  int i, value, sval, eval;
  long long a;
  if (frac <= 0)
    return 0;

  a = (long long)frac << 16;

  sval = 1;
  eval = (1 << 31) - 2;
 
  while (eval - sval > 1)
  {
    value = (eval + sval) / 2;
    if (((long long)value * value) > a)
      eval = value;
    else
      sval = value;
  }

  if ((a - (long long)sval * sval) < ((long long)eval * eval - a))
    return sval;  
  else
    return eval;
}

#endif

static void wma_lsp_to_curve_init(WMACodecContext *s, int frame_len);

#ifdef TRACE
static void dump_shorts(WMACodecContext *s, const char *name, const uint8_t *tab, int n)
{
    int i;

    tprintf(s->avctx, "%s[%d]:\n", name, n);
    for(i=0;i<n;i++) {
        if ((i & 7) == 0)
            tprintf(s->avctx, "%4d: ", i);
        tprintf(s->avctx, " %02X %02X", tab[2*i],tab[2*i+1]);
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

void (*ff_imdct_half_fixed)(MDCTContextFix *s, int *outputfix,
                   const int *input);
void ff_imdct_half_fixed_4740(MDCTContextFix *s, int *outputfix,
                   const int *input);
void ff_imdct_half_fixed_4750(MDCTContextFix *s, int *outputfix,
                   const int *input);
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

    if(jz_cpu_type_id>=CPU_ID_JZ4750){
        ff_imdct_half_fixed=ff_imdct_half_fixed_4750;
    }else{
        ff_imdct_half_fixed=ff_imdct_half_fixed_4740;
    }
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
#if 0
static inline float pow_m1_4(WMACodecContext *s, float x)
{
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
}
#else

#include "pow_m1_4_2k.h"

static inline int pow_m1_4_fix16(WMACodecContext *s, int fixa) // fixa ^ -0.25
{
  unsigned int min, max, mid, mid2, result;
  long long fixa_res, mid2_res, fix1=0x4000000000000000ll;
  float f;
  
  if (fixa <= 0)
    return 0;
  
  if (fixa < 65536)
  {
    fixa_res = (long long)fixa << (62 - 16);
    min = 1;
    max = 65536;
    while (max > min)
    {
      mid = (min + max) / 2;
      mid2 = (mid * mid) >> 1;
      mid2_res = (long long) mid2 * mid2;
      if (mid2_res >= fixa_res)
        max = mid;
      else
  	min = mid;
  			
      if (max - min == 1)
    	 break;
    }
  }
  else
  {
    fixa_res = (long long)fixa << 16;
    min = 1;
    max = (1 << 20);
  
    while (max > min)
    {
      mid = (min + max) / 2;
      mid2_res = (long long)mid * mid;
      if (mid2_res >= fixa_res)
        max = mid;
      else 
      {
    	mid2 = (unsigned int)(mid2_res >> 16);
    	if ((long long)mid2 * mid2 >= fixa_res)
    	  max = mid;
    	else
          min = mid;
      }
      if (max - min == 1)
    	break;
    }
  }
    
  fix1 = fix1 / (min);
  result = (int)(fix1 >> 30);
  if (fixa < 2048)
    return (int)result + pow_m1_4diff[fixa];
  else
    return (int)result + 1;    // to improve precision
}
#endif

static void wma_lsp_to_curve_init(WMACodecContext *s, int frame_len)
{
    float wdel, a, b;
    int i, e, m;
    wdel = M_PI / frame_len;
    for(i=0;i<frame_len;i++)
        s->lspfix_cos_table[i] = FIXED16(2.0f * cos(wdel * i));

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
}

/**
 * NOTE: We use the same code as Vorbis here
 * @todo optimize it further with SSE/3Dnow
 */
static void wma_lsp_to_curve(WMACodecContext *s,
                             int *out, int *val_max_ptr,
                             int n, int *lsp)
{
    int i, j;
    int p, q, w, vfix;
    int v, val_max;
    
    val_max = 0;
    for(i=0;i<n;i++) {
        p = 0x8000;
        q = 0x8000;
        w = s->lspfix_cos_table[i];
        for(j=1;j<NB_LSP_COEFS;j+=2){
            q = FIX_MULT16(q, (w - lsp[j - 1]));
            p = FIX_MULT16(p, (w - lsp[j]));
        }
        p = FIX_MULT16(p, FIX_MULT16(p, (0x20000 - w)));
        q = FIX_MULT16(q, FIX_MULT16(q, (0x20000 + w)));
        vfix = p + q;
        v = pow_m1_4_fix16(s, vfix);
        if (v > val_max)
            val_max = v;
        out[i] = v;
    }
    *val_max_ptr = val_max;
}

/**
 * decode exponents coded with LSP coefficients (same idea as Vorbis)
 */
static void decode_exp_lsp(WMACodecContext *s, int ch)
{
    int lsp_coefs[NB_LSP_COEFS];
    int val, i;

    for(i = 0; i < NB_LSP_COEFS; i++) {
        if (i == 0 || i >= 8)
            val = get_bits(&s->gb, 3);
        else
            val = get_bits(&s->gb, 4);
        lsp_coefs[i] = ff_wma_lspfix_codebook[i][val];
    }

    wma_lsp_to_curve(s, s->exponents_fix[ch], &s->max_exponent_fix[ch],
                     s->block_len, lsp_coefs);
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
        if (last_exp < 0){
            v = 0;
        }else if (last_exp < 64){
          v = tablefix16_10exp1div16[last_exp];
        }else {
          v = tablefix16_10exp1div16[63];
          //v = (int)(pow(10, last_exp * (1.0 / 16.0)) * (1 << FRAC_BIT));
          printf (" ++++ found error last_exp(1) = %d +++\n", last_exp);
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
        if(last_exp < 0){
          v = 0;
        }else if (last_exp < 64){
          v = tablefix16_10exp1div16[last_exp];
        }else {
          v = tablefix16_10exp1div16[63];
          //v = (int)(pow(10, last_exp * (1.0 / 16.0)) * (1 << FRAC_BIT));
          printf (" ++++ found error last_exp(2) = %d +++\n", last_exp);
        }
        if (v > max_scale)
            max_scale = v;
        n = *ptr++;
        do {
            *q++ = v;
        } while (--n);
    }
    s->max_exponent_fix[ch] = (max_scale > 0) ? max_scale : 1;
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
          outfix[i] = FIX_MULT16(infix[i], s->windowsfix[bsize][i])  + outfix[i];
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
          outfix[i+n] = FIX_MULT16(infix[i+n], s->windowsfix[bsize][i])  + outfix[i+n];
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
          outfix[i] = FIX_MULT16(infix[i], s->windowsfix[bsize][block_len - 1 - i]);
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
          outfix[n + i] = FIX_MULT16(infix[i + n], s->windowsfix[bsize][block_len - 1 - i]);

        memset(outfix+n+block_len, 0, n*sizeof(int));
    }
}


/* butter fly op */
#define MUL16(a,b) ((a) * (b))
void Calc_cmul_4750(FFTComplexFix *z,int are, int aim, int bre, int bim)
{
   S32MUL(xr1,xr2,(are), (bre));                       
   S32MSUB(xr1,xr2,(aim),(bim));                       
   S32MUL(xr3,xr4,(are),(bim));                        
   S32MADD(xr3,xr4,(bre),(aim));                       
   D32SLL(xr1,xr1,xr3,xr3,1);
   z->refix = S32M2I(xr1);                                  
   z->imfix = S32M2I(xr3);                                  
}
void Calc_cmul_4740(FFTComplexFix *z,int are, int aim, int bre, int bim)
{
  long hi;
  unsigned long lo;
  asm __volatile("mult %1,%3\n"              
      "msub %2,%4\n"                         
      "mfhi %0\n"                            
      :"=d"(hi)                     
      :"r"(are),"r"(aim), "r"(bre), "r"(bim) 
      :"hi","lo");                           
  z->refix=(hi<<1);                   
  asm __volatile("mult %1,%4\n"              
      "madd %2,%3\n"                         
      "mfhi %0\n"                            
      :"=d"(hi)                     
      :"r"(are),"r"(aim), "r"(bre), "r"(bim) 
      :"hi","lo");                           
  z->imfix=(hi<<1);                   

}


/**
 * The size of the FFT is 2^nbits. If inverse is TRUE, inverse FFT is
 * done
 */
/* cos(2*pi*x/n) for 0<=x<=n/4, followed by its reverse */
int32_t ff_cos_16[8];
int32_t ff_cos_32[16];
int32_t ff_cos_64[32];
int32_t ff_cos_128[64];
int32_t ff_cos_256[128];
int32_t ff_cos_512[256];
int32_t ff_cos_1024[512];
int32_t *ff_cos_tabs[] = {
    ff_cos_16, ff_cos_32, ff_cos_64, ff_cos_128, ff_cos_256, ff_cos_512, ff_cos_1024
};

static int split_radix_permutation(int i, int n, int inverse)
{
    int m;
    if(n <= 2) return i&1;
    m = n >> 1;
    if(!(i&m))            return split_radix_permutation(i, m, inverse)*2;
    m >>= 1;
    if(inverse == !(i&m)) return split_radix_permutation(i, m, inverse)*4 + 1;
    else                  return split_radix_permutation(i, m, inverse)*4 - 1;
}

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

    s->exptab1 = NULL;

    for(j=4; j<=nbits; j++) {
        int m = 1<<j;
        double freq = 2*M_PI/m;
        int32_t *tab = ff_cos_tabs[j-4];
        for(i=0; i<=m/4; i++)
            tab[i] = FIXED31(cos(i*freq));
        for(i=1; i<m/4; i++)
            tab[m/2-i] = tab[i];
    }
    for(i=0; i<n; i++)
        s->revtab[-split_radix_permutation(i, n, s->inverse) & (n-1)] = i;

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


/**
 * Compute inverse MDCT of size N = 2^nbits
 * @param output N samples
 * @param input N/2 samples
 * @param tmp N/2 samples
 */

/**
 * Do a complex FFT with the parameters defined in ff_fft_init(). The
 * input data must be permuted before with s->revtab table. No
 * 1.0/sqrt(n) normalization is done.
 */

#define M_SQRT1_2_FIX      1518500249/* (1/sqrt(2))*((1<<31)-1) */
#define BF(x,y,a,b) {\
    x = a - b;\
    y = a + b;\
}

#define BUTTERFLIES(a0,a1,a2,a3) {\
    BF(t3, t5, t5, t1);\
    BF(a2.refix, a0.refix, a0.refix, t5);\
    BF(a3.imfix, a1.imfix, a1.imfix, t3);\
    BF(t4, t6, t2, t6);\
    BF(a3.refix, a1.refix, a1.refix, t4);\
    BF(a2.imfix, a0.imfix, a0.imfix, t6);\
}

// force loading all the inputs before storing any.
// this is slightly slower for small data, but avoids store->load aliasing
// for addresses separated by large powers of 2.
#define BUTTERFLIES_BIG(a0,a1,a2,a3) {\
    int32_t r0=a0.refix, i0=a0.imfix, r1=a1.refix, i1=a1.imfix;\
    BF(t3, t5, t5, t1);\
    BF(a2.refix, a0.refix, r0, t5);\
    BF(a3.imfix, a1.imfix, i1, t3);\
    BF(t4, t6, t2, t6);\
    BF(a3.refix, a1.refix, r1, t4);\
    BF(a2.imfix, a0.imfix, i0, t6);\
}

#if 0
#define TRANSFORM(a0,a1,a2,a3,wre,wim) {\
    t1 = FIX_MULT31(a2.refix , wre) + FIX_MULT31(a2.imfix , wim);\
    t2 = FIX_MULT31(a2.imfix , wre) - FIX_MULT31(a2.refix , wim);\
    t5 = FIX_MULT31(a3.refix , wre) - FIX_MULT31(a3.imfix , wim);\
    t6 = FIX_MULT31(a3.imfix , wre) + FIX_MULT31(a3.refix , wim);\
    BUTTERFLIES(a0,a1,a2,a3)\
}
#else
#define TRANSFORM(a0,a1,a2,a3,wre,wim) {\
  long hi;                                            \
  asm __volatile("mult %1,%3\n"                       \
      "madd %2,%4\n"                                  \
      "mfhi %0\n"                                     \
      :"=d"(hi)                                       \
      :"r"(a2.refix),"r"(a2.imfix), "r"(wre), "r"(wim)\
      :"hi","lo");                                    \
  t1=(hi<<1);                                         \
  asm __volatile("mult %2,%3\n"                       \
      "msub %1,%4\n"                                  \
      "mfhi %0\n"                                     \
      :"=d"(hi)                                       \
      :"r"(a2.refix),"r"(a2.imfix), "r"(wre), "r"(wim)\
      :"hi","lo");                                    \
  t2=(hi<<1);                                         \
  asm __volatile("mult %1,%3\n"                       \
      "msub %2,%4\n"                                  \
      "mfhi %0\n"                                     \
      :"=d"(hi)                                       \
      :"r"(a3.refix),"r"(a3.imfix), "r"(wre), "r"(wim)\
      :"hi","lo");                                    \
  t5=(hi<<1);                                         \
  asm __volatile("mult %2,%3\n"                       \
      "madd %1,%4\n"                                  \
      "mfhi %0\n"                                     \
      :"=d"(hi)                                       \
      :"r"(a3.refix),"r"(a3.imfix), "r"(wre), "r"(wim)\
      :"hi","lo");                                    \ 
  t6=(hi<<1);                                         \
    BUTTERFLIES(a0,a1,a2,a3)\
}
#endif
#define TRANSFORM_ZERO(a0,a1,a2,a3) {\
    t1 = a2.refix;\
    t2 = a2.imfix;\
    t5 = a3.refix;\
    t6 = a3.imfix;\
    BUTTERFLIES(a0,a1,a2,a3)\
}

/* z[0...8n-1], w[1...2n-1] */
#define PASS(name)\
static void name(FFTComplexFix *z, const int32_t *wre, unsigned int n)\
{\
    int32_t t1, t2, t3, t4, t5, t6;\
    int o1 = 2*n;\
    int o2 = 4*n;\
    int o3 = 6*n;\
    const int32_t *wim = wre+o1;\
    n--;\
\
    TRANSFORM_ZERO(z[0],z[o1],z[o2],z[o3]);\
    TRANSFORM(z[1],z[o1+1],z[o2+1],z[o3+1],wre[1],wim[-1]);\
    do {\
        z += 2;\
        wre += 2;\
        wim -= 2;\
        TRANSFORM(z[0],z[o1],z[o2],z[o3],wre[0],wim[0]);\
        TRANSFORM(z[1],z[o1+1],z[o2+1],z[o3+1],wre[1],wim[-1]);\
    } while(--n);\
}

PASS(pass_4740)
#undef BUTTERFLIES
#define BUTTERFLIES BUTTERFLIES_BIG
PASS(pass_big_4740)

#define DECL_FFT(n,n2,n4)\
static void fft##n##_4740(FFTComplexFix *z)\
{\
    fft##n2##_4740(z);\
    fft##n4##_4740(z+n4*2);\
    fft##n4##_4740(z+n4*3);\
    pass_4740(z,ff_cos_##n,n4/2);\
}


 
static void fft4_4740(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6, t7, t8;

    BF(t3, t1, z[0].refix, z[1].refix);
    BF(t8, t6, z[3].refix, z[2].refix);
    BF(z[2].refix, z[0].refix, t1, t6);
    BF(t4, t2, z[0].imfix, z[1].imfix);
    BF(t7, t5, z[2].imfix, z[3].imfix);
    BF(z[3].imfix, z[1].imfix, t4, t8);
    BF(z[3].refix, z[1].refix, t3, t7);
    BF(z[2].imfix, z[0].imfix, t2, t5);
}


static void fft8_4740(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6, t7, t8;

    fft4_4740(z);

    BF(t1, z[5].refix, z[4].refix, -z[5].refix);
    BF(t2, z[5].imfix, z[4].imfix, -z[5].imfix);
    BF(t3, z[7].refix, z[6].refix, -z[7].refix);
    BF(t4, z[7].imfix, z[6].imfix, -z[7].imfix);
    BF(t8, t1, t3, t1);
    BF(t7, t2, t2, t4);
    BF(z[4].refix, z[0].refix, z[0].refix, t1);
    BF(z[4].imfix, z[0].imfix, z[0].imfix, t2);
    BF(z[6].refix, z[2].refix, z[2].refix, t7);
    BF(z[6].imfix, z[2].imfix, z[2].imfix, t8);

    TRANSFORM(z[1],z[3],z[5],z[7],M_SQRT1_2_FIX,M_SQRT1_2_FIX);
}
static void fft16_4740(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6;

    fft8_4740(z);
    fft4_4740(z+8);
    fft4_4740(z+12);

    TRANSFORM_ZERO(z[0],z[4],z[8],z[12]);
    TRANSFORM(z[2],z[6],z[10],z[14],M_SQRT1_2_FIX,M_SQRT1_2_FIX);
    TRANSFORM(z[1],z[5],z[9],z[13],ff_cos_16[1],ff_cos_16[3]);
    TRANSFORM(z[3],z[7],z[11],z[15],ff_cos_16[3],ff_cos_16[1]);
}
DECL_FFT(32,16,8)
DECL_FFT(64,32,16)
DECL_FFT(128,64,32)
DECL_FFT(256,128,64)
DECL_FFT(512,256,128)
#define pass_4740 pass_big_4740
DECL_FFT(1024,512,256)

static void (*fft_dispatch_4740[])(FFTComplexFix*) = {
    fft4_4740, fft8_4740, fft16_4740, fft32_4740, fft64_4740, fft128_4740, fft256_4740, fft512_4740, fft1024_4740
};

void ff_fft_calc_fixed_4740(FFTContextFix *s, FFTComplexFix *z)
{
    fft_dispatch_4740[s->nbits-2](z);
}
void ff_imdct_half_fixed_4740(MDCTContextFix *s, int *outputfix,
                   const int *input)
{
    int k, n8, n4, n2, n, j;
    const uint16_t *revtab = s->fft.revtab;
    const int *tcosfix;
    const int *tsinfix;
    const int *in1, *in2;
    FFTComplexFix *z = (FFTComplexFix *)outputfix;

    n = 1 << s->nbits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;

    /* pre rotation */
    tcosfix = s->tcosfix;
    tsinfix = s->tsinfix;
    in1 = input;
    in2 = input + n2 - 1;
    for(k = 0; k < n4; k++) {
        j=revtab[k];
        Calc_cmul_4740(z+j, *in2, *in1, tcosfix[k], tsinfix[k]);
        in1 += 2;
        in2 -= 2;
    }
    ff_fft_calc_fixed_4740(&s->fft, z);

    /* post rotation + reordering */
    /* XXX: optimize */
    for(k = 0; k < n8; k++) {
        FFTComplexFix tmpz[2];
        Calc_cmul_4740(tmpz, z[n8-k-1].imfix, z[n8-k-1].refix, tsinfix[n8-k-1], tcosfix[n8-k-1]);
        Calc_cmul_4740(tmpz+1, z[n8+k  ].imfix, z[n8+k  ].refix, tsinfix[n8+k  ], tcosfix[n8+k  ]);
        z[n8-k-1].refix = tmpz[0].refix;
        z[n8-k-1].imfix = tmpz[1].imfix;
        z[n8+k  ].refix = tmpz[1].refix;
        z[n8+k  ].imfix = tmpz[0].imfix;
    }

}
#undef TRANSFORM
#if 0
#define TRANSFORM(a0,a1,a2,a3,wre,wim) {\
    t1 = FIX_MULT31(a2.refix , wre) + FIX_MULT31(a2.imfix , wim);\
    t2 = FIX_MULT31(a2.imfix , wre) - FIX_MULT31(a2.refix , wim);\
    t5 = FIX_MULT31(a3.refix , wre) - FIX_MULT31(a3.imfix , wim);\
    t6 = FIX_MULT31(a3.imfix , wre) + FIX_MULT31(a3.refix , wim);\
    BUTTERFLIES(a0,a1,a2,a3)\
}
#else
#define TRANSFORM(a0,a1,a2,a3,wre,wim) {\
   S32MUL(xr1,xr2,(a2.refix), (wre));   \                    
   S32MADD(xr1,xr2,(a2.imfix),(wim));   \                   
   S32MUL(xr3,xr4,(a2.imfix),(wre));    \                   
   S32MSUB(xr3,xr4,(a2.refix),(wim));    \                  
   D32SLL(xr1,xr1,xr3,xr3,1);           \
   t1=S32M2I(xr1);                      \
   t2=S32M2I(xr3);                      \
   S32MUL(xr1,xr2,(a3.refix), (wre));   \                 
   S32MSUB(xr1,xr2,(a3.imfix),(wim));   \                   
   S32MUL(xr3,xr4,(a3.imfix),(wre));    \                   
   S32MADD(xr3,xr4,(a3.refix),(wim));   \                   
   D32SLL(xr1,xr1,xr3,xr3,1);           \
   t5 = S32M2I(xr1);                    \             
   t6 = S32M2I(xr3);                    \             
    BUTTERFLIES(a0,a1,a2,a3)\
}
#endif


/* z[0...8n-1], w[1...2n-1] */
#define PASS(name)\
static void name(FFTComplexFix *z, const int32_t *wre, unsigned int n)\
{\
    int32_t t1, t2, t3, t4, t5, t6;\
    int o1 = 2*n;\
    int o2 = 4*n;\
    int o3 = 6*n;\
    const int32_t *wim = wre+o1;\
    n--;\
\
    TRANSFORM_ZERO(z[0],z[o1],z[o2],z[o3]);\
    TRANSFORM(z[1],z[o1+1],z[o2+1],z[o3+1],wre[1],wim[-1]);\
    do {\
        z += 2;\
        wre += 2;\
        wim -= 2;\
        TRANSFORM(z[0],z[o1],z[o2],z[o3],wre[0],wim[0]);\
        TRANSFORM(z[1],z[o1+1],z[o2+1],z[o3+1],wre[1],wim[-1]);\
    } while(--n);\
}

PASS(pass_4750)
#undef BUTTERFLIES
#define BUTTERFLIES BUTTERFLIES_BIG
PASS(pass_big_4750)

#undef DECL_FFT
#define DECL_FFT(n,n2,n4)\
static void fft##n##_4750(FFTComplexFix *z)\
{\
    fft##n2##_4750(z);\
    fft##n4##_4750(z+n4*2);\
    fft##n4##_4750(z+n4*3);\
    pass_4750(z,ff_cos_##n,n4/2);\
}


static void fft4_4750(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6, t7, t8;

    BF(t3, t1, z[0].refix, z[1].refix);
    BF(t8, t6, z[3].refix, z[2].refix);
    BF(z[2].refix, z[0].refix, t1, t6);
    BF(t4, t2, z[0].imfix, z[1].imfix);
    BF(t7, t5, z[2].imfix, z[3].imfix);
    BF(z[3].imfix, z[1].imfix, t4, t8);
    BF(z[3].refix, z[1].refix, t3, t7);
    BF(z[2].imfix, z[0].imfix, t2, t5);
}


static void fft8_4750(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6, t7, t8;

    fft4_4750(z);

    BF(t1, z[5].refix, z[4].refix, -z[5].refix);
    BF(t2, z[5].imfix, z[4].imfix, -z[5].imfix);
    BF(t3, z[7].refix, z[6].refix, -z[7].refix);
    BF(t4, z[7].imfix, z[6].imfix, -z[7].imfix);
    BF(t8, t1, t3, t1);
    BF(t7, t2, t2, t4);
    BF(z[4].refix, z[0].refix, z[0].refix, t1);
    BF(z[4].imfix, z[0].imfix, z[0].imfix, t2);
    BF(z[6].refix, z[2].refix, z[2].refix, t7);
    BF(z[6].imfix, z[2].imfix, z[2].imfix, t8);

    TRANSFORM(z[1],z[3],z[5],z[7],M_SQRT1_2_FIX,M_SQRT1_2_FIX);
}
static void fft16_4750(FFTComplexFix *z)
{
    int32_t t1, t2, t3, t4, t5, t6;

    fft8_4750(z);
    fft4_4750(z+8);
    fft4_4750(z+12);

    TRANSFORM_ZERO(z[0],z[4],z[8],z[12]);
    TRANSFORM(z[2],z[6],z[10],z[14],M_SQRT1_2_FIX,M_SQRT1_2_FIX);
    TRANSFORM(z[1],z[5],z[9],z[13],ff_cos_16[1],ff_cos_16[3]);
    TRANSFORM(z[3],z[7],z[11],z[15],ff_cos_16[3],ff_cos_16[1]);
}
DECL_FFT(32,16,8)
DECL_FFT(64,32,16)
DECL_FFT(128,64,32)
DECL_FFT(256,128,64)
DECL_FFT(512,256,128)
#define pass_4750 pass_big_4750
DECL_FFT(1024,512,256)

static void (*fft_dispatch_4750[])(FFTComplexFix*) = {
    fft4_4750, fft8_4750, fft16_4750, fft32_4750, fft64_4750, fft128_4750, fft256_4750, fft512_4750, fft1024_4750
};


void ff_fft_calc_fixed_4750(FFTContextFix *s, FFTComplexFix *z)
{
    fft_dispatch_4750[s->nbits-2](z);
}
void ff_imdct_half_fixed_4750(MDCTContextFix *s, int *outputfix,
                   const int *input)
{
    int k, n8, n4, n2, n, j;
    const uint16_t *revtab = s->fft.revtab;
    const int *tcosfix;
    const int *tsinfix;
    const int *in1, *in2;
    FFTComplexFix *z = (FFTComplexFix *)outputfix;

    n = 1 << s->nbits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;

    /* pre rotation */
    tcosfix = s->tcosfix;
    tsinfix = s->tsinfix;
    in1 = input;
    in2 = input + n2 - 1;
    for(k = 0; k < n4; k++) {
        j=revtab[k];
        Calc_cmul_4750(z+j, *in2, *in1, tcosfix[k], tsinfix[k]);
        in1 += 2;
        in2 -= 2;
    }
    ff_fft_calc_fixed_4750(&s->fft, z);

    /* post rotation + reordering */
    /* XXX: optimize */
    for(k = 0; k < n8; k++) {
        FFTComplexFix tmpz[2];
        Calc_cmul_4750(tmpz, z[n8-k-1].imfix, z[n8-k-1].refix, tsinfix[n8-k-1], tcosfix[n8-k-1]);
        Calc_cmul_4750(tmpz+1, z[n8+k  ].imfix, z[n8+k  ].refix, tsinfix[n8+k  ], tcosfix[n8+k  ]);
        z[n8-k-1].refix = tmpz[0].refix;
        z[n8-k-1].imfix = tmpz[1].imfix;
        z[n8+k  ].refix = tmpz[1].refix;
        z[n8+k  ].imfix = tmpz[0].imfix;
    }

}


void ff_imdct_calc_fixed(MDCTContextFix *s, int *output,
                   const int *input, FFTSample *tmp)
{
    int k;
    int n = 1 << s->nbits;
    int n2 = n >> 1;
    int n4 = n >> 2;

    ff_imdct_half_fixed(s, output+n4, input);

    for(k = 0; k < n4; k++) {
        output[k] = -output[n2-k-1];
        output[n-k-1] = output[n2+k];
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
        s->tcosfix[i] = FIXED31(val);
        val = -sin(alpha);
        s->tsinfix[i] = FIXED31(val);
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
            if (s->max_exponent_fix[ch] <= 0)
              s->max_exponent_fix[ch] = 1;
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
    mdct_norm_fix = FIXED16(mdct_norm);
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
            if (s->use_exp_vlc) {
              mult_fix = FIX16_DIV(pow_table[total_gain], s->max_exponent_fix[ch]); //fixed16
              mult_fix = FIX_MULT16(mult_fix, mdct_norm_fix); //fixed16
            }
            else
            {
            	long long tmp_fix;
            	tmp_fix = (pow_table[total_gain] << 16) / (long long)s->max_exponent_fix[ch];
            	mult_fix = (int)((tmp_fix * mdct_norm_fix) >> 16);
            }
            coefs_fix = s->coefs_fix[ch];
            if (s->use_noise_coding) {
                mult1_fix = mult_fix;
                /* very low freqs : noise */
                for(i = 0;i < s->coefs_start; i++) {
                    int tmp;
                    //*coefs_fix++ = FIX_MULT3(s->noise_table_fix[s->noise_index],
                    //  exponents_fix[i<<bsize>>esize], (mult1_fix >> 8));
                    tmp = FIX_MULT16 (exponents_fix[i<<bsize>>esize], mult1_fix);
                    tmp = FIX_MULT18 (tmp, s->noise_table_fix[s->noise_index]);  //fixed14
                    *coefs_fix++ = (tmp >> 0); 
                    s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }
                n1 = s->exponent_high_sizes[bsize];
                /* compute power of high bands */
                exponents_fix = s->exponents_fix[ch] +
                                (s->high_band_start[bsize]<<bsize);
                last_high_band = 0; /* avoid warning */
                exp_power_fix[0] = 1;         //fixed16
                for(j=0;j<n1;j++) {
                    n = s->exponent_high_bands[s->frame_len_bits -
                                              s->block_len_bits][j];
                    if (s->high_band_coded[ch][j]) {
                        int vfix; 
                        long long e2fix;
                        e2fix = 0;
                        for(i = 0;i < n; i++) {
                          vfix = exponents_fix[i<<bsize>>esize];
                          e2fix += FIX_MULT16(vfix, vfix);            //fixed16
                        }
                        //exp_power_fix[j] = e2fix / n;
                        vfix = FIX16_DIVINT (e2fix, n);    //fixed16
                        exp_power_fix[j] = vfix;         //fixed16
                        if (vfix != 0)
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
                        if(exp_power_fix[last_high_band]==0){
                            mult1_fix=1<<31;
                        }else{
                            mult1_fix = sqrtint (FIX16_DIV(exp_power_fix[j],
                                             exp_power_fix[last_high_band]));
                        }
                        if (mult1_fix == 0)
                          mult1_fix = ((1 << 31) - 8);
                        /* XXX: use a table */
                        mult1_fix = FIX_MULT18(pow_table[s->high_band_values[ch][j]], mult1_fix);  //fixed14
                        mult1_fix = FIX16_DIV(mult1_fix, 
                                      FIX_MULT16(s->max_exponent_fix[ch], s->noise_mult_fix));  //fixed14
                        mult1_fix = FIX_MULT16 (mult1_fix, mdct_norm_fix);   // fixed14
                        for(i = 0;i < n; i++) {
                            int tmp;
                            noise_fix = s->noise_table_fix[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                            tmp = FIX_MULT16 (mult1_fix, exponents_fix[i<<bsize>>esize]);  //fixed14
                            tmp = FIX_MULT16 (tmp, noise_fix);  // fixed14
                            *coefs_fix++ = (tmp >> 0);
                            //*coefs_fix++ = FIX_MULT3( noise_fix,
                            //      exponents_fix[i<<bsize>>esize],  (mult1_fix >> 6));
                        }
                        exponents_fix += n<<bsize;
                    } else {
                        /* coded values + small noise */
                        for(i = 0;i < n; i++) {
                            int tmp;
                            noise_fix = s->noise_table_fix[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                            //*coefs_fix++ = FIX_MULT3 ((((*coefs1++) << FRAC_BIT) + noise_fix),
                            //                 exponents_fix[i<<bsize>>esize],  (mult_fix >> 8));
                            tmp = FIX_MULT16 (exponents_fix[i<<bsize>>esize], mult_fix);   //fixed16
                            tmp = FIX_MULT18 (tmp, (((*coefs1++) << 16) + noise_fix));  //fixed14
                            *coefs_fix++ = (tmp >> 0);
                        }
                        exponents_fix += n<<bsize;
                    }
                }

                /* very high freqs : noise */
                n = s->block_len - s->coefs_end[bsize];
                //mult1_fix = FIX_MULT16 ((mult_fix >> 8),  exponents_fix[((-1<<bsize))>>esize]);
                mult1_fix = FIX_MULT16 (mult_fix,  exponents_fix[((-1<<bsize))>>esize]);    //fixed16
                for(i = 0; i < n; i++) {
                  //*coefs_fix++ = FIX_MULT (s->noise_table_fix[s->noise_index], mult1_fix);
                  *coefs_fix++ = (FIX_MULT18 (s->noise_table_fix[s->noise_index], mult1_fix) >> 0);  //fixed14
                  s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }
            } else {
                /* XXX: optimize more */
                for(i = 0;i < s->coefs_start; i++)
                  *coefs_fix++ = 0; 
                n = nb_coefs[ch];
                for(i = 0;i < n; i++) {
                  int tmp;
                  //  *coefs_fix++ = FIX_MULT3 ((coefs1[i] << FRAC_BIT), 
                  //                   exponents_fix[i<<bsize>>esize], (mult_fix >> 8));
                  tmp = FIX_MULT16 (exponents_fix[i<<bsize>>esize], mult_fix);   //fixed16
                  tmp = FIX_MULT18 (tmp, (coefs1[i] << 16));    //fixed14
                  *coefs_fix++ = (tmp >> 0);
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
                wma_window(s, &s->frame_outfix[1][index]);
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
            *ptr = av_clip_int16((*iptrfix++) >> 14);
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
