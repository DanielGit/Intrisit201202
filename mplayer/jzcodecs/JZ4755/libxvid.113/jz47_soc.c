#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "sys/ioctl.h"
#include "sys/mman.h"
#include "fcntl.h"
#include <linux/fb.h>
#include <unistd.h>

#undef printf
//#define USE_FIX_4M
#ifndef USE_FIX_4M
#define USE_IMEM_ALLOC 
#endif

#define PAGE_SIZE (4096)
#define MEM_ALLOC_DEV_NUM 2

struct mem_dev {
  unsigned int vaddr;
  unsigned int paddr;
  unsigned int totalsize;
  unsigned int usedsize;
} memdev[MEM_ALLOC_DEV_NUM]; 

#ifdef USE_IMEM_ALLOC 
char *memfname[]={"/proc/jz/imem", 
                  "/proc/jz/imem",
                  "/proc/jz/imem", 
                  "/proc/jz/imem"};
#else
char *memfname[]={"/proc/jz/ipu", 
                  "/proc/jz/ipu",
                  "/proc/jz/ipu", 
                  "/proc/jz/ipu"};
#endif

unsigned int dcache[4096];
static int mem_init = 0, memdev_count = 0;
int mmapfd = 0;
volatile unsigned char *frame_buffer=NULL;
unsigned int fb_w, fb_h, phy_fb, fbmemlen, fb_line_len;

void get_phy_addr ()
{
}

void get_fbaddr_info ()
{
  int fbfd;
  struct fb_var_screeninfo fbvar;
  struct fb_fix_screeninfo fbfix;

  if ((fbfd = open("/dev/fb0", O_RDWR)) == -1) {
       printf("++ ERR: can't open /dev/fb0 ++\n");
       exit(1);
  }

  ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
  ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);

  fb_w = fbvar.xres;
  fb_h = fbvar.yres;

  phy_fb= fbfix.smem_start;
  fbmemlen = fbfix.smem_len;
  fb_line_len=  fbfix.line_length; 

  if ((frame_buffer = (unsigned char *) mmap(0, fbmemlen, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0)) == (unsigned char *) -1) 
  {
    printf ("++ jz4740_soc.c: Can't mmap frame buffer ++\n");
    exit (1);
  }

  printf ("++ jz47 fb_w=%d, fb_h=%d, phy_fb=0x%x, fbmemlen=%d, fb_line_len=%d\n",
          fb_w, fb_h, phy_fb, fbmemlen, fb_line_len);
}

#if 0
unsigned int get_phy_addr (unsigned int vaddr)
{
  int i;
  for (i = 0; i < memdev_count; i++)
  {
    if (vaddr >= memdev[i].vaddr && vaddr < memdev[i].vaddr + memdev[i].totalsize)
      return memdev[i].paddr + (vaddr -  memdev[i].vaddr);
  }
  return 0;
}
#endif

void jz4740_free_devmem()
{
  munmap((void *)frame_buffer, fbmemlen);
  mem_init = 0;
  memdev_count = 0;
}

void *jz4740_alloc_frame (int align, int size)
{
	int power, pages = 1, fd, i;
	unsigned int paddr, vaddr, data;

	/* Free all proc memory. */
	for (i = 0; i < memdev_count; i++) {
		int alloc_size, used_size, total_size;
		used_size = memdev[i].usedsize;
		total_size = memdev[i].totalsize;
		alloc_size = total_size - used_size;
		if (alloc_size >= size) {
			/* aligned to align boundary.  */
			alloc_size = (size + align) & ~(align);
			memdev[i].usedsize = used_size + alloc_size;
			vaddr = memdev[i].vaddr + used_size;
			printf ("++ jz mem alloc: vaddr = 0x%x, paddr = 0x%x size = 0x%x +++++++++\n",vaddr, memdev[i].paddr + used_size, size);
			return (void *)vaddr;
		} else {
		  printf("Fatal error! exceed reserved space, exit imediately\n");
		  exit (1);
		}
	}

	{
	  get_fbaddr_info();
	  memdev[memdev_count].vaddr = frame_buffer;
	  memdev[memdev_count].paddr = phy_fb;
	  memdev[memdev_count].totalsize = 0x1000000;
	  memdev[memdev_count].usedsize = 0;
	  memdev_count++;
	  printf ("++ jz47 Dev alloc: vaddr = 0x%x, paddr = 0x%x size = 0x%x ++\n", 
		  vaddr, paddr, memdev[memdev_count].totalsize);
	}
	for (i = 0; i < memdev_count; i++) {
		int alloc_size, used_size, total_size;
		used_size = memdev[i].usedsize;
		total_size = memdev[i].totalsize;
		alloc_size = total_size - used_size;
		if (alloc_size >= size) {
			/* align to align boundary.  */
			alloc_size = (size + align) & ~(align);
			memdev[i].usedsize = used_size + alloc_size;
			vaddr = memdev[i].vaddr + used_size;
			printf ("++ jz mem alloc: vaddr = 0x%x, paddr = 0x%x size = 0x%x ++\n", vaddr, memdev[i].paddr + used_size, size);
			return (void *)vaddr;
		}
	}
	printf ("++ Jz47 alloc: memory allocated is failed (size = %d) ++\n", size);
	exit (1);
}

unsigned int xvid_auxcodes_len, xvid_aux_task_codes[3];
