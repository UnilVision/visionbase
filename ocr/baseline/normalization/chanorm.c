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

#include "chanorm.h"

#ifndef min
#define min(a,b)		((a)<(b)? (a):(b))
#endif

#ifndef max
#define max(a,b)		((a)>(b)? (a):(b))
#endif

#ifndef PI
#define PI				3.1415926f
#endif

float aspect_radio_mapping(float r1, int dst_wid, int dst_hei,
						   int ratio_preserve_func)
{
	switch( ratio_preserve_func )
	{
	case RADIOFUNC_ASPECT:
		return r1;
	case RADIOFUNC_SQUARE:
		return (float) sqrt(r1);
	case RADIOFUNC_CUBIC:
		return (float) pow(r1, 0.3333f);
	case RADIOFUNC_SINE:
		return (float) sqrt(sin(PI*r1/2));
	default:
		return (float) min( dst_wid, dst_hei ) / max( dst_wid, dst_hei );
	}
}

void backward_linear(unsigned char* src, int src_wid, int src_hei, int src_widstep,
					 CHARECT_t* region,
					 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
					 int ratio_preserve_func)
{
	float r1, r2;
	int w1, h1;
	int w2, h2;
	int x, y;
	int xoffset, yoffset;
	int src_xoffset, src_yoffset;
	float xscale, yscale;

	w1 = region->right-region->left;
	h1 = region->bottom-region->top;
	src_xoffset = region->left;
	src_yoffset = region->top;

	r1 = (float) min( w1, h1 ) / max( w1, h1 );
	r2 = aspect_radio_mapping(r1, dst_wid, dst_hei, ratio_preserve_func);
	memset( dst, 0, dst_hei * dst_widstep);

	if( w1 > h1 )
	{
		w2 = dst_wid;
		h2 = (int) (w2 * r2);
		xoffset = 0;
		yoffset = (dst_hei - h2)/2;
	}
	else 
	{
		h2 = dst_hei;
		w2 = (int) (h2 * r2);
		xoffset = (dst_wid - w2)/2;
		yoffset = 0; 
	}

	xscale = (float)w1 / w2;
	yscale = (float)h1 / h2;

	for (y=yoffset;y<h2+yoffset;y++)
	{
		int j = y-yoffset;
		if( j < 0 )
			continue;

		for (x=xoffset;x<w2+xoffset;x++)
		{
			unsigned char* psrc, *psrc1, *psrc2, *psrc3;
			float tmp0;

			int i = x-xoffset;

			float x0 = i * xscale + src_xoffset;		
			float y0 = j * yscale + src_yoffset;

			int x0left = (int) x0;
			int y0left = (int) y0;

			if( i < 0 ) 
				continue;

			if( x0left >= src_wid-1 )
			{
				if( y0left >= src_hei-1 )
				{
					psrc = src + (src_hei-1)*src_widstep + (src_wid-1);
					psrc1 = psrc; 
					psrc2 = psrc; 
					psrc3 = psrc; 
				}
				else
				{
					psrc = src + y0left*src_widstep + (src_wid-1);
					psrc1 = src + (y0left+1)*src_widstep + (src_wid-1);
					psrc2 = src + y0left*src_widstep + (src_wid-1);
					psrc3 = src + (y0left+1)*src_widstep + (src_wid-1);
				}
			}
			else if( y0left >= src_hei-1 )
			{
				psrc = src + (src_hei-1)*src_widstep + (x0left);
				psrc1 = src + (src_hei-1)*src_widstep + (x0left);
				psrc2 = src + (src_hei-1)*src_widstep + (x0left+1);
				psrc3 = src + (src_hei-1)*src_widstep + (x0left+1);
			}
			else
			{
				psrc = src + (y0left)*src_widstep + (x0left);
				psrc1 = src + (y0left+1)*src_widstep + (x0left);
				psrc2 = src + (y0left)*src_widstep + (x0left+1);
				psrc3 = src + (y0left+1)*src_widstep + (x0left+1);
			}

			tmp0 = (1-(x0-x0left)) * ((1-(y0-y0left))* *psrc +(y0-y0left)* *psrc1);
			tmp0 += (x0-x0left) * ((1-(y0-y0left)) * *(psrc2) + (y0-y0left)* *(psrc3));

			dst[y*dst_widstep+x] = (unsigned char) tmp0;
		}
	}
}



