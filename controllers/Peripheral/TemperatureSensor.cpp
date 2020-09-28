#include "TemperatureSensor.h"


TemperatureSensor::TemperatureSensor(const char* name, uint8_t devicePin) : Peripheral::Peripheral(name) {
	//Set pin to read temperature sensor
	this->devicePin = devicePin;
	//Set starting value of temperature
	temperature = 0;
	//Set update countdown to 0 to update at next call to onUpdate
	updateCountdown = 0;
}

TemperatureSensor::~TemperatureSensor()
{

}

void TemperatureSensor::onUpdate(const unsigned long* elapsedTime)
{
	//Decrement update countdown, dived micros by 1000 to store as millis
	updateCountdown -= *elapsedTime / 1000;

	//Check if time has elapsed to read new value
	if(updateCountdown <= 0) {
		//Reset read countdown
		updateCountdown = UPDATE_INTERVAL * 1000;

		//Voltage read from thermistor
		int Vo;
		//Temperature calculation constants and variables for intermediate steps
		const float R1 = 50000;
		float logR2, R2, T;
		const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

		//Read value of thermistor
		Vo = analogRead(devicePin);

		//Calculate temperature
		R2 = R1 * (1023.0 / (float)Vo - 1.0);
		logR2 = log(R2);
		T = (1.0 / (c1 + c2 * logR2 + c3 * logR2*logR2*logR2));
		T = T - 273.15;
		float newTemp = (T * 9.0) / 5.0 + 32.0;

		//Check if newTemp is great enough difference to update temperature
		if (abs((int)temperature - (int)newTemp) >= UPDATE_DIFFERENCE) {
			//Set ambient temp to the new temp
			temperature = newTemp;
			//Send new temp over serial
			sendCallString("temperature", (int)temperature);
		}
	}
}
