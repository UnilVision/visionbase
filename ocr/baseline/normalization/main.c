/*
Clean C reimplementation of three Character Normalization algorithms.

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

#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")

#include "chanorm.h"

#define NORM_SIZE		32
#define DISP_INTERVAL	6

int main(int argc, char* argv[])
{
	int i;

	IplImage* src_img;
	IplImage* src_result;

	CHARECT_t rectlist[] = {
		{ 270, 70, 290, 92 },
		{ 332, 140, 360, 162 },
		{ 207, 74, 224, 97 },
		{ 84, 75, 145, 96 },
		{ 20, 206, 40, 225 },
	};

	cvNamedWindow("UnilVision - Character Normalization", CV_WINDOW_AUTOSIZE);
	src_img = cvLoadImage("TestImgs\\CHfig.png", 0);
	src_result = cvCreateImage(cvSize(5*NORM_SIZE+DISP_INTERVAL*4, sizeof(rectlist)/sizeof(CHARECT_t)*NORM_SIZE), IPL_DEPTH_8U, 1);
	cvSet(src_result, cvScalar(128,0,0,0), 0);

	for (i=0;i<sizeof(rectlist)/sizeof(CHARECT_t);i++)
	{
		IplImage* dst = cvCreateImage(cvSize(NORM_SIZE,NORM_SIZE), IPL_DEPTH_8U, 1);
		IplImage* slantnorm = cvCreateImage(
			cvSize(rectlist[i].right-rectlist[i].left,
			rectlist[i].bottom-rectlist[i].top), IPL_DEPTH_8U, 1);
		CHARECT_t slantregion = {0, 0, slantnorm->width,slantnorm->height};

		// Opencv resize
		cvSetImageROI(src_img, 
			cvRect(rectlist[i].left, rectlist[i].top, 
			rectlist[i].right-rectlist[i].left,rectlist[i].bottom-rectlist[i].top));
		cvSetImageROI(src_result,
			cvRect(0, i*NORM_SIZE, NORM_SIZE, NORM_SIZE));
		cvResize(src_img, src_result, CV_INTER_LINEAR);
		cvResetImageROI(src_img);
		cvResetImageROI(src_result);

		// Linear 
		backward_linear(
			src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
			rectlist+i, 
			dst->imageData, dst->width, dst->height, dst->widthStep, RADIOFUNC_FIXED);
		cvSetImageROI(src_result,
			cvRect(NORM_SIZE+DISP_INTERVAL, i*NORM_SIZE, NORM_SIZE, NORM_SIZE));
		cvCopy(dst, src_result, 0);
		cvResetImageROI(src_result);

		// Moment-based
		backward_moment(
			src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
			rectlist+i,
			dst->imageData, dst->width, dst->height, dst->widthStep, RADIOFUNC_FIXED);
		cvSetImageROI(src_result,
			cvRect(2*(NORM_SIZE+DISP_INTERVAL), i*NORM_SIZE, NORM_SIZE, NORM_SIZE));
		cvCopy(dst, src_result, 0);
		cvResetImageROI(src_result);

		// Moment-based with slant correction
		// Slant correction
		backward_moment_slantcorrection(
			src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
			rectlist+i, 
			slantnorm->imageData, slantnorm->width, slantnorm->height, slantnorm->widthStep);
		backward_moment(
			slantnorm->imageData, slantnorm->width, slantnorm->height, slantnorm->widthStep,
			&slantregion, 
			dst->imageData, dst->width, dst->height, dst->widthStep, RADIOFUNC_FIXED);
		cvSetImageROI(src_result,
			cvRect(3*(NORM_SIZE+DISP_INTERVAL), i*NORM_SIZE, NORM_SIZE, NORM_SIZE));
		cvCopy(dst, src_result, 0);
		cvResetImageROI(src_result);

		// Nonlinear Normalization
		forward_nonlinear_1d(
			src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
			rectlist+i, 
			dst->imageData, dst->width, dst->height, dst->widthStep, RADIOFUNC_FIXED);
		cvSetImageROI(src_result,
			cvRect(4*(NORM_SIZE+DISP_INTERVAL), i*NORM_SIZE, NORM_SIZE, NORM_SIZE));
		cvCopy(dst, src_result, 0);
		cvResetImageROI(src_result);


		cvReleaseImage(&dst);
		cvReleaseImage(&slantnorm);
	}

	cvShowImage("UnilVision - Character Normalization", src_result);
	cvWaitKey(0);
	cvReleaseImage(&src_img);
	cvReleaseImage(&src_result);
	return 0;
}