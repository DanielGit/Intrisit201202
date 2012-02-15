	/********************************************************************/
	/*                                                                  */
	/*  mathf.h                                                         */
	/*     copied from math.h of math.lib                               */
	/*       version 0          1992/08/31        K.Shinozuka           */
	/*       modified	    1997/03/17	      T.Katahira	            */
	/*       change function declaration to the ANSI style	            */
	/*       modified	    2003/03/10	      Iruma M.Takeishi          */
	/*       changed for float-based library                            */
	/*       modified	    2005/03/18	      K.Watanabe 	            */
	/*       change prototype definition for C++                        */
    /*       prototypes changed  to revoke extern                       */
    /*                           08/5/26        K.Watanabe              */
	/*                                                                  */
	/*  math.h contains followings.                                     */
	/*    extern declarations of math-functions                         */
	/*    extern declarations of special codes and useful numbers       */
	/*                                                                  */
	/********************************************************************/

#ifndef F_MATH_H 
#define F_MATH_H

#ifdef UNIX
#include "errno.h"
#else
#include <errno.h>
#endif

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

/* CHG K.Watanabe 2008/5/26 >>>>>>> */
#if 0
extern float f_acos( float );
extern float f_asin( float );
extern float f_atan( float );
extern float f_atan2( float, float );
extern float f_ceil( float );
extern float f_cos( float );
extern float f_cosh( float );
extern float f_exp( float );
extern float f_fabs( float );
extern float f_floor( float );
extern float f_fmod( float, float );
extern float f_frexp( float, int * );
extern float f_ldexp( float, int );
extern float f_log( float );
extern float f_log10( float );
extern float f_modf( float, float * );
extern float f_pow( float, float );
extern float f_sin( float );
extern float f_sinh( float );
extern float f_sqrt( float );
extern float f_tan( float );
extern float f_tanh( float );
#endif



float f_acos( float );
float f_asin( float );
float f_atan( float );
float f_atan2( float, float );
float f_ceil( float );
float f_cos( float );
float f_cosh( float );
float f_exp( float );
float f_fabs( float );
float f_floor( float );
float f_fmod( float, float );
float f_frexp( float, int * );
float f_ldexp( float, int );
float f_log( float );
float f_log10( float );
float f_modf( float, float * );
float f_pow( float, float );
float f_sin( float );
float f_sinh( float );
float f_sqrt( float );
float f_tan( float );
float f_tanh( float );
/* CHG K.Watanabe 2008/5/26 <<<<<<< */

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#ifndef FLT_LNG_TYPE 

#define FLT_LNG_TYPE 

typedef union {
    struct {
        unsigned long _LL;
    } st;
    float _F;
} FLT_LNG;

#endif

#endif
