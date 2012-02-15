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

#ifndef _SYS_LIST_H
#define _SYS_LIST_H

typedef struct _LIST 
{
	struct _LIST *Next;
	struct _LIST *Prev;
}LIST;

typedef LIST *PLIST;

#define ListInit(head)		((head)->Next = (head)->Prev = (head))
#define ListNext(node)		((node)->Next)
#define ListPrev(node)		((node)->Prev)
#define ListEmpty(head)		((head)->Next == (head))
#define ListFirst(head)		((head)->Next)
#define ListLast(head)		((head)->Prev)
#define ListEnd(head, node)	((node) == (head))

/*
 * Get the struct for this entry
 */
#define ListEntry(p, type, member) \
    ((type*)((DWORD)(p) - (DWORD)(&((type*)0)->member)))


/*
 * Insert new node after specified node
 */
static __inline void ListInsert(PLIST prev, PLIST node)
{
	prev->Next->Prev = node;
	node->Next = prev->Next;
	node->Prev = prev;
	prev->Next = node;
}

/*
 * Remove specified node from list
 */
static __inline void ListRemove(PLIST node)
{
	node->Prev->Next = node->Next;
	node->Next->Prev = node->Prev;
	node->Next = node;
	node->Prev = node;
}

#endif /* !_SYS_LIST_H */
