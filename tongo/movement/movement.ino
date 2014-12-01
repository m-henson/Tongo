#include <Sphero.h>

#include <RFduinoBLE.h>
#include <Sphero.h>

Sphero sphero;

char spd = 0.1;

short heading;

int button = 5;

// debounce time (in ms)
int debounce_time = 10;

// maximum debounce timeout (in ms)
int debounce_timeout = 100;

void setup() {
  Serial.begin(9600);
  heading = 0;
  pinMode(button, INPUT);
  
  //RFduinoBLE.advertisementData = "tongo";
  
  // start the BLE stack
  RFduinoBLE.begin();
}

int debounce(int state)
{
  int start = millis();
  int debounce_start = start;
  
  while (millis() - start < debounce_timeout)
    if (digitalRead(button) == state)
    {
      if (millis() - debounce_start >= debounce_time)
        return 1;
    }
    else
      debounce_start = millis();

  return 0;
}

int delay_until_button(int state)
{
  // set button edge to wake up on
  if (state)
    RFduino_pinWake(button, HIGH);
  else
    RFduino_pinWake(button, LOW);
    
  do
    // switch to lower power mode until a button edge wakes us up
    RFduino_ULPDelay(INFINITE);
  while (! debounce(state));
  
  // if multiple buttons were configured, this is how you would determine what woke you up
  if (RFduino_pinWoke(button))
  {
    // execute code here
    RFduino_resetPinWake(button);
  }
}

void loop() {
  delay_until_button(HIGH);
  sphero.roll(heading, spd);
  heading = heading + 90;
  Serial.println(heading);
  heading = heading % 360;
  
  delay_until_button(LOW);
  sphero.stop(0);
  Serial.println(0);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  String received = "onReceive: ";
  received.concat(data);
  Serial.println(received);
}
