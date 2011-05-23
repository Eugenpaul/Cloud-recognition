//============================================================================
// Name        : cloud-recognition.cpp
// Author      : ugnpaul
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================



#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <fstream>
#include <string>

#include "readhdf.h"
#include "pic.h"
#include "detection.h"
#include "convert.h"


#define MODIS_NAME	".A2010186.0900.005.2010186193307"
//".A2010186.0720.005.2010186135820"
#define LOG			"log.txt"


#define MASKDEPTH 		20



using namespace std;



bool exists(char *pathtodir)
{
	struct stat st;
	if(stat(pathtodir,&st) == 0)
		return true;
	else
		return false;
}





int main(int argc, char *argv[])
{
	unsigned short ***radiance1 = NULL;
	unsigned short ***radianceh = NULL;
	unsigned short ***radianceq = NULL;
	unsigned short ***radiance2 = NULL;

	float ***brightness1 = NULL;
	float ***brightnessh = NULL;
	float ***brightnessq = NULL;
	float ***brightness2 = NULL;

	float32 *offsets;
	float32 *scales;

	bool ***cloudmask;
	int *height;
	int *width;
	ofstream log;
	ifstream conf;
	char temppicname[255] = "";

	height = (int *)malloc(sizeof(int)*5);
	width = (int *)malloc(sizeof(int)*5);

	log.open(LOG);
	readradiance(MODIS_NAME, &radiance1, &radianceh, &radianceq, &radiance2, &offsets, &scales, height, width, log);
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
	getbrightness(radiance1, radianceh, radianceq, radiance2, height, width, &brightness1, &brightnessh, &brightnessq, &brightness2, scales, offsets, log);
	log << endl <<"getbrightness done" << endl;
	log.flush();
	detect(radiance1, radianceh, radianceq, radiance2, brightness1, brightnessh, brightnessq, brightness2, height, width, log, cloudmask);
	sprintf(temppicname, "./img/MOD02%s.png", MODIS_NAME);

	savepicture(temppicname, radiance1, radianceh, radianceq, cloudmask, height, width, log);
	log.close();
	printf("done\n");
	return 0;
}