void backward_moment(unsigned char* src, int src_wid, int src_hei, int src_widstep,
					 CHARECT_t* region,
					 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
					 int ratio_preserve_func)
{
	int x, y;
	int xc, yc;
	int w1, h1;
	long m00=0, m10=0, m01=0;
	long u20=0, u02=0;

	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			m00 += src[y*src_widstep+x];
			m10 += x*src[y*src_widstep+x];
			m01 += y*src[y*src_widstep+x];
		}
	}

	if( m00 == 0 )
		return;

	xc = m10 / m00;
	yc = m01 / m00;

	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			u20 += (x-xc)*(x-xc)*src[y*src_widstep+x];
			u02 += (y-yc)*(y-yc)*src[y*src_widstep+x];
		}
	}

	w1 = (int) (4 * sqrt( u20 / m00 ));
	h1 = (int) (4 * sqrt( u02 / m00 ));

	{
		CHARECT_t rect = {xc-w1/2, yc-h1/2, xc+w1/2, yc+h1/2};
		rect.left = min(max(rect.left, 0), src_wid);
		rect.right = min(max(rect.right, 0), src_wid);
		rect.top = min(max(rect.top, 0), src_hei);
		rect.bottom = min(max(rect.bottom, 0), src_hei);

		backward_linear(src, src_wid, src_hei, src_widstep,
			&rect, dst, dst_wid, dst_hei, dst_widstep, ratio_preserve_func);
	}
}

// slant correction
// Note>> (dst_wid, dst_hei) must equal to (region.width, region.height)
void backward_moment_slantcorrection(unsigned char* src, int src_wid, int src_hei, int src_widstep,
									 CHARECT_t* region,
									 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep)
{
	int x, y;
	int xc=0, yc=0;
	int u02=0, u11=0;
	long m00=0, m10=0, m01=0;
	float tanthe;

	if( dst_wid != region->right-region->left || 
		dst_hei != region->bottom-region->top)
		return;

	// Slant estimation
	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			m00 += src[y*src_widstep+x];
			m10 += x*src[y*src_widstep+x];
			m01 += y*src[y*src_widstep+x];
		}
	}

	if( m00 == 0 )
		return;

	xc = m10 / m00;
	yc = m01 / m00;

	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			u02 += (y-yc)*(y-yc)*src[y*src_widstep+x];
			u11 += (y-yc)*(x-xc)*src[y*src_widstep+x];
		}
	}

	tanthe = (float) u11 / u02;

	// Correction
	for (y=0;y<dst_hei;y++)
	{
		for (x=0;x<dst_wid;x++)
		{
			unsigned char* psrc, *psrc1, *psrc2, *psrc3;
			float tmp0;

			float y0 = (float) (y + region->top);
			float x0 = x + tanthe * (y0-yc) + region->left;

			int x0left = (int) x0;
			int y0left = (int) y0;

			if( x0left >= src_wid-1 )
			{
				if( y0left >= src_hei-1 )
				{
					psrc = src + (src_hei-1)*src_widstep + (src_wid-1);
					psrc1 = psrc; 
					psrc2 = psrc; 
					psrc3 = psrc; 
				}
				else
				{
					psrc = src + y0left*src_widstep + (src_wid-1);
					psrc1 = src + (y0left+1)*src_widstep + (src_wid-1);
					psrc2 = src + y0left*src_widstep + (src_wid-1);
					psrc3 = src + (y0left+1)*src_widstep + (src_wid-1);
				}
			}
			else if( y0left >= src_hei-1 )
			{
				psrc = src + (src_hei-1)*src_widstep + (x0left);
				psrc1 = src + (src_hei-1)*src_widstep + (x0left);
				psrc2 = src + (src_hei-1)*src_widstep + (x0left+1);
				psrc3 = src + (src_hei-1)*src_widstep + (x0left+1);
			}
			else
			{
				psrc = src + (y0left)*src_widstep + (x0left);
				psrc1 = src + (y0left+1)*src_widstep + (x0left);
				psrc2 = src + (y0left)*src_widstep + (x0left+1);
				psrc3 = src + (y0left+1)*src_widstep + (x0left+1);
			}

			tmp0 = (1-(x0-x0left)) * ((1-(y0-y0left))* *psrc +(y0-y0left)* *psrc1);
			tmp0 += (x0-x0left) * ((1-(y0-y0left)) * *(psrc2) + (y0-y0left)* *(psrc3));

			dst[y*dst_widstep+x] = (unsigned char) tmp0;
		}
	}
}

