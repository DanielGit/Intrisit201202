/*
 * Monkey's Audio lossless audio decoder
 * Copyright (c) 2007 Benjamin Zores <ben@geexbox.org>
 *  based upon libdemac from Dave Chapman.
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

#define ALT_BITSTREAM_READER_LE
#include "avcodec.h"
#include "dsputil.h"
#include "bitstream.h"
#include "bytestream.h"

#ifndef __LINUX__
#undef memcpy
#define memcpy uc_memcpy
#endif

/**
 * @file apedec.c
 * Monkey's Audio lossless audio decoder
 */

#define BLOCKS_PER_LOOP     4608
#define MAX_CHANNELS        2
#define MAX_BYTESPERSAMPLE  3

#define APE_FRAMECODE_MONO_SILENCE    1
#define APE_FRAMECODE_STEREO_SILENCE  3
#define APE_FRAMECODE_PSEUDO_STEREO   4

#define HISTORY_SIZE 512
#define PREDICTOR_ORDER 8
/** Total size of all predictor histories */
#define PREDICTOR_SIZE 50

#define YDELAYA (18 + PREDICTOR_ORDER*4)
#define YDELAYB (18 + PREDICTOR_ORDER*3)
#define XDELAYA (18 + PREDICTOR_ORDER*2)
#define XDELAYB (18 + PREDICTOR_ORDER)

#define YADAPTCOEFFSA 18
#define XADAPTCOEFFSA 14
#define YADAPTCOEFFSB 10
#define XADAPTCOEFFSB 5

/**
 * Possible compression levels
 * @{
 */
enum APECompressionLevel {
    COMPRESSION_LEVEL_FAST       = 1000,
    COMPRESSION_LEVEL_NORMAL     = 2000,
    COMPRESSION_LEVEL_HIGH       = 3000,
    COMPRESSION_LEVEL_EXTRA_HIGH = 4000,
    COMPRESSION_LEVEL_INSANE     = 5000
};
/** @} */

#define APE_FILTER_LEVELS 3

/** Filter orders depending on compression level */
static const uint16_t ape_filter_orders[5][APE_FILTER_LEVELS] = {
    {  0,   0,    0 },
    { 16,   0,    0 },
    { 64,   0,    0 },
    { 32, 256,    0 },
    { 16, 256, 1280 }
};

/** Filter fraction bits depending on compression level */
static const uint16_t ape_filter_fracbits[5][APE_FILTER_LEVELS] = {
    {  0,  0,  0 },
    { 11,  0,  0 },
    { 11,  0,  0 },
    { 10, 13,  0 },
    { 11, 13, 15 }
};


/** Filters applied to the decoded data */
typedef struct APEFilter {
    int16_t *coeffs;        ///< actual coefficients used in filtering
    int16_t *adaptcoeffs;   ///< adaptive filter coefficients used for correcting of actual filter coefficients
    int16_t *historybuffer; ///< filter memory
    int16_t *delay;         ///< filtered values

    int avg;
} APEFilter;

typedef struct APERice {
    uint32_t k;
    uint32_t ksum;
} APERice;

typedef struct APERangecoder {
    uint32_t low;           ///< low end of interval
    uint32_t range;         ///< length of interval
    uint32_t help;          ///< bytes_to_follow resp. intermediate value
    unsigned int buffer;    ///< buffer for input/output
} APERangecoder;

/** Filter histories */
typedef struct APEPredictor {
    int32_t *buf;

    int32_t lastA[2];

    int32_t filterA[2];
    int32_t filterB[2];

    int32_t coeffsA[2][4];  ///< adaption coefficients
    int32_t coeffsB[2][5];  ///< adaption coefficients
    int32_t historybuffer[HISTORY_SIZE + PREDICTOR_SIZE];
} APEPredictor;

/** Decoder context */
typedef struct APEContext {
    AVCodecContext *avctx;
    DSPContext dsp;
    int channels;
    int samples;                             ///< samples left to decode in current frame

    int fileversion;                         ///< codec version, very important in decoding process
    int compression_level;                   ///< compression levels
    int fset;                                ///< which filter set to use (calculated from compression level)
    int flags;                               ///< global decoder flags

    uint32_t CRC;                            ///< frame CRC
    int frameflags;                          ///< frame flags
    int currentframeblocks;                  ///< samples (per channel) in current frame
    int blocksdecoded;                       ///< count of decoded samples in current frame
    APEPredictor predictor;                  ///< predictor used for final reconstruction

    int32_t decoded0[BLOCKS_PER_LOOP];       ///< decoded data for the first channel
    int32_t decoded1[BLOCKS_PER_LOOP];       ///< decoded data for the second channel

    int16_t* filterbuf[APE_FILTER_LEVELS];   ///< filter memory

    APERangecoder rc;                        ///< rangecoder used to decode actual values
    APERice riceX;                           ///< rice code parameters for the second channel
    APERice riceY;                           ///< rice code parameters for the first channel
    APEFilter filters[APE_FILTER_LEVELS][2]; ///< filters used for reconstruction

#define DATA_BUFF_SIZE 1024*1024
    //uint8_t *data;                           ///< current frame data
    uint8_t data[DATA_BUFF_SIZE];                           ///< current frame data
    uint8_t *data_end;                       ///< frame data end
    uint8_t *ptr;                            ///< current position in frame data
    uint8_t *last_ptr;                       ///< position where last 4608-sample block ended
    int error;
} APEContext;

