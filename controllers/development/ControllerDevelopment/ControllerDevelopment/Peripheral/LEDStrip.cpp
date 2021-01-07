#include "LEDStrip.h"


void LEDStrip::setState(uint8_t state) {
    this->state = state;
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
		}
	}
	

	delete[] lowerAnimationName;
}


void LEDStrip::onUpdate(unsigned long elapsedTime) {
	//Call the function that is currently animating the LED strip when a sufficient amount of time has elapsed
	if (currentAnimation != nullptr) {
		currentAnimation->makeFirstRun();
		currentAnimation->timeRemaining = currentAnimation->timeRemaining - elapsedTime;
		if (currentAnimation->timeRemaining <= 0) {
			currentAnimation->updateAnimation();
			currentAnimation->timeRemaining = currentAnimation->updateInterval;
		}

		//Show the changes
		//FastLED.show();
	}
}

LEDStrip::LEDStrip(const char* name, int numLEDs) : Peripheral(name), numLEDs(numLEDs) {
	state = OFF;
	setState(ON);
	registerRemoteFunctions(ledRemoteFunctionMap);
	//strip = new CRGB[numLEDs];
}

LEDStrip::~LEDStrip() {
	for (auto& item : animationMap) {
		delete item.second;
	}
}

LEDStrip::Animation::Animation()
{}

LEDStrip::Animation::Animation(unsigned int updateInterval) : updateInterval(updateInterval)
{}

bool LEDStrip::Animation::makeFirstRun()
{
	if (firstRun) {
		timeRemaining = 0;
		loopCount = 0;
		firstRun = false;
	}
	return !firstRun;
}
