/*
 * a52_internal.h
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of a52dec, a free ATSC A-52 stream decoder.
 * See http://liba52.sourceforge.net/ for updates.
 *
 * a52dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * a52dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

typedef struct {
    uint8_t bai;		/* fine SNR offset, fast gain */
    uint8_t deltbae;		/* delta bit allocation exists */
    int8_t deltba[50];		/* per-band delta bit allocation */
} ba_t;

typedef struct {
    uint8_t exp[256];		/* decoded channel exponents */
    int8_t bap[256];		/* derived channel bit allocation */
} expbap_t;

struct a52_state_s {
    uint8_t fscod;		/* sample rate */
    uint8_t halfrate;		/* halfrate factor */
    uint8_t acmod;		/* coded channels */
    uint8_t lfeon;		/* coded lfe channel */
    level_t clev;		/* centre channel mix level */
    level_t slev;		/* surround channels mix level */

    int output;			/* type of output */
    level_t level;		/* output level */
    sample_t bias;		/* output bias */

    int dynrnge;		/* apply dynamic range */
    level_t dynrng;		/* dynamic range */
    void * dynrngdata;		/* dynamic range callback funtion and data */
    level_t (* dynrngcall) (level_t range, void * dynrngdata);

    uint8_t chincpl;		/* channel coupled */
    uint8_t phsflginu;		/* phase flags in use (stereo only) */
    uint8_t cplstrtmant;	/* coupling channel start mantissa */
    uint8_t cplendmant;		/* coupling channel end mantissa */
    uint32_t cplbndstrc;	/* coupling band structure */
    level_t cplco[5][18];	/* coupling coordinates */

    /* derived information */
    uint8_t cplstrtbnd;		/* coupling start band (for bit allocation) */
    uint8_t ncplbnd;		/* number of coupling bands */

    uint8_t rematflg;		/* stereo rematrixing */

    uint8_t endmant[5];		/* channel end mantissa */

    uint16_t bai;		/* bit allocation information */

    uint32_t * buffer_start;
    uint16_t lfsr_state;	/* dither state */
    uint32_t bits_left;
    uint32_t current_word;

    uint8_t csnroffst;		/* coarse SNR offset */
    ba_t cplba;			/* coupling bit allocation parameters */
    ba_t ba[5];			/* channel bit allocation parameters */
    ba_t lfeba;			/* lfe bit allocation parameters */

    uint8_t cplfleak;		/* coupling fast leak init */
    uint8_t cplsleak;		/* coupling slow leak init */

    expbap_t cpl_expbap;
    expbap_t fbw_expbap[5];
    expbap_t lfe_expbap;

    sample_t * samples;
    int downmixed;
};

#define LEVEL_PLUS6DB 2.0
#define LEVEL_PLUS3DB 1.4142135623730951
#define LEVEL_3DB 0.7071067811865476
#define LEVEL_45DB 0.5946035575013605
#define LEVEL_6DB 0.5
#define LEVEL_ONE 1

// these next two constants are used for DPL matrix encoding in downmix.c
#define LEVEL_SQRT_1_2 0.5
#define LEVEL_SQRT_3_4 0.8660254037844386

#define EXP_REUSE (0)
#define EXP_D15   (1)
#define EXP_D25   (2)
#define EXP_D45   (3)

#define DELTA_BIT_REUSE (0)
#define DELTA_BIT_NEW (1)
#define DELTA_BIT_NONE (2)
#define DELTA_BIT_RESERVED (3)

void a52_bit_allocate (a52_state_t * state, ba_t * ba, int bndstart,
		       int start, int end, int fastleak, int slowleak,
		       expbap_t * expbap);

int a52_downmix_init (int input, int flags, level_t * level,
		      level_t clev, level_t slev);
int a52_downmix_coeff (level_t * coeff, int acmod, int output, level_t level,
		       level_t clev, level_t slev);
void a52_downmix (sample_t * samples, int acmod, int output, sample_t bias,
		  level_t clev, level_t slev);
void a52_upmix (sample_t * samples, int acmod, int output);

void a52_imdct_init (uint32_t accel);
void a52_imdct_256 (sample_t * data, sample_t * delay, sample_t bias);
void a52_imdct_512 (sample_t * data, sample_t * delay, sample_t bias);

uint16_t a52_crc16_block(uint8_t *data, uint32_t num_bytes);

#define ROUND(x) ((int)((x) + ((x) > 0 ? 0.5 : -0.5)))

typedef struct {
#ifdef ARCH_PPC
    uint8_t regv[12*16];
#endif
    int dummy;
} cpu_state_t;

/* cpu_accel.c */
uint32_t a52_detect_accel (uint32_t accel);

/* cpu_state.c */
void a52_cpu_state_init (uint32_t accel);

#ifndef LIBA52_FIXED

typedef sample_t quantizer_t;
#define SAMPLE(x) (x)
#define LEVEL(x) (x)
#define MUL(a,b) ((a) * (b))
#define MUL_L(a,b) ((a) * (b))
#define MUL_C(a,b) ((a) * (b))
#define DIV(a,b) ((a) / (b))
#define BIAS(x) ((x) + bias)

#else /* LIBA52_FIXED */

typedef int16_t quantizer_t;
#define SAMPLE(x) (sample_t)((x) * (1 << 30))
#define LEVEL(x) (level_t)((x) * (1 << 26))

#define LEVEL_3DB_30 1719614413
#define LEVEL_3DB_26 47453132
#define LEVEL_PLUS6DB_26 134217728
#define LEVEL_6DB_26 33554432
#define LEVEL_PLUS3DB_26 94906265
#define LEVEL_ONE_26 67108864


#ifdef FPM_MIPS 
#define MUL(a,b) \
__extension__({   \
    int32_t __d, hi;  \
    uint32_t lo;      \
    asm ("mult %2, %3"   \
         :"=hi"(hi), "=lo"(lo)  \
         :"%r"(a), "r"(b));     \
    __d = ( hi << 2) | (lo>>30);\
})
#define MUL_L(a,b) \
__extension__({   \
    int32_t __d, hi;  \
    uint32_t lo;      \
    asm ("mult %2, %3"   \
         :"=hi"(hi), "=lo"(lo)  \
         :"%r"(a), "r"(b));     \
    __d = (hi << 6) | (lo>>26);\
})
#else
#define MUL(a,b) \
({ int32_t _ta=(a), _tb=(b), _tc; \
   _tc=(_ta & 0xffff)*(_tb >> 16)+(_ta >> 16)*(_tb & 0xffff); (int32_t)(((_tc >> 14))+ (((_ta >> 16)*(_tb >> 16)) << 2 )); })
#define MUL_L(a,b) \
({ int32_t _ta=(a), _tb=(b), _tc; \
   _tc=(_ta & 0xffff)*(_tb >> 16)+(_ta >> 16)*(_tb & 0xffff); (int32_t)((_tc >> 10) + (((_ta >> 16)*(_tb >> 16)) << 6)); })
#endif

//#define MUL_C(a,b) MUL_L (a, LEVEL (b))
//#define DIV(a,b) ((((int64_t)LEVEL (a)) << 26) / (b))
#define MUL_C(a,b) MUL_L (a, b##_26)
#define DIV(a,b) ((((int64_t)a##_26) << 26) / (b))
#define BIAS(x) (x)

#endif
