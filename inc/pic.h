/*
 * pic.h
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */



#ifndef PIC_H_
#define PIC_H_

#include <fstream>
#include <iostream>
#include <png++/png.hpp>
#include "hdf/mfhdf.h"


#define STARTH		2
#define START1		7
#define START2		22

using namespace std;

bool savepicture(char *filename,  unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, bool ***cloudmask, int *height, int *width,float32 *offsets, float32 *scales, ofstream &log);

#endif /* PIC_H_ */
