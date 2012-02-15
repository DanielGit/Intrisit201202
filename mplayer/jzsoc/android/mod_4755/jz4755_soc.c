#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "config.h"
#include "mp_msg.h"
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "sys/ioctl.h"
#include "sys/mman.h"
#include "fcntl.h"
#include <linux/fb.h>
#include <unistd.h>

#include "libavcodec/avcodec.h"
#include "libmpcodecs/img_format.h"
#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#include "libmpcodecs/img_format.h"
#include "libmpcodecs/mp_image.h"
#include "libmpcodecs/vf.h"
#include "libswscale/swscale.h"
#include "libswscale/swscale_internal.h"
#include "libmpcodecs/vf_scale.h"
#include "android_mvom.h"

#define JZ4755_IPU

#ifdef USE_ANDROID_SURFACE_FLINGER /* Wolfgang, 05-09-2009 */
#define NEED_COPY_IMGBUF
#endif	/* USE_ANDROID_SURFACE_FLINGER */

#ifdef JZ4755_IPU
#include "jz4755_ipu.h"
//#include "jz4755_lcd.h"
#endif
#undef printf
#define USE_IMEM_ALLOC
#define IPU_TO_LCD
#define LCD_BPP 16
#define FB_LCD_WIDTH  fb_w	
#define FB_LCD_HEIGHT fb_h	
#define LCD_BPP_BYTE (LCD_BPP / 8)

#define FB_TVE_WIDTH 704

//struct jz4750lcd_info jzlcd_info;
#if 1
#define MPLAYER_DBG()							\
	do {								\
		printf("%s[%d], %s()::\n", __FILE__, __LINE__, __FUNCTION__); \
	}while(0)

#define MPLAYER_PRINTF(fff, aaa...)					\
	do {								\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	} while (0)

#define please_use_av_log printf

