// memory_map.h

#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_

#define PAGE_SIZE                   1024


#define RESET_FLAG_PAGE             					124 	//0x01f000
#define BACKUP_PAGE 		        							126 	//0x01f800
#define STEP_DATA_PAGE_START 	    						128	  //0x020000
#define STEP_DATA_PAGE_COUNT					64		// 64K (FFFF BYTES)
#define SNAPSHOT_DATA_PAGE_START 							(STEP_DATA_PAGE_START+STEP_DATA_PAGE_COUNT)  //192, //0x030000
#define SNAPSHOT_DATA_PAGE_COUNT 							60  //60K (F000 BYTES)

#define ACTIVITY_LOG_PAGE_START	 			STEP_DATA_PAGE_START


#define SNAPSHOT_PAGE_END 		        (SNAPSHOT_DATA_PAGE_START+SNAPSHOT_DATA_PAGE_COUNT)


#define DEVICE_ID                   25



// flag area - RESET_FLAG_ADDRESS
#define UNAME_MAXLEN 5

#define FLAG_AREA_START          ( (uint32_t *)(RESET_FLAG_PAGE*PAGE_SIZE)    )
//#define LAST_SNAPSHOT_ADDRESS       (RESET_FLAG_ADDRESS+4)     
//#define LOG_WR_PTR_ADDRESS					(LAST_SNAPSHOT_ADDRESS+4)
#define USERNAME_ADDRESS						FLAG_AREA_START //(LOG_WR_PTR_ADDRESS+4)
#define USERNAME_LENGTH							(USERNAME_ADDRESS+UNAME_MAXLEN)
#define XYZ_ADDRESS									(USERNAME_LENGTH+1)
#define XYZ_LENGTH									(XYZ_ADDRESS+3)
#define SV_ADDRESS									(XYZ_LENGTH+1)
#define SV_LENGTH										(SV_ADDRESS+3)

// data storage area
#define STEP_DATA_ADDRESS_START     (uint32_t*)(STEP_DATA_PAGE_START*PAGE_SIZE)        //0x20000
#define SNAPSHOT_DATA_ADDRESS_START (uint32_t*)(SNAPSHOT_DATA_PAGE_START*PAGE_SIZE)    //0x30000

#define ACTIVITY_LOG_ADDRESS_START	(uint32_t*)(STEP_DATA_PAGE_START*PAGE_SIZE) 
#define ACTIVITY_LOG_PAGE_COUNT			(STEP_DATA_PAGE_COUNT + SNAPSHOT_DATA_PAGE_COUNT)
#define ACTIVITY_LOG_ADDRESS_END		(uint32_t*)((PAGE_SIZE)*(STEP_DATA_PAGE_START+STEP_DATA_PAGE_COUNT+SNAPSHOT_DATA_PAGE_COUNT ))

// backup parameters
#define DIE_FLAG				    (uint32_t*)(BACKUP_PAGE*PAGE_SIZE)	                //0x1F800	
#define CURRENT_COUNT_BACKUP	    (DIE_FLAG+4)                            //0x1F804        
#define FRACTION_OF_HOUR		    (DIE_FLAG+8)                            //0x1F808
#define LAST_SUCCESSFUL_ADDRESS	    (DIE_FLAG+12)                           //0x1F80C       	

#endif
