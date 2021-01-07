// ArduinoPeripheralManagement.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Arduino.h>

#include "Light.h"
#include "Fan.h"


Light* overheadLight;
Fan* ceilingFan;

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
    Serial.begin(19200);
    delay(500);
    //Object to control overhead light
    overheadLight = new Light("overhead_light", Light::DIMMER_TYPE, 11);

    //Object to control ceiling fan
    uint8_t fanPins[4] = { 6, 9, 8, 7 };
    ceilingFan = new Fan("ceiling_fan", Fan::MULTI_SPEED_TYPE, fanPins);

    Peripheral::sendSerial("Basement Arduino Initialized");
    delay(200);
}

void loop() {
    readSerialInputBuffer();
    Peripheral::update();
    delay(2);
}

