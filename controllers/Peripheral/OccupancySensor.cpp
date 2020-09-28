#include "OccupancySensor.h"


OccupancySensor::OccupancySensor(const char* name, uint8_t devicePin) : Peripheral::Peripheral(name) {
	//Set device pin to read occupancy sensor
	this->devicePin = devicePin;
	//Set default value of occupied
	occupied = false;
}

OccupancySensor::~OccupancySensor()
{

}

void OccupancySensor::onUpdate(const unsigned long* elapsedTime)
{
	//Read occupancy sensor
	bool newOccupied = digitalRead(devicePin);

	//Check if occupancy has changed
	if (occupied != newOccupied) {
		//Occupancy changed update variable
		occupied = newOccupied;
		//Send new state over serial
		sendCallString("occupied", newOccupied ? "true" : "false");
	}
}
