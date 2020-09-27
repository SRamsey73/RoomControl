#include <Arduino.h>

#define M_CLK 4
#define M_DIO 3

#define OVERHEAD_LIGHT_DIMMER 11
#define RELAY_LIGHT 5
#define RELAY_FAN 6
#define RELAY_HIGH 7
#define RELAY_MEDIUM 8
#define RELAY_LOW 9

#define OFF 0
#define ON 1
#define TOGGLE 2

#define FAN_LOW 1
#define FAN_MEDIUM 2
#define FAN_HIGH 3

//Serial variables-----------------------------------------------------
int incomingByte[10];
int incomingData;
int bytesRead = 0;
int dataLength;
int incomingMeaning;

//Timer Variables----------------------------------------------------------------------
// 0:Overhead Light Fader | 
unsigned long targetTime[10];
int timeLoopCount[10];
int stepTime[10];
unsigned long endTime;
unsigned long breakTime;

//Room Interaction Variables-------------------------------------------------------
int overheadLightState = 0;
int fanState = 0;
int fanSpeed = 1;
//Overhead Light Fader Variables-------------------------------------------------------
int overheadLightFadeParams[4]; //0: target brightness 2: saved brightness for occupancy 3: saved brightness when light is off
int overheadLightBrightness = 255;
bool occupancyWarningActive;


//Function prototypes
void writeToPCSerial(const char *msg);
void writeToPCSerial(int meaning, int data);
void readSerialInputBuffer();
int intFromMessage(const char *msg);
void readActions(const char *msg);

void changeOverheadLightState(int state);
void changeOverheadLightBrightness(int brightness);
void dimOverheadLight(int brightness);
void occupancyWarning();
void taskDimOverheadLight();            
void changeFanState(int state);
void changeFanSpeed(int speed);
void asynchronousTaskHandler();

// SETUP
void setup() {
	overheadLightFadeParams[3] = 255;
	pinMode(OVERHEAD_LIGHT_DIMMER, OUTPUT);
	pinMode(RELAY_LIGHT, OUTPUT);
	pinMode(RELAY_FAN, OUTPUT);
	pinMode(RELAY_HIGH, OUTPUT);
	pinMode(RELAY_MEDIUM, OUTPUT);
	pinMode(RELAY_LOW, OUTPUT);
	//pinMode(BUTTON1, INPUT);
	digitalWrite(RELAY_LIGHT, HIGH);
	digitalWrite(RELAY_FAN, HIGH);
	digitalWrite(RELAY_HIGH, HIGH);
	digitalWrite(RELAY_MEDIUM, HIGH);
	digitalWrite(RELAY_LOW, LOW);
	//Serial.setTimeout(0);
	Serial.begin(19200);
	delay(500);
	writeToPCSerial("Basement Arduino Connected");
}


//SERIAL FUNCTIONS
void writeToPCSerial(const char *msg) {
  Serial.write(msg);
  Serial.write('\4');
}

int intFromMessage(const char *msg) {
  return atoi(strstr(msg, "int:") + 4);
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
  // light commands
  if (strstr(msg, "lights:")) {
    if (strstr(msg, "off")) {
      changeOverheadLightState(OFF);
    }
    else if (strstr(msg, "on")) {
      changeOverheadLightState(ON);
    }
    else if (strstr(msg, "toggle")) {
      changeOverheadLightState(TOGGLE);
    }
    else if(strstr(msg, "brightness:")) {
      changeOverheadLightBrightness(intFromMessage(msg));
    }
  }

  // fan commands
  if (strstr(msg, "fan:")) {
    if (strstr(msg, "off")) {
      changeFanState(OFF);
    }
    else if(strstr(msg, "on")) {
      changeFanState(ON);
    }
    else if(strstr(msg, "toggle")) {
      changeFanState(TOGGLE);
    }
    else if(strstr(msg, "speed:")) {
      if(strstr(msg, "low")) {
        changeFanSpeed(FAN_LOW);
      }
      else if(strstr(msg, "medium")) {
        changeFanSpeed(FAN_MEDIUM);
      }
      else if(strstr(msg, "high")) {
        changeFanSpeed(FAN_HIGH);
      }
    }
  }
}


// ROOM HARDWARE FUNCTIONS
void changeOverheadLightState(int state) {
	switch (state) {
	case 0:
		if (occupancyWarningActive) {
			overheadLightFadeParams[0] = overheadLightFadeParams[2];
			occupancyWarningActive = false;
		}
		if (overheadLightState != 0) {
			//digitalWrite(RELAY_LIGHT, LOW);
			overheadLightFadeParams[3] = overheadLightFadeParams[0];
			dimOverheadLight(0);
			overheadLightState = 0;
			writeToPCSerial("lights:off");
		}
		break;
	case 1:
		if (occupancyWarningActive) {
			overheadLightFadeParams[0] = overheadLightFadeParams[2];
			occupancyWarningActive = false;
			targetTime[0] = millis();
		}
		if (overheadLightState != 1) {
			//digitalWrite(RELAY_LIGHT, HIGH);
			overheadLightBrightness = 40;
			if (overheadLightFadeParams[3] < 35) {
				overheadLightFadeParams[3] = 255;
			}
			overheadLightState = 1;
			dimOverheadLight(overheadLightFadeParams[3]);
			writeToPCSerial("lights:on");
		}
		break;
	case 2:
		if (overheadLightState == 0) {
			changeOverheadLightState(1);
		}
		else {
			changeOverheadLightState(0);
		}
	}
}