#define ENTER()								\
	do {								\
		printf("%s[%d], %s():: *** LEAVE ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#define LEAVE()								\
	do {								\
		printf("%s[%d], %s():: *** LEAVE ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#else
#define MPLAYER_DBG() do {} while (0)
#define MPLAYER_PRINTF(fff, aaa...) do {} while (0)
#define ENTER()	do {} while (0)
#define LEAVE()	do {} while (0)
#endif



typedef struct {
	AVCodecContext *avctx;
	AVFrame *pic;
	enum PixelFormat pix_fmt;
	float last_aspect;
	int do_slices;
	int do_dr1;
	int vo_inited;
	int best_csp;
	int b_age;
	int ip_age[2];
	int qp_stat[32];
	double qp_sum;
	double inv_qp_sum;
	int ip_count;
	int b_count;
} vd_ffmpeg_ctx;

struct vf_priv_s {
	int w, h;
	int v_chr_drop;
	double param[2];
	unsigned int fmt;
	struct SwsContext *ctx;
	struct SwsContext *ctx2;	//for interlaced slices only
	unsigned char *palette;
	int interlaced;
	int noup;
	int accurate_rnd;
	int query_format_cache[64];
};

#define PAGE_SIZE (4096)
#define MEM_ALLOC_DEV_NUM 2

struct mem_dev {
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int totalsize;
	unsigned int usedsize;
} memdev[MEM_ALLOC_DEV_NUM];

#ifdef USE_IMEM_ALLOC
struct IMEM_Info {
	char *name;
	unsigned int power;
	unsigned int vaddr;
} imem_info[4] =
{
	{"/proc/jz/imem", 12 /*16M */ , 0x24000000},
	{"/proc/jz/imem1", 11 /*8M */ , 0x0},
	{"/proc/jz/imem2", 0, 0},
	{"/proc/jz/imem3", 0, 0}};

#endif

#define FBIOPRINT_REG		0x468c	/* print lcd registers(debug) */

extern int fbfd;
/* 0 : toggle ;  1 : no toggle */
static int put_image_cnt = 0;
static int toggle_flag = 0;
unsigned int dcache[4096];
static int mem_init = 0, memdev_count = 0;
int mmapfd = 0;
int save_Wsel, save_Hsel;
static int ipu_inited = 0, ipu_size_cfg = 0;
static int get_fbinfo_already = 0, set_fg1_flag = 0;
static int ipu_rtable_init = 0, ipu_rtable_len = 486;
int ipufd, ipu_maped = 0, ipu_image_completed = 0, first_run_ipu = 0;
volatile unsigned char *ipu_vbase = NULL, *img_ptr = NULL;
//extern unsigned char *frame_buffer = NULL;
extern unsigned int fb_w, fb_h, phy_fb, fbmemlen, fb_line_len;
unsigned int rsize_w = 0, rsize_h = 0;
unsigned int out_width, out_height;
static unsigned int out_x, out_y, out_w, out_h, frame_offset = 0;
static void fullscreen(int w, int h);

/* Mplayer output interface module */
struct mplayer_video_output_config_t g_mvoc;

/* IPU -> LCDC */
//int FOUT_W = 480;
//int FOUT_H = 272;

#define IPU_LUT_LEN 32
static struct Ration2m ipu_ratio_table[(IPU_LUT_LEN) * (IPU_LUT_LEN)] = {
	{1.000000, 1, 1}, {0.500000, 1, 2}, {2.000000, 2, 1}, {0.666667, 2, 3},
	{3.000000, 3, 1}, {1.500000, 3, 2}, {0.750000, 3, 4}, {0.600000, 3, 5},
	{4.000000, 4, 1}, {1.333333, 4, 3}, {0.800000, 4, 5}, {0.571429, 4, 7},
	{5.000000, 5, 1}, {2.500000, 5, 2}, {1.666667, 5, 3}, {1.250000, 5, 4},
	{0.833333, 5, 6}, {0.714286, 5, 7}, {0.625000, 5, 8}, {0.555556, 5, 9},
	{6.000000, 6, 1}, {1.200000, 6, 5}, {0.857143, 6, 7}, {0.545455, 6, 11},
	{7.000000, 7, 1}, {3.500000, 7, 2}, {2.333333, 7, 3}, {1.750000, 7, 4},
	{1.400000, 7, 5}, {1.166667, 7, 6}, {0.875000, 7, 8}, {0.777778, 7, 9},
	{0.700000, 7, 10}, {0.636364, 7, 11}, {0.583333, 7, 12}, {0.538462, 7, 13},
	{8.000000, 8, 1}, {2.666667, 8, 3}, {1.600000, 8, 5}, {1.142857, 8, 7},
	{0.888889, 8, 9}, {0.727273, 8, 11}, {0.615385, 8, 13}, {0.533333, 8, 15},
	{9.000000, 9, 1}, {4.500000, 9, 2}, {2.250000, 9, 4}, {1.800000, 9, 5},
	{1.285714, 9, 7}, {1.125000, 9, 8}, {0.900000, 9, 10}, {0.818182, 9, 11},
	{0.692308, 9, 13}, {0.642857, 9, 14}, {0.562500, 9, 16}, {0.529412, 9, 17},
	{10.000000, 10, 1}, {3.333333, 10, 3}, {1.428571, 10, 7}, {1.111111, 10, 9},
	{0.909091, 10, 11}, {0.769231, 10, 13}, {0.588235, 10, 17}, {0.526316,  10, 19},
	{11.000000, 11, 1}, {5.500000, 11, 2}, {3.666667, 11, 3}, {2.750000, 11, 4},
	{2.200000, 11, 5}, {1.833333, 11, 6}, {1.571429, 11, 7}, {1.375000, 11, 8},
	{1.222222, 11, 9}, {1.100000, 11, 10}, {0.916667, 11, 12}, {0.846154, 11, 13},
	{0.785714, 11, 14}, {0.733333, 11, 15}, {0.687500, 11, 16}, {0.647059, 11, 17},
	{0.611111, 11, 18}, {0.578947, 11, 19}, {0.550000, 11, 20}, {0.523810, 11, 21},
	{12.000000, 12, 1}, {2.400000, 12, 5}, {1.714286, 12, 7}, {1.090909, 12, 11},
	{0.923077, 12, 13}, {0.705882, 12, 17}, {0.631579, 12, 19}, {0.521739, 12, 23},
	{13.000000, 13, 1}, {6.500000, 13, 2}, {4.333333, 13, 3}, {3.250000, 13, 4},
	{2.600000, 13, 5}, {2.166667, 13, 6}, {1.857143, 13, 7}, {1.625000, 13, 8},
	{1.444444, 13, 9}, {1.300000, 13, 10}, {1.181818, 13, 11}, {1.083333, 13, 12},
	{0.928571, 13, 14}, {0.866667, 13, 15}, {0.812500, 13, 16}, {0.764706, 13, 17},
	{0.722222, 13, 18}, {0.684211, 13, 19}, {0.650000, 13, 20}, {0.619048, 13, 21},
	{0.590909, 13, 22}, {0.565217, 13, 23}, {0.541667, 13, 24}, {0.520000, 13, 25},
	{14.000000, 14, 1}, {4.666667, 14, 3}, {2.800000, 14, 5}, {1.555556, 14, 9},
	{1.272727, 14, 11}, {1.076923, 14, 13}, {0.933333, 14, 15}, {0.823529, 14, 17},
	{0.736842, 14, 19}, {0.608696, 14, 23}, {0.560000, 14, 25}, {0.518519, 14, 27},
	{15.000000, 15, 1}, {7.500000, 15, 2}, {3.750000, 15, 4}, {2.142857, 15, 7},
	{1.875000, 15, 8}, {1.363636, 15, 11}, {1.153846, 15, 13}, {1.071429, 15, 14},
	{0.937500, 15, 16}, {0.882353, 15, 17}, {0.789474, 15, 19}, {0.681818, 15, 22},
	{0.652174, 15, 23}, {0.576923, 15, 26}, {0.535714, 15, 28}, {0.517241, 15, 29},
	{16.000000, 16, 1}, {5.333333, 16, 3}, {3.200000, 16, 5}, {2.285714, 16, 7},
	{1.777778, 16, 9}, {1.454545, 16, 11}, {1.230769, 16, 13}, {1.066667, 16, 15},
	{0.941176, 16, 17}, {0.842105, 16, 19}, {0.761905, 16, 21}, {0.695652, 16, 23},
	{0.640000, 16, 25}, {0.592593, 16, 27}, {0.551724, 16, 29}, {0.516129, 16, 31},
	{17.000000, 17, 1}, {8.500000, 17, 2}, {5.666667, 17, 3}, {4.250000, 17,
								   4},
	{3.400000, 17, 5}, {2.833333, 17, 6}, {2.428571, 17, 7}, {2.125000, 17,
								  8},
	{1.888889, 17, 9}, {1.700000, 17, 10}, {1.545455, 17, 11}, {1.416667,
								    17, 12},
	{1.307692, 17, 13}, {1.214286, 17, 14}, {1.133333, 17, 15}, {1.062500,
								     17, 16},
	{0.944444, 17, 18}, {0.894737, 17, 19}, {0.850000, 17, 20}, {0.809524,
								     17, 21},
	{0.772727, 17, 22}, {0.739130, 17, 23}, {0.708333, 17, 24}, {0.680000,
								     17, 25},
	{0.653846, 17, 26}, {0.629630, 17, 27}, {0.607143, 17, 28}, {0.586207,
								     17, 29},
	{0.566667, 17, 30}, {0.548387, 17, 31}, {0.531250, 17, 32}, {18.000000,
								     18, 1},
	{3.600000, 18, 5}, {2.571429, 18, 7}, {1.636364, 18, 11}, {1.384615, 18,
								   13},
	{1.058824, 18, 17}, {0.947368, 18, 19}, {0.782609, 18, 23}, {0.720000,
								     18, 25},
	{0.620690, 18, 29}, {0.580645, 18, 31}, {19.000000, 19, 1}, {9.500000,
								     19, 2},
	{6.333333, 19, 3}, {4.750000, 19, 4}, {3.800000, 19, 5}, {3.166667, 19,
								  6},
	{2.714286, 19, 7}, {2.375000, 19, 8}, {2.111111, 19, 9}, {1.900000, 19,
								  10},
	{1.727273, 19, 11}, {1.583333, 19, 12}, {1.461538, 19, 13}, {1.357143,
								     19, 14},
	{1.266667, 19, 15}, {1.187500, 19, 16}, {1.117647, 19, 17}, {1.055556,
								     19, 18},
	{0.950000, 19, 20}, {0.904762, 19, 21}, {0.863636, 19, 22}, {0.826087,
								     19, 23},
	{0.791667, 19, 24}, {0.760000, 19, 25}, {0.730769, 19, 26}, {0.703704,
								     19, 27},
	{0.678571, 19, 28}, {0.655172, 19, 29}, {0.633333, 19, 30}, {0.612903,
								     19, 31},
	{0.593750, 19, 32}, {20.000000, 20, 1}, {6.666667, 20, 3}, {2.857143,
								    20, 7},
	{2.222222, 20, 9}, {1.818182, 20, 11}, {1.538462, 20, 13}, {1.176471,
								    20, 17},
	{1.052632, 20, 19}, {0.952381, 20, 21}, {0.869565, 20, 23}, {0.740741,
								     20, 27},
	{0.689655, 20, 29}, {0.645161, 20, 31}, {21.000000, 21, 1}, {10.500000,
								     21, 2},
	{5.250000, 21, 4}, {4.200000, 21, 5}, {2.625000, 21, 8}, {2.100000, 21,
								  10},
	{1.909091, 21, 11}, {1.615385, 21, 13}, {1.312500, 21, 16}, {1.235294,
								     21, 17},
	{1.105263, 21, 19}, {1.050000, 21, 20}, {0.954545, 21, 22}, {0.913043,
								     21, 23},
	{0.840000, 21, 25}, {0.807692, 21, 26}, {0.724138, 21, 29}, {0.677419,
								     21, 31},
	{0.656250, 21, 32}, {22.000000, 22, 1}, {7.333333, 22, 3}, {4.400000,
								    22, 5},
	{3.142857, 22, 7}, {2.444444, 22, 9}, {1.692308, 22, 13}, {1.466667, 22,
								   15},
	{1.294118, 22, 17}, {1.157895, 22, 19}, {1.047619, 22, 21}, {0.956522,
								     22, 23},
	{0.880000, 22, 25}, {0.814815, 22, 27}, {0.758621, 22, 29}, {0.709677,
								     22, 31},
	{23.000000, 23, 1}, {11.500000, 23, 2}, {7.666667, 23, 3}, {5.750000,
								    23, 4},
	{4.600000, 23, 5}, {3.833333, 23, 6}, {3.285714, 23, 7}, {2.875000, 23,
								  8},
	{2.555556, 23, 9}, {2.300000, 23, 10}, {2.090909, 23, 11}, {1.916667,
								    23, 12},
	{1.769231, 23, 13}, {1.642857, 23, 14}, {1.533333, 23, 15}, {1.437500,
								     23, 16},
	{1.352941, 23, 17}, {1.277778, 23, 18}, {1.210526, 23, 19}, {1.150000,
								     23, 20},
	{1.095238, 23, 21}, {1.045455, 23, 22}, {0.958333, 23, 24}, {0.920000,
								     23, 25},
	{0.884615, 23, 26}, {0.851852, 23, 27}, {0.821429, 23, 28}, {0.793103,
								     23, 29},
	{0.766667, 23, 30}, {0.741935, 23, 31}, {0.718750, 23, 32}, {24.000000,
								     24, 1},
	{4.800000, 24, 5}, {3.428571, 24, 7}, {2.181818, 24, 11}, {1.846154, 24,
								   13},
	{1.411765, 24, 17}, {1.263158, 24, 19}, {1.043478, 24, 23}, {0.960000,
								     24, 25},
	{0.827586, 24, 29}, {0.774194, 24, 31}, {25.000000, 25, 1}, {12.500000,
								     25, 2},
	{8.333333, 25, 3}, {6.250000, 25, 4}, {4.166667, 25, 6}, {3.571429, 25,
								  7},
	{3.125000, 25, 8}, {2.777778, 25, 9}, {2.272727, 25, 11}, {2.083333, 25,
								   12},
	{1.923077, 25, 13}, {1.785714, 25, 14}, {1.562500, 25, 16}, {1.470588,
								     25, 17},
	{1.388889, 25, 18}, {1.315789, 25, 19}, {1.190476, 25, 21}, {1.136364,
								     25, 22},
	{1.086957, 25, 23}, {1.041667, 25, 24}, {0.961538, 25, 26}, {0.925926,
								     25, 27},
	{0.892857, 25, 28}, {0.862069, 25, 29}, {0.806452, 25, 31}, {0.781250,
								     25, 32},
	{26.000000, 26, 1}, {8.666667, 26, 3}, {5.200000, 26, 5}, {3.714286, 26,
								   7},
	{2.888889, 26, 9}, {2.363636, 26, 11}, {1.733333, 26, 15}, {1.529412,
								    26, 17},
	{1.368421, 26, 19}, {1.238095, 26, 21}, {1.130435, 26, 23}, {1.040000,
								     26, 25},
	{0.962963, 26, 27}, {0.896552, 26, 29}, {0.838710, 26, 31}, {27.000000,
								     27, 1},
	{13.500000, 27, 2}, {6.750000, 27, 4}, {5.400000, 27, 5}, {3.857143, 27,
								   7},
	{3.375000, 27, 8}, {2.700000, 27, 10}, {2.454545, 27, 11}, {2.076923,
								    27, 13},
	{1.928571, 27, 14}, {1.687500, 27, 16}, {1.588235, 27, 17}, {1.421053,
								     27, 19},
	{1.350000, 27, 20}, {1.227273, 27, 22}, {1.173913, 27, 23}, {1.080000,
								     27, 25},
	{1.038462, 27, 26}, {0.964286, 27, 28}, {0.931035, 27, 29}, {0.870968,
								     27, 31},
	{0.843750, 27, 32}, {28.000000, 28, 1}, {9.333333, 28, 3}, {5.600000,
								    28, 5},
	{3.111111, 28, 9}, {2.545455, 28, 11}, {2.153846, 28, 13}, {1.866667,
								    28, 15},
	{1.647059, 28, 17}, {1.473684, 28, 19}, {1.217391, 28, 23}, {1.120000,
								     28, 25},
	{1.037037, 28, 27}, {0.965517, 28, 29}, {0.903226, 28, 31}, {29.000000,
								     29, 1},
	{14.500000, 29, 2}, {9.666667, 29, 3}, {7.250000, 29, 4}, {5.800000, 29,
								   5},
	{4.833333, 29, 6}, {4.142857, 29, 7}, {3.625000, 29, 8}, {3.222222, 29,
								  9},
	{2.900000, 29, 10}, {2.636364, 29, 11}, {2.416667, 29, 12}, {2.230769,
								     29, 13},
	{2.071429, 29, 14}, {1.933333, 29, 15}, {1.812500, 29, 16}, {1.705882,
								     29, 17},
	{1.611111, 29, 18}, {1.526316, 29, 19}, {1.450000, 29, 20}, {1.380952,
								     29, 21},
	{1.318182, 29, 22}, {1.260870, 29, 23}, {1.208333, 29, 24}, {1.160000,
								     29, 25},
	{1.115385, 29, 26}, {1.074074, 29, 27}, {1.035714, 29, 28}, {0.966667,
								     29, 30},
	{0.935484, 29, 31}, {0.906250, 29, 32}, {30.000000, 30, 1}, {4.285714,
								     30, 7},
	{2.727273, 30, 11}, {2.307692, 30, 13}, {1.764706, 30, 17}, {1.578947,
								     30, 19},
	{1.304348, 30, 23}, {1.034483, 30, 29}, {0.967742, 30, 31}, {31.000000,
								     31, 1},
	{15.500000, 31, 2}, {10.333333, 31, 3}, {7.750000, 31, 4}, {6.200000,
								    31, 5},
	{5.166667, 31, 6}, {4.428571, 31, 7}, {3.875000, 31, 8}, {3.444444, 31,
								  9},
	{3.100000, 31, 10}, {2.818182, 31, 11}, {2.583333, 31, 12}, {2.384615,
								     31, 13},
	{2.214286, 31, 14}, {2.066667, 31, 15}, {1.937500, 31, 16}, {1.823529,
								     31, 17},
	{1.722222, 31, 18}, {1.631579, 31, 19}, {1.550000, 31, 20}, {1.476190,
								     31, 21},
	{1.409091, 31, 22}, {1.347826, 31, 23}, {1.291667, 31, 24}, {1.240000,
								     31, 25},
	{1.192308, 31, 26}, {1.148148, 31, 27}, {1.107143, 31, 28}, {1.068966,
								     31, 29},
	{1.033333, 31, 30}, {0.968750, 31, 32}, {32.000000, 32, 1}, {10.666667,
								     32, 3},
	{6.400000, 32, 5}, {4.571429, 32, 7}, {3.555556, 32, 9}, {2.909091, 32,
								  11},
	{2.461539, 32, 13}, {2.133333, 32, 15}, {1.882353, 32, 17}, {1.684211,
								     32, 19},
	{1.523810, 32, 21}, {1.391304, 32, 23}, {1.280000, 32, 25}, {1.185185,
								     32, 27},
	{1.103448, 32, 29}, {1.032258, 32, 31}, {2.285714, 16, 7}, {2.000000,
								    -1, -1},
	{1.777778, 16, 9}, {1.600000, -1, -1}, {1.454545, 16, 11}, {1.333333,
								    -1, -1},
	{1.230769, 16, 13}, {1.142857, -1, -1}, {1.066667, 16, 15}, {1.000000,
								     -1, -1},
	{0.941176, 16, 17}, {0.888889, -1, -1}, {0.842105, 16, 19}, {0.800000,
								     -1, -1},
	{0.761905, 16, 21}, {0.727273, -1, -1}, {0.695652, 16, 23}, {0.666667,
								     -1, -1},
	{0.640000, 16, 25}, {0.615385, -1, -1}, {0.592593, 16, 27}, {0.571429,
								     -1, -1},
	{0.551724, 16, 29}, {0.533333, -1, -1}, {0.516129, 16, 31}, {0.500000,
								     -1, -1},
	{17.000000, 17, 1}, {8.500000, 17, 2}, {5.666667, 17, 3}, {4.250000, 17,
								   4},
	{3.400000, 17, 5}, {2.833333, 17, 6}, {2.428571, 17, 7}, {2.125000, 17,
								  8},
	{1.888889, 17, 9}, {1.700000, 17, 10}, {1.545455, 17, 11}, {1.416667,
								    17, 12},
	{1.307692, 17, 13}, {1.214286, 17, 14}, {1.133333, 17, 15}, {1.062500,
								     17, 16},
	{1.000000, -1, -1}, {0.944444, 17, 18}, {0.894737, 17, 19}, {0.850000,
								     17, 20},
	{0.809524, 17, 21}, {0.772727, 17, 22}, {0.739130, 17, 23}, {0.708333,
								     17, 24},
	{0.680000, 17, 25}, {0.653846, 17, 26}, {0.629630, 17, 27}, {0.607143,
								     17, 28},
	{0.586207, 17, 29}, {0.566667, 17, 30}, {0.548387, 17, 31}, {0.531250,
								     17, 32},
	{18.000000, 18, 1}, {9.000000, -1, -1}, {6.000000, -1, -1}, {4.500000,
								     -1, -1},
	{3.600000, 18, 5}, {3.000000, -1, -1}, {2.571429, 18, 7}, {2.250000, -1,
								   -1},
	{2.000000, -1, -1}, {1.800000, -1, -1}, {1.636364, 18, 11}, {1.500000,
								     -1, -1},
	{1.384615, 18, 13}, {1.285714, -1, -1}, {1.200000, -1, -1}, {1.125000,
								     -1, -1},
	{1.058824, 18, 17}, {1.000000, -1, -1}, {0.947368, 18, 19}, {0.900000,
								     -1, -1},
	{0.857143, -1, -1}, {0.818182, -1, -1}, {0.782609, 18, 23}, {0.750000,
								     -1, -1},
	{0.720000, 18, 25}, {0.692308, -1, -1}, {0.666667, -1, -1}, {0.642857,
								     -1, -1},
	{0.620690, 18, 29}, {0.600000, -1, -1}, {0.580645, 18, 31}, {0.562500,
								     -1, -1},
	{19.000000, 19, 1}, {9.500000, 19, 2}, {6.333333, 19, 3}, {4.750000, 19,
								   4},
	{3.800000, 19, 5}, {3.166667, 19, 6}, {2.714286, 19, 7}, {2.375000, 19,
								  8},
	{2.111111, 19, 9}, {1.900000, 19, 10}, {1.727273, 19, 11}, {1.583333,
								    19, 12},
	{1.461538, 19, 13}, {1.357143, 19, 14}, {1.266667, 19, 15}, {1.187500,
								     19, 16},
	{1.117647, 19, 17}, {1.055556, 19, 18}, {1.000000, -1, -1}, {0.950000,
								     19, 20},
	{0.904762, 19, 21}, {0.863636, 19, 22}, {0.826087, 19, 23}, {0.791667,
								     19, 24},
	{0.760000, 19, 25}, {0.730769, 19, 26}, {0.703704, 19, 27}, {0.678571,
								     19, 28},
	{0.655172, 19, 29}, {0.633333, 19, 30}, {0.612903, 19, 31}, {0.593750,
								     19, 32},
	{20.000000, 20, 1}, {10.000000, -1, -1}, {6.666667, 20, 3}, {5.000000,
								     -1, -1},
	{4.000000, -1, -1}, {3.333333, -1, -1}, {2.857143, 20, 7}, {2.500000,
								    -1, -1},
	{2.222222, 20, 9}, {2.000000, -1, -1}, {1.818182, 20, 11}, {1.666667,
								    -1, -1},
	{1.538462, 20, 13}, {1.428571, -1, -1}, {1.333333, -1, -1}, {1.250000,
								     -1, -1},
	{1.176471, 20, 17}, {1.111111, -1, -1}, {1.052632, 20, 19}, {1.000000,
								     -1, -1},
	{0.952381, 20, 21}, {0.909091, -1, -1}, {0.869565, 20, 23}, {0.833333,
								     -1, -1},
	{0.800000, -1, -1}, {0.769231, -1, -1}, {0.740741, 20, 27}, {0.714286,
								     -1, -1},
	{0.689655, 20, 29}, {0.666667, -1, -1}, {0.645161, 20, 31}, {0.625000,
								     -1, -1},
	{21.000000, 21, 1}, {10.500000, 21, 2}, {7.000000, -1, -1}, {5.250000,
								     21, 4},
	{4.200000, 21, 5}, {3.500000, -1, -1}, {3.000000, -1, -1}, {2.625000,
								    21, 8},
	{2.333333, -1, -1}, {2.100000, 21, 10}, {1.909091, 21, 11}, {1.750000,
								     -1, -1},
	{1.615385, 21, 13}, {1.500000, -1, -1}, {1.400000, -1, -1}, {1.312500,
								     21, 16},
	{1.235294, 21, 17}, {1.166667, -1, -1}, {1.105263, 21, 19}, {1.050000,
								     21, 20},
	{1.000000, -1, -1}, {0.954545, 21, 22}, {0.913043, 21, 23}, {0.875000,
								     -1, -1},
	{0.840000, 21, 25}, {0.807692, 21, 26}, {0.777778, -1, -1}, {0.750000,
								     -1, -1},
	{0.724138, 21, 29}, {0.700000, -1, -1}, {0.677419, 21, 31}, {0.656250,
								     21, 32},
	{22.000000, 22, 1}, {11.000000, -1, -1}, {7.333333, 22, 3}, {5.500000,
								     -1, -1},
	{4.400000, 22, 5}, {3.666667, -1, -1}, {3.142857, 22, 7}, {2.750000, -1,
								   -1},
	{2.444444, 22, 9}, {2.200000, -1, -1}, {2.000000, -1, -1}, {1.833333,
								    -1, -1},
	{1.692308, 22, 13}, {1.571429, -1, -1}, {1.466667, 22, 15}, {1.375000,
								     -1, -1},
	{1.294118, 22, 17}, {1.222222, -1, -1}, {1.157895, 22, 19}, {1.100000,
								     -1, -1},
	{1.047619, 22, 21}, {1.000000, -1, -1}, {0.956522, 22, 23}, {0.916667,
								     -1, -1},
	{0.880000, 22, 25}, {0.846154, -1, -1}, {0.814815, 22, 27}, {0.785714,
								     -1, -1},
	{0.758621, 22, 29}, {0.733333, -1, -1}, {0.709677, 22, 31}, {0.687500,
								     -1, -1},
	{23.000000, 23, 1}, {11.500000, 23, 2}, {7.666667, 23, 3}, {5.750000,
								    23, 4},
	{4.600000, 23, 5}, {3.833333, 23, 6}, {3.285714, 23, 7}, {2.875000, 23,
								  8},
	{2.555556, 23, 9}, {2.300000, 23, 10}, {2.090909, 23, 11}, {1.916667,
								    23, 12},
	{1.769231, 23, 13}, {1.642857, 23, 14}, {1.533333, 23, 15}, {1.437500,
								     23, 16},
	{1.352941, 23, 17}, {1.277778, 23, 18}, {1.210526, 23, 19}, {1.150000,
								     23, 20},
	{1.095238, 23, 21}, {1.045455, 23, 22}, {1.000000, -1, -1}, {0.958333,
								     23, 24},
	{0.920000, 23, 25}, {0.884615, 23, 26}, {0.851852, 23, 27}, {0.821429,
								     23, 28},
	{0.793103, 23, 29}, {0.766667, 23, 30}, {0.741935, 23, 31}, {0.718750,
								     23, 32},
	{24.000000, 24, 1}, {12.000000, -1, -1}, {8.000000, -1, -1}, {6.000000,
								      -1, -1},
	{4.800000, 24, 5}, {4.000000, -1, -1}, {3.428571, 24, 7}, {3.000000, -1,
								   -1},
	{2.666667, -1, -1}, {2.400000, -1, -1}, {2.181818, 24, 11}, {2.000000,
								     -1, -1},
	{1.846154, 24, 13}, {1.714286, -1, -1}, {1.600000, -1, -1}, {1.500000,
								     -1, -1},
	{1.411765, 24, 17}, {1.333333, -1, -1}, {1.263158, 24, 19}, {1.200000,
								     -1, -1},
	{1.142857, -1, -1}, {1.090909, -1, -1}, {1.043478, 24, 23}, {1.000000,
								     -1, -1},
	{0.960000, 24, 25}, {0.923077, -1, -1}, {0.888889, -1, -1}, {0.857143,
								     -1, -1},
	{0.827586, 24, 29}, {0.800000, -1, -1}, {0.774194, 24, 31}, {0.750000,
								     -1, -1},
	{25.000000, 25, 1}, {12.500000, 25, 2}, {8.333333, 25, 3}, {6.250000,
								    25, 4},
	{5.000000, -1, -1}, {4.166667, 25, 6}, {3.571429, 25, 7}, {3.125000, 25,
								   8},
	{2.777778, 25, 9}, {2.500000, -1, -1}, {2.272727, 25, 11}, {2.083333,
								    25, 12},
	{1.923077, 25, 13}, {1.785714, 25, 14}, {1.666667, -1, -1}, {1.562500,
								     25, 16},
	{1.470588, 25, 17}, {1.388889, 25, 18}, {1.315789, 25, 19}, {1.250000,
								     -1, -1},
	{1.190476, 25, 21}, {1.136364, 25, 22}, {1.086957, 25, 23}, {1.041667,
								     25, 24},
	{1.000000, -1, -1}, {0.961538, 25, 26}, {0.925926, 25, 27}, {0.892857,
								     25, 28},
	{0.862069, 25, 29}, {0.833333, -1, -1}, {0.806452, 25, 31}, {0.781250,
								     25, 32},
	{26.000000, 26, 1}, {13.000000, -1, -1}, {8.666667, 26, 3}, {6.500000,
								     -1, -1},
	{5.200000, 26, 5}, {4.333333, -1, -1}, {3.714286, 26, 7}, {3.250000, -1,
								   -1},
	{2.888889, 26, 9}, {2.600000, -1, -1}, {2.363636, 26, 11}, {2.166667,
								    -1, -1},
	{2.000000, -1, -1}, {1.857143, -1, -1}, {1.733333, 26, 15}, {1.625000,
								     -1, -1},
	{1.529412, 26, 17}, {1.444444, -1, -1}, {1.368421, 26, 19}, {1.300000,
								     -1, -1},
	{1.238095, 26, 21}, {1.181818, -1, -1}, {1.130435, 26, 23}, {1.083333,
								     -1, -1},
	{1.040000, 26, 25}, {1.000000, -1, -1}, {0.962963, 26, 27}, {0.928571,
								     -1, -1},
	{0.896552, 26, 29}, {0.866667, -1, -1}, {0.838710, 26, 31}, {0.812500,
								     -1, -1},
	{27.000000, 27, 1}, {13.500000, 27, 2}, {9.000000, -1, -1}, {6.750000,
								     27, 4},
	{5.400000, 27, 5}, {4.500000, -1, -1}, {3.857143, 27, 7}, {3.375000, 27,
								   8},
	{3.000000, -1, -1}, {2.700000, 27, 10}, {2.454545, 27, 11}, {2.250000,
								     -1, -1},
	{2.076923, 27, 13}, {1.928571, 27, 14}, {1.800000, -1, -1}, {1.687500,
								     27, 16},
	{1.588235, 27, 17}, {1.500000, -1, -1}, {1.421053, 27, 19}, {1.350000,
								     27, 20},
	{1.285714, -1, -1}, {1.227273, 27, 22}, {1.173913, 27, 23}, {1.125000,
								     -1, -1},
	{1.080000, 27, 25}, {1.038462, 27, 26}, {1.000000, -1, -1}, {0.964286,
								     27, 28},
	{0.931035, 27, 29}, {0.900000, -1, -1}, {0.870968, 27, 31}, {0.843750,
								     27, 32},
	{28.000000, 28, 1}, {14.000000, -1, -1}, {9.333333, 28, 3}, {7.000000,
								     -1, -1},
	{5.600000, 28, 5}, {4.666667, -1, -1}, {4.000000, -1, -1}, {3.500000,
								    -1, -1},
	{3.111111, 28, 9}, {2.800000, -1, -1}, {2.545455, 28, 11}, {2.333333,
								    -1, -1},
	{2.153846, 28, 13}, {2.000000, -1, -1}, {1.866667, 28, 15}, {1.750000,
								     -1, -1},
	{1.647059, 28, 17}, {1.555556, -1, -1}, {1.473684, 28, 19}, {1.400000,
								     -1, -1},
	{1.333333, -1, -1}, {1.272727, -1, -1}, {1.217391, 28, 23}, {1.166667,
								     -1, -1},
	{1.120000, 28, 25}, {1.076923, -1, -1}, {1.037037, 28, 27}, {1.000000,
								     -1, -1},
	{0.965517, 28, 29}, {0.933333, -1, -1}, {0.903226, 28, 31}, {0.875000,
								     -1, -1},
	{29.000000, 29, 1}, {14.500000, 29, 2}, {9.666667, 29, 3}, {7.250000,
								    29, 4},
	{5.800000, 29, 5}, {4.833333, 29, 6}, {4.142857, 29, 7}, {3.625000, 29,
								  8},
	{3.222222, 29, 9}, {2.900000, 29, 10}, {2.636364, 29, 11}, {2.416667,
								    29, 12},
	{2.230769, 29, 13}, {2.071429, 29, 14}, {1.933333, 29, 15}, {1.812500,
								     29, 16},
	{1.705882, 29, 17}, {1.611111, 29, 18}, {1.526316, 29, 19}, {1.450000,
								     29, 20},
	{1.380952, 29, 21}, {1.318182, 29, 22}, {1.260870, 29, 23}, {1.208333,
								     29, 24},
	{1.160000, 29, 25}, {1.115385, 29, 26}, {1.074074, 29, 27}, {1.035714,
								     29, 28},
	{1.000000, -1, -1}, {0.966667, 29, 30}, {0.935484, 29, 31}, {0.906250,
								     29, 32},
	{30.000000, 30, 1}, {15.000000, -1, -1}, {10.000000, -1, -1}, {7.500000,
								       -1, -1},
	{6.000000, -1, -1}, {5.000000, -1, -1}, {4.285714, 30, 7}, {3.750000,
								    -1, -1},
	{3.333333, -1, -1}, {3.000000, -1, -1}, {2.727273, 30, 11}, {2.500000,
								     -1, -1},
	{2.307692, 30, 13}, {2.142857, -1, -1}, {2.000000, -1, -1}, {1.875000,
								     -1, -1},
	{1.764706, 30, 17}, {1.666667, -1, -1}, {1.578947, 30, 19}, {1.500000,
								     -1, -1},
	{1.428571, -1, -1}, {1.363636, -1, -1}, {1.304348, 30, 23}, {1.250000,
								     -1, -1},
	{1.200000, -1, -1}, {1.153846, -1, -1}, {1.111111, -1, -1}, {1.071429,
								     -1, -1},
	{1.034483, 30, 29}, {1.000000, -1, -1}, {0.967742, 30, 31}, {0.937500,
								     -1, -1},
	{31.000000, 31, 1}, {15.500000, 31, 2}, {10.333333, 31, 3}, {7.750000,
								     31, 4},
	{6.200000, 31, 5}, {5.166667, 31, 6}, {4.428571, 31, 7}, {3.875000, 31,
								  8},
	{3.444444, 31, 9}, {3.100000, 31, 10}, {2.818182, 31, 11}, {2.583333,
								    31, 12},
	{2.384615, 31, 13}, {2.214286, 31, 14}, {2.066667, 31, 15}, {1.937500,
								     31, 16},
	{1.823529, 31, 17}, {1.722222, 31, 18}, {1.631579, 31, 19}, {1.550000,
								     31, 20},
	{1.476190, 31, 21}, {1.409091, 31, 22}, {1.347826, 31, 23}, {1.291667,
								     31, 24},
	{1.240000, 31, 25}, {1.192308, 31, 26}, {1.148148, 31, 27}, {1.107143,
								     31, 28},
	{1.068966, 31, 29}, {1.033333, 31, 30}, {1.000000, -1, -1}, {0.968750,
								     31, 32},
	{32.000000, 32, 1}, {16.000000, -1, -1}, {10.666667, 32, 3}, {8.000000,
								      -1, -1},
	{6.400000, 32, 5}, {5.333333, -1, -1}, {4.571429, 32, 7}, {4.000000, -1,
								   -1},
	{3.555556, 32, 9}, {3.200000, -1, -1}, {2.909091, 32, 11}, {2.666667,
								    -1, -1},
	{2.461539, 32, 13}, {2.285714, -1, -1}, {2.133333, 32, 15}, {2.000000,
								     -1, -1},
	{1.882353, 32, 17}, {1.777778, -1, -1}, {1.684211, 32, 19}, {1.600000,
								     -1, -1},
	{1.523810, 32, 21}, {1.454545, -1, -1}, {1.391304, 32, 23}, {1.333333,
								     -1, -1},
	{1.280000, 32, 25}, {1.230769, -1, -1}, {1.185185, 32, 27}, {1.142857,
								     -1, -1},
	{1.103448, 32, 29}, {1.066667, -1, -1}, {1.032258, 32, 31}, {1.000000,
								     -1, -1}
};

rsz_lut h_lut[IPU_LUT_LEN];
rsz_lut v_lut[IPU_LUT_LEN];

#define MP_IMAGE_TYOE_SIZE_FINISH 0
#define MP_IMAGE_TYPE_CUSTORM     1
#define MP_IMAGE_TYPE_INITSIZE    2
#define MP_IMAGE_TYPE_EQSIZE      3
#define MP_IMAGE_TYPE_FULLSCREEN  4


unsigned int get_phy_addr(unsigned int vaddr)
{
	int i;

	for (i = 0; i < memdev_count; i++)
	{
		if (vaddr >= memdev[i].vaddr
		    && vaddr < memdev[i].vaddr + memdev[i].totalsize)
			return memdev[i].paddr + (vaddr - memdev[i].vaddr);
	}
	return 0;
}


unsigned int tran_addr_for_aux(unsigned int vaddr)
{
	int i;

	for (i = 0; i < memdev_count; i++)
	{
		if (vaddr >= memdev[i].vaddr
		    && vaddr < memdev[i].vaddr + memdev[i].totalsize)
			return memdev[i].paddr + (vaddr - memdev[i].vaddr);
	}

	return 0;
}

void jz4740_free_devmem()
{
	int power = 255, i;
	unsigned char cmdbuf[100];

	for (i = 0; i < memdev_count; i++)
	{
		if (memdev[i].vaddr)
		{
			munmap((void *) memdev[i].vaddr, memdev[i].totalsize);
			memset(&memdev[i], 0, sizeof(struct mem_dev));
		}
	}

	mem_init = 0;
	memdev_count = 0;

#ifdef USE_IMEM_ALLOC
	sprintf(cmdbuf, "echo FF > %s", imem_info[0].name);
	system(cmdbuf);
#endif
}

int jz4755_memset(unsigned int vaddr, int size)
{
	if (vaddr)
		memset(((unsigned char *) vaddr), 0, size);

	return 0;
}

void *jz4740_alloc_frame(int align, int size)
{
	int power, pages = 1, fd, i;
	int cur_pid = 0;
	unsigned int paddr, vaddr, data;
	unsigned char cmdbuf[100];

	/* Free all proc memory. */
	if (mem_init == 0)
	{
#ifdef USE_IMEM_ALLOC
		sprintf(cmdbuf, "echo FF > %s", imem_info[0].name);
		//echo FF > /proc/jz/imem
		system(cmdbuf);
#endif
		mem_init = 1;
	}
	/* open /dev/mem for mmap the memory. */
	if (mmapfd == 0)
		mmapfd = open("/dev/mem", O_RDWR);
	if (mmapfd <= 0)
	{
		printf
		    ("++ Jz47 alloc frame failed: can not mmap the memory ++\n");
		exit(1);
	}

	for (i = 0; i < memdev_count; i++)
	{
		int alloc_size, used_size, total_size;
		used_size = memdev[i].usedsize;
		total_size = memdev[i].totalsize;
		alloc_size = total_size - used_size;

		if (alloc_size >= size)
		{
			/* align to 16 boundary.  */
			alloc_size = (size + 15) & ~(15);
			memdev[i].usedsize = used_size + alloc_size;
			vaddr = memdev[i].vaddr + used_size;

			printf
			    ("1 jz mem alloc [%d]: vaddr = 0x%x, paddr = 0x%x size = 0x%x , total = 0x%x\n",
			     i, vaddr, memdev[i].paddr + used_size, size,
			     memdev[i].usedsize);
			return (void *) vaddr;
		}
	}

	if (memdev_count < MEM_ALLOC_DEV_NUM)
	{
		/* request the new memory space */
		power = imem_info[memdev_count].power;	/* request mem size of 2 ^ n pages */
		pages = 1 << power;	/* total page numbers */

		// request mem
		sprintf(cmdbuf, "echo %x > %s", power,
			imem_info[memdev_count].name);
		system(cmdbuf);
		// get paddr
		fd = open(imem_info[memdev_count].name, O_RDWR);
		if (fd >= 0)
			read(fd, &paddr, 4);
		close(fd);
		/* failed, so free the memory. */
		if (paddr == 0)
		{
			printf
			    ("++ Jz47 can not get address of reserved memory\n");
			exit(1);
		} else
		{
			/* mmap the memory and record vadd and paddr. */
			vaddr =
			    (unsigned int) mmap((void *)
						imem_info[memdev_count].vaddr,
						pages * PAGE_SIZE * 2,
						(PROT_READ | PROT_WRITE),
						MAP_SHARED, mmapfd, paddr);

			memdev[memdev_count].vaddr = vaddr;
			memdev[memdev_count].paddr = paddr;
			memdev[memdev_count].totalsize = (pages * PAGE_SIZE);
			memdev[memdev_count].usedsize = 0;
			memdev_count++;
			printf
			    ("jz Dev alloc 2: vaddr = 0x%08x, paddr = 0x%08x size = 0x%08x ++\n",
			     vaddr, paddr, (pages * PAGE_SIZE));

			toggle_jz47_tlb();
		}
	}			//if (memdev_count <  MEM_ALLOC_DEV_NUM)

	for (i = 0; i < memdev_count; i++)
	{
		int alloc_size, used_size, total_size;
		used_size = memdev[i].usedsize;
		total_size = memdev[i].totalsize;
		alloc_size = total_size - used_size;

		//printf("1 [ i=%d uesd=%d total=%d alloc_size=%d size=%d ] \n",i,memdev[i].usedsize,memdev[i].totalsize,alloc_size,size);
		if (alloc_size >= size)
		{

			/* align to 16 boundary.  */
			alloc_size = (size + 15) & ~(15);
			memdev[i].usedsize = used_size + alloc_size;
			vaddr = memdev[i].vaddr + used_size;

			printf
			    ("2 jz mem alloc [%d]: vaddr = 0x%x, paddr = 0x%x size = 0x%x\n",
			     i, vaddr, memdev[i].paddr + used_size, size);

			return (void *) vaddr;
		}
	}
	printf("++ Jz47 alloc: memory allocated is failed (size = %d) ++\n",size);
	return 0;
}

/*----------------------------------------------------------------------------------*/

char *ipu_regs_name[] = {
	"REG_CTRL",		/* 0x0 */
	"REG_STATUS",		/* 0x4 */
	"REG_D_FMT",		/* 0x8 */
	"REG_Y_ADDR",		/* 0xc */
	"REG_U_ADDR",		/* 0x10 */
	"REG_V_ADDR",		/* 0x14 */
	"REG_IN_FM_GS",		/* 0x18 */
	"REG_Y_STRIDE",		/* 0x1c */
	"REG_UV_STRIDE",	/* 0x20 */
	"REG_OUT_ADDR",		/* 0x24 */
	"REG_OUT_GS",		/* 0x28 */
	"REG_OUT_STRIDE",	/* 0x2c */
	"RSZ_COEF_INDEX",	/* 0x30 */
	"REG_CSC_C0_COEF",	/* 0x34 */
	"REG_CSC_C1_COEF",	/* 0x38 */
	"REG_CSC_C2_COEF",	/* 0x3c */
	"REG_CSC_C3_COEF",	/* 0x40 */
	"REG_CSC_C4_COEF",	/* 0x44 */
	"REG_H_LUT",		/* 0x48 */
	"REG_V_LUT",		/* 0x4c */
	"REG_CSC_OFFPARA",	/* 0x50 */
};

int jz47_dump_ipu_regs(int num)
{
	int i, total;
	if (num >= 0)
	{
		//printf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[num >> 2], REG32(ipu_vbase + num));
		return 1;
	}
	if (num == -1)
	{
		total = sizeof(ipu_regs_name) / sizeof(char *);
		for (i = 0; i < total; i++)
			printf("ipu_reg: %s: 0x%x\n", ipu_regs_name[i],
			       REG32(ipu_vbase + (i << 2)));
	}
	if (num == -2)
	{
		for (i = 0; i < IPU_LUT_LEN; i++)
			printf("ipu_H_LUT(%d): 0x%x\n", i,
			       REG32(ipu_vbase + HRSZ_LUT_BASE + i * 4));
		for (i = 0; i < IPU_LUT_LEN; i++)
			printf("ipu_V_LUT(%d): 0x%x\n", i,
			       REG32(ipu_vbase + VRSZ_LUT_BASE + i * 4));
	}
	return 1;
}

/* set ipu data format.  */
int jz47_set_ipu_csc_cfg(SwsContext * c, int outW, int outH, int Wdiff,
			 int Hdiff)
{
	unsigned int in_fmt = 0, out_fmt = 0;
	int out_width, out_height;
	int in_src_w, in_src_h, in_cnt;

	const int dstFormat = c->dstFormat;
	const int srcFormat = c->srcFormat;

	switch (srcFormat)
	{
	case PIX_FMT_YUV420P:
		Hdiff = (Hdiff + 1) & ~1;
		Wdiff = (Wdiff + 1) & ~1;
		in_fmt = INFMT_YCbCr420;
		break;

	case PIX_FMT_YUV422P:
		Wdiff = (Wdiff + 1) & ~1;
		in_fmt = INFMT_YCbCr422;
		break;

	case PIX_FMT_YUV444P:
		in_fmt = INFMT_YCbCr444;
		break;

	case PIX_FMT_YUV411P:
		in_fmt = INFMT_YCbCr411;
		break;
	}

	out_width = outW;
	out_height = outH;

	//printf ("++++++++0 outW=%d dstFormat=%d\n", outW, dstFormat);
	switch (dstFormat)
	{
		//case PIX_FMT_RGB32:
	case PIX_FMT_RGBA32:
		printf("============PIX_FMT_RGBA32============\n");
		out_fmt = OUTFMT_RGB888;
		outW = outW << 2;
		REG32(ipu_vbase + REG_CTRL) |= CSC_EN;  //Emily
		break;

	case PIX_FMT_RGB555:
		out_fmt = OUTFMT_RGB555;
		outW = outW << 1;
		REG32(ipu_vbase + REG_CTRL) |= CSC_EN;  //Emily
		break;

	case PIX_FMT_RGB565:
	case PIX_FMT_BGR565:
		if(fb_w != FB_TVE_WIDTH)//not on tv
		{
			out_fmt = OUTFMT_RGB565;
			outW = outW << 1; 
			REG32(ipu_vbase + REG_CTRL) |= CSC_EN;  //Emily
		}
		else{
			out_fmt = OUTFMT_YUV422;//OUTFMT_RGB565;
		//	outW = outW << 1; //Emily
		}
		break;
	default:
		printf("no support out format, check jz47_set_ipu_csc_cfg()\n");
	}

	//printf ("++++++++1 outW=%d out_fmt=%d\n", outW, out_fmt);
	// Set GS register
//	printf("1 c->srcW=%d  Wdiff=%d  c->srcH=%d  Hdiff=%d\n",c->srcW,Wdiff,c->srcH,Hdiff);
	//printf("m=%d  n=%d\n",ipu_ratio_table[save_Wsel].m,ipu_ratio_table[save_Wsel].n);
	if (Wdiff > 0)
		while (Wdiff)
		{
			if (((c->srcW - Wdiff) * ipu_ratio_table[save_Wsel].m / ipu_ratio_table[save_Wsel].n) < FB_LCD_WIDTH)	/* lcd width */
				Wdiff--;
			else
				break;
		}
//	printf("2 c->srcW=%d  Wdiff=%d  c->srcH=%d  Hdiff=%d\n",c->srcW,Wdiff,c->srcH,Hdiff);

	REG32(ipu_vbase + REG_IN_FM_GS) =
		IN_FM_W(c->srcW - Wdiff ) | IN_FM_H((c->srcH - Hdiff) & ~1);//Emily

	REG32(ipu_vbase + REG_OUT_GS) = OUT_FM_W(outW) | OUT_FM_H(outH); 
// set Format
	REG32(ipu_vbase + REG_D_FMT) = in_fmt | out_fmt;

// set parameter
	REG32(ipu_vbase + REG_CSC_C0_COEF) = YUV_CSC_C0;
	REG32(ipu_vbase + REG_CSC_C1_COEF) = YUV_CSC_C1;
	REG32(ipu_vbase + REG_CSC_C2_COEF) = YUV_CSC_C2;
	REG32(ipu_vbase + REG_CSC_C3_COEF) = YUV_CSC_C3;
	REG32(ipu_vbase + REG_CSC_C4_COEF) = YUV_CSC_C4;
	REG32(ipu_vbase + REG_CSC_OFFPARA) = YUV_CSC_OFFPARA;

	return 0;
}

int find_ipu_ratio_factor(float ratio)
{
	int i, sel;
	float diff, min;
	sel = ipu_rtable_len;

	for (i = 0; i < ipu_rtable_len; i++)
	{
		if (ratio > ipu_ratio_table[i].ratio)
			diff = ratio - ipu_ratio_table[i].ratio;
		else
			diff = ipu_ratio_table[i].ratio - ratio;

		if (diff < min || i == 0)
		{
			min = diff;
			sel = i;
		}
	}

	//printf ("resize: sel = %d, n=%d, m=%d\n", sel, ipu_ratio_table[sel].n,ipu_ratio_table[sel].m);
	return sel;
}

int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[]);
int resize_out_cal(int insize, int outsize, int srcN, int dstM, int upScale,
		   int *diff);
int jz47_set_ipu_resize(SwsContext * c, int *outWp, int *outHp, int *Wdiff,
			int *Hdiff)
{
	int Hsel = 0, Wsel = 0;
	int srcN, dstM, width_up, height_up;
	int Height_lut_max, Width_lut_max;
	int i;

	/*printf ("\n1 c->dstW=%d, c->dstH=%d, rsize_w=%d, rsize_h=%d\n", 
	   c->dstW, c->dstH, rsize_w, rsize_h); */
	//c->dstW=fb_w, c->dstH=fb_h;//Emily
	if (rsize_w == 0)
		rsize_w = c->dstW;
	if (rsize_h == 0)
		rsize_h = c->dstH;
	/*printf ("\n2 c->dstW=%d, c->dstH=%d, rsize_w=%d, rsize_h=%d\n", 
	   c->dstW, c->dstH, rsize_w, rsize_h); */

	rsize_w = (rsize_w > fb_w) ? fb_w : rsize_w;
	rsize_h = (rsize_h > fb_h) ? fb_h : rsize_h;
	/*printf ("\n3 rsize_w=%d, rsize_h=%d  fb_w=%d  fb_h=%d\n", 
	   rsize_w, rsize_h, fb_w,fb_h); */
	*outWp = rsize_w;
	*outHp = rsize_h;
	*Wdiff = *Hdiff = 0;

	// orig size: c->srcW, c->srcH
	printf ("c->srcW = %d, c->srcH=%d, rsize_w=%d, rsize_h=%d\n", 
		c->srcW, c->srcH, rsize_w, rsize_h);

	disable_rsize(ipu_vbase);
	//Don't need do resize ?

	if ((c->srcW == rsize_w) && (c->srcH == rsize_h) || (rsize_w > 2 * c->srcW)
	    || (rsize_h > 2 * c->srcH))
	{
	
		return 0;
	}

	//init the resize factor table
#if 0
	if (!ipu_rtable_init) {
	  init_ipu_ratio_table ();
	  ipu_rtable_init = 1;
	  }
#endif
	// enable resize
	if (c->srcW != rsize_w)
		enable_hrsize(ipu_vbase);

	if (c->srcH != rsize_h)
		enable_vrsize(ipu_vbase);

	//get the resize factor
	save_Wsel = Wsel = find_ipu_ratio_factor(((float) c->srcW) / rsize_w);
	save_Hsel = Hsel = find_ipu_ratio_factor(((float) c->srcH) / rsize_h);

	//set IPU_CTRL register
	width_up = rsize_w >= c->srcW;
	height_up = rsize_h >= c->srcH;
	REG32(ipu_vbase + REG_CTRL) |=
	    (height_up << V_SCALE_SHIFT) | (width_up << H_SCALE_SHIFT);
//Emily
	if(fb_w==FB_TVE_WIDTH){
		REG32(ipu_vbase + REG_CTRL) |= 0x1<<15; //FILED_CONF_EN
		REG32(ipu_vbase + REG_CTRL) |= 0x1<<14; //DISP_SEL
	}
//Emily

	//set IPU_INDEX register
	Width_lut_max =
	    width_up ? ipu_ratio_table[Wsel].m : ipu_ratio_table[Wsel].n;
	Height_lut_max =
	    height_up ? ipu_ratio_table[Hsel].m : ipu_ratio_table[Hsel].n;
	REG32(ipu_vbase + REG_RSZ_COEF_INDEX) =
	    ((Height_lut_max - 1) << VE_IDX_SFT) | ((Width_lut_max -
						     1) << HE_IDX_SFT);

	//calculate out W/H and LUT
	srcN = ipu_ratio_table[Wsel].n;
	dstM = ipu_ratio_table[Wsel].m;
	*outWp = resize_out_cal(c->srcW, rsize_w, srcN, dstM, width_up, Wdiff);
	resize_lut_cal(srcN, dstM, width_up, h_lut);

	srcN = ipu_ratio_table[Hsel].n;
	dstM = ipu_ratio_table[Hsel].m;
	*outHp = resize_out_cal(c->srcH, rsize_h, srcN, dstM, height_up, Hdiff);
	resize_lut_cal(srcN, dstM, height_up, v_lut);
	//printf("srcN=%d  dstM=%d  width_up=%d height_up=%d\n",srcN,dstM,width_up,height_up);
#if 0
#ifdef IPU_TO_LCD
	/* IPU OUT_GS must equal to LCDC LCDSIZE1, or frame will switch and sroll */
	printf("*outWp=%d  rsize_w=%d\n",*outWp,rsize_w); //Emily
	if (*outWp < rsize_w)
		*outWp = rsize_w;
	if (*outHp < rsize_h)
		*outHp = rsize_h;
#endif
#endif//Emily

#if 0
	int aaa;
	printf("Width_lut_max=%d\n", Width_lut_max);
	sleep(5);
	for (aaa = 0; aaa < Width_lut_max; aaa++)
		printf("h_lut[%d] = { %d : %d : %d }\n", aaa, h_lut[aaa].coef,
		       h_lut[aaa].in_n, h_lut[aaa].out_n);
	printf("Height_lut_max=%d\n", Height_lut_max);
	for (aaa = 0; aaa < Height_lut_max; aaa++)
		printf("v_lut[%d] = { %d : %d : %d }\n", aaa, v_lut[aaa].coef,
		       v_lut[aaa].in_n, v_lut[aaa].out_n);
	sleep(2);
#endif

	// set IPU LUT register
	REG32(ipu_vbase + VRSZ_LUT_BASE) = (1 << START_N_SFT);
	for (i = 0; i < Height_lut_max; i++)
		REG32(ipu_vbase + VRSZ_LUT_BASE) = (v_lut[i].coef << W_COEF_SFT)
		    | (v_lut[i].in_n << IN_N_SFT) | (v_lut[i].
						     out_n << OUT_N_SFT);

	REG32(ipu_vbase + HRSZ_LUT_BASE) = (1 << START_N_SFT);
	for (i = 0; i < Width_lut_max; i++)
		REG32(ipu_vbase + HRSZ_LUT_BASE) = (h_lut[i].coef << W_COEF_SFT)
		    | (h_lut[i].in_n << IN_N_SFT) | (h_lut[i].
						     out_n << OUT_N_SFT);

	return 0;
}

int resize_out_cal(int insize, int outsize, int srcN, int dstM,
		   int upScale, int *diff)
{
	float calsize;
	int delta;
	float tmp, tmp2;
	delta = 1;

	do
	{
		tmp = (insize - delta) * dstM / (float) srcN;
		tmp2 = tmp * srcN / dstM;
		if (upScale)
		{
			if (tmp2 == insize - delta)
				calsize = tmp + 1;
			else
				calsize = tmp + 2;
		} else		// down Scale
		{
			if (tmp2 == insize - delta)
				calsize = tmp;
			else
				calsize = tmp + 1;
		}
		delta++;
	}
	while (calsize > outsize);

	*diff = delta - 2;

	return ((int) calsize);
}

int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[])
{
	int i, t, x;
	float w_coef, factor, factor2;

	if (upScale)
	{
		for (i = 0, t = 0; i < dstM; i++)
		{
			factor = (float) (i * srcN) / (float) dstM;
			factor2 = factor - (int) factor;
			w_coef = 1.0 - factor2;
			lut[i].coef =
			    ((unsigned int) (512.0 * w_coef) & W_COEF_MSK);
// calculate in & out
			lut[i].out_n = 1;
			if (t <= factor)
			{
				lut[i].in_n = 1;
				t++;
			} else
				lut[i].in_n = 0;
		}		// end for
	} else
	{
		for (i = 0, t = 0, x = 0; i < srcN; i++)
		{
			factor = (float) (t * srcN + 1) / (float) dstM;
			if (dstM == 1)
			{
// calculate in & out
				lut[i].coef = (i == 0) ? 256 : 0;
				lut[i].out_n = (i == 0) ? 1 : 0;
			} else if (((t * srcN + 1) / dstM - i) >= 1)
				lut[i].coef = 0;
			else if (factor - i == 0)
			{
				lut[i].coef = 512;
				t++;
			} else
			{
				factor2 = (float) (t * srcN) / (float) dstM;
				factor = factor - (int) factor2;
				w_coef = 1.0 - factor;
				lut[i].coef =
				    ((unsigned int) (512.0 *
						     w_coef) & W_COEF_MSK);
				t++;
			}
// calculate in & out
			lut[i].in_n = 1;
			if (dstM != 1)
			{
				if (((x * srcN + 1) / dstM - i) >= 1)
					lut[i].out_n = 0;
				else
				{
					lut[i].out_n = 1;
					x++;
				}
			}
		}		/* for end down Scale */
	}			/* else upScale */
	return 0;
}


