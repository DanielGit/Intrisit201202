
#ifndef __VC1_TCSM_H__
#define __VC1_TCSM_H__
#include "../jzsys.h"
#include "../jzasm.h"
  
/* TCSM space defined here*/
#define TCSM_BASE 0xF4000000
#define TCSM_BANK0 0xF4000000
#define TCSM_BANK1 0xF4001000
#define TCSM_BANK2 0xF4002000   
#define TCSM_BANK3 0xF4003000

#define TCSM1_BASE 0xF4008000
#define TCSM1_VBASE 0x93808000
#define TCSM1_BANK0 0xF4008000
#define TCSM1_BANK1 0xF4009000
#define TCSM1_BANK2 0xF400A000
#define TCSM1_BANK3 0xF400B000
#define TCSM1_BANK4 0xF400C000
#define TCSM1_BANK5 0xF400D000
#define TCSM1_BANK6 0xF400E000
#define TCSM1_BANK7 0xF400F000

#define TCSM1_VADDR(addr)          ((addr & 0x0000FFFF) | 0x93800000)
#define TCSM1_VADDR_UNCACHE(addr)  ((addr & 0x0000FFFF) | 0xB3800000)

#define SPACE_TYPE_CHAR 0x0
#define SPACE_TYPE_SHORT 0x1
#define SPACE_TYPE_WORD 0x2
#define MB_LUMA_WIDTH 16
#define MB_CHROM_WIDTH 8
#define MB_LUMA_EDGED_WIDTH (MB_LUMA_WIDTH + 4)
#define MB_CHROM_EDGED_WIDTH (MB_CHROM_WIDTH + 4)
#define VC1MP_MAX_FRAME_WIDTH 1280
#define DEFAULT_VC1_MAX_MBNUM1 1900     //3600  1280x720
#define DEFAULT_VC1_MAX_MBNUM 3600  //1280x720

#define JZC_CACHE_LINE 32
#define	ALIGN(p,n)     ((uint8_t*)(p)+((((uint8_t*)(p)-(uint8_t*)(0)) & ((n)-1)) ? ((n)-(((uint8_t*)(p)-(uint8_t*)(0)) & ((n)-1))):0))
#define TCSM1_ALN4(x)  ((x & 0x3)? (x + 4 - (x & 0x3)) : x)


#define PMON_BUF_LEN (4)
#define PMON_MC_BUF (TCSM1_BANK7)
#define PMON_IDCT_BUF (PMON_MC_BUF + PMON_BUF_LEN)
#define PMON_DBLK_BUF (PMON_IDCT_BUF + PMON_BUF_LEN)
#define PMON_INTRA_BUF (PMON_DBLK_BUF + PMON_BUF_LEN)
#define PMON_MDMA_BUF (PMON_INTRA_BUF + PMON_BUF_LEN)
#define PMON_SYNC_BUF (PMON_MDMA_BUF + PMON_BUF_LEN)

#define TCSM1_INSN_LEN 0x1000 /*suppose up-to 4bank fill instructions*/
#define TCSM1_TASK_LEN 0x1400 /*suppose up-to 5bank instructions and data*/


/*TCSM1 space for main-aux sync/handshake*/
#define TCSM1_ALN4(x)                ((x & 0x3)? (x + 4 - (x & 0x3)) : x)
#define AUX_COMD_BUF_LEN 64
#define TCSM1_FIFO_WP (TCSM1_BANK5)
#define TCSM1_FOFO_WP_VADDR TCSM1_VADDR_UNCACHE(TCSM1_FIFO_WP)
#define TCSM1_TASK_DONE (TCSM1_FIFO_WP+4)
#define TCSM1_TASK_DONE_VADDR TCSM1_VADDR_UNCACHE(TCSM1_TASK_DONE)
#define TCSM1_BLOCK_DONE (TCSM1_TASK_DONE+4)
#define TCSM1_BLOCK_DONE_VADDR TCSM1_VADDR_UNCACHE(TCSM1_BLOCK_DONE)
#define TCSM1_FIFO_RP (TCSM1_BLOCK_DONE+32)
#define TCSM1_FIFO_RP_VADDR TCSM1_VADDR_UNCACHE(TCSM1_FIFO_RP)

