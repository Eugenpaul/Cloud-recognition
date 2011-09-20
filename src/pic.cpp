/*
 * pic.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "pic.h"

using namespace std;

bool savepicture(char *filenamemap, char *filenamemask,  unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, bool ***cloudmask, int *height, int *width, float32 *offsets, float32 *scales, ofstream &log)
{
	char modifiedfilename[255];
	int widthq = width[0];
	int heightq = height[0];
	int widthh = width[1];
	int heighth = height[1];
	//int width1 = width[2];
	//int height1 = height[2];
	bool detected = false;
	int i; int j;
	png::image<png::rgb_pixel> image(widthq,heightq);
	image.resize(widthq, heightq);
	

	int hi = 0;
	int hj = 0;
	int r, g, b;
	
	for (i = 0; i < heightq; i++)
	{
		hi = int(heighth/(double)heightq*i);
		for (j = 0 ; j < widthq; j++)
		{
			hj = int(widthh/(double)widthq*j);
			
			
			r = ((radianceq[0][i][j] - offsets[0])*scales[0]*255)/(150);
			g = ((radianceh[1][hi][hj] - offsets[STARTH + 1])*scales[STARTH+1]*255)/(150);
			b = ((radianceh[0][hi][hj] - offsets[STARTH])*scales[STARTH]*255)/(150);
			
			//r = ((radianceq[0][i][j])*255)/16000;
			//g = ((radianceh[1][hi][hj])*255)/16000;
			//b = ((radianceh[0][hi][hj])*255)/16000;
			//printf("%f %f\n", offsets[0], scales[0]);
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			image[i][j] = png::rgb_pixel(r, g, b);
		}

	}
	image.write(filenamemap);

	/*bool up, down, left, right;
	for (i = 1; i < heightq - 1; i++)
	{
		for (j = 1; j < widthq - 1; j++)
		{
			if ((!cloudmask[0][i][j])&&(!cloudmask[1][i][j])&&(!cloudmask[2][i][j])&&(!cloudmask[4][i][j])&&(!cloudmask[5][i][j])&&(!cloudmask[7][i][j]))
			{
				up = cloudmask[0][i-1][j]||cloudmask[1][i-1][j]||cloudmask[2][i-1][j]||cloudmask[3][i-1][j]||cloudmask[4][i-1][j]||cloudmask[5][i-1][j]||cloudmask[6][i-1][j]||cloudmask[7][i-1][j];
				down = cloudmask[0][i+1][j]||cloudmask[1][i+1][j]||cloudmask[2][i+1][j]||cloudmask[3][i+1][j]||cloudmask[4][i+1][j]||cloudmask[5][i+1][j]||cloudmask[6][i+1][j]||cloudmask[7][i+1][j];
				left = cloudmask[0][i][j-1]||cloudmask[1][i][j-1]||cloudmask[2][i][j-1]||cloudmask[3][i][j-1]||cloudmask[4][i][j-1]||cloudmask[5][i][j-1]||cloudmask[6][i][j-1]||cloudmask[7][i][j-1];
				right = cloudmask[0][i][j+1]||cloudmask[1][i][j+1]||cloudmask[2][i][j+1]||cloudmask[3][i][j+1]||cloudmask[4][i][j+1]||cloudmask[5][i][j+1]||cloudmask[6][i][j+1]||cloudmask[7][i][j+1];
				if(up||down||left||right)
				{
					r = 250;
					g = 0;
					b = 0;
					image[i][j] = png::rgb_pixel(r,g,b);
				}
			}
		}
	}
	sprintf(modifiedfilename, "%s%s", filename, "borders.png");
	image.write(modifiedfilename);
*/
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
				r += 50;
				detected = true;
				//image[i][j] = png::rgb_pixel(255, 70, 70);
			}
			if (cloudmask[1][i][j])
			{
				b += 50;
				detected = true;
				//image[i][j] = png::rgb_pixel(70, 70, 255);
			}

			if (cloudmask[2][i][j])
			{
				//g += 50;
				//detected = true;
				//image[i][j] = png::rgb_pixel(70, 255, 70);
			}
			if (cloudmask[3][i][j]) //nearby
			{
				//g = 100;
				//r = 250;
				//b = 100;
				//detected = true;
			}
			if (cloudmask[4][i][j])
			{

				r += 100;

				detected = true;
			}

			if (cloudmask[5][i][j])
			{
				/*g += 100;
				detected = true;*/
			}

			if (cloudmask[7][i][j])
			{
				b += 100;
				detected = true;
			}

			if (cloudmask[10][i][j]) //wintertest
			{
				/*r = 0;
				g = 0;
				b = 0;
				detected = true;*/
			}

			if (detected)
			{
				image[i][j] = png::rgb_pixel(r, g, b);
			}
			else
			{
			  image[i][j] = png::rgb_pixel(0, 0, 0);
			}

		}
	}
	//sprintf(modifiedfilename, "%s%s", filename, "mask.png");
	image.write(filenamemask);
	
	return true;
}
