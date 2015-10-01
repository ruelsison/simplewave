// fall_detection.c

#include "main_includes.h"
#include "accelerometer.h"
#include "coefficients.h"
#include <math.h>

int xWindow[2] = {0,0};
int yWindow[2] = {0,0};
int zWindow[2] = {0,0};
    
bool fd_check_zero_g (void)
{
    unsigned char ff_trigger;
    
    mpu6050_register_read(MPU6050_RA_INT_STATUS, &ff_trigger, 1);
    
    if ( (ff_trigger & 0x80) == 0x80 )
        return true;
    else
        return false;
}

bool fd_check_motion_interrupt (void)
{
    unsigned char mt_trigger;
    
    mpu6050_register_read(MPU6050_RA_INT_STATUS, &mt_trigger, 1);
    
    if ( (mt_trigger & 0x40) == 0x40 )
        return true;
    else
        return false;
}

uint8_t fd_check_for_impact (double sum_vector)
{
	uint8_t impact_trigger = 0;
//			if ( checkOrientation() == ORIENTATION_STANDING)
//			{
//				if (sum_vector >= getPerso()->impact_threshold)
//					impact_trigger = 0xFF;
//			}
//			else
//			{
//				if (sum_vector >= NOT_STANDING_IMPACT_HIGH_DEFAULT)
//					impact_trigger = 0xFF;
//			}

//    if (sum_vector >= IMPACT_HIGH) // || sum_vector < IMPACT_LOW)  
    if (sum_vector >= getPerso()->impact_threshold)
      impact_trigger = 0xFF;
			
   
    return impact_trigger;
}

void fd_check_for_velocity (int x, int y, int z)
{
	double Vx, Vy, Vz;
    char s[64];
    
    xWindow[1] = xWindow[0];
    yWindow[1] = yWindow[0];
    zWindow[1] = zWindow[0];
    
    xWindow[0] = CONVERT_TO_G(x);
    yWindow[0] = CONVERT_TO_G(y);
    zWindow[0] = CONVERT_TO_G(z);
    
    // integration of acceleration data to velocity
    Vx = 0.5 * ACCEL_SAMP_RATE * ((xWindow[0]-1)+(xWindow[1]-1));
    Vy = 0.5 * ACCEL_SAMP_RATE * ((yWindow[0]-1)+(yWindow[1]-1));
    Vz = 0.5 * ACCEL_SAMP_RATE * ((zWindow[0]-1)+(zWindow[1]-1));
    
    sprintf(s, "Vx = %.3f\tVy = %.3f\tVz = %.3f", Vx, Vy, Vz);
}

/*

bool fd_start_detection(void)
{
}

bool fd_confirm_fall(void) 
{
}

*/

