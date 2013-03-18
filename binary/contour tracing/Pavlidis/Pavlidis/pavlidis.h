#ifndef __PAVLIDIS_H__
#define __PAVLIDIS_H__

#include <windows.h>

void 
Pavlidis_contour_tracing(unsigned char* srcbin, 
						 int wid, int hei, int widstep, 
						 unsigned char* dstbin, 
						 int dst_wid, int dst_hei, int dst_widstep);

#endif