/*
 *  JZ4740  mobile_tv  Project  V1.0.0
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *  Author: <dsqiu@ingenic.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  2007-12-07, dsqiu
 *  Created.
 */
#define VALIDCHECK  0

#define TRUE       1
#define FALSE      0
#define FREE		   0
#define RESERVED	 1
/* Memory block header = 9 bytes = 4 previous + 4 next + status */
#define SIZE_HEADER	32
#define local_prev(i)		(*((unsigned int *) (i + 4)))
#define local_next(i)		(*((unsigned int *) (i + 8)))
#define local_status(i)	(*((unsigned int *) (i + 0)))
#define local_size(i)		(local_next(i)-i-SIZE_HEADER)
#define local_setalign(i,y)  (*((unsigned char *)(i-1))) = (y + SIZE_HEADER) 
#define local_getalign(i)  ((*((unsigned char *)(i-1))) - SIZE_HEADER) 

/* if going to split free block, need at least 4 bytes in new free part */
#define MIN_FREE_BYTES   32

//static unsigned char memory[MEM_LENGHT] __attribute__ ((aligned (MIN_FREE_BYTES)));
#define local_first(x) (*(unsigned int *)x)      /*stores address of first byte of heap*/
#define local_last(x) (*(unsigned int *)(x+4)) /*store address of last byte of heap + 1*/ 

//#define inline 
//#define static
//#undef printf
/*
   Local Heap Initiale 
   note: heap is 4 byte aligned
*/   
 
static inline int Local_HeapInit(unsigned int heap,unsigned int size)
{
	if((heap % 4) != 0)
		return 0;
	if(size < 16)
		return 0;
	if(MIN_FREE_BYTES > 8)
		local_first(heap) = (unsigned int)(heap + MIN_FREE_BYTES);
	else		
		local_first(heap) = (unsigned int)(heap + 8);
	local_last(heap) = ((unsigned int)heap + size - SIZE_HEADER) / 4 * 4;
	
	local_prev(local_first(heap))=0;
	local_next(local_first(heap))=local_last(heap);
	local_status(local_first(heap))=FREE;
	
	local_prev(local_last(heap))=local_first(heap);
	local_next(local_last(heap))=0;
	local_status(local_last(heap))=RESERVED;
	return 1;

}
/*end heapInit*/

static inline int currentNodeAlloc(unsigned int heap,unsigned int i,unsigned int nbytes)
{
	unsigned int free_size;
   	/*handle case of current block being the last*/
   
 	if(local_next(i) == 0){
		free_size = local_last(heap) - i - SIZE_HEADER;
	}else{
	    free_size = local_size(i);
	}
   
	/*either split current block, use entire current block, or fail*/
    
	if(free_size >= nbytes + SIZE_HEADER + MIN_FREE_BYTES)
	{
		unsigned int old_next;
		unsigned int old_block;
		unsigned int new_block;

		old_next = local_next(i);
		old_block = i;

		/*fix up original block*/

		local_next(i) = i+SIZE_HEADER+nbytes;	
		new_block = local_next(i);
		local_status(i)=RESERVED;

		/*set up new free block*/

		i = local_next(i);					
		local_next(i)=old_next;
		local_prev(i)=old_block;
		local_status(i)=FREE;

		/*right nieghbor must point to new free block*/

		if(local_next(i)!=0)			
		{
			i = local_next(i);
			local_prev(i)=new_block;
		}
		
		return(TRUE);
	}
	else if(free_size >= nbytes)
	{
		local_status(i)=RESERVED;
		return(TRUE);
	}

	return(FALSE);

}
/*end currentNodeAlloc*/
static inline unsigned int my_alloc(unsigned int heap,unsigned int nbytes)
{
	int ret;
	unsigned int i;
 //printf("alloc %x\n",nbytes);
	nbytes = ((nbytes  + MIN_FREE_BYTES - 1)/ MIN_FREE_BYTES )  * MIN_FREE_BYTES;
	i=local_first(heap);
	if(local_status(i)==FREE)
	{
		ret = currentNodeAlloc(heap,i,nbytes);
		if(ret==TRUE)
		{
			//F("alloc mem = 0x%x,size = %d\n",i+SIZE_HEADER,nbytes); 
			return(i+SIZE_HEADER);
		}
	}
#if VALIDINDEX 
	unsigned int old_addr = i;
	
#endif
	while(local_next(i)!=0)
	{
#if VALIDINDEX 
		if(i < 0x80000000)
		{
			printf("error address *[0x%x] = %x\n",i,local_next(i));
			printf("up address *[0x%x] = %x\n",old_addr,local_next(old_addr));
	  }
		old_addr = i; 	
#endif
		
		i=local_next(i);
		if(local_status(i)==FREE)
		{
			ret = currentNodeAlloc(heap,i,nbytes);
			if(ret==TRUE)
			{
				//F("alloc mem = 0x%x,size = %d\n",i+SIZE_HEADER,nbytes); 
				return(i+SIZE_HEADER);
			}
		}
	}
	printf("mem too small!\n");
	return 0;
}