/*TCSM1 space for main cpu --> aux parameters transfer: dMB and dFRM*/
#define TCSM1_FRM_ARGS (TCSM1_FIFO_WP+AUX_COMD_BUF_LEN)
#define TCSM1_FRM_ARGS_VADDR TCSM1_VADDR_UNCACHE(TCSM1_FRM_ARGS)
#define FRM_ARGS_LEN (TCSM1_ALN4(sizeof(VC1_Frame_GlbARGs)))
#define TCSM1_MBARGS_BUF (TCSM1_FRM_ARGS+FRM_ARGS_LEN)
#define MBARGS_BUF_LEN (TCSM1_ALN4(sizeof(VC1_MB_DecARGs)))
#define TCSM1_MBARGS_BUF2 (TCSM1_MBARGS_BUF + MBARGS_BUF_LEN)

#define DMB_BUF_LEN MBARGS_BUF_LEN
#define dMB_CURR_ARG_LEN (96)
#define dMB_RESIDUAL_ARG_LEN (DMB_BUF_LEN - dMB_CURR_ARG_LEN)
/*AUX global struct*/
#define TCSM1_AUX_T (TCSM1_MBARGS_BUF2+MBARGS_BUF_LEN)
#define AUX_T_LEN (TCSM1_ALN4(sizeof(VC1_AUX_T)))

//GP1 chain(ARG used to comm dMB args, TRAN used for AUX MB transfer)
#define TCSM1_GP1_TRAN_CHAIN (TCSM1_AUX_T+AUX_T_LEN)
#define GP1_TRAN_CHAIN_LEN ((4*5)<<SPACE_TYPE_WORD)
#define TCSM1_GP1_TRAN_CHAIN2 (TCSM1_GP1_TRAN_CHAIN + GP1_TRAN_CHAIN_LEN)
#define TCSM1_GP1_ARG_CHAIN (TCSM1_GP1_TRAN_CHAIN2 + GP1_TRAN_CHAIN_LEN)
#define GP1_ARG_CHAIN_LEN ((4*1)<<SPACE_TYPE_WORD)

//MC chain
#define MC_CHAIN_LEN    ((12*10)<<SPACE_TYPE_WORD)
#define TCSM1_MC_CHAIN0  (TCSM1_GP1_ARG_CHAIN + GP1_ARG_CHAIN_LEN)
#define TCSM1_MC_CHAIN1  (TCSM1_MC_CHAIN0 +  MC_CHAIN_LEN)
     
//RECON/MC buffer  
#define RECON_U_OFFSET   (20)
#define RECON_V_OFFSET   (40)
#define RECON_BUF_STRD   (60)
#define RECON_BUF_LEN   (20 * 60)

#define TCSM1_YRECON_BUF (TCSM1_MC_CHAIN1 +  MC_CHAIN_LEN)
#define TCSM1_URECON_BUF (TCSM1_YRECON_BUF + RECON_U_OFFSET)
#define TCSM1_VRECON_BUF (TCSM1_YRECON_BUF + RECON_V_OFFSET)

#define TCSM1_YRECON_AVG_BUF (TCSM1_YRECON_BUF + RECON_BUF_LEN)
#define TCSM1_URECON_AVG_BUF (TCSM1_YRECON_AVG_BUF + RECON_U_OFFSET)   
#define TCSM1_VRECON_AVG_BUF (TCSM1_YRECON_AVG_BUF + RECON_V_OFFSET)

#define TCSM1_YRECON_XCH_BUF (TCSM1_YRECON_AVG_BUF + RECON_BUF_LEN)
#define TCSM1_URECON_XCH_BUF (TCSM1_YRECON_XCH_BUF + RECON_U_OFFSET)
#define TCSM1_VRECON_XCH_BUF (TCSM1_YRECON_XCH_BUF + RECON_V_OFFSET)

#define TCSM1_YRECON_XCH_AVG_BUF (TCSM1_YRECON_XCH_BUF + RECON_BUF_LEN)
#define TCSM1_URECON_XCH_AVG_BUF (TCSM1_YRECON_XCH_AVG_BUF + RECON_U_OFFSET)
#define TCSM1_VRECON_XCH_AVG_BUF (TCSM1_YRECON_XCH_AVG_BUF + RECON_V_OFFSET) 

