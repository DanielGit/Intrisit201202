
#ifndef _JZ4750_ANDROID_IPU_H_
#define _JZ4750_ANDROID_IPU_H_

#define REG32(val)  (*((volatile unsigned int *)(val)))

// CLKGR
#define CPM_CLKGR         0x10000020
#define CPM_CLKGR_VADDR   0xB0000020

// Module for CLKGR
#define IDCT_CLOCK      (1 << 27)
#define DBLK_CLOCK      (1 << 26)
#define ME_CLOCK        (1 << 25)
#define MC_CLOCK        (1 << 24)
#define IPU_CLOCK       (1 << 13)

// IPU_REG_BASE
#define IPU_P_BASE  0x13080000
#define IPU__OFFSET 0x13080000
#define IPU__SIZE   0x00001000

#define ME__SIZE   0x00001000
#define MC__SIZE   0x00001000
#define DBLK__SIZE   0x00001000
#define IDCT__SIZE   0x00001000


#define ME__OFFSET 0x130A0000
#define MC__OFFSET 0x13090000
#define DBLK__OFFSET 0x130B0000
#define IDCT__OFFSET 0x130C0000

struct ipu_module
{
  unsigned int reg_ctrl;      // 0x0
  unsigned int reg_status;    // 0x4
  unsigned int reg_d_fmt;     // 0x8
  unsigned int reg_y_addr;    // 0xc
  unsigned int reg_u_addr;    // 0x10
  unsigned int reg_v_addr;    // 0x14
  unsigned int reg_in_fm_gs;  // 0x18
  unsigned int reg_y_stride;  // 0x1c
  unsigned int reg_uv_stride; // 0x20
  unsigned int reg_out_addr;  // 0x24
  unsigned int reg_out_gs;    // 0x28
  unsigned int reg_out_stride;// 0x2c
  unsigned int rsz_coef_index;// 0x30
  unsigned int reg_csc_c0_coef;      // 0x34
  unsigned int reg_csc_c1_coef;      // 0x38
  unsigned int reg_csc_c2_coef;      // 0x3c
  unsigned int reg_csc_c3_coef;      // 0x40
  unsigned int reg_csc_c4_coef;      // 0x44
  unsigned int hrsz_coef_lut;    // 0x48
  unsigned int vrsz_coef_lut;    // 0x4c
  unsigned int reg_csc_offpara;  // 0x50
};

typedef struct {
   unsigned int coef;
   unsigned short int in_n;
   unsigned short int out_n;
} rsz_lut;


// REG_IN_FM_GS field define
#define IN_FM_W(val)    ((val) << 16)
#define IN_FM_H(val)    ((val) << 0)

// REG_IN_FM_GS field define
#define OUT_FM_W(val)    ((val) << 16)
#define OUT_FM_H(val)    ((val) << 0)

// REG_UV_STRIDE field define
#define U_STRIDE(val)     ((val) << 16)
#define V_STRIDE(val)     ((val) << 0)


#define VE_IDX_SFT        0
#define HE_IDX_SFT        16

// RSZ_LUT_FIELD
#define OUT_N_SFT         0
#define OUT_N_MSK         0x1
#define IN_N_SFT          1
#define IN_N_MSK          0x1
#define W_COEF_SFT        2
#define W_COEF_MSK        0x3FF
#define START_N_SFT       12



////////////////////////////////////////////////////////////////////////////
// Data Format Register
#define RGB_888_OUT_FMT				( 1 << 24 )

#define RGB_OUT_OFT_BIT				( 21 )
#define RGB_OUT_OFT_MASK			( 7 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RGB				( 0 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RBG				( 1 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GBR				( 2 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GRB				( 3 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BRG				( 4 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BGR				( 5 << RGB_OUT_OFT_BIT )

#define OUT_FMT_BIT					( 19 )
#define OUT_FMT_MASK				( 3 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB555				( 0 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB565				( 1 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB888				( 2 <<  OUT_FMT_BIT )
#define OUT_FMT_YUV422				( 3 <<  OUT_FMT_BIT )

#define YUV_PKG_OUT_OFT_BIT			( 16 )
#define YUV_PKG_OUT_OFT_MASK		( 7 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1UY0V		( 0 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1VY0U		( 1 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY1VY0		( 2 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY1UY0		( 3 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0UY1V		( 4 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0VY1U		( 5 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY0VY1		( 6 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY0UY1		( 7 << YUV_PKG_OUT_OFT_BIT )

