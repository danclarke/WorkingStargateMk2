// StargateControl.h

#ifndef _STARGATECONTROL_h
#define _STARGATECONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "config.h"
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

class StargateControlClass
{
 public:
	StargateControlClass();
	void init();
	void fullCalibration();
	void quickCalibration();
	void displaySymbolOrder();
	void cycleChevrons();

	void lightChevron(uint8_t index, bool exclusive = false);
	void darkChevron(uint8_t index) { bitClear(_ledState, chevronPositions[index]); displayLeds(); }
	void clearDisplay() { _ledState = 0; displayLeds(); }
	void fullDisplay() { _ledState = 0xFFFF; displayLeds(); }

	void displayProgress(uint32_t step, uint32_t max);
	void displayProgress(float percent);

	void lockChevron(bool lighting = true);
	void unlockChevron(bool lighting = true);

	bool isAtHome();
	void moveToHome();
	void moveToSymbol(uint8_t index, uint8_t direction);

protected:
	Adafruit_MotorShield _motorShield;
	Adafruit_StepperMotor *_motorGate;
	Adafruit_StepperMotor *_motorChevron;

	uint16_t _ledState;
	uint8_t _currentSymbol;

	uint16_t getLdrValue();
	void displayLeds();

	uint16_t getTargetCalibrationBrightness();

private:
	float _stepPerSymbol;
	uint16_t _calibrationBrightness;
};

extern StargateControlClass StargateControl;

#endif

