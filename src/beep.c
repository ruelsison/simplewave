//beep.c

#include "main_includes.h"

void beep(uint32_t delay, unsigned char n)
{
	while (n > 0) 
	{
		
		nrf_gpio_pin_write(BEEP,0);
		nrf_delay_ms(delay);
		nrf_gpio_pin_write(BEEP,0XFF);
		n--;
		if (n==0) break;
		nrf_delay_ms(delay/2);
	}
}
