
/*
        Copyright (C), 1993 SEIKO EPSON Corp.
                ALL RIGHTS RESERVED
 
        file name : ctype.h
 
 
        revision history
                1st release     1993/10/1       mizutani
                change prototype definition
                                1993/12/17      shinozuka
                change prototype definition for C++
                                2005/03/18      K.Watanabe
               change the prototype of the functions
                                 07/12/17       K.Watanabe
               prototypes changed  to revoke extern
                                 08/5/26        K.Watanabe

*/

#ifndef _CTYPE
#define _CTYPE

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */

int isalnum( int ) ;
int isalpha( int ) ;
int iscntrl( int ) ;
int isdigit( int ) ;
int isgraph( int ) ;
int islower( int ) ;
int isprint( int ) ;
int ispunct( int ) ;
int isspace( int ) ;
int isupper( int ) ;
int isxdigit( int ) ;
int tolower( int ) ;
int toupper( int ) ;

/* ADD K.Watanabe 2008/5/26 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe 2008/5/26 <<<<<<< */

#endif
