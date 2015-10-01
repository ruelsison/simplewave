//averages.c
#include "main_includes.h"
#include "averages.h"
#include "accelerometer.h"
#include "data_storage.h"
#include "coefficients.h"

float latestR;
float sv_currAve; //2second average
float sv_2minAve; //2minute average
float sv_hourAve; 
float sv_8hrAve; 
float sv_dayAve; 

float sv_2minPeak; //2minute peak

//20Hz Processing
#define FACTOR_CUR_AVE  	(1.0/NUMBER_OF_ENTRIES) //SMOOTHING_FACTOR_SV_DEFAULT
#define FACTOR_MIN_AVE  	(FACTOR_CUR_AVE/60)

// every 2 minutes
#define FACTOR_HOUR_AVE 	(1.0/30)
#define FACTOR_8HOUR_AVE 	(FACTOR_HOUR_AVE/8)
#define FACTOR_DAY_AVE 		(FACTOR_HOUR_AVE/24)

void updateAverages(void)
{
	Sensor_Reading *rdg = rawDataPeek();
	
	latestR = getR(rdg->val.x_ac,rdg->val.y_ac,rdg->val.z_ac);
	
	sv_currAve = FACTOR_CUR_AVE*latestR + (1.0-FACTOR_CUR_AVE)*sv_currAve;
	sv_2minAve = FACTOR_MIN_AVE*latestR + (1.0-FACTOR_MIN_AVE)*sv_2minAve;
	
	if ( (latestR > sv_2minPeak) ) // && (latestR < getPerso()->impact_threshold) )
	{
		sv_2minPeak = latestR;
	}
	
}

void updatePeakAverages(void)
{
	
	sv_hourAve = FACTOR_HOUR_AVE*sv_2minPeak + (1.0-FACTOR_HOUR_AVE)*sv_hourAve;
	sv_8hrAve = FACTOR_8HOUR_AVE*sv_2minPeak + (1.0-FACTOR_8HOUR_AVE)*sv_8hrAve;
	sv_dayAve = FACTOR_DAY_AVE*sv_2minPeak + (1.0-FACTOR_DAY_AVE)*sv_dayAve;
	
	sv_2minPeak = 0;
	
}

void initAverages(float val)
{
	sv_currAve = val;
	sv_2minAve = val;
	sv_hourAve = 1;
	sv_8hrAve = 1;
	sv_dayAve = 1;
	
	sv_2minPeak = 0;
}

float latestResultant(void)
{
	return latestR;
}

float averageResultant (void)
{
    return sv_currAve;
}

float getActivityLevel(void)
{
	return sv_2minPeak;
}

