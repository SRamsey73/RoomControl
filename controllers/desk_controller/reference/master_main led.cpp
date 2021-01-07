#include <Arduino.h>

#include <Event.h>
#include <Timer.h>

#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>
#include <SevenSegmentTM1637.h>

#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>

#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

//LED Definitions===================================================================================
#define BORDER_LED_DATA 47
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 90
#define BRIGHTNESS 200
#define SECTION_LENGTH 2
CRGB leds[NUM_LEDS];

//LED Constans
#define RWB_LAYOUT_DELAY 6


//Timer Variables 0:PIR | 1:fadeOut | 2:LED Timer | 3: rwbFade Timer | 4: Clock Timer | 5: Tail Timer | 6: rwbCenter Timer | 7: rwbBounce Timer | 11: temperature Timer
unsigned long targetTime[12];
int timeLoopCount[12];
int stepTime[12];
unsigned long endTime;
unsigned long breakTime;
//Time Variables
int timeHours = 10;
int timeMinutes = 05;
int timeSeconds = 0;
bool colon;
int time = 0;
//LED Variables====================================================================================
int borderLEDState = 0; 
const int AMBIENT = 1;
const int FADE_OUT = 2;
const int RWB_FADE = 3;
const int RWB_TAIL = 4;
const int RWB_CENTER = 5;
const int RWB_BOUNCE = 6;
const int RWB_LAYOUT = 7;
const int COMPUTER_LISTENING = 26;
const int COMPUTER_SPEAKING = 27;
const int COMPUTER_FINISHED_SPEAKING = 28;
const int OCCUPANCY_WARNING = 20;
bool ledFirstRun = true;
int ledPIRControl;

int fadeOutInProgress;
int fadeRWBInProgress;
int fadeDirection;
int fadeBrightness;
int fadeColor;

//tailEffect
int tailPosistion;
int tailColor = 0;
int previousTailColor = 2;
//rwbCenter
int rwbCenterData[5];
//rwbBounce
int rwbBounceData[5];
//Computer Listening
const int listeningTailLength = 3;
int leftTail;
int rightTail;
int tailDirection;
bool centerReached;
//Computer Speaking
int tailGrowth;

long previousMillis;





void occupancyWarningLED() {
	if (ledFirstRun) {
		FastLED.setBrightness(200);
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i] = CRGB::Yellow;
		}
		FastLED.show();
		ledFirstRun = false;
	}
	else {
		return;
	}
}


void rwbCenter() {
	const int POSISTION = 0;
	const int DIRECTION = 1;
	const int LAYING = 2;
	const int COLOR = 3;
	if (ledFirstRun) {
		rwbCenterData[POSISTION] = 0; //posistion
		rwbCenterData[DIRECTION] = 1; //direction
		rwbCenterData[LAYING] = 1;
		rwbCenterData[COLOR] = 0; //color
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i].setRGB(0, 0, 0);
		}
		ledFirstRun = false;
		targetTime[6] = millis();		
	}
	else if(millis() >= targetTime[6]) {
		if (rwbCenterData[LAYING] == 1) {
			switch (rwbCenterData[COLOR])
			{
			case 0:
				leds[rwbCenterData[POSISTION]].setRGB(255, 0, 0);
				leds[89 - rwbCenterData[POSISTION]].setRGB(255, 0, 0);
				break;
			case 1:
				leds[rwbCenterData[POSISTION]].setRGB(255, 255, 255);
				leds[89 - rwbCenterData[POSISTION]].setRGB(255, 255, 255);
				break;
			case 2:
				leds[rwbCenterData[POSISTION]].setRGB(0, 0, 255);
				leds[89 - rwbCenterData[POSISTION]].setRGB(0, 0, 255);
				break;
			}
		}
		else {
			leds[rwbCenterData[POSISTION]].setRGB(0, 0, 0);
			leds[89 - rwbCenterData[POSISTION]].setRGB(0, 0, 0);
		}
		rwbCenterData[POSISTION] += rwbCenterData[DIRECTION];
		if (rwbCenterData[POSISTION] < 0 || rwbCenterData[POSISTION] > 45) { //Reached end, change parameters
			if (rwbCenterData[LAYING] == 0) { //If leds were being deleted, turn around and lay new colored ones
				rwbCenterData[LAYING] = 1;
				if (rwbCenterData[POSISTION] < 0) {
					if (rwbCenterData[COLOR] == 2) {
						rwbCenterData[COLOR] = 0;
					}
					else {
						rwbCenterData[COLOR]++;
					}
				}
				if (rwbCenterData[DIRECTION] == 1) {
					rwbCenterData[DIRECTION] = -1;
					rwbCenterData[POSISTION] = 45;
				}
				else {
					rwbCenterData[DIRECTION] = 1;
					rwbCenterData[POSISTION] = 0;
				}
			}
			else if (rwbCenterData[LAYING] == 1) { //If leds were being added, go to other end and delete them
				rwbCenterData[LAYING] = 0;
				if (rwbCenterData[POSISTION] >= 45) {
					rwbCenterData[POSISTION] = 0;
				}
				else {
					rwbCenterData[POSISTION] = 45;
				}
			}
			targetTime[6] += 250
;
		}
		targetTime[6] += 30;
		FastLED.show();
	}
}


