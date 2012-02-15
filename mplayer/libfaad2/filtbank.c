/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2004 M. Bakker, Ahead Software AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Ahead Software through Mpeg4AAClicense@nero.com.
**
** $Id: filtbank.c,v 1.3 2008/12/27 08:47:55 xyzhang Exp $
**/

#include "common.h"
#include "structs.h"

#ifdef __LINUX__
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include <mplaylib.h>
#endif

#ifdef _WIN32_WCE
#define assert(x)
#else
#include <assert.h>
#endif

#include "filtbank.h"
#include "decoder.h"
#include "syntax.h"
#include "kbd_win.h"
#include "sine_win.h"
#include "mdct.h"


fb_info *filter_bank_init(uint16_t frame_len)
{
    uint16_t nshort = frame_len/8;
#ifdef LD_DEC
    uint16_t frame_len_ld = frame_len/2;
#endif

    fb_info *fb = (fb_info*)faad_malloc(sizeof(fb_info));
    memset(fb, 0, sizeof(fb_info));

    /* normal */
    fb->mdct256 = faad_mdct_init(2*nshort);
    fb->mdct2048 = faad_mdct_init(2*frame_len);
#ifdef LD_DEC
    /* LD */
    fb->mdct1024 = faad_mdct_init(2*frame_len_ld);
#endif

#ifdef ALLOW_SMALL_FRAMELENGTH
    if (frame_len == 1024)
    {
#endif
        fb->long_window[0]  = sine_long_1024;
        fb->short_window[0] = sine_short_128;
        fb->long_window[1]  = kbd_long_1024;
        fb->short_window[1] = kbd_short_128;
#ifdef LD_DEC
        fb->ld_window[0] = sine_mid_512;
        fb->ld_window[1] = ld_mid_512;
#endif
#ifdef ALLOW_SMALL_FRAMELENGTH
    } else /* (frame_len == 960) */ {
        fb->long_window[0]  = sine_long_960;
        fb->short_window[0] = sine_short_120;
        fb->long_window[1]  = kbd_long_960;
        fb->short_window[1] = kbd_short_120;
#ifdef LD_DEC
        fb->ld_window[0] = sine_mid_480;
        fb->ld_window[1] = ld_mid_480;
#endif
    }
#endif

    return fb;
}

void filter_bank_end(fb_info *fb)
{
    if (fb != NULL)
    {
#ifdef PROFILE
        printf("FB:                 %I64d cycles\n", fb->cycles);
#endif

        faad_mdct_end(fb->mdct256);
        faad_mdct_end(fb->mdct2048);
#ifdef LD_DEC
        faad_mdct_end(fb->mdct1024);
#endif

        faad_free(fb);
    }

}

static INLINE void imdct_long(fb_info *fb, real_t *in_data, real_t *out_data, uint16_t len)
{
#ifdef LD_DEC
    mdct_info *mdct = NULL;

    switch (len)
    {
    case 2048:
    case 1920:
        mdct = fb->mdct2048;
        break;
    case 1024:
    case 960:
        mdct = fb->mdct1024;
        break;
    }

    faad_imdct(mdct, in_data, out_data);
#else
    faad_imdct(fb->mdct2048, in_data, out_data);
#endif
}


