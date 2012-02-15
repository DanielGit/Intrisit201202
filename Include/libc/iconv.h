// -*- C++ -*- header wrapper.

// Copyright (C) 2000 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

#ifndef __ICONV_H
#define __ICONV_H

#define  _LIBICONV_VERSION   0x0109 
#define  iconv_t   libiconv_t 
#define  iconv_open   libiconv_open 
#define  iconv   libiconv 
#define  iconv_close   libiconv_close 
#define  iconvctl   libiconvctl 
#define  ICONV_TRIVIALP   0 
#define  ICONV_GET_TRANSLITERATE   1 
#define  ICONV_SET_TRANSLITERATE   2 
#define  ICONV_GET_DISCARD_ILSEQ   3 
#define  ICONV_SET_DISCARD_ILSEQ   4 
#define  iconvlist   libiconvlist 


typedef void *  iconv_t;

iconv_t  iconv_open (const char *tocode, const char *fromcode) ;
size_t  iconv (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
int  iconv_close (iconv_t cd) ;
int  iconvctl (iconv_t cd, int request, void *argument) ;
void  iconvlist (int(*do_one)(unsigned int namescount, const char *const *names, void *data), void *data) ;
void  libiconv_set_relocation_prefix (const char *orig_prefix, const char *curr_prefix) ;



#endif //__ICONV_H

