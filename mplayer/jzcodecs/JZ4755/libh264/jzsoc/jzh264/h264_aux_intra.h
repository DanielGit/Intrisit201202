
#ifndef __H264_AUX_INTRA_H__
#define __H264_AUX_INTRA_H__


#define MUL_12 0x00010002
#define MUL_34 0x00030004
#define MUL_56 0x00050006
#define MUL_78 0x00070008
#define MUX_H16 0x0000ffff

// for block 4x4
#define VERT_PRED             0
#define HOR_PRED              1
#define DC_PRED               2
#define DIAG_DOWN_LEFT_PRED   3
#define DIAG_DOWN_RIGHT_PRED  4
#define VERT_RIGHT_PRED       5
#define HOR_DOWN_PRED         6
#define VERT_LEFT_PRED        7
#define HOR_UP_PRED           8
#define LEFT_DC_PRED          9
#define TOP_DC_PRED           10
#define DC_128_PRED           11
// for block 8x8 and 16x16
#define DC_PRED8x8            0
#define HOR_PRED8x8           1
#define VERT_PRED8x8          2
#define PLANE_PRED8x8         3
#define LEFT_DC_PRED8x8       4
#define TOP_DC_PRED8x8        5
#define DC_128_PRED8x8        6

void Intra_pred_chroma(int chroma_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *top);

void Intra_pred_luma_4x4(int luma_4x4_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *topright, uint8_t *top, uint8_t *topleft);

void Intra_pred_luma_16x16(int luma_16x16_pred_mode, uint8_t *dst, uint8_t *src, uint8_t *top);

static void predict_8x8_load_left( uint8_t *l, int has_topleft, uint8_t *src, uint8_t *topleft, int stride );
static void predict_8x8_load_top(uint8_t *t, int has_topleft, int has_topright, uint8_t *src, uint8_t *top, uint8_t *topleft );
static void predict_8x8_load_topright(uint8_t *t, int has_topright, uint8_t *top );

static uint32_t predict_8x8_dc( uint8_t *src);

void Intra_pred_luma_8x8l(int luma_8x8l_pred_mode, uint8_t *dst, int has_topleft, int has_topright, uint8_t *src, uint8_t *top, uint8_t *topleft, int stride);

#endif // __H264_AUX_INTRA_H__
