    /*********************************************************/
    /*                                                       */
    /*                                                       */
    /*         Copyright (C), 1993  SEIKOEPSON Corp.         */
    /*         ALL RIGHTS RESERVED                           */
    /*                                                       */
    /*                                                       */
    /*  file name : stdarg.h                                 */
    /*                                                       */
    /*  Revision history                                     */
    /*      93/11/30    first release       K.Shinozuka      */
    /*      03/02/25    modify va_start     H.Watanabe       */
    /*      03/06/18    add include smcvals.h  T.Tazaki      */
    /*                                                       */
    /*    stdarg.h contains followings.                      */
    /*        a declaration of a type                        */
    /*        three definitions of a macro                   */
    /*            for a variable number of arguments         */
    /*            of varying types                           */
    /*                                                       */
    /*********************************************************/

#include <smcvals.h>        /* add T.Tazaki 2003/06/18 */

#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;

#define va_end( tdArgP )    (void) 0
#define _SIZEOF( type )  ((( sizeof( type ) + _BOUNDARY - 1 ) / _BOUNDARY ) * _BOUNDARY )


#ifdef  BIG_ENDIAN
#   define _ENDSIZE( type ) sizeof( type )
#else
#   define _ENDSIZE( type ) ((( sizeof( type ) + _BOUNDARY - 1 ) / _BOUNDARY ) * _BOUNDARY )
#endif


#if !defined(UNIX) || defined(ANSI)

#   define va_start( tdArgP, lastparm )   \
          (tdArgP = ( (va_list) __builtin_next_arg (lastparm)))
                        /* initialization of tdArgP */
#   define va_arg( tdArgP, type )    \
          *((type *)( (tdArgP =  tdArgP + _SIZEOF( type )) - _ENDSIZE( type ) ))
                                                /* return next argument and make tdArgP move forward */

#else

#   define va_alist __builtin_va_alist
#   define va_dcl int va_alist;
#   define va_start( tdArgP ) tdArgP = (char *) &va_alist
#   define va_arg( tdArgP, type )    \
          *((type *)(( tdArgP = tdArgP + _SIZEOF( type ) ) - _ENDSIZE( type ) ))
                                                /* return next argument and make tdArgP move forward */

#endif   /* end UNIX & ANSI */

#endif   /* end _STDARG_H */
