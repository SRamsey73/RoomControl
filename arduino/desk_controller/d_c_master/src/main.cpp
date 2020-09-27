#include <Arduino.h>

#include <TemperatureSensor.h>
#include <LightSensor.h>
#include <OccupancySensor.h>

TemperatureSensor* roomTemperauteSensor;
LightSensor* roomLightSensor;
OccupancySensor* roomOccupancySensor;


//Read serial input buffer
void readSerialInputBuffer() {
	// Check if data is in serial buffer
  	if (Serial.available() > 0) {
      char msg[1024] = {};
      Serial.readBytesUntil('\4', msg, sizeof(msg));
      Peripheral::callRemoteFunction(msg);
    }
}


void setup() {
    //Open serial port
    Serial.begin(19200);
    delay(500);

    //Call constructors for sensors
    roomTemperauteSensor = new TemperatureSensor("temperature_sensor", A14);
    roomLightSensor = new LightSensor("light_sensor", A15);
    roomOccupancySensor = new OccupancySensor("occupancy_sensor", 45);

    Peripheral::sendSerial("Desk Arduino Initialized");
}


void loop() {
    readSerialInputBuffer();
    Peripheral::update();
    delay(1);
}