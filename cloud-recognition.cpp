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
#define CONFIG		"config.txt"


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

bool readconfig(ifstream &conf, char **path, char ***modisnames, int *number)
{
	char temp[255];
	*number = 0;
	*path = (char *)malloc(255*sizeof(char));
	int namescount = 10;

	*modisnames = (char **)malloc(namescount*sizeof(char*));

	for (int i = 0; i < namescount; i++)
	{
		modisnames[0][i] = (char *)malloc(255*sizeof(char));
	}

	conf >> temp;
	while (strcmp(temp, "") != 0)
	{
		if (strcmp(temp, "path")==0)
		{
			conf >> *path;
		}
		if (strcmp(temp, "name")==0)
		{
			strcpy(temp, "");
			conf >> temp;
			sprintf(modisnames[0][*number], "%s%s", ".", temp);
			//modisnames[0][*number][0] = '.';
			printf("%s\n", modisnames[0][*number]);
			(*number)++;
		}
		strcpy(temp, "");
		conf >> temp;
	}
}

int main(int argc, char *argv[])
{
	char *path;// = "modisdata/russia05.07.10/";
	char **modisnames;
	int number;

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


	log.open(LOG);
	conf.open(CONFIG);
	readconfig(conf, &path, &modisnames, &number);
	height = (int *)malloc(sizeof(int)*5);
	width = (int *)malloc(sizeof(int)*5);
	cloudmask = (bool ***)malloc(sizeof(bool **)*MASKDEPTH);

	for (int k = 0; k < number ; k++)
	{

		printf("%s\n",modisnames[k]);
		readradiance(path, modisnames[k], &radiance1, &radianceh, &radianceq, &radiance2, &offsets, &scales, height, width, log);
		log << "heights:" << height[0] << ", " << height[1] << ", " << height[2];
		log << "width" << width[0] << ", " << width[1] << ", " << width[2];
		log << endl << "radiance1[00] = " << radiance1[0][0][0] << endl;
		log.flush();
		log << endl << "readradiance done" << endl;
		log.flush();

		for (int depth = 0; depth < MASKDEPTH; depth++)
		{
			log << "one" << endl;
			log.flush();

			cloudmask[depth] = (bool **)malloc(height[0]*sizeof(bool *));
			for (int i = 0; i < height[0]; i++)
			{
				cloudmask[depth][i] = (bool *)malloc(width[0]*sizeof(bool));
			}
		}
		log << "allocating cloudmask done" << endl;
		log.flush();
		getbrightness(radiance1, radianceh, radianceq, radiance2, height, width, &brightness1, &brightnessh, &brightnessq, &brightness2, scales, offsets, log);
		log << "getbrightness done" << endl;
		log.flush();
		detect(radiance1, radianceh, radianceq, radiance2, brightness1, brightnessh, brightnessq, brightness2, height, width, scales, offsets, log, cloudmask);
		sprintf(temppicname, "./img/MOD02%s.png", modisnames[k]);

		savepicture(temppicname, radiance1, radianceh, radianceq, cloudmask, height, width, offsets, scales, log);
	}
	log.close();
	printf("done\n");
	return 0;
}
