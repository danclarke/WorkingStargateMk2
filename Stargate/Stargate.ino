/*
 Name:		Stargate.ino
 Created:	4/29/2018 8:08:51 PM
 Author:	dancl
*/

#include "StargateLogic.h"
#include "WebServer.h"
#include "StargateAudio.h"
#include "AnimationChase.h"
#include "DialProgram.h"
#include "StargateControl.h"

// Chevron light ordering, first = Bottom-left, last = Bottom-right
uint8_t chevronPositions[] = {3, 4, 5, 2, 7, 1, 8, 0, 6};

// Set ordering by running StargateControl.displaySymbolOrder()
// Then create a table like so:
//
//	0 8
//	1 6
//	2 4
//	3 1
//	4 2
//	5 3
//	6 9
//	7 5
//	8 7
//
// The first value is fixed, and is the array index. The second value is which light (numbered 1-9, from bottom-left) lit up in order
// So for me the first chevron that lit was 8, then 6, then 4, and so on
// Then in the chevronPositions[], find the chevron number from 1-9, and enter the array index to the left
// So for me chevron 1 (the first one) was at array index 3, so I paste 3 as the first value, then 4, then 5, then 2, and so on

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);

	// Init everything
	StargateControl.init();
	StargateAudio.init();
	StargateLogic.init();
	WebServer.init();

	// Run this FIRST to populate chevronPositions[] array
	//StargateControl.displaySymbolOrder();

	// Run this SECOND to verify population above
	//StargateControl.cycleChevrons();

	// Run this THIRD to get calibration values, update config.h, then only quick calibration neccesary
	//StargateControl.fullCalibration();

	// Run this NORMALLY to home the gate on start up
	StargateControl.quickCalibration();

	// Run this to TEST the audio playback
	//StargateAudio.playTheme();

	// Run this to TEST the dial sequence
	//uint8_t address[] = {26,6,14,31,11,29,0};
	//DialProgram.dial(address);
}

// the loop function runs over and over again until power down or reset
uint8_t loopCount = 0;
void loop()
{
	if (loopCount++ == 0)
	{
		Serial.print("Free Heap: ");
		Serial.println(ESP.getFreeHeap());
	}

	// Run logic at required interval
	uint32_t delayTime = StargateLogic.loop();
	delay(delayTime);
}
