#ifndef __CONFIG_H
#define __CONFIG_H


typedef struct pic_info
{
	unsigned char *pathname;		//文件名
	unsigned int width;				//图片宽度
	unsigned int hight;				//图片高度
	unsigned int bpp;				
	unsigned char *pdata;			//图片数据

}pic_info;

/*我们规定最大支持1920*1080的图片 BPP最大24*/
#define BMP_MAX_RES		(1920*1080)
#define BMP_BUF_SIZE	(BMP_MAX_RES*3)
#define PATHNAME_MAX	255
#define IMAGES_MAX		255


unsigned char rgb_buf[BMP_BUF_SIZE];



#define DEVICE_TOUCHSCREEN		"/dev/input/event1"
#define TOUCH_WIDTH				200




int is_jpeg(char *pathmame);
int is_bmp(char *pathmame);
int is_png(char *pathmame);



int display_jpg(const char *pathname);
int display_bmp(const char *pathname);
int display_png(const char *pathname);




#endif









