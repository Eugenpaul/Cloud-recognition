/*
 * detection.h
 *
 *  Created on: May 15, 2011
 *      Author: eugenpaul
 */

#ifndef DETECTION_H_
#define DETECTION_H_

#include <fstream>

#define INFRARED_THRESHOLD 430
#define ULTRAVIOLET_THRESHOLD 60000
#define HIGHT_CLOUD_THRESHOLD 630	//430
#define THRESHOLD_STABLE 1000

#define INFRAREDBAND 14
#define ULTRAVIOLETBAND 0

#define WATER250THRESHOLD1 4000


#define T11 (32-22)
#define T12 (33-22)

#define THRESHOLD250LOW 0.75
#define THRESHOLD250HIGH 1.1
//1.05
#define THRESHOLDT11 273


using namespace std;

bool detect(unsigned short ***radiance1, unsigned short ***radianceh, unsigned short ***radianceq, unsigned short ***radiance2,
			float ***brightness1, float ***brightnessh, float ***brightnessq, float ***brightness2,
			int *height, int *width, ofstream &log, bool ***cloudmask);

#endif /* DETECTION_H_ */