#ifndef JZ4740_MXU_OPT
// TODO: dsputilize
static inline void vector_add(int16_t * v1, int16_t * v2, int order)
{
    while (order--)
       *v1++ += *v2++;
}

// TODO: dsputilize
static inline void vector_sub(int16_t * v1, int16_t * v2, int order)
{
    while (order--)
       *v1++ -= *v2++;
}

// TODO: dsputilize
static inline int32_t scalarproduct(int16_t * v1, int16_t * v2, int order)
{
    int res;

    res = 0;
    while (order--)
        res += *v1++ * *v2++;
    return res;
}

#endif
static int ape_decode_init(AVCodecContext * avctx)
{
    APEContext *s = avctx->priv_data;
    int i;

    if (avctx->extradata_size != 6) {
        av_log(avctx, AV_LOG_ERROR, "Incorrect extradata\n");
        return -1;
    }
    if (avctx->bits_per_sample != 16) {
        av_log(avctx, AV_LOG_ERROR, "Only 16-bit samples are supported\n");
        return -1;
    }
    if (avctx->channels > 2) {
        av_log(avctx, AV_LOG_ERROR, "Only mono and stereo is supported\n");
        return -1;
    }
    s->avctx             = avctx;
    s->channels          = avctx->channels;
    s->fileversion       = AV_RL16(avctx->extradata);
    s->compression_level = AV_RL16(avctx->extradata + 2);
    s->flags             = AV_RL16(avctx->extradata + 4);

    av_log(avctx, AV_LOG_DEBUG, "Compression Level: %d - Flags: %d\n", s->compression_level, s->flags);
    if (s->compression_level % 1000 || s->compression_level > COMPRESSION_LEVEL_INSANE) {
        av_log(avctx, AV_LOG_ERROR, "Incorrect compression level %d\n", s->compression_level);
        return -1;
    }
    s->fset = s->compression_level / 1000 - 1;
    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[s->fset][i])
            break;
        s->filterbuf[i] = av_malloc((ape_filter_orders[s->fset][i] * 3 + HISTORY_SIZE) * 4);
    }

    dsputil_init(&s->dsp, avctx);
    return 0;
}

static int ape_decode_close(AVCodecContext * avctx)
{
    APEContext *s = avctx->priv_data;
    int i;

    for (i = 0; i < APE_FILTER_LEVELS; i++)
        av_freep(&s->filterbuf[i]);

    return 0;
}

/**
 * @defgroup rangecoder APE range decoder
 * @{
 */

#define CODE_BITS    32
#define TOP_VALUE    ((unsigned int)1 << (CODE_BITS-1))
#define SHIFT_BITS   (CODE_BITS - 9)
#define EXTRA_BITS   ((CODE_BITS-2) % 8 + 1)
#define BOTTOM_VALUE (TOP_VALUE >> 8)

/** Start the decoder */
static inline void range_start_decoding(APEContext * ctx)
{
    ctx->rc.buffer = bytestream_get_byte(&ctx->ptr);
    ctx->rc.low    = ctx->rc.buffer >> (8 - EXTRA_BITS);
    ctx->rc.range  = (uint32_t) 1 << EXTRA_BITS;
}

/** Perform normalization */
static inline void range_dec_normalize(APEContext * ctx)
{
    while (ctx->rc.range <= BOTTOM_VALUE) {
        ctx->rc.buffer <<= 8;
        if(ctx->ptr < ctx->data_end)
            ctx->rc.buffer += *ctx->ptr;
        ctx->ptr++;
        ctx->rc.low    = (ctx->rc.low << 8)    | ((ctx->rc.buffer >> 1) & 0xFF);
        ctx->rc.range  <<= 8;
    }
}

/**
 * Calculate culmulative frequency for next symbol. Does NO update!
 * @param tot_f is the total frequency or (code_value)1<<shift
 * @return the culmulative frequency
 */
static inline int range_decode_culfreq(APEContext * ctx, int tot_f)
{
    range_dec_normalize(ctx);
    ctx->rc.help = ctx->rc.range / tot_f;
    return ctx->rc.low / ctx->rc.help;
}

