#include <RFduinoBLE.h>

int i;
int waiting;
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
    //RFduinoBLE.txPowerLevel = +4;
  
    // start the BLE stack
    RFduinoBLE.begin();
    
    // Reconnect to Bluetooth and configure to master    
    //Serial1.begin(115200);  // *** This too ***
    
    i = 1;
    sphero_id = "";
}

void loop(){
  //RFduinoBLE.sendByte(char(0xFF));
  //RFduinoBLE.sendByte(char(0xFF));
  //RFduinoBLE.sendByte(char(0x00));
  //RFduinoBLE.sendByte(char(0x01));
  //RFduinoBLE.sendByte(char(0x11));
  //RFduinoBLE.sendByte(char(0x01));
  //char sum = 0x00 + 0x01 +0x11 + 0x01;
  //RFduinoBLE.sendByte(char(~sum));
  
  char sop1 = char(0xFF);
  char sop2 = char(0xFF);
  char did = char(0x01);
  char cid = char(0x11);
  char seq = char(0x00);
  char dlen = char(0x01);
  char sum = (did + cid + seq + dlen) % 256;
  char chk = char(~sum);
  //char chk = char(0xEC);
  Serial.println(chk, HEX);
  Serial.println(sop1, HEX);
  
  char message[7]; 
  message[0] = sop1;
  message[1] = sop2;
  message[2] = did;
  message[3] = cid;
  message[4] = seq;
  message[5] = dlen;
  message[6] = chk;
  
  delay(1000);
  
  //RFduinoBLE.send(message, 7);
  //Serial.println(message);
  
  //RFduinoBLE.sendByte(sop1);
  //RFduinoBLE.sendByte(sop2);
  //RFduinoBLE.sendByte(did);
  //RFduinoBLE.sendByte(cid);
  //RFduinoBLE.sendByte(seq);
  //RFduinoBLE.sendByte(dlen);
  //RFduinoBLE.sendByte(chk);
  
  
  //sendCmd("$$$");
  i = 0;
  //sendCmd("C,68:86:e7:02:0b:80");
  //sendCmd("C,6886E7020B80");
  //RFduinoBLE.send("C,6886E7020B80", 14);
  RFduinoBLE.send("C,", 2);
  RFduinoBLE.sendByte(char(0x6886E7020B80));
  //RFduinoBLE.send("C,6886E7020B80\r", 15);
  //RFduinoBLE.send("$$$C,6886E7020B80\r", 18);
  
  
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
  //RFduinoBLE.send(cmd_char, sizeof(cmd_char));
  RFduinoBLE.send(cmd_char, cmd.length());
  Serial.println(cmd_char);
  Serial.println(sizeof(cmd_char));
  
  delay(2000);
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
  Serial.println("hello");
  return;
  Serial.println(data);
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
  {/*
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
    }*/
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
