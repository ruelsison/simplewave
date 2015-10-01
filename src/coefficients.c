//globals.c
#include "main_includes.h"
#include "coefficients.h"
#include "memory_map.h"


PersoValues tempPerso;
PersoValues livePerso;

const PersoValues defaultPerso = {
	"default user\n",
	25,
	SMOOTHING_FACTOR_XYZ_DEFAULT,
	SMOOTHING_FACTOR_SV_DEFAULT,
	FALL_DETECTION_WINDOW_DEFAULT,
	FREEFALL_THRESHOLD_DEFAULT,
	X_ANGLE_THRESHOLD_DEFAULT,
	Y_ANGLE_THRESHOLD_DEFAULT,
	IMPACT_HIGH_DEFAULT
	
	

};



float string_to_f(uint8_t* s)
{
	float val = 0;
	
	val = s[0] - '0';
	if (s[1] != '.')
		return val;
	
	val = val + (s[2]-'0')/10.0;
	
	return val;
}

//PersoValues *livePerso = (PersoValues*) FLAG_AREA_START;
/*
void copyLiveToTemp(void)
{
	uint32_t* dst = (uint32_t*)&tempPerso;
	uint32_t* src = (uint32_t*)livePerso;
	
	for (int i=0;i<PERSO_SIZE;i++)
		dst[i] = src[i];
}

void copyDefaultToTemp(void)
{
	uint32_t* dst = (uint32_t*)&tempPerso;
	uint32_t* src = (uint32_t*)&defaultPerso;
	
	for (int i=0;i<PERSO_SIZE;i++)
		dst[i] = src[i];
}
*/
void copyCurrentToTemp(void)
{
	uint32_t* dst = (uint32_t*)&tempPerso;
	uint32_t* src = (uint32_t*)getPerso();
	
	for (int i=0;i<PERSO_SIZE;i++)
		dst[i] = src[i];
}

void copyFlashToLive(void)
{
	uint32_t* dst = (uint32_t*)&livePerso;
	uint32_t* src = (uint32_t*)flashPerso;
	
	for (int i=0;i<PERSO_SIZE;i++)
		dst[i] = src[i];
}

void copyDefaultToLive(void)
{
	uint32_t* dst = (uint32_t*)&livePerso;
	uint32_t* src = (uint32_t*)&defaultPerso;
	
	for (int i=0;i<PERSO_SIZE;i++)
		dst[i] = src[i];
}

void initCoefficients(void)
{
	if (flashPerso->uname[0] == (char)0xff)
		copyDefaultToLive();
	else
		copyFlashToLive();

}

void revert_to_defaults(void)
{
	ble_flash_page_erase(RESET_FLAG_PAGE);
	copyDefaultToLive();
}

void writeTempToFlashPerso(void)
{
	ble_flash_page_erase(RESET_FLAG_PAGE);
	ble_flash_block_write((uint32_t *)flashPerso,(uint32_t*)&tempPerso,PERSO_SIZE);
	
	copyFlashToLive();
	
	//insert verify: compare live to temp
}

PersoValues* getPerso(void)
{
//	if (livePerso.uname[0] == 0xff)
//		return (PersoValues*)&defaultPerso;
//	else
		return &livePerso;
}

void setUsername(char* name)
{
	ble_flash_block_write((uint32_t *)livePerso.uname, (uint32_t*)name,UNAME_MAXLEN);
}

void setFFThreshold(float th)
{
	ble_flash_word_write((uint32_t*)&(livePerso.freefall_threshold),(uint32_t)th);	
}

void setImpactThreshold(float th)
{
	ble_flash_word_write((uint32_t*)&(livePerso.impact_threshold),(uint32_t)th);	
}

void setXAngleThreshold(float th)
{
	ble_flash_word_write((uint32_t*)&(livePerso.x_angle_threshold),(uint32_t)th);	
}

void setYAngleThreshold(float th)
{
	ble_flash_word_write((uint32_t*)&(livePerso.y_angle_threshold),(uint32_t)th);	
}
