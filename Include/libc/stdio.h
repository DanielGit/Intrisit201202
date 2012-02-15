/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	8.5 (Berkeley) 4/29/95
 */

#ifndef	_STDIO_H_
#define	_STDIO_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <arch/ansi.h>
#include <sys/null.h>

#ifdef __STRICT_ANSI__
    /* "compiled with -ansi" switch */
#   define TYPEOF __typeof__
#else
#   define TYPEOF typeof
#endif

/* CHG K.Watanabe V1.7 <<<<<<< */

typedef long            fpos_t;         /* file type size */

#if !defined(NULL)
/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus                     
#define NULL         0
#else
/* ADD K.Watanabe V1.7 <<<<<<< */
#  define NULL          ((void *)0L)    /* null pointer */
#endif /* __cplusplus */                /*  ADD K.Watanabe V1.7 */
#endif

#define EOF             (-1)            /* eof flag */

#define BUFSIZ          1024

#define SEEK_SET        0               /* seek from beginning */
#define SEEK_CUR        1               /* seek from here */
#define SEEK_END        2               /* seek from end */

#define _NFILE          3
#define FOPEN_MAX       _NFILE

#define FILENAME_MAX    0

#ifdef  UNIX
#define L_tmpnam        25 
#else
#define L_tmpnam        0
#endif

#define TMP_MAX         0

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
#define _IOFBF          0000            /* fully buffered io */
#define _IOLBF          0000            /* line buffered */
#define _IONBF          0000            /* unbuffered */
#endif

#define _IOFBF          0000            /* fully buffered io */
#define _IOLBF          0001            /* line buffered */
#define _IONBF          0002            /* unbuffered */
/* CHG K.Watanabe V1.7 <<<<<<< */

extern  FILE            _iob[FOPEN_MAX + 1];

extern  FILE            *stdin;         /* standard input stream */
extern  FILE            *stdout;        /* standard output stream */
extern  FILE            *stderr;        /* standard error stream */

#ifdef va_start
#define _VA_LIST va_list
#else
#define _VA_LIST char *
#endif

/* prototype definition */

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

extern  FILE    *tmpfile( void );
extern  char    *tmpnam( char * );
extern  int     remove( const char * );
extern  int     rename( const char *, const char * );
extern  FILE    *fopen( const char *, const char * );
extern  FILE    *freopen( const char *, const char *, FILE * );
extern  int     fclose( FILE * );

extern  void    setbuf( FILE *, char * );
extern  int     setvbuf( FILE *, char *, int, size_t );
extern  int     fflush( FILE * );

extern  void    clearerr( FILE * );
extern  int     feof( FILE * );
extern  int     ferror( FILE * );
extern  void    perror( const char * );

extern  int     fseek( FILE *, long, int );
extern  int     fgetpos( FILE *, fpos_t * );
extern  int     fsetpos(  FILE *, const fpos_t * );
extern  long    ftell( FILE * );
extern  void    rewind( FILE * );

extern  int     getchar( void );
extern  int     fgetc( FILE * );
extern  int     getc( FILE * );

extern  char    *gets( char * );
extern  char    *fgets( char *, int, FILE * );

extern  int     scanf( const char *, ... );
extern  int     fscanf( FILE *, const char *, ... );
extern  int     sscanf( const char *, const char *, ... );

extern  size_t  fread( void *, size_t, size_t, FILE * );

extern  int     putchar( int );
extern  int     fputc( int, FILE * );
extern  int     putc( int, FILE * );

extern  int     puts( const char * );
extern  int     fputs( const char *, FILE * );

extern  int     ungetc( int, FILE * );

extern  int     printf( const char *, ...  );
extern  int     fprintf( FILE *, const char *, ... );
extern  int     sprintf( char *, const char *, ... );
extern  int     vprintf( const char *, _VA_LIST );
extern  int     vfprintf( FILE *, const char *, _VA_LIST );
extern  int     vsprintf( char *, const char *, _VA_LIST );

extern  size_t  fwrite( const void *, size_t, size_t, FILE * );

#if defined(_KERNEL_)
extern FILE *kfopen(const char *, const char *);
extern int kfseek(FILE *, long, int);
extern long kftell(FILE *);
extern long kfsize(FILE *);
extern size_t kfwrite(const void *, size_t, size_t, FILE *);
extern size_t kfread(void *, size_t, size_t, FILE *);
extern int kfremove(const char *pname);
extern int kfrename(const char *oldname, const char *newname);
extern int kfmove(const char *oldname, const char *newname);
extern int kftrim(const char *pname, long size);
#endif

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#endif /* _STDIO_H */