/**
 * Decode value with given size in bits
 * @param shift number of bits to decode
 */
static inline int range_decode_culshift(APEContext * ctx, int shift)
{
    range_dec_normalize(ctx);
    ctx->rc.help = ctx->rc.range >> shift;
    return ctx->rc.low / ctx->rc.help;
}


/**
 * Update decoding state
 * @param sy_f the interval length (frequency of the symbol)
 * @param lt_f the lower end (frequency sum of < symbols)
 */
static inline void range_decode_update(APEContext * ctx, int sy_f, int lt_f)
{
    ctx->rc.low  -= ctx->rc.help * lt_f;
    ctx->rc.range = ctx->rc.help * sy_f;
}

/** Decode n bits (n <= 16) without modelling */
static inline int range_decode_bits(APEContext * ctx, int n)
{
    int sym = range_decode_culshift(ctx, n);
    range_decode_update(ctx, 1, sym);
    return sym;
}


#define MODEL_ELEMENTS 64

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.97
 */
static const uint32_t counts_3970[65] = {
        0, 14824, 28224, 39348, 47855, 53994, 58171, 60926,
    62682, 63786, 64463, 64878, 65126, 65276, 65365, 65419,
    65450, 65469, 65480, 65487, 65491, 65493, 65494, 65495,
    65496, 65497, 65498, 65499, 65500, 65501, 65502, 65503,
    65504, 65505, 65506, 65507, 65508, 65509, 65510, 65511,
    65512, 65513, 65514, 65515, 65516, 65517, 65518, 65519,
    65520, 65521, 65522, 65523, 65524, 65525, 65526, 65527,
    65528, 65529, 65530, 65531, 65532, 65533, 65534, 65535,
    65536
};

/**
 * Probability ranges for symbols in Monkey Audio version 3.97
 */