int jz47_ipu_init(struct vf_instance_s *vf)
{
	SwsContext *c = vf->priv->ctx;
	int outW, outH, Wdiff, Hdiff;
	struct fb_fix_screeninfo fbfix;
	mvom_lcd_get_size(&fb_w, &fb_h); 
//	ioctl(fbfd,FBIODISPOFF,NULL);//disable lcdc
	outW = c->dstW;
	outH = c->dstH;

	printf("ipu 0  fb_w=%d  fb_h=%d outW=%d, outH=%d \n",
	       fb_w, fb_h ,outW, outH);

#if 0
#ifdef IPU_TO_LCD
	if (!set_fg1_flag)
	{
		set_lcd_fg1(&g_mvoc);
		set_fg1_flag = 1;
	}
#endif
	if (ipu_is_enable(ipu_vbase))
		while (!polling_end_flag(ipu_vbase));
       

#endif
	stop_ipu(ipu_vbase);
	reset_ipu(ipu_vbase);
       
	clear_end_flag(ipu_vbase);

	//disable irq
	disable_irq(ipu_vbase);

	ipu_size_cfg = g_mvoc.resize_mode;

	if (ipu_size_cfg)
	{
		outW = c->srcW;
		outH = c->srcH;
		switch (ipu_size_cfg)
		{
		case MP_IMAGE_TYPE_CUSTORM:
			break;
		case MP_IMAGE_TYPE_INITSIZE:
			if ((outH <= FB_LCD_HEIGHT) && (outW <= FB_LCD_WIDTH))
			{
				rsize_w = outW;
				rsize_h = outH;
				out_x = (FB_LCD_WIDTH - outW) / 2;
				out_y = (FB_LCD_HEIGHT - outH) / 2;
			} else
				eqscale(outW, outH);
			break;
		case MP_IMAGE_TYPE_EQSIZE:
			eqscale(outW, outH);
			break;
		case MP_IMAGE_TYPE_FULLSCREEN:
			fullscreen(outW, outH);
			break;
		}
	}			//ipu_size_cfg
	// set IPU resize field
	jz47_set_ipu_resize(c, &outW, &outH, &Wdiff, &Hdiff);
	printf("ffffffffffffffffffoutW=%d outH=%d \n",outW, outH);
	// set CSC parameter and format
	jz47_set_ipu_csc_cfg(c, outW, outH, Wdiff, Hdiff);
	
	frame_offset = (out_x + out_y * FB_LCD_WIDTH) * LCD_BPP_BYTE;
	out_w = outW;
	out_h = outH;
	
	ipu_size_cfg = 0;

	// set out frame buffer
#ifdef NEED_COPY_IMGBUF
	img_ptr = (unsigned char *) jz4740_alloc_frame(32, fbmemlen);
	REG32(ipu_vbase + REG_OUT_ADDR) =
		get_phy_addr((unsigned int) img_ptr);
#else	

#ifdef IPU_TO_LCD	
	enable_ipu_direct(ipu_vbase);
	enable_ipu_addrsel(ipu_vbase);
//	jz4755_ipu_init(c);
#else
	disable_ipu_direct(ipu_vbase);
	REG32(ipu_vbase + REG_OUT_ADDR) = phy_fb+frame_offset;
#endif
#endif

/****************************************************************************/
	printf("ipu sssssssssssssssssssssssss out_x=%d  out_y=%d outW=%d outH=%d \n",
	       out_x, out_y,outW, outH);

	mvom_lcd_setup(&g_mvoc, out_x, out_y, out_w, out_h);
	ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
	fb_line_len = fbfix.line_length;
	REG32(ipu_vbase + REG_OUT_STRIDE) = fb_line_len;
/****************************************************************************/
	ipu_inited = 1;

//
//	run_ipu(ipu_vbase);

	set_fg1_flag = 0;
	first_run_ipu = 0;
//	ioctl(fbfd,FBIODISPOFF,NULL);//disable lcdc
	return 0;
}

