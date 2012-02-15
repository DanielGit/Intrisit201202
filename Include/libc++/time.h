/********************************************************
 *
 *         Copyright (C), 1994  SEIKO EPSON Corp.
 *         ALL RIGHTS RESERVED
 *
 *      file name : time.h
 *
 *  This is include file for time functions.
 *
 *      Revision history
 *          93/12/17  1.st release      S.Akaiwa
 *              94/02/07  modify for 88     M.Kudo
 *      97/03/25  modify for 33     M.Kudo
 *      05/03/18  modify            K.Watanabe
 *                change prototype definition for C++
 *      2008/5/26  prototypes changed to revoke extern
 *                 and comments of parameters   K.Watanabe
 *
 ********************************************************/

#ifndef _TIME_H
#define _TIME_H

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;     /* size of type */
#endif
#endif


#ifdef __STRICT_ANSI__
    /* "compiled with -ansi" switch */
#   define TYPEOF __typeof__
#else
#   define TYPEOF typeof
#endif

#ifndef _SIZE_T
#define _SIZE_T
#ifdef __STDC__
    typedef TYPEOF(sizeof(0)) size_t;
#else
    /* compiled with "-traditional" switch */
    typedef unsigned size_t;
#endif
#endif
/* CHG K.Watanabe V1.7 <<<<<<< */


#ifndef __clock_t_defined           /* ADD K.Watanabe  V1.7 */
typedef long            clock_t;    /* clock type */
#define __clock_t_defined           /* ADD K.Watanabe  V1.7 */
#endif                              /* ADD K.Watanabe  V1.7 */

typedef long            time_t;     /* time type */

#if !defined(NULL)
/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
#define NULL         0
#else
/* ADD K.Watanabe V1.7 <<<<<<< */
#  define NULL          ((void *)0L)/* null pointer */
#endif /* __cplusplus */            /* ADD K.Watanabe V1.7 */
#endif

struct  tm {                        /* time structure */
    int tm_sec;                     /* seconds (0-59) */
    int tm_min;                     /* minutes (0-59) */
    int tm_hour;                    /* hours (0-23) */
    int tm_mday;                    /* days (1-31) */
    int tm_mon;                     /* months (0-11) */
    int tm_year;                    /* year -1900 */
    int tm_wday;                    /* day of week (sun = 0) */
    int tm_yday;                    /* day of year (0 - 365) */
    int tm_isdst;                   /* non-zero if DST */
};

#define CLOCKS_PER_SEC  1000000     /* 1000000 cycles is 1 sec */

/* prototype definition */

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */

time_t      time(  time_t *  );
time_t      mktime(  struct tm *  );

/* CHG K.Watanabe 2008/5/26 >>>>>>> */
#if 0
struct tm   *gmtime(  time_t *  );
#endif

struct tm   *gmtime(  const time_t *  );
/* CHG K.Watanabe 2008/5/26 <<<<<<< */

double      difftime( time_t, time_t );
clock_t     clock( void );
struct tm   *localtime( const time_t * );
char        *asctime( const struct tm * );
char        *ctime( const time_t * );

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */

#endif  /* _TIME_H */
