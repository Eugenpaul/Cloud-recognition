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

using namespace std;

bool savepicture(char *filename,  unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, bool ***cloudmask, int *height, int *width, ofstream &log);

#endif /* PIC_H_ */