void eqscale(int w, int h)
{
	int xscale, yscale;
	int scale;

	xscale = w * 1000 / FB_LCD_WIDTH;
	yscale = h * 1000 / FB_LCD_HEIGHT;

	scale = xscale > yscale ? xscale : yscale;

	if (scale <= 500)
		scale = 500;
	rsize_w = w * 1000 / scale;
	rsize_h = h * 1000 / scale;

	if (rsize_h > FB_LCD_HEIGHT)
		rsize_h = FB_LCD_HEIGHT;
	if (rsize_w > FB_LCD_WIDTH)
		rsize_w = FB_LCD_WIDTH;

	out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	out_y = (FB_LCD_HEIGHT - rsize_h) / 2;


}
static void fullscreen(int w, int h)
{
	int xscale, yscale;
	int scale;
	int scrw, scrh;
	xscale = w * 1000 / FB_LCD_WIDTH;
	yscale = h * 1000 / FB_LCD_HEIGHT;

	if (xscale <= 500)
		rsize_w = w * 1000 / 500;
	else
		rsize_w = FB_LCD_WIDTH;
	if (yscale <= 500)
		rsize_h = h * 1000 / 500;
	else
		rsize_h = FB_LCD_HEIGHT;

	if (rsize_h > FB_LCD_HEIGHT)
		rsize_h = FB_LCD_HEIGHT;
	if (rsize_w > FB_LCD_WIDTH)
		rsize_w = FB_LCD_WIDTH;

	out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	out_y = (FB_LCD_HEIGHT - rsize_h) / 2;
}

