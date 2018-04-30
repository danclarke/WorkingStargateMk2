#include "AnimationRingLights.h"
#include "StargateControl.h"

uint32_t AnimationRingLightsClass::animate()
{
	StargateControl.lightChevron(_currentChevron);

	delay(20);
	if (_currentChevron > 0)
		StargateControl.darkChevron(_currentChevron - 1);
	else
		StargateControl.darkChevron(NUM_CHEVRONS - 1);

	if (++_currentChevron >= NUM_CHEVRONS)
		_currentChevron = 0;

	return 120;
}


AnimationRingLightsClass AnimationRingLights;
