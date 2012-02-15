    
    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*         Copyright (C), 1993  SEIKO EPSON Corp.       */
    /*         ALL RIGHTS RESERVED                          */
    /*                                                      */
    /*                                                      */
    /*  file name : errno.h                                 */
    /*                                                      */
    /*  Revision history                                    */
    /*      93/08/31    first release       K.Shinozuka     */
    /*      93/11/31    small modification  K.Shinozuka     */
    /*                                                      */
    /********************************************************/


#ifndef SMC_ERRNO_H
#define SMC_ERRNO_H

extern int errno;

#define EPERM       1       /* Operation not permitted */
#define ENOENT      2       /* No such file or directory */
#define ENOMEM      12      /* Cannot allocate memory */
#define EINVAL      22      /* Invalid argument */
#define ESPIPE      29      /* Illegal seek */
#define EDOM        33      /* domain error */
#define ERANGE      34      /* range error */

#endif