void toggle_jz47_tlb(void)
{
	int cur_pid = 0;
	unsigned char cmdbuf[100];

	if (!toggle_flag && memdev[0].vaddr && memdev[0].paddr)
	{
		toggle_flag = 1;
		cur_pid = getpid();

		sprintf(cmdbuf, "echo 00%08x 00%08x 00%08x 00%08x > %s",
			cur_pid, memdev[0].vaddr, memdev[0].paddr, 0x400000,
			"/proc/jz/ipu");

		//printf("toggle : system call command: cmdbuf=%s\n",cmdbuf);
		system(cmdbuf);
	}

	return;
}

void free_jz47_tlb(void)
{
	int cur_pid = 0;
	unsigned char cmdbuf[100];

	if (toggle_flag)
	{
		sprintf(cmdbuf, "echo %s > %s", "release tlb", "/proc/jz/ipu");
		//printf("free : system call command: cmdbuf=%s\n",cmdbuf);
		system(cmdbuf);
		jz4740_free_devmem();
		toggle_flag = 0;
	}

	return;
}

void ipu_image_start()
{
	jz4740_free_devmem();
	ipu_inited = 0;
	ipu_image_completed = 0;
	first_run_ipu = 0;
	rsize_w = 0;
	rsize_h = 0;
	out_x = out_y = 0;
	ipu_maped = 0;
	frame_offset = 0;
	toggle_flag = 0;
	set_fg1_flag = 0;
}

