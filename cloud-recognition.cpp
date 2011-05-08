//============================================================================
// Name        : cloud-recognition.cpp
// Author      : ugnpaul
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================



#include <png++/png.hpp>

#include "hdf/mfhdf.h"
#include <vector>

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define SDSNAMEMAXSIZE 64
//#define MAX_VAR_DIMS 32
#define MODIS_DIR "modisdata/russia05.07.10/"

#define MODIS_TYPE1	"MOD021KM"
#define MODIS_TYPEH "MOD02HKM"
#define MODIS_TYPEQ	"MOD02QKM"

#define MODIS_NAME	".A2010186.0900.005.2010186193307"
#define MODIS_EXT	".hdf"

#define TEMP_PATH	"tmp/"
#define LOG			"log.txt"
#define X_LENGTH	1000
#define Y_LENGTH	1000

//for colored picture
/*#define REDBAND		3
#define GREENBAND	2
#define BLUEBAND	1
*/
#define INFRAREDBAND 14
#define ULTRAVIOLETBAND 0

#define WATER250THRESHOLD1 4000

#define MASKDEPTH 		20
#define INFRARED_THRESHOLD 430
#define ULTRAVIOLET_THRESHOLD 60000
#define HIGHT_CLOUD_THRESHOLD 630	//430
#define THRESHOLD_STABLE 1000

#define THRESHOLD250LOW 0.85
#define THRESHOLD250HIGH 1.05
#define QKM	true

using namespace std;

const float c1 = 2.742E-22;
const float c2 = 0.01438;
const float pi = 3.1415926;

const float wavelengths[36] = {	659.0,	865.0,	470.0,	555.0,	1240.0,	1640.0,
								2130.0,	415.0,	443.0,	490.0,	531.0,	565.0,
								653.0,	681.0,	750.0,	865.0,	905.0,	936.0,
								940.0,	1375.0,	3750.0,	3959.0,	4050.0,	4465.0,
								0,		0,		0,		0,		0,		0
								};

bool exists(char *pathtodir)
{
	struct stat st;
	if(stat(pathtodir,&st) == 0)
		return true;
	else
		return false;
}

bool savepicture(char *filename,  unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, bool ***cloudmask, int *height, int *width, ofstream &log)
{
	int widthq = width[0];
	int heightq = height[0];
	int widthh = width[1];
	int heighth = height[1];
	int width1 = width[2];
	int height1 = height[2];
	char *temppicname;
	char *temppath;
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

	for (i = 0; i < heightq; i++)
	{
		for (j = 0; j < widthq; j++)
		{
			if (cloudmask[0][i][j])
			{
				image[i][j] = png::rgb_pixel(255, 70, 70);
			}
		}
	}
	sprintf(filename, "%s%s", filename, "mask.png");
	image.write(filename);

	return true;
}

bool detect(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, int *height, int *width, ofstream &log, bool ***cloudmask)
{
	log << endl << "before heights" << endl;
	log.flush();
	int i, j;
	int heightq = height[0];
	int widthq = width[0];
	int heighth = height[1];
	int widthg = width[1];
	int height1 = height[2];
	int width1 = width[2];
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
	int ii = 0, jj = 0, ki = 0, kj = 0;

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
	return true;

}

float radiancetobrightness(int radiance, float wavelength)
{
	return (c2/(wavelength*log(c1/(radiance/10000.0*pi*wavelength*wavelength*wavelength*wavelength*wavelength) + 1)));
}

void getbrightness(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, int *height, int *width, float ****brightness1, float ****brightnessh, float ****brightnessq, ofstream log)
{
	int heightq = height[0];
	int widthq = width[0];
	int heighth = height[1];
	int widthg = width[1];
	int height1 = height[2];
	int width1 = width[2];

	int i, j, depth;
	for(depth = 0; depth < 15; depth++)
	{
		float wavelength = 100.0f; // _+ wavetable
		for (i = 0; i < widthq; i++)
		{
			for (j = 0; j < heightq; i++)
			{
				brightnessq[0][depth][i][j] = radiancetobrightness(radianceq[depth][i][j], wavelengths[6 + depth]); //getattr to know wavelength id
			}
		}
	}
}

