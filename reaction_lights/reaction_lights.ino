#define DELAY 10

#define PIN_CUE_RIGHT 10
#define PIN_CUE_FORWARD 9
#define PIN_CUE_LEFT 8
#define PIN_CUE_BRAKE 11

#define PIN_C0 2
#define PIN_C1 3

boolean c1;
boolean c0;

boolean c1_new;
boolean c0_new;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_C0, INPUT);
  pinMode(PIN_C1, INPUT);
  pinMode(PIN_CUE_RIGHT, OUTPUT);
  pinMode(PIN_CUE_FORWARD, OUTPUT);
  pinMode(PIN_CUE_LEFT, OUTPUT);
  pinMode(PIN_CUE_BRAKE, OUTPUT);
  digitalWrite(PIN_CUE_RIGHT, LOW);
  digitalWrite(PIN_CUE_FORWARD, LOW);
  digitalWrite(PIN_CUE_LEFT, LOW);
  digitalWrite(PIN_CUE_BRAKE, HIGH);
  c1 = HIGH;
  c0 = HIGH;
}

void loop()
{
  delay(DELAY);
  
  c1_new = digitalRead(PIN_C1);
  c0_new = digitalRead(PIN_C0);
  if (c1_new != c1 || c0_new != c0)
  {
    writeLed(c1, c0, LOW);
    writeLed(c1_new, c0_new, HIGH);
    c1 = c1_new;
    c0 = c0_new;
  }
}

void writeLed(boolean c0, boolean c1, boolean voltage)
{
  if (!c1 && !c0)
  {
    digitalWrite(PIN_CUE_RIGHT, voltage);
  }
  else if (!c1 && c0)
  {
    digitalWrite(PIN_CUE_FORWARD, voltage);
  }
  else if (c1 && !c0)
  {
    digitalWrite(PIN_CUE_LEFT, voltage);
  }
  else if (c1 && c0)
  {
    digitalWrite(PIN_CUE_BRAKE, voltage);
  }
}
