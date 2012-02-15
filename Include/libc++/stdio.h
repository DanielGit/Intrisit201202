
    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*         Copyright (C), 1993  SEIKO EPSON Corp.       */
    /*         ALL RIGHTS RESERVED                          */
    /*                                                      */
    /*                                                      */
    /*  file name : stdio.h                                 */
    /*                                                      */
    /*  Revision history                                    */
    /*      93/12/17    first release       S.Akaiwa        */
    /*      97/3/17 modified        T.Katahira      */
    /*          change function declaration     */
    /*                  to the ANSI style   */
    /*                                                      */
    /*      05/03/15    modified            K.Watanabe      */
    /*                  change prototype definition for C++ */
    /*         change the prototype of the functions        */
    /*                           07/12/17       K.Watanabe  */
    /*      2008/5/26 prototypes changed to revoke extern   */
    /*                                    K.Watanabe        */
    /*                                                      */
    /********************************************************/
                                        
#ifndef _STDIO_H
#define _STDIO_H

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;         /* size of type */
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

typedef long            fpos_t;         /* file type size */


typedef struct  {                       /* FILE structure */
        short           _flg;           /* unget exist flag */
        unsigned char   _buf;           /* unget character */
        int             _fd;            /* file descriptor */  
} FILE;

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

// CHG K.Watanabe 2008/5/26 >>>>>>>
#if 0
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
#if defined(UNIX) && !defined(ANSI)
extern  int     scanf( /* va_alist */ );
extern  int     fscanf( /* va_alilst */ );
extern  int     sscanf( /* va_alilst */ );
#else
extern  int     scanf( const char *, ... );
extern  int     fscanf( FILE *, const char *, ... );
/* CHG K.Watanabe V1.7 >>>>>>> */
/* extern  int     sscanf( char *, const char *, ... ); */
extern  int     sscanf( const char *, const char *, ... );
/* CHG K.Watanabe V1.7 <<<<<<< */
#endif
extern  size_t  fread( void *, size_t, size_t, FILE * );
extern  int     putchar( int );
extern  int     fputc( int, FILE * );
extern  int     putc( int, FILE * );
// CHG K.Watanabe 2007/12/17 >>>>>>>
#if 0
extern  int     puts( const char * );
extern  int     fputs( const char *, FILE * );
#endif
extern  int     puts( char * );
extern  int     fputs( char *, FILE * );
// CHG K.Watanabe 2007/12/17 <<<<<<<
extern  int     ungetc( int, FILE * );
#if defined(UNIX) && !defined(ANSI)
extern  int     printf( /* char *, ... */ );
extern  int     fprintf( /* FILE *, char *, ... */ );
extern  int     sprintf( /* char *, char *, ... */ );
extern  int     vprintf( /* char *, va_list */ );
extern  int     vfprintf( /* FILE *, char *, va_list */ );
extern  int     vsprintf( /* char *, char *, va_list */ );
#else
extern  int     printf( const char *, ...  );
extern  int     fprintf( FILE *, const char *, ... );
extern  int     sprintf( char *, const char *, ... );
extern  int     vprintf( const char *, _VA_LIST );
extern  int     vfprintf( FILE *, const char *, _VA_LIST );
extern  int     vsprintf( char *, const char *, _VA_LIST );
#endif
extern  size_t  fwrite( const void *, size_t, size_t, FILE * );
#endif




FILE    *tmpfile( void );
char    *tmpnam( char * );
int     remove( const char * );
int     rename( const char *, const char * );
FILE    *fopen( const char *, const char * );
FILE    *freopen( const char *, const char *, FILE * );
int     fclose( FILE * );
void    setbuf( FILE *, char * );
int     setvbuf( FILE *, char *, int, size_t );
int     fflush( FILE * );
void    clearerr( FILE * );
int     feof( FILE * );
int     ferror( FILE * );
void    perror( const char * );
int     fseek( FILE *, long, int );
int     fgetpos( FILE *, fpos_t * );
int     fsetpos(  FILE *, const fpos_t * );
long    ftell( FILE * );
void    rewind( FILE * );
int     getchar( void );
int     fgetc( FILE * );
int     getc( FILE * );
char    *gets( char * );
char    *fgets( char *, int, FILE * );

#if defined(UNIX) && !defined(ANSI)
int     scanf( /* va_alist */ );
int     fscanf( /* va_alilst */ );
int     sscanf( /* va_alilst */ );
#else
int     scanf( const char *, ... );
int     fscanf( FILE *, const char *, ... );
int     sscanf( const char *, const char *, ... );
#endif

size_t  fread( void *, size_t, size_t, FILE * );
int     putchar( int );
int     fputc( int, FILE * );
int     putc( int, FILE * );
int     puts( char * );
int     fputs( char *, FILE * );
int     ungetc( int, FILE * );

#if defined(UNIX) && !defined(ANSI)
int     printf( /* char *, ... */ );
int     fprintf( /* FILE *, char *, ... */ );
int     sprintf( /* char *, char *, ... */ );
int     vprintf( /* char *, va_list */ );
int     vfprintf( /* FILE *, char *, va_list */ );
int     vsprintf( /* char *, char *, va_list */ );
#else
int     printf( const char *, ...  );
int     fprintf( FILE *, const char *, ... );
int     sprintf( char *, const char *, ... );
int     vprintf( const char *, _VA_LIST );
int     vfprintf( FILE *, const char *, _VA_LIST );
int     vsprintf( char *, const char *, _VA_LIST );
#endif

size_t  fwrite( const void *, size_t, size_t, FILE * );
// CHG K.Watanabe 2008/5/26 <<<<<<<

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#endif /* _STDIO_H */
