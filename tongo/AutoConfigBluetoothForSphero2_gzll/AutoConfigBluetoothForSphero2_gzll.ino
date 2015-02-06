#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

int i;
String sphero_id;

void setup(){
    // Give user a chance to power cycle Bluetooth Module
    delay(5000);
    
    Serial.begin(9600);
    Serial.println("Initializing Bluetooth");
    
    // Init Blutooth Serial
    // Uncomment to change baud rate
    /*Serial1.begin(115200);
    sendCmd("$$$");    
    sendCmd("U,115200,N");  // *** Replace this with desired Baud Rate ***
    sendCmd("---");
    */
    
    device_t role = HOST;
    RFduinoGZLL.begin(role);
    
    
    // Reconnect to Bluetooth and configure to master    
    //Serial1.begin(115200);  // *** This too ***
    
    i = 1;
    sphero_id = "";
}

void loop(){
  i = 0;
  
  RFduinoGZLL.sendToDevice(DEVICE0, "$$$");
  
  
  RFduino_ULPDelay(INFINITE);
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  Serial.println("received data");
}
