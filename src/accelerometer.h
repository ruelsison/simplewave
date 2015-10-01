// accelerometer.h

#include "coefficients.h"

#define SCALE_PLUS_MINUS2 (16384.0)
#define SCALE_PLUS_MINUS4 (SCALE_PLUS_MINUS2/2) 
#define SCALE_PLUS_MINUS8 (SCALE_PLUS_MINUS2/4)
#define SCALE_PLUS_MINUS16 (SCALE_PLUS_MINUS2/8) 

#define ACCEL_SETTING_PLUS_MINUS2 0
#define ACCEL_SETTING_PLUS_MINUS4 0x08
#define ACCEL_SETTING_PLUS_MINUS8 0x10
#define ACCEL_SETTING_PLUS_MINUS16 0x18

#define GYRO_SCALE_PLUS_MINUS250 (0X7FFF / 250.0)
#define GYRO_SCALE_PLUS_MINUS500 (GYRO_SCALE_PLUS_MINUS250/2) 
#define GYRO_SCALE_PLUS_MINUS1000 (GYRO_SCALE_PLUS_MINUS250/4)
#define GYRO_SCALE_PLUS_MINUS2000 (GYRO_SCALE_PLUS_MINUS250/8) 

//should match with accelerometer config
#define SCALE SCALE_PLUS_MINUS16 
#define SETTING_ACCEL_SCALE (ACCEL_SETTING_PLUS_MINUS16)

#define CONVERT_TO_G(raw)           ((short)(raw)/SCALE)
#define ACCEL_SAMP_RATE             0.05

#define CONVERT_TO_DEG_PER_SEC(raw)           ((short)(raw)/GYRO_SCALE_PLUS_MINUS250)
	
#define ORIENTATION_STANDING (0x5a)
#define ORIENTATION_NOT_STANDING (0xa5)

void accel_get_values(int *xOut, int *yOut, int *zOut);
void accel_get_angles(float *x_angle, float *y_angle);
bool accel_setup(void);
float accel_get_sum_vector (int xRaw, int yRaw, int zRaw);
double accel_get_average_sv (void);
void sensor_get_values(uint16_t *values);
float getR(int x,int y, int z);
int checkOrientation(void);
