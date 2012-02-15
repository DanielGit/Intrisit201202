/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H264)
 *
* $Id: vc1_aux_mc_hw.h,v 1.3 2009/07/03 07:14:21 gwliu Exp $
 *
 ****************************************************************************/

#ifndef __VC1_MC_HW_H__
#define __VC1_MC_HW_H__

#if 0
typedef struct VC1_MC_DesNode{
  /*Y node*/
  uint32_t YNodeFlag;
  uint32_t YNodeHead;
  uint32_t YStride;   /*Y {reference frame, recon buffer} stride*/
  uint32_t YTap2Coef; /*second stage filter's tap coeffient*/
  uint32_t YTap1Coef; /*first stage filter's tap coeffient*/
  uint32_t YIntpInfo; /*interpolate information*/
  uint32_t YBLKInfo;  /*block information*/
  uint32_t YCurrAddr; /*current block address*/
  uint32_t YRefAddr;  /*reference address*/
  uint32_t YMCStatus; /*MC status*/
  /*U node*/
  uint32_t UNodeFlag;
  uint32_t UNodeHead;
  uint32_t UStride;
  uint32_t UTap2Coef;
  uint32_t UTap1Coef;
  uint32_t UIntpInfo;
  uint32_t UBLKInfo;   
  uint32_t UCurrAddr;
  uint32_t URefAddr;   
  uint32_t UMCStatus;
  /*V node*/
  uint32_t VNodeFlag;
  uint32_t VNodeHead;
  uint32_t VCurrAddr;
  uint32_t VRefAddr; 
  uint32_t VMCStatus;
}VC1_MC_DesNode;

#else
typedef struct VC1_MC_DesNode{
  /*node*/  
  uint32_t NodeFlag;
  uint32_t NodeHead;
  uint32_t Stride;   /*Y {reference frame, recon buffer} stride*/
  uint32_t Tap2Coef; /*second stage filter's tap coeffient*/
  uint32_t Tap1Coef; /*first stage filter's tap coeffient*/
  uint32_t IntpInfo; /*interpolate information*/
  uint32_t BLKInfo;  /*block information*/
  uint32_t CurrAddr; /*current block address*/
  uint32_t RefAddr;  /*reference address*/
  uint32_t MCStatus; /*MC status*/
}VC1_MC_DesNode;

#endif
# define VC1_YUHEAD 0x2008004d
# define VC1_YUHEAD_UNLINK 0x2008004c
# define VC1_MC_SYNC_HEAD 0x10064
# define VC1_VHEAD_LINK 0x0c03004d
# define VC1_VHEAD_UNLINK 0x0c03004c
# define VC1_VHEAD_SYNC 0x0c030048
#define TCSM_MC_CHAIN0 0xF4000000

  
#endif /*__VC1_MC_HW_H__*/
