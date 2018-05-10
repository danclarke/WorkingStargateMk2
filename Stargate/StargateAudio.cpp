#include "StargateAudio.h"
#include "AdafruitVS1053.h"
#include "Config.h"

#include "SPI.h"
#include "SD.h"

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(-1, AUDIO_MCS, AUDIO_DCS, AUDIO_DREQ, AUDIO_CCS);

void StargateAudioClass::init()
{
	Serial.println("SD Init");
	if (!SD.begin(AUDIO_CCS))
	{
		Serial.println("SD failed, or not present");
		while (true); 
	}

	Serial.println("Audio Init");
	if (!musicPlayer.begin())
	{
		Serial.println("Couldn't find VS1053, do you have the right pins defined?");
		while (true);
	}

	Serial.println("Interrupt init");
	if (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
	{
		Serial.println("Could not init audio interrupts");
		while (true);
	}

	Serial.println("Audio config");
	musicPlayer.setVolume(15, 15);

	Serial.println("Audio successfully initialised!");
}

void StargateAudioClass::playChevronLockEngage()
{
	musicPlayer.startPlayingFile("/chev1.mp3");
}

void StargateAudioClass::playChevronLockDisengage()
{
	musicPlayer.startPlayingFile("/chev2.mp3");
}

void StargateAudioClass::playOpenWormhole()
{
	musicPlayer.startPlayingFile("/open.mp3");
}

void StargateAudioClass::playCloseWormhole()
{
	musicPlayer.startPlayingFile("/close.mp3");
}

void StargateAudioClass::playRotating()
{
	musicPlayer.startPlayingFile("/roll.mp3");
}

void StargateAudioClass::playTheme()
{
	musicPlayer.startPlayingFile("/sg1thm.mp3");
}

bool StargateAudioClass::isPlaying()
{
	return !musicPlayer.stopped();
}

void StargateAudioClass::stop()
{
	if (!musicPlayer.stopped())
		musicPlayer.stopPlaying();
}


StargateAudioClass StargateAudio;
