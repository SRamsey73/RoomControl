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

//Sensor Definitions================================================================================
#define OCCUPANCY_PIR 45
#define LDR A15
#define THERMISTOR A14
#define BUTTON1 53

//Seven Segment Display=============================================================================
#define SSD_DIO 51
#define SSD_CLK 49
SevenSegmentExtended SSD(SSD_CLK, SSD_DIO);

//Variables----------------------------------------------------------------------------------
//Serial Port Variables
int incomingByte[10];
int incomingData;
int bytesRead = 0;
int dataLength;
int incomingMeaning;
const int OFF = 0;
const int ON = 1;
const int HIGH_SPEED = 3;
const int MEDIUM_SPEED = 2;
const int LOW_SPEED = 1;
const int TOGGLE = 2;
const int OVERHEAD_LIGHTS = 0;
const int FAN = 1;
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
//Room State=======================================================================================
bool overheadLightState;
int fanState;
int fanSpeed;
//PIR
int roomOccupied = 0;
bool occupancyPIRState;
bool occupancyUpdated = true;
//LDR
#define LDR_THRESHOLD 500
bool ldrEnabled = true;
bool brightnessLDRState;
bool brightnessUpdated;
//Night Mode
bool nightModeEnabled = false;
#define NIGHT_LIGHT_BRIGHTNESS 78
//Overhead Light Variables
int overheadPIRControl;
int overheadLDRControl;
int overheadPIRTimeout;
 //Fan Variables
int fanPIRTimeout;
int tempControl;
//Temperature
float ambientTemperature;
int previousAmbientTemperature;
int targetTemperature;
int Vo;
float R1 = 50000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

long previousMillis;

//Timers===========================================================================================
Timer timer;

//Function prototypes==============================================================================
//SERIAL
void sendMsgSerial(const char *msg);
void sendMsgSerial(const char *msg, int data);
void sendMsgSerial(const char *msg, float data);
void sendMsgSerial(const char*msg, bool state);
void readSerialInputBuffer();
char* boolToState(bool b);
int intFromMessage(const char *msg);
float floatFromMessage(const char *msg);
void readActions(const char *msg);
void sync();

//ROOM INTERACTIONS
void overheadLight(int state);
void overheadLightBrightness(int brightness);
void automaticLighting();
void automaticLED();
void automaticFan();
void checkRoomBrightness();
void checkRoomOcuppancy();
void changeOverheadPIRState(int state);
void changePIRTimeout(int target, int timeout);
void changeOverheadLDRState(int state);
void display(int state);
void nightMode(int state);
void playGreeting();

//DESK LEDS
void updateBorderLEDs();
void changeBorderLEDState(int state, bool restart);
void clearBorderLEDS(bool persistant);
void computerListeningLED();
void computerSpeakingLED();
void computerFinishedSpeakingLED();
void occupancyWarningLED();
void rwbLayoutAsynchronous(bool show);
void tailEffect();
void rwbCenter();
void rwbBounce();
void rwbLayout();
void fadeRWB(int speed);
void fadeOut(int duration);

//Temperature
void readTemperature(bool readNow);
void changeTargetTemp(int temperature);
void changeTempControlState(int state);

//SSD Updates
void updateSSD();




//Setup============================================================================================
void setup() {
  SSD.begin();            // initializes the display
  SSD.setBacklight(60);  // set display brightness
  //Set Pinmodes
  pinMode(BORDER_LED_DATA, OUTPUT);
  pinMode(OCCUPANCY_PIR, INPUT);
  pinMode(LDR, INPUT);
  pinMode(THERMISTOR, INPUT);
  pinMode(BUTTON1, INPUT);
  
  //Initialize Serial Port
  Serial.begin(19200);

  //LED Setup
  delay(500);
  FastLED.addLeds<LED_TYPE,BORDER_LED_DATA,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(200);
  //Blink Border LEDS to indicate on state
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  changeBorderLEDState(AMBIENT, true);
  SSD.setColonOn(false);
  SSD.printTime(12,30,true);
  targetTime[4] = 1000;
  //SSD.print(TM1637_CHAR_DOT);
  //Wait for button press to start
  while(digitalRead(BUTTON1) == LOW) {}
  //Retreive current state of room
  delay(500);
  sync();
  delay(50);
  for (int i = 0; i < 20; i++) {
	  readSerialInputBuffer();
	  delay(10);
  }

  targetTime[5] = millis();
  SSD.clear();
}

