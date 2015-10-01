//data_storage.h

#include "main_includes.h"
#include "snapshot.h"

#define SEARCH_STEP_SPACE 0x0A
#define SEARCH_SNAPSHOT_SPACE 0xA0


void initDataStorage (void);
void eraseAllItems(void);
void eraseAllStepData(void);
void eraseAllSnapshots(void);
uint32_t getAddressToWriteSnapshot (void);
void setAddressToWriteSnapshot (uint32_t addr);

void writeSumVectorToBuffer (float sum_vector);
void writeSnapshotToFlash (void);

void backupCurrentSteps(uint32_t nSteps,uint32_t clockTicks,uint32_t *lastAddress);
void backupChangeAddress(uint32_t *newAddress);

void setResetFlagValue (uint32_t resetFlag);
uint32_t getResetFlagValue (void);
uint32_t *findNextWriteAddress (unsigned char space);
uint32_t getNumberOfItems(unsigned char space);


