/*
 * pic.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#include "pic.h"

using namespace std;

bool savepicture(char *filenamemap, char *filenamemask,  arrays *a, ProgressViewer *progress, int progresspart)
{
	char modifiedfilename[255];
	bool detected = false;
	int i; int j;
        int progress_steps_number = 12;
        int progress_bit = progresspart/progress_steps_number;

	png::image<png::rgb_pixel> image(a->widthq,a->heightq);
	image.resize(a->widthq, a->heightq);

	int hi = 0;
	int hj = 0;
	int kmi = 0;
	int kmj = 0;
	int r, g, b;


        if (filenamemap)
        {
            for (i = 0; i < a->heightq; i++)
            {
                    hi = int(a->heighth/(double)a->heightq*i);

                    for (j = 0 ; j < a->widthq; j++)
                    {
                            hj = int(a->widthh/(double)a->widthq*j);


                            r = ((a->radianceq[0][i][j] - a->offsetsq[0])*a->scalesq[0]*255)/(150);
                            g = ((a->radianceh[1][hi][hj] - a->offsetsh[1])*a->scalesh[1]*255)/(150);
                            b = ((a->radianceh[0][hi][hj] - a->offsetsh[0])*a->scalesh[0]*255)/(150);


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
            progress->setValue(progress->Value() + progress_bit);
            for (i = 0; i < a->height1; i++)
            {
                    kmi = int(a->heightq/(double)a->height1*i);
                    for (j = 0; j < a->width1; j++)
                    {
                            if ((a->latitude[i][j] - int(a->latitude[i][j])) < 0.01)
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    image[kmi][kmj] = png::rgb_pixel(250, 0, 0);
                            }
                            if ((-a->latitude[i][j] + int(a->latitude[i][j])) > - 0.01)
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    image[kmi][kmj] = png::rgb_pixel(0, 0, 0);
                            }
                    }
            }
            progress->setValue(progress->Value() + progress_bit);
            for (i = 0; i < a->height1; i++)
            {
                    kmi = int(a->heightq/(double)a->height1*i);
                    for (j = 0; j < a->width1; j++)
                    {
                            if ((a->longitude[i][j] - int(a->longitude[i][j])) < 0.03)
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    image[kmi][kmj] = png::rgb_pixel(250, 0, 0);
                            }
                            if ((-a->longitude[i][j] + int(a->longitude[i][j])) > - 0.03)
                            {
                                    kmj = int(a->widthq/(double)a->width1*j);
                                    image[kmi][kmj] = png::rgb_pixel(0, 0, 0);
                            }
                    }
            }
            progress->setValue(progress->Value() + progress_bit);
            image.write(filenamemap);
        }
        progress->setValue(progress->Value() + progress_bit*3);

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
        if (filenamemask)
        {
            r = 55;
            g = 55;
            b = 55;
            int k = 0;
            int pixelmask[7];
            for (i = 0; i < a->heightq; i++)
            {
                    for (j = 0; j < a->widthq; j++)
                    {
                            for (k = 0; k < 7; k++)
                            {
                                pixelmask[k] = a->cloudmask[i][j] % 2;
                                a->cloudmask[i][j] /= 2;
                            }
                            r = 55;
                            g = 55;
                            b = 55;
                            detected = false;
                            if (pixelmask[0])
                            {
                                    r += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(255, 70, 70);
                            }
                            if (pixelmask[1])
                            {
                                    b += 50;
                                    detected = true;
                                    //image[i][j] = png::rgb_pixel(70, 70, 255);
                            }


                            if (pixelmask[3]) //nearby
                            {
                                    //g = 100;
                                    //r = 250;
                                    //b = 100;
                                    //detected = true;
                            }
                            if (pixelmask[4])
                            {

                                    r += 100;

                                    detected = true;
                            }
                            if (pixelmask[2])
                            {
                                    g += 50;
                                    detected = false;
                                    //image[i][j] = png::rgb_pixel(70, 255, 70);
                            }

                            if (pixelmask[5])
                            {
                                    /*g += 100;
                                    detected = true;*/
                            }

                            if (pixelmask[6])
                            {
                                    b += 100;
                                    detected = true;
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
            progress->setValue(progress->Value() + progress_bit);
            //sprintf(modifiedfilename, "%s%s", filename, "mask.png");
            image.write(filenamemask);
        }
	progress->setValue(progress->Value() + progress_bit*3);

	return true;
}
