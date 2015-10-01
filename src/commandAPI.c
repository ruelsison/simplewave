//CommandAPI.c

#include "main_includes.h"
#include "step_counter.h"
#include "timestamp.h"
#include "beep.h"
#include "accelerometer.h"
#include "data_storage.h"
#include "LEDS.H"
#include "memory_map.h"
#include "timestamp.h"
#include <math.h>

#include <stdlib.h>


extern ble_nus_t    m_nus;
extern uint8_t Dumper;
extern uint8_t SetTimestamp;
extern uint8_t ReadDataBuffer;
extern uint8_t ClearDataBuffer;
extern uint8_t ReadSnapshots;
extern uint8_t ClearSnapshots;
extern uint8_t clearPerso;
extern uint8_t ReadBigSnapshot;
extern uint8_t sendRawData;
extern uint8_t rewritePerso;
extern uint8_t sendOrientation;

extern uint8_t ReadDataBufferPartial;
extern uint8_t PartialCount1;
extern uint8_t PartialCount2;

extern uint8_t ReadSnapshotsPartial;
extern uint8_t PartialSnap1;
extern uint8_t PartialSnap2;

uint8_t usernameFlag = 0;
uint8_t FreeFallFlag = 0;
uint8_t impactFlag = 0;
uint8_t xAngleFlag = 0;
uint8_t yAngleFlag = 0;


uint8_t writingPerso = 0;

/*
extern char tempUsername[];
extern uint16_t tempUsernameLength;
extern uint8_t writeUsername;

extern char UsernameBuffer[];

uint8_t XYZFlag = 0;
extern char tempXYZ[];
extern uint16_t tempXYZLength;
extern uint8_t writeXYZ;

extern char XYZBuffer[];

uint8_t SVFlag = 0;
extern char tempSV[];
extern uint16_t tempSVLength;
extern uint8_t writeSV;

extern char SVBuffer[];

*/


void sendRawDataItem(void) 
	{
		//Sensor_Reading *rdg = rawDataPeek();
		Sensor_Reading r;
		Sensor_Reading *rdg = &r;
		char outbuf[20];
		
		int x,y,z;
		
		accel_get_values(&x,&y,&z);
		r.val.x_ac = x;
		r.val.y_ac = y;
		r.val.z_ac = z;
		
		sprintf (outbuf, "Ax: %04x = %04f",rdg->val.x_ac,CONVERT_TO_G(rdg->val.x_ac));
		ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
		sprintf (outbuf, "Ay: %04x = %04f",rdg->val.y_ac,CONVERT_TO_G(rdg->val.y_ac));
		ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
		sprintf (outbuf, "Az: %04x = %04f",rdg->val.z_ac,CONVERT_TO_G(rdg->val.z_ac));
		ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
		sprintf (outbuf, "R: %04f",getR(rdg->val.x_ac,rdg->val.y_ac,rdg->val.z_ac));
		ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
	}

