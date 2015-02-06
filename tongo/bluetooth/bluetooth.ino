
#include <RFduinoBLE.h>

int button = 5;
void setup() {
  Serial.begin(9600);
  
  pinMode(button, INPUT);
  
  RFduinoBLE.advertisementData = "rfduino";
  
  RFduinoBLE.begin();
}
//d846963f78b7
void loop() {
  // switch to lower power mode
  RFduino_ULPDelay(INFINITE);
}

void RFduinoBLE_onAdvertisement(bool start)
{
   Serial.print("RFduino is doing BLE advertising: ");
   Serial.println(start);
}

void RFduinoBLE_onConnect()
{
  Serial.println("RFduino BLE connection successful");
}

void RFduinoBLE_onDisconnect()
{
  Serial.println("RFduino BLE disconnected");
}

void RFduinoBLE_onReceive(char *data, int len) {
  Serial.println("RFduino received data");
  Serial.println(data);
}
