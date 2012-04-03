#include "uclib.h"
#include "LocalMemMgr.h"
#undef perror
extern void noah_kprintf(const char *format, ... );

int perror(const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start (ap, fmt);
  ret =  kprintf(fmt, ap);
  va_end (ap);
  return ret;
}
int please_use_av_log(const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start (ap, fmt);
  ret = kprintf(fmt, ap);
  va_end (ap);
  return ret;

}

static char *mp_memory = NULL;
static unsigned int mp_memory_size = 0;
unsigned int mp_memory_empty = 0;

#define MEM_STAT    0 
#if MEM_STAT
#undef printf
static char *old_memory = NULL;
static void print_mem_use(void *mem)
{
	if((unsigned int ) mem > (unsigned int )old_memory)
	{
		kprintf("cur mem max using %x\n",(unsigned int)mem - (unsigned int)mp_memory);
		old_memory = mem;
	}
}
#else
#define print_mem_use(x)
#endif
void mplayer_memory_set(void *mp,unsigned int size)
{
	mp_memory_size = size;
	mp_memory = mp;
	#if MEM_STAT
	old_memory = mp_memory; 
	#endif
}

int mp_memory_init(int mode)
{
	memset(mp_memory,0,mp_memory_size);
	Local_HeapInit((unsigned int)mp_memory,mp_memory_size);
	return 1;
}

void mp_memory_deinit(int mode)
{
	mp_memory = NULL;
	mp_memory_size = 0;
}

#define MEM_ADDRESS(x) ((unsigned int) x - (unsigned int)*(char *)((unsigned int)x - 1))
#define isBuddy(address) (*(unsigned int *)MEM_ADDRESS(address) > (unsigned int)0x80000000)

#define BUDDY_MEM  1
void *uc_malloc(unsigned int size)
{
	void *ret = NULL;
	#if BUDDY_MEM
	if(size <= (BUDDY_BLOCK_SIZE / 4))
		ret = (void *)buddyAlloc((unsigned int)mp_memory,size);
	#endif
	
	if(ret == NULL)
		ret = (void *)Local_Alloc((unsigned int)mp_memory,size);
	print_mem_use(ret+size);
	
	if (!ret)
	{
		kprintf("mplayer %s error size: %d\n", __FUNCTION__, size);
		mp_memory_empty = 1;	
	 }
	return ret;
}

void uc_free(void *addr)
{
	if(addr != NULL)
	{
		#if BUDDY_MEM
			if(isBuddy(addr))
			{	
				//unsigned int *d = (unsigned int *)MEM_ADDRESS(addr);		
				buddyFree((unsigned int)mp_memory,addr);
			}
			else{
		#endif
				Local_Dealloc((unsigned int)mp_memory,(unsigned int)addr);
			#if BUDDY_MEM
			}
			#endif
	}
}
void *uc_calloc(unsigned int x,unsigned int n)
{
	void *ret = NULL;
	unsigned int size = x * n;
	#if BUDDY_MEM
	if(size <= (BUDDY_BLOCK_SIZE / 4))
		ret = (void *)buddyCalloc((unsigned int)mp_memory,x,n);
	#endif
	if(ret == NULL)
		ret = (void *)Local_Calloc((unsigned int)mp_memory,x,n);
	print_mem_use(ret+n*x);
	if (!ret)
	{
		kprintf("mplayer %s error size: %d\n", __FUNCTION__, n);
		mp_memory_empty = 1;	
	 }
	return ret;
}

void *uc_realloc(void *addr,unsigned int size)
{
	void *ret = NULL;
	if(addr == NULL)
	{
			addr = uc_malloc(size);
			return addr;
	}
	#if BUDDY_MEM
	if(isBuddy(addr))
		ret = (void *)buddyRealloc((unsigned int)mp_memory,addr,size);
	#endif
	if(ret == NULL)
		ret = (void *)Local_Realloc((unsigned int)mp_memory,(unsigned int)addr,size);
	print_mem_use(ret+size);
	if (!ret)
	{
		kprintf("mplayer %s error size: %d\n", __FUNCTION__, size);
		mp_memory_empty = 1;	
	 }
	return ret;
}

