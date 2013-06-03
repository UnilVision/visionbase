/*
Clean C reimplementation of Gabor filtering algorithm.

Copyright (C) 2013 UnilVision.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "gabor.h"

#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")

int main(int argc, char* argv[])
{
	IplImage* src_img;
	IplImage* show_img;

	// src_img must be a gray-scale image;
	src_img = cvLoadImage("test.png", 0);
	show_img = cvCreateImage(
		cvSize(src_img->width*8, src_img->height*2), IPL_DEPTH_8U, 1);

	gabor_filter(8, 3, 3, 0, 
		src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
		show_img->imageData, show_img->width, show_img->height, show_img->widthStep, 
		0, 0);

	gabor_filter(8, 3, 7, 0, 
		src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
		show_img->imageData, show_img->width, show_img->height, show_img->widthStep, 
		0, src_img->height);

	cvSaveImage("out.jpg", show_img);
	cvReleaseImage(&src_img);
	cvReleaseImage(&show_img);

	return 0;
}