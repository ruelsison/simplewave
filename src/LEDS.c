#include "main_includes.h"
#include "LEDS.H"


void errorLEDs(void) {
	
		#define STARTUP_DELAY 60
		
		char retValue = 5;
	
		while (retValue > 0) {
			
			nrf_gpio_pin_write(LED_OTHER,1);
			nrf_gpio_pin_write(LED_RESETTING,0);
			nrf_gpio_pin_write(LED_NO_FALL,1);
			nrf_gpio_pin_write(LED_FALL_DETECTED,0);
			nrf_delay_ms(STARTUP_DELAY*5);
			nrf_gpio_pin_write(LED_OTHER,0);
			nrf_gpio_pin_write(LED_RESETTING,1);
			nrf_gpio_pin_write(LED_NO_FALL,0);
			nrf_gpio_pin_write(LED_FALL_DETECTED,1);
			nrf_delay_ms(STARTUP_DELAY*5);
			
			retValue--;
		}
	
}

void welcomeLEDs(void) {
	
		#define STARTUP_DELAY 60
		
		char retValue = 3;
		while (retValue > 0) {
			
			nrf_gpio_pin_write(LED_OTHER,1);
			nrf_delay_ms(STARTUP_DELAY);
			nrf_gpio_pin_write(LED_OTHER,0);
			
			nrf_gpio_pin_write(LED_RESETTING,1);
			nrf_delay_ms(STARTUP_DELAY);
			nrf_gpio_pin_write(LED_RESETTING,0);

			nrf_gpio_pin_write(LED_NO_FALL,1);
			nrf_delay_ms(STARTUP_DELAY);
			nrf_gpio_pin_write(LED_NO_FALL,0);
			
			nrf_gpio_pin_write(LED_FALL_DETECTED,1);
			nrf_delay_ms(STARTUP_DELAY);
			nrf_gpio_pin_write(LED_FALL_DETECTED,0);
			
			retValue--;
		}
		
		retValue = 2;
		while (retValue > 0) {
			
			nrf_gpio_pin_write(LED_OTHER,1);
			nrf_gpio_pin_write(LED_RESETTING,1);
			nrf_gpio_pin_write(LED_NO_FALL,1);
			nrf_gpio_pin_write(LED_FALL_DETECTED,1);
			nrf_delay_ms(STARTUP_DELAY*3);
			nrf_gpio_pin_write(LED_OTHER,0);
			nrf_gpio_pin_write(LED_RESETTING,0);
			nrf_gpio_pin_write(LED_NO_FALL,0);
			nrf_gpio_pin_write(LED_FALL_DETECTED,0);
			nrf_delay_ms(STARTUP_DELAY*2);
			
			retValue--;
		}
	
}