void changeOverheadLightBrightness(int brightness) {
  dimOverheadLight(brightness);
  char msg[50];
  sprintf(msg, "lights:brightness:int:%i", brightness);
	writeToPCSerial(msg);
}

void dimOverheadLight(int brightness) {
	if (overheadLightState) {
		overheadLightFadeParams[0] = brightness;
		targetTime[0] = millis();
	}
	else {
		overheadLightFadeParams[3] = brightness;
	}
}


void occupancyWarning() {
	occupancyWarningActive = true;
	overheadLightFadeParams[2] = overheadLightFadeParams[0];
	if (overheadLightFadeParams[0] > 35 && overheadLightFadeParams[0] <= 45) {
		overheadLightFadeParams[0] = 37;
	}
	else if (overheadLightFadeParams[0] > 45 && overheadLightFadeParams[0] <= 66) {
		overheadLightFadeParams[0] = 45;
	}
	else if (overheadLightFadeParams[0] > 66 && overheadLightFadeParams[0] <= 86) {
		overheadLightFadeParams[0] = 57;
	}
	else if (overheadLightFadeParams[0] > 86 && overheadLightFadeParams[0] <= 127) {
		overheadLightFadeParams[0] = 77;
	}
	else if (overheadLightFadeParams[0] > 127 && overheadLightFadeParams[0] <= 255) {
		overheadLightFadeParams[0] = 103;
	}
	targetTime[0] = millis();
}


void taskDimOverheadLight() {
	if (abs(overheadLightBrightness - overheadLightFadeParams[0]) > 2 && millis() >= targetTime[0]) {
		if (overheadLightBrightness > overheadLightFadeParams[0]) {
			overheadLightBrightness -= 3;
		}
		else if (overheadLightBrightness < overheadLightFadeParams[0]) {
			overheadLightBrightness += 3;
			if (overheadLightBrightness >= 200) {
				overheadLightBrightness = 255;
				overheadLightFadeParams[0] = 255;
			}
		}
		analogWrite(OVERHEAD_LIGHT_DIMMER, overheadLightBrightness);
		if (overheadLightFadeParams[0] > overheadLightBrightness) {
			targetTime[0] += 25;
		}
		else {
			targetTime[0] += 18;
		}
	}
	else if (millis() > targetTime[0] && abs(overheadLightBrightness - overheadLightFadeParams[0]) > 0 && abs(overheadLightBrightness - overheadLightFadeParams[0]) <= 2) {
		overheadLightBrightness = overheadLightFadeParams[0];
		analogWrite(OVERHEAD_LIGHT_DIMMER, overheadLightBrightness);
		///writeToPCSerial(244, 0);
	}
}


void changeFanState(int state) {
	switch (state) {
	case 0:
		digitalWrite(RELAY_FAN, HIGH);
		fanState = 0;
		writeToPCSerial("fan:off");
		break;
	case 1:
		digitalWrite(RELAY_FAN, LOW);
		fanState = 1;
		writeToPCSerial("fan:on");
		break;
	case 2:
		if (fanState == 0) {
			changeFanState(1);
		}
		else {
			changeFanState(0);
		}
		break;
	}
}


void changeFanSpeed(int speed) {
	fanSpeed = speed;
	digitalWrite(RELAY_FAN, HIGH);
	delay(20);
	switch (speed) {
	case 1: //LOW
		digitalWrite(RELAY_HIGH, HIGH);
		digitalWrite(RELAY_MEDIUM, HIGH);
		digitalWrite(RELAY_LOW, LOW);
    writeToPCSerial("fan:speed:low");
		break;
	case 2: // MEDIUM
		digitalWrite(RELAY_HIGH, HIGH);
		digitalWrite(RELAY_MEDIUM, LOW);
		digitalWrite(RELAY_LOW, HIGH);
    writeToPCSerial("fan:speed:medium");
		break;
	case 3: //HIGH
		digitalWrite(RELAY_HIGH, LOW);
		digitalWrite(RELAY_MEDIUM, HIGH);
		digitalWrite(RELAY_LOW, HIGH);
    writeToPCSerial("fan:speed:high");
		break;
	}
	digitalWrite(RELAY_FAN, !fanState);
}


//TASK HANDLER
void asynchronousTaskHandler() {
	taskDimOverheadLight();
}


int loopCount = 0;

void loop() {
	readSerialInputBuffer();
	asynchronousTaskHandler();
}