//Loop=============================================================================================
void loop() {
	if (digitalRead(BUTTON1) == HIGH) {
		nightMode(TOGGLE);
		if (nightModeEnabled) {
			overheadLight(OFF);
		}
		delay(300);
	}
	readTemperature(false);
	checkRoomOcuppancy();
	checkRoomBrightness();
	readSerialInputBuffer();
	automaticLighting();
	automaticLED();
	updateBorderLEDs();
	//updateClock();
}

//Functions-------------------------------------------------------------------
//SERIAL FUNCTIONS
void sendMsgSerial(const char *msg) {
  Serial.write(msg);
  Serial.write('\4');
}


void sendMsgSerial(const char *msg, int data) {
  char msg_frmt[50];
  sprintf(msg_frmt, "%s:int:%i", msg, data);
  sendMsgSerial(msg_frmt);
}


void sendMsgSerial(const char *msg, float data) {
  char msg_frmt[50];
  sprintf(msg_frmt, "%s:float:%f", msg, (double) data);
  sendMsgSerial(msg_frmt);
}


void sendMsgStateSerial(const char *msg, int state) {
	char msg_frmt[50];
	if(state == 0) {
		strcpy(msg_frmt, "off");
	}
	else if(state == 1) {
		strcpy(msg_frmt, "on");
	}
	else {
		strcpy(msg_frmt, "toggle");
	}
	sprintf(msg_frmt, "%s:%s", msg, msg_frmt);
	sendMsgSerial(msg_frmt);
}


int intFromMessage(const char *msg) {
  return atoi(strstr(msg, "int:") + 4);
}


int stateFromMessage(const char *msg) {
	if(strstr(msg, ":on") - msg == strlen(msg) - 3) {
		return 1;
	}
	else if(strstr(msg, ":off") - msg == strlen(msg) - 4) {
		return 0;
	}
	else if(strstr(msg, ":toggle") - msg == strlen(msg) - 7) {
		return 2;
	}
	else {
		return -1;
	}
}

void readSerialInputBuffer() {
	// Check if data is in serial buffer
  	if (Serial.available() > 0) {
      char msg[1024] = {};
      Serial.readBytesUntil('\4', msg, sizeof(msg));
      readActions(msg);
    }
}