void rwbBounce() {
	const int POSISTION = 0;
	const int DIRECTION = 1;
	const int COLOR = 2;
	const int tailLength = 9;
	if (ledFirstRun) {
		rwbBounceData[POSISTION] = 89;
		rwbBounceData[DIRECTION] = -1;
		rwbBounceData[COLOR] = 0;
		ledFirstRun = false;
		targetTime[7] = millis();
	}
	else if (millis() >= targetTime[7]) {
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i].setRGB(0, 0, 0);
		}
		for (int i = 0; i < tailLength; i++) {
			if ((rwbBounceData[POSISTION] - i) >= 0 && (rwbBounceData[POSISTION] - i) <= 89) {
				switch (rwbBounceData[COLOR])
				{
				case 0:
					leds[rwbBounceData[POSISTION] - i].setRGB(255, 0, 0);
					break;
				case 1:
					leds[rwbBounceData[POSISTION] - i].setRGB(255, 255, 255);
					break;
				case 2:
					leds[rwbBounceData[POSISTION] - i].setRGB(0, 0, 255);
					break;
				}
			}
		}
		rwbBounceData[POSISTION] += rwbBounceData[DIRECTION];
		if (rwbBounceData[POSISTION] - (tailLength - 1) < 0 || rwbBounceData[POSISTION] > 89) {
			if (rwbBounceData[POSISTION] > 89) {
				rwbBounceData[DIRECTION] = -1;
				rwbBounceData[POSISTION] = 89;
			}
			else {
				rwbBounceData[DIRECTION] = 1;
				rwbBounceData[POSISTION] = tailLength - 1;
			}
			if (rwbBounceData[COLOR] >= 2) {
				rwbBounceData[COLOR] = 0;
			}
			else {
				rwbBounceData[COLOR]++;
			}
		}
		targetTime[7] += 30;
		FastLED.show();
	}
}


void rwbLayout() {
    for (int i = NUM_LEDS-1; i >= 0; --i) {
      leds[i] = CRGB::Red;
      FastLED.show();
      delay(RWB_LAYOUT_DELAY);
    }
    for (int i = NUM_LEDS-4; i >= 0; i-=9) {
      delay(RWB_LAYOUT_DELAY*3);
      for(int w = 0; w < 6; w++)  {
        leds[i-w] = CRGB::White; 
        FastLED.show();
        delay(RWB_LAYOUT_DELAY);
      }
    }
    for (int i = NUM_LEDS-7; i >= 0; i-=9) {
      delay(RWB_LAYOUT_DELAY*6);
      for(int b = 0; b < 3; b++)  {
        leds[i-b] = CRGB::Blue; 
        FastLED.show();
        delay(RWB_LAYOUT_DELAY);
      }
    }
}


void fadeRWB(int speed) {

	//SSD.print(fadeDirection);
	if (ledFirstRun) {
		fadeColor = 2;
		fadeDirection = 0;
		timeLoopCount[3] = 0;
		targetTime[3] = millis();
		stepTime[3] = speed;
		fadeBrightness = FastLED.getBrightness();
	}

	ledFirstRun = false;
	if (millis() >= targetTime[3]) {
		targetTime[3] += stepTime[3];
		fadeBrightness = FastLED.getBrightness();
		if (fadeDirection == 0) {
			fadeBrightness = fadeBrightness - 1;
			FastLED.setBrightness(fadeBrightness);
			//SSD.clear();
			//SSD.print(fadeBrightness);
		}
		else {
			fadeBrightness = fadeBrightness + 1;
			FastLED.setBrightness(fadeBrightness);
		}
		FastLED.show();
		if (fadeBrightness == 0) {
			fadeDirection = 1;
			if (fadeColor == 2) {
				fadeColor = 0;
			}
			else {
				fadeColor++;
			}
			switch (fadeColor)
			{
			case 0:
				for (int i = 0; i < NUM_LEDS; ++i) {
					leds[i] = CRGB::Red;
				}
				break;
			case 1:
				for (int i = 0; i < NUM_LEDS; ++i) {
					leds[i] = CRGB::White;
				}
				break;
			case 2:
				for (int i = 0; i < NUM_LEDS; ++i) {
					leds[i] = CRGB::Blue;
				}
				break;
			}
		}
		else if (fadeBrightness >= 200) {
			fadeDirection = 0;
		}
	}
}


void fadeOut(int duration) {
	if (FastLED.getBrightness() == 0) {
		return;
	}
	if (ledFirstRun) {
		timeLoopCount[1] = 0;
		targetTime[1] = millis();
		stepTime[1] = duration;
	}
		ledFirstRun = false;
	if (millis() >= targetTime[1] && FastLED.getBrightness() != 0) {
		targetTime[1] += stepTime[1];
		timeLoopCount[1]++;
		FastLED.setBrightness(FastLED.getBrightness() - 1);
		FastLED.show();
		//SSD.print(FastLED.getBrightness());
		return;
	}
}




