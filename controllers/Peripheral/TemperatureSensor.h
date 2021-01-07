#pragma once
#include "Peripheral.h"

class TemperatureSensor :
    public Peripheral
{
/***PINS***/
private:
    //Pin to interact with temperature sensor
    uint8_t devicePin;



/*TEMPERATURE CALC CONSTANTS*/
private:
    const int R1 = 50000;
    const double c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;



/***TEMPERATURE VARIABLES***/
    //Temperature difference requried to update temperature
    const float UPDATE_DIFFERENCE = 1;
    double temperature;



/***UPDATE INTERVAL***/
    //Interval in milliseconds to update sensor value
    const long UPDATE_INTERVAL = 60000L;
    //Stores time remaining to next update
    long updateCountdown;



/**CONSTRUCTOR / DESTRUCTOR**/
public:
    TemperatureSensor(const char* name, uint8_t devicePin);
    ~TemperatureSensor();



/***UPDATE***/
private:
    void onUpdate(unsigned long elapsedTime) override;
};

