#ifndef _CONFIG_H
#define _CONFIG_H

#include <Adafruit_MotorShield.h>

// Pins for various things
#define PIN_DATA      27
#define PIN_LATCH     14
#define PIN_CLOCK     12
#define PIN_CAL_LED   13
#define PIN_CAL_LDR   2

// Pins for audio shield
#define AUDIO_DREQ		26
#define AUDIO_CCS		25
#define AUDIO_DCS		17
#define AUDIO_MCS		16

// Motor 'number' on the Adafruit shield
#define STEPPER_GATE    1
#define STEPPER_CHEVRON 2

// Motor control settings
#define MOTOR_RPM	60
#define MOTOR_DRIVE	INTERLEAVE // Depending on rpm, SINGLE, DOUBLE, or INTERLEAVE will result in the smoothest / quietest operation

// Offset in the shift register value where the gantry LED is connected
// Most likely this is the first position after the 9 chevrons, so '9' due to 0-index of array
#define GANTRY_OFFSET   9

// Calibration settings
#define STEPS_CHEVRON_LOCK			15			// Number of steps on motor to move top chevron
#define STEPS_PER_SYMBOL			30.77f		// Number of steps per symbol, unused if a full calibration is performed. Take the full calibration value and paste it in here
#define CALIBRATION_PERCENTAGE		150			// How much brighter the calibration point must be at a minimum above normal background lighting
#define NUM_CALIBRATION_SAMPLES		10			// Number of initial calibration light readings to take to get a background lighting value
#define STEPS_CALIBRATION_SAMPLE	50			// Number of steps to move symbol ring when gathering average lighting
#define CALIBRATION_BRIGHTNESS		1000		// Default brightness for calibration, unused if a full calibration is performed. Take the full calibration value and paste it in here
#define STARGATE_FORWARD			BACKWARD	// 'Forward' direction of gate, relative to motor
#define STARGATE_BACKWARD			FORWARD		// 'Backward' direction of gate, relative to motor
#define CHEVRON_ENGAGE				FORWARD		// Motor direction of chevron to 'engage' the chevron
#define CHEVRON_DISENGAGE			BACKWARD	// Motor direction of chevron to 'disengage' the chevron
#define CHEVRON_ENGAGE_TIME_MS		1500		// Number of milliseconds chevron should be locked before moving on
#define HOME_INITIAL_REVERSE		STEPS_PER_SYMBOL * 2	// Initial homing 'backward' movement to see if the position has just gone over slightly

// Audio settings
#define AUDIO_PLAY_DELAY_MS		200	// How many ms to wait between request of audio and performing the next action to compensate for playback delay

// Constants for more readable code
#define NUM_SYMBOLS			39
#define NUM_CHEVRONS		9
#define NUM_STEPS_CIRCLE	1200
#define TOP_CHEVRON			4

// Chevron light positioning - set in Stargate.ino
extern uint8_t chevronPositions[];

#endif