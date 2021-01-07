#include <Arduino.h>

#include <TemperatureSensor.h>
#include <LightSensor.h>
#include <OccupancySensor.h>
#include <LEDStrip.h>

//Defines led strip animations in a separte file to keep main tidy
#include "DeskAnimations.h"

TemperatureSensor* roomTemperauteSensor;
LightSensor* roomLightSensor;
OccupancySensor* roomOccupancySensor;

const int numLEDs = 90;
CRGB* strip = new CRGB[numLEDs];
LEDStrip* deskLEDStrip;


//Read serial input buffer
void readSerialInputBuffer() {
	// Check if data is in serial buffer
  	if (Serial.available() > 0) {
      char msg[1024] = {};
      Serial.readBytesUntil('\4', msg, sizeof(msg) - 1);
      Peripheral::callRemoteFunction(msg);
    }
}


void setup() {
    //Open serial port
    Serial.begin(19200);
    delay(500);

    //Constructors for sensors
    roomTemperauteSensor = new TemperatureSensor("temperature_sensor", A14);
    roomLightSensor = new LightSensor("light_sensor", A15);
    roomOccupancySensor = new OccupancySensor("occupancy_sensor", 45);

    //Create LED strip
    FastLED.addLeds<WS2812B,47,GRB>(strip, numLEDs).setCorrection(TypicalLEDStrip); // initializes strip
    //Create object to interact with strip
    deskLEDStrip = new LEDStrip("desk_leds", strip, numLEDs, {
        { "tail_effect", new TailEffect(strip, numLEDs) },
        { "rwb_center", new RWBCenter(strip, numLEDs) }
    });

    Peripheral::sendSerial("Desk Arduino Initialized");
    delay(200);
}


void loop() {
    readSerialInputBuffer();
    Peripheral::update();
    delay(2);
}