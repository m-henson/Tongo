#include <RFduinoGZLL.h>

#define ROLE DEVICE0
#define DELAY 50

#define PIN_RIGHT 4
#define PIN_FORWARD 3
#define PIN_LEFT 2

#define RIGHT 0
#define FORWARD 1
#define LEFT 2
#define BACKWARD 3
#define BRAKE 4

#define V_MAX 1000
#define V_MIN 250
#define V_DIV 50.0

/*#define V_MAX 491
#define V_MIN 11
#define V_DIV 32.0*/

#define N_MAGS 16

int reading_right;
int reading_forward;
int reading_left;

String state_new;
String state_current;
int heading;
int mag_new;
int mag_current;

void setup()
{
  //Serial.begin(9600);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_FORWARD, INPUT);
  pinMode(PIN_LEFT, INPUT);
  state_current = "";
  mag_current = -1;
  RFduinoGZLL.begin(ROLE);
}

void loop()
{
  delay(DELAY);
  
  reading_right = analogRead(PIN_RIGHT);
  reading_forward = analogRead(PIN_FORWARD);
  reading_left = analogRead(PIN_LEFT);
  /*Serial.print(reading_left);
  Serial.print("     ");
  Serial.print(reading_forward);
  Serial.print("     ");
  Serial.println(reading_right);*/
  
  // forward
  if (reading_forward >= reading_right && reading_forward >= reading_left)
  {
    heading = FORWARD;
    mag_new = getMag(reading_forward);
  }
  // right
  else if (reading_right >= reading_left)
  {
    heading = RIGHT;
    mag_new = getMag(reading_right);
  }
  // left
  else
  {
    heading = LEFT;
    mag_new = getMag(reading_left);
  }
  
  if (mag_new <= 0)
  {
    heading = BRAKE;
  }
  
  state_new = String(heading*N_MAGS + mag_new);
  if (state_new != state_current)
  {
    /*Serial.print(state_new);
    Serial.print(": ");
    Serial.print(heading);
    Serial.print(" -> ");
    Serial.print(mag_new);
    Serial.print("        ");
    Serial.print(reading_left);
    Serial.print("        ");
    Serial.print(reading_forward);
    Serial.print("        ");
    Serial.println(reading_right);*/
    RFduinoGZLL.sendToHost(state_new);
    state_current = state_new;
    mag_current = mag_new;
  }
}

int getMag(int voltage)
{
  voltage = min(V_MAX, max(V_MIN, voltage));
  return round((double(voltage) - double(V_MIN)) / V_DIV);
}
