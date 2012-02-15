
#ifndef INT_TYPES_H
#define INT_TYPES_H
#include "arch/mips/types.h"
typedef unsigned long long int uint64_t;
typedef long long int int64_t;

typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef int int_fast16_t;
typedef unsigned int uint_fast16_t;
typedef int int_fast32_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long int uint_fast64_t;

#define __PRIPTR_PREFIX    "l"
#define PRIdFAST16  __PRIPTR_PREFIX "d"
#define PRId32              "d"
#define PRIX32              "X"
#define PRIdFAST32  __PRIPTR_PREFIX "d"

#define __PRI64_PREFIX     "ll"
#define PRId64              __PRI64_PREFIX "d"
#define PRIX64              __PRI64_PREFIX "X"
#define PRIx64              __PRI64_PREFIX "x"
#define PRIu64              __PRI64_PREFIX "u"

#define EILSEQ          88      /* Illegal byte sequence */

#endif
