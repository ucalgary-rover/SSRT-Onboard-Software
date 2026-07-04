//reads responses from the XLR terminal and displays in Serial Monitor

#include <SoftwareSerial.h>

SoftwareSerial xlr(2, 3); // RX=D2, TX=D3

void setup() {
  Serial.begin(115200);
  xlr.begin(115200);
}

void loop() {
  if (xlr.available()) {
    char c = xlr.read();
    Serial.write(c);
  }
}