void readActions(const char *msg) {
  //DESK_LEDS
  if(strstr(msg, "desk_leds:state:")){
    int state = intFromMessage(msg);
    if(state == OFF) {
      clearBorderLEDS(true);
    }
    else {
      changeBorderLEDState(state, true);
    }
  }

  //LIGHTS
  if(strstr(msg, "lights:")) {
    if(strstr(msg, "off")) {
      overheadLightState = OFF;
    }
    else if(strstr(msg, "on")){
      overheadLightState = ON;
    }
  }

  //FAN
  if(strstr(msg, "fan:")) {
    if(strstr(msg, "off")) {
      fanState = OFF;
    }
    else if(strstr(msg, "on")) {
      fanState = ON;
    }
    if(strstr(msg, "speed:")) {
      fanSpeed = intFromMessage(msg);
    }    
  }

  //PIR
  if(strstr(msg, "pir:")) {
    if(strstr(msg, "lights@P:")) {
      if(strstr(msg, "off")) {
        changeOverheadPIRState(OFF);
      }
      else if(strstr(msg, "on")) {
        changeOverheadPIRState(ON);
      }
      else if(strstr(msg, "timeout:")) {
        changePIRTimeout(OVERHEAD_LIGHTS, intFromMessage(msg));
      }
    }
    else if(strstr(msg, "fan@P:")) {
      if(strstr(msg, "off")) {
        //changeFanPIRState(OFF);
      }
      else if(strstr(msg, "on")) {
        //changeFanPIRState(ON);
      }
      else if(strstr(msg, "timeout:")) {
        changePIRTimeout(FAN, intFromMessage(msg));
      }
    }
  }

  //LDR
  if(strstr(msg, "ldr:")) {
    if(strstr(msg, "lights@P:")) {
      if(strstr(msg, "off")) {
        changeOverheadLDRState(OFF);
      }
      else if(strstr(msg, "on")) {
        changeOverheadLDRState(ON);
      }
    }
    else if(strstr(msg, "fan@P:")) {
      if(strstr(msg, "off")) {
        //changeFanLDRState(OFF);
      }
      else if(strstr(msg, "on")) {
        //changeFanLDRState(ON);
      }
    }
  }

  //NIGHT_MODE
  if(strstr(msg, "night_mode:")) {
    if(strstr(msg, "off")) {
      nightMode(OFF);
    }
    else if(strstr(msg, "on")) {
      nightMode(ON);
    }
    else if(strstr(msg, "toggle")) {
      nightMode(TOGGLE);
    }
  }

  //TEMPERATURE
  if(strstr(msg, "temperature:")) {
    if(strstr(msg, "control:state:")) {
      if(strstr(msg, "off")) {
        changeTempControlState(OFF);
      }
      else if(strstr(msg, "on")) {
        changeTempControlState(ON);
      }
      else if(strstr(msg, "toggle")) {
        changeTempControlState(TOGGLE);
      }
    }
    else if(strstr(msg, "ambient:read")) {
      readTemperature(true);
    }
    else if(strstr(msg, "target:")) {
      changeTargetTemp(intFromMessage(msg));
    }
  }
  
}


void sync() {
	sendMsgSerial("sync");
}


void updateClock() {
	/*if (millis() > targetTime[4]) {
		targetTime[4] += 1100;
		SSD.print(time);
		time++;
	}*/
	if (millis() >= targetTime[4]) {
		targetTime[4] += 1050;
		//timeSeconds++;
		if (timeSeconds >= 59) {
			timeSeconds = 0;
			if (timeMinutes >= 59) {
				timeMinutes = 0;
				if (timeHours >= 12) {
					timeHours = 1;
				}
				else {
					timeHours++;
				}
			}
			else {
				timeMinutes++;
			}
		}
		else {
			timeSeconds++;
		}
		//SSD.clear();
		SSD.printTime(timeHours, timeMinutes, false);
		//SSD.print(millis() / 1000);
	}
}

//Room Interactions================================================================================
void overheadLight(int state) {
  switch(state) {
    case OFF:
      sendMsgSerial("lights:off");
      break;
    case ON:
      sendMsgSerial("lights:on");
      break;
    case TOGGLE:
      sendMsgSerial("lights:toggle");
      break;
  }
}


void overheadLightBrightness(int brightness) {
	sendMsgSerial("lights:brightness", brightness);
}


void automaticLighting() {
	if (occupancyUpdated) {
		if (roomOccupied == 1 && overheadPIRControl && ((overheadLDRControl && brightnessLDRState) || !overheadLDRControl)) {
			overheadLight(ON);
			//SSD.print("ON");
			//delay(500);
			//SSD.clear();
		}
		else if (roomOccupied == 0 && overheadPIRControl) {
			overheadLight(OFF);
			//SSD.print("OFF");
			//delay(500);
			//SSD.clear();
		}
		else if (roomOccupied == 2 && overheadPIRControl) {
			sendMsgSerial("lights:occupancy_warning");
		}
	}
	/*if (!overheadPIRControl && overheadLDRControl && brightnessLDRState && brightnessUpdated) {
		OverheadLight(ON);
	}*/
}


