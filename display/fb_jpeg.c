/*********************************************************************************
*JPEG文件解码文件
*2017.6.2
*powered by xph
*********************************************************************************/

#include <stdio.h>

#include <jpeglib.h>
#include <jerror.h>
//#include <setjmp.h>

#include <string.h>

#include <config.h>

#include <debug.h>





/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:解析jpeg文件 并将解析出来的数据丢到jpeg数组中
*
*/
int is_jpeg(char *pathmame)
{
	FILE *file = NULL;
	char buff[2] = {0};

	file = fopen(pathmame, "rb");
	if(NULL == file)
	{
		fprintf(stderr, "open %s fail!\n", pathmame);
		goto fail;
	}

	fread(buff, sizeof(char), sizeof(buff), file);
	debug("read is : 0x%x%x \n", buff[0], buff[1]);

	if(!((buff[0]==0xff)&&(buff[1]==0xd8)))
	{
		goto fail;
	}

	fseek(file, -2 , SEEK_END);
	
	fread(buff, sizeof(char), sizeof(buff), file);
	debug("read is : 0x%x%x \n", buff[0], buff[1]);

	if(!((buff[0]==0xff)&&(buff[1]==0xd9)))
	{
		goto fail;
	}
	fclose(file);
	return 0;
	
fail:
	fclose(file);
	return -1;

}
struct my_error_mgr 
{  
	struct jpeg_error_mgr pub;	
	/* "public" fields */  
//	jmp_buf setjmp_buffer;	
	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;

/* 
* Here's the routine that will replace the standard error_exit method: 
*/
METHODDEF(void)my_error_exit (j_common_ptr cinfo)
{  
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */  
//	my_error_ptr myerr = (my_error_ptr) cinfo->err;  
	/* Always display the message. */  
	/* We could postpone this until after returning, if we chose. */  
//	(*cinfo->err->output_message) (cinfo);  
	/* Return control to the setjmp point */ 
//	longjmp(myerr->setjmp_buffer, 1);

	fprintf(stderr, "my_error_exit \n");

}


/******************************************************************* 
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:解析bmp文件 并将解析出来的数据丢到bmp_buf数组中
*
***************************************************************************/
static int jpeg_analyse(struct pic_info *pic)
{
	struct jpeg_decompress_struct cinfo;	 /*cinfo 是贯穿整个解码过程的信息记录和传递的数据结构*/
	struct my_error_mgr jerr;  				/*错误处理的信息结构体*/
    FILE * infile;							/* source file 文件指针*/  
	//JSAMPARRAY buffer = NULL;						/* Output row buffer 数据解码行数据指针 */
	char * buffer;
	int row_stride;							/* physical row width in output buffer 一行数据的大小*/	

	/**第一步:打开文件 如果错误则退出**/
	if ((infile = fopen(pic->pathname, "rb")) == NULL) 
	{    
		fprintf(stderr, "can't open %s\n", pic->pathname);    
		return 0;  
	}

	/*错误处理的回调函数的绑定*/
	cinfo.err = jpeg_std_error(&jerr.pub);  
	jerr.pub.error_exit = my_error_exit;
#if 0
	if (setjmp(jerr.setjmp_buffer)) 
	{	
		/* If we get here, the JPEG code has signaled an error.	 
		* We need to clean up the JPEG object, close the input file, and return.	  
		*/	
		jpeg_destroy_decompress(&cinfo);	
		fclose(infile);    
		return 0;  
	}

#endif

	/* Now we can initialize the JPEG decompression object. 
	   初始化jpeg解码                                         */	
	jpeg_create_decompress(&cinfo);
	/*cinfo和infile之间的一个绑定 将infile中的数据传递给cinfo结构体中的相关成员变量*/
	jpeg_stdio_src(&cinfo, infile);

	 /*读取头信息*/
	(void) jpeg_read_header(&cinfo, TRUE);

	(void) jpeg_start_decompress(&cinfo); /*开始解码*/

	 fprintf(stderr, "image resolution : %d * %d , bpp/8 = %d ,\n", cinfo.output_width, cinfo.output_height,  cinfo.output_components);
	 
	

	row_stride = cinfo.output_width * cinfo.output_components;	 /*计算一行图片数据需要多大空间*/

	 /* buffer 申请内存空间 函数指针 接受四个参数*/
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	 if(NULL == buffer)
	 {
		fprintf(stderr, "*cinfo.mem->alloc_sarray error \n");
		return -1;

	 }

	while (cinfo.output_scanline < cinfo.output_height) 
	{	 
		/* jpeg_read_scanlines expects an array of pointers to scanlines.	   
		* Here the array is only one element long, but you could ask for 	
		* more than one scanline at a time if that's more convenient.	
		*解码一行数据放在buffer中
		*/	
		//(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		(void) jpeg_read_scanlines(&cinfo, &buffer, 1);
		
		/* Assume put_scanline_someplace wants a pointer and sample count. */    
//		put_scanline_someplace(buffer[0], row_stride); /*实例函数 需要处理图像数据*/

		memcpy(pic->pdata + (cinfo.output_scanline-1) * row_stride, buffer, row_stride);
	}

	// 图片数据填充
	pic->hight	 =  cinfo.output_height;
	pic->width	 =  cinfo.output_width ;
	pic->bpp  	 = cinfo.output_components * 8;


	

	(void) jpeg_finish_decompress(&cinfo);	/*结束解码*/
	jpeg_destroy_decompress(&cinfo);		/*释放申请的内存*/

	fclose(infile);							/*关闭打开的文件*/

	/* And we're done! */  
	return 1;


}


/******************************************************************* 
*
*输入参数: *path 要解析的bmp文件的pathname
*输出参数:正确返回0 不正确返回-1
*函数功能:显示jpg图片
*
***************************************************************************/

int display_jpg(const char *pathname)
{

	struct pic_info pic;
	
	if(is_jpeg(pathname))
	{
		printf("this file is not a jpg file! open fail!!! \n");
		return -1;
	}

	pic.pathname = pathname;
	pic.pdata = rgb_buf;

	jpeg_analyse(&pic);
	lcd_draw_image2(&pic);
	return 0;

}
















































