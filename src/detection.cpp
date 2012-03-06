/*
 * detection.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "detection.h"

float lookupt11m12[7] = {0.5,	1.0,	1.5,	2.0,	2.5,	3.0,	3.5};
float lookupt8m11[7] = {-3.5,	-4.5,	-5.3,	-5.9,	-6.15,	-6.25,	-6.3};
char maskbit[7] = {1, 2, 4, 8, 16, 32, 64};

void cirrustest(int *height, int *width, short unsigned int **radiance, float threshold, float32 scale, float32 offset, bool **cloudmask)
{
	int i, j, ki, kj;
	int heightq = height[0];
	int widthq = width[0];
	int height1 = height[2];
	int width1 = width[2];

	double jinjj = widthq/(double)width1;
	double iinii = heightq/(double)height1;
		for (i = 0; i < height1; i++)
			for (j = 0; j < width1; j++)
			{
				if ((radiance[i][j] - offset)*scale > threshold)
				{
					for (ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
					{
						for (kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
						{
							cloudmask[ki][kj] = true;
						}
					}
				}
			}
}

void relationtest(arrays *a, int thresholdlow, int thresholdhigh, int cloudmaskid)
{
	int i, j;
	short unsigned int **radiance1 = a->radianceq[1];
	short unsigned int **radiance0 = a->radianceq[0];
	char **cloudmask = a->cloudmask;
	double rr;
	for (i = 0; i < a->heightq; i ++)
		for (j = 0; j < a->widthq; j++)
		{
			rr = ((radiance1[i][j] - a->offsetsq[1])*a->scalesq[1])/((radiance0[i][j] - a->offsetsq[0])*a->scalesq[0]);
			if ((rr < thresholdhigh)&&(rr > thresholdlow))
			{
				if (radiance1[i][j] > WATER250THRESHOLD1)
				{
					cloudmask[i][j] += maskbit[cloudmaskid];
				}
			}
		}
}

void simplebttest(arrays *a, float threshold, int cloudmaskid)
{
	int i, j, ki, kj;
	float **brightness = a->brightness2[T11];
	char **cloudmask = a->cloudmask;
	double jinjj = a->widthq/(double)a->width2;
	double iinii = a->heightq/(double)a->height2;
		for (i = 0; i < a->height2; i++)
		{
			for (j = 0; j < a->width2; j++)
			{
				if (brightness[i][j] < threshold)
				{
					if (brightness[i][j] > 0.0)
					for (ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < a->heightq); ki++)
					{
						for (kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < a->widthq); kj++)
						{
							cloudmask[ki][kj] += maskbit[cloudmaskid];
						}
					}
				}
			}
		}
}

void differencetest(arrays *a, int cloudmaskid)
{
	float **brightness11 = a->brightness2[T11];
	float **brightness12 = a->brightness2[T12];
	float **brightness8 = a->brightness2[T8];
	float t11m12, t8m11;
	int i, j, k;

	double jinjj = a->widthq/(double)a->width2;
	double iinii = a->heightq/(double)a->height2;
	for (i = 0; i < a->height2; i ++)
		for (j = 0; j < a->width2; j++)
		{
			t11m12 = brightness11[i][j] - brightness12[i][j];
			t8m11 = brightness8[i][j] - brightness11[i][j];
			k = -1;
			if ((t11m12 <= lookupt11m12[6])&&(t11m12 >= lookupt11m12[0]))
			{
				k = 0;
				while ((k < 7)&&(t11m12 < lookupt11m12[k]))
				{
					k++;
				}
			}
			if ((k != -1) && (t8m11 > lookupt8m11[k] - 0.5) && (t8m11 < lookupt8m11[k] + 0.5))
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < a->heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < a->widthq); kj++)
					{
						a->cloudmask[ki][kj] += maskbit[cloudmaskid];
					}
				}
			}
		}
}

void differencetest19(arrays *a, float threshold, int cloudmaskid)
{

	float t11m39;
	int i, j;
	float **brightness11 = a->brightness2[T11];
	float **brightness39 = a->brightness2[T39];
	char **cloudmask = a->cloudmask;
	double jinjj = a->widthq/(double)a->width2;
	double iinii = a->heightq/(double)a->height2;
	for (i = 0; i < a->height2; i ++)
		for (j = 0; j < a->width2; j++)
		{
			t11m39 = brightness11[i][j] - brightness39[i][j];
			if (t11m39 < threshold)
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < a->heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < a->widthq); kj++)
					{
						cloudmask[ki][kj] += maskbit[cloudmaskid];
					}
				}
			}
		}
}

void nearbydifferencetest(int *height, int *width, float **brightness, float threshold, bool **cloudmask)
{
	int i, j, ki, kj;
	int heightq = height[0];
	int widthq = width[0];
	int height2 = height[3];
	int width2 = width[3];
	double jinjj = widthq/(double)width2;
	double iinii = heightq/(double)height2;
		for (i = 1; i < height2; i ++)//width
			for (j = 1; j < width2 - 1; j++)
			{
				if 	((abs(brightness[i][j] - brightness[i-1][j]) > threshold)||
					(abs(brightness[i][j] - brightness[i+1][j]) > threshold)||
					(abs(brightness[i][j] - brightness[i][j + 1]) > threshold)||
					(abs(brightness[i][j] - brightness[i][j - 1]) > threshold))
				{
					if (brightness[i][j] > 0.0)
					for (ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
					{
						for (kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
						{
							cloudmask[ki][kj] = true;
						}
					}
				}
			}
}

void nightdifferencetest17(int *height, int *width, float **brightness37, float **brightness12, float threshold, bool **cloudmask)
{
	float t37m12;
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	int height2 = height[3];
	int width2 = width[3];
	double jinjj = widthq/(double)width2;
	double iinii = heightq/(double)height2;
	for (i = 0; i < height2; i ++)
		for (j = 0; j < width2; j++)
		{
			t37m12 = brightness37[i][j] - brightness12[i][j];
			if (t37m12 < threshold)
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
					{
						cloudmask[ki][kj] = true;
					}
				}
			}
		}
}

void nightdifferencetest23(int *height, int *width, float **brightness73, float **brightness11, float threshold, bool **cloudmask)
{
	float t73m11;
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	int height2 = height[3];
	int width2 = width[3];
	double jinjj = widthq/(double)width2;
	double iinii = heightq/(double)height2;
	for (i = 0; i < height2; i ++)
		for (j = 0; j < width2; j++)
		{
			t73m11 = brightness73[i][j] - brightness11[i][j];
			if (t73m11 < threshold)
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
					{
						cloudmask[kj][ki] = true;
					}
				}
			}
		}
}

void wintertest(int *height, int *width, float **brightness11, float **brightness67, float threshold, bool **cloudmask)
{
	float t11m67;
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	int height2 = height[3];
	int width2 = width[3];
	double jinjj = widthq/(double)width2;
	double iinii = heightq/(double)height2;
	for (i = 0; i < height2; i ++)
		for (j = 0; j < width2; j++)
		{
			t11m67 = brightness11[i][j] - brightness67[i][j];
			if (t11m67 < threshold)
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
					{
						cloudmask[ki][kj] = true;
					}
				}
			}
		}
}

void visiblereflectancetest(int *height, int *width, int **reflectance66, int **reflectance87, float threshold66, float threshold87, float32 *scales, float32 *offsets, bool **cloudmask)
{
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	for (i = 0; i < heightq; i++)
	      for (j = 0; j < widthq; j++)
	      {

	      }
}

void shadowtest()
{

}

bool detect(arrays *a, ProgressViewer *progress, int progresspart)
{
	int progress_steps_number = 3;
        int progress_bit = progresspart/progress_steps_number;

	relationtest(a, THRESHOLD250LOW, THRESHOLD250HIGH, 0);
        progress->setValue(progress->Value() + progress_bit);
	simplebttest(a, THRESHOLDT11, 1);
        progress->setValue(progress->Value() + progress_bit);
	differencetest(a, 2);
        progress->setValue(progress->Value() + progress_bit);
	//nearbydifferencetest(height, width, brightness2[T11], 4, cloudmask[3]);
	differencetest19(a, -14,  4);
	//cirrustest(height, width, radiance1[T26], CIRRUSTHRESHOLD, scales[T26 + START1], offsets[T26 + START1], cloudmask[7]);
	//wintertest(height, width, brightness2[T11], brightness2[T67], -10, cloudmask[10]);
	//nightdifferencetest17(height, width, brightness1[T37], brightness2[T12], 10, cloudmask[5]);
	//nightdifferencetest23(height, width, brightness2[T73], brightness2[T11], -10, cloudmask[6]);

	//2 T11, 2 T12, 2 T8, 2 T39
	//1 T37,

	return true;

}
