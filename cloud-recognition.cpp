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
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define SDSNAMEMAXSIZE 64
//#define MAX_VAR_DIMS 32
#define MODIS_DIR "modisdata/"

#define MODIS_TYPE1	"MOD021KM"
#define MODIS_TYPEQ	"MOD02QKM"

#define MODIS_NAME	".A2005152.0750.005.2010153063845"
#define MODIS_EXT	".hdf"

#define TEMP_PATH	"tmp/"
#define LOG			"log.txt"
#define X_LENGTH	1000
#define Y_LENGTH	1000

//for colored picture
#define REDBAND		4
#define GREENBAND	3
#define BLUEBAND	2

#define INFRAREDBAND 14
#define ULTRAVIOLETBAND 0

#define DEPTH 		20
#define INFRARED_THRESHOLD 430
#define ULTRAVIOLET_THRESHOLD 8000
#define HIGHT_CLOUD_THRESHOLD 500	//430
#define THRESHOLD_STABLE 1000

#define THRESHOLD250LOW 0.75
#define THRESHOLD250HIGH 1.1

using namespace std;

bool exists(char *pathtodir)
{
	struct stat st;
	if(stat(pathtodir,&st) == 0)
		return true;
	else
		return false;
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


			for (i = 0; i < *rank; i++)
			{
				log << "start" << i << ": " << start[i] << endl;
				log << "edges" << i << ": " << edges[i] << endl;
				log << "stride" << i << ": " << stride[i] << endl;
			}
			log.flush();

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
				log << "SUCCEED" << endl;
				log.flush();
			}
			for (i = 0; i < edges[*rank - 1]; i++)
				for (j = 0; j < edges[*rank - 2]; j++)
				{
					dest[hpos + j][wpos + i] = temp[(i + j*edges[*rank-1])/X_LENGTH][(i + j*edges[*rank-1])%X_LENGTH];
				}
			hpos += Y_LENGTH;

			log << "done\n";
			log.flush();
		}
		wpos += X_LENGTH;
		hpos = 0;
	}
	return true;
}



