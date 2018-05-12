#include <Wire.h>

#include "config.h"
#include "StargateControl.h"

// Override for I2C bus speed for higher motor rpms
#define I2C_SPEED	400000L // Can be 500khz

StargateControlClass::StargateControlClass()
	: _ledState(0), _currentSymbol(0), _stepPerSymbol(STEPS_PER_SYMBOL), _calibrationBrightness(CALIBRATION_BRIGHTNESS)
{
	_motorShield = Adafruit_MotorShield();
	_motorGate = _motorShield.getStepper(200, STEPPER_GATE);
	_motorChevron = _motorShield.getStepper(200, STEPPER_CHEVRON);
}

void StargateControlClass::init()
{
	// Init motors
	Wire.begin();//(-1, -1, I2C_SPEED);
	_motorShield.begin();  // create with the default frequency 1.6KHz
	//Wire.setClock(I2C_SPEED);
  
	_motorGate->setSpeed(MOTOR_RPM);
	_motorChevron->setSpeed(MOTOR_RPM);

	// Analog config for ESP32
	analogReadResolution(12); //12 bits
    analogSetAttenuation(ADC_11db);  //For all pins

	// Init general pins
	pinMode(PIN_DATA, OUTPUT);
	pinMode(PIN_LATCH, OUTPUT);
	pinMode(PIN_CLOCK, OUTPUT);
	pinMode(PIN_CAL_LED, OUTPUT);
	pinMode(PIN_CAL_LDR, INPUT);
}

uint16_t StargateControlClass::getLdrValue()
{
	uint16_t val;

	// analogRead() likes to return 4095 from time to time, which is obviously invalid
	// so keep retrying until something sensible arrives
	do 
	{
		val = analogRead(PIN_CAL_LDR);
	} while (val == 4095);

	return val;
}

void StargateControlClass::fullCalibration()
{
	Serial.println("Full Calibration");

	// Calibrate bright probe value
	_calibrationBrightness = getTargetCalibrationBrightness();
	delay(500);
	clearDisplay();

	// Move to starting position so we can start counting
	moveToHome();
	delay(500);
	clearDisplay();

	// Go through entire ring, count how many steps
	digitalWrite(PIN_CAL_LED, HIGH);

	// Initially move nicely away from the calibration point
	_motorGate->step(5, STARGATE_FORWARD, MICROSTEP);
	uint32_t step = 5;

	while (getLdrValue() < _calibrationBrightness)
	{
		_motorGate->step(1, STARGATE_FORWARD, MICROSTEP);
		displayProgress(++step, NUM_STEPS_CIRCLE);
	}

	// Big error, don't continue to prevent damage
	if (step < 20)
	{
		while (true)
		{
			Serial.printf("ERROR!!! TOO FEW STEPS DETECTED: %i\r\n", step);

			clearDisplay();
			delay(250);
			fullDisplay();
			delay(250);
		}
	}

	// Success, log the data
	fullDisplay();
	_stepPerSymbol = (float)step / (float)NUM_SYMBOLS;
	Serial.printf("Num steps: %i\r\n", step);
	Serial.printf("Steps per symbol: %.2f\r\n", _stepPerSymbol);
	digitalWrite(PIN_CAL_LED, LOW);
	_motorGate->release();
	_currentSymbol = 0;

	delay(500);
	clearDisplay();
}

void StargateControlClass::quickCalibration()
{
	Serial.println("Quick Calibration");

	clearDisplay();
	moveToHome();
}

void StargateControlClass::displaySymbolOrder()
{
	Serial.println("Displaying chevrons in wire-order");

	clearDisplay();

	for (uint8_t i = 0; i < NUM_CHEVRONS; ++i)
	{
		_ledState = (0x1 << i);
		displayLeds();
		delay(5000);
	}

	clearDisplay();
}

void StargateControlClass::cycleChevrons()
{
	Serial.println("Displaying chevrons in configured-order");

	clearDisplay();

	for (uint8_t i = 0; i < NUM_CHEVRONS; ++i)
	{
		lightChevron(i, true);
		delay(1000);
	}

	clearDisplay();
}

void StargateControlClass::lightChevron(uint8_t index, bool exclusive)
{
	if (!exclusive)
		bitSet(_ledState, chevronPositions[index]); 
	else
		_ledState = (0x1 << chevronPositions[index]);
	
	displayLeds();
}

void StargateControlClass::displayLeds()
{
	digitalWrite(PIN_LATCH, LOW);
	shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, highByte(_ledState));
	shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, lowByte(_ledState));
	digitalWrite(PIN_LATCH, HIGH);
}

void StargateControlClass::displayProgress(uint32_t step, uint32_t max)
{
	float progress = (float)step / (float)max;
	displayProgress(progress);
}