static const uint16_t counts_diff_3970[64] = {
    14824, 13400, 11124, 8507, 6139, 4177, 2755, 1756,
    1104, 677, 415, 248, 150, 89, 54, 31,
    19, 11, 7, 4, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.98
 */
static const uint32_t counts_3980[65] = {
        0, 19578, 36160, 48417, 56323, 60899, 63265, 64435,
    64971, 65232, 65351, 65416, 65447, 65466, 65476, 65482,
    65485, 65488, 65490, 65491, 65492, 65493, 65494, 65495,
    65496, 65497, 65498, 65499, 65500, 65501, 65502, 65503,
    65504, 65505, 65506, 65507, 65508, 65509, 65510, 65511,
    65512, 65513, 65514, 65515, 65516, 65517, 65518, 65519,
    65520, 65521, 65522, 65523, 65524, 65525, 65526, 65527,
    65528, 65529, 65530, 65531, 65532, 65533, 65534, 65535,
    65536
};

/**
 * Probability ranges for symbols in Monkey Audio version 3.98
 */
static const uint16_t counts_diff_3980[64] = {
    19578, 16582, 12257, 7906, 4576, 2366, 1170, 536,
    261, 119, 65, 31, 19, 10, 6, 3,
    3, 2, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

/**
 * Decode symbol
 * @param counts probability range start position
 * @param count_diffs probability range widths
 */
static inline int range_get_symbol(APEContext * ctx,
                                   const uint32_t counts[],
                                   const uint16_t counts_diff[])
{
    int symbol, cf;

    cf = range_decode_culshift(ctx, 16);

    if(cf > 65492){
        symbol= cf - 65535 + 63;
        range_decode_update(ctx, 1, cf);
        if(cf > 65535)
            ctx->error=1;
        return symbol;
		}
    /* figure out the symbol inefficiently; a binary search would be much better */
    for (symbol = 0; counts[symbol + 1] <= cf; symbol++);

    range_decode_update(ctx, counts_diff[symbol], counts[symbol]);

    return symbol;
}
/** @} */ // group rangecoder

static inline void update_rice(APERice *rice, int x)
{
    rice->ksum += ((x + 1) / 2) - ((rice->ksum + 16) >> 5);

    if (rice->k == 0)
        rice->k = 1;
    else if (rice->ksum < (1 << (rice->k + 4)))
        rice->k--;
    else if (rice->ksum >= (1 << (rice->k + 5)))
        rice->k++;
}

static inline int ape_decode_value(APEContext * ctx, APERice *rice)
{
    int x, overflow;

    if (ctx->fileversion < 3980) {
        int tmpk;

        overflow = range_get_symbol(ctx, counts_3970, counts_diff_3970);

        if (overflow == (MODEL_ELEMENTS - 1)) {
            tmpk = range_decode_bits(ctx, 5);
            overflow = 0;
        } else
            tmpk = (rice->k < 1) ? 0 : rice->k - 1;

        if (tmpk <= 16)
            x = range_decode_bits(ctx, tmpk);
        else {
            x = range_decode_bits(ctx, 16);
            x |= (range_decode_bits(ctx, tmpk - 16) << 16);
        }
        x += overflow << tmpk;
    } else {
        int base, pivot;

        pivot = rice->ksum >> 5;
        if (pivot == 0)
            pivot = 1;

        overflow = range_get_symbol(ctx, counts_3980, counts_diff_3980);

        if (overflow == (MODEL_ELEMENTS - 1)) {
            overflow  = range_decode_bits(ctx, 16) << 16;
            overflow |= range_decode_bits(ctx, 16);
        }

        base = range_decode_culfreq(ctx, pivot);
        range_decode_update(ctx, 1, base);

        x = base + overflow * pivot;
    }

    update_rice(rice, x);

    /* Convert to signed */
    if (x & 1)
        return (x >> 1) + 1;
    else
        return -(x >> 1);
}

static void entropy_decode(APEContext * ctx, int blockstodecode, int stereo)
{
    int32_t *decoded0 = ctx->decoded0;
    int32_t *decoded1 = ctx->decoded1;

    ctx->blocksdecoded = blockstodecode;

    if (ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE) {
        /* We are pure silence, just memset the output buffer. */
        memset(decoded0, 0, blockstodecode * sizeof(int32_t));
        memset(decoded1, 0, blockstodecode * sizeof(int32_t));
    } else {
        while (blockstodecode--) {
            *decoded0++ = ape_decode_value(ctx, &ctx->riceY);
            if (stereo)
                *decoded1++ = ape_decode_value(ctx, &ctx->riceX);
        }
    }

    if (ctx->blocksdecoded == ctx->currentframeblocks)
        range_dec_normalize(ctx);   /* normalize to use up all bytes */
}

static void init_entropy_decoder(APEContext * ctx)
{
    /* Read the CRC */
    ctx->CRC = bytestream_get_be32(&ctx->ptr);

    /* Read the frame flags if they exist */
    ctx->frameflags = 0;
    if ((ctx->fileversion > 3820) && (ctx->CRC & 0x80000000)) {
        ctx->CRC &= ~0x80000000;

        ctx->frameflags = bytestream_get_be32(&ctx->ptr);
    }

    /* Keep a count of the blocks decoded in this frame */
    ctx->blocksdecoded = 0;

    /* Initialise the rice structs */
    ctx->riceX.k = 10;
    ctx->riceX.ksum = (1 << ctx->riceX.k) * 16;
    ctx->riceY.k = 10;
    ctx->riceY.ksum = (1 << ctx->riceY.k) * 16;

    /* The first 8 bits of input are ignored. */
    ctx->ptr++;

    range_start_decoding(ctx);
}

static const int32_t initial_coeffs[4] = {
    360, 317, -109, 98
};

static void init_predictor_decoder(APEContext * ctx)
{
    APEPredictor *p = &ctx->predictor;

    /* Zero the history buffers */
    memset(p->historybuffer, 0, PREDICTOR_SIZE * sizeof(int32_t));
    p->buf = p->historybuffer;

    /* Initialise and zero the co-efficients */
    memcpy(p->coeffsA[0], initial_coeffs, sizeof(initial_coeffs));
    memcpy(p->coeffsA[1], initial_coeffs, sizeof(initial_coeffs));
    memset(p->coeffsB, 0, sizeof(p->coeffsB));

    p->filterA[0] = p->filterA[1] = 0;
    p->filterB[0] = p->filterB[1] = 0;
    p->lastA[0]   = p->lastA[1]   = 0;
}

/** Get inverse sign of integer (-1 for positive, 1 for negative and 0 for zero) */
static inline int APESIGN(int32_t x) {
    return (x < 0) - (x > 0);
}

static int predictor_update_filter(APEPredictor *p, const int decoded, const int filter, const int delayA, const int delayB, const int adaptA, const int adaptB)
{
    int32_t predictionA, predictionB;

    p->buf[delayA]     = p->lastA[filter];
    p->buf[adaptA]     = APESIGN(p->buf[delayA]);
    p->buf[delayA - 1] = p->buf[delayA] - p->buf[delayA - 1];
    p->buf[adaptA - 1] = APESIGN(p->buf[delayA - 1]);

    predictionA = p->buf[delayA    ] * p->coeffsA[filter][0] +
                  p->buf[delayA - 1] * p->coeffsA[filter][1] +
                  p->buf[delayA - 2] * p->coeffsA[filter][2] +
                  p->buf[delayA - 3] * p->coeffsA[filter][3];

    /*  Apply a scaled first-order filter compression */
    p->buf[delayB]     = p->filterA[filter ^ 1] - ((p->filterB[filter] * 31) >> 5);
    p->buf[adaptB]     = APESIGN(p->buf[delayB]);
    p->buf[delayB - 1] = p->buf[delayB] - p->buf[delayB - 1];
    p->buf[adaptB - 1] = APESIGN(p->buf[delayB - 1]);
    p->filterB[filter] = p->filterA[filter ^ 1];

    predictionB = p->buf[delayB    ] * p->coeffsB[filter][0] +
                  p->buf[delayB - 1] * p->coeffsB[filter][1] +
                  p->buf[delayB - 2] * p->coeffsB[filter][2] +
                  p->buf[delayB - 3] * p->coeffsB[filter][3] +
                  p->buf[delayB - 4] * p->coeffsB[filter][4];

    p->lastA[filter] = decoded + ((predictionA + (predictionB >> 1)) >> 10);
    p->filterA[filter] = p->lastA[filter] + ((p->filterA[filter] * 31) >> 5);

    if (!decoded) // no need updating filter coefficients
        return p->filterA[filter];

    if (decoded > 0) {
        p->coeffsA[filter][0] -= p->buf[adaptA    ];
        p->coeffsA[filter][1] -= p->buf[adaptA - 1];
        p->coeffsA[filter][2] -= p->buf[adaptA - 2];
        p->coeffsA[filter][3] -= p->buf[adaptA - 3];

        p->coeffsB[filter][0] -= p->buf[adaptB    ];
        p->coeffsB[filter][1] -= p->buf[adaptB - 1];
        p->coeffsB[filter][2] -= p->buf[adaptB - 2];
        p->coeffsB[filter][3] -= p->buf[adaptB - 3];
        p->coeffsB[filter][4] -= p->buf[adaptB - 4];
    } else {
        p->coeffsA[filter][0] += p->buf[adaptA    ];
        p->coeffsA[filter][1] += p->buf[adaptA - 1];
        p->coeffsA[filter][2] += p->buf[adaptA - 2];
        p->coeffsA[filter][3] += p->buf[adaptA - 3];

        p->coeffsB[filter][0] += p->buf[adaptB    ];
        p->coeffsB[filter][1] += p->buf[adaptB - 1];
        p->coeffsB[filter][2] += p->buf[adaptB - 2];
        p->coeffsB[filter][3] += p->buf[adaptB - 3];
        p->coeffsB[filter][4] += p->buf[adaptB - 4];
    }
    return p->filterA[filter];
}

static void predictor_decode_stereo(APEContext * ctx, int count)
{
    int32_t predictionA, predictionB;
    APEPredictor *p = &ctx->predictor;
    int32_t *decoded0 = ctx->decoded0;
    int32_t *decoded1 = ctx->decoded1;

    while (count--) {
        /* Predictor Y */
        predictionA = predictor_update_filter(p, *decoded0, 0, YDELAYA, YDELAYB, YADAPTCOEFFSA, YADAPTCOEFFSB);
        predictionB = predictor_update_filter(p, *decoded1, 1, XDELAYA, XDELAYB, XADAPTCOEFFSA, XADAPTCOEFFSB);
        *(decoded0++) = predictionA;
        *(decoded1++) = predictionB;

        /* Combined */
        p->buf++;

        /* Have we filled the history buffer? */
        if (p->buf == p->historybuffer + HISTORY_SIZE) {
            memmove(p->historybuffer, p->buf, PREDICTOR_SIZE * sizeof(int32_t));
            p->buf = p->historybuffer;
        }
    }
}

static void predictor_decode_mono(APEContext * ctx, int count)
{
    APEPredictor *p = &ctx->predictor;
    int32_t *decoded0 = ctx->decoded0;
    int32_t predictionA, currentA, A;

    currentA = p->lastA[0];

    while (count--) {
        A = *decoded0;

        p->buf[YDELAYA] = currentA;
        p->buf[YDELAYA - 1] = p->buf[YDELAYA] - p->buf[YDELAYA - 1];

        predictionA = p->buf[YDELAYA    ] * p->coeffsA[0][0] +
                      p->buf[YDELAYA - 1] * p->coeffsA[0][1] +
                      p->buf[YDELAYA - 2] * p->coeffsA[0][2] +
                      p->buf[YDELAYA - 3] * p->coeffsA[0][3];

        currentA = A + (predictionA >> 10);

        p->buf[YADAPTCOEFFSA]     = APESIGN(p->buf[YDELAYA    ]);
        p->buf[YADAPTCOEFFSA - 1] = APESIGN(p->buf[YDELAYA - 1]);

        if (A > 0) {
            p->coeffsA[0][0] -= p->buf[YADAPTCOEFFSA    ];
            p->coeffsA[0][1] -= p->buf[YADAPTCOEFFSA - 1];
            p->coeffsA[0][2] -= p->buf[YADAPTCOEFFSA - 2];
            p->coeffsA[0][3] -= p->buf[YADAPTCOEFFSA - 3];
        } else if (A < 0) {
            p->coeffsA[0][0] += p->buf[YADAPTCOEFFSA    ];
            p->coeffsA[0][1] += p->buf[YADAPTCOEFFSA - 1];
            p->coeffsA[0][2] += p->buf[YADAPTCOEFFSA - 2];
            p->coeffsA[0][3] += p->buf[YADAPTCOEFFSA - 3];
        }

        p->buf++;

        /* Have we filled the history buffer? */
        if (p->buf == p->historybuffer + HISTORY_SIZE) {
            memmove(p->historybuffer, p->buf, PREDICTOR_SIZE * sizeof(int32_t));
            p->buf = p->historybuffer;
        }

        p->filterA[0] = currentA + ((p->filterA[0] * 31) >> 5);
        *(decoded0++) = p->filterA[0];
    }

    p->lastA[0] = currentA;
}

static void do_init_filter(APEFilter *f, int16_t * buf, int order)
{
    f->coeffs = buf;
    f->historybuffer = buf + order;
    f->delay       = f->historybuffer + order * 2;
    f->adaptcoeffs = f->historybuffer + order;

    memset(f->historybuffer, 0, (order * 2) * sizeof(int16_t));
    memset(f->coeffs, 0, order * sizeof(int16_t));
    f->avg = 0;
}

static void init_filter(APEContext * ctx, APEFilter *f, int16_t * buf, int order)
{
    do_init_filter(&f[0], buf, order);
    do_init_filter(&f[1], buf + order * 3 + HISTORY_SIZE, order);
}

static inline void do_apply_filter(int version, APEFilter *f, int32_t *data, int count, int order, int fracbits)
{
    int res;
    int absres;
    int round_val = (1 << (fracbits - 1));
    int16_t *coeffs_p, *adaptcoeffs_p, *delay_p, *coeffs_deta, *delay_deta, *adapt_deta;
    int halforder;
#if defined(JZ4740_MXU_OPT) && !defined(JZ4750_OPT)
    uint32_t  src1_aln, src2_aln, src1_off, src2_off;
#endif
    coeffs_p = f->coeffs;
    adaptcoeffs_p = f->adaptcoeffs;
    delay_p = f->delay;

    while (count--) {

 
#ifdef JZ4740_MXU_OPT
        coeffs_deta = coeffs_p;
        delay_deta = delay_p - order;
        adapt_deta = adaptcoeffs_p - order;
        halforder = order >> 1;
        S32LDD(xr1, coeffs_deta, 0); 
#ifdef JZ4750_OPT
        S16LDD(xr2, delay_deta, 0x0, ptn0);
        S16LDD(xr2, delay_deta, 0x2, ptn1);
#else
        src1_aln = ((uint32_t)delay_deta)&0xFFFFFFFC;
        src1_off = 4 - (((uint32_t)delay_deta)&0x3);
        S32LDD(xr13, src1_aln, 0x0);
        S32LDD(xr14, src1_aln, 0x4);
        S32ALN(xr2, xr14, xr13, src1_off);
#endif
        if(*data==0){
           D16MUL_WW(xr4, xr1, xr2, xr5);
           while(--halforder){
              S32LDI(xr1, coeffs_deta, 0x4);
#ifdef JZ4750_OPT
              S16LDI(xr2, delay_deta, 0x4, ptn0);
              S16LDD(xr2, delay_deta, 0x2, ptn1);
#else
              S32LDI(xr13, src1_aln, 0x4);
              S32LDD(xr14, src1_aln, 0x4);
              S32ALN(xr2, xr14, xr13, src1_off);
#endif
              D16MAC_AA_WW(xr4, xr1, xr2, xr5);
           }
        }else{
#ifdef JZ4750_OPT
           S16LDD(xr3, adapt_deta, 0x0, ptn0);
           S16LDD(xr3, adapt_deta, 0x2, ptn1);
#else
           src2_aln = ((uint32_t)adapt_deta)&0xFFFFFFFC;
           src2_off = 4 - (((uint32_t)adapt_deta)&0x3);
           S32LDD(xr13, src2_aln, 0x0);
           S32LDD(xr14, src2_aln, 0x4);
           S32ALN(xr3, xr14, xr13, src2_off);
#endif
           S32LDD(xr10, data, 0x0);
           D16MUL_WW(xr4, xr1, xr2, xr5);
           Q16ADD_AS_WW(xr6, xr1, xr3, xr7);
#ifdef JZ4750_OPT
           S32SLT(xr11, xr10,xr0);
           S32MOVZ(xr6, xr11, xr7);
#else
           if(*data>0)
             Q16ADD_AA_WW(xr6, xr7,xr0,xr0);
#endif
           S32STD(xr6, coeffs_deta, 0x0);
           while(--halforder){
              S32LDI(xr1, coeffs_deta, 0x4);
#ifdef JZ4750_OPT
              S16LDI(xr2, delay_deta, 0x4, ptn0);
              S16LDD(xr2, delay_deta, 0x2, ptn1);
#else
              S32LDI(xr13, src1_aln, 0x4);
              S32LDD(xr14, src1_aln, 0x4);
              S32ALN(xr2, xr14, xr13, src1_off);
#endif

#ifdef JZ4750_OPT
              S16LDI(xr3, adapt_deta, 0x4, ptn0);
              S16LDD(xr3, adapt_deta, 0x2, ptn1);
#else
              S32LDI(xr13, src2_aln, 0x4);
              S32LDD(xr14, src2_aln, 0x4);
              S32ALN(xr3, xr14, xr13, src2_off);
#endif
              D16MAC_AA_WW(xr4, xr1, xr2, xr5);
              Q16ADD_AS_WW(xr6, xr1, xr3, xr7);
#ifdef JZ4750_OPT
              S32MOVZ(xr6, xr11, xr7);
#else
              if(*data>0)
                Q16ADD_AA_WW(xr6, xr7,xr0,xr0);
#endif
              S32STD(xr6, coeffs_deta, 0x0);
           }
        }
        D32ADD_AA(xr4,xr5,xr4,xr0);
        res = S32M2I(xr4);
        res += round_val;
        res >>= fracbits;
#else //JZ4750_OPT
        /* round fixedpoint scalar product */
        res = (scalarproduct(delay_p-order, coeffs_p, order) + round_val) >> fracbits;

        if (*data < 0)
            vector_add(coeffs_p, adaptcoeffs_p - order, order);
        else if (*data > 0)
            vector_sub(coeffs_p, adaptcoeffs_p - order, order);
#endif //JZ4750_OPT
        res += *data;

        *data++ = res;

        /* Update the output history */
        *delay_p++ = av_clip_int16(res);

        if (version < 3980) {
            /* Version ??? to < 3.98 files (untested) */
            adaptcoeffs_p[0]  = (res == 0) ? 0 : ((res >> 28) & 8) - 4;
            adaptcoeffs_p[-4] >>= 1;
            adaptcoeffs_p[-8] >>= 1;
        } else {
            /* Version 3.98 and later files */

            /* Update the adaption coefficients */
            absres = (res < 0 ? -res : res);

            if (absres > (f->avg * 3))
                *adaptcoeffs_p = ((res >> 25) & 64) - 32;
            else if (absres > (f->avg * 4) / 3)
                *adaptcoeffs_p = ((res >> 26) & 32) - 16;
            else if (absres > 0)
                *adaptcoeffs_p = ((res >> 27) & 16) - 8;
            else
                *adaptcoeffs_p = 0;

            f->avg += (absres - f->avg) / 16;

            adaptcoeffs_p[-1] >>= 1;
            adaptcoeffs_p[-2] >>= 1;
            adaptcoeffs_p[-8] >>= 1;
        }

        adaptcoeffs_p++;

        /* Have we filled the history buffer? */
        if (delay_p == f->historybuffer + HISTORY_SIZE + (order * 2)) {
            memmove(f->historybuffer, delay_p - (order * 2),
                    (order * 2) * sizeof(int16_t));
            delay_p = f->historybuffer + order * 2;
            adaptcoeffs_p = f->historybuffer + order;
        }
    }
    f->adaptcoeffs = adaptcoeffs_p;
    f->delay = delay_p;
}

static void apply_filter(APEContext * ctx, APEFilter *f,
                         int32_t * data0, int32_t * data1,
                         int count, int order, int fracbits)
{
    do_apply_filter(ctx->fileversion, &f[0], data0, count, order, fracbits);
    if (data1)
        do_apply_filter(ctx->fileversion, &f[1], data1, count, order, fracbits);
}

static void ape_apply_filters(APEContext * ctx, int32_t * decoded0,
                              int32_t * decoded1, int count)
{
    int i;

    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[ctx->fset][i])
            break;
        apply_filter(ctx, ctx->filters[i], decoded0, decoded1, count, ape_filter_orders[ctx->fset][i], ape_filter_fracbits[ctx->fset][i]);
    }
}

