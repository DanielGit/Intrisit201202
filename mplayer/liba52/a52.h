/*
 * a52.h
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

#ifndef A52_H
#define A52_H
#if defined(LIBA52_FIXED)
typedef int sample_t;
typedef int level_t;
#elif defined(LIBA52_DOUBLE)
typedef double sample_t;
typedef double level_t;

#else
typedef float sample_t;
typedef float level_t;
#endif

#ifdef __LINUX__
#include <inttypes.h>
#else
#include "uclib.h"
#endif
	
typedef struct a52_state_s a52_state_t;

#define A52_CHANNEL 0
#define A52_MONO 1
#define A52_STEREO 2
#define A52_3F 3
#define A52_2F1R 4
#define A52_3F1R 5
#define A52_2F2R 6
#define A52_3F2R 7
#define A52_CHANNEL1 8
#define A52_CHANNEL2 9
#define A52_DOLBY 10
#define A52_CHANNEL_MASK 15

#define A52_LFE 16
#define A52_ADJUST_LEVEL 32

// this next constant can be ORed with A52_DOLBY to tell liba52 to use 5.0 DPLII matrix encoding,
// rather than just 4.0 Dolby Surround matrix encoding
#define A52_USE_DPLII 64	
	
#define A52_ACCEL_DJBFFT 65536

#define A52_ACCEL_X86_MMX 1
#define A52_ACCEL_X86_3DNOW 2
#define A52_ACCEL_X86_MMXEXT 4
#define A52_ACCEL_X86_SSE2 8
#define A52_ACCEL_X86_SSE3 16
#define A52_ACCEL_PPC_ALTIVEC 1
#define A52_ACCEL_ALPHA 1
#define A52_ACCEL_ALPHA_MVI 2
#define A52_ACCEL_SPARC_VIS 1
#define A52_ACCEL_SPARC_VIS2 2
#define A52_ACCEL_DETECT 0x80000000

uint32_t a52_accel (uint32_t accel);
a52_state_t * a52_init (void);
sample_t * a52_samples (a52_state_t * state);
int a52_syncinfo (const uint8_t * buf, int * flags,
		  int * sample_rate, int * bit_rate);
int a52_crc (uint8_t * buf, int len);
int a52_frame (a52_state_t * state, uint8_t * buf, int * flags,
	       level_t * level, sample_t bias);
void a52_dynrng (a52_state_t * state,
		 level_t (* call) (level_t, void *), void * data);
int a52_block (a52_state_t * state);
void a52_free (a52_state_t * state);
void* a52_resample_init(uint32_t mm_accel,int flags,int chans);
extern int (* a52_resample) (sample_t * _f, int16_t * s16);
#endif /* A52_H */