void ipu_outsize_changed(int x, int y, int w, int h)
{
	out_x = x;
	out_y = y;
	if ((w == 0) && (h == 0))
	{
		if ((rsize_w == 0) && (rsize_h == 0))
			return;
		rsize_w = 0;
		rsize_h = 0;
		ipu_size_cfg = 1;
	} else
	{
		if (rsize_w != w || rsize_h != h)
		{
			rsize_w = w;
			rsize_h = h;
			ipu_size_cfg = 1;
		}
	}
}

#if 0
void copy_image_to_fb()
{
	memcpy((void *) frame_buffer, (void *) img_ptr, fbmemlen);
	return;
}
#endif

static unsigned int buff0 = 0, buff1 = 0;

int jz47_put_image_with_ipu(struct vf_instance_s *vf, mp_image_t * mpi,
			    double pts)
{
	unsigned int paddr;
	SwsContext *c = vf->priv->ctx;
	unsigned int img_area = c->srcW * c->srcH;
	int ret;
	static int first_print = 0;
//	MPLAYER_DBG();

#if 0
	if (((unsigned int) mpi->planes[0]) == buff0
	    || ((unsigned int) mpi->planes[0]) == buff1)
		printf
		    (" +++ found buff swaping error: 0=0x%08x, 1=0x%08x, curr=0x%08x\n",
		     buff0, buff1, (unsigned int) mpi->planes[0]);
	buff0 = buff1;
	buff1 = (unsigned int) mpi->planes[0];
#endif
//Emily
	if (!first_run_ipu){
		//enable lcdc
		//	ioctl(fbfd,FBIODISPOFF,NULL);
		stop_ipu(ipu_vbase);
		//	jz47_dump_ipu_regs();
		//	if (ipu_is_enable(ipu_vbase))
		//	while (!polling_end_flag(ipu_vbase));
		//	printf("-----------polling_end_flag-----???------\n");
		clear_end_flag(ipu_vbase);
	}
	if (!ipu_inited)
		jz47_ipu_init(vf);

#ifdef NEED_COPY_IMGBUF
	if (ipu_image_completed)
		copy_image_to_fb();
#endif
	//printf("reg_addr_ctrl=0x%08x\n",REG32(ipu_vbase + REG_ADDR_CTRL));
	/*if (REG32(ipu_vbase + REG_ADDR_CTRL) == 0x0) */
	{
		// set Y addr register (mpi->planes[0])
		/*printf("Y:0x%08x  U:0x%08x  V:0x%08x\n",
		   (unsigned int)mpi->planes[0],
		   (unsigned int)mpi->planes[1],
		   (unsigned int)mpi->planes[2]); */
		paddr = get_phy_addr((unsigned int) mpi->planes[0]);
		REG32(ipu_vbase + REG_Y_ADDR) = paddr;

		// set U addr register (mpi->planes[1])
		paddr = get_phy_addr((unsigned int) mpi->planes[1]);
		REG32(ipu_vbase + REG_U_ADDR) = paddr;

		// set V addr register (mpi->planes[2])
		paddr = get_phy_addr((unsigned int) mpi->planes[2]);
		REG32(ipu_vbase + REG_V_ADDR) = paddr;
		enable_yuv_ready(ipu_vbase);
	}

	// set Y(mpi->stride[0]) stride
	REG32(ipu_vbase + REG_Y_STRIDE) = mpi->stride[0];
	//REG32(ipu_vbase + REG_Y_STRIDE) = mpi->stride[0];
	// set U(mpi->stride[1]) V(mpi->stride[2]) stride
	REG32(ipu_vbase + REG_UV_STRIDE) =
	    U_STRIDE(mpi->stride[1]) | V_STRIDE(mpi->stride[2]);

	if (!first_run_ipu){
		//	clear_end_flag(ipu_vbase);
		run_ipu(ipu_vbase);
		first_run_ipu=1;
		//enable lcdc
		//	ioctl(fbfd,FBIODISPON,NULL);
	}
	ipu_image_completed = 1;
#if 1
#ifdef IPU_TO_LCD
	if (!set_fg1_flag)
	{
		set_lcd_fg1(&g_mvoc);
		set_fg1_flag = 1;
	}
#endif
#endif
#if 1
	if (first_print==100) {
		jz47_dump_ipu_regs(-1);
		first_print++;
	}
#endif
	return 1;
}

