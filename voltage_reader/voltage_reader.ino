#include <RFduinoBLE.h>

int button = 5;

void setup() {
  pinMode(button, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(button) * 5.0 / 1000.0);
  delay(100);
}
