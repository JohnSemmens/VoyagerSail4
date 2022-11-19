// utility.h

#ifndef _UTILITY_h
#define _UTILITY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void wiggleServo(void);

void FlashLed(int FlashCount);
void FlashLed2(int FlashCount);
void FlashBothLeds(int FlashCount);

#endif

