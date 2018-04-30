#ifndef _ANIMATIONRINGLIGHTS_h
#define _ANIMATIONRINGLIGHTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnimationRingLightsClass
{
public:
	uint32_t animate();

private:
	uint8_t _currentChevron = 0;
};

extern AnimationRingLightsClass AnimationRingLights;

#endif