void automaticLED() {
	if (occupancyUpdated && overheadPIRControl) {
		if (roomOccupied == 0) {
			clearBorderLEDS(true);
		}
		else if (roomOccupied == 1) {
			changeBorderLEDState(AMBIENT, true);
		}
		else if (roomOccupied == 2) {
			changeBorderLEDState(OCCUPANCY_WARNING, true);
		}
	}
}


void automaticFan() {
	if (tempControl) {
		if ((ambientTemperature - targetTemperature) >= 6) {
			if (fanSpeed != HIGH_SPEED) {
				sendMsgSerial("fan:speed:high");
			}
		}
		else if ((ambientTemperature - targetTemperature) >= 3) {
			if (fanSpeed != MEDIUM_SPEED) {
				sendMsgSerial("fan:speed:medium");
			}
		}
		else if ((ambientTemperature - targetTemperature) >= 0) {
			if (fanSpeed != LOW_SPEED) {
				sendMsgSerial("fan:speed:low");
			}
		}

		if ((ambientTemperature - targetTemperature) >= 0) {
			if (fanState != ON) {
				sendMsgSerial("fan:on");
			}
		}
		else if (fanState != OFF) {
			sendMsgSerial("fan:off");
		}
	}
}


void checkRoomBrightness() {
	brightnessUpdated = false;
	if (analogRead(LDR) <= LDR_THRESHOLD) {
		if (brightnessLDRState == false) {
			brightnessLDRState = true;
			brightnessUpdated = true;
		}
		else {
			return;
		}
	}
	else {
		if (brightnessLDRState == true) {
			brightnessLDRState = false;
			brightnessUpdated = true;
		}
	}
}


void checkRoomOcuppancy() {
	occupancyUpdated = false;
	if (digitalRead(OCCUPANCY_PIR) == 1) {
		occupancyPIRState = 1;
		if (roomOccupied != 1) {
			if (roomOccupied != 2 && !nightModeEnabled) {
				playGreeting();
			}
			roomOccupied = 1;
			occupancyUpdated = true;
		}
		else {
			return;
		}
	}
	else {
		if (occupancyPIRState == 1) {
			occupancyPIRState = 0;
			targetTime[0] = millis() + 8000;
		}
		else if(millis() >= targetTime[0]) {
			if (roomOccupied == 1) {
				occupancyUpdated = true;
				roomOccupied = 2;
				targetTime[0] = millis() + 5000;
			}
			else {
				if (roomOccupied != 0) {
					roomOccupied = 0;
					occupancyUpdated = true;
				}
				else {
					return;
				}
			}
		}
	}
}


void changeOverheadPIRState(int state) {
	switch (state) {
	case OFF:
		overheadPIRControl = 0;
		sendMsgSerial("pir:lights@P:off");
		changeOverheadLDRState(OFF);
		break;
	case ON:
		occupancyUpdated = true;
		overheadPIRControl = 1;
		sendMsgSerial("pir:lights@P:on");
		break;
	case TOGGLE:
		if (overheadPIRControl == 0) {
			changeOverheadPIRState(ON);
		}
		else {
			changeOverheadPIRState(OFF);
		}
	}
}


void changePIRTimeout(int target, int timeout) {
	switch (target) {
		case OVERHEAD_LIGHTS:
			overheadPIRTimeout = timeout;
			sendMsgSerial("pir:lights@P:timeout", overheadPIRTimeout);
			break;
		case FAN:
			fanPIRTimeout = timeout;
			sendMsgSerial("pir:fan@P:timeout", fanPIRTimeout);
			break;
	}
}


void changeOverheadLDRState(int state) {
	switch (state) {
	case OFF:
		overheadLDRControl = 0;
		sendMsgSerial("ldr:lights@P:off");
		break;
	case ON:
		if (overheadPIRControl) {
			brightnessUpdated = true;
			overheadLDRControl = 1;
			sendMsgSerial("ldr:lights@P:on");
		}
		break;
	case TOGGLE:
		if (overheadLDRControl == 0) {
			changeOverheadLDRState(ON);
		}
		else {
			changeOverheadLDRState(OFF);
		}
	}
}