// copy from vf_scale.c

static void scale(struct SwsContext *sws1, struct SwsContext *sws2,
		  uint8_t * src[3], int src_stride[3], int y, int h,
		  uint8_t * dst[3], int dst_stride[3], int interlaced)
{
	if (interlaced)
	{
		int i;
		uint8_t *src2[3] = { src[0], src[1], src[2] };
		uint8_t *dst2[3] = { dst[0], dst[1], dst[2] };
		int src_stride2[3] =
		    { 2 * src_stride[0], 2 * src_stride[1], 2 * src_stride[2] };
		int dst_stride2[3] =
		    { 2 * dst_stride[0], 2 * dst_stride[1], 2 * dst_stride[2] };

		sws_scale_ordered(sws1, src2, src_stride2, y >> 1, h >> 1, dst2,
				  dst_stride2);
		for (i = 0; i < 3; i++)
		{
			src2[i] += src_stride[i];
			dst2[i] += dst_stride[i];
		}
		sws_scale_ordered(sws2, src2, src_stride2, y >> 1, h >> 1, dst2,
				  dst_stride2);
	} else
	{
		sws_scale_ordered(sws1, src, src_stride, y, h, dst, dst_stride);
	}
}

static int visual = 0;
void drop_image()
{
	visual = 0;
}