void *uc_memalign(unsigned int x,unsigned int size)
{
	void *ret = NULL;
	#if BUDDY_MEM
	if(size <= (BUDDY_BLOCK_SIZE / 4))
		ret = (void *)buddyAlignalloc((unsigned int)mp_memory,x,(unsigned int)size); 
	#endif
	if(ret == NULL)
		ret = (void *)Local_alignAlloc((unsigned int)mp_memory,x,(unsigned int)size); 
	print_mem_use(ret+size);
	if (!ret)
	{
		kprintf("mplayer %s error size: %d\n", __FUNCTION__, size);
		mp_memory_empty = 1;	
	 }
	return ret;
}

char * uc_strdup(const char *str)
{
   char *p;
   if (!str)
      return(NULL);
   if ((p = (char *)uc_malloc(strlen(str) + 1)) != 0)
      return(strcpy(p,str));
   return(NULL);
}

#ifndef USE_16M_SDRAM
//-----------------------------drv mem--------------------------------------------
void *uc_malloc_static(unsigned int addr)
{
	return (void *)alloc((unsigned int)addr);
}
void uc_free_static(void *addr)
{
	if(addr != NULL)
		deAlloc((unsigned int)addr);
}
void *uc_calloc_static(unsigned int x,unsigned int n)
{
	return (void *)Drv_calloc(x,n);
}
void *uc_realloc_static(void *addr,unsigned int size)
{
	return (void *)Drv_realloc((unsigned int)addr,(unsigned int)size);
}
void *uc_memalign_static(unsigned int x,unsigned int size)
{
	return (void *)alignAlloc(x,(unsigned int)size);
}
//-------------------------------------------------------------------------
#else
//-----------------------------module mem--------------------------------------------
void *uc_malloc_static(unsigned int size)
{
	
	return uc_malloc(size);
}
void uc_free_static(void *addr)
{
	uc_free(addr);
}
void *uc_calloc_static(unsigned int x,unsigned int n)
{
	return uc_calloc(x,n);
}
void *uc_realloc_static(void *addr,unsigned int size)
{
	return uc_realloc(addr,size);
}
void *uc_memalign_static(unsigned int x,unsigned int size)
{
	return uc_memalign(x,size);
}
#endif

void dumpdatabuf(unsigned char *buf,unsigned int len)
{
	int i,j;
	printf("\n");
	for(i = 0;i < len / 16;i++)
	{
		printf("%04x: ",i);
		for(j = 0; j < 16; j++)
			printf("%02x ",buf[j + i * 16]);
		printf("\n");	
	}
	if((i & 15) > 0)
	{
			printf("%02x: ",j);
		for(j = 0; j < (i & 15); j++)
			printf("%02x ",buf[j + i * 16]);
		printf("\n");	
	}
}


#if 0
#define MAX_TEST 8 * 1024 * 1024	
unsigned int findmax_mem(unsigned int dsize)
{
  void * mem,*firstmem;
  unsigned int i;
  firstmem = uc_malloc(4);
  uc_free(firstmem);
	for(i = 0; i < dsize / 4 ;i++)
	{
		mem = uc_malloc(4 * i);
		
		if(mem == 0)
		{
			break;
		}else
			uc_free(mem);	
	}
	return ((unsigned int)mem - (unsigned int)firstmem);
}
#endif
/*** 
*qsort.c - quicksort algorithm; qsort() library function for sorting arrays 
* 
* Copyright (c) 1985-1997, Microsoft Corporation. All rights reserved. 
* 
*Purpose: 
* To implement the qsort() routine for sorting arrays. 
* 
*******************************************************************************/ 
/*
#include <cruntime.h> 
#include <stdlib.h> 
#include <search.h>
*/ 

#define __cdecl
/* prototypes for local routines */ 
static void __cdecl shortsort(char *lo, char *hi, unsigned width, 
int (__cdecl *comp)(const void *, const void *)); 
static void __cdecl swap(char *p, char *q, unsigned int width); 

/* this parameter defines the cutoff between using quick sort and 
insertion sort for arrays; arrays with lengths shorter or equal to the 
below value use insertion sort */ 

#define CUTOFF 8 /* testing shows that this is good value */ 