void display(int state) {
	switch(state) {
		case OFF:
			sendMsgSerial("pc_display:off");
			break;
		case ON:
			sendMsgSerial("pc_display:on");
			break;
	}
}


void nightMode(int state) {
	switch (state)
	{
	case OFF:
		changeBorderLEDState(ON, true);
		changeOverheadPIRState(ON);
		changeOverheadLDRState(ON);
		nightModeEnabled = false;
		SSD.setBacklight(60);
		if(analogRead(LDR) < LDR_THRESHOLD) {
			overheadLightBrightness(NIGHT_LIGHT_BRIGHTNESS);
			overheadLight(ON);
		}
		sendMsgSerial("night_mode:off");
		break;
	case ON:
		nightModeEnabled = true;
		SSD.setBacklight(0);
		clearBorderLEDS(false);
		changeBorderLEDState(OFF, true);
		changeOverheadLDRState(OFF);
		changeOverheadPIRState(OFF);
		if(analogRead(LDR) < LDR_THRESHOLD) {
			overheadLightBrightness(NIGHT_LIGHT_BRIGHTNESS);
			overheadLight(ON);
		}
		sendMsgSerial("night_mode:on");
		//display(OFF);
		//SSD.print("  ON");
		break;
	case TOGGLE:
		nightMode(!nightModeEnabled);
		break;
	}
}


void playGreeting() {
	if (!nightModeEnabled) {
		sendMsgSerial("play_greeting");
	}
}

//LED functions=====================================================================================
void updateBorderLEDs() {
	switch (borderLEDState) {
	case OFF:
		break;
	case AMBIENT:
		if (!nightModeEnabled) {
			if(ledFirstRun) {
				randomSeed(analogRead(A5));
				ledFirstRun = false;
				changeBorderLEDState(random(3, 8), true);
			}
		}
		else {
			clearBorderLEDS(false);
		}
		break;
	case FADE_OUT:
		fadeOut(1);
		break;
	case RWB_FADE:
		fadeRWB(7);
		break;
	case RWB_TAIL:
		tailEffect();
			break;
	case RWB_CENTER:
		rwbCenter();
		break;
	case RWB_BOUNCE:
		rwbBounce();
		break;
	case RWB_LAYOUT:
		rwbLayoutAsynchronous(true);
		break;
	case COMPUTER_LISTENING:
		computerListeningLED();
		break;
	case COMPUTER_SPEAKING:
		computerSpeakingLED();
		break;
	case COMPUTER_FINISHED_SPEAKING:
		computerFinishedSpeakingLED();
		break;
	case OCCUPANCY_WARNING:
		occupancyWarningLED();
		break;
	}
}


void changeBorderLEDState(int state, bool restart) {
	borderLEDState = state;
	if (borderLEDState == 0 || borderLEDState == 1) {
		sendMsgSerial("desk_leds:state", borderLEDState);
	}
	if (restart || state == AMBIENT) {
		ledFirstRun = true;
	}
	targetTime[5] = millis(); //reset tail effect timer, prevents catching up
}


void clearBorderLEDS(bool persistant) {
	if (persistant) {
		borderLEDState = OFF;
	}
  for (int i = 0; i < NUM_LEDS; ++i) {
   leds[i] = CRGB::Black;
  }
  FastLED.show();
}


