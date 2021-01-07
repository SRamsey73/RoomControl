#include "LightSensor.h"

LightSensor::LightSensor(const char* name, uint8_t devicePin) : Peripheral::Peripheral(name) {
	//Set pin to read light sensor
	this->devicePin = devicePin;
	//Set starting value of lightLevel
	lightLevel = 0;
	//Set update countdown to 0 to update at next call to onUpdate
	updateCountdown = 0;
}

LightSensor::~LightSensor()
{

}

void LightSensor::onUpdate(unsigned long elapsedTime)
{
	//Decrement update countdown
	updateCountdown -= elapsedTime;

	//Check if time has elapsed to read new value
	if(updateCountdown <= 0) {
		//Reset read countdown
		updateCountdown = UPDATE_INTERVAL;

		//Read new value of light sensor
		int newLightLevel = analogRead(devicePin);
		
		//Check if new light level is great enough difference to update
		if (abs(lightLevel - newLightLevel) >= UPDATE_DIFFERENCE) {
			//Set light level
			lightLevel = newLightLevel;
			//Send new light level over serial
			sendCallString("light_level", lightLevel);
		}
	}
}
