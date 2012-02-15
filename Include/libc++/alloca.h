/*
        Copyright (C), 2004 SEIKO EPSON Corp.
                ALL RIGHTS RESERVED
 
        file name : alloca.h
 
        revision history
                1st release     2004/12/24 K.Watanabe
*/


# define alloca(n)   __builtin_alloca (n)