static void forward_push_val(unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
							 unsigned char val,
							 float x, float y, float scalex, float scaley)
{
	int i, j;
	float fl, ft, fr, fb;
	int l, t, r, b;

	fl =(x - scalex/2);
	fr =(x + scalex/2);
	ft =(y - scaley/2); 
	fb =(y + scaley/2); 

	l = (int)fl;
	r = (int)fr+1;
	t = (int)ft;
	b = (int)fb+1;

	l = min(max(l, 0), dst_wid-1);
	r = min(max(r, 0), dst_wid-1);
	t = min(max(t, 0), dst_hei-1);
	b = min(max(b, 0), dst_hei-1);

	for (j=t;j<=b;j++)
	{
		for (i=l;i<=r;i++)
		{
			// float intersect_area;
			float xg = min(i+1, fr) - max(i, fl);
			float yg = min(j+1, fb) - max(j, ft);

			if( xg > 0 && yg > 0 )
			{
				dst[j*dst_widstep+i] += (unsigned char)( xg * yg * val);
			}
		}
	}
}

static void forward_push_val2(unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
							 unsigned char val,
							 float fl, float ft, float fr, float fb, float scalex, float scaley)
{
	int i, j;
	int l, t, r, b;

	l = (int)fl;
	r = (int)fr+1;
	t = (int)ft;
	b = (int)fb+1;

	l = min(max(l, 0), dst_wid-1);
	r = min(max(r, 0), dst_wid-1);
	t = min(max(t, 0), dst_hei-1);
	b = min(max(b, 0), dst_hei-1);

	for (j=t;j<=b;j++)
	{
		for (i=l;i<=r;i++)
		{
			// float intersect_area;
			float xg = min(i+1, fr) - max(i, fl);
			float yg = min(j+1, fb) - max(j, ft);

			if( xg > 0 && yg > 0 )
			{
				dst[j*dst_widstep+i] += (unsigned char)( xg * yg * val);
			}
		}
	}
}

