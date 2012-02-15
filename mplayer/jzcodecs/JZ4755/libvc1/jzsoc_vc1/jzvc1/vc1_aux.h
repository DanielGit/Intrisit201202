/*****************************************************************************
 *
 * JZC VC1 Decoder Architecture
 *
 ****************************************************************************/

#ifndef __JZC_VC1_AUX_H__
#define __JZC_VC1_AUX_H__

  
#define __vc1_J1_tcsm_text__ __attribute__ ((__section__ (".vc1_J1_tcsm_text")))
#define __tcsm1_main __attribute__ ((__section__ (".tcsm1_main")))
//#define __tcsm1_data __attribute__ ((__section__ (".tcsm1_data")))

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

typedef struct VC1_Frame_Ptr{
  uint8_t * ptr[3];
}VC1_Frame_Ptr;

typedef struct VC1_Frame_GlbARGs{
  uint8_t mb_width;
  uint8_t mb_height;
  uint8_t mb_stride;
  char pq;

  uint8_t chroma_y_shift;
  uint8_t chroma_x_shift;
  uint8_t lowres;
  uint8_t draw_horiz_band;
  uint8_t picture_structure;
  
  uint8_t res_fasttx;   //
  uint8_t overlap;      // 
  uint8_t res_x8;       //
  uint8_t pict_type;    //
  uint8_t profile;      //
  uint8_t bi_type;      //
  uint8_t rangeredfrm;  //
  uint8_t rnd;
  uint8_t fastuvmc;
  uint8_t mspel;
  uint8_t use_ic;
  uint16_t flags;
  char scale;
  short int shift;
 
  uint8_t *last_data[4];
  uint8_t *next_data[4];
  uint8_t *curr_data[3];

  uint8_t * emu_edge_buf_aux;

  uint16_t h_edge_pos;
  uint16_t v_edge_pos;

  uint16_t coded_width;
  uint16_t coded_height;  

  uint16_t slinesize[3];  //
  uint16_t linesize;
  uint16_t uvlinesize;  

  uint8_t * dMB_baseaddr_aux;
  VC1_Frame_Ptr current_picture;
  VC1_Frame_Ptr current_picture_save;
}VC1_Frame_GlbARGs;

typedef struct VC1_MB_DecARGs{
  uint8_t vc1_fourmv;  //
  uint8_t vc1_skipped;  //
  uint8_t vc1_b_mc_skipped;
  uint8_t vc1_direct;  //
  uint8_t mv_mode;
  uint8_t mv_mode2;
  uint8_t mb_x;     
  uint8_t mb_y; 
  uint8_t vc1_overlap;       //
  uint8_t pintra;
  uint8_t bintra;
  uint8_t first_slice_line;
  uint8_t dmb_len;
  uint8_t bfmv_type;   //-
  //uint8_t dummy0[6];   //-20

  uint8_t vc1_blk_index[6];  //- 
  uint8_t vc1_mb_has_coeffs[2]; //
  uint8_t sintra[4];   //-32

  
  uint8_t subblockpat[6];  //-   
  uint8_t ttblock[6];    //44
  uint8_t vc1_intra[6];
  uint8_t pb_val[6];
  uint8_t nnz_flag;

  uint8_t idct_row[6];
  uint8_t idct_row_4x4[24];
  uint8_t idct_row_8x4[12];
  uint8_t dummy2[3];   //-64
 
  int vc1_mv[4][2]; //128
  
  DCTELEM mb[6*64];
}VC1_MB_DecARGs;


typedef struct VC1_AUX_T{
  // linesize for mc
  unsigned char mc_des_dirty;
  int mb_linesize;
  int mb_uvlinesize;

  // reconstruction buffer
  uint8_t *recon[3];
  uint8_t *recon_xchg[3];
  uint8_t *recon_xchg1[3];

  uint8_t *recon_avg[3];
  uint8_t *recon_xchg_avg[3];

  uint32_t mc_chain[2];

}VC1_AUX_T;

#endif /*__JZC_VC1_AUX_H__*/
   
