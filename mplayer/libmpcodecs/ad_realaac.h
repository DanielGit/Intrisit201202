

#ifndef _AACDEC_H
#define _AACDEC_H

#if defined(_WIN32) && !defined(_WIN32_WCE)
#
#elif defined(_WIN32) && defined(_WIN32_WCE) && defined(ARM)
#
#elif defined(_WIN32) && defined(WINCE_EMULATOR)
#
#elif defined (__arm) && defined (__ARMCC_VERSION)
#
#elif defined(_SYMBIAN) && defined(__WINS__)
#
#elif defined(__GNUC__) && defined(__arm__)
#
#elif defined(__GNUC__) && defined(__mips__)
#
#elif defined(__GNUC__) && defined(__i386__)
#
#elif defined(_OPENWAVE_SIMULATOR) || defined(_OPENWAVE_ARMULATOR)
#
#else
#error No platform defined. See valid options in aacdec.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3)
 * max size of input buffer = 
 *    6144 bits =  768 bytes per SCE or CCE-I
 *   12288 bits = 1536 bytes per CPE
 *       0 bits =    0 bytes per CCE-D (uses bits from the SCE/CPE/CCE-I it is coupled to)
 */
#ifndef AAC_MAX_NCHANS				/* if max channels isn't set in makefile, */
#define AAC_MAX_NCHANS		2		/* set to default max number of channels  */
#endif
#define AAC_MAX_NSAMPS		1024
#define AAC_MAINBUF_SIZE	(768 * AAC_MAX_NCHANS)

#define AAC_NUM_PROFILES	3
#define AAC_PROFILE_MP		0
#define AAC_PROFILE_LC		1
#define AAC_PROFILE_SSR		2

/* define these to enable decoder features */
#define AAC_ENABLE_SBR
#define AAC_ENABLE_MPEG4

enum {
	ERR_AAC_NONE                          =   0,
	ERR_AAC_INDATA_UNDERFLOW              =  -1,
	ERR_AAC_NULL_POINTER                  =  -2,
	ERR_AAC_INVALID_ADTS_HEADER           =  -3,
	ERR_AAC_INVALID_ADIF_HEADER           =  -4,
	ERR_AAC_INVALID_FRAME                 =  -5,
	ERR_AAC_MPEG4_UNSUPPORTED             =  -6,
	ERR_AAC_CHANNEL_MAP                   =  -7,
	ERR_AAC_SYNTAX_ELEMENT                =  -8,

	ERR_AAC_DEQUANT                       =  -9,
	ERR_AAC_STEREO_PROCESS                = -10,
	ERR_AAC_PNS                           = -11,
	ERR_AAC_SHORT_BLOCK_DEINT             = -12,
	ERR_AAC_TNS                           = -13,
	ERR_AAC_IMDCT                         = -14,
	ERR_AAC_NCHANS_TOO_HIGH               = -15,

	ERR_AAC_SBR_INIT                      = -16,
	ERR_AAC_SBR_BITSTREAM                 = -17,
	ERR_AAC_SBR_DATA                      = -18,
	ERR_AAC_SBR_PCM_FORMAT                = -19,
	ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,
	ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,

	ERR_AAC_RAWBLOCK_PARAMS               = -22,

	ERR_UNKNOWN               = -9999
};

typedef struct _AACFrameInfo {
	int bitRate;
	int nChans;
	int sampRateCore;
	int sampRateOut;
	int bitsPerSample;
	int outputSamps;
	int profile;
	int tnsUsed;
	int pnsUsed;
} AACFrameInfo;

typedef void *HAACDecoder;

/* public C API */
HAACDecoder AACInitDecoder(void);
void AACFreeDecoder(HAACDecoder hAACDecoder);
int AACDecode(HAACDecoder hAACDecoder, unsigned char **inbuf, int *bytesLeft, short *outbuf);

int AACFindSyncWord(unsigned char *buf, int nBytes);
void AACGetLastFrameInfo(HAACDecoder hAACDecoder, AACFrameInfo *aacFrameInfo);
int AACSetRawBlockParams(HAACDecoder hAACDecoder, int copyLast, AACFrameInfo *aacFrameInfo);
int AACFlushCodec(HAACDecoder hAACDecoder);

#ifdef __cplusplus
}
#endif

#endif	/* _AACDEC_H */
#ifndef GA_CONFIG_H
#define GA_CONFIG_H

#define UINT32 uint32_t
#define HXBOOL int32_t
#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

typedef struct ga_config_data_struct
{
    UINT32 audioObjectType;
    UINT32 samplingFrequency;
    UINT32 extensionSamplingFrequency;
    UINT32 frameLength;
    UINT32 coreCoderDelay;
    UINT32 numChannels;
    UINT32 numFrontChannels;
    UINT32 numSideChannels;
    UINT32 numBackChannels;
    UINT32 numFrontElements;
    UINT32 numSideElements;
    UINT32 numBackElements;
    UINT32 numLfeElements;
    UINT32 numAssocElements;
    UINT32 numValidCCElements;
    HXBOOL bSBR;
} ga_config_data;

enum AudioObjectType
{
    AACMAIN = 1,
    AACLC   = 2,
    AACSSR  = 3,
    AACLTP  = 4,
    AACSBR  = 5,
    AACSCALABLE = 6,
    TWINVQ  = 7
};
#define READBUF_SIZE    (2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS) 
#define SBR_MUL         2

struct BITSTREAM
{
    const unsigned char *buffer ; /**< points to the buffer holding the bits */
    const unsigned char *pkptr ;  /**< read pointer */
    unsigned long cache ;   /**< cache, always holds next bits left-aligned. */
    int cacheBitsLeft ;     /**< number of bits left in cache */
    int nBits ;    /**< the number of bits in the buffer */
    int inc ;      /**< read direction (forward/backward) */
} ;


UINT32 ga_config_get_data(struct BITSTREAM *bs, ga_config_data *data);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef GA_CONFIG_H */
