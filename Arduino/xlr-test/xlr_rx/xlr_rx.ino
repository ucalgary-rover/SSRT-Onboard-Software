void setup() {
  Serial.begin(115200);   // USB / Serial Monitor
  Serial1.begin(115200);  // XLR terminal
}

void loop() {
  // Serial Monitor -> XLR
  if (Serial.available()) {
    char c = Serial.read();
    Serial1.write(c);
  }

  // XLR -> Serial Monitor
  if (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
  }
}