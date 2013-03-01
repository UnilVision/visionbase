/*
Clean C reimplementation of KDE algorithm in paper:
Non-parametric Model for Background Subtraction, 
Ahmed Elgammal, David Harwood, Larry Davis, ECCV(2000).

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

#include "kde2000.h"

#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "highgui.lib")

int main(int argc, char* argv[])
{
	KDEBSM_t* kde2000 = 0;
	CvCapture * capture = cvCreateFileCapture ("Videos\\Cam1_Outdoor_Xvid.avi");
	cvNamedWindow("UnilVision - KDE2000",CV_WINDOW_AUTOSIZE);

	while(1)
	{ 
		IplImage* frame = cvQueryFrame( capture );
		IplImage* src = cvCreateImage(cvSize(IMG_WID, IMG_HEI), IPL_DEPTH_8U, 3);
		IplImage* disp = cvCreateImage(cvSize(IMG_WID*2, IMG_HEI), IPL_DEPTH_8U, 3);

		if(!frame)
			break;

		// Prepare source
		cvResize(frame, src, 1);

		// Run kde
		if( ! kde2000 )
		{
			kde2000 = (KDEBSM_t*) calloc( 1, sizeof(KDEBSM_t));
			kdebs_init(kde2000, src->imageData, src->widthStep, 5000);
		}
		else
		{
			int i, j;

			kdebs_foreground(kde2000, src->imageData, src->widthStep, 8000, 1.5f);
			kdebs_cuttiny(kde2000, 10);
			kdebs_update(kde2000, src->imageData, src->widthStep);

			for (j=0;j<IMG_HEI;j++)
			{
				for (i=0;i<IMG_WID;i++)
				{
					disp->imageData[j*disp->widthStep+3*i] = kde2000->src0[i][j].r;
					disp->imageData[j*disp->widthStep+3*i+1] = kde2000->src0[i][j].g;
					disp->imageData[j*disp->widthStep+3*i+2] = kde2000->src0[i][j].b;

					disp->imageData[j*disp->widthStep+3*(IMG_WID+i)] = kde2000->foreground[i][j] * 255;
					disp->imageData[j*disp->widthStep+3*(IMG_WID+i)+1] = kde2000->foreground[i][j] * 255;
					disp->imageData[j*disp->widthStep+3*(IMG_WID+i)+2] = kde2000->foreground[i][j] * 255;
				}
			}
		}

		cvShowImage("UnilVision - KDE2000", disp);
		cvWaitKey(1);
		cvReleaseImage(&src);
		cvReleaseImage(&disp);
	}

	cvReleaseCapture(&capture);
	return 0;
}