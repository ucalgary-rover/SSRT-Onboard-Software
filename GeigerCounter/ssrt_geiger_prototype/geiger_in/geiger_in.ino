#include <SoftwareSerial.h>

SoftwareSerial geigerInSerial(4,5);


void setup() {
  // put your setup code here, to run once:
  geigerInSerial.begin(115200);
  Serial.begin(115200);
}

void loop() {
  // Forward data from Serial to Serial1
  if (geigerInSerial.available()) {
    char c = geigerInSerial.read();
    Serial.write(c);
  }
}
