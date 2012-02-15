/*
 * Copyright (c) 2005-2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _EXEC_H
#define _EXEC_H

#include <kernel/kernel.h>
#include <kernel/task.h>
#include <kernel/ipc.h>
#include <sys/param.h>
#include <sys/syslog.h>
#include <sys/stat.h>

#define HEADER_SIZE		512

// Definition for exec loader
typedef struct _EXEC_LOADER
{
	const char *Name;								/* name of loader */
	void	(*Init)(void);							/* initialize routine */
	int	(*Probe)(void *);							/* probe routine */
	int	(*Load)(void*, HANDLE, FILE*, void**, void (**)(void));	/* load routine */
	int (*ProgSize)(void*, FILE*);
}EXEC_LOADER;
typedef EXEC_LOADER *PEXEC_LOADER;

#if defined(STC_EXP)
HANDLE sCreateProcess(const char* program, const char *cmdline);
HANDLE sCreateSimDebugProcess(const char* program, const char *cmdline,  int(*entry)(int, char**));
#else
HANDLE CreateProcess(const char* program, const char *cmdline);
HANDLE CreateSimDebugProcess(const char* program, const char *cmdline, int(*entry)(int, char**));
#endif

#endif /* !_EXEC_H */