static void init_frame_decoder(APEContext * ctx)
{
    int i;
    init_entropy_decoder(ctx);
    init_predictor_decoder(ctx);
		for (i = 0; i < APE_FILTER_LEVELS; i++) {

        if (!ape_filter_orders[ctx->fset][i])
            break;
        init_filter(ctx, ctx->filters[i], ctx->filterbuf[i], ape_filter_orders[ctx->fset][i]);
    }
}

static void ape_unpack_mono(APEContext * ctx, int count)
{
    int32_t left;
    int32_t *decoded0 = ctx->decoded0;
    int32_t *decoded1 = ctx->decoded1;

    if (ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE) {
        entropy_decode(ctx, count, 0);
        /* We are pure silence, so we're done. */
        av_log(ctx->avctx, AV_LOG_DEBUG, "pure silence mono\n");
        return;
    }

    entropy_decode(ctx, count, 0);
    ape_apply_filters(ctx, decoded0, NULL, count);

    /* Now apply the predictor decoding */
    predictor_decode_mono(ctx, count);

    /* Pseudo-stereo - just copy left channel to right channel */
    if (ctx->channels == 2) {
        while (count--) {
            left = *decoded0;
            *(decoded1++) = *(decoded0++) = left;
        }
    }
}

static void ape_unpack_stereo(APEContext * ctx, int count)
{
    int32_t left, right;
    int32_t *decoded0 = ctx->decoded0;
    int32_t *decoded1 = ctx->decoded1;

    if (ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE) {
        /* We are pure silence, so we're done. */
        av_log(ctx->avctx, AV_LOG_DEBUG, "pure silence stereo\n");
        return;
    }

    entropy_decode(ctx, count, 1);
    ape_apply_filters(ctx, decoded0, decoded1, count);

    /* Now apply the predictor decoding */
    predictor_decode_stereo(ctx, count);


    /* Decorrelate and scale to output depth */
    while (count--) {
        left = *decoded1 - (*decoded0 / 2);
        right = left + *decoded0;

        *(decoded0++) = left;
        *(decoded1++) = right;
    }
}

