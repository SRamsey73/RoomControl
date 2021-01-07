#pragma once
#include "Peripheral.h"
class LightSensor :
    public Peripheral
{
/***PINS***/
private:
    //Pin to interact with temperature sensor
    uint8_t devicePin;



/***TEMPERATURE VARIABLES***/
private:
    //Light level read by sensor
    int lightLevel;
    //Difference to update light level
    const int UPDATE_DIFFERENCE = 75;



/***UPDATE INTERVAL***/
    //Interval in milliseconds to update sensor value
    const long UPDATE_INTERVAL = 1000L;
    //Stores time remaining to next update
    long updateCountdown;



/**CONSTRUCTOR / DESTRUCTOR**/
public:
    LightSensor(const char* name, uint8_t devicePin);
    ~LightSensor();



/***UPDATE***/
private:
    void onUpdate(unsigned long elapsedTime) override;
};

