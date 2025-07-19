#include <SoftwareSerial.h> 

int randomNumber;
SoftwareSerial geigerOutSerial(2, 3);

void setup() {
  // initialize pin to be used for data transfer
  // Serial1.begin(115200);

  // used for initial debugging
  Serial.begin(115200);
  geigerOutSerial.begin(115200);
  randomSeed(analogRead(0));  // use analog noise to intialize random generator
}

void loop() {
  // make 8 bit hex number
  randomNumber = random(256); // random number 0-255

  // Serial1.println(randomNumber);
  Serial.println(randomNumber, HEX);  // REMOVE WHEN DONE DEBUGGING
  geigerOutSerial.println(randomNumber, HEX);
  delay(100);
}
