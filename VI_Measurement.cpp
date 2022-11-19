// 
// 
// 

#include "VI_Measurement.h"
#include "SDL_Arduino_INA3221.h"

SDL_Arduino_INA3221 ina3221;

extern float SolarCell_V;
extern float SolarCell_mA;
extern float Charge_V;
extern float Charge_mA;
extern float Discharge_V;
extern float Discharge_mA;

#define Ch1 1
#define Ch2 2
#define Ch3 3

void init_ina3221(void)
{
	ina3221.begin();
}

void read_ina3221(void)
{
	SolarCell_V = ina3221.getBusVoltage_V(Ch1);
	SolarCell_mA = ina3221.getCurrent_mA(Ch1);

	Charge_V = ina3221.getBusVoltage_V(Ch2);
	Charge_mA = ina3221.getCurrent_mA(Ch2);

	Discharge_V = ina3221.getBusVoltage_V(Ch3);
	Discharge_mA = -ina3221.getCurrent_mA(Ch3); // x -1
}