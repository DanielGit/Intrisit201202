typedef struct { 
  unsigned int header ;
  unsigned int reg_input ;
  unsigned int value_input ;
  unsigned int reg_output ;
  unsigned int value_output ;
  unsigned int reg_stride ;
  unsigned int value_stride ;  
} idct_chain_nod ;

void h264_idct_error_add_hw (uint8_t *dst, uint8_t *error, int idct_block, 
			     int dst_stride, int err_stride ){
    int i;
    DCTELEM *block = idct_block;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    for ( i =0 ; i < 4 ; i ++ ) {
    dst[i + 0*dst_stride]= cm[ error[i + 0*err_stride] + block[i+0*96]];
    dst[i + 1*dst_stride]= cm[ error[i + 1*err_stride] + block[i+1*96]];
    dst[i + 2*dst_stride]= cm[ error[i + 2*err_stride] + block[i+2*96]];
    dst[i + 3*dst_stride]= cm[ error[i + 3*err_stride] + block[i+3*96]];
    }
}

static inline void h264_idct_init(H264Context *h)
{
  unsigned int  *ptr;
  int i ;
  ptr = (unsigned int )TCSM_IDCT_DESC_TABLE;
  enable_idct() ;
  fresh_idct(1) ;
  fresh_idct(0) ;
  set_type_video(1, //video
		 1    //type
		 ) ;
  set_over_flow_protect(0/*over_flow_sel*/);
  desp_enable_idct();
  set_ddma_dha(TCSM_IDCT_DESC_TABLE);
  
  for ( i =0 ; i < H264_IDCT_MB_BLK_NUM ; i ++ ){
    ptr[0] = 0x31 ;
    ptr[1] = REG_INPUT_ADDR ;    
    ptr[2] = 0 ;    
    ptr[3] = REG_OUT_ADDR ;
    ptr[4] = 0;
    ptr[5] = REG_STR ;    
    ptr[6] = 0;
    ptr += H264_IDCT_DES_NODE_LEN ;
    idct_cbp [i] = 0 ;
  }
}

static inline void hw_idct_descrip_nod_conf(DCTELEM *dct_src, DCTELEM *dct_ptr, 
					    uint32_t *des_ptr) {
  des_ptr[0] = 0x31;
  des_ptr[2] = (int)dct_src;
  des_ptr[4] = (int)dct_ptr;
  des_ptr[6] = (4<<SPACE_TYPE_SHORT) /*in_stride*/ |	           
               ((96<<SPACE_TYPE_SHORT) << 16) /*out_stride*/;	   
}

#define hw_run_idct_c() \
({clean_end_flag(); \
run_idct(); \
})

#define hw_wait_idct_c() \
({   do { \
} while (!polling_end_flag() ) ; \
})
