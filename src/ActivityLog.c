//ActivityLog.c

#include "main_includes.h"
#include "ActivityLog.h"
#include "timestamp.h"
#include "memory_map.h"

uint32_t err_code;

uint32_t *activityLogWritePointer;

uint32_t *activityLogStartAddress (void)
{
	uint32_t *last;

		last = (ACTIVITY_LOG_ADDRESS_START);
	
	return last;
}
uint32_t *findNextActivityLogWriteAddress (void)
{
  uint32_t *last = activityLogStartAddress();
	
    while(*last != 0xFFFFFFFF){
     //   if ((uint32_t)last < (uint32_t)SNAPSHOT_DATA_ADDRESS_START)
        {
            last++;
        }
    }
    
    return last;
}

// Initialization
void initActivityLog (void)
{    
	
		activityLogWritePointer = findNextActivityLogWriteAddress();

}

uint32_t getActivityLogSize(void)
{
	return 0;
}

void eraseActivityLog(void)
{
    int start_page;
		int size;
	
		start_page = ACTIVITY_LOG_PAGE_START;
		size = ACTIVITY_LOG_PAGE_COUNT;

		
    
    for (int i = 0; i <= size; i++)
    {
        err_code = ble_flash_page_erase(i+start_page);
        APP_ERROR_CHECK(err_code);
        //nrf_delay_ms(50);
    }
}

uint32_t writeActivityLogEntry (uint16_t eventCode) 
{
	ActivityLogEntry e;
	
	e.item.minute = getMinute();
	e.item.eventCode = eventCode;
	
	err_code = ble_flash_word_write(activityLogWritePointer++, e.word);
	APP_ERROR_CHECK(err_code);
	
	return err_code;

}

//

uint32_t writeActivityLogData (uint16_t descriptor, uint8_t *logData, uint8_t size) 
{
	ActivityLogDataDescriptor e;
	e.item.descriptor = descriptor;
	e.item.size = size; //size is in 32 bit WORDS
	
	err_code = ble_flash_word_write(activityLogWritePointer++, e.word);
	APP_ERROR_CHECK(err_code);
	
	ble_flash_block_write(activityLogWritePointer,(uint32_t*)logData,size);
	
	activityLogWritePointer += size;
	
	return err_code;

}

//Log Reading Functions



ActivityLogEntry * getFirstActivityLogEntry(void)
{
		ActivityLogEntry * entry = (ActivityLogEntry*)activityLogStartAddress();
	
		return entry;
}

ActivityLogEntry * getNextActivityLogEntryAfterEntry(ActivityLogEntry *lastEntry)
{
		ActivityLogEntry * entry = lastEntry;
	
		entry++;
	
			if (	((uint32_t)entry >= (uint32_t)ACTIVITY_LOG_ADDRESS_END) 
						|| (entry->item.eventCode == 0xffff)  ) 
			{
				return 0;
			}
	
		return entry;
}




ActivityLogEntry * getNextActivityLogEntryAfterTime(uint16_t min)
{
		ActivityLogEntry * entry = (ActivityLogEntry*)activityLogStartAddress();
	
		
	
		while (min > entry->item.minute) 
		{
			entry = getNextActivityLogEntryAfterEntry(entry);
			
			if (entry == 0)
				break;
			
			
		}
			
	
		return entry;
}


#ifdef TEST_ACTIVITY_LOG
void testActivityLog(void)
{
	eraseActivityLog();
	initActivityLog();
	
	writeActivityLogEntry(0xabcd);
	writeActivityLogEntry(0x1111);
	writeActivityLogEntry(0x4444);
	writeActivityLogEntry(0x8888);
	writeActivityLogEntry(0xcdcd);
	writeActivityLogEntry(0xeecc);
	
	ActivityLogEntry * entry = (ActivityLogEntry*)activityLogStartAddress();
	
	entry = getNextActivityLogEntryAfterEntry(entry);
	entry = getNextActivityLogEntryAfterEntry(entry);
	entry = getNextActivityLogEntryAfterEntry(entry);
	
	
	while(1);
	//stop. do not continue into the program.
	
}

#endif


