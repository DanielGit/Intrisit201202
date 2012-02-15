#ifndef __JZC_TCSM_H__
#define __JZC_TCSM_H__

/* TCSM space defined here*/
#define TCSM_BASE 0xF4000000
#define TCSM_BANK0 0xF4000000
#define TCSM_BANK1 0xF4001000
#define TCSM_BANK2 0xF4002000
#define TCSM_BANK3 0xF4003000


#define ALIGN4(x)                ((x & 0x3)? (x + 4 - (x & 0x3)) : x)
 
 

/* Type define */
#define SPACE_TYPE_CHAR 0x0
#define SPACE_TYPE_SHORT 0x1
#define SPACE_TYPE_WORD 0x2
#define MB_LUMA_SIZE 256
#define MB_CHROM_SIZE 64

/*VLC */
#define VLCINTRA_LEN 2048
#define VLCINTER_LEN 2048
/* TCSM0 space   */
#define TCSM0_VLCINTRA_ADDR TCSM_BANK0
#define TCSM0_VLCINTER_ADDR (TCSM0_VLCINTRA_ADDR + VLCINTER_LEN)

/*MC BUFFER*/
/*see struct RCON_IMAGE in deocder.c*/
#define RCON_MB_LUMA_STRIDE 16
#define RCON_MB_CHROMA_STRIDE 8
#define RCON_IMAGE_BUF0  TCSM_BANK1
#define RCON_IMAGE_BUF1  (RCON_IMAGE_BUF0+ALIGN4(sizeof(RCON_IMAGE)))

/*IDCT*/
/*6 8x8 blocks*/
#define IDCT_BUFFER_SIZE 6*64*2
#define TCSM_IDCT_BUFFER0 TCSM_BANK2
#define TCSM_IDCT_BUFFER1 (TCSM_IDCT_BUFFER0 +ALIGN4(IDCT_BUFFER_SIZE))

/*space for 256 zeros used by  memset*,see zero_table in decoder.c */
#define ZERO_TABLE_SIZE 256
#define TCSM_ZERO_TABLE   (TCSM_IDCT_BUFFER1 +ALIGN4(IDCT_BUFFER_SIZE))
/*MC discriptor*/
/*two mc dspcritptor lines*/
#define TCSM_MC_DSPLINE0 TCSM_BANK3
#define DESP_NODE_COUNT 12
#define TCSM_MC_DSPLINE1 (TCSM_BANK3 + ALIGN4(sizeof(RV_MC_DesNode)*DESP_NODE_COUNT))




#endif /*__JZC_TCSM_H__*/
