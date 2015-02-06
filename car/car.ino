#include <RFduinoGZLL.h>

#define PIN_I1 5
#define PIN_I2 4
#define PIN_I3 3
#define PIN_I4 2
#define PIN_EA 6
#define PIN_EB 1

#define RIGHT 0
#define FORWARD 1
#define LEFT 2
#define BACKWARD 3
#define BRAKE 4

#define N_MAGS 16

#define ROLE HOST

String state;

int speed_current;
int direction_current;
int update_speed;
int update_direction;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_I1, OUTPUT);
  pinMode(PIN_I2, OUTPUT);
  pinMode(PIN_I3, OUTPUT);
  pinMode(PIN_I4, OUTPUT);
  pinMode(PIN_EA, OUTPUT);
  pinMode(PIN_EB, OUTPUT);
  speed_current = 0;
  direction_current = 5; // neither forward, backwards, left, right, or brake
  RFduinoGZLL.begin(ROLE);
}

void loop()
{
  /*
  delay(10000);
  controlCar(0x1F);
  delay(1000);
  controlCar(0x3F);
  delay(1000);
  controlCar(0x0F);
  delay(1000);
  controlCar(0x2F);
  delay(1000);
  controlCar(0x40);
  */
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  state = String(data);
  controlCar(state.toInt());
}

void controlCar(int update)
{
  update_speed = update % N_MAGS;
  update_direction = (update - update_speed) / N_MAGS;
  
  if (update_speed == 0)
  {
    brake();
    return;
  }
  
  switch (update_direction)
  {
    case RIGHT:
      update_speed = round(11.0 * 17.0 + double(update_speed) * 68.0 / 15.0);
      goRight(update_speed);
      break;
    case FORWARD:
      update_speed = round(6.0 * 17.0 + double(update_speed) * 10.2);
      goForward(update_speed);
      break;
    case LEFT:
      update_speed = round(11.0 * 17.0 + double(update_speed) * 68.0 / 15.0);
      goLeft(update_speed);
      break;
    case BACKWARD:
      update_speed = round(6.0 * 17.0 + double(update_speed) * 10.2);
      goBackward(update_speed);
      break;
    default:
      brake();
      break;
  }
}

void newSpeed(int speed_new)
{
  if (speed_new != speed_current)
  {
    analogWrite(PIN_EA, speed_new);
    analogWrite(PIN_EB, speed_new);
    speed_current = speed_new;
  }
}

void brake()
{
  newSpeed(0);
  if (direction_current != BRAKE)
  {
    digitalWrite(PIN_I1, HIGH);
    digitalWrite(PIN_I2, HIGH);
    digitalWrite(PIN_I3, HIGH);
    digitalWrite(PIN_I4, HIGH);
    direction_current = BRAKE;
  }
}

void goForward(int speed_new)
{
  newSpeed(speed_new);
  if (direction_current != FORWARD)
  {
    digitalWrite(PIN_I1, LOW);
    digitalWrite(PIN_I2, HIGH);
    digitalWrite(PIN_I3, HIGH);
    digitalWrite(PIN_I4, LOW);
    direction_current = FORWARD;
  }
}

void goBackward(int speed_new)
{
  newSpeed(speed_new);
  if (direction_current != BACKWARD)
  {
    digitalWrite(PIN_I1, HIGH);
    digitalWrite(PIN_I2, LOW);
    digitalWrite(PIN_I3, LOW);
    digitalWrite(PIN_I4, HIGH);
    direction_current = BACKWARD;
  }
}

void goLeft(int speed_new)
{
  newSpeed(speed_new);
  if (direction_current != LEFT)
  {
    digitalWrite(PIN_I1, HIGH);
    digitalWrite(PIN_I2, LOW);
    digitalWrite(PIN_I3, HIGH);
    digitalWrite(PIN_I4, LOW);
    direction_current = LEFT;
  }
}

void goRight(int speed_new)
{
  newSpeed(speed_new);
  if (direction_current != RIGHT)
  {
    digitalWrite(PIN_I1, LOW);
    digitalWrite(PIN_I2, HIGH);
    digitalWrite(PIN_I3, LOW);
    digitalWrite(PIN_I4, HIGH);
    direction_current = RIGHT;
  }
}
