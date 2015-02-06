/*
This sketch demonstrates how to send data from a Device
to a Host in a Gazell network.

When Button A on the Device is pressed and released,
the green led on the host will toggle.
*/

#include <RFduinoGZLL.h>

#define ROLE DEVICE0
#define DELAY 100

#define PIN_RIGHT 2
#define PIN_FORWARD 3
#define PIN_LEFT 4

#define RIGHT 0
#define FORWARD 1
#define LEFT 2
#define BACKWARD 3
#define BRAKE 4

#define V_MAX 300
#define V_MIN 0
#define V_DIV 20.0

#define N_MAGS 16

int reading_right;
int reading_forward;
int reading_left;

String state_new;
String state_current;
int heading;
int mag;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_FORWARD, INPUT);
  pinMode(PIN_LEFT, INPUT);
  state_current = "";
  RFduinoGZLL.begin(ROLE);
}

void loop()
{
  delay(DELAY);
  
  reading_right = analogRead(PIN_RIGHT);
  reading_forward = analogRead(PIN_FORWARD);
  reading_left = analogRead(PIN_LEFT);
  
  // forward
  if (reading_forward >= reading_right && reading_forward >= reading_left)
  {
    heading = FORWARD;
    mag = getMag(reading_forward);
  }
  // right
  else if (reading_right >= reading_left)
  {
    heading = RIGHT;
    mag = getMag(reading_right);
  }
  // left
  else
  {
    heading = LEFT;
    mag = getMag(reading_left);
  }
  
  state_new = String(heading*N_MAGS + mag);
  if (state_new != state_current)
  {
    RFduinoGZLL.sendToHost(state_new);
    state_current = state_new;
  }
  /*
  delay(10000);
  RFduinoGZLL.sendToHost(0x1F);
  delay(1000);
  RFduinoGZLL.sendToHost(0x3F);
  delay(1000);
  RFduinoGZLL.sendToHost(0x0F);
  delay(1000);
  RFduinoGZLL.sendToHost(0x2F);
  delay(1000);
  RFduinoGZLL.sendToHost(0x40);
  */
}

int getMag(int voltage)
{
  voltage = min(V_MAX, max(V_MIN, voltage));
  return round((double(voltage) - double(V_MIN)) / V_DIV);
}
