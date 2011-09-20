/*
 * readhdf.h
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#ifndef READHDF_H_
#define READHDF_H_

#include "hdf/mfhdf.h"
#include <stdint.h>
#include <fstream>


#define SDSNAMEMAXSIZE 64
#define SDSATTRNAMEMAXSIZE 64
#define MAX_VAR_DIMS 32
//#define MODIS_DIR "modisdata/russia05.07.10/"
#define MODIS_TYPE1	"MOD021KM"
#define MODIS_TYPEH 	"MOD02HKM"
#define MODIS_TYPEQ	"MOD02QKM"
#define MODIS_EXT	".hdf"
#define X_LENGTH	1000
#define Y_LENGTH	1000
#define STARTH		2
#define START1		7
#define START2		22

using namespace std;

bool readarray(int32 sds_id, unsigned short **dest, int32 *rank, int32 *dimsizes, int32 *datatype, int32 *numattr, ofstream &log, int depth);
bool readradiance(char *path, char *modisname, unsigned short ****radiance1, unsigned short ****radianceh, unsigned short ****radianceq,
					unsigned short ****radiance2, float32 **offsets, float32 **scales, int *height, int *width, ofstream &log);
bool readradiance(char *path1, char *pathh, char *pathq, unsigned short ****radiance1, unsigned short ****radianceh, unsigned short ****radianceq, 							unsigned short ****radiance2, float32 **offsets, float32 **scales, int *height, int *width, ofstream &log);

#endif /* READHDF_H_ */