static inline unsigned int Local_Alloc(unsigned int heap,unsigned int nbytes)
{
	
	unsigned int i = my_alloc(heap,nbytes);
	if(i != 0)
		local_setalign(i,0);    
	return i;
}
/*end Local_Alloc*/

static inline void my_free(unsigned int heap,unsigned int address)
{
	unsigned int block;
	unsigned int lblock;
	unsigned int rblock;
  
	block = address-SIZE_HEADER;
	lblock = local_prev(block);
	rblock = local_next(block);

	/*
	4 cases: FFF->F, FFR->FR, RFF->RF, RFR 
	always want to merge free blocks 
	*/

	if((lblock!=0)&&(rblock!=0)&&(local_status(lblock)==FREE)&&(local_status(rblock)==FREE))
	{
		local_next(lblock)=local_next(rblock);
		local_status(lblock)=FREE;
		if(local_next(rblock)!=0){ local_prev(local_next(rblock))=lblock; }
	}
	else if((lblock!=0)&&(local_status(lblock)==FREE))
	{
		local_next(lblock)=local_next(block);
		local_status(lblock)=FREE;
		if(local_next(block)!=0){ local_prev(local_next(block))=lblock; }
	}
	else if((rblock!=0)&&(local_status(rblock)==FREE))
	{
		local_next(block)=local_next(rblock);
		local_status(block)=FREE;
		if(local_next(rblock)!=0){ local_prev(local_next(rblock))=block; }
	}
	else{ local_status(block)=FREE; }

}


/*Note: disaster will strike if fed wrong address*/

static inline void Local_Dealloc(unsigned int heap,unsigned int address)
{
	
	
	/*
	4 cases: FFF->F, FFR->FR, RFF->RF, RFR 
	always want to merge free blocks 
	*/
  address -= local_getalign(address);
	my_free(heap,address);
	return;

}
/*end Local_Dealloc*/




static inline unsigned int Local_Realloc(unsigned int heap,unsigned int address,unsigned int nbytes)
{
	unsigned int rr,addr,oldsize;
	unsigned int block,rblock,rrblock;
	unsigned int bsize,align;
	unsigned int len;
  
	oldsize = nbytes;
	
	nbytes = ((nbytes  + MIN_FREE_BYTES - 1)/ MIN_FREE_BYTES )  * MIN_FREE_BYTES;
 
	rr = address;
	if (nbytes == 0) {
			Local_Dealloc(heap,rr);
			return 0;
  }
	if (address == 0)
	{	
	    addr = my_alloc(heap,nbytes);
	    if(addr != 0)
	    	local_setalign(addr,0);
      return addr;
	}
	align = local_getalign(address);
	len = (nbytes + align  + MIN_FREE_BYTES - 1) &(~(MIN_FREE_BYTES - 1));

	address -= local_getalign(address);
	address -= SIZE_HEADER;    
	bsize = local_size(address);
	//printf("align = %d,address = %x  %d %d\n",align,address,nbytes,bsize);
	if(nbytes <= bsize-align)
	{
		return rr;
	}	
	
	rblock = local_next(address);
	if((rblock != 0) &&(local_status(rblock) == FREE) )
	{
		//printf("rblock = %x %d %d",rblock,local_status(rblock),size(rblock));
		bsize += local_size(rblock);
		if(bsize >= nbytes + align)
		{
			rrblock = local_next(rblock);
			local_next(address) = address + len + SIZE_HEADER;
			block = local_next(address);
			local_prev(block) = address;
			local_next(block) = rrblock; 
			if(rrblock)
				local_prev(rrblock) = block; 
			local_status(block) = FREE;
	
			return rr;
	  }
	}   
	addr = my_alloc(heap,len);
	//printf("realloc %x %x %x %x\n",addr,rr,nbytes,bsize);
	
	if(addr == 0)
		return 0;
  addr += align;		
	local_setalign(addr,align);	
	memcpy((void *)addr,(void *)rr,bsize);
	Local_Dealloc(heap,rr);
	
  return addr;	

}


