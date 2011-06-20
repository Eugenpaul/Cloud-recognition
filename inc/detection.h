/*
 * detection.h
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#ifndef DETECTION_H_
#define DETECTION_H_

#include <fstream>
#include <stdlib.h>
#include "hdf/mfhdf.h"

//#define INFRARED_THRESHOLD 430
//#define ULTRAVIOLET_THRESHOLD 60000
//#define HIGHT_CLOUD_THRESHOLD 630	//430
//#define THRESHOLD_STABLE 1000

#define INFRAREDBAND 14
#define ULTRAVIOLETBAND 0



#define STARTH		2
#define START1		7
#define START2		22

#define T8 (30-22)
#define T11 (32-22)
#define T12 (33-22)
#define T26 14
#define T37 (21-7)
#define T39 (23-22)
#define T67 (28-22)

#define THRESHOLD250LOW 0.85
#define THRESHOLD250HIGH 1.05
#define WATER250THRESHOLD1 10000
#define CIRRUSTHRESHOLD 0.35

#define THRESHOLDT11 277




using namespace std;



bool detect(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, unsigned short ***radiance2,
			float ***brightness1, float ***brightnessh, float ***brightnessq, float ***brightness2,
			int *height, int *width, float32 *scales, float32 *offsets, ofstream &log, bool ***cloudmask);

#endif /* DETECTION_H_ */
