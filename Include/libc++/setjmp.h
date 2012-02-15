
    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*         Copyright (C), 2001  SEIKO EPSON Corp.       */
    /*         ALL RIGHTS RESERVED                          */
    /*                                                      */
    /*                                                      */
    /*  file name : setjmp.h                                */
    /*                                                      */
    /*  Revision history                                    */
    /*    2001/08/09    first release       T.Tazaki        */
    /*    2005/03/18    modified            K.Watanabe      */
    /*                  change prototype definition for C++ */
    /*    2008/5/26     prototypes changed to revoke extern */
    /*                                      K.Watanabe      */
    /*                                                      */
    /********************************************************/

#ifndef _SETJMP_H
#define _SETJMP_H

#if defined(__mips__)
typedef int jmp_buf[12];
#elif defined(__arm__)
typedef int jmp_buf[24];
#elif defined(__i386__)
typedef int jmp_buf[6];
#else
#error arch no support
#endif

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
extern "C" {
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

// CHG K.Watanabe 2008/5/26 >>>>>>>
#if 0
extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);
#endif

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);
// CHG K.Watanabe 2008/5/26 <<<<<<<

/* ADD K.Watanabe V1.7 >>>>>>> */
#ifdef __cplusplus
}
#endif
/* ADD K.Watanabe V1.7 <<<<<<< */

#endif /* _SETJMP_H */
