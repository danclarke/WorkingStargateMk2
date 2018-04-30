#ifndef _ANIMATIONCHASE_h
#define _ANIMATIONCHASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnimationChaseClass
{
public:
	uint32_t animate();

private:
	int8_t _currentChevron = 0;
	uint8_t _currentState = 0;
};

extern AnimationChaseClass AnimationChase;

#endif

