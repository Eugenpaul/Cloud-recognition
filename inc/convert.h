/*
 * convert.h
 *
 *  Created on: May 17, 2011
 *      Author: eugenpaul
 */

#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdlib.h>
#include <cmath>
#include <fstream>
#include "hdf/mfhdf.h"
#include "structures.h"

#define STARTH		2
#define START1		7
#define START2		22
#define BRIGHTNESS2NUMBER 4
#define BRIGHTNESS1NUMBER 1

const float c1 = 3.741775E-22;
const float c2 = 0.01438;
const float pi = 3.1415926;



const float wavelengths[37] = {	0, 659.0E-9,	865.0E-9,	470.0E-9,	555.0E-9,	1240.0E-9,	1640.0E-9,
				2130.0E-9,	415.0E-9,	443.0E-9,	490.0E-9,	531.0E-9,	565.0E-9,
				653.0E-9,	681.0E-9,	750.0E-9,	865.0E-9,	905.0E-9,	936.0E-9,
				940.0E-9,	3750.0E-9,	3959.0E-9,	3959.0E-9,	4050.0E-9,	4465.0E-9,
				4515.0E-9,	1375.0E-9,	6715.0E-9,	7325.0E-9,	8550.0E-9,	9730.0E-9,
				11030.0E-9,	12020.0E-9,	13335.0E-9,	13635.0E-9,	13935.0E-9,	14235.0E-9
				};
const int wt[38] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 14, 14, 15, 16, 17, 18, 19, 26, 20,21,22,23,24,25,27,28,29,30,31,32,33,34,35,36 };

const int brightness2essential[BRIGHTNESS2NUMBER] = {10, 11, 8, 1};
const int brightness1essential[BRIGHTNESS1NUMBER] = {14};


float radiancetobrightness(int radiance, float wavelength, float32 scale, float32 offset);
void getbrightness(arrays *a, ProgressViewer *progress, int progresspart);

#endif /* CONVERT_H_ */
