/*
 * readhdf.cpp
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */
#include "readhdf.h"

bool readarray(int32 sds_id, unsigned short **dest, int32 *rank, int32 *dimsizes, int32 *datatype, int32 *numattr, int depth)
{
  int32 *start, *edges, *stride;
  intn status;
  int height = 0, width = 0;
  start = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  edges = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  stride = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  height = dimsizes[rank[0] - 2];
  width = dimsizes[rank[0] - 1];
  unsigned short *temp = (unsigned short *)malloc(width*height*sizeof(unsigned short));
  int i = 0, j = 0;
  for (i = 0; i < *rank; i++)
  {
    start[i] = depth;
    edges[i] = 1;
    stride[i] = 1;
  }
  start[*rank - 1] = 0;
  start[*rank - 2] = 0;
  edges[*rank - 1] = width;
  edges[*rank - 2] = height;
  status = SDreaddata(sds_id, start, stride, edges, (VOIDP)temp);
  if (status == FAIL)
  {
    free(temp);
    return false;
  }
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      dest[i][j] = temp[i*width + j];
    }

  }
    free(temp);
  return true;
}

bool readarray32(int32 sds_id, float32 **dest, int32 *rank, int32 *dimsizes, int32 *datatype, int32 *numattr, int depth)
{
  int32 *start, *edges, *stride;
  intn status;
  int height = 0, width = 0;
  start = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  edges = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  stride = (int32 *)calloc(sizeof(int32)*rank[0], 1);
  height = dimsizes[rank[0] - 2];
  width = dimsizes[rank[0] - 1];
  float32 *temp = (float32 *)malloc(width*height*sizeof(float32));
  int i = 0, j = 0;
  for (i = 0; i < *rank; i++)
  {
    start[i] = depth;
    edges[i] = 1;
    stride[i] = 1;
  }
  start[*rank - 1] = 0;
  start[*rank - 2] = 0;
  edges[*rank - 1] = width;
  edges[*rank - 2] = height;
  status = SDreaddata(sds_id, start, stride, edges, (VOIDP)temp);
  if (status == FAIL)
  {
    free(temp);
    return false;
  }
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      dest[i][j] = temp[i*width + j];
    }

  }
    free(temp);
  return true;
}

