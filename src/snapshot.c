//snapshot.c

#include "snapshot.h"
#include "accelerometer.h"
#include "memory_map.h"
#include "timestamp.h"

SnapshotHeader hdr;
Sensor_Reading raw_data[NUMBER_OF_ENTRIES];
Sensor_Reading raw_data_backup[NUMBER_OF_ENTRIES];

uint32_t ramBufferIndex;

void initSnapshotBuffer (void)
{
    
    ramBufferIndex = 0;
    for (int i = 0; i < NUMBER_OF_ENTRIES; i++)
    {
			for (int j = 0; j<7;j++)
				raw_data[i].buf[j] = 0;
    }
}

int readSensorsAndStore(void)
{
	sensor_get_values(raw_data[ramBufferIndex].buf);
	
	ramBufferIndex++;
	
	if (ramBufferIndex >= NUMBER_OF_ENTRIES)
	{
			ramBufferIndex = 0;
	} 

	return ramBufferIndex;
}




SnapshotHeader* getSnapshotHeader(void)
{
	hdr.data.time = getHour();
	hdr.data.num_of_data_points = NUMBER_OF_ENTRIES;
	hdr.data.latest_data_point_index = ramBufferIndex;
	
	return &hdr;
}


void backupRawData(void)
{
	for (int i = 0;i < STORED_SNAPSHOT_SIZE-1; i++)
	{
		((uint32_t*)raw_data_backup)[i] = ((uint32_t*)raw_data)[i];
	}
}

void writeSnapshotHeader(void)
{
//	ble_flash_word_write(snapshot_wr_ptr,getSnapshotHeader()->word);
	
//	snapshot_wr_ptr++;
}

Sensor_Reading* rawDataPeek(void)
{
	if (ramBufferIndex == 0)
		return &raw_data[NUMBER_OF_ENTRIES-1];
	else
		return &raw_data[ramBufferIndex-1];
	
}

void writeBigSnapshotData(void) 
{
//	ble_flash_block_write(snapshot_wr_ptr,(uint32_t*)raw_data_backup,(sizeof(Sensor_Reading)*NUMBER_OF_ENTRIES/sizeof(uint32_t)));
	
//	snapshot_wr_ptr+= sizeof(Sensor_Reading)*NUMBER_OF_ENTRIES/sizeof(uint32_t);
}

void writeSnapshotToFlash (void)
{
	//TODO: Replace with calls to ActivityLog.c
	
	writeSnapshotHeader();
	writeBigSnapshotData();
	
}


//#define UNIT_TEST
#ifdef UNIT_TEST

void make_dummy_sensor_data(void)
{
	for (int i = 0; i<NUMBER_OF_ENTRIES; i++) 
	{
		for (int j = 0; j < 7; j++)
		{
			raw_data[i].buf[j] = i*j;
		}
	}
}

void test_data_storage(void)
{
	make_dummy_sensor_data();
	backupRawData();
	
	writeSnapshotToFlash();
	
}

#endif
