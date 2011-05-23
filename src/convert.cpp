/*
 * convert.cpp
 *
 *  Created on: May 17, 2011
 *      Author: eugenpaul
 */

#include "convert.h"

float radiancetobrightness(int radiance, float wavelength, float32 scale, float32 offset)
{
	//return (c2/(wavelength*log(c1/((radiance - 1577.3397)*(8.40022E-4)*pi*wavelength*wavelength*wavelength*wavelength*wavelength) + 1)));
	return (c2/(wavelength*log(c1/((radiance - offset)*(scale)*pi*wavelength*wavelength*wavelength*wavelength*wavelength) + 1)));
	///// offset 1577.3397, scale 8.40022E-4;
}

void getbrightness(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, unsigned short ***radiance2,
					int *height, int *width,
					float ****brightness1, float ****brightnessh, float ****brightnessq, float ****brightness2,
					float32 *scales, float32 *offsets,
					ofstream &log)
{
	int heightq = height[0];
	int widthq = width[0];
	int heighth = height[1];
	int widthh = width[1];
	int height1 = height[2];
	int width1 = width[2];
	int height2 = height[3];
	int width2 = width[3];
	float wavelength;
	int i, j, depth;


	*brightnessq = (float ***)malloc(sizeof(float **)*2);
	for (depth = 0; depth < 2; depth++)
	{
		brightnessq[0][depth] = (float **)malloc(heightq*sizeof(float *));
		for (i = 0; i < heightq; i++)
		{
			brightnessq[0][depth][i] = (float *)malloc(widthq*sizeof(float));
		}
	}
	*brightnessh = (float ***)malloc(sizeof(float **)*5);
	for (depth = 0; depth < 5; depth++)
	{
		brightnessh[0][depth] = (float **)malloc(heighth*sizeof(float *));
		for (i = 0; i < heighth; i++)
		{
			brightnessh[0][depth][i] = (float *)malloc(widthh*sizeof(float));
		}
	}
	*brightness1 = (float ***)malloc(sizeof(float **)*15);
	for (depth = 0; depth < 15; depth++)
	{
		brightness1[0][depth] = (float **)malloc(height1*sizeof(float *));
		for (i = 0; i < height1; i++)
		{
			brightness1[0][depth][i] = (float *)malloc(width1*sizeof(float));
		}
	}
	*brightness2 = (float ***)malloc(sizeof(float **)*16);
	for (depth = 0; depth < 16; depth++)
	{

		brightness2[0][depth] = (float **)malloc(height2*sizeof(float *));
		for (i = 0; i < height2; i++)
		{
			brightness2[0][depth][i] = (float *)malloc(width2*sizeof(float));
		}
	}


	for(depth = 0; depth < 15; depth++)
	{
		wavelength = wavelengths[wt[START1 + depth]]; // _+ wavetable
		for (i = 0; i < width1; i++)
		{
			for (j = 0; j < height1; j++)
			{
				brightness1[0][depth][i][j] = radiancetobrightness(radiance1[depth][i][j], wavelength, scales[START1 + depth], offsets[START1 + depth]);
			}

		}
	}
	for(depth = 0; depth < 16; depth ++)
	{
		wavelength = wavelengths[wt[START2 + depth]];
		for (i = 0; i < width2; i++)
		{
			for (j = 0; j < height2; j++)
			{
				brightness2[0][depth][i][j] = radiancetobrightness(radiance2[depth][i][j], wavelength, scales[START2 + depth], offsets[START2 + depth]);
				//if (wt[depth + 22] == 32)
				if (brightness2[0][depth][i][j] <= 0.1)
				{
					printf("%d ", radiance2[depth][i][j]);
				}
			}
			/*if (wt[depth + 22] == 32)
			{
				log << "SCALE " << scales[START2 + depth] << endl;
				log << brightness2[0][depth][i][j-1] << endl;
			}*/
		}
	}
	for(depth = 0; depth < 5; depth++)
	{
		wavelength = wavelengths[wt[STARTH + depth]];
		for (i = 0; i < widthh; i++)
		{
			for (j = 0; j < heighth; j++)
			{
				brightnessh[0][depth][i][j] = radiancetobrightness(radianceh[depth][i][j], wavelength, scales[STARTH + depth], offsets[STARTH + depth]);
			}
		}
	}
	for(depth = 0; depth < 2; depth++)
	{
		wavelength = wavelengths[wt[depth]];
		for (i = 0; i < widthq; i++)
		{
			for (j = 0; j < heightq; j++)
			{
				brightnessq[0][depth][i][j] = radiancetobrightness(radianceq[depth][i][j], wavelength, scales[depth], offsets[depth]);
			}
		}
	}
}