void computerListeningLED() {
	if (ledFirstRun) {
		FastLED.setBrightness(BRIGHTNESS);
		clearBorderLEDS(false);
		timeLoopCount[2] = 0;
		leftTail = 0;
		rightTail = NUM_LEDS;
		tailDirection = 1;
		centerReached = false;
		ledFirstRun = false;
		targetTime[2] = millis();
	}
	else if (millis() >= targetTime[2]) {
		if (!centerReached) {
			for (int i = 0; i < NUM_LEDS; i++) {
				leds[i].setRGB(0, 0, 0);
			}
			for (int i = leftTail; i > leftTail - listeningTailLength; i--) {
				if (i >= 0) {
					leds[i].setRGB(255, 50, 0);
				}
			}
			leftTail++;
			for (int i = rightTail; i < rightTail + listeningTailLength; i++) {
				if (i <= 89) {
					leds[i-1].setRGB(255, 50, 0);
				}
			}
			rightTail--;
			if (leftTail == 44) {
				centerReached = true;
			}
			targetTime[2] += 20;
		}
		else {
			if (leftTail >= 44) {
				tailDirection = -1;
			}
			else if(leftTail < 24 + listeningTailLength) { //edge:24
				tailDirection = 1;
			}
			leftTail += tailDirection;
			rightTail -= tailDirection;
			for (int i = 0; i < NUM_LEDS; i++) {
				leds[i].setRGB(0, 0, 0);
			}
			for (int i = leftTail; i > leftTail - listeningTailLength; i--) {
				if (i >= 0) {
					leds[i].setRGB(255, 50, 0);
				}
			}
			for (int i = rightTail; i < rightTail + listeningTailLength; i++) {
				if (i <= 89) {
					leds[i-1].setRGB(255, 50, 0);
				}
			}
			targetTime[2] += 50;
		}
		FastLED.show();
	}
}


void computerSpeakingLED() {
	if (ledFirstRun) {
		timeLoopCount[2] = 0;
		targetTime[2] = millis();
		ledFirstRun = false;
	}
	else if (millis() >= targetTime[2]) {
		int i = timeLoopCount[2];
			if((leftTail + 1 + i) <= 45) {
				leds[leftTail + 1 + i].setRGB(255, 50, 0);
				leds[rightTail - 1 - i].setRGB(255, 50, 0);
			}
			if ((leftTail - listeningTailLength - i) > 22) {
				leds[leftTail - (listeningTailLength - 1) - i].setRGB(255, 50, 0);
				leds[rightTail + (listeningTailLength - 2) + i].setRGB(255, 50, 0);
			}
			if (timeLoopCount[2] >= 20) {
				return;
			}
			else {
				timeLoopCount[2]++;
			}			
		targetTime[2] += 20;
		FastLED.show();
	}
}


void computerFinishedSpeakingLED() {
	if (ledFirstRun) {
		timeLoopCount[2] = 0;
		targetTime[2] = millis();
		ledFirstRun = false;
	}
	else if(millis() >= targetTime[2]) {
		if (timeLoopCount[2] >= 44) {
			changeBorderLEDState(AMBIENT, true);
			return;
		}
		else {
			leds[44 - timeLoopCount[2]].setRGB(0, 0, 0);
			leds[45 + timeLoopCount[2]].setRGB(0, 0, 0);
		}
		timeLoopCount[2]++;
		targetTime[2] += 20;
		FastLED.show();
	}
}


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


void rwbLayoutAsynchronous(bool show) {
	if (ledFirstRun) {
		FastLED.setBrightness(BRIGHTNESS);
		ledFirstRun = false;
	}
	for (int i = 3; i < NUM_LEDS-3; i++) {
		leds[i] = CRGB::Red;
	}
	for (int i = 6; i < NUM_LEDS-3; i += 9) {
		for (int w = 0; w < 6; w++) {
			leds[i + w] = CRGB::White;
		}
	}
	for (int i = 87; i < NUM_LEDS; i++) {
		leds[i] = CRGB::White;
	}
	for (int i = 9; i < NUM_LEDS-3; i += 9) {
		for (int w = 0; w < 3; w++) {
			leds[i + w] = CRGB::Blue;
		}
	}
	for (int i = 0; i < 3; i++) {
		leds[i] = CRGB::Blue;
	}
	if (show) {
		FastLED.show();
	}
}


