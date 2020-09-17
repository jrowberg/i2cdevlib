/*
I2Cdev library collection - MPU6050 RPi calibration program
Based on the example in Arduino/MPU6050/

==============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================

To compile on a Raspberry Pi (1 or 2 or 3)
  1. install the bcm2835 library, see http://www.airspayce.com/mikem/bcm2835/index.html
  2. enable i2c on your RPi , see https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
  3. connect your i2c devices
  4. then from bash
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o IMU_zero ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/examples/IMU_zero.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/ ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/MPU6050/MPU6050.cpp -l bcm2835 -l m
      $ sudo ./IMU_zero

If an MPU6050 
      * is an ideal member of its tribe, 
      * is properly warmed up, 
      * is at rest in a neutral position, 
      * is in a location where the pull of gravity is exactly 1g, and 
      * has been loaded with the best possible offsets, 
then it will report 0 for all accelerations and displacements, except for 
Z acceleration, for which it will report 16384 (that is, 2^14).  Your device 
probably won't do quite this well, but good offsets will all get the baseline 
outputs close to these target values.

  Put the MPU6050 on a flat and horizontal surface, and leave it operating for 
5-10 minutes so its temperature gets stabilized.

  Run this program.  A "----- done -----" line will indicate that it has done its best.
With the current accuracy-related constants (NFast = 1000, NSlow = 10000), it will take 
a few minutes to get there.

  Along the way, it will generate a dozen or so lines of output, showing that for each 
of the 6 desired offsets, it is 
      * first, trying to find two estimates, one too low and one too high, and
      * then, closing in until the bracket can't be made smaller.

  The line just above the "done" line will look something like
    [567,567] --> [-1,2]  [-2223,-2223] --> [0,1] [1131,1132] --> [16374,16404] [155,156] --> [-1,1]  [-25,-24] --> [0,3] [5,6] --> [0,4]
As will have been shown in interspersed header lines, the six groups making up this
line describe the optimum offsets for the X acceleration, Y acceleration, Z acceleration,
X gyro, Y gyro, and Z gyro, respectively.  In the sample shown just above, the trial showed
that +567 was the best offset for the X acceleration, -2223 was best for Y acceleration, 
and so on.

  The need for the delay between readings (usDelay) was brought to attention by Nikolaus Doppelhammer.
===============================================
*/


#include <stdio.h>
#include <cstdio>
#include <bcm2835.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <math.h>

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high


const char LBRACKET = '[';
const char RBRACKET = ']';
const char COMMA    = ',';
const char BLANK    = ' ';
const char PERIOD   = '.';

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;

const int usDelay = 3150;   // empirical, to hold sampling to 200 Hz
const int NFast =  1000;    // the bigger, the better (but slower)
const int NSlow = 10000;    // ..
const int LinesBetweenHeaders = 5;


int LowValue[6];
int HighValue[6];
int Smoothed[6];
int LowOffset[6];
int HighOffset[6];
int Target[6];
int LinesOut;
int N;


void ForceHeader()
{ 
	LinesOut = 99; 
}
    
void GetSmoothed()
{ 
	int16_t RawValue[6];
	int i;
	long Sums[6];
	for (i = iAx; i <= iGz; i++)
	{ 
		Sums[i] = 0; 
	}
	//    unsigned long Start = micros();

	for (i = 1; i <= N; i++)
	{ 
		// get sums
		accelgyro.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz], 
							&RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
		if ((i % 500) == 0)
		{
			printf("%c", PERIOD);
		}
		delayMicroseconds(usDelay);
		for (int j = iAx; j <= iGz; j++)
		{
			Sums[j] = Sums[j] + RawValue[j];
		}
	} // get sums
	//    unsigned long usForN = micros() - Start;
	//    printf(" reading at %d Hz\n", 1000000/((usForN+N/2)/N));
	for (i = iAx; i <= iGz; i++)
	{ 
		Smoothed[i] = (Sums[i] + N/2) / N ; 
	}
} // GetSmoothed

void Initialize()
{
	// initialize device
	printf("Initializing I2C devices...\n");
	I2Cdev::initialize();

	// verify connection
	printf("Testing device connections...\n");
	if(!accelgyro.testConnection()) 
	{
		printf("MPU6050 connection failed\n");
		return;
	}

	printf("MPU6050 connection successful\n"); 
	accelgyro.initialize();
	printf("Initialization done!\n");
} // Initialize

void SetAveraging(int NewN)
{ 
	N = NewN;
	printf("averaging %d  readings each time\n", N);
} // SetAveraging

