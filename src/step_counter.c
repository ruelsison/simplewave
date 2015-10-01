//step_counter.c

/*
 *  This module counts step data based from acceleration data.
 *  This module should include the following:
 *  - initialization of step counter
 *  - algorithms for converting acceleration data into step data.
 *  - getter function for accessing number of steps
 *  ...
 */


//ADC part
#include "main_includes.h"
#include <math.h>
#include "nrf_soc.h"
#include "timestamp.h"

		
static uint32_t current_step_count;
static unsigned char debounce;

const unsigned char testData[] = 
	{ 
		108,108,108,108,108,
		140,140,140,140,140,
		108,108,108,108,108,
		140,140,140,140,140
	};
	
static unsigned char testIndex;

typedef struct 
{
	Timestamp time;
//	union 
//	{
		uint8_t data[3];
/*		struct 
		{
			uint8_t x,y,z;
		}v; 
	}acc; 
*/
	//unsigned char x,y,z;
} Acc_Reading;

#define ACC_LOG_LEN 100
static Acc_Reading acc_log[ACC_LOG_LEN];
static uint8_t log_wIndex, log_rIndex;

void insert_fake_data(void)
{
	uint8_t i=0;
	
	for (i=0;i<20;i++)
	{
		acc_log[i].time.year = 15;
		acc_log[i].time.day = 366;
		acc_log[i].time.minute = 1500;
		acc_log[i].data[0] = testData[i];
		acc_log[i].data[1] = testData[i];
		acc_log[i].data[2] = testData[i];
	}
	
	log_wIndex = 20;
	log_rIndex = 0;
	
}
void init_step_counter(void) 
{
	current_step_count = 0;
	debounce = 0;
	
	testIndex = 0;
	log_wIndex = 0;
	log_rIndex = 0;
	
	insert_fake_data();
}

/**@brief Function for reading an ADC pin
 */

//sensors.c

unsigned char adc_start(unsigned char input)
{
    uint8_t     adc_result;
	
	switch (input) 
	{
		case 0:
			input = ADC_CONFIG_PSEL_AnalogInput2;
			break;
		case 1:
			input = ADC_CONFIG_PSEL_AnalogInput4;
			break;
		case 2:
			input = ADC_CONFIG_PSEL_AnalogInput6;
			break;
		default:
			input = ADC_CONFIG_PSEL_AnalogInput2;
			
	}		

    // Configure ADC
    NRF_ADC->INTENSET   = ADC_INTENSET_END_Msk;
    NRF_ADC->CONFIG     = (ADC_CONFIG_RES_8bit                        << ADC_CONFIG_RES_Pos)     |
                          (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  |
                          (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)  |
                          (input                   										<< ADC_CONFIG_PSEL_Pos)    |
                          (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;


    NRF_ADC->EVENTS_END  = 0;    // Stop any running conversions.
    NRF_ADC->TASKS_START = 1;
	
	while (NRF_ADC->EVENTS_END == 0);
	
        NRF_ADC->EVENTS_END     = 0;
        adc_result              = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP     = 1;
				
				return adc_result;
}

double convert_to_g(unsigned char adc_val)
{
	// Typical ADC values at different g
	//  1g at around 128
	//  0g at around 108
	// -1g at around  88
	
	//  acc = (adc_val - 108)/20
	
	double val = adc_val - 108;
	val = val /20;
	
	return val;
}

void read_adc(unsigned char *acc)
{
	if (testIndex > 20)
		testIndex = 0;
	
	acc[0] = testData[testIndex++]; //adc_start(0);
	acc[1] = 0;//adc_start(1);
	acc[2] = 0;//adc_start(2);
}

double get_magnitude(unsigned char *acc) 
{
	float x,y,z;
	double	r, r2;
	
	x = convert_to_g(acc[0]);
	y = convert_to_g(acc[1]);
	z = convert_to_g(acc[2]);
	
	r2 = x*x + y*y + z*z;
	r = sqrt(r2);
	
	return r;
	
}


#define getTime()

void log_adc(void)
{
	
	uint8_t i,acc[3];
//	rdg.time = getTime();

	
		if (testIndex > 20)
			testIndex = 0;
/*	
	acc[0] = testData[testIndex++]; //adc_start(0);
	acc[1] = 0;//adc_start(1);
	acc[2] = 0;//adc_start(2);
*/
	acc[0] = testData[testIndex]; //adc_start(0);
	acc[1] = testData[testIndex]; //adc_start(1);
	acc[2] = testData[testIndex++]; //adc_start(2);
		
	//memcpy(&(acc_log[log_wIndex++]), (void*)(&rdg), sizeof(Acc_Reading));
		acc_log[log_wIndex].time = getTimestamp();
	for (i=0;i<3;i++)
	{
		acc_log[log_wIndex].data[i] = acc[i];
	}
	log_wIndex++;
	
	if (log_wIndex >= ACC_LOG_LEN)
		log_wIndex = 0;
	
}



void process_count(Acc_Reading *rdg)
{
		unsigned char *acc;
		double r;
	
		acc = rdg->data;
	
//		read_adc(acc);
	
		r = get_magnitude(acc);
		
		if ( (r > 1.5) && (debounce == 0) )
		{
			current_step_count++;
			debounce = 1;
		}
		
		if (r < 1.5)
		{
			debounce = 0;
		}
	
	
}

void process_log(void)
{
	unsigned char *acc;
	double r;
	
	while (log_rIndex != log_wIndex)
	{
	//	process_count(&acc_log(log_rIndex++));
		acc = acc_log[log_rIndex++].data;
		
		r = get_magnitude(acc);
		
		#define THRESHOLD (0.8)
		
		if ( (r > THRESHOLD) && (debounce == 0) )
		{
			current_step_count++;
			debounce = 1;
		}
		
		if ( (r < THRESHOLD) && (debounce == 1) )
		{
			debounce = 0;
		}
		
		if (log_rIndex >= 100)
			log_rIndex = 0;
	}
	
	
	
}

uint32_t get_steps(void)
{
	return current_step_count;
}

