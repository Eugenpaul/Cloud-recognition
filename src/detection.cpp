/*
 * detection.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "detection.h"

bool detect(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, unsigned short ***radiance2,
			float ***brightness1, float ***brightnessh, float ***brightnessq, float ***brightness2,
			int *height, int *width, ofstream &log, bool ***cloudmask)
{
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	int heighth = height[1];
	int widthh = width[1];
	int height1 = height[2];
	int width1 = width[2];
	int height2 = height[3];
	int width2 = width[3];
	bool clouddetected = false;
	for (i = 0; i < height1; i ++)
		for (j = 0; j < width1; j++)
		{

			clouddetected = false;
			if (radiance1[INFRAREDBAND][i][j] > INFRARED_THRESHOLD)
			{
				clouddetected = true;
				cloudmask[0][i][j] = true;
			}
			if (radiance1[ULTRAVIOLETBAND][i][j] > ULTRAVIOLET_THRESHOLD)
			{
				clouddetected = true;
				cloudmask[0][i][j] = true;
			}
		}

	double iinii = heightq/(double)height1, jinjj = widthq/(double)width1;
	for (i = 0; i < height1; i ++)
		for (j = 0; j < width1; j++)
		{
			if (radiance1[INFRAREDBAND][i][j] > HIGHT_CLOUD_THRESHOLD)
			{
				for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
				{
					for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
					{
						cloudmask[0][ki][kj] = true;
					}
				}
			}
		}
	for (i = 0; i < heightq; i ++)
		for (j = 0; j < widthq; j++)
			if ((radianceq[1][i][j]/radianceq[0][i][j] < THRESHOLD250HIGH)&&(radianceq[1][i][j]/radianceq[0][i][j] > THRESHOLD250LOW))
			{
				if (radianceq[1][i][j] > WATER250THRESHOLD1)
				{
					cloudmask[0][i][j] = true;
				}
				else
				{
					cloudmask[0][i][j] = false;
				}
			}
	iinii = heightq/(double)height2; jinjj = widthq/(double)width2;
		for (i = 0; i < height2; i ++)
			for (j = 0; j < width2; j++)
			{
				//log << brightness2[T11][i][j] << " ";
				if (brightness2[T11][i][j] < THRESHOLDT11)
				{
					if (brightness2[T11][i][j] > 0.0)
					for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
					{
						for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
						{
							cloudmask[1][ki][kj] = true;
						}
					}
					//printf("%f-%d, ", brightness2[T11][i][j], radiance2[T11][i][j]);

				}
				if ((brightness2[T11][i][j] - brightness2[T12][i][j]) > 2 )
				{
					if (brightness2[T11][i][j] > 0.0)
					for (int ki = ((int)((i)*iinii)); (ki < ((int)((i+1)*iinii)))&&(ki < heightq); ki++)
					{
						for (int kj = ((int)((j)*jinjj)); (kj < ((int)((j+1)*jinjj)))&&(kj < widthq); kj++)
						{
							cloudmask[2][ki][kj] = true;
						}
					}
				}
				//printf("%f ", (brightness2[T11][i][j] - brightness2[T12][i][j]));

			}

	return true;

}
