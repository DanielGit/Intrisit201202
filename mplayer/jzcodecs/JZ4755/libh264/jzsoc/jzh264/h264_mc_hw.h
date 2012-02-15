/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H264)
 *
* $Id: h264_mc_hw.h,v 1.2 2009/04/21 01:06:52 pwang Exp $
 *
 ****************************************************************************/

#ifndef __H264_MC_HW_H__
#define __H264_MC_HW_H__

typedef struct H264_MC_DesNode{
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
}H264_MC_DesNode;

# define H264_YUHEAD 0x2008004d
# define H264_MC_SYNC_HEAD 0x10064
# define H264_VHEAD_LINK 0x0c03004d
# define H264_VHEAD_UNLINK 0x0c03004c
# define H264_VHEAD_SYNC 0x0c030048


#endif /*__H264_MC_HW_H__*/
