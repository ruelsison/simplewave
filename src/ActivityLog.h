//ActivityLog.h

#include "main_includes.h"

#define TEST_ACTIVITY_LOG

typedef union 
{
	struct 
	{
		uint16_t eventCode; // b8 - 1 followed by data, 0 no data follows.
		uint16_t minute; // number of minutes from the golden time
		
	}item;
	uint32_t word;
} ActivityLogEntry;

typedef union 
{
	struct 
	{
		uint16_t descriptor;  // additional meta descriptor for the data. RFU
		uint16_t size; //size of data in WORDS.
	}item;
	uint32_t word;
} ActivityLogDataDescriptor;

/*****

Activity Log is composed of Log entries with optional data attached.
A full log entry is composed of either 
   a single ActivityLogEntry,
or a single ActivityLogEntry followed by an ActivityLogDataDescriptor and binary data.

 

*****/

void initActivityLog (void);
void eraseActivityLog(void);
uint32_t writeActivityLogEntry (uint16_t eventCode);
uint32_t writeActivityLogData (uint16_t descriptor, uint8_t *logData, uint8_t size);

ActivityLogEntry * getNextActivityLogEntryAfterTime(uint16_t min);
ActivityLogEntry * getNextActivityLogEntryAfterEntry(ActivityLogEntry *entry);

uint32_t getActivityLogSize(void);

//ActivityLogEntry * getNextActivityLogEntryWithEventCodeAfterTime(uint16_t min);

#ifdef TEST_ACTIVITY_LOG
void testActivityLog(void);
#endif
	