static int ape_decode_frame(AVCodecContext * avctx,
                            void *data, int *data_size,
                            uint8_t * buf, int buf_size)
{
    APEContext *s = avctx->priv_data;
    int16_t *samples = data;
    int nblocks;
    int i, n;
    int blockstodecode;
    int bytes_used;

    if (buf_size == 0 && !s->samples) {
        *data_size = 0;
        return 0;
    }

    if (DATA_BUFF_SIZE<buf_size) {
        *data_size = 0;
        return buf_size;
    }

   /* should not happen but who knows */
    if (BLOCKS_PER_LOOP * 2 * avctx->channels > *data_size) {
        av_log (avctx, AV_LOG_ERROR, "Packet size is too big to be handled in lavc! (max is %d where you have %d)\n", *data_size, s->samples * 2 * avctx->channels);
        return -1;
    }

    if(!s->samples){
        //s->data = av_realloc(s->data, (buf_size + 3) & ~3);
        s->dsp.bswap_buf(s->data, buf, buf_size >> 2);
        s->ptr = s->last_ptr = s->data;
        s->data_end = s->data + buf_size;

        nblocks = s->samples = bytestream_get_be32(&s->ptr);
        n =  bytestream_get_be32(&s->ptr);
        if(n < 0 || n > 3){
            av_log(avctx, AV_LOG_ERROR, "Incorrect offset passed\n");
            return -1;
        }
        s->ptr += n;

        s->currentframeblocks = nblocks;
        buf += 4;
        if (s->samples <= 0) {
            *data_size = 0;
            return buf_size;
        }

        memset(s->decoded0,  0, sizeof(s->decoded0));
        memset(s->decoded1,  0, sizeof(s->decoded1));

        /* Initialize the frame decoder */
        init_frame_decoder(s);
    }

    nblocks = s->samples;
    blockstodecode = FFMIN(BLOCKS_PER_LOOP, nblocks);

    if ((s->channels == 1) || (s->frameflags & APE_FRAMECODE_PSEUDO_STEREO)){
        ape_unpack_mono(s, blockstodecode);
    } else {
        ape_unpack_stereo(s, blockstodecode);
    }
   if(s->error || s->ptr > s->data_end){
        s->samples=0;
        av_log(avctx, AV_LOG_ERROR, "Error decoding frame\n");
        return -1;
    }
    for (i = 0; i < blockstodecode; i++) {
        *samples++ = s->decoded0[i];
        if(s->channels == 2)
            *samples++ = s->decoded1[i];
    }

    s->samples -= blockstodecode;

    *data_size = blockstodecode * 2 * s->channels;
    bytes_used = s->samples ? s->ptr - s->last_ptr : buf_size;
    s->last_ptr = s->ptr;
    return bytes_used;
}

AVCodec ape_decoder = {
    "ape",
    CODEC_TYPE_AUDIO,
    CODEC_ID_APE,
    sizeof(APEContext),
    ape_decode_init,
    NULL,
    ape_decode_close,
    ape_decode_frame,
};