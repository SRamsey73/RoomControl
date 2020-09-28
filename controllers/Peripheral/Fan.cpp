#include "Fan.h"

void Fan::setState(uint8_t state)
{
	//Check if state has changed
	if (this->state != state) {
		//Check if new state is valid
		if (state == OFF || state == ON) {
			//Updates to make if state is being changed to off
			if (state == OFF) {
				//Open state relay
				digitalWrite(devicePins[STATE_PIN], HIGH);
			}
			//Updates to make if state is bring changed to on
			else if (state == ON) {
				//Close state relay
				sendSerial("ceiling fan set to on");
				digitalWrite(devicePins[STATE_PIN], LOW);
			}

			//Set new state
			this->state = state;

			//Send new state over serial
			if (state == OFF) {
				sendCallString("state", "off");
			}
			else if (state == ON) {
				sendCallString("state", "on");
			}
		}
		else if (state == TOGGLE) {
			setState(!this->state);
			return;
		}
	}
}

void Fan::setState(const char* state)
{
	if (strcmp(state, "off") == 0) {
		setState(OFF);
	}
	else if(strcmp(state, "on") == 0) {
		setState(ON);
	}
	else if (strcmp(state, "toggle") == 0) {
		setState(TOGGLE);
		return;
	}
}

void Fan::setSpeed(uint8_t speed)
{
	//Check that speed is valid
	if (speed >= LOW_SPEED && speed <= HIGH_SPEED) {
		//Open all control relays
		digitalWrite(devicePins[LOW_PIN], HIGH);
		digitalWrite(devicePins[MEDIUM_PIN], HIGH);
		digitalWrite(devicePins[HIGH_PIN], HIGH);

		//Close relay corresponding with new speed
		digitalWrite(devicePins[speed], LOW);

		//Set new speed
		this->speed = speed;

		//Send new speed over serial
		if (speed == LOW_SPEED) {
			sendCallString("speed", "low");
		}
		else if (speed == MEDIUM_SPEED) {
			sendCallString("speed", "medium");
		}
		else if (speed == HIGH_SPEED) {
			sendCallString("speed", "high");
		}
	}
}

void Fan::setSpeed(const char* speed)
{
	//Resolve to function call
	if (strcmp(speed, "low") == 0) {
		setSpeed(LOW_SPEED);
	}
	else if (strcmp(speed, "medium") == 0) {
		setSpeed(MEDIUM_SPEED);
	}
	else if (strcmp(speed, "high") == 0) {
		setSpeed(HIGH_SPEED);
	}

}

void Fan::onUpdate(const unsigned long* elapsedTime)
{
}

bool Fan::callRemoteFunctionByIndex(size_t functionIndex, const char* param)
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

Fan::Fan(const char* name, const uint8_t fanType, const uint8_t devicePins[NUM_DEVICE_PINS]) : Peripheral::Peripheral(name, remoteFuncNames, numberOfRemoteFuncs)
{
	//Copy contents of devicePins
	for(unsigned int i = 0; i < NUM_DEVICE_PINS; i++)
		this->devicePins[i] = devicePins[i];

	//Set pinModes
	for(unsigned int i = 0; i < NUM_DEVICE_PINS; i++) {
		//Set device pins to output
		pinMode(this->devicePins[i], OUTPUT);
		//Open relays
		digitalWrite(this->devicePins[i], HIGH);
	}

	//Set invalid state and speed of fan to be reset with call to setters
	state = 10;
	speed = 10;
	//Set default states and speeds
	setState(OFF);
	setSpeed(LOW_SPEED);

	//setState(OFF);
	//setSpeed(MEDIUM_SPEED);
}
