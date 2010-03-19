// JpegTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JpegLib.h"
#include "stdlib.h"
GLOBAL(void)
    write_JPEG_file (char * filename, int quality);

unsigned char buf[320*240*3] = {0};
JSAMPLE * image_buffer = buf;	/* Points to large array of R,G,B-order data */
int image_height = 320;	/* Number of rows in image */
int image_width = 240;		/* Number of columns in image */

int _tmain(int argc, _TCHAR* argv[])
{
    memset(buf,0xff,320*240);
    write_JPEG_file ("123.jpg", 100);
	return 0;
}

