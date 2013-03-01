#ifndef __CHA_NORM_H__
#define __CHA_NORM_H__

#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct _cha_rect
{
	int left, top, right, bottom;
} CHARECT_t;

#define RADIOFUNC_FIXED			0
#define RADIOFUNC_ASPECT		1
#define RADIOFUNC_SQUARE		2
#define RADIOFUNC_CUBIC			3
#define RADIOFUNC_SINE			4

void backward_linear(unsigned char* src, int src_wid, int src_hei, int src_widstep,
					 CHARECT_t* region,
					 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
					 int ratio_preserve_func);

void backward_moment(unsigned char* src, int src_wid, int src_hei, int src_widstep,
					 CHARECT_t* region,
					 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
					 int ratio_preserve_func);
// slant correction
// Note>> (dst_wid, dst_hei) must equal to (region.width, region.height)
void backward_moment_slantcorrection(unsigned char* src, int src_wid, int src_hei, int src_widstep,
									 CHARECT_t* region,
									 unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep);

// Nonlinear Normalization
// The input src must be a binary image. In the following implementation, pixels with value 
// smaller than 128 are considered as background pixel while others are foreground pixels.
void forward_nonlinear_1d(unsigned char* src, int src_wid, int src_hei, int src_widstep,
						  CHARECT_t* region,
						  unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
						  int ratio_preserve_func);

#endif