void SetOffsets(int TheOffsets[6])
{ 
	accelgyro.setXAccelOffset(TheOffsets [iAx]);
	accelgyro.setYAccelOffset(TheOffsets [iAy]);
	accelgyro.setZAccelOffset(TheOffsets [iAz]);
	accelgyro.setXGyroOffset (TheOffsets [iGx]);
	accelgyro.setYGyroOffset (TheOffsets [iGy]);
	accelgyro.setZGyroOffset (TheOffsets [iGz]);
} // SetOffsets

void ShowProgress()
{ 
	if (LinesOut >= LinesBetweenHeaders)
	{ 
		// show header
		printf("\tXAccel\t\t\tYAccel\t\t\t\tZAccel\t\t\tXGyro\t\t\tYGyro\t\t\tZGyro\n");
		LinesOut = 0;
	} // show header
	printf("%c", BLANK);
	for (int i = iAx; i <= iGz; i++)
	{ 
		printf("%c%d%c%d] --> [%d%c%d", LBRACKET, LowOffset[i], COMMA, HighOffset[i],
						LowValue[i], COMMA, HighValue[i]);
		if (i == iGz)
		{ 
			printf("%c\n", RBRACKET); 
		}
		else
		{ 
			printf("]\t"); 
		}
	}
	LinesOut++;
} // ShowProgress

void PullBracketsIn()
{ 
	bool AllBracketsNarrow;
	bool StillWorking;
	int NewOffset[6];

	printf("\nclosing in:\n");
	AllBracketsNarrow = false;
	ForceHeader();
	StillWorking = true;
	while (StillWorking) 
	{ 
		StillWorking = false;
		if (AllBracketsNarrow && (N == NFast))
		{ 
			SetAveraging(NSlow); 
		}
		else
		{ 
			AllBracketsNarrow = true; 
		}// tentative
		for (int i = iAx; i <= iGz; i++)
		{ 
			if (HighOffset[i] <= (LowOffset[i]+1))
			{ 
			NewOffset[i] = LowOffset[i]; 
			}
			else
			{ 
			// binary search
				StillWorking = true;
				NewOffset[i] = (LowOffset[i] + HighOffset[i]) / 2;
				if (HighOffset[i] > (LowOffset[i] + 10))
				{ 
					AllBracketsNarrow = false; 
				}
			} // binary search
		}
		SetOffsets(NewOffset);
		GetSmoothed();
		for (int i = iAx; i <= iGz; i++)
		{ 
			// closing in
			if (Smoothed[i] > Target[i])
			{ 
				// use lower half
				HighOffset[i] = NewOffset[i];
				HighValue[i] = Smoothed[i];
			} // use lower half
			else
			{ 
				// use upper half
				LowOffset[i] = NewOffset[i];
				LowValue[i] = Smoothed[i];
			} // use upper half
		} // closing in
		ShowProgress();
	} // still working
} // PullBracketsIn

void PullBracketsOut()
{ 
	bool Done = false;
	int NextLowOffset[6];
	int NextHighOffset[6];

	printf("expanding:\n");
	ForceHeader();

	while (!Done)
	{
		Done = true;
		SetOffsets(LowOffset);
		GetSmoothed();
		for (int i = iAx; i <= iGz; i++)
		{
			// got low values
			LowValue[i] = Smoothed[i];
			if (LowValue[i] >= Target[i])
			{
				Done = false;
				NextLowOffset[i] = LowOffset[i] - 1000;
			}
			else
			{
				NextLowOffset[i] = LowOffset[i]; 
			}
		} // got low values

		SetOffsets(HighOffset);
		GetSmoothed();
		for (int i = iAx; i <= iGz; i++)
		{
			// got high values
			HighValue[i] = Smoothed[i];
			if (HighValue[i] <= Target[i])
			{
				Done = false;
				NextHighOffset[i] = HighOffset[i] + 1000;
			}
			else
			{ 
				NextHighOffset[i] = HighOffset[i]; 
			}
		} // got high values
		ShowProgress();
		for (int i = iAx; i <= iGz; i++)
		{ 
			LowOffset[i] = NextLowOffset[i];   // had to wait until ShowProgress done
			HighOffset[i] = NextHighOffset[i]; // ..
		}
	} // keep going
} // PullBracketsOut



int main(int argc, char **argv)
{ 
	Initialize();
	for (int i = iAx; i <= iGz; i++)
	{ // set targets and initial guesses
		Target[i] = 0; // must fix for ZAccel 
		HighOffset[i] = 0;
		LowOffset[i] = 0;
	} // set targets and initial guesses
	Target[iAz] = 16384;
	SetAveraging(NFast);
	PullBracketsOut();
	PullBracketsIn();

	printf("-------------- done --------------\n\n");
	return 0;
}
