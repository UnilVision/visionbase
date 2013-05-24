/*
Clean C reimplementation of IG saliency detection algorithm.

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

#include "ig.h"

#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")

int main(int argc, char* argv[])
{
	int i, j;

	IplImage* src_img;
	IplImage* dst;
	IplImage* show_img;

	cvNamedWindow("UnilVision - Saliency IG", CV_WINDOW_AUTOSIZE);

	src_img = cvLoadImage("test2.png", 1);
	dst = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
	show_img = cvCreateImage(cvSize(src_img->width*2, src_img->height), IPL_DEPTH_8U, src_img->nChannels);

	ig_saliency_map(
		(unsigned char*)src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
		(unsigned char*)dst->imageData, dst->widthStep);

	// Binary 
	for (j=0;j<src_img->height;j++)
	{
		for (i=0;i<src_img->width;i++)
		{
			show_img->imageData[j*show_img->widthStep+3*(src_img->width+i)] = dst->imageData[j*dst->widthStep+i];
			show_img->imageData[j*show_img->widthStep+3*(src_img->width+i)+1] = dst->imageData[j*dst->widthStep+i];
			show_img->imageData[j*show_img->widthStep+3*(src_img->width+i)+2] = dst->imageData[j*dst->widthStep+i];
		}
	}

	// Display
	cvSetImageROI(show_img, cvRect(0, 0, src_img->width, src_img->height));
	cvCopy(src_img, show_img, 0);
	cvResetImageROI(show_img);

	cvShowImage("UnilVision - Saliency IG", show_img);
	cvWaitKey(-1);

	cvReleaseImage(&src_img);
	cvReleaseImage(&dst);
	cvReleaseImage(&show_img);

	return 0;
}