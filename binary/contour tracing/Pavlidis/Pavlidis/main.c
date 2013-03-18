/*
Clean C reimplementation of Theo Pavlidis Contour Tracing Algorithm.

Copyright (C) 2012 UnilVision.

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

#include "pavlidis.h"

#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")

int main(int argc, char* argv[])
{
	int i, j;

	IplImage* src_img;
	IplImage* src_bin, *dst_contour;
	IplImage* show_img;

	cvNamedWindow("UnilVision - Theo Pavlidis", CV_WINDOW_AUTOSIZE);

	src_img = cvLoadImage("test.jpg", 0);
	src_bin = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
	dst_contour = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
	show_img = cvCreateImage(cvSize(src_img->width*2, src_img->height), IPL_DEPTH_8U, 1);

	// Binary 
	for (j=0;j<src_img->height;j++)
	{
		for (i=0;i<src_img->width;i++)
		{
			unsigned char g = (unsigned char) src_img->imageData[j*src_img->widthStep+i];

			if( (int) g > 128  )
			{
				src_bin->imageData[j*src_bin->widthStep+i] = 255;
			}
			else
			{
				src_bin->imageData[j*src_bin->widthStep+i] = 0;
			}
		}
	}

	// Contour Tracing
	Pavlidis_contour_tracing(src_bin->imageData, src_bin->width, src_bin->height, src_bin->widthStep,
		dst_contour->imageData, dst_contour->width, dst_contour->height, dst_contour->widthStep);

	// Display
	cvSetImageROI(show_img, cvRect(0, 0, src_bin->width, src_bin->height));
	cvCopy(src_bin, show_img, 0);
	cvSetImageROI(show_img, cvRect(src_bin->width, 0, src_bin->width, src_bin->height));
	cvCopy(dst_contour, show_img, 0);
	cvResetImageROI(show_img);

	cvShowImage("UnilVision - Theo Pavlidis", show_img);
	cvWaitKey(0);
	cvReleaseImage(&src_img);
	cvReleaseImage(&src_bin);
	cvReleaseImage(&dst_contour);
	cvReleaseImage(&show_img);
	return 0;
}