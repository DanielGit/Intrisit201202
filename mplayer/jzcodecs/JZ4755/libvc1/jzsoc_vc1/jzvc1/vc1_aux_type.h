/*****************************************************************************
 *
 * JZC H264 Decoder Type Defines
 *
 ****************************************************************************/

#ifndef __JZC_VC1_AUX_TYPE_H__
#define __JZC_VC1_AUX_TYPE_H__

typedef short DCTELEM;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef int int32_t;
typedef short int16_t;
typedef char int8_t;

#define TRUE 1

#define PROFILE_ADVANCED 3
#define I_TYPE  1 // Intra
#define P_TYPE  2 // Predicted
#define B_TYPE  3

#define MV_PMODE_INTENSITY_COMP 4
#define BMV_TYPE_INTERPOLATED   2
#define BMV_TYPE_BACKWARD       0
#define CODEC_FLAG_GRAY         0x2000

/**
 * clip a signed integer value into the amin-amax range
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static inline int av_clip(int a, int amin, int amax)
{
    if (a < amin)      return amin;
    else if (a > amax) return amax;
    else               return a;
}

/**
 * clip a signed integer value into the 0-255 range
 * @param a value to clip
 * @return clipped value
 */
static inline uint8_t av_clip_uint8(int a)
{
    if (a&(~255)) return (-a)>>31;
    else          return a;
}

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMAX(a,b) ((a) < (b) ? (b) : (a))

/* median of 3 */
static inline int mid_pred(int a, int b, int c)
{
#ifdef HAVE_CMOV
    int i=b;
    asm volatile(
        "cmp    %2, %1 \n\t"
        "cmovg  %1, %0 \n\t"
        "cmovg  %2, %1 \n\t"
        "cmp    %3, %1 \n\t"
        "cmovl  %3, %1 \n\t"
        "cmp    %1, %0 \n\t"
        "cmovg  %1, %0 \n\t"
        :"+&r"(i), "+&r"(a)
        :"r"(b), "r"(c)
    );
    return i;
#elif 0
    int t= (a-b)&((a-b)>>31);
    a-=t;
    b+=t;
    b-= (b-c)&((b-c)>>31);
    b+= (a-b)&((a-b)>>31);

    return b;
#else
    if(a>b){
        if(c>b){
            if(c>a) b=a;
            else    b=c;
        }
    }else{
        if(b>c){
            if(c>a) b=c;
            else    b=a;
        }
    }
    return b;
#endif
}

/** @name Block types for P/B frames */
//@{
enum TransformTypes {
    TT_8X8,
    TT_8X4_BOTTOM,
    TT_8X4_TOP,
    TT_8X4, //Both halves
    TT_4X8_RIGHT,
    TT_4X8_LEFT,
    TT_4X8, //Both halves
    TT_4X4
};
//@}
#define XCHG2(a,b,t)   t=a;a=b;b=t
#define PICT_FRAME         3

#endif //__JZC_VC1_AUX_TYPE_H__
