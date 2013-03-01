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

#include "kde2000.h"

#define BAND_DIV		(0.68f*0.68f*2)
#define DEFAULT_COV		9
#define EXP_DIV			1000.f

#define MIN_COV			60
#define MAX_COV			600

#ifndef min
#define min(a,b)		((a)<(b)? (a):(b))
#endif

#ifndef max
#define max(a,b)		((a)>(b)? (a):(b))
#endif

int 
kdebs_density(
	KDEBSM_t* model,
	KDSPX_t* pl, 
	KDSAMPLE_t* pm, 
	int th);

// Connected Component Analysis
static float 
cca8(
	KDEBSM_t* model,
	int image[IMG_WID][IMG_HEI],	// Source image
	int* lx, int* ly,				// Last x and last y for speedup
	int Pr[IMG_WID][IMG_HEI],		// Pr image
	int* rect,						// CCA Region
	int* count,						// Pixel count of the component
	int* label)						// Region label
{
#define CONS_FOREGROUND			1

	int i, j;
	int init_found = 0;
	float pr_mul = 1.0f;

	POINT_t* pq;
	int pq_left=0, pq_right=0;

	(*label) ++;
	rect[0] = IMG_WID-1;
	rect[1] = IMG_HEI-1;
	rect[2] = 0;
	rect[3] = 0;
	*count=0;

	// queue
	pq =  model->cca_pq;

	for(j=*ly;j<IMG_HEI;j++)
	{
		for(i=0;i<IMG_WID;i++)
		{
			// We only analyze the pixels with label (1)
			if( image[i][j] == CONS_FOREGROUND)
			{
				POINT_t localp = {i, j};

				pq[pq_right++] = localp;

				image[i][j] = *label;
				pr_mul = Pr[i][j] / 10000.f;

				*lx = i;
				*ly = j;
				init_found = 1;
				break;
			}
		}

		if( init_found )
			break;
	}

	// 8 components connect labeling algorithm
	while(pq_right - pq_left > 0)
	{
		POINT_t* localp = pq + pq_left;

		if(localp->x+1<IMG_WID && image[localp->x+1][localp->y] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x+1, localp->y};
			pq[pq_right++] = localp2;
			image[localp->x+1][localp->y] = *label; 
			pr_mul *= Pr[localp->x+1][localp->y]/10000.f;
		}
		if(localp->x>0 && image[ localp->x-1][localp->y] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x-1, localp->y}; 
			pq[pq_right++] = localp2;
			image[ localp->x-1][localp->y] = *label; 
			pr_mul *= Pr[localp->x-1][localp->y] / 10000.f;
		}
		if(localp->y>0 && localp->x>0 && image[localp->x-1][localp->y-1] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x-1, localp->y-1}; 
			pq[pq_right++] = localp2;
			image[localp->x-1][localp->y-1] = *label; 
			pr_mul *= Pr[localp->x-1][localp->y-1] / 10000.f;
		}
		if(localp->y>0 && image[localp->x][localp->y-1] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x, localp->y-1}; 
			pq[pq_right++] = localp2;
			image[localp->x][localp->y-1] = *label; 
			pr_mul *= Pr[localp->x][localp->y-1] / 10000.f;
		}
		if(localp->y>0 && localp->x+1<IMG_WID && image[ localp->x+1][localp->y-1]== CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x+1, localp->y-1}; 
			pq[pq_right++] = localp2;
			image[ localp->x+1][localp->y-1] = *label; 
			pr_mul *= Pr[localp->x+1][localp->y-1] / 10000.f;
		}
		if(localp->y+1<IMG_HEI && localp->x>0 && image[ localp->x-1][localp->y+1] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x-1, localp->y+1}; 
			pq[pq_right++] = localp2;
			image[ localp->x-1][localp->y+1] = *label; 
			pr_mul *= image[ localp->x-1][localp->y+1] / 10000.f;
		}
		if(localp->y+1<IMG_HEI && image[ localp->x][localp->y+1] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x, localp->y+1}; 
			pq[pq_right++] = localp2;
			image[ localp->x][localp->y+1] = *label; 
			pr_mul *= Pr[localp->x][localp->y+1] / 10000.f;
		}
		if(localp->y+1<IMG_HEI && localp->x+1<IMG_WID && image[ localp->x+1][localp->y+1] == CONS_FOREGROUND)
		{
			POINT_t localp2 = {localp->x+1, localp->y+1}; 
			pq[pq_right++] = localp2;
			image[localp->x+1][localp->y+1] = *label; 
			pr_mul *= Pr[localp->x+1][localp->y+1] / 10000.f;
		}

		rect[0] = min(rect[0], localp->x);
		rect[2] = max(rect[2], localp->x);
		rect[1] = min(rect[1], localp->y);
		rect[3] = max(rect[3], localp->y);

		(*count)++;
		pq_left++;
	}

	return pr_mul;
}