#define TCSM1_YRECON_XCH_BUF1 (TCSM1_YRECON_XCH_AVG_BUF + RECON_BUF_LEN)
#define TCSM1_URECON_XCH_BUF1 (TCSM1_YRECON_XCH_BUF1 + RECON_U_OFFSET)   
#define TCSM1_VRECON_XCH_BUF1 (TCSM1_YRECON_XCH_BUF1 + RECON_V_OFFSET)
  
// MC EDGE EMULATE BUFFER
#define EMU_Y_W 24
#define EMU_Y_H 19
#define EMU_Y_4MV_W 12
#define EMU_Y_4MV_H 11
#define EMU_UV_W 12
#define EMU_UV_H 9

#define EMU_BUF_STRD (EMU_Y_W*EMU_Y_H + 2*EMU_UV_W*EMU_UV_H)
#define EMU_BUF_1MV_Y_STRD (EMU_Y_W*EMU_Y_H)
#define EMU_BUF_4MV_Y_STRD (EMU_Y_4MV_W*EMU_Y_4MV_H)
#define EMU_BUF_UV_STRD (EMU_UV_W*EMU_UV_H)

#define EMU_EDGE_LUMA_STRIDE 24
#define EMU_EDGE_CHROM_STRIDE 12 
#define TCSM1_EMU_EDGE_BUF  (TCSM1_YRECON_XCH_BUF1 + RECON_BUF_LEN)
#define EMU_EDGE_BUF_UCACHE (TCSM1_VADDR_UNCACHE(TCSM1_EMU_EDGE_BUF))

#ifdef JZC_AUX_RUN
#define EMU_EDGE_BUF TCSM1_EMU_EDGE_BUF
#else
#define EMU_EDGE_BUF EMU_EDGE_BUF_UCACHE
#endif //JZC_AUX_RUN

#define EMU_BUF_LEN (672)

#define DBLOCK_TOPBAK_BUF  (TCSM1_EMU_EDGE_BUF + EMU_BUF_LEN)
#define DBLOCK_TOPBUF_LEN  (2*1280)

#define DBLOCK_LEFTBAK_BUF  (TCSM1_EMU_EDGE_BUF + EMU_BUF_LEN + DBLOCK_BUF_LEN )
#define DBLOCK_LEFTBUF_LEN  (2*720)

#define TCSM0_DMB_BUF0  (TCSM_BANK0)
#define TCSM0_DMB_BUF1  (TCSM_BANK1)
   
#define TCSM0_LUTY_BUF  (TCSM0_DMB_BUF1 + sizeof(VC1_MB_DecARGs) + DMB_BUF_LEN)
#define TCSM0_LUTUV_BUF  (TCSM0_LUTY_BUF + 256)

#define TCSM0_BLOCK_DONE (TCSM0_LUTUV_BUF + 4)
#define TCSM0_TASK_DONE  (TCSM0_BLOCK_DONE + 4)  

//#define TCSM0_EMU_BUF0   (TCSM_BANK2)
//#define TCSM0_EMU_BUF1   (TCSM0_EMU_BUF0 + EMU_BUF_STRD)

//#define LUT_YUV_BUF      (TCSM_BANK2)
// AUX DEBUG
#define TCSM1_DBG_info (TCSM1_FIFO_WP - 32)

/*Reconstruction/Pixel Buffer:*/
/* Used for MC output. size: 16*56=896 Bytes */
/* 
   -- Buffers are ping-pong buffered
     +-----+-------------+-------------+-----+----------+----------+---------
     |     |             |             |     |          |          |       ^
     | (1) |  (3)        |  (4)        | (5) | (7)      | (8)      |       |
     |     |             |             |     |          |          |       |
     |     |  RECON      |  MC_AVG     |     | RECON    | MC_AVG   |       |
     |     |   Y         |   Y         |     |  U       |  U       |       |
     |     |             |             |     |          |          |
     |     |             |             |     |          |          |       16
     |     |             |             +-----+----------+----------+      Byte
     |     |             |             |     |          |          |
     |     |             |             | (9) | (11)     | (12)     |       |
     |     |             |             |     |          |          |       |
     |     |             |             |     | RECON    |  MC_AVG  |       |  
     |     |             |             |     |  V       |   V      |       |
     |     |             |             |     |          |          |       |
     |<4B> |<- 16Byte -> |<- 16Byte -> |<4B> |<-8Byte-> |<-8Byte-> |       
     +-----+-------------+-------------+-----+----------+----------+--------
*/


#endif /*__VC1_TCSM_H__*/
