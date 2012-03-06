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
#include "structures.h"


#define STARTH		2
#define START1		7
#define START2		22

using namespace std;

bool savepicture(char *filenamemap, char *filenamemask, arrays *a, ProgressViewer *progress, int progresspart);

#endif /* PIC_H_ */
