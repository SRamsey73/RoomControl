#include <Arduino.h>

#define BUTTON1 53

void readSerialInputBuffer();
void writeToPCSerial(char *);
int intFromMessage(char *msg);

void setup() {
pinMode(LED_BUILTIN, OUTPUT);
pinMode(BUTTON1, INPUT);

  // Open serial
  Serial.begin(19200);
  Serial.setTimeout(0);

  for (int i = 0; i < 5; i++){
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {
  readSerialInputBuffer();

  digitalWrite(LED_BUILTIN, digitalRead(BUTTON1));

  if(digitalRead(BUTTON1) == HIGH) {
    digitalWrite(LED_BUILTIN, HIGH);
    writeToPCSerial("message:Hello There");
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void readSerialInputBuffer() {
  // Check if data is in serial buffer
  if (Serial.available() > 0) {
    char readChar;
    char msg[1024] = {};
    int index = 0;

    // Read in data until terminator is received
    while (readChar != '\4') {
      // Possibility to hang if terminator is never transferred

      // Check that data is available before reading
      if (Serial.available() > 0) {
        // Read in char in store in array
        readChar = Serial.read();
        msg[index] = readChar;
        index++;
      }
    }

    // Last character read was the transmission terminator. Replace with null terminator
    msg[index - 1] = '\0';

    // light commands
    if (strstr(msg, "lights:")) {
      if(strstr(msg, "brightness:")) {
        int recv_int = intFromMessage(msg);
        writeToPCSerial("message:received brightness change");
      }
    }
  }
}

void writeToPCSerial(char *msg) {
  Serial.write(msg);
  Serial.write('\4');
}

int intFromMessage(char *msg) {
  return atoi(strstr(msg, "int:") + 4);
}