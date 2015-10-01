//Timestamp.h

#define MINUTES_IN_A_DAY (60*24)

uint16_t getHour(void);
uint16_t getSecond(void);

void setGoldenTime(uint16_t hours, uint16_t ticks);
void initTimeStamp(void);

//increments the internals second count. returns the current count.
uint16_t tickSecond(void);

uint16_t getMinute(void);