/*** 
*qsort(base, num, wid, comp) - quicksort function for sorting arrays 
* 
*Purpose: 
* quicksort the array of elements 
* side effects: sorts in place 
* 
*Entry: 
* char *base = pointer to base of array 
* unsigned num = number of elements in the array 
* unsigned width = width in bytes of each array element 
* int (*comp)() = pointer to function returning analog of strcmp for 
* strings, but supplied by user for comparing the array elements. 
* it accepts 2 pointers to elements and returns neg if 1<2, 0 if 
* 1=2, pos if 1>2. 
* 
*Exit: 
* returns void 
* 
*Exceptions: 
* 
*******************************************************************************/ 

/* sort the array between lo and hi (inclusive) */ 
void __cdecl qsort ( 
void *base, 
unsigned num, 
unsigned width, 
int (__cdecl *comp)(const void *, const void *) 
) 
{ 
	char *lo, *hi; /* ends of sub-array currently sorting */ 
	char *mid; /* points to middle of subarray */ 
	char *loguy, *higuy; /* traveling pointers for partition step */ 
	unsigned size; /* size of the sub-array */ 
	char *lostk[30], *histk[30]; 

	int stkptr; /* stack for saving sub-array to be processed */ 
	/* Note: the number of stack entries required is no more than 
	1 + log2(size), so 30 is sufficient for any array */ 
	
	if (num < 2 || width == 0) 
		return; /* nothing to do */ 
	
	stkptr = 0; /* initialize stack */ 
	
	lo = (char *)base; 
	hi = (char *)base + width * (num-1); /* initialize limits */ 
	
	/* this entry point is for pseudo-recursion calling: setting 
	lo and hi and jumping to here is like recursion, but stkptr is 
	prserved, locals aren't, so we preserve stuff on the stack */ 
	recurse: 
	
	size = (hi - lo) / width + 1; /* number of el's to sort */ 
	
	/* below a certain size, it is faster to use a O(n^2) sorting method */ 
	if (size <= CUTOFF) { 
		shortsort(lo, hi, width, comp); 
	} 
	else { 
		/* First we pick a partititioning element. The efficiency of the 
		algorithm demands that we find one that is approximately the 
		median of the values, but also that we select one fast. Using 
		the first one produces bad performace if the array is already 
		sorted, so we use the middle one, which would require a very 
		wierdly arranged array for worst case performance. Testing shows 
		that a median-of-three algorithm does not, in general, increase 
		performance. */ 
		
		mid = lo + (size / 2) * width; /* find middle element */ 
		swap(mid, lo, width); /* swap it to beginning of array */ 
		
		/* We now wish to partition the array into three pieces, one 
		consisiting of elements <= partition element, one of elements 
		equal to the parition element, and one of element >= to it. This 
		is done below; comments indicate conditions established at every 
		step. */ 
		
		loguy = lo; 
		higuy = hi + width; 
		
		/* Note that higuy decreases and loguy increases on every iteration, 
		so loop must terminate. */ 
		for (;;) { 
			/* lo <= loguy < hi, lo < higuy <= hi + 1, 
			A[i] <= A[lo] for lo <= i <= loguy, 
			A[i] >= A[lo] for higuy <= i <= hi */ 
			
			 do { 
					loguy += width; 
				} while (loguy <= hi && comp(loguy, lo) <= 0); 
			
			/* lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy, 
			either loguy > hi or A[loguy] > A[lo] */ 
			
			do { 
				higuy -= width; 
			} while (higuy > lo && comp(higuy, lo) >= 0); 
			
			/* lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi, 
			either higuy <= lo or A[higuy] < A[lo] */ 
			
			if (higuy < loguy) 
				break; 
			
			/* if loguy > hi or higuy <= lo, then we would have exited, so 
			A[loguy] > A[lo], A[higuy] < A[lo], 
			loguy < hi, highy > lo */ 
			
			swap(loguy, higuy, width); 
			
			/* A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top 
			of loop is re-established */ 
		} 
		/* A[i] >= A[lo] for higuy < i <= hi, 
		A[i] <= A[lo] for lo <= i < loguy, 
		higuy < loguy, lo <= higuy <= hi 
		implying: 
		A[i] >= A[lo] for loguy <= i <= hi, 
		A[i] <= A[lo] for lo <= i <= higuy, 
		A[i] = A[lo] for higuy < i < loguy */ 
		
		swap(lo, higuy, width); /* put partition element in place */ 
		
		/* OK, now we have the following: 
		A[i] >= A[higuy] for loguy <= i <= hi, 
		A[i] <= A[higuy] for lo <= i < higuy 
		A[i] = A[lo] for higuy <= i < loguy */ 
		
		/* We've finished the partition, now we want to sort the subarrays 
		[lo, higuy-1] and [loguy, hi]. 
		We do the smaller one first to minimize stack usage. 
		We only sort arrays of length 2 or more.*/ 
		
		if ( higuy - 1 - lo >= hi - loguy ) { 
			if (lo + width < higuy) { 
				lostk[stkptr] = lo; 
				histk[stkptr] = higuy - width; 
				++stkptr; 
			} /* save big recursion for later */ 
			
			if (loguy < hi) { 
				lo = loguy; 
				goto recurse; /* do small recursion */ 
			} 
		} 
		else { 
			if (loguy < hi) { 
				lostk[stkptr] = loguy; 
				histk[stkptr] = hi; 
				++stkptr; /* save big recursion for later */ 
			} 
			
			if (lo + width < higuy) { 
				hi = higuy - width; 
				goto recurse; /* do small recursion */ 
			} 
		} 
	} 
	
	/* We have sorted the array, except for any pending sorts on the stack. 
	Check if there are any, and do them. */ 
	
	--stkptr; 
	if (stkptr >= 0) { 
		lo = lostk[stkptr]; 
		hi = histk[stkptr]; 
		goto recurse; /* pop subarray from stack */ 
	} 
	/* all subarrays done */ 

} 


