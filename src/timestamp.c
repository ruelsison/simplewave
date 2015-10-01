//Timestamp.c
#include "main_includes.h"
#include "timestamp.h"

// timekeeper.c

uint16_t goldenTime;
uint16_t tickCount;
uint16_t hourCount;

void initTimeStamp(void)
{
	hourCount = 0xFFFF;
	tickCount = 0;
}

uint16_t tickSecond(void)
{
	if (++tickCount >= 3600)
	{
		hourCount++;
		tickCount = 0;
	}
	
	return tickCount;
}

uint16_t getSecond(void)
{
	return tickCount;
}

uint16_t getMinute(void)
{
	return tickCount/60;
}


uint16_t getHour(void) 
{
	if (hourCount == 0xffff) 
	{
		hourCount = 0;
		return 0xFFFF;
	}

	return goldenTime + hourCount;
}

void setGoldenTime(uint16_t hours,uint16_t ticks) 
{
	goldenTime = hours;
	hourCount = 0;
	tickCount = ticks;
}

