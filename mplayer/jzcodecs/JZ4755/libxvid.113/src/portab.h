/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Portable macros, types and inlined assembly -
 *
 *  Copyright(C) 2002      Michael Militzer <isibaar@xvid.org>
 *               2002-2003 Peter Ross <pross@xvid.org>
 *               2002-2003 Edouard Gomez <ed.gomez@free.fr>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: portab.h,v 1.1.1.1 2009/03/31 01:50:25 yzhai Exp $
 *
 ****************************************************************************/

#ifndef _PORTAB_H_
#define _PORTAB_H_

#    define CACHE_LINE  64

#    define ptr_t     int
#    define uintptr_t unsigned int

#    define int8_t   char
#    define uint8_t  unsigned char
#    define int16_t  short
#    define uint16_t unsigned short
#    define int32_t  int
#    define uint32_t unsigned int
#    define int64_t  long long int
#    define uint64_t unsigned long long int

static __inline void DPRINTF(int level, char *format, ...) {}

#    define DECLARE_ALIGNED_MATRIX(name,sizex,sizey,type,alignment) \
	type name##_storage[(sizex)*(sizey)+(alignment)-1]; \
type * name = (type *) (((ptr_t) name##_storage+(alignment - 1)) & ~((ptr_t)(alignment)-1))


#    define BSWAP(a) \
	((a) = (((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
	 (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))


#endif /* PORTAB_H */