#ifdef LTP_DEC
static INLINE void mdct(fb_info *fb, real_t *in_data, real_t *out_data, uint16_t len)
{
    mdct_info *mdct = NULL;

    switch (len)
    {
    case 2048:
    case 1920:
        mdct = fb->mdct2048;
        break;
    case 256:
    case 240:
        mdct = fb->mdct256;
        break;
#ifdef LD_DEC
    case 1024:
    case 960:
        mdct = fb->mdct1024;
        break;
#endif
    }

    faad_mdct(mdct, in_data, out_data);
}
#endif
#if defined(JZ4750_OPT)
void ifilter_bank(fb_info *fb, uint8_t window_sequence, uint8_t window_shape,
                  uint8_t window_shape_prev, real_t *freq_in,
                  real_t *time_out, real_t *overlap,
                  uint8_t object_type, uint16_t frame_len)
{
    int16_t i;
    ALIGN real_t transf_buf[2*1024] = {0};

    const real_t *window_long = NULL;
    const real_t *window_long_prev = NULL;
    const real_t *window_short = NULL;
    const real_t *window_short_prev = NULL;
    real_t *pover, *ptime;
    real_t *idxfreq, *idxtran;
    real_t tmpidx, tranidx, winidx1,winidx2;

    uint16_t nlong = frame_len;
    uint16_t nshort = frame_len>>3;
    uint16_t trans = nshort>>1;

    uint16_t nflat_ls = (nlong-nshort)>>1;


    /* select windows of current frame and previous frame (Sine or KBD) */
#ifdef LD_DEC
    if (object_type == LD)
    {
        window_long       = fb->ld_window[window_shape];
        window_long_prev  = fb->ld_window[window_shape_prev];
    } else {
#endif
        window_long       = fb->long_window[window_shape];
        window_long_prev  = fb->long_window[window_shape_prev];
        window_short      = fb->short_window[window_shape];
        window_short_prev = fb->short_window[window_shape_prev];
#ifdef LD_DEC
    }
#endif



    switch (window_sequence)
    {
    case ONLY_LONG_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        pover = overlap - 1;
        ptime = time_out - 1;
        for (i = 0; i < nlong; i+=4)
        {
            S32MUL(xr3,xr4, transf_buf[i], window_long_prev[i]);
            S32LDI(xr1, pover, 4);
            S32MUL(xr5,xr6, transf_buf[i+1], window_long_prev[i+1]);
            S32LDI(xr2, pover, 4);
            D32SLL(xr3,xr3,xr5,xr5,1);
            D32ASUM_AA(xr3,xr1,xr2,xr5);
            S32SDI(xr3, ptime, 4);
            S32MUL(xr9,xr10, transf_buf[i+2], window_long_prev[i+2]);
            S32SDI(xr5, ptime, 4);
            S32MUL(xr11,xr12, transf_buf[i+3], window_long_prev[i+3]);
            S32LDI(xr7, pover, 4);
            S32LDI(xr8, pover, 4);
            D32SLL(xr9,xr9,xr11,xr11,1);
            D32ASUM_AA(xr9,xr7,xr8,xr11);
            S32SDI(xr9, ptime, 4);
            S32SDI(xr11, ptime, 4);
        }

        pover = overlap - 1;
        /* window the second half and save as overlap for next frame */
        for (i = 0; i < nlong; i+=4)
        {
            int idxtranl;
            int idxwinl;
            
            idxtranl = nlong + i;
            idxwinl = nlong - i - 1;
            S32MUL(xr1, xr2, transf_buf[idxtranl++],window_long[idxwinl--]);
            S32MUL(xr3, xr4, transf_buf[idxtranl++],window_long[idxwinl--]);
            S32MUL(xr5, xr6, transf_buf[idxtranl++],window_long[idxwinl--]);
            S32MUL(xr7, xr8, transf_buf[idxtranl++],window_long[idxwinl--]);
            D32SLL(xr1,xr1,xr3,xr3,1);
            S32SDI(xr1,pover, 4);
            S32SDI(xr3,pover, 4);
            D32SLL(xr5,xr5,xr7,xr7,1);
            S32SDI(xr5,pover, 4);
            S32SDI(xr7,pover, 4);
        }
        break;

    case LONG_START_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        for (i = 0; i < nlong; i+=4)
        {
            time_out[i]   = overlap[i]   + MUL_F(transf_buf[i],window_long_prev[i]);
            time_out[i+1] = overlap[i+1] + MUL_F(transf_buf[i+1],window_long_prev[i+1]);
            time_out[i+2] = overlap[i+2] + MUL_F(transf_buf[i+2],window_long_prev[i+2]);
            time_out[i+3] = overlap[i+3] + MUL_F(transf_buf[i+3],window_long_prev[i+3]);
        }

        /* window the second half and save as overlap for next frame */
        /* construct second half window using padding with 1's and 0's */
        for (i = 0; i < nflat_ls; i++)
            overlap[i] = transf_buf[nlong+i];
        for (i = 0; i < nshort; i++)
            overlap[nflat_ls+i] = MUL_F(transf_buf[nlong+nflat_ls+i],window_short[nshort-i-1]);
        for (i = 0; i < nflat_ls; i++)
            overlap[nflat_ls+nshort+i] = 0;

        break;

    case EIGHT_SHORT_SEQUENCE:
        idxfreq = freq_in;
        idxtran = transf_buf;

        /* perform iMDCT for each short block */
        faad_imdct(fb->mdct256, idxfreq,idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);
        idxfreq += nshort;
        idxtran += (nshort << 1);
        faad_imdct(fb->mdct256, idxfreq, idxtran);

        /* add second half output of previous frame to windowed output of current frame */
        for (i = 0; i < nflat_ls; i++)
            time_out[i] = overlap[i];
        tmpidx = nflat_ls - nshort;
        for(i = 0; i < nshort; i++)
        {
        pover = overlap + tmpidx;
        ptime = time_out + tmpidx++;
            tranidx = i;
            winidx1 = window_short[i];
            winidx2 = window_short[nshort-1-i];
            S32MUL(xr1, xr2, transf_buf[tranidx],window_short_prev[i]);
            tranidx += nshort;
            S32MUL(xr3, xr4, transf_buf[tranidx],winidx2);
            tranidx += nshort;
            S32MADD(xr3, xr4, transf_buf[tranidx],winidx1);
            tranidx += nshort;
            S32LDIV(xr5, pover,nshort,2); 
            S32LDIV(xr6, pover,nshort,2); 
            D32SLL(xr1, xr1, xr3, xr3,1);
            D32ASUM_AA(xr1,xr5,xr6,xr3);
            S32SDIV(xr1, ptime, nshort, 2);
            S32SDIV(xr3, ptime, nshort, 2);
            S32MUL(xr7, xr8, transf_buf[tranidx],winidx2);
            tranidx += nshort;
            S32MADD(xr7, xr8, transf_buf[tranidx],winidx1);
            tranidx += nshort;
            S32MUL(xr9, xr10, transf_buf[tranidx],winidx2);
            tranidx += nshort;
            S32MADD(xr9, xr10, transf_buf[tranidx],winidx1);
            S32LDIV(xr11, pover,nshort,2); 
            S32LDIV(xr12, pover,nshort,2); 
            D32SLL(xr7, xr7, xr9, xr9,1);
            D32ASUM_AA(xr7, xr11, xr12, xr9);
            S32SDIV(xr7, ptime, nshort, 2);
            S32SDIV(xr9, ptime, nshort, 2);
            if (i < trans)
            {
            tranidx += nshort;
               S32MUL(xr1, xr2, transf_buf[tranidx],winidx2);
            tranidx += nshort;
               S32MADD(xr1, xr2, transf_buf[tranidx],winidx1);
               S32LDIV(xr3, pover,nshort,2); 
               D32SLL(xr1, xr1, xr0,xr0,1);
               D32ASUM_AA(xr1,xr3,xr0,xr0);
               S32SDIV(xr1, ptime, nshort, 2);
            }
        }

        /* window the second half and save as overlap for next frame */
        tmpidx = nflat_ls+(nshort<<2)-nlong;
        for(i = 0; i < nshort; i++)
        {
            pover = overlap + tmpidx++;
            tranidx = 9 * nshort + i;
            winidx1 = window_short[i];
            winidx2 = window_short[nshort - 1 -i];
            if (i >= trans)
            {
               pover -= nshort;
               tranidx -= (nshort << 1);
               S32MUL(xr1,xr2,transf_buf[tranidx],winidx2);
               tranidx += nshort;
               S32MADD(xr1,xr2,transf_buf[tranidx],winidx1);
               tranidx += nshort;
               D32SLL(xr1,xr1,xr0,xr0,1);
               S32SDIV(xr1, pover,nshort,2);
            } 
            S32MUL(xr1, xr2, transf_buf[tranidx],winidx2);
               tranidx += nshort;
            S32MADD(xr1, xr2, transf_buf[tranidx],winidx1);   
               tranidx += nshort;
            S32MUL(xr3, xr4, transf_buf[tranidx],winidx2);
               tranidx += nshort;
            S32MADD(xr3, xr4, transf_buf[tranidx],winidx1);
               tranidx += nshort;
            D32SLL(xr1,xr1,xr3,xr3,1);
            S32SDIV(xr1, pover, nshort,2);
            S32SDIV(xr3, pover, nshort,2);
            S32MUL(xr5, xr6, transf_buf[tranidx],winidx2);
               tranidx += nshort;
            S32MADD(xr5,xr6, transf_buf[tranidx],winidx1);
               tranidx += nshort;
            S32MUL(xr7, xr8, transf_buf[tranidx],winidx2);
            D32SLL(xr5, xr5,xr7, xr7,1);
            S32SDIV(xr5, pover, nshort,2);
            S32SDIV(xr7, pover, nshort,2);
        }
        for (i = 0; i < nflat_ls; i++)
            overlap[nflat_ls+nshort+i] = 0;
        
        break;

    case LONG_STOP_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        /* construct first half window using padding with 1's and 0's */
        for (i = 0; i < nflat_ls; i++)
            time_out[i] = overlap[i];
        for (i = 0; i < nshort; i++)
            time_out[nflat_ls+i] = overlap[nflat_ls+i] + MUL_F(transf_buf[nflat_ls+i],window_short_prev[i]);
        for (i = 0; i < nflat_ls; i++)
            time_out[nflat_ls+nshort+i] = overlap[nflat_ls+nshort+i] + transf_buf[nflat_ls+nshort+i];

        /* window the second half and save as overlap for next frame */
        for (i = 0; i < nlong; i++)
            overlap[i] = MUL_F(transf_buf[nlong+i],window_long[nlong-1-i]);
		break;
    }



}
#else //JZ4750_OPT
void ifilter_bank(fb_info *fb, uint8_t window_sequence, uint8_t window_shape,
                  uint8_t window_shape_prev, real_t *freq_in,
                  real_t *time_out, real_t *overlap,
                  uint8_t object_type, uint16_t frame_len)
{
    int16_t i;
    ALIGN real_t transf_buf[2*1024] = {0};

    const real_t *window_long = NULL;
    const real_t *window_long_prev = NULL;
    const real_t *window_short = NULL;
    const real_t *window_short_prev = NULL;

    uint16_t nlong = frame_len;
    uint16_t nshort = frame_len/8;
    uint16_t trans = nshort/2;

    uint16_t nflat_ls = (nlong-nshort)/2;

#ifdef PROFILE
    int64_t count = faad_get_ts();
#endif

    /* select windows of current frame and previous frame (Sine or KBD) */
#ifdef LD_DEC
    if (object_type == LD)
    {
        window_long       = fb->ld_window[window_shape];
        window_long_prev  = fb->ld_window[window_shape_prev];
    } else {
#endif
        window_long       = fb->long_window[window_shape];
        window_long_prev  = fb->long_window[window_shape_prev];
        window_short      = fb->short_window[window_shape];
        window_short_prev = fb->short_window[window_shape_prev];
#ifdef LD_DEC
    }
#endif

#if 0
    for (i = 0; i < 1024; i++)
    {
        printf("%d\n", freq_in[i]);
    }
#endif

#if 0
    printf("%d %d\n", window_sequence, window_shape);
#endif

    switch (window_sequence)
    {
    case ONLY_LONG_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        for (i = 0; i < nlong; i+=4)
        {
            time_out[i]   = overlap[i]   + MUL_F(transf_buf[i],window_long_prev[i]);
            time_out[i+1] = overlap[i+1] + MUL_F(transf_buf[i+1],window_long_prev[i+1]);
            time_out[i+2] = overlap[i+2] + MUL_F(transf_buf[i+2],window_long_prev[i+2]);
            time_out[i+3] = overlap[i+3] + MUL_F(transf_buf[i+3],window_long_prev[i+3]);
        }

        /* window the second half and save as overlap for next frame */
        for (i = 0; i < nlong; i+=4)
        {
            overlap[i]   = MUL_F(transf_buf[nlong+i],window_long[nlong-1-i]);
            overlap[i+1] = MUL_F(transf_buf[nlong+i+1],window_long[nlong-2-i]);
            overlap[i+2] = MUL_F(transf_buf[nlong+i+2],window_long[nlong-3-i]);
            overlap[i+3] = MUL_F(transf_buf[nlong+i+3],window_long[nlong-4-i]);
        }
        break;

    case LONG_START_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        for (i = 0; i < nlong; i+=4)
        {
            time_out[i]   = overlap[i]   + MUL_F(transf_buf[i],window_long_prev[i]);
            time_out[i+1] = overlap[i+1] + MUL_F(transf_buf[i+1],window_long_prev[i+1]);
            time_out[i+2] = overlap[i+2] + MUL_F(transf_buf[i+2],window_long_prev[i+2]);
            time_out[i+3] = overlap[i+3] + MUL_F(transf_buf[i+3],window_long_prev[i+3]);
        }

        /* window the second half and save as overlap for next frame */
        /* construct second half window using padding with 1's and 0's */
        for (i = 0; i < nflat_ls; i++)
            overlap[i] = transf_buf[nlong+i];
        for (i = 0; i < nshort; i++)
            overlap[nflat_ls+i] = MUL_F(transf_buf[nlong+nflat_ls+i],window_short[nshort-i-1]);
        for (i = 0; i < nflat_ls; i++)
            overlap[nflat_ls+nshort+i] = 0;

        break;

    case EIGHT_SHORT_SEQUENCE:
        /* perform iMDCT for each short block */
        faad_imdct(fb->mdct256, freq_in+0*nshort, transf_buf+2*nshort*0);
        faad_imdct(fb->mdct256, freq_in+1*nshort, transf_buf+2*nshort*1);
        faad_imdct(fb->mdct256, freq_in+2*nshort, transf_buf+2*nshort*2);
        faad_imdct(fb->mdct256, freq_in+3*nshort, transf_buf+2*nshort*3);
        faad_imdct(fb->mdct256, freq_in+4*nshort, transf_buf+2*nshort*4);
        faad_imdct(fb->mdct256, freq_in+5*nshort, transf_buf+2*nshort*5);
        faad_imdct(fb->mdct256, freq_in+6*nshort, transf_buf+2*nshort*6);
        faad_imdct(fb->mdct256, freq_in+7*nshort, transf_buf+2*nshort*7);

        /* add second half output of previous frame to windowed output of current frame */
        for (i = 0; i < nflat_ls; i++)
            time_out[i] = overlap[i];
        for(i = 0; i < nshort; i++)
        {
            time_out[nflat_ls+         i] = overlap[nflat_ls+         i] + MUL_F(transf_buf[nshort*0+i],window_short_prev[i]);
            time_out[nflat_ls+1*nshort+i] = overlap[nflat_ls+nshort*1+i] + MUL_F(transf_buf[nshort*1+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*2+i],window_short[i]);
            time_out[nflat_ls+2*nshort+i] = overlap[nflat_ls+nshort*2+i] + MUL_F(transf_buf[nshort*3+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*4+i],window_short[i]);
            time_out[nflat_ls+3*nshort+i] = overlap[nflat_ls+nshort*3+i] + MUL_F(transf_buf[nshort*5+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*6+i],window_short[i]);
            if (i < trans)
                time_out[nflat_ls+4*nshort+i] = overlap[nflat_ls+nshort*4+i] + MUL_F(transf_buf[nshort*7+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*8+i],window_short[i]);
        }

        /* window the second half and save as overlap for next frame */
        for(i = 0; i < nshort; i++)
        {
            if (i >= trans)
                overlap[nflat_ls+4*nshort+i-nlong] = MUL_F(transf_buf[nshort*7+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*8+i],window_short[i]);
            overlap[nflat_ls+5*nshort+i-nlong] = MUL_F(transf_buf[nshort*9+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*10+i],window_short[i]);
            overlap[nflat_ls+6*nshort+i-nlong] = MUL_F(transf_buf[nshort*11+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*12+i],window_short[i]);
            overlap[nflat_ls+7*nshort+i-nlong] = MUL_F(transf_buf[nshort*13+i],window_short[nshort-1-i]) + MUL_F(transf_buf[nshort*14+i],window_short[i]);
            overlap[nflat_ls+8*nshort+i-nlong] = MUL_F(transf_buf[nshort*15+i],window_short[nshort-1-i]);
        }
        for (i = 0; i < nflat_ls; i++)
            overlap[nflat_ls+nshort+i] = 0;
        
        break;

    case LONG_STOP_SEQUENCE:
        /* perform iMDCT */
        imdct_long(fb, freq_in, transf_buf, 2*nlong);

        /* add second half output of previous frame to windowed output of current frame */
        /* construct first half window using padding with 1's and 0's */
        for (i = 0; i < nflat_ls; i++)
            time_out[i] = overlap[i];
        for (i = 0; i < nshort; i++)
            time_out[nflat_ls+i] = overlap[nflat_ls+i] + MUL_F(transf_buf[nflat_ls+i],window_short_prev[i]);
        for (i = 0; i < nflat_ls; i++)
            time_out[nflat_ls+nshort+i] = overlap[nflat_ls+nshort+i] + transf_buf[nflat_ls+nshort+i];

        /* window the second half and save as overlap for next frame */
        for (i = 0; i < nlong; i++)
            overlap[i] = MUL_F(transf_buf[nlong+i],window_long[nlong-1-i]);
		break;
    }

#if 0
    for (i = 0; i < 1024; i++)
    {
        printf("%d\n", time_out[i]);
        //printf("0x%.8X\n", time_out[i]);
    }
#endif


#ifdef PROFILE
    count = faad_get_ts() - count;
    fb->cycles += count;
#endif
}
#endif //JZ4750_OPT


