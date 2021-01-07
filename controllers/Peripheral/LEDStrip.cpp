#include "LEDStrip.h"


void LEDStrip::setState(uint8_t state) {
	if(this->state != state) {
		if(state == OFF) {
			this->state = OFF;
			clearStrip();
		}
		else if(state == ON) {
			this->state = ON;
			currentAnimation->firstRun = true;
		}
		else if(state == TOGGLE) {
			setState(!this->state);
		}
	}
}


void LEDStrip::setState(const char* state)
{
	if (strcmp(state, "off") == 0) {
		setState(OFF);
	}
	else if (strcmp(state, "on") == 0) {
		setState(ON);
	}
	else if (strcmp(state, "toggle") == 0) {
		setState(TOGGLE);
	}
}


void LEDStrip::clearStrip() {
	for (int i = 0; i < numLEDs; ++i) {
   		strip[i] = CRGB::Black;
  	}
  	FastLED.show();
}


void LEDStrip::registerAnimations(AnimationMap& animationMap)
{
	this->animationMap = animationMap;
	setAnimation(animationMap.begin()->second);
}

void LEDStrip::setAnimation(Animation* animation) {
	currentAnimation = animation;
	currentAnimation->firstRun = true;
}


void LEDStrip::setAnimation(const char* animationName) {
	//Lowercase string
	char* lowerAnimationName = new char[strlen(animationName) + 1]{ 0 };
	for (unsigned int i = 0; i < strlen(animationName) + 1; i++)
		*(lowerAnimationName + i) = *(animationName + i);

	for (auto& item : animationMap) {
		if (strcmp(item.first, lowerAnimationName) == 0 && currentAnimation != item.second) {
			setAnimation(item.second);
			sendCallString("animation", item.first);
		}
	}
	
	delete[] lowerAnimationName;
}


void LEDStrip::onUpdate(unsigned long elapsedTime) {
	if(state) {
		//Call the function that is currently animating the LED strip when a sufficient amount of time has elapsed
		if (currentAnimation != nullptr) {

			//Make first run will set timing variables back to their inital values
			currentAnimation->makeFirstRun();
			currentAnimation->timeRemaining = currentAnimation->timeRemaining - elapsedTime;

			//Check if enough time has elapsed to update the current animation
			if (currentAnimation->timeRemaining <= 0) {
				currentAnimation->updateAnimation();
				currentAnimation->timeRemaining = currentAnimation->updateInterval;
				//Show the changes
				FastLED.show();
			}

			//First run is not updated until the animation has had a chance to run, update it here
			currentAnimation->firstRun = false;

		}
	}
}

LEDStrip::LEDStrip(const char* name, CRGB* strip, int numLEDs, AnimationMap animationMap) : Peripheral(name), strip(strip), numLEDs(numLEDs) {
	//Call setState to properly initialze LED strip
	state = OFF;
	setState(ON);

	registerAnimations(animationMap);
	registerRemoteFunctions(ledRemoteFunctionMap);

	FastLED.setBrightness(200);
    //Blink Border LEDS
    for (int i = 0; i < numLEDs; ++i) {
        strip[i] = CRGB::Green;
    }
    FastLED.show();
    delay(300);
    for (int i = 0; i < numLEDs; ++i) {
        strip[i] = CRGB::Black;
    }
    FastLED.show();
}

LEDStrip::~LEDStrip() {
	for (auto& item : animationMap) {
		delete item.second;
	}
}


LEDStrip::Animation::Animation(CRGB* strip, int numLEDs) : strip(strip), numLEDs(numLEDs) {}

LEDStrip::Animation::Animation(CRGB* strip, int numLEDs, unsigned int updateInterval) : strip(strip), numLEDs(numLEDs), updateInterval(updateInterval) {}

bool LEDStrip::Animation::makeFirstRun()
{
	//firstRun is not updated because it cannot be done until the animation has had a chance to update
	//This allows user defined animations to perform their own first run initializations

	if (firstRun) {
		timeRemaining = 0;
		loopCount = 0;
		//Clear strip
		for (int i = 0; i < numLEDs; ++i) strip[i] = CRGB::Black;
	}
	return firstRun;
}
