//forwards commands typed in Serial Monitor out to the XLR terminal

#include <SoftwareSerial.h>

SoftwareSerial xlr(2, 3); // RX=D2, TX=D3

void setup() {
  Serial.begin(115200);
  xlr.begin(115200);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    xlr.write(c);
  }
}