#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define IPU__OFFSET 0x13080000
#define ME__OFFSET 0x130A0000
#define MC__OFFSET 0x13090000
#define DBLK__OFFSET 0x130B0000
#define IDCT__OFFSET 0x130C0000

#define IPU__SIZE   0x00001000
#define ME__SIZE   0x00001000
#define MC__SIZE   0x00001000
#define DBLK__SIZE   0x00001000
#define IDCT__SIZE   0x00001000

int vae_fd;
int tcsm_fd;
volatile unsigned char *ipu_base;
volatile unsigned char *mc_base;
volatile unsigned char *me_base;
volatile unsigned char *dblk_base;
volatile unsigned char *idct_base;

void VAE_map() {
	/* open and map flash device */
	vae_fd = open("/dev/mem", O_RDWR);
	// tricky appoach to use TCSM
	tcsm_fd = open("/dev/tcsm", O_RDWR);
	if (vae_fd < 0) {
	  printf("open /dev/mem error.\n");
	  exit(1);
	}
	if (tcsm_fd < 0) {
	  printf("open /dev/tcsm error.\n");
	  exit(1);
	}
	ipu_base = mmap((void *)0, IPU__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vae_fd, 
			IPU__OFFSET);
	mc_base = mmap((void *)0, MC__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vae_fd, 
			MC__OFFSET);
	me_base = mmap((void *)0, ME__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vae_fd, 
			ME__OFFSET);
	dblk_base = mmap((void *)0, DBLK__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vae_fd, 
			DBLK__OFFSET);
	idct_base = mmap((void *)0, IDCT__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vae_fd, 
			IDCT__OFFSET);	
	printf("VAE mmap successfully done!\n");
}

void VAE_unmap() {
  munmap(ipu_base, IPU__SIZE);
  munmap(mc_base, MC__SIZE);
  munmap(me_base, ME__SIZE);
  munmap(dblk_base, DBLK__SIZE);
  munmap(idct_base, IDCT__SIZE);
  close(vae_fd);
  close(tcsm_fd);
}
