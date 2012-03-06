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

void getbrightness(arrays *a, ProgressViewer *progress, int progresspart )
{
	float wavelength;
	int i, j, k, depth;
        int progress_steps_number = BRIGHTNESS1NUMBER*2 + BRIGHTNESS2NUMBER*2;
        int progress_bit = progresspart/progress_steps_number;
	/*
	a->brightnessq = (float ***)malloc(sizeof(float **)*2);
	for (depth = 0; depth < 2; depth++)
	{
		a->brightnessq[depth] = (float **)malloc(a->heightq*sizeof(float *));
		for (i = 0; i < a->heightq; i++)
		{
			a->brightnessq[depth][i] = (float *)malloc(a->widthq*sizeof(float));
		}
	}
	log << "brightnessq array height: " << a->heightq << " width: " << a->widthq << endl;
	a->brightnessh = (float ***)malloc(sizeof(float **)*5);
	for (depth = 0; depth < 5; depth++)
	{
		a->brightnessh[depth] = (float **)malloc(a->heighth*sizeof(float *));
		for (i = 0; i < a->heighth; i++)
		{
			a->brightnessh[depth][i] = (float *)malloc(a->widthh*sizeof(float));
		}
	}
	log << "brightnessh array height: " << a->heighth << " width: " << a->widthh << endl;*/
	a->brightness1 = (float ***)malloc(sizeof(float **)*15);
	for (j = 0; j < BRIGHTNESS1NUMBER; j++)
	{
		depth = brightness1essential[j];
		a->brightness1[depth] = (float **)malloc(a->height1*sizeof(float *));
		for (i = 0; i < a->height1; i++)
		{
			a->brightness1[depth][i] = (float *)malloc(a->width1*sizeof(float));
		}
		progress->setValue(progress->Value() + progress_bit);
	}

	a->brightness2 = (float ***)malloc(sizeof(float **)*16);
	for (j = 0; j < BRIGHTNESS2NUMBER; j++)
	{
		depth = brightness2essential[j];
		a->brightness2[depth] = (float **)malloc(a->height2*sizeof(float *));//width2
		for (i = 0; i < a->height2; i++)//width2
		{
			//brightness2[0][depth][i] = (float *)malloc(height2*sizeof(float));
			a->brightness2[depth][i] = (float *)calloc(a->width2, sizeof(float));
		}
		progress->setValue(progress->Value() + progress_bit);
	}

	for (k = 0; k < BRIGHTNESS2NUMBER; k++)
	{
		depth = brightness2essential[k];
		wavelength = wavelengths[wt[START2 + depth]];

		for (i = 0; i < a->height2; i++)
		{
			for (j = 0; j < a->width2; j++)
			{
				a->brightness2[depth][i][j] = radiancetobrightness(a->radiance2[depth][i][j], wavelength, a->scales2[depth], a->offsets2[depth]);
			}
			/*if (wt[depth + 22] == 32)
			{
				log << "SCALE " << scales[START2 + depth] << endl;
				log << brightness2[0][depth][i][j-1] << endl;
			}*/
		}
		progress->setValue(progress->Value() + progress_bit);
	}

	for (j = 0; j < BRIGHTNESS1NUMBER; j++)
	{
		depth = brightness1essential[j];
		wavelength = wavelengths[wt[START1 + depth]]; // _+ wavetable
		for (i = 0; i < a->height1; i++)
		{
			for (j = 0; j < a->width1; j++)
			{
				a->brightness1[depth][i][j] = radiancetobrightness(a->radiance1[depth][i][j], wavelength, a->scales1[depth], a->offsets1[depth]);
			}

		}
		progress->setValue(progress->Value() + progress_bit);
	}
	/*for(depth = 0; depth < 5; depth++)
	{
		wavelength = wavelengths[wt[STARTH + depth]];
		for (i = 0; i < a->heighth; i++)
		{
			for (j = 0; j < a->widthh; j++)
			{
				a->brightnessh[depth][i][j] = radiancetobrightness(a->radianceh[depth][i][j], wavelength, a->scalesh[depth], a->offsetsh[depth]);
			}
		}
	}
	for(depth = 0; depth < 2; depth++)
	{
		wavelength = wavelengths[wt[depth]];
		for (i = 0; i < a->heightq; i++)
		{
			for (j = 0; j < a->widthq; j++)
			{
				a->brightnessq[depth][i][j] = radiancetobrightness(a->radianceq[depth][i][j], wavelength, a->scalesq[depth], a->offsetsq[depth]);
			}
		}
	}*/
}
