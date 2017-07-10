#ifndef IMAGE_MANGER__H
#define IMAGE_MANGER__H

#include <config.h>





typedef enum image_type
{
	IMAGE_TYPE_BMP,
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_UNKONW,
}image_type_e;


typedef struct image_manger
{
	char pathname[PATHNAME_MAX];
	image_type_e image_type;

	
}image_manger_t;

extern image_manger_t images[20];
extern unsigned int  image_index;

int scan_image(char *basePath);
int scan_image2(const char *path);

void print_images(void);







#endif





























