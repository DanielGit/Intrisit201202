#ifndef _ANDROID_MVOM_H_
#define _ANDROID_MVOM_H_
#include <jz4750_android_ipu.h>
enum resize_mode_t
{
	MVOM_CUSTORM  = 1,          //自定义模式,应用指定
	MVOM_INITSIZE,              //原始尺寸
	MVOM_EQSIZE,                //等比全屏
	MVOM_FULLSCREEN,            //按屏幕大小全屏
};


struct mplayer_video_output_config_t
{
	int view_w,veiw_h;	                  //应用获得的显示窗口大小,仅供参考
	int vo_x,vo_y,vo_w,vo_h;                  //自定义缩放模式的时候,指定位置和大小
	enum resize_mode_t resize_mode;                //缩放方式,共四种
	unsigned int fg0_colorkey;                //FG0的colorkey
	unsigned int fg0_alpha;                   //FG0的alpha
	unsigned int bg_color;                    //BG的颜色
	int ipu_direct_enable;                    //是否使用IPU直通
};


/* Ingenic android lcd IOCTL define start */
struct android_display_info_t {
	unsigned int flag;
	unsigned int fg0_number; /**/
	unsigned int fg0_index;
	unsigned int fg0_alpha; /* */
	unsigned int fg0_colorkey;/**/
	unsigned int fg0_enable;/**/
	unsigned int fg0_x;        /*fg0 start position x*/
	unsigned int fg0_y;        /*fg0 start position y*/
	unsigned int fg0_w;        /*the weight of fg0*/
	unsigned int fg0_h;        /*the height of fg0*/
	unsigned int fg1_x;        /*fg1 start position x*/
	unsigned int fg1_y;        /*fg1 start position y*/
	unsigned int fg1_w;        /*the weight of fg1*/
	unsigned int fg1_h;        /*the height of fg1*/
	unsigned int fg1_enable;   /*start or stop fg1*/
	unsigned int fg1_short_cut;/*IPU direct*/
        unsigned int ipu_cmd;      /*for ipu_ioctl command */
        img_param_t *buff;                /*for ipu_ioctl*/
        unsigned char *ipu_vbase;
};

#define FBIO_ANDROID_CTL                        0xad10

#define ANDROID_GET_DISPLAY_NUM 		0x00000001 
#define ANDROID_GET_DISPLAY_INFO 		0x00000002 
#define ANDROID_SET_DISPLAY_INDEX		0x00000004 
#define ANDROID_SET_FG0_ALPHA   		0x00000008
#define ANDROID_SET_FG0_COLORKEY  		0x00000010
#define ANDROID_SET_FG0_ENABLE    		0x00000020
#define ANDROID_SET_FG1_POS       		0x00000040
#define ANDROID_SET_FG1_SIZE       		0x00000080
#define ANDROID_SET_FG1_ENABLE      		0x00000100
#define ANDROID_SET_FG1_IPU_DIRECT      	0x00000200
#define ANDROID_GET_PANEL_SIZE                  0x00000400
#define ANDROID_IPU_IOCTL                       0x00000800
#define ANDROID_FG1_RESIZE                      0x00001000
#define ANDROID_SET_IPU                         0x00002000
#define ANDROID_SET_IPU_VBASE                   0x00004000
#define ANDROID_SET_LCD_PANEL                   0x00008000
/* Ingenic android lcd IOCTL define start */

int mvom_lcd_init(void);
int mvom_lcd_exit(void);
int mvom_lcd_setup(struct mplayer_video_output_config_t *mvoc, int x, int y, int w, int h);
int mvom_lcd_get_size(unsigned int *display_mode, int *w, int *h);
#endif //_ANDROID_MVOM_H_
