#ifndef _STARGATELOGIC_h
#define _STARGATELOGIC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class StargateLogicClass
{
public:
	void init();
	uint32_t loop();
	void processRequest(const char* jsonStr, size_t jsonStrLen);

private:
	SemaphoreHandle_t _loopLock;
	uint8_t _state = 0;
	uint8_t _address[7] = {26,6,14,31,11,29,0};
};

extern StargateLogicClass StargateLogic;

#endif

