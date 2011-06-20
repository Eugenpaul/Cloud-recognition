/*
 * readhdf.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */
#include "readhdf.h"

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

bool readradiance(char *path, char *modisname, unsigned short ****radiance1, unsigned short ****radianceh, unsigned short ****radianceq, unsigned short ****radiance2,
					float32 **offsets, float32 **scales,
					int *height, int *width, ofstream &log)
{
	int32 sd_id, sds_id, sds_index, attr_index;
	intn status, attrstatus;
	int depth, heightq, widthq, heighth, widthh;
	int i;
	char modisfilepath[255] = "";
	char sdsname[SDSNAMEMAXSIZE] = "";
	char attrname[SDSATTRNAMEMAXSIZE] = "";
	int32 *rank = new int32();
	int32 *dimsizes = new int32[MAX_VAR_DIMS];
	int32 *datatype = new int32();
	int32 *numattr = new int32();

	int32 *nt = new int32();
	int32 *count = new int32();

	*offsets = (float32*)malloc(sizeof(float32)*38);
	*scales = (float32*)malloc(sizeof(float32)*38);

	status = 0;
	attrstatus = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", path, MODIS_TYPE1, modisname, MODIS_EXT);
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



			if (strcmp(sdsname, "EV_1KM_RefSB") == 0)
			{
				height[2] = dimsizes[rank[0] - 2];
				width[2] = dimsizes[rank[0] -1];
				log << "ref1 array " << height[2] << "x" << width[2] << endl;
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

				//############# attr

				attr_index = 0;
				while (attrstatus == 0)
				{
					attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
					log << attrname << endl;
					if (strcmp(attrname, "radiance_offsets") == 0)
					{
						SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + START1));
						log << "1KM radiance_offsets: ";
						for (i = 0; i < *count; i++)
						{
							log << offsets[0][START1+i] << ", ";
						}
						log << endl;
					}
					else
						if (strcmp(attrname, "radiance_scales") == 0)
						{
							SDreadattr(sds_id, attr_index, (VOIDP)(*scales + START1));
							log << "1KM radiance_scales: ";
							for (i = 0; i < *count; i++)
							{
								log << scales[0][START1+i] << ", ";
							}
							log << endl;
						}
					attr_index++;
				}

				//############ attr end

			}
			else
				if (strcmp(sdsname, "EV_1KM_Emissive") == 0)
				{
					height[3] = dimsizes[rank[0] - 2];
					width[3] = dimsizes[rank[0] - 1];

					log << "emissive array " << height[3] << "x" << width[3] << endl;
					*radiance2 = (unsigned short ***)malloc(sizeof(unsigned short **)*16);
					printf("width %d, height%d\n", width[3], height[3]);
					for (depth = 0; depth < 16; depth++)
					{
						radiance2[0][depth] = (unsigned short **)malloc(height[3]*sizeof(unsigned short *));
						for (i = 0; i < height[3]; i++)
						{
							radiance2[0][depth][i] = (unsigned short *)malloc(width[3]*sizeof(unsigned short));
						}
						readarray(sds_id, radiance2[0][depth], rank, dimsizes, datatype, numattr, log, depth);
					}

					//############# attr

					attr_index = 0;
					attrstatus = 0;
					while (attrstatus == 0)
					{
						attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
						log << attrname << endl;
						if (strcmp(attrname, "radiance_offsets") == 0)
						{
							SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + START2));
							log << "emissve 1KM radiance_offsets: ";
							for (i = 0; i < *count; i++)
							{
								log << offsets[0][START2+i] << ", ";
							}
							log << endl;
						}
						else
							if (strcmp(attrname, "radiance_scales") == 0)
							{
								SDreadattr(sds_id, attr_index, (VOIDP)(*scales + START2));
								log << "emissive 1KM radiance_scales: ";
								for (i = 0; i < *count; i++)
								{
									log << scales[0][START2+i] << ", ";
								}
								log << endl;
							}
						attr_index++;
					}

					//############ attr end

				}
		}
		sds_index++;
	}


	status = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", path, MODIS_TYPEH, modisname, MODIS_EXT);
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
					log << "refh array " << height[1] << "x" << width[1] << endl;
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

					//attr

					attr_index = 0;
					attrstatus = 0;
					while (attrstatus == 0)
					{
						attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
						log << attrname << endl;
						if (strcmp(attrname, "radiance_offsets") == 0)
						{
							SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + STARTH));
							log << ".5KM radiance_offsets: ";
							for (i = 0; i < *count; i++)
							{
								log << offsets[0][STARTH+i] << ", ";
							}
							log << endl;
						}
						else
							if (strcmp(attrname, "radiance_scales") == 0)
							{
								SDreadattr(sds_id, attr_index, (VOIDP)(*scales + STARTH));
								log << ".5KM radiance_scales: ";
								for (i = 0; i < *count; i++)
								{
									log << scales[0][STARTH+i] << ", ";
								}
								log << endl;
							}
							attr_index++;
					}

					//attr
				}
			}
			sds_index++;
		}

	status = 0;
	sds_index = 0;
	sprintf(modisfilepath, "%s%s%s%s", path, MODIS_TYPEQ, modisname, MODIS_EXT);
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
				log << "refq array " << height[0] << "x" << width[0] << endl;
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
				//attr
				attr_index = 0;
				attrstatus = 0;
				while (attrstatus == 0)
				{
					attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
					log << attrname << endl;
					if (strcmp(attrname, "radiance_offsets") == 0)
					{
						SDreadattr(sds_id, attr_index, (VOIDP)(*offsets));
						log << "QKM radiance_offsets: ";
						for (i = 0; i < *count; i++)
						{
							log << offsets[0][i] << ", ";
						}
						log << endl;
					}
					else
						if (strcmp(attrname, "radiance_scales") == 0)
						{
							SDreadattr(sds_id, attr_index, (VOIDP)(*scales));
							log << "QKM radiance_scales: ";
							for (i = 0; i < *count; i++)
							{
								log << scales[0][START2+i] << ", ";
							}
							log << endl;
						}
						attr_index++;
					}
				//attr
			}


		}
		sds_index++;
	}

	status = SDendaccess (sds_id);
	status = SDend (sd_id);

	return true;
}
