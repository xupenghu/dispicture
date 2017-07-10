/*********************************************************************************
*png文件解码文件
*2017.6.4
*powered by xph
*********************************************************************************/

#include <string.h>
#include <stdio.h>
#include <zlib.h>
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>

#include <config.h>
#include <debug.h>

/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:是否为png图片
*
*/
#define PNG_BYTES_TO_CHECK 4
int is_png(char *pathname)
{
	FILE *fp;
	  char buf[PNG_BYTES_TO_CHECK];   
	  /* Open the prospective PNG file. */   
	  if ((fp = fopen(pathname, "rb")) == NULL)      
	  		return -1;   
	  /* Read in some of the signature bytes */   
	  if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)      
	  		return -1;  
	  debug("%x %x %x %x \n", buf[0], buf[1], buf[2], buf[3]);
	  /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.      
	 	 Return nonzero (true) if they match */   
	  return(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:解析png文件 并将解析出来的数据丢到rgb_buf数组中
*
***************************************************************************/
static int png_analyse(struct pic_info *pic)
{
	FILE *fp = NULL;
	png_structp png_ptr;   
	png_infop info_ptr;
	int color_type = 0;
	int len = 0 ;
	int i = 0 , j =0;
	int pos = 0;
	png_bytep* row_pointers;
	
	//第0步 打开文件
	fp = fopen(pic->pathname, "rb");
	if(NULL == fp)
	{
		printf("open %s fail! \n", pic->pathname);
		return -1;
	}
	debug("0 open %s \n", pic->pathname);

	// 第1步: 相关数据结构实例化
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (png_ptr == 0)
	{
		fclose(fp);
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
  	if (info_ptr == 0)
  	{
   		png_destroy_read_struct(&png_ptr, 0, 0);
   		fclose(fp);
   		return -1;
  	}
    debug("1 png_create_info_struct png_create_read_struct\n");
	// 第2步: 设置错误处理函数
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
		return -1;
	}
	debug("2 setjmp\n");
	// 第3步: 将要解码的png图片的文件指针和png解码器绑定起来
	png_init_io(png_ptr, fp);
	debug("3 png_init_io\n");
	// 第4步: 读取png图片信息
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);
	debug("4 png_read_png\n");
	// 第5步: 相关图片信息打印出来看一看
	color_type = info_ptr->color_type;
	debug("color_type = %d\n", color_type);

	pic->width = info_ptr->width;
	pic->hight= info_ptr->height;
	pic->bpp = info_ptr->pixel_depth;
	len = info_ptr->width * info_ptr->height * info_ptr->pixel_depth / 8;
	debug("width = %u, height = %u, bpp = %u\n", pic->width, pic->hight, pic->bpp);

	// 第6步: 读取真正的图像信息
	row_pointers = png_get_rows(png_ptr,info_ptr);

	// 只处理RGB24位真彩色图片，其他格式的图片不管
	// 第7步: 图像数据移动到我们自己的buf中
	if(color_type == PNG_COLOR_TYPE_RGB)
  	{
   		//memcpy(pPic->pData, row_pointers, len);
		for(i=0; i<pic->hight; i++)
		{
			for(j=0; j<3*pic->width; j+=3)
			{
				pic->pdata[pos++] = row_pointers[i][j+0];		
				pic->pdata[pos++] = row_pointers[i][j+1];		
				pic->pdata[pos++] = row_pointers[i][j+2];		
			}
		}
  	}

	// 第8步: 收尾处理
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	// close file
	fclose(fp);
	
	return 0;
}


/******************************************************************* 
*
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:显示png图片
*
***************************************************************************/

int display_png(const char *pathname)
{

	struct pic_info pic;
	
	if(is_png(pathname))
	{
		printf("this file is not a png file! open fail!!! \n");
		return -1;
	}	
	debug("%s is a png image!\n", pathname);

	pic.pdata = rgb_buf;
	pic.pathname = pathname;
	png_analyse(&pic);

	lcd_draw_image2(&pic);
	
	return 0;
}






















