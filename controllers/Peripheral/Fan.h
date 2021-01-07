#pragma once
#include "Peripheral.h"

class Fan :
    public Peripheral
{

/***PINS and DEVICE TYPE***/
private:
    //Type of fan: MULTI_SPEED_TYPE or RELAY_TYPE
    uint8_t deviceType;
    //Pins to interact with light
    static const size_t NUM_DEVICE_PINS = 4;
    uint8_t devicePins[NUM_DEVICE_PINS]; //4 pins possible for fan STATE_PIN, LOW, MEDIUM, HIGH, RELAYS
    enum {
        STATE_PIN,
        LOW_PIN = 1,
        MEDIUM_PIN,
        HIGH_PIN
    };
public:
    //Fan Types
    enum FanTypes {
        RELAY_TYPE,
        MULTI_SPEED_TYPE
    };



/***STATE VARIABLES***/
private:
    //Stores the state of the fan ON/OFF
    uint8_t state;
public:
    //Possible states
    enum {
        OFF,
        ON,
        TOGGLE //Passed to set state to toggle state of fan
    };
    void setState(uint8_t state);
    void setState(const char* state);

private:
    //Stores speed of the fan
    uint8_t speed;
public:
    //Speeds
    enum {
        LOW_SPEED = LOW_PIN, //Match numbering assigned to devicePins
        MEDIUM_SPEED,
        HIGH_SPEED
    };
    void setSpeed(uint8_t speed);
    void setSpeed(const char* speed);


private:
    RemoteFunctionMap fanRemoteFunctionMap = { 
        { "state", static_cast<RemoteFunction>(&Fan::setState) },
        { "speed", static_cast<RemoteFunction>(&Fan::setSpeed) }
     };

/***UPDATE***/
private:
    void onUpdate(unsigned long elapsedTime) override;


/***CONSTRUCTOR / DESTRUCTOR***/
public:
    Fan(const char* name, const uint8_t fanType, const uint8_t devicePins[4]);
};