void 
kdebs_init(
		   KDEBSM_t* model,						// Model
		   unsigned char* src, int widstep,		// Source image (Must be pre-resized to IMG_WID, IMG_HEI)
		   int Wlong)							// Long-term window
{
	int i, j;

	// Exponential function, divided by LOOKUP_TALBE_SIZE, range from 0 to 10
	for (i=0;i<LOOKUP_TALBE_SIZE;i++)
	{
		model->lookup1[i] = (int) (10000 * exp(-i/EXP_DIV));
	}

	// Sqrt function, range from 0 - 2*PI*LOOKUP_TALBE_SIZE
	for (i=0;i<LOOKUP_TALBE_SIZE;i++)
	{
		model->lookup2[i] = (int) (10000 / sqrt(2*3.1415926*i));
	}

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			model->src0[i][j].r = *(src+widstep*j+3*i);
			model->src0[i][j].g = *(src+widstep*j+3*i+1);
			model->src0[i][j].b = *(src+widstep*j+3*i+2);
		}
	}

	model->framenum = 1;
	model->Nshort = N_SHORT;
	model->Wlong = Wlong;
	model->use_long_term = 1;

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			model->pixels_st[i][j].cap = N_SHORT;
			model->pixels_st[i][j].tail = -1;

			// Default bandwidth^2
			model->pixels_st[i][j].phi.r = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
			model->pixels_st[i][j].phi.g = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
			model->pixels_st[i][j].phi.b = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
		} 
	}

	// Both long-term and short-term use Nshort samples to KDE;
	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			model->pixels_lt[i][j].cap = N_SHORT;
			model->pixels_lt[i][j].tail = -1;

			// Default bandwidth^2
			model->pixels_lt[i][j].phi.r = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
			model->pixels_lt[i][j].phi.g = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
			model->pixels_lt[i][j].phi.b = (int) (DEFAULT_COV*DEFAULT_COV/BAND_DIV);
		}
	}

	// For pixels belongs to foreground, we init the samples queue;
	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			int n = 0;
			KDSPX_t* sts = model->pixels_st[i][j].s;
			KDSPX_t* lts = model->pixels_lt[i][j].s;

			// tail will add
			model->pixels_st[i][j].tail = 1;
			model->pixels_lt[i][j].tail = 2;

			model->pixels_st[i][j].llen = N_SHORT;
			model->pixels_lt[i][j].llen = N_SHORT;

			sts[0].r = *(src+widstep*j+3*i);
			sts[0].g = *(src+widstep*j+3*i+1);
			sts[0].b = *(src+widstep*j+3*i+2);

			lts[0].r = *(src+widstep*j+3*i);
			lts[0].g = *(src+widstep*j+3*i+1);
			lts[0].b = *(src+widstep*j+3*i+2);

			for (n=1;n<N_SHORT;n++)
			{
				sts[n].r = (rand() % 3 - 1)*DEFAULT_COV + sts[n-1].r;
				sts[n].g = (rand() % 3 - 1)*DEFAULT_COV + sts[n-1].g;
				sts[n].b = (rand() % 3 - 1)*DEFAULT_COV + sts[n-1].b;

				lts[n].r = (rand() % 3 - 1)*DEFAULT_COV + lts[n-1].r;
				lts[n].g = (rand() % 3 - 1)*DEFAULT_COV + lts[n-1].g;
				lts[n].b = (rand() % 3 - 1)*DEFAULT_COV + lts[n-1].b;
			}
		}
	}
}

