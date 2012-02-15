/********************************************************
 *
 *         Copyright (C), 2007  SEIKO EPSON Corp.
 *         ALL RIGHTS RESERVED
 *
 *      file name : stddef.h
 *
 *  This is include file for the basic definition.
 *
 *      Revision history
 *          2007/8/30  1.st release      K.Watanabe
 *
 ********************************************************/

#ifndef STDDEF_H
#define STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __STRICT_ANSI__
#   define TYPEOF __typeof__
#else
#   define TYPEOF typeof
#endif

#ifdef __cplusplus
    typedef int ptrdiff_t;
#else
#ifdef __STDC__
    typedef TYPEOF((void *)1-(void *)0) ptrdiff_t;
#else
    typedef int ptrdiff_t;
#endif
#endif /* __cplusplus */


#ifndef _SIZE_T
#define _SIZE_T
#ifdef __STDC__
    typedef TYPEOF(sizeof(0)) size_t;
#else
    typedef unsigned size_t;
#endif
#endif

#ifndef NULL
#ifdef __cplusplus                     
#define NULL         0
#else
#define NULL         ((void *)0)
#endif
#endif

#define offsetof(type,id) ((size_t)&(((type *)0)->id))


#ifndef _WCHART
#define _WCHART
typedef unsigned short  _Wchart;
#ifndef __cplusplus
typedef _Wchart wchar_t;
#endif
#endif
#ifdef __cplusplus
}
#endif
#endif
