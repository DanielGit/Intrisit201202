#include "../jz4750e_idct.h"
#include "h264_tcsm.h"

#undef printf
void h264_idct_error_add_hw (uint8_t *dst, short *error,
			     int dst_stride, int err_stride )
{
    int i;
    uint8_t *block = dst;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    //    printf("ERR ADD: 0x%08x\n",error);
    for ( i =0 ; i < 4 ; i ++ ) {
      //  printf("%x,%x,%x,%x\n", error[0 + i*err_stride], error[1 + i*err_stride], error[2 + i*err_stride], error[3 + i*err_stride] );      
      //    printf("%x\t,%x\t,%x\t,%x\n", error[i + 0*err_stride], error[i + 1*err_stride], error[i + 2*err_stride], error[i + 3*err_stride] );      
    //printf("%x,%x,%x,%x\n", block[i+0*dst_stride],block[i+1*dst_stride],block[i+2*dst_stride],block[i+3*dst_stride]);      
    dst[i + 0*dst_stride]= cm[ error[i + 0*err_stride] + block[i+0*dst_stride]];
    dst[i + 1*dst_stride]= cm[ error[i + 1*err_stride] + block[i+1*dst_stride]];
    dst[i + 2*dst_stride]= cm[ error[i + 2*err_stride] + block[i+2*dst_stride]];
    dst[i + 3*dst_stride]= cm[ error[i + 3*err_stride] + block[i+3*dst_stride]];
    }
}

static inline void h264_idct_init(H264Context *h)
{
  unsigned int  *ptr;
  int i ;
  ptr = (unsigned int )TCSM_IDCT_DES_BUF0;

  enable_idct() ;
  fresh_idct(1) ;
  fresh_idct(0) ;
  set_type_video(1, //video
		 1    //type
		 ) ;
  set_over_flow_protect(0/*over_flow_sel*/);
  
  set_stride((unsigned int)8,
             (unsigned int)8
             );

  desp_enable_idct();
  //set_ddma_dha(TCSM1_IDCT_DES_BUF0);
}

#define IDCT_NODE_LEN 4
#define IDCT_NODE_CMD 1
#define IDCT_UNLINK_HEAD 0x1402004c
#define IDCT_LINK_HEAD  0x1402004d
#define IDCT_END_HEAD 0x10024//fix lhuang

#define hw_idct_descrip_nod_conf(dct_src, dct_ptr)	\
({                                  \
idct_chain_tab[0]= 0x80000000;  \
idct_chain_tab[1] = IDCT_LINK_HEAD ; \
idct_chain_tab[3] = dct_src; \
idct_chain_tab[2] = dct_ptr ; \
 })

#define hw_run_idct_c() \ 
({clean_end_flag();	\
  set_idct_dcs();	\
})

#define hw_wait_idct_c() \
({   do { \
} while (!polling_end_flag() ) ; \
})
