/*
    inttypes.h
 
    Contributors:
      Created by Marek Michalkiewicz <marekm@linux.org.pl>
 
    THIS SOFTWARE IS NOT COPYRIGHTED
 
   This source code is offered for use in the public domain.  You may
   use, modify or distribute it freely.

   This code is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY.  ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
   DISCLAIMED.  This includes but is not limited to warranties of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef __INTTYPES_H_
#define __INTTYPES_H_

#include "stdint.h"

#if defined(WIN32)
#define __attribute__(a)
#endif

#define	PRId32			"d"
#define	PRIi32			"i"
#define	PRIo32			"o"
#define	PRIu32			"u"
#define	PRIx32			"x"
#define	PRIX32			"X"
#define	SCNd32			"d"
#define	SCNi32			"i"
#define	SCNo32			"o"
#define	SCNu32			"u"
#define	SCNx32			"x"

#define	PRId64			"ld"
#define	PRIi64			"li"
#define	PRIo64			"lo"
#define	PRIu64			"lu"
#define	PRIx64			"lx"
#define	PRIX64			"lX"
#define	SCNd64			"ld"
#define	SCNi64			"li"
#define	SCNo64			"lo"
#define	SCNu64			"lu"
#define	SCNx64			"lx"

#define	PRIdFAST8		"d"
#define	PRIiFAST8		"i"
#define	PRIoFAST8		"o"
#define	PRIuFAST8		"u"
#define	PRIxFAST8		"x"
#define	PRIXFAST8		"X"
#define	SCNdFAST8		"d"
#define	SCNiFAST8		"i"
#define	SCNoFAST8		"o"
#define	SCNuFAST8		"u"
#define	SCNxFAST8		"x"

#define	PRIdFAST16		"d"
#define	PRIiFAST16		"i"
#define	PRIoFAST16		"o"
#define	PRIuFAST16		"u"
#define	PRIxFAST16		"x"
#define	PRIXFAST16		"X"
#define	SCNdFAST16		"d"
#define	SCNiFAST16		"i"
#define	SCNoFAST16		"o"
#define	SCNuFAST16		"u"
#define	SCNxFAST16		"x"

#define	PRIdFAST32		"d"
#define	PRIiFAST32		"i"
#define	PRIoFAST32		"o"
#define	PRIuFAST32		"u"
#define	PRIxFAST32		"x"
#define	PRIXFAST32		"X"
#define	SCNdFAST32		"d"
#define	SCNiFAST32		"i"
#define	SCNoFAST32		"o"
#define	SCNuFAST32		"u"
#define	SCNxFAST32		"x"

#define	PRIdLEAST64		"ld"
#define	PRIiLEAST64		"li"
#define	PRIoLEAST64		"lo"
#define	PRIuLEAST64		"lu"
#define	PRIxLEAST64		"lx"
#define	PRIXLEAST64		"lX"
#define	SCNdLEAST64		"ld"
#define	SCNiLEAST64		"li"
#define	SCNoLEAST64		"lo"
#define	SCNuLEAST64		"lu"
#define	SCNxLEAST64		"lx"

#endif