void tailEffect() {
	int tailLength = 15;
	if (ledFirstRun) {
		tailPosistion = tailLength-1;
		tailColor = 0;
		ledFirstRun = false;
		targetTime[5] = millis();
	}
	else if (millis() >= targetTime[5]) {
		//rwbLayoutAsynchronous(false);
			if (tailPosistion == 0) {
				previousTailColor = tailColor;
				if (tailColor == 2) {
					tailColor = 0;
				}
				else {
					tailColor++;
				}
			}
			for (int i = 0; i < NUM_LEDS; i++) {
				switch (tailColor)
				{
				case 0:
					leds[i].setRGB(255, 0, 0);
					break;
				case 1:
					leds[i].setRGB(255, 255, 255);
					break;
				case 2:
					leds[i].setRGB(0, 0, 255);
					break;
				}
				if (tailPosistion < 70) {
					if(i > 70)
					switch (previousTailColor)
					{
					case 0:
						leds[i].setRGB(255, 0, 0);
						break;
					case 1:
						leds[i].setRGB(255, 255, 255);
						break;
					case 2:
						leds[i].setRGB(0, 0, 255);
						break;
					}
				}
			}
		for (int i = 0; i < NUM_LEDS; i++) {
			if (tailPosistion < tailLength - 1) {
				if (!(i <= tailPosistion || i >= (NUM_LEDS - 1) - (tailLength - (tailPosistion + 2)))) {
					leds[i].setRGB(0, 0, 0);
				}
			}
			else if(!(i <= tailPosistion && i > tailPosistion - tailLength)) {
				leds[i].setRGB(0,0,0);
			}
			//leds[tailPosistion + i] = CRGB::Yellow;
		}
		if (tailPosistion >= NUM_LEDS - 1) {
			tailPosistion = 0;
		}
		else {
			tailPosistion++;
		}
		targetTime[5] += 35;
		FastLED.show();
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

//Temperature
void readTemperature(bool readNow) {
	if (millis() >= targetTime[11] || readNow) {
		previousAmbientTemperature = (int)ambientTemperature;
		Vo = analogRead(THERMISTOR);
		R2 = R1 * (1023.0 / (float)Vo - 1.0);
		logR2 = log(R2);
		T = (1.0 / (c1 + c2 * logR2 + c3 * logR2*logR2*logR2));
		T = T - 273.15;
		ambientTemperature = (T * 9.0) / 5.0 + 32.0;
		//SSD.clear();
		//SSD.print("  F");
		//SSD.print((int)ambientTemperature);
		if ((int)ambientTemperature != previousAmbientTemperature) {
			automaticFan();
		}
		sendMsgSerial("temperature:ambient", (int) ambientTemperature);
		targetTime[11] += 300000;
	}
}


void changeTargetTemp(int temperature) {
	targetTemperature = temperature;
	sendMsgSerial("temperature:target", targetTemperature);
	automaticFan();
}


void changeTempControlState(int state) {
	switch (state) {
	case OFF:
		tempControl = OFF;
    sendMsgSerial("temperature:control:off");
		break;
	case ON:
		tempControl = ON;
    sendMsgSerial("temperature:control:on");
		break;
	case TOGGLE:
		if (tempControl) {
			changeTempControlState(OFF);
		}
		else {
			changeTempControlState(ON);
		}
		return;
		break;
	}
}

//SSD Updates
void updateSSD() {
	SSD.clear();
	SSD.print(occupancyPIRState);
	//SSD.print("  " + (int)targetTemperature);
	//SSD.print((int)ambientTemperature);
	//SSD.print((int)targetTemperature);
	//SSD.print("   " + incomingData);
	//SSD.print(100 + roomOccupied);
	//SSD.print(10 + overheadLDRControl);
	//SSD.print(overheadPIRControl);
	//SSD.print((int)ambientTemperature);
	//SSD.print(overheadPIRTimeout);
	//SSD.print(digitalRead(OCCUPANCY_PIR));
}


