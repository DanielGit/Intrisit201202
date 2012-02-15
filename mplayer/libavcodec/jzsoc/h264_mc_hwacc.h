/*****************************************************************************
 *
 * JZC MC HardWare Core Accelerate(H.264 Baseline-profile)
 *
* $Id: h264_mc_hwacc.h,v 1.2 2009/03/25 06:33:31 jyu Exp $
 *
 ****************************************************************************/

#ifndef __JZC_H264_MC_HWACC_H__
#define __JZC_H264_MC_HWACC_H__

typedef struct h264_mc_des_node{
  /*luma:*/
  int luma_head;
  int luma_ref_addr_i; /*_i stands for index*/
  int luma_ref_addr_d; /*_d stands for data*/
  int luma_ref2_addr_i; /*_i stands for index*/
  int luma_ref2_addr_d; /*_d stands for data*/
  int luma_cur_addr_i;
  int luma_cur_addr_d;
  int luma_blk_info_i;
  int luma_blk_info_d;
  int luma_itp_info_i;
  int luma_itp_info_d;
  int luma_cur_strd_i;
  int luma_cur_strd_d;
  int luma_ref2_strd_i;
  int luma_ref2_strd_d;
  int luma_tap_coef_i;
  int luma_tap_coef_d;
  int luma_mc_stat_i;
  int luma_mc_stat_d;
  /*chrom_cb:*/
  int chrom_cb_head;
  int chrom_cb_ref_addr_i;
  int chrom_cb_ref_addr_d;
  int chrom_cb_ref2_addr_i;
  int chrom_cb_ref2_addr_d;
  int chrom_cb_cur_addr_i;
  int chrom_cb_cur_addr_d;
  int chrom_cb_blk_info_i;
  int chrom_cb_blk_info_d;
  int chrom_cb_itp_info_i;
  int chrom_cb_itp_info_d;
  int chrom_cb_cur_strd_i;
  int chrom_cb_cur_strd_d;
  int chrom_cb_ref2_strd_i;
  int chrom_cb_ref2_strd_d;
  int chrom_cb_tap_reg1_i;
  int chrom_cb_tap_reg1_d;
  int chrom_cb_tap_reg3_i;
  int chrom_cb_tap_reg3_d;
  int chrom_cb_mc_stat_i;
  int chrom_cb_mc_stat_d;
  /*chrom_cr:*/
  int chrom_cr_head;
  int chrom_cr_ref_addr_i;
  int chrom_cr_ref_addr_d;
  int chrom_cr_ref2_addr_i;
  int chrom_cr_ref2_addr_d;
  int chrom_cr_cur_addr_i;
  int chrom_cr_cur_addr_d;
  int chrom_cr_mc_stat_i;
  int chrom_cr_mc_stat_d;
}h264_mc_des_node;

#endif /*__JZC_H264_MC_HWACC_H__*/
