// VI_Measurement.h

#ifndef _VI_MEASUREMENT_h
#define _VI_MEASUREMENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
	void init_ina3221(void);
	void read_ina3221(void);

#endif