void commandAPI_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
	      nrf_gpio_pin_set(LED_RESETTING); 
        nrf_delay_ms(20);
        nrf_gpio_pin_clear(LED_RESETTING);	
	
	char outbuf[20];
	
	if(writingPerso == 0)
		{
	
			if(p_data[0] == 'A') //0xEE; SET GOLDEN TIMESTAMP
			{
					ble_nus_send_string(&m_nus, "OK", 2);
	/*
					
					// Time Stamp Processing Device Side
					timeYear = (uint16_t)(p_data[1]);
					timeYear = (timeYear << 8) | p_data[2];
	 */       
			 int   timeHour = (uint16_t)(p_data[1]);
					timeHour = (timeHour  << 8) | p_data[2];
					
			 int   timeSeconds = p_data[3]* 60 + p_data[4];
				
				setGoldenTime(timeHour,timeSeconds);
					
				//  SetTimestamp = 1;
					
					ble_nus_send_string(&m_nus, "SetTimestamp        ", 20);  //comment out on final
			}
			else if (p_data[0] == 'O') //Check for orientation
			{
		//sendOrientation = 1;   //comment out on final
		//ble_nus_send_string(&m_nus, "SendOrientation\n   ", 16);  //comment out on final
		
		//		if (sendOrientation == 1)
		{
			float p,r;
			accel_get_angles(&p, &r);			
			sprintf(outbuf, "pitch: %04f",p);
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			
			sprintf(outbuf, "roll: %04f", r);
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			
			if (checkOrientation() == ORIENTATION_STANDING)
			{
				sprintf (outbuf, "standing: %04f",p);
				ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			}
			else
			{	
				sprintf (outbuf, "not standing: %04f",p);
				ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			}
		}
	}
			else if (p_data[0] == 'B')//0xB4; READ STEP DATA
			{
					ReadDataBuffer = 1;
					
					ble_nus_send_string(&m_nus, "ReadDataBuffer      ", 20);  //comment out on final
	
			}
			
			else if (p_data[0] == 'J')//0xB4; READ STEP DATA
			{
					ReadDataBufferPartial = 1;
					
					ble_nus_send_string(&m_nus, "ReadDataBuffer      ", 20);  //comment out on final
					PartialCount1 = p_data[1];
					PartialCount2 = p_data[2];
	
			}
			
			else if (p_data[0] == 'C')//0xB8; ERASE STEP DATA
			{

					
					ClearDataBuffer = 1;
					
					ble_nus_send_string(&m_nus, "OK", 2);
					ble_nus_send_string(&m_nus, "ClearDataBuffer     ", 20);  //comment out on final
			}
			else if (p_data[0] == 'D')//0xC4; READ SNAPSHOTS
			{

					
					ReadSnapshots = 1;
					
					ble_nus_send_string(&m_nus, "ReadSnapshots       ", 20);  //comment out on final
			}
			else if (p_data[0] == 'K')//0xC4; READ SNAPSHOTS
			{

					
					ReadSnapshotsPartial = 1;
					
					ble_nus_send_string(&m_nus, "ReadSnapshots       ", 20);  //comment out on final
					
					PartialSnap1 = p_data[1];
					PartialSnap2 = p_data[2];
			}
			else if (p_data[0] == 'E')//0xC8; ERASE SNAPSHOTS
			{

				 
					ClearSnapshots = 1;
					
					ble_nus_send_string(&m_nus, "OK", 2);
					ble_nus_send_string(&m_nus, "ClearSnapshots                    ", 20);  //comment out on final

			}
			
    else if ( (p_data[0] == 'X') || (p_data[0] == 'x') )  //DATA COUNT
    {


					Dumper = 1;   //comment out on final
					
					ble_nus_send_string(&m_nus, "DumpData                    ", 20);  //comment out on final
				
			}
			
			else if (p_data[0] == 'S') //READ BIG SNAPSHOT LIVE
			{

					ReadBigSnapshot = 1;   //comment out on final
					
					ble_nus_send_string(&m_nus, "ReadBigSnapshot\n   ", 16);  //comment out on final
				
			}
			
/*			else if(p_data[0] == 'J') //TYPE SF_XYZ
			{	
				ble_nus_send_string(&m_nus, "TYPE SF_XYZ", 13);
				XYZFlag = 1;
				
			}	
			
			else if(p_data[0] == 'K') //TYPE SF_SV
			{	
			
				ble_nus_send_string(&m_nus, "TYPE SF_SV", 13);
				SVFlag = 1;
				
			}				

			else if(p_data[0] == 'L')  //READ XYZ_ADDRESS
			{
				uint8_t xyzVar;
				uint8_t xyzVar2;
				for (xyzVar=0;xyzVar<5;xyzVar++)
				{
					for(xyzVar2=0;xyzVar2<4;xyzVar2++)
					{
						if(((xyzVar*4)+xyzVar2)<(*XYZ_LENGTH))
						{
							XYZBuffer[(xyzVar*4)+xyzVar2]=(((*(XYZ_ADDRESS+xyzVar))>>(xyzVar2*8))&0xFF);
						}
					}
				}
				ble_nus_send_string(&m_nus, (unsigned char*)XYZBuffer, 20);
			}	
			
			else if(p_data[0] == 'M') //read SVBuffer
			{
				uint8_t svVar;
				uint8_t svVar2;
				for (svVar=0;svVar<5;svVar++)
				{
					for(svVar2=0;svVar2<4;svVar2++)
					{
						if(((svVar*4)+svVar2)<(*SV_LENGTH))
						{
							SVBuffer[(svVar*4)+svVar2]=(((*(SV_ADDRESS+svVar))>>(svVar2*8))&0xFF);
						}
					}
				}
				ble_nus_send_string(&m_nus, (unsigned char*)SVBuffer, 20);
			}	
	*/		
/*			else if(p_data[0] == 'N') //TYPE USERNAME
			{	
			
				ble_nus_send_string(&m_nus, "TYPE USERNAME", 13);
				usernameFlag = 1;
				
			}	
			
			else if(p_data[0] == 'U') //read USERNAME
			{
				uint8_t uVar;
				uint8_t uVar2;
				for (uVar=0;uVar<5;uVar++)
				{
					for(uVar2=0;uVar2<4;uVar2++)
					{
						if(((uVar*4)+uVar2)<(*USERNAME_LENGTH))
						{
							UsernameBuffer[(uVar*4)+uVar2]=(((*(USERNAME_ADDRESS+uVar))>>(uVar2*8))&0xFF);
						}
					}
				} 
				ble_nus_send_string(&m_nus, (unsigned char*)UsernameBuffer, 20);
			}	
*/			
			else if(p_data[0] == 'R') // CLEAR PERSO
			{	
			
				ble_nus_send_string(&m_nus, "PAGE CLEARED", 12);
				clearPerso = 1;
				
			}

			else if ( (p_data[0] == 'P') || (p_data[0] == 'N')) // Write PERSO
			{	
			
				ble_nus_send_string(&m_nus, "New Personalization", 20);
				copyCurrentToTemp();
				ble_nus_send_string(&m_nus, "TYPE USERNAME", 13);
				
				writingPerso = 1;
				usernameFlag = 1;
				
			}			
			
						else if (p_data[0] == 'v') //SEND RAW DATA READING
			{


					sendRawDataItem();
				
			}
			
			else if (p_data[0] == 'Z') //START SENDING DATA AT 1HZ
			{
					sendRawData = 1;   
					
					ble_nus_send_string(&m_nus, "SendRawData-Start\n   ", 16);  //comment out on final
				
			}
			else if (p_data[0] == 'z') //STOP SENDING DATA AT 1HZ
			{


					sendRawData = 0;   
					
					ble_nus_send_string(&m_nus, "SendRawData-Stop\n   ", 16);  //comment out on final
				
			}
			
			else if (p_data[0] == 'w') //current Perso
			{

					sprintf (outbuf, "impact: %02f",getPerso()->impact_threshold);
					ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
					sprintf (outbuf, "freeFall: %02f",getPerso()->freefall_threshold);
					ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
					sprintf (outbuf, "xAngle: %02f",getPerso()->x_angle_threshold);
					ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
					sprintf (outbuf, "yAngle: %02f",getPerso()->y_angle_threshold);
					ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
				
			}
			
			
			else
			{
					
					ble_nus_send_string(&m_nus, "NAK", 3);  //comment out on final
					ble_nus_send_string(&m_nus, "NoCommand           ", 20);  //comment out on final
			}
		}
		
		else if (usernameFlag==1)
		{
				uint8_t i;
				for(i=0;i<length;i++)
				{
					tempPerso.uname[i]=p_data[i];
				}
				tempPerso.uname[length] = '\n';
				usernameFlag = 0;
				
				ble_nus_send_string(&m_nus, "TYPE IMPACT COEF", 16);
				impactFlag = 1;
				
		}
