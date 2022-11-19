// CLI.h

#ifndef _CLI_h
#define _CLI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void CLI_Process_Message(void);
void CLI_Processor(void);


#endif