int main(int argc, char *argv[])
{
	int32 sd_id, sds_id, sds_index;
	intn status;
	int height, width, depth;
	float data[Y_LENGTH][X_LENGTH];


	unsigned short ***colorpic;
	unsigned short **specialpic;

	char modisfilepath[255] = "";

	int i, j, k;
	ofstream log;
	ifstream conf;
	char temppicname[255] = "";
	char colorpicname[255] = "";
	char temppath[255] = "";


	char sdsname[SDSNAMEMAXSIZE] = "";
	int32 *rank = new int32();
	int32 *dimsizes = new int32[MAX_VAR_DIMS];
	int32 *datatype = new int32();
	int32 *numattr = new int32();

	log.open(LOG);

	sprintf(temppicname, "./png");
	png::image<png::rgb_pixel> image(1,1);
	status = 0;
	sds_index = 0;

	sprintf(modisfilepath, "%s%s%s%s", MODIS_DIR, MODIS_TYPE1, MODIS_NAME, MODIS_EXT);
	while (status == 0)
	{
		sd_id = SDstart (modisfilepath, DFACC_READ);
		sds_id = SDselect (sd_id, sds_index);

		status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
		if (status == 0)
		{
			log << "=> reading id" << sds_index << "(status :" << status << "): '"<< sdsname <<"', rank "<< *rank <<", datatype " << *datatype << ";\ndimsizes:";
			for (i = 0; i < *rank; i++)
			{
				log << dimsizes[i] << ", ";
			}
			log << endl;
			//SDattrinfo(sds_id,  )

			if (strcmp(sdsname, "EV_1KM_RefSB") == 0)
			{

				height = dimsizes[rank[0] - 2];
				width = dimsizes[rank[0] -1];
				colorpic = (unsigned short ***)malloc(sizeof(unsigned short **)*15);
				log << "allocating image array " << width << "x" << height << "... ";
				log.flush();
				image.resize(width, height);
				log << "done\n";
				log.flush();


				for (depth = 0; depth < 15; depth++)
				{

					colorpic[depth] = (unsigned short **)malloc(height*sizeof(unsigned short *));
					for (i = 0; i < height; i++)
					{
						colorpic[depth][i] = (unsigned short *)malloc(width*sizeof(unsigned short));
					}
					log << "done\n";
					log.flush();

					log << "reading data array... ";
					log.flush();


					readarray(sds_id, colorpic[depth], rank, dimsizes, datatype, numattr, log, depth);
					log << "done\n";
					log.flush();


					sprintf(temppath, "./img/%s", TEMP_PATH);
					if (!exists(temppath))
					{
						mkdir(temppath, 0777);
					}
					sprintf(temppicname, "%s%s:%d:%d.png",temppath, sdsname, sds_index, depth);
					//picture of one band
					for (i = 0; i < height; i++)
					{
						for (j = 0 ; j < width; j++)
						{
							image[i][j] = png::rgb_pixel((colorpic[depth][i][j]*255)/32768,(colorpic[depth][i][j]*255)/32768,(colorpic[depth][i][j]*255)/32768 );
						}

					}
					image.write(temppicname);
				}
			}
			else
			if (strcmp(sdsname, "EV_Band26") == 0)
			{

					height = dimsizes[rank[0] - 2];
					width = dimsizes[rank[0] -1];
					image.resize(width, height);
					specialpic = (unsigned short **)malloc(height*sizeof(unsigned short *));
					for (i = 0; i < height; i++)
					{
						specialpic[i] = (unsigned short *)malloc(width*sizeof(unsigned short));
					}

					readarray(sds_id, specialpic, rank, dimsizes, datatype, numattr, log, 0);

					image.resize(width, height);
					sprintf(temppath, "./img/%s", TEMP_PATH);
					if (!exists(temppath))
					{
						mkdir(temppath, 0777);
					}
					sprintf(temppicname, "%s%s:%d:%d.png",temppath, sdsname, sds_index, depth);
					for (i = 0; i < height; i++)
					{
						for (j = 0 ; j < width; j++)
						{
							image[i][j] = png::rgb_pixel((specialpic[i][j]*255)/32768,(specialpic[i][j]*255)/32768,(specialpic[i][j]*255)/32768 );
						}

					}
					image.write(temppicname);


					bool clouddetected = false;
					sprintf(colorpicname, "./img/combo:clouds.png");
					log << "\n begin detecting...";
					log << endl;
					log.flush();
					for (i = 0; i < height; i ++)
						for (j = 0; j < width; j++)
						{

							clouddetected = false;
								if (colorpic[INFRAREDBAND][i][j] > INFRARED_THRESHOLD)
								{
									clouddetected = true;
									image[i][j] = png::rgb_pixel(255,50,50);
								}

								if (colorpic[ULTRAVIOLETBAND][i][j] > ULTRAVIOLET_THRESHOLD)
								{
									clouddetected = true;
									image[i][j] = png::rgb_pixel(50, 0, 150);
								}

								if (specialpic[i][j] > HIGHT_CLOUD_THRESHOLD)
								{
									clouddetected = true;
									image[i][j] = png::rgb_pixel(100,100,255);
								}

								if (!clouddetected)
								{
									image[i][j] = png::rgb_pixel((colorpic[REDBAND][i][j]*255)/32768,(colorpic[GREENBAND][i][j]*255)/32768,(colorpic[BLUEBAND][i][j]*255)/32768);
								}
						}
					log<<" done";
					log.flush();
					image.write(colorpicname);
				}
				sprintf(colorpicname, "./img/%s:color.png", MODIS_TYPE1);
				for (i = 0; i < height; i ++)
					for (j = 0; j < width; j++)
					{
							image[i][j] = png::rgb_pixel((colorpic[REDBAND][i][j]*255)/32768,(colorpic[GREENBAND][i][j]*255)/32768,(colorpic[BLUEBAND][i][j]*255)/32768);
					}
				image.write(colorpicname);

		}
		sds_index++;
	}



		status = 0;
		sds_index = 0;

		sprintf(modisfilepath, "%s%s%s%s", MODIS_DIR, MODIS_TYPEQ, MODIS_NAME, MODIS_EXT);
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

					height = dimsizes[rank[0] - 2];
					width = dimsizes[rank[0] -1];
					colorpic = (unsigned short ***)malloc(sizeof(unsigned short **)*15);
					log << "allocating image array " << width << "x" << height << "... ";
					log.flush();
					image.resize(width, height);
					log << "done\n";
					log.flush();


					for (depth = 0; depth < 2; depth++)
					{

						colorpic[depth] = (unsigned short **)malloc(height*sizeof(unsigned short *));
						for (i = 0; i < height; i++)
						{
							colorpic[depth][i] = (unsigned short *)malloc(width*sizeof(unsigned short));
						}

						readarray(sds_id, colorpic[depth], rank, dimsizes, datatype, numattr, log, depth);


						sprintf(temppath, "./img/%s", TEMP_PATH);
						if (!exists(temppath))
						{
							mkdir(temppath, 0777);
						}
						sprintf(temppicname, "%s%s:%d:%d.png",temppath, sdsname, sds_index, depth);
						//picture of one band
						for (i = 0; i < height; i++)
						{
							for (j = 0 ; j < width; j++)
							{
								image[i][j] = png::rgb_pixel((colorpic[depth][i][j]*255)/32768,(colorpic[depth][i][j]*255)/32768,(colorpic[depth][i][j]*255)/32768 );
							}

						}
						image.write(temppicname);
					}
				}



			}
			sds_index++;
		}
		sprintf(colorpicname, "./img/%s:color.png", MODIS_TYPEQ);
		for (i = 0; i < height; i ++)
			for (j = 0; j < width; j++)
			{
				image[i][j] = png::rgb_pixel((colorpic[1][i][j]*255)/32768,((colorpic[0][i][j]*255)+2*(colorpic[1][i][j]*255))/(32768*3),(2*(colorpic[0][i][j]*255)+(colorpic[1][i][j]*255))/(32768*3));
			}
		image.write(colorpicname);
		sprintf(colorpicname, "./img/%s:cloud.png", MODIS_TYPEQ);
		for (i = 0; i < height; i ++)
			for (j = 0; j < width; j++)
			{
				if ((colorpic[1][i][j]/colorpic[0][i][j] < THRESHOLD250HIGH)&&(colorpic[1][i][j]/colorpic[0][i][j] > THRESHOLD250LOW))
				{
					image[i][j] = png::rgb_pixel(255, 0, 255);
				}
			}
		image.write(colorpicname);

	printf("done\n");
	status = SDendaccess (sds_id);
	status = SDend (sd_id);
	log.close();
	return 0;
}
