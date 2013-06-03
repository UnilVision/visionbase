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


#ifndef __GABOR_H__
#define __GABOR_H__


void gabor_filter(int gabor_theta_div,
				  float gabor_lambda, float gabor_sigma, float gabor_phase_offset,
				  unsigned char* src, int src_wid, int src_hei, int src_widstep,
				  unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
				  int dst_offsetx, int dst_offsety);
#endif