static inline unsigned int Local_Calloc(unsigned int heap,unsigned int size,unsigned int n)
{
	unsigned int rr;
	rr = Local_Alloc(heap,size * n);
	if(rr != 0)
		memset((void *)rr,0,size * n);
	return rr;
}

static inline unsigned int Local_alignAlloc(unsigned int heap,unsigned int align,unsigned int size)
{
	unsigned int i2 = 0, i = my_alloc(heap,size + align);
	if(i != 0)
	{
		i2 = (i +  align - 1) & ~(align - 1);
		local_setalign(i2, i2 - i);
	}
	
	return i2;
}
/*end alloc*/
//-----------------------------------------------------------------------------
//  Budyy Alloc 
//-----------------------------------------------------------------------------
///////////////////////////////////////
# define  ORDER     7   // 32K / 32 = 2^10 ;   32K=2^15 so ORDER+MINORDER=15
#define BUDDY_BLOCK_SIZE  (32 * (1 << (ORDER)))
# define  MINORDER	 5   //miniorder should big >=4 ; 32=2^5 so MINORDER=5

struct BuddyInfo{
	int freemem;    
 	int offset;
 	int reserver[(32 - 8)/4];	
};	
typedef struct BuddyInfo BuddyInfo;
//分配出去内存块块头信息
struct TBuddyHeader{
	BuddyInfo * buddyinfo;
	unsigned int reserve[2];
	unsigned int status;        //块的状态
	unsigned int k;            //块的大小信息:k,实际大小k<<2
	unsigned int align;
	unsigned int size;
	unsigned short rest;
	unsigned char res;
	unsigned char offset;
};
typedef struct TBuddyHeader TBuddyHeader;
struct TBuddyBlock;
typedef struct TBuddyBlock TBuddyBlock;
    //空闲块块头信息
struct TBuddyBlock{
		BuddyInfo * buddyinfo;
    unsigned int reserve[2];
    unsigned int status;
    unsigned int k;   
    TBuddyBlock* prev;
    TBuddyBlock* next;
    unsigned int reserve1[1];
};

#define BUDDYMAX  ((1<<(ORDER+MINORDER))-sizeof(TBuddyHeader))


static TBuddyBlock * freelist[ORDER+1] = {0};
#define BUDDY_DEBUG 0
#if BUDDY_DEBUG
static unsigned int freemem = 0;
static unsigned int orderinfo;
#endif 
/*
     reserver 16byte
     0 - 3       buddyinfo
     4 - 7       status:1;        //块的状态
								 k:31;            //块的大小信息:k,实际大小k<<2
	   8 - 11      align;
	   12 - 15     buddy to first addree size;
*/    
    
//buddy块,将地址的第k位反转
static inline	TBuddyBlock* Buddy(TBuddyBlock* p,int offset)
{
	unsigned int a;
	a = (unsigned int)p;
	a -= offset;
	a ^= 1<<(p->k);
	a += offset;

	return (TBuddyBlock*)a;
}
static inline	TBuddyBlock* Buddy2(TBuddyBlock* p,int offset,int k)
{
	unsigned int a;
	a = (unsigned int)p;
	a -= offset;
	a ^= 1<<k;
	a += offset;

	return (TBuddyBlock*)a;
}
static inline int order(unsigned int bytes)
{	
	int i = MINORDER;
	bytes--;
	while(bytes >> MINORDER)
	{
		bytes = bytes >> 1;
		i++;
	}
	return i;
}

