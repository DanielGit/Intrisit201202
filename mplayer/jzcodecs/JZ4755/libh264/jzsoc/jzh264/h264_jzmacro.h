/*
  JZ SOC Macros are defined here
*/
#ifndef __H264_JZ_MACRO_H__
#define __H264_JZ_MACRO_H__

#define JZC_AUX_DBG

#define JZC_AUX_RUN

#define _UCOS_

#define AUX_VADDR_SPACE

#define AUX_EMULATE_CAPABLE
#define MOTION_NEXT_ADD
//#define H264_FIELD_ENABLE //support field decode

//#define JZC_AUX_PMON
#ifdef JZC_AUX_PMON
#define PMON_ONLY_CLK
#endif // JZC_AUX_PMON

//#define JZC_MC_OPT
//#define JZC_TCSM_OPT
//#define JZC_SHRINK_OPT
//#define JZC_MXU_OPT 

#endif /*__H264_JZ_MACRO_H__*/

