/*
 * pic.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "pic.h"

using namespace std;

bool savepicture(char *filename,  unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, bool ***cloudmask, int *height, int *width, ofstream &log)
{
	int widthq = width[0];
	int heightq = height[0];
	int widthh = width[1];
	int heighth = height[1];
	int width1 = width[2];
	int height1 = height[2];
	bool detected = false;
	int i; int j;
	log << "allocating image array " << widthq << "x" << heightq << "... ";
	log.flush();
	png::image<png::rgb_pixel> image(widthq,heightq);
	image.resize(widthq, heightq);
	log << "done\n";
	log.flush();


	/*sprintf(temppath, "./img/%s", TEMP_PATH);
	if (!exists(temppath))
	{
		mkdir(temppath, 0777);
	}*/


	int hi = 0;
	int hj = 0;
	int r, g, b;
	for (i = 0; i < heightq; i++)
	{
		hi = int(heighth/(double)heightq*i);
		for (j = 0 ; j < widthq; j++)
		{
			hj = int(widthh/(double)widthq*j);
			r = (radianceq[0][i][j]*255)/16000;
			g = (radianceh[1][hi][hj]*255)/16000;
			b = (radianceh[0][hi][hj]*255)/16000;
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			image[i][j] = png::rgb_pixel(r, g, b);
		}
	}
	image.write(filename);

	r = 55;
	g = 55;
	b = 55;
	for (i = 0; i < heightq; i++)
	{
		for (j = 0; j < widthq; j++)
		{
			r = 55;
			g = 55;
			b = 55;
			detected = false;
			if (cloudmask[0][i][j])
			{
				r += 100;
				detected = true;
				//image[i][j] = png::rgb_pixel(255, 70, 70);
			}
			if (cloudmask[1][i][j])
			{
				b += 100;
				detected = true;
				//image[i][j] = png::rgb_pixel(70, 70, 255);
			}

			if (cloudmask[2][i][j])
			{
				g += 100;
				detected = true;
				//image[i][j] = png::rgb_pixel(70, 255, 70);
			}
			if (detected)
			{
				image[i][j] = png::rgb_pixel(r, g, b);
			}

		}
	}
	sprintf(filename, "%s%s", filename, "mask.png");
	image.write(filename);

	return true;
}