// Nonlinear Normalization
// The input src must be a binary image. In the following implementation, pixels with value 
// smaller than 128 are considered as background pixel while others are foreground pixels.
void forward_nonlinear_1d(unsigned char* src, int src_wid, int src_hei, int src_widstep,
						  CHARECT_t* region,
						  unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
						  int ratio_preserve_func)
{
	int x, y;
	int xc, yc;
	int w1, h1;
	int w2, h2;

	long m00=0, m10=0, m01=0;
	long u20=0, u02=0;

	float* dx, *dy;
	float* px, *py;
	float* hx, *hy;
	float dx_sum = 0;
	float dy_sum = 0;

	float xscale = 0;
	float yscale = 0;

	float xoffset, yoffset;

	float r1, r2;
	int l, r, t, b;

	const float constval = 0.001f;

	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			m00 += src[y*src_widstep+x];
			m10 += x*src[y*src_widstep+x];
			m01 += y*src[y*src_widstep+x];
		}
	}

	if( m00 == 0 )
		return;

	xc = m10 / m00;
	yc = m01 / m00;

	for (y=region->top;y<region->bottom;y++)
	{
		for (x=region->left;x<region->right;x++)
		{
			u20 += (x-xc)*(x-xc)*src[y*src_widstep+x];
			u02 += (y-yc)*(y-yc)*src[y*src_widstep+x];
		}
	}

	w1 = (int) (4 * sqrt( u20 / m00 ));
	h1 = (int) (4 * sqrt( u02 / m00 ));

	l = xc - w1/2;
	r = xc + w1/2 + 1;
	t = yc - h1/2;
	b = yc + h1/2 + 1;
	l = min(max(l, 0), src_wid);
	r = min(max(r, 0), src_wid);
	t = min(max(t, 0), src_hei);
	b = min(max(b, 0), src_hei);

	// all this arrays are inited to 0 when calloc is called.
	dx = (float*) calloc((r-l)*(b-t), sizeof(float));
	dy = (float*) calloc((r-l)*(b-t), sizeof(float));
	px = (float*) calloc((r-l), sizeof(float));
	py = (float*) calloc((b-t), sizeof(float));
	hx = (float*) calloc((r-l), sizeof(float));
	hy = (float*) calloc((b-t), sizeof(float));

	for (y=t;y<b;y++)
	{
		int run_start = -1;
		int run_end = -1;

		for (x=l;x<r;x++)
		{
			if( src[y*src_widstep+x] < 128 )
			{
				if( run_start < 0 )
				{
					run_start = x;
					run_end = x;
				}
				else 
				{
					run_end = x;
				}
			}
			else 
			{
				if( run_start < 0 )
				{
					dx[(y-t)*(r-l)+x-l] = constval;	// constant
				}
				else
				{
					int i;
					float d = 1.f / (w1 + run_end - run_start + 1);

					dx[(y-t)*(r-l)+x-l] = constval;	// constant
					for (i=run_start;i<=run_end;i++)
					{
						dx[(y-t)*(r-l)+i-l] = d;
					}

					run_end = run_start = -1;
				}
			}
		}

		if( run_start > 0 )
		{
			int i;
			float d = 1.f / (w1 + run_end - run_start + 1);

			for (i=run_start;i<=run_end;i++)
			{
				dx[(y-t)*(r-l)+i-l] = d;
			}
		}
	}

	for (x=l;x<r;x++)
	{
		int run_start = -1;
		int run_end = -1;

		for (y=t;y<b;y++)
		{
			if( src[y*src_widstep+x] < 128 )
			{
				if( run_start < 0 )
				{
					run_start = y;
					run_end = y;
				}
				else 
				{
					run_end = y;
				}
			}
			else 
			{
				if( run_start < 0 )
				{
					dy[(y-t)*(r-l)+x-l] = constval;	// constant
				}
				else
				{
					int i;
					float d = 1.f / (h1 + run_end - run_start + 1);

					dy[(y-t)*(r-l)+x-l] = constval;	// constant
					for (i=run_start;i<=run_end;i++)
					{
						dy[(i-t)*(r-l)+x-l] = d; 
					}

					run_end = run_start = -1;
				}
			}
		}

		if( run_start > 0 )
		{
			int i;
			float d = 1.f / (h1 + run_end - run_start + 1);

			for (i=run_start;i<=run_end;i++)
			{
				dy[(i-t)*(r-l)+x-l] = d;
			}
		}
	}

	for (y=t;y<b;y++)
	{
		for (x=l;x<r;x++)
		{
			dx_sum += dx[(y-t)*(r-l)+x-l];
			dy_sum += dy[(y-t)*(r-l)+x-l];
		}
	}

	for (y=t;y<b;y++)
	{
		for (x=l;x<r;x++)
		{
			py[y-t] += dy[(y-t)*(r-l)+x-l];
		}
		py[y-t] /= dy_sum;
	}

	for (x=l;x<r;x++)
	{
		for (y=t;y<b;y++)
		{
			px[x-l] += dx[(y-t)*(r-l)+x-l];
		}
		px[x-l] /= dx_sum;
	}

	for (x=l;x<r;x++)
	{
		int i;

		for (i=l;i<x;i++)
		{
			hx[x-l] += px[i-l];
		}
	}

	for (y=t;y<b;y++)
	{
		int j;

		for (j=t;j<y;j++)
		{
			hy[y-t] += py[j-t];
		}
	}

	r1 = (float) min( (r-l), (b-t) ) / max( (r-l), (b-t) );
	r2 = aspect_radio_mapping(r1, dst_wid, dst_hei, ratio_preserve_func);
	memset( dst, 0, dst_hei * dst_widstep);

	if( w1 > h1 )
	{
		w2 = dst_wid;
		h2 = (int) (w2 * r2);
		xoffset = 0;
		yoffset = (float) (dst_hei - h2)/2;
	}
	else 
	{
		h2 = dst_hei;
		w2 = (int) (h2 * r2);
		xoffset = (float) (dst_wid - w2)/2;
		yoffset = 0; 
	}

	xscale = (float)w2 / w1;
	yscale = (float)h2 / h1;

	// forward mapping
	for (y=t;y<b;y++)
	{
		for (x=l;x<r;x++)
		{
			float x1 = w2 * hx[x-l];
			float y1 = h2 * hy[y-t];

			if( src[y*src_widstep+x] > 128 )
				src[y*src_widstep+x] = src[y*src_widstep+x];

			if( y==b-1 || x==r-1 )
			{
				forward_push_val(dst, dst_wid, dst_hei, dst_widstep,
					src[y*src_widstep+x],
					x1+xoffset, y1+yoffset, xscale, yscale);
			}
			else
			{
				float x2 = w2 * hx[x-l+1];
				float y2 = h2 * hy[y-t+1];

				forward_push_val2(dst, dst_wid, dst_hei, dst_widstep,
					src[y*src_widstep+x],
					x1+xoffset, y1+yoffset, 
					x2+xoffset, y2+yoffset, 
					xscale, yscale);

			}
		}
	}

	free(dx);
	free(dy);
	free(px);
	free(py);
	free(hx);
	free(hy);
}