#include "../jz4750e_dblk.h"

void filter_mb_dblk(H264_MB_DecARGs * dMB_aux, int mb_x, int mb_y, uint8_t *img_y, uint8_t *img_cb, uint8_t *img_cr, unsigned int linesize, unsigned int uvlinesize) ;
void filter_mb_hw(unsigned char *img_y, unsigned char *img_cb, unsigned char *img_cr, unsigned int linesize, unsigned int uvlinesize, int qp, int qp_u, int qp_v, unsigned int *bsary, unsigned int upen, unsigned int leften, int ofst_a, int ofst_b);
