// step_counter_new.c

#include "main_includes.h"
#include <math.h>

#define SAMPLING_RATE 20	// sampling rate
#define PRECISION 1000/8		// Precision to be adjusted, lower = more sensitive

int i;

int xWind[4] = {0, 0, 0, 0};
int yWind[4] = {0, 0, 0, 0};
int zWind[4] = {0, 0, 0, 0};
						
int xMax = 0;
int xMin = 65535;
int yMax = 0;
int yMin = 65535;
int zMax = 0;
int zMin = 65535;
						
int nSamples = 0;
							 
int xThresh = 0;
int yThresh = 0;
int zThresh = 0;
							
int xSample_new = 0;
int ySample_new = 0;
int zSample_new = 0;
							
int xSample_old = 0;
int ySample_old = 0;
int zSample_old = 0;
							
int xSample_result = 0;
int ySample_result = 0;
int zSample_result = 0;
				
int xChange = 0;
int yChange = 0;
int zChange = 0;
				
int fThresh = 0;
int fSample_old = 0;
int fSample_new = 0;
				
int timeWindow = 0;

int detect_step (int xInput, int yInput, int zInput)
{
	// shift array for mean filter of size 4
	for (i = 3; i > 0; i--)
	{
		xWind[i] = xWind[i-1];
		yWind[i] = yWind[i-1];
		zWind[i] = zWind[i-1];
	}
	xWind[0] = xInput/4; // weighted mean implementation
	yWind[0] = yInput/4;
	zWind[0] = zInput/4;
								
	xSample_result = xWind[0] + xWind[1] + xWind[2] + xWind[3];
	ySample_result = yWind[0] + yWind[1] + yWind[2] + yWind[3];
	zSample_result = zWind[0] + zWind[1] + zWind[2] + zWind[3];
								
	// max and min update
	if (xSample_result > xMax) xMax = xSample_result;
	if (ySample_result > yMax) yMax = ySample_result;
	if (zSample_result > zMax) zMax = zSample_result;
								
	if (xSample_result < xMin) xMin = xSample_result;
	if (ySample_result < yMin) yMin = ySample_result;
	if (zSample_result < zMin) zMin = zSample_result;
								
	nSamples = nSamples + 1;	//sample count out of sampling rate
						
	if (nSamples >= SAMPLING_RATE)
	{
		// update threshold and reset maxima and minima
		nSamples  = 0;
						
		xThresh = (xMin + xMax)/2;
		yThresh = (yMin + yMax)/2;
		zThresh = (zMin + zMax)/2;
									
		xMax = 0;
		xMin = 65535;
		yMax = 0;
		yMin = 65535;
		zMax = 0;
		zMin = 65535;
									
	}
					
	xSample_old = xSample_new;
	xChange = abs(xSample_result - xSample_new);
	if (xChange > PRECISION) xSample_new = xSample_result;
					
	ySample_old = ySample_new;
	yChange = abs(ySample_result - ySample_new);
	if (yChange > PRECISION) ySample_new = ySample_result;
								
	zSample_old = zSample_new;
	zChange = abs(zSample_result - zSample_new);
	if (zChange > PRECISION) zSample_new = zSample_result;			
								
	if(xChange >= yChange && xChange >= zChange)
	{
		fThresh = xThresh;
		fSample_old = xSample_old;
		fSample_new = xSample_new;
	} else if(yChange >= xChange && yChange >= zChange)
	{
		fThresh = yThresh;
		fSample_old = ySample_old;
		fSample_new = ySample_new;
	} else if(zChange >= xChange && zChange >= yChange)
	{
		fThresh = zThresh;
		fSample_old = zSample_old;
		fSample_new = zSample_new;
	}
        
	if (fSample_old > fThresh && fSample_new < fThresh && timeWindow >= 7) 
	{
		timeWindow = 0;
		return 1;
	} else {
		timeWindow++;
		return 0;
	}
}
