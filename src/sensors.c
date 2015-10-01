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