#include "AnimationChase.h"
#include "config.h"
#include "StargateControl.h"

uint32_t AnimationChaseClass::animate()
{
	switch (_currentState)
	{
	case 0:
		StargateControl.lightChevron(_currentChevron++);
		if (_currentChevron >= NUM_CHEVRONS)
		{
			_currentChevron = 0;
			_currentState = 1;
		}
		break;

	case 1:
		StargateControl.darkChevron(_currentChevron++);
		if (_currentChevron >= NUM_CHEVRONS)
		{
			_currentChevron = NUM_CHEVRONS - 1;
			_currentState = 2;
		}
		break;

	case 2:
		StargateControl.lightChevron(_currentChevron--);
		if (_currentChevron < 0)
		{
			_currentChevron = NUM_CHEVRONS - 1;
			_currentState = 3;
		}
		break;

	case 3:
		StargateControl.darkChevron(_currentChevron--);
		if (_currentChevron < 0)
		{
			_currentChevron = 0;
			_currentState = 0;
		}
		break;

	default:
		_currentChevron = 0;
		_currentState = 0;
		return 0;
	}

	return 250;
}


AnimationChaseClass AnimationChase;