// kdebs_free
// Free the model;
void kdebs_free(KDEBSM_t* model)
{
	// o, we have nothing to be deleted.
}

// kdebs_phase1 
// Detect foreground mask by either short-model or long-term model;
void 
kdebs_phase1(		   
	KDEBSM_t* model,				// Model
	KDSAMPLE_t term[IMG_WID][IMG_HEI],// Term model
	int Pr[IMG_WID][IMG_HEI],		// Pr buffer,
	int fore_img[IMG_WID][IMG_HEI], // Foreground buffer
	unsigned char* src,	int widstep,// Source image
	int th)							// Threshold
{
	int i, j;

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			KDSPX_t pixel;
			int pr;

			pixel.r = *(src+widstep*j+3*i);
			pixel.g = *(src+widstep*j+3*i+1);
			pixel.b = *(src+widstep*j+3*i+2);

			pr = kdebs_density(model, &pixel, &term[i][j], th);

			if( pr > th )
				fore_img[i][j] = 0;
			else
				fore_img[i][j] = 1;

			Pr[i][j] = pr;
		}
	}
}

// kdebs_phase2
// False alarm suppression;
void 
kdebs_phase2(
	KDEBSM_t* model,					// Model
	KDSAMPLE_t term[IMG_WID][IMG_HEI],	// Term model
	int Pr[IMG_WID][IMG_HEI],			// Pr buffer,
	int fore_img[IMG_WID][IMG_HEI],		// Foreground buffer
	unsigned char* src, int widstep,	// Source image
	int th,								// Threshold1
	float th2								// THreshold2
)
{
	int	i, j, x, y;
	int	lx=0, ly=0;
	int	comp_rect[4]; 

	int	pixel_num	= 0;
	int	clabel		= 2;
	int	count		= 0;

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			KDSPX_t pixel;
			int pr;
			int maxpr=0;
			int l, t, r, b;

			// Background
			if( fore_img[i][j] == 0 )
				continue;

			// For every foreground pixel, we compute the PDF around its neighborhood;
			pixel.r = *(src+widstep*j+3*i);
			pixel.g = *(src+widstep*j+3*i+1);
			pixel.b = *(src+widstep*j+3*i+2);

			l = i - 3; l = max(0, l);
			r = i + 3 + 1; r = min(IMG_WID, r);
			t = j - 3; t = max(0, t);
			b = j + 3 + 1; b = min(IMG_HEI, b);

			for (y=t;y<b;y++)
			{
				for (x=l;x<r;x++)
				{
					if( fore_img[x][y] )
						continue;

					pr = kdebs_density(model, &pixel, &term[x][y], th);

					maxpr = max(pr, maxpr);
				} 
			}

			Pr[i][j] = maxpr;
		}
	}

	// Group background detection using CCA;

	do
	{
		float pr_mul = cca8(model, fore_img, &lx, &ly, Pr, comp_rect, &pixel_num, &clabel);

		// Ignore small components
		if( pixel_num == 0) 
		{
			continue;
		}
		else
		{
			int x, y;

			if( pr_mul > th2 )
			{
				// Get AvgPN
				for (y=comp_rect[1];y<=comp_rect[3];y++)
				{
					for (x=comp_rect[0];x<=comp_rect[2];x++)
					{
						int ilabel = fore_img[x][y];
						int prmax = Pr[x][y];

						if( ilabel == clabel && prmax > th ) {
							fore_img[x][y] = 0;
						} 
					}
				}
			}
		}
	} while(pixel_num != 0);
}