bool readarray(int32 sds_id, unsigned short **dest, int32 *rank, int32 *dimsizes, int32 *datatype, int32 *numattr, ofstream &log, int depth)
{
	int32 *start, *edges, *stride;
	intn status;
	int height = 0, width = 0;
	int hpos = 0, wpos = 0;
	unsigned short temp[Y_LENGTH][X_LENGTH]; //parts
	start = (int32 *)calloc(sizeof(int32)*rank[0],1);
	edges = (int32 *)calloc(sizeof(int32)*rank[0],1);
	stride = (int32 *)calloc(sizeof(int32)*rank[0],1);
	height = dimsizes[rank[0] - 2];
	width = dimsizes[rank[0] -1];
	int i = 0, j = 0;
	for (i = 0; i < *rank; i++)
	{
		start[i] = depth;
		edges[i] = 1;
		stride[i] = 1;
	}
	while (wpos < width)
	{
		while (hpos < height)
		{

			for (i = 0; i < X_LENGTH; i++)
				for (j = 0; j < Y_LENGTH; j++)
					temp[j][i] = 0;
			start[*rank - 1] = wpos;
			start[*rank - 2] = hpos;
			edges[*rank - 1] = X_LENGTH;
			if ((edges[*rank - 1] + wpos) > width)
			{

				edges[*rank - 1] = width - wpos;
			}
			edges[*rank - 2] = Y_LENGTH;
			if ((edges[*rank - 2] + hpos) > height)
			{

				edges[*rank - 2] = height - hpos;
			}


			/*for (i = 0; i < *rank; i++)
			{
				log << "start" << i << ": " << start[i] << endl;
				log << "edges" << i << ": " << edges[i] << endl;
				log << "stride" << i << ": " << stride[i] << endl;
			}
			log.flush();
*/
			status = SDreaddata(sds_id, start, stride, edges, (VOIDP)temp);
			if (status == FAIL)
			{
				log << "FAIL" << endl;
				log.flush();
				log.close();
				return false;
			}
			else if (status == SUCCEED)
			{
				//log << "SUCCEED" << endl;
				//log.flush();
			}
			for (i = 0; i < edges[*rank - 1]; i++)
				for (j = 0; j < edges[*rank - 2]; j++)
				{
					dest[hpos + j][wpos + i] = temp[(i + j*edges[*rank-1])/X_LENGTH][(i + j*edges[*rank-1])%X_LENGTH];
				}
			hpos += Y_LENGTH;

			//log << "done\n";
			//log.flush();
		}
		wpos += X_LENGTH;
		hpos = 0;
	}
	return true;
}

