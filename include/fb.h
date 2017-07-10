#ifndef __FB_H
#define __FB_H

#include <config.h>

#define FBDEVICE		"/dev/fb0"


#define ROW				(600)
#define COL				(1024)
#define HOZVAL			(COL-1)
#define LINEVAL			(ROW-1)

#define XSIZE			COL
#define YSIZE			ROW

// 常用颜色定义
#define BLUE	0x0000FF
#define RED		0xFF0000
#define GREEN	0x00FF00
#define WHITE	0xFFFFFF
#define BLACK	0x000000

typedef  unsigned int u32;


extern int fb_open(void);

extern int fb_close(void);

extern void lcd_draw_background(u32 color);

extern void lcd_draw_image1( struct pic_info *pic);
extern void lcd_draw_image2( struct pic_info *pic);
void lcd_draw_image3( struct pic_info *pPic);


void lcd_draw_image(unsigned char *pic);





#endif






















