#pragma once
#include "Peripheral.h"
class OccupancySensor :
    public Peripheral
{
/***PINS***/
private:
    //Pin to interact with temperature sensor
    uint8_t devicePin;



/***OCCUPANCY VARIABLES***/
    bool occupied;



/**CONSTRUCTOR / DESTRUCTOR**/
public:
    OccupancySensor(const char* name, uint8_t devicePin);
    ~OccupancySensor();



/***UPDATE***/
private:
    void onUpdate(const unsigned long* elapsedTime) override;
};