/*** 
*shortsort(hi, lo, width, comp) - insertion sort for sorting short arrays 
* 
*Purpose: 
* sorts the sub-array of elements between lo and hi (inclusive) 
* side effects: sorts in place 
* assumes that lo < hi 
* 
*Entry: 
* char *lo = pointer to low element to sort 
* char *hi = pointer to high element to sort 
* unsigned width = width in bytes of each array element 
* int (*comp)() = pointer to function returning analog of strcmp for 
* strings, but supplied by user for comparing the array elements. 
* it accepts 2 pointers to elements and returns neg if 1<2, 0 if 
* 1=2, pos if 1>2. 
* 
*Exit: 
* returns void 
* 
*Exceptions: 
* 
*******************************************************************************/ 

static void __cdecl shortsort ( 
char *lo, 
char *hi, 
unsigned width, 
int (__cdecl *comp)(const void *, const void *) 
) 
{ 
		char *p, *max; 
		
		/* Note: in assertions below, i and j are alway inside original bound of 
		array to sort. */ 
		
		while (hi > lo) { 
		/* A[i] <= A[j] for i <= j, j > hi */ 
		max = lo; 
		for (p = lo+width; p <= hi; p += width) { 
			/* A[i] <= A[max] for lo <= i < p */ 
			if (comp(p, max) > 0) { 
				max = p; 
			} 
			/* A[i] <= A[max] for lo <= i <= p */ 
		} 
		
		/* A[i] <= A[max] for lo <= i <= hi */ 
		
		swap(max, hi, width); 
		
		/* A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi */ 
		
		hi -= width; 
		
		/* A[i] <= A[j] for i <= j, j > hi, loop top condition established */ 
		} 
		/* A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j, 
		so array is sorted */ 
} 


/*** 
*swap(a, b, width) - swap two elements 
* 
*Purpose: 
* swaps the two array elements of size width 
* 
*Entry: 
* char *a, *b = pointer to two elements to swap 
* unsigned width = width in bytes of each array element 
* 
*Exit: 
* returns void 
* 
*Exceptions: 
* 
*******************************************************************************/ 

static void __cdecl swap ( 
char *a, 
char *b, 
unsigned width 
) 
{ 
	char tmp; 
	
	if ( a != b ) 
	/* Do the swap one character at a time to avoid potential alignment 
	problems. */ 
	while ( width-- ) { 
		tmp = *a; 
		*a++ = *b; 
		*b++ = tmp; 
	}
}

