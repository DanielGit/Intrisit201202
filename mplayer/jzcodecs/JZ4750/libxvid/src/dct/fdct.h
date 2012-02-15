/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Forward DCT header  -
 *
 *  Copyright(C) 2001-2003 Michael Militzer <isibaar@xvid.org>
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
 * $Id: fdct.h,v 1.1.1.1 2009/10/13 06:37:38 gysun Exp $
 *
 ****************************************************************************/

#ifndef _FDCT_H_
#define _FDCT_H_

typedef void (fdctFunc) (short *const block);
typedef fdctFunc *fdctFuncPtr;

extern fdctFuncPtr fdct;

fdctFunc fdct_int32;

#ifdef ARCH_IS_IA32
fdctFunc fdct_mmx_ffmpeg;
fdctFunc fdct_xmm_ffmpeg;
fdctFunc fdct_mmx_skal;
fdctFunc fdct_xmm_skal;
fdctFunc fdct_sse2_skal;
#endif

#ifdef ARCH_IS_IA64
fdctFunc fdct_ia64;
#endif

#ifdef ARCH_IS_X86_64
fdctFunc fdct_skal_x86_64;
#endif

#endif							/* _FDCT_H_ */