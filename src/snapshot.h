//snapshot.h
#include "main_includes.h"

#define NUMBER_OF_ENTRIES 	125
#define SNAPSHOT_SIZE 		(NUMBER_OF_ENTRIES*sizeof(float) )

#define STORED_SNAPSHOT_SIZE (sizeof(SnapshotHeader)+sizeof(Sensor_Reading)*NUMBER_OF_ENTRIES)

typedef union 
{
	struct 
	{
		uint16_t hour;
		uint16_t sec;
		float activity_level;
	}item;
	uint32_t word[2];
} LogEntry;

typedef union 
{
	uint16_t buf[7];
	struct 
		{
			uint16_t x_ac;
			uint16_t y_ac;
			uint16_t z_ac;
			uint16_t temp;
			uint16_t x_gy;
			uint16_t y_gy;
			uint16_t z_gy;
		} val;
} Sensor_Reading;

typedef union
{
		struct 
		{
			uint16_t time;
			uint8_t num_of_data_points;
			uint8_t latest_data_point_index;
		} data;
		uint32_t word;
} SnapshotHeader;

typedef struct 
{
	SnapshotHeader hdr;
	Sensor_Reading r[NUMBER_OF_ENTRIES];
	
	
} FullSnapshotEntry;

extern Sensor_Reading raw_data[NUMBER_OF_ENTRIES];
Sensor_Reading* rawDataPeek(void);

int readSensorsAndStore(void);
SnapshotHeader* getSnapshotHeader(void);
void backupRawData(void);

void writeSnapshotHeader(void);
void writeBigSnapshotData(void) ;

void writeSnapshotToFlash(void);

void initSnapshotBuffer (void);
