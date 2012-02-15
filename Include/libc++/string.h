/*
        Copyright (C), 1993 SEIKO EPSON Corp.
                ALL RIGHTS RESERVED
 
        file name : string.h
 
 
        revision history
                1st release     1993/10/1       mizutani
                modify to work on native environment
                                1993/12/17      shinozuka
                modify to change prototype definition for C++
                                2005/03/18      K.Watanabe
               change the prototype of some functions
                                 07/12/17       K.Watanabe
                prototypes changed to revoke comments of parameters
                                2008/5/26       K.Watanabe
*/

#ifndef _STRING_H
#define _STRING_H

#ifndef NULL
#define NULL    0
#endif

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t ;
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

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */

void *memchr( const void *, int, size_t ) ;
int memcmp( const void *,const void *,size_t );
void *memcpy( void *,const void *,size_t );
void *memmove( void *,const void *,size_t );
void *memset( void *,int,size_t );
char *strcat( char *, const char * ) ;
char *strchr( const char *, int ) ;
int strcmp(  const char *, const char * ) ;
char *strcpy( char *, const char * ) ;
size_t strcspn( const char *, const char * ) ;
char *strerror( int ) ;
size_t strlen(  const char * ) ;
char *strncat( char *, const char *, size_t ) ;
int strncmp( const char *, const char *, size_t ) ;
char *strncpy( char *, const char *, size_t ) ;
char *strpbrk( const char *, const char * ) ;
char *strrchr( const char *, int ) ;
size_t strspn( const char *, const char * ) ;
char *strstr( const char *, const char * ) ;
char *strtok( char *, const char * ) ;

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */
#endif