// kdebs_shortterm
// Short term result 
void kdebs_shortterm(
					 KDEBSM_t* model,
					 unsigned char* src, int widstep,
					 int th1, float th2)
{
	kdebs_phase1(model, model->pixels_st, model->St_Pr, model->St_fore, src, widstep, th1);
	kdebs_phase2(model, model->pixels_st, model->St_Pr, model->St_fore, src, widstep, th1, th2);
}

// kdebs_shortterm
// Short term result 
void kdebs_longterm(
					 KDEBSM_t* model,
					 unsigned char* src, int widstep, 
					 int th1, float th2)
{
	kdebs_phase1(model, model->pixels_lt, model->Lt_Pr, model->Lt_fore, src, widstep, th1);
	kdebs_phase2(model, model->pixels_lt, model->Lt_Pr, model->Lt_fore, src, widstep, th1, th2);
}

// kdebs_foreground
// Foreground detection by short-term & long-term model;
void kdebs_foreground(
					  KDEBSM_t* model,
					  unsigned char* src, int widstep, 
					  int th1, float th2)
{
	int i, j;

	kdebs_shortterm(	model, src, widstep, th1, th2); 
	kdebs_longterm(		model, src, widstep, th1, th2);

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			if( model->St_fore[i][j] && model->Lt_fore[i][j] )
				model->foreground[i][j] = 1;
			else
				model->foreground[i][j] = 0;
		}
	}
}

void kdebs_cuttiny(
	KDEBSM_t* model,					// Model
	int minregion)						// Minimal region
{
	int lx = 0, ly = 0;
	int comp_rect[4];
	int clabel = 2;
	int pixel_num = 0;
	int x, y;

	do
	{
		float pr_mul = cca8(model, model->foreground, &lx, &ly, 
			model->Lt_Pr, comp_rect, &pixel_num, &clabel);

		// Ignore small components
		if( pixel_num < minregion) 
		{
			// Get AvgPN
			for (y=comp_rect[1];y<=comp_rect[3];y++)
			{
				for (x=comp_rect[0];x<=comp_rect[2];x++)
				{
					int ilabel = model->foreground[x][y];

					if( ilabel == clabel ) 
					{
						model->foreground[x][y] = 0;
					} 
				}
			}
		}
	} while( pixel_num );

	for (y=0;y<IMG_HEI;y++)
	{
		for (x=0;x<IMG_WID;x++)
		{
			if( model->foreground[x][y] )
				model->foreground[x][y] = 1;
		}
	}
}

