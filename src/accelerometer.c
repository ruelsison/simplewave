// accelerometer.c

#include "main_includes.h"
#include "accelerometer.h"
#include "data_storage.h"
#include "averages.h"
#include <math.h>



bool write_and_verify(uint8_t register_address, uint8_t value)
{
	uint8_t readback;
	bool transfer_succeeded = true;
	
		transfer_succeeded &= mpu6050_register_write(register_address, value);
	
		nrf_delay_ms(50);
	
		mpu6050_register_read(register_address,&readback,1);
	
		transfer_succeeded = (readback == value);
			return transfer_succeeded;
	
	
}
	

bool accel_setup(void)
{
    bool transfer_succeeded = true;
    
//     //Sets sample rate to 8000/1+7 = 1000Hz
     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_SMPLRT_DIV, 0x07);
//     //Disable FSync, 256Hz DLPF
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_CONFIG, 0x00);
//     //Disable gyro self tests, scale of 500 degrees/s
	transfer_succeeded &= write_and_verify(MPU6050_RA_GYRO_CONFIG, 0x00);
	
    //Disable accel self tests, scale of +-2g, no DHPF
    transfer_succeeded &= write_and_verify(MPU6050_RA_ACCEL_CONFIG, SETTING_ACCEL_SCALE);
    //Freefall threshold of |100mg|
    transfer_succeeded &= write_and_verify(MPU6050_RA_FF_THR, 0x64);
    //Freefall duration limit of 100ms
    transfer_succeeded &= write_and_verify(MPU6050_RA_FF_DUR, 0x64);
    //Motion threshold of 0mg
    transfer_succeeded &= write_and_verify(MPU6050_RA_MOT_THR, 0xfa); //200mg
    //Motion duration of 0s
    transfer_succeeded &= write_and_verify(MPU6050_RA_MOT_DUR, 0xfa);//200ms
//    //Zero motion threshold
//    transfer_succeeded &= mpu6050_register_write(MPU6050_RA_ZRMOT_THR, 0x00);
//     //Zero motion duration threshold
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_ZRMOT_DUR, 0x00);
//     //Disable sensor output to FIFO buffer
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_FIFO_EN, 0x00);
//  
//     //AUX I2C setup
//     //Sets AUX I2C to single master control, plus other config
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_MST_CTRL, 0x00);
//     //Setup AUX I2C slaves
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV0_ADDR, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV0_REG, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV0_CTRL, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV1_ADDR, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV1_REG, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV1_CTRL, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV2_ADDR, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV2_REG, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV2_CTRL, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV3_ADDR, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV3_REG, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV3_CTRL, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV4_ADDR, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV4_REG, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV4_DO, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV4_CTRL, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV4_DI, 0x00);
//  
//     //Setup INT pin and AUX I2C pass through
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_INT_PIN_CFG, 0x00);
    //Enable freefall interrupt (0x80) and Motion Detector interrupt (0x40)
    transfer_succeeded &= write_and_verify(MPU6050_RA_INT_ENABLE, 0xC0);
 
//     //Slave out, dont care
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV0_DO, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV1_DO, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV2_DO, 0x00);
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_SLV3_DO, 0x00);
//     //More slave config
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
//     //Reset sensor signal paths
//     //transfer_succeeded &= mpu6050_register_write(MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
     //Motion detection control
     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_MOT_DETECT_CTRL, 0x10);
//     //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_USER_CTRL, 0x00);
//     
// 		//Sets clock source to gyro reference w/ PLL
//     //transfer_succeeded &= mpu6050_register_write(MPU6050_RA_PWR_MGMT_1, 0x02);
		
    //Clear 'sleep' bit to start sensor
    transfer_succeeded &= write_and_verify(MPU6050_RA_PWR_MGMT_1, 0x00);
//     //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_PWR_MGMT_2, 0x00);
//     //Data transfer to and from the FIFO buffer
//     transfer_succeeded &= mpu6050_register_write(MPU6050_RA_FIFO_R_W, 0x00);

    // Initialize moving average values
		
		int x,y,z;
		int x_prevAve, y_prevAve, z_prevAve;
		
		nrf_delay_ms(50);
		accel_get_values(&x,&y,&z);
		nrf_delay_ms(50);
		accel_get_values(&x,&y,&z);
		nrf_delay_ms(50);
		accel_get_values(&x,&y,&z);
		nrf_delay_ms(50);
		accel_get_values(&x,&y,&z);
		
		
    x_prevAve = CONVERT_TO_G(x);
    y_prevAve = CONVERT_TO_G(y);
    z_prevAve = CONVERT_TO_G(z);
    initAverages(sqrt(pow(x_prevAve,2)+pow(y_prevAve,2)+pow(z_prevAve,2)));
    
    return transfer_succeeded;
}

//Gets raw accelerometer data, performs no processing
void accel_get_values(int *xOut, int *yOut, int *zOut)
{
	unsigned char ar[6];
	
	mpu6050_register_read(MPU6050_RA_ACCEL_XOUT_H,ar,6);
	*xOut = (ar[0]*256 + ar[1]);// - 32767;           
	*yOut = (ar[2]*256 + ar[3]);//  - 32767;        
	*zOut = (ar[4]*256 + ar[5]);//	- 32767;
	
	
}	

//Gets all sensor data, performs no processing
void sensor_get_values(uint16_t *values)
{	

	unsigned char ar[14];
	// Data Format: AXH AXL AYH AYL AZH AZL TH TL GXH GXL GYH GYL GZH GZL
	mpu6050_register_read(MPU6050_RA_ACCEL_XOUT_H,ar,14);
	
	for (int i=0;i<7;i++) 
	{
		values[i] = (ar[2*i]*256 + ar[2*i+1]);
		
		
	}
	
	
}	
 
//Converts the already acquired accelerometer data into 3D euler angles
void accel_get_angles(float *pitch, float *roll)
{
	float x,y,z;
	//float yaw;
	
	//accel_get_values(&x, &y, &z);
	Sensor_Reading* rdg = rawDataPeek();
	x = CONVERT_TO_G(rdg->val.x_ac);
	y = CONVERT_TO_G(rdg->val.y_ac);
	z = CONVERT_TO_G(rdg->val.z_ac);
	
	*roll = 57.295*atan((float)y/sqrt(pow((float)z,2)+pow((float)x,2)));
	*pitch = 57.295*atan((float)-x/sqrt(pow((float)z,2)+pow((float)y,2)));
//		yaw = 57.295*acos((float)z);
}

float getR(int x,int y, int z)
{
	float xg = CONVERT_TO_G(x);
	float yg = CONVERT_TO_G(y);
	float zg = CONVERT_TO_G(z);
	float result = sqrt(xg*xg + yg*yg + zg*zg);
	return result;
}

int checkOrientation(void)
{ 
	int x,y,z;
	
	accel_get_values(&x, &y, &z);
	
	float xg = CONVERT_TO_G(x);
//  float yg = CONVERT_TO_G(y);
//	float zg = CONVERT_TO_G(z);
	
	if (fabsf(xg) >= 0.8) // && ( fabsf(yg) <= 0.37 || fabsf(zg) <= 0.37) )
	{ 
		return ORIENTATION_STANDING;
	}
	else
	{
		return ORIENTATION_NOT_STANDING;
	}
	
}
