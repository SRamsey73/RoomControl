#pragma once
#include "Peripheral.h"
#include <map>

#define LED_TYPE WS2812B

class LEDStrip : public Peripheral {

/***STRIP STATE***/
//Stores the state of the LEDStrip: ON/OFF
private:
    uint8_t state;
public:
    enum {
        OFF,
        ON,
        TOGGLE //Passed to setState to toggle state of strip
    };
    void setState(uint8_t state);
    void setState(const char* state);



/***STRIP INTERACTIONS***/
protected:
    //Physical LED strip
    int numLEDs;
    //CRGB* strip;

    //Animations
protected:

    struct Animation {
        long timeRemaining = 0;
        unsigned int updateInterval = 20 * 1000;
        int loopCount = 0;
        bool firstRun = false;
        Animation();
        Animation(unsigned int updateInterval);
        bool makeFirstRun();
        virtual void updateAnimation() = 0;
    };

    typedef std::map<const char*, Animation*> AnimationMap;

private:
    AnimationMap animationMap;
    Animation* currentAnimation = nullptr;
protected:
    void registerAnimations(AnimationMap& animationNameMap);

public:
    void setAnimation(Animation* animation);
    void setAnimation(const char* animationName);


/***REMOTE FUNCTIONS***/
private:
    RemoteFunctionMap ledRemoteFunctionMap = {
        { "state", static_cast<RemoteFunction>(&LEDStrip::setState) },
        { "animation", static_cast<RemoteFunction>(&LEDStrip::setAnimation) }
    };


/***UPDATE***/
private:
    void onUpdate(unsigned long elapsedTime) override;



/***CONSTRUCTOR / DESTRUCTOR***/
public:
    LEDStrip(const char* name, int numLEDs);
    ~LEDStrip();
};