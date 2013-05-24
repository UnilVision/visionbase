#include "ig.h"
#include <math.h>
#include <stdlib.h>

#define EPS		0.008856f

static void ig_bgr_lab(unsigned char* src, int wid, int hei, int swidstep,
					   float* dstL, float* dstA, float* dstB)
{
	int i, j;
	unsigned char* psrc = src;

	for( j = 0; j < hei; j++ )
	{
		psrc = src + j*swidstep;
		for ( i=0; i< wid; i++ )
		{
			int b = *psrc++;
			int g = *psrc++;
			int j = *psrc++;

			float fr = j / 255.0f;
			float fg = g / 255.0f;
			float fb = b / 255.0f;

			float kr, kg, kb;
			float X, Y, Z;

			float kappa   = 903.3f;	

			float Xr = 0.950456f;
			float Yr = 1.0f;
			float Zr = 1.088754f;

			float xr, yr, zr, fx, fy, fz;

			if(fr <= 0.04045f)	
				kr = fr/12.92f;
			else				
				kr = (float) pow((fr+0.055f)/1.055f,2.4f);

			if(fg <= 0.04045f)	
				kg = fg/12.92f;
			else				
				kg = (float) pow((fg+0.055f)/1.055f,2.4f);

			if(fb <= 0.04045)	
				kb = fb/12.92f;
			else				
				kb = (float) pow((fb+0.055f)/1.055f,2.4f);
			
			X = kr*0.4124564f + kg*0.3575761f + kb*0.1804375f;
			Y = kr*0.2126729f + kg*0.7151522f + kb*0.0721750f;
			Z = kr*0.0193339f + kg*0.1191920f + kb*0.9503041f;
			
			xr = X/Xr;
			yr = Y/Yr;
			zr = Z/Zr;

			if(xr > EPS)	
				fx = (float) pow(xr, 1.0/3.0);
			else				
				fx = (kappa*xr + 16.0f)/116.0f;
			if(yr > EPS)	
				fy = (float) pow(yr, 1.0/3.0);
			else				
				fy = (kappa*yr + 16.0f)/116.0f;
			if(zr > EPS)	
				fz = (float) pow(zr, 1.0/3.0);
			else				
				fz = (kappa*zr + 16.0f)/116.0f;

			*dstL = 116.0f*fy-16.0f;
			*dstA = 500.0f*(fx-fy);
			*dstB = 200.0f*(fy-fz);

			dstL++; dstA++; dstB++;
		}
	}
}

static void ig_smooth(float* src, int wid, int hei, float* dst)
{
	int		i, j;
	float*	tmp;

	tmp = (float*) calloc(wid*hei, sizeof(float));

	for( j = 0; j < hei; j++ )
	{
		float* psrc = src+j*wid;
		float* ptmp = tmp+j*wid;

		ptmp[0] = (psrc[0]+ psrc[0] + psrc[1]) / 3;

		for( i = 1; i < wid-1; i++ )
		{
			ptmp[i] = (psrc[0] + psrc[1] + psrc[1] + psrc[2]) / 4;
			psrc++;
		}

		ptmp[wid-1] = (psrc[0]+ psrc[1] + psrc[1]) / 3;
	}

	for( i = 0; i < wid; i++ )
	{
		float* psrc = tmp + i;
		float* pdst = dst + i;

		pdst[0] = (psrc[0]+ psrc[0] + psrc[wid]) / 3;
		for( j = 1; j < hei-1; j++ )
		{
			pdst[j*wid] = (psrc[0] + psrc[wid] + psrc[wid] + psrc[2*wid]) / 4;
			psrc += wid;
		}
		pdst[(hei-1)*wid] = (psrc[0]+ psrc[wid] + psrc[wid]) / 3;
	}

	free(tmp);
}

void ig_saliency_map(unsigned char* src, int wid, int hei, int swidstep,
					 unsigned char* dst, int dwidstep)
{
	int i, j;
	float uL=0, uA=0, uB=0;
	float* fL, *fA, *fB;
	float* gfL, *gfA, *gfB;
	float* fsa;
	float maxd=0, mind=10E6;
	float dran;

	fL = (float*) calloc(wid*hei, sizeof(float));
	fA = (float*) calloc(wid*hei, sizeof(float));
	fB = (float*) calloc(wid*hei, sizeof(float));

	gfL = (float*) calloc(wid*hei, sizeof(float));
	gfA = (float*) calloc(wid*hei, sizeof(float));
	gfB = (float*) calloc(wid*hei, sizeof(float));

	fsa = (float*) calloc(wid*hei, sizeof(float));

	ig_bgr_lab(src, wid, hei, swidstep, fL, fA, fB);
	for( i=0; i < wid*hei; i++ )
	{
		uL += fL[i];
		uA += fA[i];
		uB += fB[i];
	}

	uL /= (wid*hei); uA /= (wid*hei); uB /= (wid*hei);

	ig_smooth(fL, wid, hei, gfL);
	ig_smooth(fA, wid, hei, gfA);
	ig_smooth(fB, wid, hei, gfB);

	for( i=0; i < wid*hei; i++ )
	{
		fsa[i] = 
			(gfL[i]-uL)*(gfL[i]-uL) +
			(gfA[i]-uA)*(gfA[i]-uA) +
			(gfB[i]-uB)*(gfB[i]-uB);

		if( maxd < fsa[i])
			maxd = fsa[i];

		if( mind > fsa[i])
			mind = fsa[i];
	}

	dran = maxd-mind+1;

	for (j=0;j<hei;j++)
	{
		unsigned char* pdst = dst + j*dwidstep;
		float* pfsa = fsa + j*wid;

		for (i=0;i<wid;i++)
		{
			*pdst = (unsigned char) (255*(*pfsa - mind)/dran);
			pfsa++; pdst++;
		}
	}

	free(fL);free(fA);free(fB);
	free(gfL);free(gfA);free(gfB);
	free(fsa);
}