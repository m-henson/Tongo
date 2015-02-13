#include <RFduinoGZLL.h>

#define ROLE DEVICE0
#define DELAY 10

#define PIN_RIGHT 2
#define PIN_FORWARD 3
#define PIN_LEFT 4

#define RIGHT 0
#define FORWARD 1
#define LEFT 2
#define BACKWARD 3
#define BRAKE 4

#define V_MAX 77
#define V_MIN 2
#define V_DIV 5.0

#define N_MAGS 16


#define PIN_CUE_RIGHT 5
#define PIN_CUE_FORWARD 0
#define PIN_CUE_LEFT 6
#define PIN_CUE_BRAKE 1

#define PIN_CUE_0 5
#define PIN_CUE_1 6

#define WAIT_MIN 2000
#define WAIT_MAX 8000
#define WAIT_STABLE 100

#define TEST_STATE_WAIT 0
#define TEST_STATE_TRANSITION 1
#define TEST_STATE_HOLD 2
#define TEST_STATE_FAILED 3

int reading_right;
int reading_forward;
int reading_left;

String state_new;
String state_current;
int heading;
int mag_new;
int mag_current;


unsigned long time_stamp;
unsigned long time_stable;
unsigned long time_last;
unsigned long time_next;
int heading_stable;
int heading_last;
int heading_next;
int test_state;
boolean delaying;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_FORWARD, INPUT);
  pinMode(PIN_LEFT, INPUT);
  /*pinMode(PIN_CUE_RIGHT, OUTPUT);
  pinMode(PIN_CUE_FORWARD, OUTPUT);
  pinMode(PIN_CUE_LEFT, OUTPUT);
  pinMode(PIN_CUE_BRAKE, OUTPUT);
  digitalWrite(PIN_CUE_RIGHT, LOW);
  digitalWrite(PIN_CUE_FORWARD, LOW);
  digitalWrite(PIN_CUE_LEFT, LOW);
  digitalWrite(PIN_CUE_BRAKE, LOW);*/
  pinMode(PIN_CUE_0, OUTPUT);
  pinMode(PIN_CUE_1, OUTPUT);
  state_current = "";
  mag_current = -1;
  heading_last = BRAKE + 1;
  heading_stable = heading_last;
  heading_next = BRAKE;
  time_last = millis();
  time_stable = -2 * WAIT_STABLE;
  time_next = time_last + 10000;
  test_state = TEST_STATE_TRANSITION;
  delaying = false;
  debugTest(false, millis(), time_next - time_last, heading_next);
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
  
  
  time_stamp = millis();
  if (heading != heading_stable)
  {
    heading_stable = heading;
    time_stable = time_stamp;
    delaying = true;
  }
  else if (delaying && time_stamp - time_stable >= WAIT_STABLE)
  {
    delaying = false;
    
    if (test_state == TEST_STATE_WAIT || test_state == TEST_STATE_TRANSITION)
    {
      if (test_state == TEST_STATE_WAIT && heading_next != BRAKE && heading == BRAKE)
      {
        test_state = TEST_STATE_TRANSITION;
      }
      else if (heading == heading_next)
      {
        test_state = TEST_STATE_HOLD;
        debugResult(heading, 1, time_stable - time_last, false);
      }
      else
      {
        test_state = TEST_STATE_FAILED;
        debugResult(heading, 0, time_stable - time_last, true);
      }
    }
    else if (test_state == TEST_STATE_HOLD && heading != heading_next)
    {
      debugResult(heading, 0, time_stable - time_last, true);
      test_state = TEST_STATE_FAILED;
    }
  }
  
  time_stamp = millis();
  if (time_stamp >= time_next)
  {
    if (test_state == TEST_STATE_TRANSITION || test_state == TEST_STATE_WAIT)
    {
      test_state = TEST_STATE_FAILED;
      debugResult(heading_stable, 0, time_stable - time_last, true);
    }
    
    //writeLed(heading_last, LOW);
    
    heading_last = heading_next;
    while (heading_last == heading_next || heading_next == BACKWARD || heading_next == heading)
    {
      heading_next = random(BRAKE + 1);
    }
    time_next = random(WAIT_MIN, WAIT_MAX + 1);
    time_last = millis();
    
    time_next += time_last;
    
    writeLed(heading_next, HIGH);
    
    debugTest(test_state == TEST_STATE_HOLD, time_last, time_next - time_last, heading_next);
    test_state = TEST_STATE_WAIT;
  }
    
  if (state_new != state_current)
  {
    /*Serial.print(state_new);
    Serial.print(": ");
    Serial.print(heading);
    Serial.print(" -> ");
    Serial.println(mag_new);*/
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

void debugTest(boolean new_line, unsigned long t, long diff, int desired)
{
  if (new_line)
  {
    Serial.println(",");
  }
  Serial.print(t);
  Serial.print(",");
  Serial.print(diff);
  Serial.print(",");
  Serial.print(desired);
}

void debugResult(int result, int pass, long diff, boolean new_line)
{
  Serial.print(",");
  Serial.print(result);
  Serial.print(",");
  Serial.print(pass);
  Serial.print(",");
  if (new_line)
  {
    Serial.println(diff);
  }
  else
  {
    Serial.print(diff);
  }
}

void writeLed(int h, boolean voltage)
{
  switch (h)
    {
      case RIGHT:
        //digitalWrite(PIN_CUE_RIGHT, voltage);
        digitalWrite(PIN_CUE_0, LOW);
        digitalWrite(PIN_CUE_1, LOW);
        break;
      case FORWARD:
        //digitalWrite(PIN_CUE_FORWARD, voltage);
        digitalWrite(PIN_CUE_0, HIGH);
        digitalWrite(PIN_CUE_1, LOW);
        break;
      case LEFT:
        //digitalWrite(PIN_CUE_LEFT, voltage);
        digitalWrite(PIN_CUE_0, LOW);
        digitalWrite(PIN_CUE_1, HIGH);
        break;
      case BRAKE:
        //digitalWrite(PIN_CUE_BRAKE, voltage);
        digitalWrite(PIN_CUE_0, HIGH);
        digitalWrite(PIN_CUE_1, HIGH);
        break;
      default:
        Serial.print("ERROR: bad heading ");
        Serial.println(heading_next);
        break;
    }
}