// kdebs_update
// Model update. This function is supposed to be called after kdebs_foreground();
void kdebs_update(
				  KDEBSM_t* model,
				  unsigned char* src, int widstep)
{
	int	i, j, k;

	model->framenum ++;

	// Blind Update for Long-term model
	if( model->framenum % (model->Wlong / model->Nshort) == 0 ) 
	{
		for (j=0;j<IMG_HEI;j++)
		{
			for (i=0;i<IMG_WID;i++)
			{
				KDSAMPLE_t* s = &model->pixels_lt[i][j];

				if( s->llen < s->cap ) 
				{
					if( model->foreground[i][j] == 0 ) 
					{
						if( s->tail < 0 )
							s->tail = 0;

						s->s[s->tail].r		= model->src0[i][j].r;
						s->s[s->tail].g		= model->src0[i][j].g;
						s->s[s->tail].b		= model->src0[i][j].b;
						s->s[s->tail+1].r	= *(src+widstep*j+3*i);
						s->s[s->tail+1].g	= *(src+widstep*j+3*i+1);
						s->s[s->tail+1].b	= *(src+widstep*j+3*i+2);

						s->tail += 2;
						s->llen += 2;

						s->phi.r = 0;
						s->phi.g = 0;
						s->phi.b = 0;
						for (k=0;k<s->llen;k+=2)
						{
							s->phi.r += (s->s[k+1].r - s->s[k].r) * (s->s[k+1].r - s->s[k].r);
							s->phi.g += (s->s[k+1].g - s->s[k].g) * (s->s[k+1].g - s->s[k].g);
							s->phi.b += (s->s[k+1].b - s->s[k].b) * (s->s[k+1].b - s->s[k].b);
						}

						s->phi.r /= s->llen/2;
						s->phi.g /= s->llen/2;
						s->phi.b /= s->llen/2;

						s->phi.r = (int) (s->phi.r/BAND_DIV);
						s->phi.g = (int) (s->phi.g/BAND_DIV);
						s->phi.b = (int) (s->phi.b/BAND_DIV);

						s->phi.r = min(max(s->phi.r, MIN_COV), MAX_COV);
						s->phi.g = min(max(s->phi.g, MIN_COV), MAX_COV);
						s->phi.b = min(max(s->phi.b, MIN_COV), MAX_COV);
					}
				}
				else 
				{
					s->s[s->tail].r		= model->src0[i][j].r;
					s->s[s->tail].g		= model->src0[i][j].g;
					s->s[s->tail].b		= model->src0[i][j].b;
					s->s[s->tail+1].r	= *(src+widstep*j+3*i);
					s->s[s->tail+1].g	= *(src+widstep*j+3*i+1);
					s->s[s->tail+1].b	= *(src+widstep*j+3*i+2);

					s->phi.r = 0;
					s->phi.g = 0;
					s->phi.b = 0;

					for (k=0;k<s->llen;k+=2)
					{
						s->phi.r += (s->s[k+1].r - s->s[k].r) * (s->s[k+1].r - s->s[k].r);
						s->phi.g += (s->s[k+1].g - s->s[k].g) * (s->s[k+1].g - s->s[k].g);
						s->phi.b += (s->s[k+1].b - s->s[k].b) * (s->s[k+1].b - s->s[k].b);
					}

					s->phi.r /= s->llen/2;
					s->phi.g /= s->llen/2;
					s->phi.b /= s->llen/2;

					s->phi.r = (int) (s->phi.r/BAND_DIV);
					s->phi.g = (int) (s->phi.g/BAND_DIV);
					s->phi.b = (int) (s->phi.b/BAND_DIV);

					s->phi.r = min(max(s->phi.r, MIN_COV), MAX_COV);
					s->phi.g = min(max(s->phi.g, MIN_COV), MAX_COV);
					s->phi.b = min(max(s->phi.b, MIN_COV), MAX_COV);

					s->tail += 2;
					if( s->tail >= s->cap )
						s->tail = 0;
				}
			}
		}
	}

	// Selective update for short-term model
	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			KDSAMPLE_t* s = &model->pixels_st[i][j];

			if( s->llen < s->cap )
			{
				if( model->foreground[i][j] == 0 ) {
					if( s->tail < 0 )
						s->tail = 0;

					s->s[s->tail].r		= model->src0[i][j].r;
					s->s[s->tail].g		= model->src0[i][j].g;
					s->s[s->tail].b		= model->src0[i][j].b;
					s->s[s->tail+1].r	= *(src+widstep*j+3*i);
					s->s[s->tail+1].g	= *(src+widstep*j+3*i+1);
					s->s[s->tail+1].b	= *(src+widstep*j+3*i+2);

					s->tail += 2;
					s->llen += 2;

					s->phi.r = 0;
					s->phi.g = 0;
					s->phi.b = 0;
					for (k=0;k<s->llen;k+=2)
					{
						s->phi.r += (s->s[k+1].r - s->s[k].r) * (s->s[k+1].r - s->s[k].r);
						s->phi.g += (s->s[k+1].g - s->s[k].g) * (s->s[k+1].g - s->s[k].g);
						s->phi.b += (s->s[k+1].b - s->s[k].b) * (s->s[k+1].b - s->s[k].b);
					}

					s->phi.r /= s->llen/2;
					s->phi.g /= s->llen/2;
					s->phi.b /= s->llen/2;

					s->phi.r = (int) (s->phi.r/BAND_DIV);
					s->phi.g = (int) (s->phi.g/BAND_DIV);
					s->phi.b = (int) (s->phi.b/BAND_DIV);

					s->phi.r = min(max(s->phi.r, MIN_COV), MAX_COV);
					s->phi.g = min(max(s->phi.g, MIN_COV), MAX_COV);
					s->phi.b = min(max(s->phi.b, MIN_COV), MAX_COV);
				}
			}
			else {
				if( model->foreground[i][j] == 0 ) 
				{

					// In the short-term model, the frames are always consequence;
					s->s[s->tail].r		= *(src+widstep*j+3*i);
					s->s[s->tail].g		= *(src+widstep*j+3*i+1);
					s->s[s->tail].b		= *(src+widstep*j+3*i+2);

					s->tail ++;
					if( s->tail >= s->cap )
						s->tail = 0;

					s->phi.r = 0;
					s->phi.g = 0;
					s->phi.b = 0;

					for (k=0;k<s->llen-1;k++)
					{
						s->phi.r += (s->s[k+1].r - s->s[k].r) * (s->s[k+1].r - s->s[k].r);
						s->phi.g += (s->s[k+1].g - s->s[k].g) * (s->s[k+1].g - s->s[k].g);
						s->phi.b += (s->s[k+1].b - s->s[k].b) * (s->s[k+1].b - s->s[k].b);
					}

					s->phi.r /= s->llen-1;
					s->phi.g /= s->llen-1;
					s->phi.b /= s->llen-1;

					s->phi.r = (int) (s->phi.r/BAND_DIV);
					s->phi.g = (int) (s->phi.g/BAND_DIV);
					s->phi.b = (int) (s->phi.b/BAND_DIV);

					s->phi.r = min(max(s->phi.r, MIN_COV), MAX_COV);
					s->phi.g = min(max(s->phi.g, MIN_COV), MAX_COV);
					s->phi.b = min(max(s->phi.b, MIN_COV), MAX_COV);
				}
			}
		}
	}

	for (j=0;j<IMG_HEI;j++)
	{
		for (i=0;i<IMG_WID;i++)
		{
			model->src0[i][j].r = *(src+widstep*j+3*i);
			model->src0[i][j].g = *(src+widstep*j+3*i+1);
			model->src0[i][j].b = *(src+widstep*j+3*i+2);
		}
	}
}

