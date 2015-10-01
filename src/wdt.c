//wdt.c

//NRF_WDT

#include "main_includes.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

void setup_wdt(void)
{
	NRF_WDT->CRV = 0xffff;
	NRF_WDT->RREN = 1;
	NRF_WDT->CONFIG = 1;
	NRF_WDT->TASKS_START = 1;
	
}

void reset_wdt(void)
{
	NRF_WDT->RR[0] = 0x6E524635;
}

void wdt_isr(void)
{
	
}

