// DialProgram.h

#ifndef _DIALPROGRAM_h
#define _DIALPROGRAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class DialProgramClass
{
 public:
	void dial(uint8_t address[]);

protected:

};

extern DialProgramClass DialProgram;

#endif

