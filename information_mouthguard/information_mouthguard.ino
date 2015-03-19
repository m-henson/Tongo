#define DELAY 10

#define PIN_RIGHT A0
#define PIN_FORWARD A1
#define PIN_LEFT A2

#define RIGHT 0
#define FORWARD 1
#define LEFT 2
#define BACKWARD 3
#define BRAKE 4

#define V_MAX 260
#define V_MIN 20
#define V_DIV 16.0

#define N_MAGS 16

#define PIN_CUE_RIGHT 9
#define PIN_CUE_FORWARD 8
#define PIN_CUE_LEFT 7
#define PIN_CUE_BRAKE 10

#define PIN_CUE_0 5
#define PIN_CUE_1 6

#define PIN_WAIT_0 11
#define PIN_WAIT_1 12
#define PIN_WAIT_2 13


#define WAIT_0 600
#define WAIT_1 800
#define WAIT_2 1000
#define WAIT_STABLE 50

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

int time_diff;
boolean reading_0;
boolean reading_1;
boolean reading_2;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_RIGHT, INPUT);
  pinMode(PIN_FORWARD, INPUT);
  pinMode(PIN_LEFT, INPUT);
  pinMode(PIN_CUE_RIGHT, OUTPUT);
  pinMode(PIN_CUE_FORWARD, OUTPUT);
  pinMode(PIN_CUE_LEFT, OUTPUT);
  pinMode(PIN_CUE_BRAKE, OUTPUT);
  pinMode(PIN_WAIT_0, INPUT);
  pinMode(PIN_WAIT_1, INPUT);
  pinMode(PIN_WAIT_2, INPUT);
  digitalWrite(PIN_CUE_RIGHT, LOW);
  digitalWrite(PIN_CUE_FORWARD, LOW);
  digitalWrite(PIN_CUE_LEFT, LOW);
  digitalWrite(PIN_CUE_BRAKE, LOW);
  state_current = "";
  mag_current = -1;
  heading_last = BRAKE + 1;
  heading_stable = heading_last;
  heading_next = BRAKE;
  writeLed(heading_next, HIGH);
  time_last = millis();
  time_stable = -2 * WAIT_STABLE;
  time_next = time_last + 10000;
  test_state = TEST_STATE_TRANSITION;
  delaying = false;
  debugTest(false, millis(), time_next - time_last, heading_next);
  time_diff = 0;
}

void loop()
{
  delay(DELAY);
  
  reading_0 = digitalRead(PIN_WAIT_0);
  reading_1 = digitalRead(PIN_WAIT_1);
  reading_2 = digitalRead(PIN_WAIT_2);
  
  if (reading_0)
  {
    set_time_diff(WAIT_0);
  }
  else if (reading_1)
  {
    set_time_diff(WAIT_1);
  }
  else
  {
    set_time_diff(WAIT_2);
  }
  
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
      heading_next = random(BRAKE);
    }
    time_next = time_diff;
    time_last = millis();
    
    time_next += time_last;
    
    writeLed(heading_next, HIGH);
    
    debugTest(test_state == TEST_STATE_HOLD, time_last, time_next - time_last, heading_next);
    test_state = TEST_STATE_WAIT;
  }
  
  if (state_new != state_current)
  {
    state_current = state_new;
    mag_current = mag_new;
  }
}

void set_time_diff(int diff)
{
  if (diff != time_diff)
  {
    time_diff = diff;
    Serial.println(",");
    Serial.println(",");
    Serial.println(",");
    Serial.print("T = ");
    Serial.println(time_diff);
    Serial.println(",");
    Serial.println(",");
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
        digitalWrite(PIN_CUE_RIGHT, voltage);
        digitalWrite(PIN_CUE_FORWARD, LOW);
        digitalWrite(PIN_CUE_LEFT, LOW);
        digitalWrite(PIN_CUE_BRAKE, LOW);
        break;
      case FORWARD:
        digitalWrite(PIN_CUE_FORWARD, voltage);
        digitalWrite(PIN_CUE_RIGHT, LOW);
        digitalWrite(PIN_CUE_LEFT, LOW);
        digitalWrite(PIN_CUE_BRAKE, LOW);
        break;
      case LEFT:
        digitalWrite(PIN_CUE_LEFT, voltage);
        digitalWrite(PIN_CUE_FORWARD, LOW);
        digitalWrite(PIN_CUE_RIGHT, LOW);
        digitalWrite(PIN_CUE_BRAKE, LOW);
        break;
      case BRAKE:
        digitalWrite(PIN_CUE_BRAKE, voltage);
        digitalWrite(PIN_CUE_FORWARD, LOW);
        digitalWrite(PIN_CUE_LEFT, LOW);
        digitalWrite(PIN_CUE_RIGHT, LOW);
        break;
      default:
        Serial.print("ERROR: bad heading ");
        Serial.println(h);
        break;
    }
}
