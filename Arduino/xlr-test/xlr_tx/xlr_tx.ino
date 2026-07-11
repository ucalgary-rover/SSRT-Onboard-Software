//forwards commands typed in Serial Monitor out to the XLR terminal

#include <SoftwareSerial.h>

SoftwareSerial xlr(2, 3); // RX=D2, TX=D3
String inputBuffer = "";

void setup() {
  xlr.begin(115200);
}

void loop() {
  while (xlr.available()) {
    char c = xlr.read();
    
    if (c == '\n') {
      xlr.print("RECEIVED: ");
      xlr.println(inputBuffer);

      inputBuffer = "";
    }
    else if (c != '\r') {
      inputBuffer += c;
    }
  }
}