// kdebs_density
// Estimate the density related to a given sample sequence;
int kdebs_density(KDEBSM_t* model, KDSPX_t* pl, KDSAMPLE_t* pm, int th)
{
	int i;
	int dist;
	int Pr = 0;
	int div2 = (int)(EXP_DIV / 2);

	if( pm->tail < 0 )
		return 0;

	for (i=0;i<pm->llen;i++)
	{
		int Pdr = 0;

		dist = (pm->s[i].r - pl->r) * (pm->s[i].r - pl->r);
		dist = dist * div2 / pm->phi.r;

		if( dist > LOOKUP_TALBE_SIZE )
			continue;

		Pdr = model->lookup1[dist] / model->lookup2[pm->phi.r];

		dist = (pm->s[i].g - pl->g) * (pm->s[i].g - pl->g);
		dist = dist * div2 / pm->phi.g;

		if( dist > LOOKUP_TALBE_SIZE )
			continue;

		Pdr *= model->lookup1[dist] / model->lookup2[pm->phi.g];

		dist = (pm->s[i].b - pl->b) * (pm->s[i].b - pl->b);
		dist = dist * div2 / pm->phi.b;

		if( dist > LOOKUP_TALBE_SIZE )
			continue;

		Pdr *= model->lookup1[dist] / model->lookup2[pm->phi.b];

		Pr += Pdr;
		if( Pr > th )
			return Pr;
	}

	return Pr;
}