/*		
		else if (XYZFlag==1)
		{
				uint8_t j;
				for(j=0;j<length;j++)
				{
					tempXYZ[j]=p_data[j];
				}
				tempXYZLength = length;
				ble_nus_send_string(&m_nus, "SF_XYZ ENTERED", 14);
				XYZFlag = 0;
				writeXYZ = 1;
		}

		else if (SVFlag==1)
		{
				uint8_t k;
				for(k=0;k<length;k++)
				{
					tempSV[k]=p_data[k];
				}
				tempSVLength = length;
				ble_nus_send_string(&m_nus, "SF_SV ENTERED", 13);
				SVFlag = 0;
				writeSV = 1;
		}
*/		
		else if (impactFlag == 1)
		{
			tempPerso.impact_threshold = string_to_f(p_data);
			impactFlag = 0;
						
			ble_nus_send_string(&m_nus, "TYPE FF COEF", 16);
			FreeFallFlag = 1;
			
		}
		
		else if (FreeFallFlag == 1)
		{
			tempPerso.freefall_threshold = string_to_f(p_data);
			FreeFallFlag = 0;
			
			ble_nus_send_string(&m_nus, "TYPE XANGLE", 16);
			xAngleFlag = 1;
			
		}
		
		else if (xAngleFlag == 1)
		{
			tempPerso.x_angle_threshold = string_to_f(p_data);
			xAngleFlag = 0;
			
			ble_nus_send_string(&m_nus, "TYPE YANGLE", 16);
			yAngleFlag = 1;
			
			
		}
		
		else if (yAngleFlag == 1)
		{
			tempPerso.y_angle_threshold = string_to_f(p_data);
			yAngleFlag = 0;
			
			ble_nus_send_string(&m_nus, "rewriting perso", 16);
			rewritePerso = 1;
			writingPerso = 0;
			
			
		}
		




}
