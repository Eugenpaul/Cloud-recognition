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
#define MODIS_TYPEQ	"MOD021Q"

#define MODIS_NAME	".A2010213.0845.005.2010213163715"
#define MODIS_EXT	".hdf"

#define TEMP_PATH	"tmp/"
#define LOG			"log.txt"
#define X_LENGTH	1000
#define Y_LENGTH	1000
#define REDBAND		4
#define GREENBAND	3
#define BLUEBAND	2

#define DEPTH 		20
#define THRESHOLD_UNSTABLE 350	//430
#define THRESHOLD_STABLE 1000



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
	//unsigned short pic[Y_LENGTH][X_LENGTH];
	unsigned short **pic; //full picture
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
	png::image<png::rgb_pixel> tempimage(1,1);
	png::image<png::rgb_pixel> colorimage(1,1);
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
				log << "allocating colorimage array " << width << "x" << height << "... ";
				log.flush();
				colorimage.resize(width, height);
				log << "done\n";
				log.flush();
				for (depth = 0; depth < 15; depth++)
				{
					log << "allocating pic array " << width << "x" << height << "... ";
					log.flush();
					pic = (unsigned short **)malloc(height*sizeof(unsigned short *));
					for (i = 0; i < height; i++)
					{
						pic[i] = (unsigned short *)malloc(width*sizeof(unsigned short));
					}
					colorpic[depth] = (unsigned short **)malloc(height*sizeof(unsigned short *));
					for (i = 0; i < height; i++)
					{
						colorpic[depth][i] = (unsigned short *)malloc(width*sizeof(unsigned short));
					}
					log << "done\n";
					log.flush();

					log << "reading data array... ";
					log.flush();

					readarray(sds_id, pic, rank, dimsizes, datatype, numattr, log, depth);
					readarray(sds_id, colorpic[depth], rank, dimsizes, datatype, numattr, log, depth);
					log << "done\n";
					log.flush();

					log << "allocating image array " << width << "x" << height << "... ";
					log.flush();
					tempimage.resize(width, height);
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
							tempimage[i][j] = png::rgb_pixel((pic[i][j]*255)/32768,(pic[i][j]*255)/32768,(pic[i][j]*255)/32768 );

		/*
							if (pic[i][j] < THRESHOLD_UNSTABLE)
							{
								image[i][j] = png::rgb_pixel(255,0,0);
							}
							else
							if (pic[i][j] < THRESHOLD_STABLE)
							{
								image[i][j] = png::rgb_pixel(0,0, int((pic[i][j]*pic[i][j]/(65535.0*65535.0))*255.0));
							}
							else
							{
								image[i][j] = png::rgb_pixel(0,255,0);
							}
		*/
						}

					}
					tempimage.write(temppicname);
				}

				if (strcmp(sdsname, "EV_Band26") == 0)
				{

					height = dimsizes[rank[0] - 2];
					width = dimsizes[rank[0] -1];
					colorimage.resize(width, height);
					specialpic = (unsigned short **)malloc(height*sizeof(unsigned short *));
					for (i = 0; i < height; i++)
					{
						pic[i] = (unsigned short *)malloc(width*sizeof(unsigned short));
					}

					readarray(sds_id, specialpic, rank, dimsizes, datatype, numattr, log, 0);

					tempimage.resize(width, height);
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
							tempimage[i][j] = png::rgb_pixel((specialpic[i][j]*255)/32768,(specialpic[i][j]*255)/32768,(specialpic[i][j]*255)/32768 );
						}

					}
					tempimage.write(temppicname);


					colorimage.resize(width, height);

					sprintf(colorpicname, "./img/%s:clouds.png", sdsname);
					for (i = 0; i < height; i ++)
						for (j = 0; j < width; j++)
						{
							if (specialpic[i][j] < THRESHOLD_UNSTABLE)
							{
								colorimage[i][j] = png::rgb_pixel((colorpic[REDBAND][i][j]*255)/32768,(colorpic[GREENBAND][i][j]*255)/32768,(colorpic[BLUEBAND][i][j]*255)/32768);
							}
							else
								if (specialpic[i][j] < THRESHOLD_STABLE)
								{
									colorimage[i][j] = png::rgb_pixel(0,0, int((pic[i][j]*pic[i][j]/(65535.0*65535.0))*255.0));
								}
								else
								{
									colorimage[i][j] = png::rgb_pixel(0,255,0);
								}
						}
					colorimage.write(colorpicname);
				}


				/*
				for (int rr = 14; rr < 15; rr++)
					for (int gg = 0; gg < 15; gg++)
						for (int bb = 0; bb < 15; bb++)
						{

							sprintf(colorpicname, "./img/colortest/%s:%d:%d:%d:color.png", sdsname, rr, gg, bb);
							//color picture
							for (i = 0; i < height; i++)
							{
								for (j = 0 ; j < width; j++)
								{
									colorimage[i][j] = png::rgb_pixel((colorpic[rr][i][j]*255)/32768,(colorpic[gg][i][j]*255)/32768,(colorpic[bb][i][j]*255)/32768 );
								}
							}
							colorimage.write(colorpicname);
						}
						*/
			}
		}
		sds_index++;
	}

	printf("done\n");
	status = SDendaccess (sds_id);
	status = SDend (sd_id);
	log.close();
	return 0;
}
