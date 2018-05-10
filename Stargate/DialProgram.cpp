#include "DialProgram.h"

#include "StargateControl.h"
#include "StargateAudio.h"

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
		StargateAudio.playRotating();
		delay(AUDIO_PLAY_DELAY_MS); // MP3s have a little lag in playback, so compensate for it with a delay
		StargateControl.moveToSymbol(address[i], direction);

		StargateAudio.stop();
		StargateAudio.playChevronLockEngage();
		delay(AUDIO_PLAY_DELAY_MS);
		StargateControl.lockChevron();
		delay(CHEVRON_ENGAGE_TIME_MS);

		StargateAudio.stop();
		StargateAudio.playChevronLockDisengage();
		delay(AUDIO_PLAY_DELAY_MS);
		StargateControl.unlockChevron();
		StargateControl.lightChevron(chevronLightOrder7Symbol[i]);

		if (direction == FORWARD)
			direction = BACKWARD;
		else
			direction = FORWARD;

		while (StargateAudio.isPlaying())
			delay(1);
	}

	StargateAudio.playOpenWormhole();
	StargateControl.fullDisplay();
	
	while (StargateAudio.isPlaying())
		delay(100);

	StargateAudio.playTheme();
	delay(57000);

	StargateAudio.playCloseWormhole();
	StargateControl.clearDisplay();
}

DialProgramClass DialProgram;