bool readradiance(char *modisname, unsigned short ****radiance1, unsigned short ****radianceh, unsigned short ****radianceq, int *height, int *width, ofstream &log)
{
	int32 sd_id, sds_id, sds_index;
	intn status;
	int depth, heightq, widthq, heighth, widthh;
	int i, j, k;
	char modisfilepath[255] = "";
	char sdsname[SDSNAMEMAXSIZE] = "";
	int32 *rank = new int32();
	int32 *dimsizes = new int32[MAX_VAR_DIMS];
	int32 *datatype = new int32();
	int32 *numattr = new int32();

	status = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", MODIS_DIR, MODIS_TYPE1, modisname, MODIS_EXT);
	while (status == 0)
	{
		sd_id = SDstart (modisfilepath, DFACC_READ);
		sds_id = SDselect (sd_id, sds_index);
		status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
		if (status == 0)
		{
			log << endl << "=> reading id" << sds_index << "(status :" << status << "): '"<< sdsname <<"', rank "<< *rank <<", datatype " << *datatype << ";\ndimsizes:";
			for (i = 0; i < *rank; i++)
			{
				log << dimsizes[i] << ", ";
			}
			log << endl;
			//SDattrinfo(sds_id,  )
			if (strcmp(sdsname, "EV_1KM_RefSB") == 0)
			{
				height[2] = dimsizes[rank[0] - 2];
				width[2] = dimsizes[rank[0] -1];
				log << "height 2 = " << height[2] << endl;
				log.flush();
				*radiance1 = (unsigned short ***)malloc(sizeof(unsigned short **)*15);
				for (depth = 0; depth < 15; depth++)
				{
					radiance1[0][depth] = (unsigned short **)malloc(height[2]*sizeof(unsigned short *));
					for (i = 0; i < height[2]; i++)
					{
						radiance1[0][depth][i] = (unsigned short *)malloc(width[2]*sizeof(unsigned short));
					}
					readarray(sds_id, radiance1[0][depth], rank, dimsizes, datatype, numattr, log, depth);
				}
			}
		}
		sds_index++;
	}


	status = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", MODIS_DIR, MODIS_TYPEH, modisname, MODIS_EXT);
	while (status == 0)
		{
			sd_id = SDstart (modisfilepath, DFACC_READ);
			sds_id = SDselect (sd_id, sds_index);
			status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
			if (status == 0)
			{
				for (i = 0; i < *rank; i++)
				{
					log << dimsizes[i] << ", ";
				}
				log << endl;
				//SDattrinfo(sds_id,  )
				if (strcmp(sdsname, "EV_500_RefSB") == 0)
				{
					heighth = dimsizes[rank[0] - 2];
					widthh = dimsizes[rank[0] -1];
					width[1] = widthh;
					height[1] = heighth;
					*radianceh = (unsigned short ***)malloc(sizeof(unsigned short **)*5);
					for (depth = 0; depth < 5; depth++)
					{
						radianceh[0][depth] = (unsigned short **)malloc(heighth*sizeof(unsigned short *));
						for (i = 0; i < heighth; i++)
						{
							radianceh[0][depth][i] = (unsigned short *)malloc(widthh*sizeof(unsigned short));
						}
						readarray(sds_id, radianceh[0][depth], rank, dimsizes, datatype, numattr, log, depth);
					}
				}
			}
			sds_index++;
		}

	status = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", MODIS_DIR, MODIS_TYPEQ, modisname, MODIS_EXT);
	if (QKM)
	while (status == 0)
	{
		sd_id = SDstart (modisfilepath, DFACC_READ);
		sds_id = SDselect (sd_id, sds_index);
		status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
		if (status == 0)
		{
			for (i = 0; i < *rank; i++)
			{
				log << dimsizes[i] << ", ";
			}
			log << endl;
			//SDattrinfo(sds_id,  )
			if (strcmp(sdsname, "EV_250_RefSB") == 0)
			{
				heightq = dimsizes[rank[0] - 2];
				widthq = dimsizes[rank[0] -1];
				width[0] = widthq;
				height[0] = heightq;
				*radianceq = (unsigned short ***)malloc(sizeof(unsigned short **)*2);
				for (depth = 0; depth < 2; depth++)
				{
					radianceq[0][depth] = (unsigned short **)malloc(heightq*sizeof(unsigned short *));
					for (i = 0; i < heightq; i++)
					{
						radianceq[0][depth][i] = (unsigned short *)malloc(widthq*sizeof(unsigned short));
					}
					readarray(sds_id, radianceq[0][depth], rank, dimsizes, datatype, numattr, log, depth);
				}
			}
		}
		sds_index++;
	}

	status = SDendaccess (sds_id);
	status = SDend (sd_id);

	return true;
}





int main(int argc, char *argv[])
{
	unsigned short ***radiance1 = NULL;
	unsigned short ***radianceh = NULL;
	unsigned short ***radianceq = NULL;
	bool ***cloudmask;
	int *height;
	int *width;
	ofstream log;
	ifstream conf;
	char temppicname[255] = "";
	char colorpicname[255] = "";
	char temppath[255] = "";
	height = (int *)malloc(sizeof(int)*4);
	width = (int *)malloc(sizeof(int)*4);

	log.open(LOG);
	readradiance(MODIS_NAME, &radiance1, &radianceh, &radianceq, height, width, log);
	log << "heights:" << height[0] << ", " << height[1] << ", " << height[2];
	log << endl << "radiance1[00] = " << radiance1[0][0][0] << endl;
	log.flush();
	log << endl << "readradiance done" << endl;
	cloudmask = (bool ***)malloc(sizeof(bool **)*MASKDEPTH);
	for (int depth = 0; depth < MASKDEPTH; depth++)
	{
		cloudmask[depth] = (bool **)malloc(height[0]*sizeof(bool *));
		for (int i = 0; i < height[0]; i++)
		{
			cloudmask[depth][i] = (bool *)malloc(width[0]*sizeof(bool));
		}
	}

	detect(radiance1, radianceh, radianceq, height, width, log, cloudmask);
	sprintf(temppicname, "./img/MOD02%s.png", MODIS_NAME);

	savepicture(temppicname, radiance1, radianceh, radianceq, cloudmask, height, width, log);
	log.close();
	printf("done\n");
	return 0;
}
