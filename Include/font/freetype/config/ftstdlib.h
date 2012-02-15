/***************************************************************************/
/*                                                                         */
/*  ftstdlib.h                                                             */
/*                                                                         */
/*    ANSI-specific library and header configuration file (specification   */
/*    only).                                                               */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2005, 2006, 2007 by                        */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* This file is used to group all #includes to the ANSI C library that   */
  /* FreeType normally requires.  It also defines macros to rename the     */
  /* standard functions within the FreeType source code.                   */
  /*                                                                       */
  /* Load a file which defines __FTSTDLIB_H__ before this one to override  */
  /* it.                                                                   */
  /*                                                                       */
  /*************************************************************************/


#ifndef __FTSTDLIB_H__
#define __FTSTDLIB_H__


#include <libc/stddef.h>
#include <kernel/kernel.h>

#define ft_ptrdiff_t  ptrdiff_t


  /**********************************************************************/
  /*                                                                    */
  /*                           integer limits                           */
  /*                                                                    */
  /* UINT_MAX and ULONG_MAX are used to automatically compute the size  */
  /* of `int' and `long' in bytes at compile-time.  So far, this works  */
  /* for all platforms the library has been tested on.                  */
  /*                                                                    */
  /* Note that on the extremely rare platforms that do not provide      */
  /* integer types that are _exactly_ 16 and 32 bits wide (e.g. some    */
  /* old Crays where `int' is 36 bits), we do not make any guarantee    */
  /* about the correct behaviour of FT2 with all fonts.                 */
  /*                                                                    */
  /* In these case, `ftconfig.h' will refuse to compile anyway with a   */
  /* message like `couldn't find 32-bit type' or something similar.     */
  /*                                                                    */
  /* IMPORTANT NOTE: We do not define aliases for heap management and   */
  /*                 i/o routines (i.e. malloc/free/fopen/fread/...)    */
  /*                 since these functions should all be encapsulated   */
  /*                 by platform-specific implementations of            */
  /*                 `ftsystem.c'.                                      */
  /*                                                                    */
  /**********************************************************************/


#include <libc/limits.h>

#define FT_CHAR_BIT   CHAR_BIT
#define FT_INT_MAX    INT_MAX
#define FT_UINT_MAX   UINT_MAX
#define FT_ULONG_MAX  ULONG_MAX


  /**********************************************************************/
  /*                                                                    */
  /*                 character and string processing                    */
  /*                                                                    */
  /**********************************************************************/


#include <libc/string.h>

#define ft_memchr   kmemchr
#define ft_memcmp   kmemcmp
#define ft_memcpy   kmemcpy
#define ft_memmove  kmemmove
#define ft_memset   kmemset
#define ft_strcat   kstrcat
#define ft_strcmp   kstrcmp
#define ft_strcpy   kstrcpy
#define ft_strlen   kstrlen
#define ft_strncmp  kstrncmp
#define ft_strncpy  kstrncpy
#define ft_strrchr  kstrrchr
#define ft_strstr   kstrstr


  /**********************************************************************/
  /*                                                                    */
  /*                           file handling                            */
  /*                                                                    */
  /**********************************************************************/


#include <libc/stdio.h>

extern FILE* TTFFileOpen(const char *pname, const char *pmode);
extern int TTFFileClose(FILE *file);
extern long TTFFileTell(FILE *file);
extern int TTFFileSeek(FILE *file, int offset, int origin);
extern int TTFFileWrite(void *buffer, unsigned int count, unsigned int size, FILE *file);
extern int TTFFileRead(BYTE *buffer, DWORD count, DWORD size, FILE* file);

#define FT_FILE     FILE
#define ft_fclose   TTFFileClose
#define ft_fopen    TTFFileOpen
#define ft_fread    TTFFileRead
#define ft_fseek    TTFFileSeek
#define ft_ftell    TTFFileTell
#define ft_sprintf  ksprintf


  /**********************************************************************/
  /*                                                                    */
  /*                             sorting                                */
  /*                                                                    */
  /**********************************************************************/


#include <libc/stdlib.h>

#define ft_qsort  kqsort

#define ft_exit   kexit    /* only used to exit from unhandled exceptions */


  /**********************************************************************/
  /*                                                                    */
  /*                        memory allocation                           */
  /*                                                                    */
  /**********************************************************************/


#define ft_scalloc   kcalloc
#define ft_sfree     kfree
#define ft_smalloc   kmalloc
#define ft_srealloc  krealloc


  /**********************************************************************/
  /*                                                                    */
  /*                          miscellaneous                             */
  /*                                                                    */
  /**********************************************************************/


#define ft_atol   katol
#define ft_labs   klabs


  /**********************************************************************/
  /*                                                                    */
  /*                         execution control                          */
  /*                                                                    */
  /**********************************************************************/


#include <libc/setjmp.h>

#define ft_jmp_buf     jmp_buf  /* note: this cannot be a typedef since */
                                /*       jmp_buf is defined as a macro  */
                                /*       on certain platforms           */

#define ft_longjmp     klongjmp
#define ft_setjmp( b ) ksetjmp( *(jmp_buf*) &(b) )    /* same thing here */


  /* the following is only used for debugging purposes, i.e., if */
  /* FT_DEBUG_LEVEL_ERROR or FT_DEBUG_LEVEL_TRACE are defined    */

#include <libc/stdarg.h>


#endif /* __FTSTDLIB_H__ */


/* END */
