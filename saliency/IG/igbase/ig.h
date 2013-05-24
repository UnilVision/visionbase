#ifndef __IG_H__
#define __IG_H__

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

void ig_saliency_map(unsigned char* src, int wid, int hei, int swidstep,
					 unsigned char* dst, int dwidstep);


#endif // __IG_H__