#ifdef LTP_DEC
/* only works for LTP -> no overlapping, no short blocks */
void filter_bank_ltp(fb_info *fb, uint8_t window_sequence, uint8_t window_shape,
                     uint8_t window_shape_prev, real_t *in_data, real_t *out_mdct,
                     uint8_t object_type, uint16_t frame_len)
{
    int16_t i;
    ALIGN real_t windowed_buf[2*1024] = {0};

    const real_t *window_long = NULL;
    const real_t *window_long_prev = NULL;
    const real_t *window_short = NULL;
    const real_t *window_short_prev = NULL;

    uint16_t nlong = frame_len;
    uint16_t nshort = frame_len/8;
    uint16_t nflat_ls = (nlong-nshort)/2;

    assert(window_sequence != EIGHT_SHORT_SEQUENCE);

#ifdef LD_DEC
    if (object_type == LD)
    {
        window_long       = fb->ld_window[window_shape];
        window_long_prev  = fb->ld_window[window_shape_prev];
    } else {
#endif
        window_long       = fb->long_window[window_shape];
        window_long_prev  = fb->long_window[window_shape_prev];
        window_short      = fb->short_window[window_shape];
        window_short_prev = fb->short_window[window_shape_prev];
#ifdef LD_DEC
    }
#endif

    switch(window_sequence)
    {
    case ONLY_LONG_SEQUENCE:
        for (i = nlong-1; i >= 0; i--)
        {
            windowed_buf[i] = MUL_F(in_data[i], window_long_prev[i]);
            windowed_buf[i+nlong] = MUL_F(in_data[i+nlong], window_long[nlong-1-i]);
        }
        mdct(fb, windowed_buf, out_mdct, 2*nlong);
        break;

    case LONG_START_SEQUENCE:
        for (i = 0; i < nlong; i++)
            windowed_buf[i] = MUL_F(in_data[i], window_long_prev[i]);
        for (i = 0; i < nflat_ls; i++)
            windowed_buf[i+nlong] = in_data[i+nlong];
        for (i = 0; i < nshort; i++)
            windowed_buf[i+nlong+nflat_ls] = MUL_F(in_data[i+nlong+nflat_ls], window_short[nshort-1-i]);
        for (i = 0; i < nflat_ls; i++)
            windowed_buf[i+nlong+nflat_ls+nshort] = 0;
        mdct(fb, windowed_buf, out_mdct, 2*nlong);
        break;

    case LONG_STOP_SEQUENCE:
        for (i = 0; i < nflat_ls; i++)
            windowed_buf[i] = 0;
        for (i = 0; i < nshort; i++)
            windowed_buf[i+nflat_ls] = MUL_F(in_data[i+nflat_ls], window_short_prev[i]);
        for (i = 0; i < nflat_ls; i++)
            windowed_buf[i+nflat_ls+nshort] = in_data[i+nflat_ls+nshort];
        for (i = 0; i < nlong; i++)
            windowed_buf[i+nlong] = MUL_F(in_data[i+nlong], window_long[nlong-1-i]);
        mdct(fb, windowed_buf, out_mdct, 2*nlong);
        break;
    }
}
#endif
