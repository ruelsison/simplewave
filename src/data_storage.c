//data_storage.c

#include "main_includes.h"
#include "data_storage.h"
#include "accelerometer.h"
#include "memory_map.h"
#include "timestamp.h"

uint32_t err_code;



uint32_t *log_wr_ptr;
uint32_t *snapshot_wr_ptr;



uint32_t *startAddress (unsigned char space)
{
	uint32_t *last;
	if (space == SEARCH_STEP_SPACE)
		last = (STEP_DATA_ADDRESS_START);
	else if (space == SEARCH_SNAPSHOT_SPACE)
		last = (SNAPSHOT_DATA_ADDRESS_START);
	
	return last;
}
uint32_t *findNextWriteAddress (unsigned char space)
{
  uint32_t *last = startAddress(space);
	
    while(*last != 0xFFFFFFFF){
     //   if ((uint32_t)last < (uint32_t)SNAPSHOT_DATA_ADDRESS_START)
        {
            last++;
        }
    }
    
    return last;
}





/**
 *  DATA STORAGE FUNCTIONS
 */



uint32_t getNumberOfItems(unsigned char space)
{
	uint32_t add = (uint32_t)findNextWriteAddress(space) - (uint32_t)startAddress(space);
	uint32_t num;
	
	if (space == SEARCH_STEP_SPACE)
		num = (int)add / sizeof(LogEntry);
	else if (space == SEARCH_SNAPSHOT_SPACE)
		num = (int)add / sizeof(FullSnapshotEntry);
	
	return num;
}


