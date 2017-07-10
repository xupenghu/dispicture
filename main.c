#include <stdio.h>
#include <fb.h>
#include <fb_bmp.h>

#include <debug.h>
#include <config.h>
#include <unistd.h>

#include <image_manger.h>






void display_image(void)
{
	int i = 0 ;
	
	for (i = 0; i < image_index ; i ++)
	{	
		if(0 == images[i].image_type)
		{
			display_bmp(images[i].pathname);
		}
		else if(1 == images[i].image_type)
		{
			display_jpg(images[i].pathname);
		}
		else if(2 == images[i].image_type)
		{
			display_png(images[i].pathname);
		}
		sleep(2);
	}





}
int main()
{
	printf("image player...\n");

	scan_image2("./image");
	print_images();
	
	fb_open();
	lcd_draw_background(BLUE);

/*
	display_jpg("image/timg.jpg");
	sleep(2);
	display_bmp("image/111.bmp");
	sleep(2);
	display_png("image/123.png");
*/

	while(1)
	{
		ts_updown();
	}
	fb_close();

	return 0;

}






































