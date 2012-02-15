/* Copyright (C) 1991,92,96,97,99,2000,2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef	_STRINGS_H
#define	_STRINGS_H	1

int    bcmp(const void *, const void *, size_t);
void   bcopy(const void *, void *, size_t);
void   bzero(void *, size_t);
int    ffs(int);
char   *index(const char *, int);
char   *rindex(const char *, int);
int    strcasecmp(const char *, const char *);
int    strncasecmp(const char *, const char *, size_t);

#endif	/* strings.h  */
