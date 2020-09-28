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
    //Interval in seconds to update sensor value
    const float UPDATE_INTERVAL = 1;
    //Stores time remaining to next update
    int updateCountdown;



/**CONSTRUCTOR / DESTRUCTOR**/
public:
    LightSensor(const char* name, uint8_t devicePin);
    ~LightSensor();



/***UPDATE***/
private:
    void onUpdate(const unsigned long* elapsedTime) override;
};

