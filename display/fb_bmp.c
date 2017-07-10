/*本文件用来解析BMP图片 并显示到fb中*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <fb_bmp.h>
#include "bmp.h"
#include <fb.h>
#include <debug.h>
#include <config.h>
#include <string.h>





/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:判断是否为bmp图片
*
*/
int is_bmp(char *pathname)
{
	int fd = -1;
	int ret = -1;
	unsigned char buf[2]={0};
	
	/*第一步 打开bmp文件*/
	fd = open(pathname, O_RDONLY);
	if(fd < 0 )
	{
		debug(stderr, "open %s is fail \n",pathname);
		ret = -1;
		goto close;
		
	}
	debug("open is success ! \n");

	/*第二步:读取文件*/
	ret = read(fd, buf, 2);
	if (ret != 2)
	{
		fprintf(stderr, "read %s is fail \n",pathname);
		ret = -1;
		goto close;
	}	
	debug("read is success ! \n");

	/*第三步:判断是否为bmp文件*/
	if ((buf[0] == 'B') && (buf[1] == 'M'))
	{
		
		ret = 0 ;
		goto close;
	}
	else
	{
		fprintf(stderr, "%s is not a real bmp file \n",pathname);
		ret = -1;
		goto close;
	}

close:
	close(fd);
	return ret;

	return 0;

}



/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:解析bmp文件 并将解析出来的数据丢到bmp_buf数组中
*
***************************************************************************/
static int bmp_analyse(struct pic_info *pic)
{
	int fd = -1;
	int res = -1;
	int len = -1;
	BitMapFileHeader fHeader;
	BitMapInforHeader infoHeader;
	
	/*第一步 打开bmp文件*/
	fd = open(pic->pathname, O_RDONLY);
	if(fd < 0 )
	{
		fprintf(stderr, "open %s is fail \n", pic->pathname);
		return -1;
	}

	/*第二步:读取文件*/
	res = read(fd, &bfType, 2);
	debug("------%c%c--------\n",*((unsigned char *)&bfType),*((unsigned char *)&bfType+1));


	res = read(fd, &fHeader, sizeof(fHeader));
	debug("The size is %ld Byte \n",fHeader.bfSize);
	debug("The bfOffBits is %ld \n",fHeader.bfOffBits);
	
	res = read(fd, &infoHeader, sizeof(infoHeader));
	debug("The resolution is %ld * %ld \n", infoHeader.biWidth, infoHeader.biHeight);
	debug("The biBitCount is %d  \n", infoHeader.biBitCount);

	memset(rgb_buf, 0, sizeof(rgb_buf));
	lseek(fd, fHeader.bfOffBits, SEEK_SET );
	len = infoHeader.biWidth * infoHeader.biHeight * (infoHeader.biBitCount/8);
	debug("len =  %ld Byte \n",len);
	res = read(fd, pic->pdata, len);
	debug("res =  %d  \n", res);

	// 图片数据填充
	pic->hight	 = infoHeader.biHeight;
	pic->width	 = infoHeader.biWidth ;
	pic->bpp  	 = infoHeader.biBitCount;
//	pic->pdata	 = bmp_buf;

	//lcd_draw_image(bmp_buf);
	lcd_draw_image3(pic);
	
	close(fd);

	return 0;

}

/******************************************************************* 
*
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:显示bmp图片
*
***************************************************************************/

int display_bmp(const char *pathname)
{

	struct pic_info pic;
	
	if(is_bmp(pathname))
	{
		printf("this file is not a jpg file! open fail!!! \n");
		return -1;
	}

	pic.pathname = pathname;
	pic.pdata = rgb_buf;

	return bmp_analyse(&pic);

}













































