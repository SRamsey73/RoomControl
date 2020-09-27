#include "Light.h"


void Light::setState(uint8_t state) {
	//Check if state has changed
	if (this->state != state) {
		//Verify that state is valid
		//OFF or ON states
		if (state == OFF || state == ON) {
			//Updates to make if state has been set to off
			if (state == OFF) {
				//Save the target brightness so it can be returned to when light is turned back on
				brightness[BRIGHTNESS_SAVED] = brightness[BRIGHTNESS_TARGET];
				//Set the target brightness to MIN to fade out light
				brightness[BRIGHTNESS_TARGET] = BRIGHTNESS_MIN;
			}
			//Updates to make if state has been set to on
			else if (state == ON) {
				//Set target to the saved brighness to return the light to its previous brightness
				brightness[BRIGHTNESS_TARGET] = brightness[BRIGHTNESS_SAVED];
			}

			this->state = state;

			//Send the new state of the light over the serial port
			if (state) {
				sendCallString("state", "on");
			}
			else {
				sendCallString("state", "off");
			}
		}
		//Check if toggle was passed
		else if (state == TOGGLE) {
			setState(!this->state);
			return;
		}
	}
	//State was invlalid
	else {
		return;
	}
}


void Light::setState(const char* state)
{
	if (strcmp(state, "off") == 0) {
		setState(OFF);
	}
	else if (strcmp(state, "on") == 0) {
		setState(ON);
	}
	else if (strcmp(state, "toggle") == 0) {
		setState(TOGGLE);
	}
}


void Light::setBrightness(uint8_t brightness) {
	//Check if brightness is within range
	if(brightness >= BRIGHTNESS_MIN && brightness <= BRIGHTNESS_MAX) {
		//Check light state to determine if brightness needs faded or can be set directly
		if (state == ON) {
			//Light is on so the transition needs faded
			this->brightness[BRIGHTNESS_TARGET] = brightness;
		}
		else if (state == OFF) {
			//Light is off so the transition does NOT need faded
			//Save the brightness to apply when light is turned back on
			this->brightness[BRIGHTNESS_SAVED] = brightness;
		}
		sendCallString("brightness", brightness);
	}
}


void Light::setBrightness(const char* brightness) {
	int intBrightness = atoi(brightness);
	if (intBrightness > BRIGHTNESS_MAX) {
		intBrightness = BRIGHTNESS_MAX;
	}
	else if (intBrightness < BRIGHTNESS_MIN) {
		intBrightness = BRIGHTNESS_MIN;
	}
	setBrightness(atoi(brightness) > BRIGHTNESS_MAX ? BRIGHTNESS_MAX : atoi(brightness));
}


void Light::onUpdate(const unsigned long* elapsedTime) //Elapsed time since last update in micros
{
	//Update brightness if light is dimmable
	if (deviceType == DIMMER_TYPE) {
		//Check if target is greater than actual
		if (brightness[BRIGHTNESS_TARGET] > brightness[BRIGHTNESS_ACTUAL]) {
			//Increase actual brightness to aproach target
			
			//Check if brightness is below on threshold to skip dead zone
			if (brightness[BRIGHTNESS_ACTUAL] < BRIGHTNESS_VISIBLE_MIN) {
				brightness[BRIGHTNESS_ACTUAL] = BRIGHTNESS_VISIBLE_MIN;
			}

			//Equation adjusts light brightness transition to reflect human perception of linear brightness changes
			brightness[BRIGHTNESS_ACTUAL] = pow((sqrt(brightness[BRIGHTNESS_ACTUAL] / BRIGHTNESS_MAX * BRIGHTNESS_TIME * BRIGHTNESS_TIME) + *elapsedTime / 1000.0) / BRIGHTNESS_TIME, 2) * BRIGHTNESS_MAX;
			if (brightness[BRIGHTNESS_TARGET] < brightness[BRIGHTNESS_ACTUAL]) {
				brightness[BRIGHTNESS_ACTUAL] = brightness[BRIGHTNESS_TARGET];
			}
			analogWrite(devicePin, brightness[BRIGHTNESS_ACTUAL] / BRIGHTNESS_MAX * 255);
		}
		//Check if target is less than actual
		else if (brightness[BRIGHTNESS_TARGET] < brightness[BRIGHTNESS_ACTUAL]) {
			//Decrease actual brightness to aproach target
			//Equation adjusts light brightness transition to reflect human perception of linear brightness changes
			brightness[BRIGHTNESS_ACTUAL] = pow((sqrt(brightness[BRIGHTNESS_ACTUAL] / BRIGHTNESS_MAX * BRIGHTNESS_TIME * BRIGHTNESS_TIME) - *elapsedTime / 1000.0) / BRIGHTNESS_TIME, 2) * BRIGHTNESS_MAX;
			if (abs(brightness[BRIGHTNESS_TARGET] - brightness[BRIGHTNESS_ACTUAL]) < .1) {
				brightness[BRIGHTNESS_ACTUAL] = brightness[BRIGHTNESS_TARGET];
			}
			analogWrite(devicePin, brightness[BRIGHTNESS_ACTUAL] / BRIGHTNESS_MAX * 255);
		}
	}
}


bool Light::callRemoteFunctionByIndex(size_t functionIndex, const char* param)
{
	//Check that function index is within range before calling function
	if (functionIndex <= sizeof(remoteFunctions) / sizeof(RemoteFunction)) {
		//Call to remote function
		((*this).*(remoteFunctions[functionIndex]))(param);
		//Call successful return true
		return true;
	}
	else {
		//Call failed return false
		return false;
	}
}


Light::Light(const char* name, const uint8_t deviceType, const uint8_t devicePin)
	: Peripheral::Peripheral(name, remoteFuncNames, numberOfRemoteFuncs)
{
	//Set type of light (either a dimmable light or one controlled with a relay)
	this->deviceType = deviceType;
	//Set pin the light is controlled with
	this->devicePin = devicePin;
	//Set pinModes
	pinMode(devicePin, OUTPUT);

	//Set starting state of light
	//Set invalid state of light to be updated by setter
	state = 10;

	//Set light brightness to starting values
	brightness[BRIGHTNESS_TARGET] = BRIGHTNESS_MAX;
	brightness[BRIGHTNESS_ACTUAL] = BRIGHTNESS_MIN;
	brightness[BRIGHTNESS_SAVED] = BRIGHTNESS_MAX;

	//Turn light off and set brightness to max
	setState(OFF);
	setBrightness(BRIGHTNESS_MAX);

}