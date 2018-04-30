#include "DialProgram.h"

#include "StargateControl.h"

// Order of the chevron lighting, 7 symbol address
uint8_t chevronLightOrder7Symbol[] = {
	5, 6, 7, 1, 2, 3
};

void DialProgramClass::dial(uint8_t address[])
{
	StargateControl.clearDisplay();

	// Home the gate
	if (!StargateControl.isAtHome())
	{
		StargateControl.moveToHome();
		delay(500);
	}

	// Go through dialing sequence
	StargateControl.clearDisplay();
	uint8_t direction = FORWARD;
	for (uint8_t i = 0; i < 7; ++i)
	{
		StargateControl.moveToSymbol(address[i], direction);
		StargateControl.lockChevron();
		delay(CHEVRON_ENGAGE_TIME_MS);
		StargateControl.unlockChevron();
		StargateControl.lightChevron(chevronLightOrder7Symbol[i]);

		if (direction == FORWARD)
			direction = BACKWARD;
		else
			direction = FORWARD;
	}

	StargateControl.fullDisplay();
	delay(30000);
	StargateControl.clearDisplay();
}

DialProgramClass DialProgram;

