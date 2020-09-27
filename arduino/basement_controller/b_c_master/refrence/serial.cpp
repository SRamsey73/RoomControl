#include <Arduino.h>

void readSerialInputBuffer();
void writeToPCSerial(char *);
int intFromMessage(char *msg);

void setup() {
  // Open serial
  Serial.begin(19200);
  Serial.setTimeout(0);
  delay(500);
}

void loop() {
  readSerialInputBuffer();
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

    writeToPCSerial(msg);

    // light commands
    if (strstr(msg, "lights:")) {
      if(strstr(msg, "brightness:")) {
        int recv_int = intFromMessage(msg);
        writeToPCSerial("message:received brightness");
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