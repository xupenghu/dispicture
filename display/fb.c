/***************************************************************************
*fb.c 操作frambuffer的基础代码，包括fb的打开 ioctl获取信息 基本的fb测试代码
*
*
*
*****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>

#include "image.h"


#include <fb.h>
#include <debug.h>
#include <config.h>


unsigned int *pfb = NULL ;
static int fb_fd = -1;  



// 在像素点(x, y)处填充为color颜色
static void lcd_draw_pixel(u32 x, u32 y, u32 color)
{
	*(pfb + COL * y + x) = color;
}

// 把整个屏幕全部填充成一个颜色color
void lcd_draw_background(u32 color)
{
	u32 i, j;
	
	for (j=0; j<ROW; j++)
	{
		for (i=0; i<COL; i++)
		{
			lcd_draw_pixel(i, j, color);
		}
	}
}

// 绘制横线，起始坐标为(x1, y)到(x2, y),颜色是color
static void lcd_draw_hline(u32 x1, u32 x2, u32 y, u32 color)
{
	u32 x;
	
	for (x = x1; x<x2; x++)
	{
		lcd_draw_pixel(x, y, color);
	}
}

void lcd_draw_image(unsigned char *pic)
 {
 	unsigned char *p = pic ;
	u32 x, y;
	u32 cnt,a;
	a = 600*902*3 ;
	
	for (y=0; y<600; y++)       //行
	{
		for (x=0; x<902; x++)     //列
		{
			cnt = COL * y + x; //定位需要写的像素点
			*(pfb + cnt) = ((p[a + 2 ]<<16) | (p[a + 1 ]<<8) | (p[a + 0 ]<<0));
			a -= 3;
		}
	}

}


void lcd_draw_image1( struct pic_info *pPic)
 {
	 const char *pData = (const char *)pPic->pdata; 	 // 指针指向图像数组
	 unsigned int cnt = 0, a = 0;
	 unsigned int x, y;
	 
	 debug("image resolution: %d * %d, bpp=%d.\n", 
		 pPic->width, pPic->hight, pPic->bpp);
	 
	 if ((pPic->bpp != 32) && (pPic->bpp != 24))
	 {
		 fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		 return;
	 }
	 
	 a = pPic->hight * pPic->width * 3 - 3;
	 for (y=0; y<pPic->hight; y++)
	 {
		 for (x=0; x<pPic->width; x++)
		 { 
			 //cnt表示当前像素点的编号
			 cnt = COL * y + x;
			 // 当前像素点对应的图像数据的RGB就应该分别是:
			 // pData[cnt+0]  pData[cnt+1]	pData[cnt+2]  
			 // 当前像素点的数据
			 *(pfb + cnt) = ((pData[a+0]<<16) | (pData[a+1]<<8)| (pData[a+2]<<0)); 
			 //*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			 a -= 3;
		 }
	 }


}

void lcd_draw_image2( struct pic_info *pPic)
 {
	 const char *pData = (const char *)pPic->pdata; 	 // 指针指向图像数组
	 unsigned int cnt = 0, a = 0;
	 unsigned int x, y;
	 
	 debug("image resolution: %d * %d, bpp=%d.\n", 
		 pPic->width, pPic->hight, pPic->bpp);
	 
	 if ((pPic->bpp != 32) && (pPic->bpp != 24))
	 {
		 fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		 return;
	 }
	 
	 a = 0;
	 for (y=0; y<pPic->hight; y++)
	 {
		 for (x=0; x<pPic->width; x++)
		 { 
			 //cnt表示当前像素点的编号
			 cnt = COL * y + x;
			 // 当前像素点对应的图像数据的RGB就应该分别是:
			 // pData[cnt+0]  pData[cnt+1]	pData[cnt+2]  
			 // 当前像素点的数据
			 *(pfb + cnt) = ((pData[a+0]<<16) | (pData[a+1]<<8)| (pData[a+2]<<0)); 
			 //*p = ((pData[cnt+0]<<16) | (pData[cnt+1]<<8)| (pData[cnt+2]<<0)); 
			 a += 3;
		 }
	 }

	printf("\n");

}

void lcd_draw_image3( struct pic_info *pPic)
 {
	 const char *pData = (const char *)pPic->pdata; 	 // 指针指向图像数组
	 unsigned int cnt = 0, a = 0;
	 unsigned int x, y;
	 
	 debug("image resolution: %d * %d, bpp=%d.\n", 
		 pPic->width, pPic->hight, pPic->bpp);
	 
	 if ((pPic->bpp != 32) && (pPic->bpp != 24))
	 {
		 fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		 return;
	 }
	 
	 a = pPic->hight * pPic->width * 3 - 3;
	 for (y=0; y<pPic->hight; y++)
	 {
		 for (x=0; x<pPic->width; x++)
		 { 
			 //cnt表示当前像素点的编号
			 cnt = COL * y + x;
			 // 当前像素点对应的图像数据的RGB就应该分别是:
			 // pData[cnt+0]  pData[cnt+1]	pData[cnt+2]  
			 // 当前像素点的数据
			 *(pfb + cnt) = ((pData[a+0]<<0) | (pData[a+1]<<8)| (pData[a+2]<<16)); 
			 a -= 3;
		 }
	 }

}

int fb_open(void)
{
	int ret = -1;
	struct fb_fix_screeninfo finfo = {0};
	struct fb_var_screeninfo vinfo = {0};
	/*打开设备*/
	fb_fd = open(FBDEVICE, O_RDWR);
	if(fb_fd<0)
	{
		perror("open");
		return 0;
	}
	debug("open file :"FBDEVICE" success... \n");
	
	/*获取硬件设备信息*/
	ret = ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
	if(ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	debug("smem_start = 0x%x; smem_len = %d \n" , finfo.smem_start, finfo.smem_len);
	
	ret = ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	if(ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	debug("xres		= %4d; yres		= %4d \n" , vinfo.xres, vinfo.yres);
	debug("xres_virtual	= %4d; yres_virtual	= %4d \n" , vinfo.xres_virtual, vinfo.yres_virtual);
	debug("xoffset		= %4d; yoffset		= %4d \n" , vinfo.xoffset, vinfo.yoffset);

#if 0	
	/*修改驱动中的屏幕分辨率*/
	vinfo.xres = COL;
	vinfo.yres = ROW;
	vinfo.xres_virtual = COL;
	vinfo.yres_virtual = ROW*2;
	ret = ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo);
	if(ret < 0)
	{
		perror("ioctl");
		return -1;
	}
	printf("**********************************************\n");
	printf("xres		= %4d; yres		= %4d \n" , vinfo.xres, vinfo.yres);
	printf("xres_virtual	= %4d; yres_virtual	= %4d \n" , vinfo.xres_virtual, vinfo.yres_virtual);
	printf("xoffset		= %4d; yoffset		= %4d \n" , vinfo.xoffset, vinfo.yoffset);
#endif	
	
	/*进行mmap*/
	pfb = mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if (NULL == pfb)
	{
		perror("mmap");
		return -1;
	}
	debug("pfb = %p \n", pfb);

	


	return 0 ;
}



int fb_close(void)
{
	close(fb_fd);
	return 0;
}






















