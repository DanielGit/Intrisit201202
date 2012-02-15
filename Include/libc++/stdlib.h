    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*         Copyright (C), 1993  SEIKO EPSON Corp.       */
    /*         ALL RIGHTS RESERVED                          */
    /*                                                      */
    /*                                                      */
    /*  file name : stdlib.h                                */
    /*                                                      */
    /*  Revision history                                    */
    /*      93/12/17    first release       S.Akaiwa        */
    /*      97/02/20    modified            T.Katahira      */
    /*                  change function declaration         */
    /*                          to the ANSI style           */
    /*      00/06/01    modified            M.Kakinuma      */
    /*      01/12/21    modified wchar_t    T.Tazaki        */
    /*                  stddef.h and a multiplex definition */
    /*      05/03/18    modified            K.Watanabe      */
    /*                  change prototype definition for C++ */
    /*      2008/5/26   prototypes changed  to revoke extern */
    /*                                      K.Watanabe      */
    /*                                                      */
    /********************************************************/

#ifndef _STDLIB_H
#define _STDLIB_H

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

#ifndef _WCHART
#define _WCHART
typedef unsigned short  _Wchart;
#ifndef __cplusplus                 /* ADD K.Watanabe V1.7 */
typedef _Wchart wchar_t;
#endif  /* __cplusplus */           /* ADD K.Watanabe V1.7 */
#endif

typedef struct {                    /* division type */
  int rem;
  int quot; } div_t;

typedef struct {                    /* long division type */
  long rem;
  long quot; } ldiv_t;

#if !defined(NULL)
/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus                     
#define NULL         0
#else
/* ADD K.Watanabe V1.7 <<<<<<< */
#  define NULL          ((void *)0L)/* null pointer */
#endif /* __cplusplus */            /* ADD K.Watanabe V1.7 */
#endif

#define RAND_MAX        0x7fff      /* random maximum number */

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/*  global, memory allocation proc.*/
extern unsigned char *ansi_ucStartAlloc;   /* allocate area, start address */
extern unsigned char *ansi_ucEndAlloc;     /* allocate area, end address   */
extern unsigned char *ansi_ucNxtAlcP;      /* next allocate address        */
extern unsigned char *ansi_ucTblPtr;       /* next address for table       */
extern unsigned long ansi_ulRow;           /* table count   */


extern  unsigned int    seed;       /* random seed */

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

typedef int fn_t( const void *, const void * );

/* prototype definition */
// CHG K.Watanabe 2008/5/26 >>>>>>>
#if 0
extern  void            abort( void );
extern  void            exit( int ); 
extern  int             atexit( void (*)(void) );
extern  char *          getenv( const char * );
extern  int             system( const char * );
extern  void *          malloc( size_t );
extern  void *          calloc( size_t, size_t );
extern  void *          realloc( void *, size_t );
extern  void            free( void * ); 
extern  int             ansi_InitMalloc(unsigned long, unsigned long);
extern  int             atoi( const char * );
extern  long            atol( const char * );
extern  double          atof( const char * );
extern  long            strtol( const char *, char **, int );
extern  unsigned long   strtoul( const char *, char **, int );
/* ADD K.Watanabe V1.7 >>>>>>> */
extern  long long           strtoll( const char *, char **, int );
extern  unsigned long long  strtoull( const char *, char **, int );
/* ADD K.Watanabe V1.7 <<<<<<< */
extern  double          strtod( const char *, char ** );
extern  int             abs( int );
extern  long            labs( long );
extern  div_t           div( int, int );
extern  ldiv_t          ldiv( long, long );
extern  int             rand( void );
extern  void            srand( unsigned int );
extern  void *          bsearch( const void *, const void *, 
                        size_t, size_t, fn_t * );
extern  void            qsort( void *, size_t, size_t, fn_t * );
#endif



void            abort( void );
void            exit( int ); 
int             atexit( void (*)(void) );
char *          getenv( const char * );
int             system( const char * );
void *          malloc( size_t );
void *          calloc( size_t, size_t );
void *          realloc( void *, size_t );
void            free( void * ); 
int             ansi_InitMalloc(unsigned long, unsigned long);
int             atoi( const char * );
long            atol( const char * );
double          atof( const char * );
long            strtol( const char *, char **, int );
unsigned long   strtoul( const char *, char **, int );
long long           strtoll( const char *, char **, int );
unsigned long long  strtoull( const char *, char **, int );
double          strtod( const char *, char ** );
int             abs( int );
long            labs( long );
div_t           div( int, int );
ldiv_t          ldiv( long, long );
int             rand( void );
void            srand( unsigned int );
void *          bsearch( const void *, const void *, 
                        size_t, size_t, fn_t * );
void            qsort( void *, size_t, size_t, fn_t * );
// CHG K.Watanabe 2008/5/26 <<<<<<<

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#endif  /* _STDLIB_H */
