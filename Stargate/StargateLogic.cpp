#include "StargateLogic.h"
#include "DialProgram.h"
#include "AnimationChase.h"
#include "AnimationRingLights.h"
#include "cJSON.h"

#define STATE_ANIM_CHASE	0
#define STATE_ANIM_RING		1
#define STATE_ANIM_DIAL		2

void StargateLogicClass::init()
{
	_state = STATE_ANIM_CHASE;
	_loopLock = xSemaphoreCreateBinary();
	xSemaphoreGive(_loopLock);
}

uint32_t StargateLogicClass::loop()
{
	uint8_t state;

	if (xSemaphoreTake(_loopLock, 5000 / portTICK_PERIOD_MS) == pdFALSE)
		state = STATE_ANIM_CHASE;
	else
	{
		state = _state;
		xSemaphoreGive(_loopLock);
	}

	switch (state)
	{
	case STATE_ANIM_CHASE:
		return AnimationChase.animate();

	case STATE_ANIM_RING:
		return AnimationRingLights.animate();

	case STATE_ANIM_DIAL:
		DialProgram.dial(_address);
		_state = STATE_ANIM_CHASE;
		return 100;
	}

	return 1000;
}

void StargateLogicClass::processRequest(const char* jsonStr, size_t jsonStrLen)
{
	if (xSemaphoreTake(_loopLock, 5000 / portTICK_PERIOD_MS) == pdFALSE)
		return;

	// Load in JSON for easy parsing
	cJSON *json = cJSON_Parse(jsonStr);
	if (json == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL)
			Serial.printf("Error before: %s\r\n", error_ptr);
		else
			Serial.println("Could not parse JSON");

		xSemaphoreGive(_loopLock);
		return;
	}

	const cJSON *val = cJSON_GetObjectItemCaseSensitive(json, "anim");
	if (cJSON_IsNumber(val))
		_state = val->valueint;
	else
	{
		xSemaphoreGive(_loopLock);
		return;
	}

	// Only dial has multiple args
	if (_state != 2)
	{
		xSemaphoreGive(_loopLock);
		return;
	}

	// Read in dial sequence and get ready for dial sequence
	const cJSON *sequence = cJSON_GetObjectItemCaseSensitive(json, "sequence");
	if (!cJSON_IsArray(sequence))
	{
		_state = 0;
		xSemaphoreGive(_loopLock);
		return;
	}

	for (uint8_t i = 0; i < 7; ++i)
	{
		const cJSON *item = cJSON_GetArrayItem(sequence, i);
		if (item == NULL || !cJSON_IsNumber(item))
		{
			_state = 0;
			xSemaphoreGive(_loopLock);
			return;
		}

		_address[i] = item->valueint;
		Serial.printf("Chevron: %i\r\n", _address[i]);
	}	

	xSemaphoreGive(_loopLock);
}

StargateLogicClass StargateLogic;