void StargateControlClass::displayProgress(float percent)
{
	if (percent > 1.0f)
		percent = 1.0f;

	uint8_t numLeds = roundf(percent * 9.0f);

	_ledState = 0;
	for (uint8_t i = 0; i < numLeds; ++i)
		bitSet(_ledState, chevronPositions[i]);

	displayLeds();
}

void StargateControlClass::lockChevron(bool lighting)
{
	if (lighting)
	{
		lightChevron(TOP_CHEVRON);
		displayLeds();
	}

	_motorChevron->step(STEPS_CHEVRON_LOCK, CHEVRON_ENGAGE, MOTOR_DRIVE);
	_motorChevron->release();
}

void StargateControlClass::unlockChevron(bool lighting)
{
	if (lighting)
	{
		darkChevron(TOP_CHEVRON);
		displayLeds();
	}

	_motorChevron->step(STEPS_CHEVRON_LOCK, CHEVRON_DISENGAGE, MOTOR_DRIVE);
	_motorChevron->release();
}

bool StargateControlClass::isAtHome()
{
	bool atHome;

	digitalWrite(PIN_CAL_LED, HIGH);
	atHome = getLdrValue() > _calibrationBrightness;
	digitalWrite(PIN_CAL_LED, LOW);

	return atHome;
}

void StargateControlClass::moveToHome()
{
	Serial.println("Moving to home");
	digitalWrite(PIN_CAL_LED, HIGH);

	// Initially move a few symbols 'backward' to see if we've got a small overrun
	uint16_t currentStep = 0;
	bool found = false;
	bool motorActive = false;
	Serial.printf("LDR: %i, Threshold: %i\r\n", getLdrValue(), _calibrationBrightness);

	while (getLdrValue() < _calibrationBrightness)
	{
		_motorGate->step(1, STARGATE_BACKWARD, MICROSTEP);
		motorActive = true;
		if (++currentStep > HOME_INITIAL_REVERSE)
		{
			// Move motor quickly back to where we started, more or less
			_motorGate->step(HOME_INITIAL_REVERSE - 2, STARGATE_FORWARD, MOTOR_DRIVE);
			break;
		}
	}

	// If previous find worked, LDR will still be lit up and this loop won't execute
	// Otherwise scan the entire gate
	currentStep = 0;
	while (getLdrValue() < _calibrationBrightness)
	{
		_motorGate->step(1, STARGATE_FORWARD, MICROSTEP);
		displayProgress(++currentStep, NUM_STEPS_CIRCLE);
		motorActive = true;
	}

	if (motorActive)
		_motorGate->release();
	clearDisplay();
	digitalWrite(PIN_CAL_LED, LOW);

	_currentSymbol = 0;
}

void StargateControlClass::moveToSymbol(uint8_t index, uint8_t direction)
{
	int16_t delta;
	uint8_t motorDirection;

	if (direction == FORWARD)
	{
		motorDirection = STARGATE_FORWARD;
		delta = index - _currentSymbol;
		if (delta < 0)
			delta += NUM_SYMBOLS;
	}
	else
	{
		motorDirection = STARGATE_BACKWARD;
		delta = _currentSymbol - index;
		if (delta < 0)
			delta += NUM_SYMBOLS;
	}

	uint16_t numSteps = (uint16_t)roundf(delta * _stepPerSymbol);

	Serial.printf("Index: %i, Direction: %i\r\n", index, direction);
	Serial.printf("Delta: %i\r\n", delta);
	Serial.printf("Num steps: %i\r\n", numSteps);
	
	_currentSymbol = index;
	_motorGate->step(numSteps, motorDirection, MOTOR_DRIVE);
	_motorGate->release();
}

uint16_t StargateControlClass::getTargetCalibrationBrightness()
{
	digitalWrite(PIN_CAL_LED, HIGH);

	// First get the target brightness for the calibration point
	uint32_t brightness = getLdrValue();
	Serial.printf("Read in: %i\r\n", brightness);
	delay(2000);

	for (uint8_t i = 0; i < NUM_CALIBRATION_SAMPLES; ++i)
	{
		displayProgress(i, NUM_CALIBRATION_SAMPLES);
		uint16_t val = getLdrValue();
		Serial.printf("Read in: %i\r\n", val);

		brightness += val;
		_motorGate->step(STEPS_CALIBRATION_SAMPLE, STARGATE_BACKWARD, MOTOR_DRIVE);
	}

	fullDisplay();
	digitalWrite(PIN_CAL_LED, LOW);

	uint16_t average = brightness / NUM_CALIBRATION_SAMPLES;
	Serial.printf("Average: %i\r\n", average);

	uint16_t targetVal = average + ((average / 100) * CALIBRATION_PERCENTAGE);
	Serial.printf("Target val: %i\r\n", targetVal);

	_motorGate->release();

	return targetVal;
}

StargateControlClass StargateControl;

