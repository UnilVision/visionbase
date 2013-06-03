#include "gabor.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef PI
#define PI 3.1415926f
#endif

static float conlv22(unsigned char* src, int px, int py, int widstep, 
					 float* kernel, int bandw)
{
	int x=0, y=0;
	int r = bandw*2+1;
	float v = 0;

	for(y=-bandw;y<=bandw;y++)
	{
		for(x=-bandw;x<=bandw;x++)
		{
			v += src[ (py+y)*widstep + (px+x) ] * (kernel[ (y+bandw)*r+x+bandw]);
		}
	}

	return v/r/r;
}

static void border_ext(unsigned char* src, int wid, int hei, int widstep,
					   unsigned char* dst, int border_size)
{
	int i, j;
	int dst_widstep = widstep + 2*border_size;
	int dst_wid = wid + 2*border_size;
	int dst_hei = hei + 2*border_size;

	for (j=0;j<hei;j++)
	{
		unsigned char* sptr = src + j*widstep;
		unsigned char* dptr = dst + (j+border_size)*(dst_widstep) + border_size;

		memcpy(dptr, sptr, wid);

		for (i=0;i<border_size;i++)
		{
			dst[(j+border_size)*(dst_widstep)+i] = 
				dst[(j+border_size)*(dst_widstep)+border_size];
			dst[(j+border_size)*(dst_widstep)+dst_wid-i-1] =
				dst[(j+border_size)*(dst_widstep)+dst_wid-border_size-1];
		}
	}

	for (j=0;j<border_size;j++)
	{
		unsigned char* dptr = dst + j * dst_widstep;
		unsigned char* dptr2 = dst + (j+border_size) * dst_widstep;

		memcpy(dptr, dptr2, dst_widstep);

		dptr = dst + (dst_hei-j-1) * dst_widstep;
		dptr2 = dst + (dst_hei-border_size-1) * dst_widstep;

		memcpy(dptr, dptr2, dst_widstep);
	}
}

void gabor_filter(int gabor_theta_div,
				  float gabor_lambda, float gabor_sigma, float gabor_phase_offset,
				  unsigned char* src, int src_wid, int src_hei, int src_widstep,
				  unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
				  int dst_offsetx, int dst_offsety)
{
	float S =  1.0f / (1.414f * gabor_sigma);
	float F = 1.0f / gabor_lambda;
	float P = gabor_phase_offset;
	int kernel_size = (int) (1.5f / S);
	int r = 2*kernel_size+1;
	int i, x, y, d;
	float** kernels_r, ** kernels_i;
	unsigned char* tmpbord;
	float** tmpoutput;
	int tmp_wid = 2*kernel_size + src_wid;
	int tmp_hei = 2*kernel_size + src_hei;
	float maxr=0, minr=1E6;

	tmpbord = (unsigned char*) calloc(tmp_wid * tmp_hei, 1);
	tmpoutput = (float**) calloc(gabor_theta_div, sizeof(float*));
	border_ext(src, src_wid, src_hei, src_widstep, tmpbord, kernel_size);

	kernels_r = (float**) calloc( gabor_theta_div, sizeof(float*));
	kernels_i = (float**) calloc( gabor_theta_div, sizeof(float*));
	for (i=0;i<gabor_theta_div;i++)
	{
		kernels_r[i] = (float*) calloc(r*r, sizeof(float));
		kernels_i[i] = (float*) calloc(r*r, sizeof(float));
		tmpoutput[i] = (float*) calloc(src_wid * src_hei, sizeof(float));
	}

	for (d=0;d<gabor_theta_div;d++)
	{
		float W = PI*d/gabor_theta_div;

		for (x=-kernel_size;x<=kernel_size;x++)
		{
			for (y=-kernel_size;y<=kernel_size;y++)
			{
				float a = (float) ( exp(-PI*S*S*(x*x+y*y)) * 
					( cos(2*PI*F*(x*cos(W)+y*sin(W))+P) ));
				float b = (float) ( exp(-PI*S*S*(x*x+y*y)) * 
					( sin(2*PI*F*(x*cos(W)+y*sin(W))+P) ));

				kernels_r[d][(y+kernel_size)*r+x+kernel_size]=a;
				kernels_i[d][(y+kernel_size)*r+x+kernel_size]=b;
			}
		}
	}

	for(d=0;d<gabor_theta_div;d++)
	{
		float* kernelr = kernels_r[d];
		float* kerneli = kernels_i[d];

		for(y=kernel_size;y<tmp_hei-kernel_size;y++)
		{
			for(x=kernel_size;x<tmp_wid-kernel_size;x++)
			{
				float rpart = conlv22(tmpbord, x, y, tmp_wid, kernelr, kernel_size); 
				float ipart = conlv22(tmpbord, x, y, tmp_wid, kerneli, kernel_size); 

				float magi = (float) sqrt(rpart*rpart+ipart*ipart);
				tmpoutput[d][(y-kernel_size)*src_wid+(x-kernel_size)] = magi;

				maxr = max( magi, maxr);
				minr = min( magi, minr);
			}
		}
	}

	for(d=0;d<gabor_theta_div;d++)
	{
		for(y=kernel_size;y<tmp_hei-kernel_size;y++)
		{
			for(x=kernel_size;x<tmp_wid-kernel_size;x++)
			{
				dst[(y-kernel_size+dst_offsety)*dst_widstep+d*src_wid+(x-kernel_size+dst_offsetx)] = 
					(unsigned char) (tmpoutput[d][(y-kernel_size)*src_wid+(x-kernel_size)] * 255 / (maxr-minr+1));
			}
		}
	}

	for (i=0;i<gabor_theta_div;i++)
	{
		free(kernels_r[i]);
		free(kernels_i[i]);
		free(tmpoutput[i]);
	}
	free(kernels_r);
	free(kernels_i);
	free(tmpbord);
	free(tmpoutput);
}