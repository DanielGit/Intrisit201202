#ifndef __VAE_MAP_H__
#define __VAE_MAP_H__
extern unsigned char *ipu_base;
extern unsigned char *mc_base;
extern unsigned char *me_base;
extern unsigned char *dblk_base;
extern unsigned char *idct_base;

void VAE_map();
void VAE_unmap();
#endif /*__VAE_MAP_H__*/