int jz47_put_image(struct vf_instance_s *vf, mp_image_t * mpi, double pts)
{
//	MPLAYER_DBG();
	if (!vf || !mpi)
		return 0;

	mp_image_t *dmpi = mpi->priv;

#if 0
	if (put_image_cnt <= 1)
	{
		put_image_cnt++;
		if (put_image_cnt == 2)
			toggle_jz47_tlb();
	}
#endif

	if (!(mpi->flags & MP_IMGFLAG_DRAW_CALLBACK && dmpi))
	{
		if (mpi->pict_type == 1)
			visual = 1;
		if (visual || (mpi->pict_type == 0))
			jz47_put_image_with_ipu(vf, mpi, pts);
	}

	return 0;
}



static int ipu_init(void)
{
	if (!ipu_maped)
	{
		ipufd = open ("/dev/mem", O_RDWR);
		if (ipufd < 0) {
			printf("++ open /dev/mem error. program aborted. ++\n");
			return 0;
		}
		ipu_vbase = mmap((void *)0, IPU__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ipufd, IPU__OFFSET);
		ipu_maped = 1;
	}
	ipu_image_start();
}


/* Mplayer output interface module */
int mvom_init(void)
{
//	mvom_data_init();
	mvom_lcd_init();
	ipu_init();
//	ahb_get_bus_clock();
}

void mvom_exit(void)
{
	printf("mplayer exit!\n");

	free_jz47_tlb();
	ipu_image_start();
//	mvom_lcd_exit();
}

int mvom_config(struct mplayer_video_output_config_t *mvoc)
{
	int ret;

	/* check start position */
	if (out_x < 0 || out_y < 0 
	    || out_x > fb_w || out_y > fb_h)
	{
		printf("mvom_config par check fail! \n");
		return -1;
	}
	memcpy(&g_mvoc, mvoc, sizeof(struct mplayer_video_output_config_t));
	
	ipu_inited = 0;
	return 0;
}

/* Mplayer output interface module */