static inline void push(TBuddyBlock *buddy)
{
	buddy->next = freelist[buddy->k - MINORDER];
	if(buddy->next)
		freelist[buddy->k - MINORDER]->prev = buddy;
#if BUDDY_DEBUG
	else
		orderinfo ^= 1<<buddy->k;
		printf("orderinfo=%x\n",orderinfo);
#endif
	buddy->prev = NULL;
	freelist[buddy->k - MINORDER] = buddy;
	//printf("push buddy->k = %d %x\n",buddy->k,freelist[buddy->k-MINORDER]);
}
static inline void pop(TBuddyBlock *buddy)
{
	//printf("pop buddy->k = %d %x\n",buddy->k,freelist[buddy->k-MINORDER]);
	freelist[buddy->k-MINORDER] = buddy->next;
	if(buddy->next)
		buddy->next->prev = NULL;
	else
	{	
#if BUDDY_DEBUG		
		orderinfo ^= 1<<(buddy->k);
		printf("orderinfo=%x\n",orderinfo);
#endif
	}
}
static inline void del(TBuddyBlock *buddy)
{
	if(buddy->prev)
	{	
		if(buddy->next)
		{
			printf("del-1,%d\n",buddy->k);
			buddy->next->prev = buddy->prev;
			buddy->prev->next = buddy->next;
		}
		else
		{
			printf("del-2,%d\n",buddy->k);
			buddy->prev->next = NULL;
			
			
		}
	}
	else 
	{
		if(buddy->next)
		{
			printf("del-3,%d\n",buddy->k);
			buddy->next->prev = NULL;
			freelist[buddy->k-MINORDER] = buddy->next;
		}
		else
		{
			printf("del-4,%d\n",buddy->k);
			freelist[buddy->k-MINORDER] = NULL; 

#if BUDDY_DEBUG
			orderinfo ^= (1<<buddy->k);
			printf("orderinfo=%x\n",orderinfo);
#endif

		}
		buddy->prev = buddy->next = NULL;
		//printf("del buddy->k = %d %x\n",buddy->k,freelist[buddy->k-MINORDER]);
	
	}
	
}
static inline void* buddyAlloc(unsigned int heap,int nbytes)
{

	printf("+++%s %d\n",__FUNCTION__,nbytes);
	unsigned int k = order(nbytes + sizeof(TBuddyBlock));
	TBuddyBlock* block;
	unsigned int i,j;
	for(i = (k-MINORDER); i<=ORDER && freelist[i]==NULL; i++);//寻找空闲块
	if(i > (ORDER)){
	    unsigned int buddystart = (unsigned int)Local_Alloc(heap,((1<<(ORDER+MINORDER)) + sizeof(BuddyInfo)) * 16);
  		if(buddystart == 0) return 0;
  		
  		TBuddyBlock* rawmem;
			TBuddyBlock* rawmem_last;
  		unsigned int temp = (1<<(ORDER+MINORDER))-1;
  		for(j = 0;j< 16;j++)
  		{
  			rawmem = (TBuddyBlock *)(buddystart+sizeof(BuddyInfo) + j*((1<<(ORDER+MINORDER)) + sizeof(BuddyInfo)));
	  		
	  		rawmem->buddyinfo = (BuddyInfo * )(buddystart + j*((1<<(ORDER+MINORDER)) + sizeof(BuddyInfo)));
	  		rawmem->status = 0;
	  		rawmem->k      =  ORDER+MINORDER;
	 			rawmem->buddyinfo->offset =(unsigned int)(rawmem) & temp;
	 			if(j == 0)
	 			{	
	 				rawmem->prev   = rawmem->next = NULL;
	 				rawmem_last = rawmem;
	 				freelist[ORDER] = rawmem;    //第一个大块放入m-freelist
  	  	}
  	  	else
  	  	{
  	  		rawmem->next = NULL;
  	  		rawmem->prev = rawmem_last;
  	  		rawmem_last->next = rawmem;
  	  		rawmem_last = rawmem;
  	  	}	
  	  }
  		
#if BUDDY_DEBUG			
  		freemem += (1<<(ORDER+MINORDER));
  		printf("freemem=%x\n",freemem);
#endif
			block = freelist[ORDER];
			
			//--i;
#if BUDDY_DEBUG			
			orderinfo ^= 1<<(i+MINORDER);
			printf("orderinfo=%x\n",orderinfo);
			
#endif
	}
	else
		block = freelist[i];

  printf("+++block = %x %x %d\n",block,block->buddyinfo,k);
	pop(block);
	printf("---block = %x\n",block);
#if BUDDY_DEBUG	
	freemem -= (1<<k);
	printf("freemem=%x\n",freemem);
#endif
	while(block->k > k)
	{
		block->k--;
		printf("block->buddyinfo->offset = %x %x\n",block->buddyinfo->offset,block->k);
		TBuddyBlock* buddy = Buddy(block,block->buddyinfo->offset);
		buddy->buddyinfo = block->buddyinfo;
		buddy->k = block->k;
		buddy->status = 0;
    printf("+++push\n");  
		//将buddy块插入到空闲链表
		push(buddy);
		printf("---push\n");
	}
  
	block->status = 1;
	//printf("block = 0x%x\n",&(block->status));
	TBuddyHeader *temp = (TBuddyHeader *)block;
	temp->offset = sizeof(TBuddyHeader);
	temp->size = nbytes;
	temp->rest = (1 << (temp->k))- nbytes - sizeof(TBuddyHeader);
	temp->align = 1;
	//printf("---%s\n",__FUNCTION__);
	return (void*)((char*)block+sizeof(TBuddyHeader));
}