bool complexreadarray(int32 sds_id, unsigned short **dest, int32 *rank, int32 *dimsizes, int32 *datatype, int32 *numattr, int depth)
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


      status = SDreaddata(sds_id, start, stride, edges, (VOIDP)temp);
      if (status == FAIL)
      {
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
	  //dest[hpos + i][wpos + j] = temp[(i + j*edges[*rank-1])/X_LENGTH][(i + j*edges[*rank-1])%X_LENGTH];

	  dest[hpos + i][wpos + j] = temp[i][j];
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
		  int *height, int *width)
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
      if (strcmp(sdsname, "EV_1KM_RefSB") == 0)
      {
	height[2] = dimsizes[rank[0] - 2];
	width[2] = dimsizes[rank[0] -1];
	*radiance1 = (unsigned short ***)malloc(sizeof(unsigned short **)*15);
	for (depth = 0; depth < 15; depth++)
	{
	  radiance1[0][depth] = (unsigned short **)malloc(height[2]*sizeof(unsigned short *));
	  for (i = 0; i < height[2]; i++)
	  {
	    radiance1[0][depth][i] = (unsigned short *)malloc(width[2]*sizeof(unsigned short));
	  }
	  readarray(sds_id, radiance1[0][depth], rank, dimsizes, datatype, numattr, depth);
	}

	//############# attr

	attr_index = 0;
	while (attrstatus == 0)
	{
	  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
	  if (strcmp(attrname, "radiance_offsets") == 0)
	  {
	    SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + START1));
	  }
	  else
	    if (strcmp(attrname, "radiance_scales") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(*scales + START1));
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

	  *radiance2 = (unsigned short ***)malloc(sizeof(unsigned short **)*16);

	  for (depth = 0; depth < 16; depth++)
	  {
	    radiance2[0][depth] = (unsigned short **)malloc(height[3]*sizeof(unsigned short *));
	    for (i = 0; i < height[3]; i++)
	    {
	      radiance2[0][depth][i] = (unsigned short *)malloc(width[3]*sizeof(unsigned short));
	    }
	    readarray(sds_id, radiance2[0][depth], rank, dimsizes, datatype, numattr, depth);
	  }

	  //############# attr

	  attr_index = 0;
	  attrstatus = 0;
	  while (attrstatus == 0)
	  {
	    attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );

	    if (strcmp(attrname, "radiance_offsets") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + START2));
	    }
	    else
	      if (strcmp(attrname, "radiance_scales") == 0)
	      {
		SDreadattr(sds_id, attr_index, (VOIDP)(*scales + START2));
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
		  readarray(sds_id, radianceh[0][depth], rank, dimsizes, datatype, numattr, depth);
		}

		//attr

		attr_index = 0;
		attrstatus = 0;
		while (attrstatus == 0)
		{
		  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
		  if (strcmp(attrname, "radiance_offsets") == 0)
		  {
		    SDreadattr(sds_id, attr_index, (VOIDP)(*offsets + STARTH));
		  }
		  else
		    if (strcmp(attrname, "radiance_scales") == 0)
		    {
		      SDreadattr(sds_id, attr_index, (VOIDP)(*scales + STARTH));
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
			  readarray(sds_id, radianceq[0][depth], rank, dimsizes, datatype, numattr, depth);
			}
			//attr
			attr_index = 0;
			attrstatus = 0;
			while (attrstatus == 0)
			{
			  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
			  if (strcmp(attrname, "radiance_offsets") == 0)
			  {
			    SDreadattr(sds_id, attr_index, (VOIDP)(*offsets));
			  }
			  else
			    if (strcmp(attrname, "radiance_scales") == 0)
			    {
			      SDreadattr(sds_id, attr_index, (VOIDP)(*scales));
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

bool readradiance(char *path1, char *pathh, char *pathq, arrays *a, ProgressViewer *progress, int progresspart)
{
  int32 sd_id, sds_id, sds_index, attr_index;
  intn status, attrstatus;

  int depth;
  int progress_steps_number = 8;
  int progress_bit = progresspart/progress_steps_number;
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

  //38
  a->offsets1 = (float32*)malloc(sizeof(float32)*15);
  a->scales1 = (float32*)malloc(sizeof(float32)*15);

  a->offsets2 = (float32*)malloc(sizeof(float32)*16);
  a->scales2 = (float32*)malloc(sizeof(float32)*16);

  a->offsetsh = (float32*)malloc(sizeof(float32)*5);
  a->scalesh = (float32*)malloc(sizeof(float32)*5);

  a->offsetsq = (float32*)malloc(sizeof(float32)*2);
  a->scalesq = (float32*)malloc(sizeof(float32)*2);

  status = 0;
  attrstatus = 0;
  sds_index = 0;

  while (status == 0)
  {
    sd_id = SDstart (path1, DFACC_READ);
    sds_id = SDselect (sd_id, sds_index);
    status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
    if (status == 0)
    {

      if (strcmp(sdsname, "EV_1KM_RefSB") == 0)
      {
        progress->setValue(progress->Value() + progress_bit);
	a->height1 = dimsizes[rank[0] - 2];
	a->width1 = dimsizes[rank[0] -1];
	a->radiance1 = (unsigned short ***)malloc(sizeof(unsigned short **)*15);
	for (depth = 0; depth < 15; depth++)
	{
	  a->radiance1[depth] = (unsigned short **)malloc(a->height1*sizeof(unsigned short *));
	  for (i = 0; i < a->height1; i++)
	  {
	    a->radiance1[depth][i] = (unsigned short *)malloc(a->width1*sizeof(unsigned short));
	  }
	  readarray(sds_id, a->radiance1[depth], rank, dimsizes, datatype, numattr, depth);
	}
	attr_index = 0;
	while (attrstatus == 0)
	{
	  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
	  if (strcmp(attrname, "radiance_offsets") == 0)
	  {
	    SDreadattr(sds_id, attr_index, (VOIDP)(a->offsets1));
	  }
	  else
	    if (strcmp(attrname, "radiance_scales") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(a->scales1));
	    }
	    attr_index++;
	}
      }
      else
	if (strcmp(sdsname, "EV_1KM_Emissive") == 0)
	{
          progress->setValue(progress->Value() + progress_bit);
	  a->height2 = dimsizes[rank[0] - 2];
	  a->width2 = dimsizes[rank[0] - 1];
	  a->radiance2 = (unsigned short ***)malloc(sizeof(unsigned short **)*16);

	  for (depth = 0; depth < 16; depth++)
	  {
	    a->radiance2[depth] = (unsigned short **)malloc(a->height2*sizeof(unsigned short *));
	    for (i = 0; i < a->height2; i++)
	    {
	      a->radiance2[depth][i] = (unsigned short *)malloc(a->width2*sizeof(unsigned short));
	    }
	    readarray(sds_id, a->radiance2[depth], rank, dimsizes, datatype, numattr, depth);
	  }

	  attr_index = 0;
	  attrstatus = 0;
	  while (attrstatus == 0)
	  {
	    attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
	    if (strcmp(attrname, "radiance_offsets") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(a->offsets2));
	    }
	    else
	      if (strcmp(attrname, "radiance_scales") == 0)
	      {
		SDreadattr(sds_id, attr_index, (VOIDP)(a->scales2));
	      }
	      attr_index++;
	  }
	}
    }
    sds_index++;
  }


  status = 0;
  sds_index = 0;
  while (status == 0)
  {
    sd_id = SDstart (pathh, DFACC_READ);
    sds_id = SDselect (sd_id, sds_index);
    status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
    if (status == 0)
    {
      if (strcmp(sdsname, "EV_500_RefSB") == 0)
      {
        progress->setValue(progress->Value() + progress_bit);
	a->heighth = dimsizes[rank[0] - 2];
	a->widthh = dimsizes[rank[0] -1];
	a->radianceh = (unsigned short ***)malloc(sizeof(unsigned short **)*5);
	for (depth = 0; depth < 5; depth++)
	{
	  a->radianceh[depth] = (unsigned short **)malloc(a->heighth*sizeof(unsigned short *));
	  for (i = 0; i < a->heighth; i++)
	  {
	    a->radianceh[depth][i] = (unsigned short *)malloc(a->widthh*sizeof(unsigned short));
	  }
	  readarray(sds_id, a->radianceh[depth], rank, dimsizes, datatype, numattr, depth);
	}

	attr_index = 0;
	attrstatus = 0;
	while (attrstatus == 0)
	{
	  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
	  if (strcmp(attrname, "radiance_offsets") == 0)
	  {
	    SDreadattr(sds_id, attr_index, (VOIDP)(a->offsetsh));

	  }
	  else
	    if (strcmp(attrname, "radiance_scales") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(a->scalesh));

	    }
	    attr_index++;
	}
      }
    }
    sds_index++;
  }

  status = 0;
  sds_index = 0;
  while (status == 0)
  {
    sd_id = SDstart (pathq, DFACC_READ);
    sds_id = SDselect (sd_id, sds_index);
    status = SDgetinfo(sds_id, sdsname, rank, dimsizes, datatype, numattr);
    if (status == 0)
    {
      if (strcmp(sdsname, "Latitude") == 0)
      {
        progress->setValue(progress->Value() + progress_bit);
	printf("latitude\n");
	a->height1 = dimsizes[rank[0] - 2];
	a->width1 = dimsizes[rank[0] - 1];
	printf("check a->height1 = %d\n", a->height1);
	printf("check a->width1 = %d\n", a->width1);
	a->latitude = (float32 **)malloc(a->height1*sizeof(float32 *));
	for (i = 0; i < a->height1; i++)
	{
	  a->latitude[i] = (float32 *)malloc(a->width1*sizeof(float32));
	}
	readarray32(sds_id, a->latitude, rank, dimsizes, datatype, numattr, depth);
      }
      else
      if (strcmp(sdsname, "Longitude") == 0)
      {
        progress->setValue(progress->Value() + progress_bit);
	printf("longitude\n");
	a->height1 = dimsizes[rank[0] - 2];
	a->width1 = dimsizes[rank[0] - 1];
	printf("check a->height1 = %d\n", a->height1);
	printf("check a->width1 = %d\n", a->width1);
	a->longitude = (float32 **)malloc(a->height1*sizeof(float32 *));
	for (i = 0; i < a->height1; i++)
	{
	  a->longitude[i] = (float32 *)malloc(a->width1*sizeof(float32));
	}
	readarray32(sds_id, a->longitude, rank, dimsizes, datatype, numattr, depth);

      }
      else
      if (strcmp(sdsname, "EV_250_RefSB") == 0)
      {
        progress->setValue(progress->Value() + progress_bit);
	a->heightq = dimsizes[rank[0] - 2];
	a->widthq = dimsizes[rank[0] -1];
	a->radianceq = (unsigned short ***)malloc(sizeof(unsigned short **)*2);
	for (depth = 0; depth < 2; depth++)
	{
	  a->radianceq[depth] = (unsigned short **)malloc(a->heightq*sizeof(unsigned short *));
	  for (i = 0; i < a->heightq; i++)
	  {
	    a->radianceq[depth][i] = (unsigned short *)malloc(a->widthq*sizeof(unsigned short));
	  }
	  readarray(sds_id, a->radianceq[depth], rank, dimsizes, datatype, numattr, depth);
	}
	attr_index = 0;
	attrstatus = 0;
	while (attrstatus == 0)
	{
	  attrstatus = SDattrinfo(sds_id, attr_index, attrname, nt, count );
	  if (strcmp(attrname, "radiance_offsets") == 0)
	  {
	    SDreadattr(sds_id, attr_index, (VOIDP)(a->offsetsq));
	  }
	  else
	    if (strcmp(attrname, "radiance_scales") == 0)
	    {
	      SDreadattr(sds_id, attr_index, (VOIDP)(a->scalesq));
	    }
	    attr_index++;
	}
      }

    }
    sds_index++;
  }

  status = SDendaccess (sds_id);
  status = SDend (sd_id);
  progress->setValue(progress->Value() + progress_bit);
  return true;
}

bool createnames(char *input, char ***output, int *number)
{

  const char *MODISNAMES[] = {"MOD021KM", "MOD02HKM", "MOD02QKM"};
  int i = 0;
  int n = *number;
  *output = (char **)malloc(sizeof(char *)*n);
  for (i = 0; i < n; i++)
  {
    output[0][i] = (char *)calloc(1, MAXNAMESIZE);
  }
  for (i = 0; i < n; i++)
  {
    sprintf(output[0][i], "%s%s%s", MODISNAMES[i], input, MODIS_EXT);
  }

  return true;
}

bool createpaths(char *path, char **names, char ***output, int *number)
{
  int i = 0;
  int n = *number;
  *output = (char **)malloc(sizeof(char *)*n);
  for (i = 0; i < n; i++)
  {
    output[0][i] = (char *)calloc(1, MAXNAMESIZE);
  }
  for (i = 0; i < n; i++)
  {
    sprintf(output[0][i], "%s%s", path, names[i]);
  }

  return true;
}