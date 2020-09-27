#pragma once
#include "Peripheral.h"
#include <math.h>

class Light :
    public Peripheral
{

/***PINS and DEVICE TYPE***/
private:
    //Type of light: DIMMER_TYPE or RELAY_TYPE
    uint8_t deviceType;
    //Pin to interact with light
    uint8_t devicePin;
public:
    //Light Type
    enum LightTypes {
        RELAY_TYPE,
        DIMMER_TYPE
    };



/***STATE VARIABLES***/
private:
    //Stores the state of the light
    uint8_t state;
public:
    //Possible states
    enum {
        OFF,
        ON,
        TOGGLE //Passed to set state to toggle state of light
    };
    //Getters Setters
    void setState(uint8_t state);
    void setState(const char* state);

private:
    //Stores the brightness of the light
    double brightness[3];
    //Enum to specify array index for each brightness value
    enum BrightnessValueIndices {
        BRIGHTNESS_ACTUAL, //Always matches brightness of light
        BRIGHTNESS_TARGET, //Brightness light is targeted to reach
        BRIGHTNESS_SAVED,  //Used to save a value to return to when light is off and turned back on
    };
    //Operating range of the light
    const uint8_t BRIGHTNESS_MAX = 100;
    const uint8_t BRIGHTNESS_MIN = 0;
    //Dimmable light minimum display threshold
    const unsigned int BRIGHTNESS_VISIBLE_MIN = BRIGHTNESS_MAX * 0.25;
    //Time to fade light from min to max in millis
    const unsigned int BRIGHTNESS_TIME = 2000;
    //Getters Setters
    void setBrightness(uint8_t brightness);
    void setBrightness(const char* brightness);



/***REMOTE FUNCTIONS***/
private:
    //Typedef for a pointer to a remote function
    typedef void (Light::* RemoteFunction)(const char*);
    //Arrays to store names and pointers to remote functions
    static const size_t numberOfRemoteFuncs = 2;
    const char* remoteFuncNames[numberOfRemoteFuncs] = { "state", "brightness" };
    const RemoteFunction remoteFunctions[numberOfRemoteFuncs] = { &Light::setState, &Light::setBrightness };
    //Call a remote function in a derived class from its index
    bool callRemoteFunctionByIndex(size_t functionIndex, const char* param) override;



/***UPDATE***/
private:
    void onUpdate(const unsigned long* elapsedTime) override;



/***CONSTRUCTOR / DESTRUCTOR***/
public:
    Light(const char* name, const uint8_t lightType, const uint8_t devicePin);
};

