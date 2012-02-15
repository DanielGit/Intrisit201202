// 基本三角函数
extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double, double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
// 双曲函数
extern double cosh(double);
extern double sinh(double);
extern double tanh(double);

extern double exp(double);
extern double frexp(double, int *);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double modf(double, double *);

extern double pow(double, double);
extern double sqrt(double);

extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);

extern double erf(double);
extern double erfc(double);
extern double gamma(double);
extern double hypot(double, double);
extern int isinf(double);
extern int isnan(double);
extern int finite(double);
extern double j0(double);
extern double j1(double);
extern double jn(int, double);
extern double lgamma(double);
extern double y0(double);
extern double y1(double);
extern double yn(int, double);

extern double acosh(double);
extern double asinh(double);
extern double atanh(double);
extern double cbrt(double);
extern double logb(double);
extern double nextafter(double, double);
extern double remainder(double, double);
extern double scalb(double, double);

extern int matherr(struct exception *);

/*
 * IEEE Test Vector
 */
extern double significand(double);

/*
 * Functions callable from C, intended to support IEEE arithmetic.
 */
extern double copysign(double, double);
extern int ilogb(double);
extern double rint(double);
extern double scalbn(double, int);

/*
 * BSD math library entry points
 */
extern double drem(double, double);
extern double expm1(double);
extern double log1p(double);

/*
 * Reentrant version of gamma & lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
#ifdef _REENTRANT
extern double gamma_r(double, int *);
extern double lgamma_r(double, int *);
#endif /* _REENTRANT */


/* float versions of ANSI/POSIX functions */
extern float acosf(float);
extern float asinf(float);
extern float atanf(float);
extern float atan2f(float, float);
extern float cosf(float);
extern float sinf(float);
extern float tanf(float);

extern float coshf(float);
extern float sinhf(float);
extern float tanhf(float);

extern float expf(float);
extern float frexpf(float, int *);
extern float ldexpf(float, int);
extern float logf(float);
extern float log10f(float);
extern float modff(float, float *);

extern float powf(float, float);
extern float sqrtf(float);

extern float ceilf(float);
extern float fabsf(float);
extern float floorf(float);
extern float fmodf(float, float);

extern float erff(float);
extern float erfcf(float);
extern float gammaf(float);
extern float hypotf(float, float);
extern int isnanf(float);
extern int finitef(float);
extern float j0f(float);
extern float j1f(float);
extern float jnf(int, float);
extern float lgammaf(float);
extern float y0f(float);
extern float y1f(float);
extern float ynf(int, float);

extern float acoshf(float);
extern float asinhf(float);
extern float atanhf(float);
extern float cbrtf(float);
extern float logbf(float);
extern float nextafterf(float, float);
extern float remainderf(float, float);
extern float scalbf(float, float);

/*
 * float version of IEEE Test Vector
 */
extern float significandf(float);

/*
 * Float versions of functions callable from C, intended to support
 * IEEE arithmetic.
 */
extern float copysignf(float, float);
extern int ilogbf(float);
extern float rintf(float);
extern float scalbnf(float, int);

/*
 * float versions of BSD math library entry points
 */
extern float dremf(float, float);
extern float expm1f(float);
extern float log1pf(float);

/*
 * Float versions of reentrant version of gamma & lgamma; passes
 * signgam back by reference as the second argument; user must
 * allocate space for signgam.
 */
extern float gammaf_r(float, int *);
extern float lgammaf_r(float, int *);
