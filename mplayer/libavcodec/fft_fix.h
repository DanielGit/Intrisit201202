#ifndef FFT_FIXED_H
#define FFT_FIXED_H

#include <math.h>

#ifndef __LINUX__
#include "uclib.h"
#undef memcpy
#define memcpy uc_memcpy
#endif

#define M_PI 3.14159265358979323846

// fixed fft definite
#if 0
typedef double FFTSample;
typedef struct FFTComplex {
    FFTSample re, im;
} FFTComplex;
#endif

typedef int   FFTSample_fix;
typedef struct FFTComplex_fix {
    FFTSample_fix re, im;
} FFTComplex_fix;

struct MDCTContext_fix;

typedef struct FFTContext_fix {
    int nbits;
    int inverse;
    unsigned short *revtab;
    FFTComplex_fix *exptab;
    void (*fft_calc)(struct FFTContext_fix *s, FFTComplex_fix *z);
    void (*imdct_calc)(struct MDCTContext_fix *s, FFTSample_fix *output,
                       const FFTSample_fix *input, FFTSample_fix *tmp);

} FFTContext_fix;
/* butter fly op */
#define FFT_BF_fix(pre, pim, qre, qim, pre1, pim1, qre1, qim1) \
{\
  FFTSample_fix ax, ay, bx, by;\
  bx=pre1;\
  by=pim1;\
  ax=qre1;\
  ay=qim1;\
  pre = (bx + ax);\
  pim = (by + ay);\
  qre = (bx - ax);\
  qim = (by - ay);\
}
#define FFT_FRICTIONS 31
#define FFT_SAMPLE(x)  ((FFTSample_fix)((x)*((1<<FFT_FRICTIONS) -1)))
#if 0
#define FFT_MUL_fix(a, b)  (((long long)(a) * (b) + (1<<(FFT_FRICTIONS - 1))) >> FFT_FRICTIONS) 
#define FFT_CMUL_fix(pre, pim, are, aim, bre, bim) \
{\
    FFTSample_fix _are = (are);\
    FFTSample_fix _aim = (aim);\
    FFTSample_fix _bre = (bre);\
    FFTSample_fix _bim = (bim);\
    (pre) = FFT_MUL_fix(_are, _bre) - FFT_MUL_fix(_aim, _bim);\
    (pim) = FFT_MUL_fix(_are, _bim) + FFT_MUL_fix(_aim, _bre);\
}
#else
#define FFT_MUL_fix(a, b)  \
({                         \
   long hi;                \
   unsigned long lo;            \
   asm __volatile("mult %2,%3"      \
             :"=h"(hi),"=l"(lo)      \
             :"%r"(a), "r"(b));     \
   ((hi<<(32-FFT_FRICTIONS))|(lo >>FFT_FRICTIONS)); \
})
#define FFT_CMUL_fix(pre, pim, are, aim, bre, bim) \
{\
    long hi;                                       \
    unsigned long lo;                              \
    FFTSample_fix _are = (are);\
    FFTSample_fix _aim = (aim);\
    FFTSample_fix _bre = (bre);\
    FFTSample_fix _bim = (bim);\
    asm ("mult %2,%3\n"                            \
         "msub %4,%5\n"                            \
         :"=h"(hi),"=l"(lo)                        \
         :"r"(_are),"r"(_bre),"r"(_aim), "r"(_bim));   \
    (pre) = ((hi<<(32-FFT_FRICTIONS))|(lo >>FFT_FRICTIONS)); \
    asm ("mult %2,%3\n"                            \
         "madd %4,%5\n"                            \
         :"=h"(hi),"=l"(lo)                        \
         :"r"(_are),"r"(_bim),"r"(_aim), "r"(_bre));   \
    (pim) = ((hi<<(32-FFT_FRICTIONS))|(lo >>FFT_FRICTIONS)); \
}
#endif

int  fft_init_fix(FFTContext_fix *s, int nbits, int inverse);
void fft_permute_fix(FFTContext_fix *s, FFTComplex_fix *z);
void fft_calc_fix(FFTContext_fix *s, FFTComplex_fix *z);
void fft_end_fix(FFTContext_fix *s);
#endif //FFT_FIXED_H

