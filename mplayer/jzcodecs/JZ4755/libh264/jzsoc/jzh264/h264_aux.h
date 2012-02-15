/*****************************************************************************
 *
 * JZC H264 Decoder Architecture
 *
 ****************************************************************************/

#ifndef __JZC_H264_ARCH_H__
#define __JZC_H264_ARCH_H__


#define __tcsm1_text __attribute__ ((__section__ (".tcsm1_text")))
#define __tcsm1_main __attribute__ ((__section__ (".tcsm1_main")))
#define __tcsm1_data __attribute__ ((__section__ (".tcsm1_data")))

#define AUX_VADDR    0xB3800000
#define AUX_START()               \
({                                \
  volatile unsigned int * addr;             \
  addr = (unsigned int *)AUX_VADDR;         \
  *addr = 2;                      \
})

#define TCSM1_VADDR(addr)            ((addr & 0x0000FFFF) | 0x93800000)
#define TCSM1_VADDR_UNCACHE(addr)    ((addr & 0x0000FFFF) | 0xB3800000)
#define TCSM1_VADDR_FOR_MC(addr)     ((addr & 0x0000FFFF) | 0x13800000)

/*Main-CPU <--> AUX-CPU SPINLOCK*/
#define SPINLOCK_CPU 0xB3800004
#define SPINLOCK_AUX 0x13800004

#define SPINLOCK_CPU_SET(fifo_sema)  ({while(fifo_sema[2]!=2);})
#define SPINLOCK_AUX_SET(fifo_sema)  ({while(fifo_sema[1]!=1);})
#define SPINLOCK_CLR(fifo_sema)      ({ fifo_sema[0] = 0; })


typedef struct H264_Frame_Ptr{
  uint8_t * y_ptr;
  uint8_t * u_ptr;
  uint8_t * v_ptr;
}H264_Frame_Ptr;

typedef struct H264_Frame_GlbARGs{
  uint8_t mb_width;
  uint8_t mb_height;
  uint8_t mb_stride;
  uint8_t picture_structure;
  uint8_t emu_edge_width;
  uint8_t emu_edge_height;
  uint16_t first_mb_in_frame;
  uint16_t linesize;
  uint16_t uvlinesize;
  uint8_t * dMB_baseaddr_aux;
  H264_Frame_Ptr current_picture;
  H264_Frame_Ptr ref_list[2][16];
}H264_Frame_GlbARGs;

typedef struct H264_MB_DecARGs{
  //Weighted pred stuff
  char use_weight;
  char use_weight_chroma;
  char luma_log2_weight_denom;
  char chroma_log2_weight_denom;
  char luma_weight[2][16];
  char luma_offset[2][16];
  char chroma_weight[2][16][2];
  char chroma_offset[2][16][2];
  char implicit_weight[16];//53 words
  // dmb_prev
  uint8_t dmb_len;
  // dmb_curr
  uint8_t mb_x;
  uint8_t mb_y;
  uint8_t mb_field_decoding_flag;
  uint8_t deblocking_filter;
  uint8_t next_mb_no_weight_flag;
  uint8_t dblk_start[2];//2 words
  unsigned int mb_type;
  unsigned int left_mb_type;
  unsigned int top_mb_type;
  uint16_t sub_mb_type[4]; // 2+5=7 words
  uint8_t qscale;
  uint8_t qp_left;
  uint8_t qp_top;
  uint8_t chroma_qp[2];
  uint8_t chroma_qp_left[2];
  uint8_t chroma_qp_top[2];
  uint8_t slice_alpha_c0_offset;
  uint8_t slice_beta_offset;
  uint8_t slice_type; // 7+3=10 words
  uint8_t non_zero_count_cache_luma[25];
  // indicate weather to transfer weight or residual part
  uint8_t next_mb_skip_flag;
  int8_t ref_cache[2][25];
  int16_t mv_cache[2][25][2]; // 10+50+19=79 words
  // dmb_future
  uint8_t non_zero_count_cache_chroma[8];
  uint8_t intra4x4_pred_mode_cache[16];
  uint8_t chroma_pred_mode;
  uint8_t intra16x16_pred_mode;
  uint8_t curr_mb_skip_flag;
  uint8_t dummy1[1];
  uint32_t dequant4_coeff[3];
  unsigned int topleft_samples_available;
  unsigned int topright_samples_available; // 79+12=91 words
  // rsidual
  short mb[24*16]; // 144+192=336 words
}H264_MB_DecARGs;

typedef struct H264_AUX_T{
  // linesize for mc
  unsigned char mc_des_dirty;
  int mb_linesize;
  int mb_uvlinesize;

  // reconstruction buffer
  uint8_t *h264_yrecon[2];
  uint8_t *h264_urecon[2];
  uint8_t *h264_vrecon[2];

  // for mc Bi-direction average/weight
  uint8_t *h264_ymcavg[2];
  uint8_t *h264_umcavg[2];
  uint8_t *h264_vmcavg[2];

  // dblk dst buffer
  uint8_t *h264_ydblk[2];

  // dblk des pointers
  int h264_dblk_des_ptr[2];

  // mc des pointers
  int h264_mc_des_ptr[2];

  //weight back up for next mb
  uint16_t sub_mb_type[4];

  // back-up-bottom buffer
  uint8_t (*BackupMBbottom_Y)[16];
  uint8_t (*BackupMBbottom_U)[8];
  uint8_t (*BackupMBbottom_V)[8];

}H264_AUX_T;

#endif /*__JZC_H264_ARCH_H__*/