#define IN_OFT_BIT					( 2 )
#define IN_OFT_MASK					( 3 << IN_OFT_BIT )
#define IN_OFT_Y1UY0V				( 0 << IN_OFT_BIT )
#define IN_OFT_Y1VY0U				( 1 << IN_OFT_BIT )
#define IN_OFT_UY1VY0				( 2 << IN_OFT_BIT )
#define IN_OFT_VY1UY0				( 3 << IN_OFT_BIT )

#define IN_FMT_BIT					( 0 )
#define IN_FMT_MASK					( 3 << IN_FMT_BIT )
#define IN_FMT_YUV420				( 0 << IN_FMT_BIT )
#define IN_FMT_YUV422				( 1 << IN_FMT_BIT )
#define IN_FMT_YUV444				( 2 << IN_FMT_BIT )
#define IN_FMT_YUV411				( 3 << IN_FMT_BIT )

struct Ration2m
{
	unsigned int ratio;
	int n, m;
};


// parameter
// R = 1.164 * (Y - 16) + 1.596 * (cr - 128)    {C0, C1}
// G = 1.164 * (Y - 16) - 0.392 * (cb -128) - 0.813 * (cr - 128)  {C0, C2, C3}
// B = 1.164 * (Y - 16) + 2.017 * (cb - 128)    {C0, C4}

#if 0
#define YUV_CSC_C0					0x4A8        /* 1.164 * 1024 */
#define YUV_CSC_C1					0x662        /* 1.596 * 1024 */
#define YUV_CSC_C2					0x191        /* 0.392 * 1024 */
#define YUV_CSC_C3					0x341        /* 0.813 * 1024 */
#define YUV_CSC_C4					0x811        /* 2.017 * 1024 */
#define YUV_CSC_CHROM				128
#define YUV_CSC_LUMA				16
#define YUV_CSC_OFFPARA         0x800010  /* chroma,luma */
#else
#define YUV_CSC_C0					0x400
#define YUV_CSC_C1					0x59C
#define YUV_CSC_C2					0x161
#define YUV_CSC_C3					0x2DC
#define YUV_CSC_C4					0x718
#define YUV_CSC_CHROM				128
#define YUV_CSC_LUMA				0
#define YUV_CSC_OFFPARA         0x800000  /* chroma,luma */
#endif

struct YuvCsc
{					// YUV(default)	or	YCbCr
	unsigned int csc0;		//	0x400			0x4A8
	unsigned int csc1;              //	0x59C   		0x662
	unsigned int csc2;              //	0x161   		0x191
	unsigned int csc3;              //	0x2DC   		0x341
	unsigned int csc4;              //	0x718   		0x811
	unsigned int chrom;             //	128				128
	unsigned int luma;              //	0				16
};

struct YuvStride
{
	unsigned int y;
	unsigned int u;
	unsigned int v;
	unsigned int out;
};

typedef struct
{
	unsigned int		ipu_ctrl;			// IPU Control Register
	unsigned int		ipu_d_fmt;			// IPU Data Format Register
	unsigned int		in_width;
	unsigned int		in_height;
	unsigned int		in_bpp;
	unsigned int		out_x;
	unsigned int		out_y;
//	unsigned int		in_fmt;
//	unsigned int		out_fmt;
	unsigned int		out_width;
	unsigned int		out_height;
	unsigned char*		y_buf;
	unsigned char*		u_buf;
	unsigned char*		v_buf;
	unsigned char*		out_buf;
	unsigned char*		y_t_addr;				// table address
	unsigned char*		u_t_addr;
	unsigned char*		v_t_addr;
	unsigned char*		out_t_addr;
	struct Ration2m*	ratio_table;
	struct YuvCsc*		csc;
	struct YuvStride*	stride;
} img_param_t;

#define IOCTL_IPU_SET_BUFF				0x0010
#define IOCTL_IPU_CHANGE_BUFF				0x0011
#define IOCTL_IPU_START					0x0012
#define IOCTL_IPU_STOP					0x0013
#define IOCTL_IPU_FB_SIZE				0x0014
#define IOCTL_IPU_SET_CTRL_REG				0x0015
#define IOCTL_IPU_SET_FMT_REG				0x0016
#define IOCTL_IPU_SET_CSC				0x0017
#define IOCTL_IPU_SET_STRIDE				0x0018
#define IOCTL_IPU_SET_OUTSIZE				0x0019
#define IOCTL_IPU_PENDING_OUTEND			0x0020
#define IOCTL_IPU_GET_ADDR                              0x0021

#endif  //endif _JZ4750_ANDROID_IPU_H_
