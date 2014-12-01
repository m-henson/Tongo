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
    
    //RFduinoBLE.advertisementData = "tongo";
    //RFduinoBLE.advertisementInterval = MILLISECONDS(500);
    //RFduinoBLE.deviceName = "RFduino";
    //RFduinoBLE.txPowerLevel = -20;
  
    // start the BLE stack
    RFduinoBLE.begin();
    
    // Reconnect to Bluetooth and configure to master    
    //Serial1.begin(115200);  // *** This too ***
    
    i = 1;
    sphero_id = "";
}

void loop(){
  sendCmd("$$$");
  
  RFduino_ULPDelay(INFINITE);
}

void sendCmd(String cmd){
  String line = "";
  
  // Show which cmd is being sent
  Serial.println("> " + cmd);
  
  // Send command
  if (cmd != "$$$")
  {
    cmd.concat("\n");
  }
  char cmd_char[sizeof(cmd)];
  cmd.toCharArray(cmd_char, sizeof(cmd_char));
  RFduinoBLE.send(cmd_char, cmd.length());
  
  delay(1000);
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

void RFduinoBLE_onReceive(char *data, int len)
{
  String line = data;
  String serial = "< ";
  serial.concat(line);
  Serial.println(serial);
  if (i == 1)
  {
    i += 1;
    sendCmd("SM,1");
  }
  else if (i == 2)
  {
    i += 1;
    sendCmd("---");
  }
  else if (i == 3)
  {
    i += 1;
    Serial.println("Entering Command Mode");
    sendCmd("$$$");
  }
  else if (i == 4)
  {
    i += 1;
    Serial.println("Scanning for Spheros...");
    sendCmd("I,10");
  }
  else if (i == 5)
  {
    if (line.indexOf("Sphero") != -1)
    {
      //Save result
      //We can't exit because we need to wait for the command to finish
      if (sphero_id == "")
      {
        sphero_id = data;
        serial = "sphero_id = ";
        serial.concat(sphero_id);
        Serial.println(serial);
      }
      else
      {
        serial = "Found Sphero but didn't set sphero_id: ";
        serial.concat(data);
        Serial.println(serial);
      }
    }
    if (line.indexOf("Done") != -1 && sphero_id != "")
    {
      i += 1;
    }
  }
  else if (i == 6)
  {
    if (sphero_id != "")
    {
      Serial.print("Connecting to ");
      Serial.println(sphero_id.substring(13, sphero_id.indexOf(',', 13)));
      
      // Connect to Sphero Address
      sendCmd("C," + sphero_id.substring(0, 12));
      i += 1;
    }
    else
    {
      i = 5;
    }
  }
  else if (i == 7)
  {
    i += 1;
    Serial.println("Connected.");
    
    // Save address
    sendCmd("SR," + sphero_id.substring(0, 12));
  }
  else if (i == 8)
  {
    i += 1;   
    // We're done here!
    sendCmd("---");
  }
  else if (i == 9)
  {
    i += 1;
    Serial.println("Configuration complete!\nYou're Sphero's address has been saved in the Bluetooth Module\n\nHave fun ^^;");
  }
}
