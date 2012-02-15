    /********************************************************************/
    /*                                                                  */
    /*  math.h                                                          */
    /*       version 0        1992/08/31        K.Shinozuka             */
    /*       modified         1997/03/17        T.Katahira              */
    /*       change function declaration to the ANSI style              */
    /*       modified         2005/03/18        K.Watanabe              */
    /*       change prototype declaration for C++                       */
    /*                                                                  */
    /*       prototypes changed to revoke extern                        */
    /*                        2008/5/26         K.Watanabe              */
    /*                                                                  */
    /*  math.h contains followings.                                     */
    /*    extern declarations of math-functions                         */
    /*    extern declarations of special codes and useful numbers       */
    /*                                                                  */
    /********************************************************************/

#ifndef MATH_H 
#define MATH_H

#ifdef UNIX
#include "errno.h"
#else
#include <errno.h>
#endif

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe V1.7 <<<<<<<< */

// CHG K.Watanabe 2008/5/26 >>>>>>>
#if 0
extern double acos( double );
extern double asin( double );
extern double atan( double );
extern double atan2( double, double );
extern double ceil( double );
extern double cos( double );
extern double cosh( double );
extern double exp( double );
extern double fabs( double );
extern double floor( double );
extern double fmod( double, double );
extern double frexp( double, int * );
extern double ldexp( double, int );
extern double log( double );
extern double log10( double );
extern double modf( double, double * );
extern double pow( double, double );
extern double sin( double );
extern double sinh( double );
extern double sqrt( double );
extern double tan( double );
extern double tanh( double );
#endif


double acos( double );
double asin( double );
double atan( double );
double atan2( double, double );
double ceil( double );
double cos( double );
double cosh( double );
double exp( double );
double fabs( double );
double floor( double );
double fmod( double, double );
double frexp( double, int * );
double ldexp( double, int );
double log( double );
double log10( double );
double modf( double, double * );
double pow( double, double );
double sin( double );
double sinh( double );
double sqrt( double );
double tan( double );
double tanh( double );
// CHG K.Watanabe 2008/5/26 <<<<<<<

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#ifndef DBL_LNG_TYPE 

#define DBL_LNG_TYPE 

typedef union {
    struct {
#ifdef BIG_ENDIAN
        unsigned long _LL;
        unsigned long _LH;
#else  
        unsigned long _LH;
        unsigned long _LL;
#endif 
    } st;
    double _D;
} DBL_LNG;

#endif

#endif
