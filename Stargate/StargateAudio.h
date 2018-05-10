// StargateAudio.h

#ifndef _STARGATEAUDIO_h
#define _STARGATEAUDIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class StargateAudioClass
{
public:
	void init();

	void playChevronLockEngage();
	void playChevronLockDisengage();
	void playOpenWormhole();
	void playCloseWormhole();
	void playRotating();
	void playTheme();

	bool isPlaying();
	void stop();
};

extern StargateAudioClass StargateAudio;

#endif

