#ifndef __KDE_2000_H__
#define __KDE_2000_H__

#include <math.h>
#include <stdlib.h>

// Std image resolution;
#define IMG_WID	320
#define IMG_HEI 240

#define N_SHORT				50
#define LOOKUP_TALBE_SIZE	10000

typedef struct _kdebs_pixel
{
	int r, g, b;
} KDSPX_t;

typedef struct _kdebs_point
{
	int x, y;
} POINT_t;

typedef struct _kdebs_samples
{
	KDSPX_t		s[N_SHORT];	// Samples. Note that the s(2*i) and s(2*i+1) are 
							// assumed to be sampled consequently;

	int			tail;		// Query tail;
	int			llen;		// Query length		
	int			cap;		// Query capability;
	KDSPX_t		phi;		// Kernel bandwidth; (Fixed point)
} KDSAMPLE_t;

typedef struct _kdebs_model
{
	KDSAMPLE_t	pixels_st[IMG_WID][IMG_HEI];	// Short-term model
	KDSAMPLE_t  pixels_lt[IMG_WID][IMG_HEI];	// Long-term model

	int			Nshort, Wlong;
	int			use_long_term;					// If we perform long-term update

	int			lookup1[LOOKUP_TALBE_SIZE];		// for exp	
	int			lookup2[LOOKUP_TALBE_SIZE];		// for sqrt(2*pi*bandw^2)

	int			St_Pr[IMG_WID][IMG_HEI];		// Pr image for short-term model
	int			Lt_Pr[IMG_WID][IMG_HEI];		// Pr image for long-term model
	int			St_fore[IMG_WID][IMG_HEI];		// Short term foreground;
	int			Lt_fore[IMG_WID][IMG_HEI];		// Long term foreground;
	int			foreground[IMG_WID][IMG_HEI];	// Foreground layer;

	int			framenum;						// Frame counter;
	KDSPX_t		src0[IMG_WID][IMG_HEI];			// Last frame;

	POINT_t		cca_pq[IMG_WID*IMG_HEI];		// query for connected component labeling
} KDEBSM_t;

void 
kdebs_init(
	KDEBSM_t* model,					// Model
	unsigned char* src, int widstep,	// Source image (Must be pre-resized to IMG_WID, IMG_HEI)
	int Wlong);							// Long-term window

void kdebs_free(
	KDEBSM_t* model);					// Model

void kdebs_foreground(
	KDEBSM_t* model,					// Model
	unsigned char* src, int widstep,	// Source image (Must be pre-resized to IMG_WID, IMG_HEI)
	int th1, float th2);				// Two parameters, th1 for single-pixel threshold, th2 for component threshold.

void kdebs_cuttiny(
	KDEBSM_t* model,					// Model
	int minregion);						// Minimal region

void kdebs_update(						
	KDEBSM_t* model,					// Model
	unsigned char* src, int widstep);	// Source image (Must be pre-resized to IMG_WID, IMG_HEI)

#endif