/*
 * COOK compatible decoder
 * Copyright (c) 2003 Sascha Sommer
 * Copyright (c) 2005 Benjamin Larsson
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
 * @file cook.c
 * Cook compatible decoder. Bastardization of the G.722.1 standard.
 * This decoder handles RealNetworks, RealAudio G2 data.
 * Cook is identified by the codec name cook in RM files.
 *
 * To use this decoder, a calling application must supply the extradata
 * bytes provided from the RM container; 8+ bytes for mono streams and
 * 16+ for stereo streams (maybe more).
 *
 * Codec technicalities (all this assume a buffer length of 1024):
 * Cook works with several different techniques to achieve its compression.
 * In the timedomain the buffer is divided into 8 pieces and quantized. If
 * two neighboring pieces have different quantization index a smooth
 * quantization curve is used to get a smooth overlap between the different
 * pieces.
 * To get to the transformdomain Cook uses a modulated lapped transform.
 * The transform domain has 50 subbands with 20 elements each. This
 * means only a maximum of 50*20=1000 coefficients are used out of the 1024
 * available.
 */

#ifdef __LINUX__
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#define F(...)
#else
#include <mplaylib.h>
#endif

#include "avcodec.h"
#include "bitstream.h"
#include "dsputil.h"
#include "bytestream.h"
/* ra8lbr opaque data struct */
typedef struct ra8lbr_data_struct
{
    unsigned int   version;
    unsigned short   nSamples;
    unsigned short   nRegions;
    int   delay;
    unsigned short  cplStart ;
    unsigned short  cplQBits ;
    int   channelMask ;
} ra8lbr_data;
typedef void *HGecko2Decoder;
ra8lbr_data unpackedData;
HGecko2Decoder gecko2Decoder = NULL;
typedef int HX_RESULT;
#define MAKE_HX_FACILITY_RESULT(sev,fac,code) \
    ((HX_RESULT) (((unsigned int)(sev) << 31) | ((unsigned int)(fac)<<16) | ((unsigned int)(code))))

#define HXR_FAIL MAKE_HX_FACILITY_RESULT(1,0,0x4005)

/**
 * Cook uninit
 */

static int cook_decode_close(AVCodecContext *avctx)
{
    if(gecko2Decoder)
    {	
    	Gecko2FreeDecoder(gecko2Decoder);
    	gecko2Decoder = NULL;
    }
    return 0;
}



/**
 * Cook frame decoding
 *
 * @param avctx     pointer to the AVCodecContext
 */

static int cook_decode_frame(AVCodecContext *avctx,
            void *data, int *data_size,
            uint8_t *buf, int buf_size) {
    int lostFlag = 0;
    int retVal;
    if (buf_size < avctx->block_align)
        return buf_size;
		 
		 retVal = Gecko2Decode(gecko2Decoder, buf, lostFlag, data);
     
 		 if (retVal != 0)
     {
           *data_size = 0;
           retVal = HXR_FAIL;
     }

		*data_size = unpackedData.nSamples * 2;	
    /* Discard the first two frames: no valid audio. */
    if (avctx->frame_number < 2) *data_size = 0;

    return avctx->block_align;
}
#define GECKO_VERSION               ((1L<<24)|(0L<<16)|(0L<<8)|(3L))
#define GECKO_MC1_VERSION           ((2L<<24)|(0L<<16)|(0L<<8)|(0L))
static int
ra8lbr_unpack_opaque_data(ra8lbr_data* pData,
                          unsigned char*  pBuf,
                          unsigned int ulLength)
{
    int retVal = -1;
    unsigned char* off = pBuf;

    if (pBuf != 0 && ulLength != 0)
    {
        retVal = 0;

        pData->version = ((int)*off++)<<24;
        pData->version |= ((int)*off++)<<16;
        pData->version |= ((int)*off++)<<8;
        pData->version |= ((int)*off++);

        pData->nSamples = *off++<<8;
        pData->nSamples |= *off++;

        pData->nRegions = *off++<<8;
        pData->nRegions |= *off++;

        if (pData->version >= GECKO_VERSION)
        {
            pData->delay = ((int)*off++)<<24;
            pData->delay |= ((int)*off++)<<16;
            pData->delay |= ((int)*off++)<<8;
            pData->delay |= ((int)*off++);

            pData->cplStart = *off++<<8;
            pData->cplStart |= *off++;

            pData->cplQBits = *off++<<8;
            pData->cplQBits |= *off++;
        }
        else
        {
            /* the fixed point ra8lbr decoder supports dual-mono decoding with
               a single decoder instance if cplQBits is set to zero. */
            pData->cplStart = 0;
            pData->cplQBits = 0;
        }

        if (pData->version == GECKO_MC1_VERSION)
        {
            //retVal = HXR_UNSUPPORTED_AUDIO;
            /* MultiChannel not supported!
               pData->channelMask = ((INT32)*off++)<<24;
               pData->channelMask |= ((INT32)*off++)<<16;
               pData->channelMask |= ((INT32)*off++)<<8;
               pData->channelMask |= ((INT32)*off++);
            */
        }
    }

    return retVal;
}


/**
 * Cook initialization
 *
 * @param avctx     pointer to the AVCodecContext
 */

static int cook_decode_init(AVCodecContext *avctx)
{
    uint8_t *edata_ptr = avctx->extradata;
		int codingDelay;
		HX_RESULT retVal = HXR_FAIL;
		if (avctx->extradata_size <= 0) {
        av_log(avctx,AV_LOG_ERROR,"Necessary extradata missing!\n");
        return -1;
    }
    memset(&unpackedData,0,sizeof(unpackedData));
		retVal = ra8lbr_unpack_opaque_data(&unpackedData,edata_ptr,avctx->extradata_size);
		if(retVal == -1)
		{
				av_log(avctx,AV_LOG_ERROR,"Necessary extradata error!\n");
				return -1;
		}
		F("unpackedData.nSamples = %d\n",unpackedData.nSamples);
		F("avctx->channels = %d\n",avctx->channels);
		F("avctx->block_align = %d\n",avctx->block_align);
		F("avctx->sample_rate = %d\n",avctx->sample_rate);
		F("avctx->bit_rate = %d\n",avctx->bit_rate);
		
		gecko2Decoder = Gecko2InitDecoder(
		unpackedData.nSamples / avctx->channels, 
		avctx->channels, 
		unpackedData.nRegions, 
		avctx->block_align * 8,
		avctx->sample_rate, 
		unpackedData.cplStart,
		unpackedData.cplQBits,
		&codingDelay);
		F("-------------------------------------------\ncodingDelay = %d\n",codingDelay);
		if (!gecko2Decoder)
		{
				av_log(avctx,AV_LOG_ERROR,"memory overflow!\n");
				return -1 ;
		}
		return 0;
}


AVCodec cook_decoder =
{
    .name = "cook",
    .type = CODEC_TYPE_AUDIO,
    .id = CODEC_ID_COOK,
    .priv_data_size = 0,
    .init = cook_decode_init,
#ifdef __LINUX__
    .close = cook_decode_close,
#else
    .avclose = cook_decode_close,
#endif
    .decode = cook_decode_frame,
};