static inline void  buddyFree(unsigned int heap,void* addr)
{
	TBuddyBlock* block = (TBuddyBlock*)((char*)addr - *((unsigned char*)addr-1) );
   printf("+++%s  -> %x\n",__FUNCTION__,(unsigned int)addr);
 
    TBuddyBlock* p;// = block;
#if BUDDY_DEBUG    
    freemem += 1<<(block->k);
    printf("freemem=%x\n",freemem);
#endif

		if(block->status == 0) 
		{
				printf("repeat free %x\n",(unsigned int)addr);
				*(unsigned int *)0x80000001 = 1;
    }
    block->status = 0;
    for(p = block; p->k < (ORDER+MINORDER); p->k++)
	{
        TBuddyBlock* buddy = Buddy(p,block->buddyinfo->offset);
		if( (buddy->status == 1)||(buddy->k != p->k) )
            break;       
		del(buddy);//删除buddy块
               
        if(buddy < p)//p指向合并后的块头
		{
            p = buddy;
        }
  }
  #if 0
	if((MINORDER+ORDER) - p->k)
	{
	#endif
		push(p);//将p插入到p->k空闲链表
	#if 0
	}
	else
	{
		//printf("free os! %x\n",(unsigned int)p->buddyinfo);
		Local_Dealloc(heap,(unsigned int)p->buddyinfo);
#if BUDDY_DEBUG
		freemem -= 1<<(ORDER+MINORDER);
		printf("freemem=%x\n",freemem);
#endif
	}
	#endif
	//printf("---%s\n",__FUNCTION__);
	
    return;
}

static inline void *buddyAlignalloc(unsigned int heap,unsigned int  align,unsigned int size)
{
	printf("size = %d %d\n",size,align);
	unsigned int i2 = 0,i = (unsigned int)buddyAlloc(heap,size + align);
	if(i != 0)
	{
		
		printf("i = %x\n",i);
		TBuddyHeader * temp=(TBuddyHeader *)(i - sizeof(TBuddyHeader));
		temp->size = size;
		i2 = (i +  align - 1) & ~(align - 1);
		(*((unsigned char *)(i2-1))) = i2 - i + sizeof(TBuddyHeader);//useless 
		//((TBuddyHeader *)(i-sizeof(TBuddyHeader)))->align=align;
		temp->align=align;
		temp->rest += (align - (i2-i));
	}
	return (void*)i2;
}

static inline void *buddyCalloc(unsigned int heap,size_t num,size_t size)
{
	int temp = num*size;
	char *p = buddyAlloc(heap,temp);
	if(p)
		memset(p,0,temp);
	return p;
}
static inline void *buddyRealloc(unsigned int heap,void *memblock, size_t size)
{
	TBuddyBlock* p =(TBuddyBlock*)((char*)memblock - *((unsigned char*)memblock - 1));
	TBuddyBlock* buddy;
	unsigned int k = p->k;
	
	void * r;
	
	TBuddyHeader * b=(TBuddyHeader *)p;
	//printf("dd\n");
	int temp = (int)(b->size + b->rest) - (int)size;
	
	if( temp >= 0)
	{
			b->size = size;
			b->rest = (unsigned short)temp;

			return memblock;
	}
	else
		temp=0-temp;
  while((temp > 0) && (k < (ORDER + MINORDER - 1)))
	{ 
		buddy = Buddy2(p,p->buddyinfo->offset,k);
		if( (buddy->status == 0)&&(buddy > p)&&(buddy->k == k))
		{
			temp -= (1<<k);
			k++;
			//printf("buddyRealloc a: %d\n",k);
		
		}
		else
			break;
	}
	if (temp>0)
	{	
		unsigned int length = size;
		r = 0;
		if(length < (1<<(ORDER+MINORDER-1)))
		{
			r = buddyAlignalloc(heap,((TBuddyHeader*)p)->align,length);
			
		}
		if(r == 0)
		{
			r = (void *)Local_alignAlloc(heap,((TBuddyHeader*)p)->align,length);
		}
		memcpy(r,memblock,b->size);
		buddyFree(heap,memblock);
		return r;
	}
	else
	{
		unsigned int a = k;
		while(k > p->k)
		{
			buddy = Buddy2(p,p->buddyinfo->offset,--k);			
			//printf("cc k = %d \n",k);
			del(buddy);//删除buddy块
#if BUDDY_DEBUG
			freemem -= 1<<buddy->k;
			printf("freemem=%x\n",freemem);
#endif

		}		
		p->k=a;
		b->size=size;
		b->rest=0-temp;
		
		//printf("buddyRealloc a: 0x%x\n",memblock);
		return memblock;
	
	}
 	return 0;
}
