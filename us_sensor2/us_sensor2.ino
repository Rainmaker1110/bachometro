#include <LiquidCrystal.h>

#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED

#define SAMPLES_MAX_READ 100

typedef struct sample_info
{
  char sensor_id;
  byte values[SAMPLES_MAX_READ];
} 
sample_info;

int minimumRange = 1; // Minimum range needed
int maximumRange = 200; // Maximum range needed

int index;

int pc_ready;

long distance;

sample_info sensorA;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin (115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)

  lcd.begin(16, 2);

  index = 0;

  pc_ready = 0;

  sensorA.sensor_id = 'A';
}

void loop()
{
  if (Serial.available())
  {
    pc_ready = Serial.read();
  }
  /* The following trigPin/echoPin cycle is used to determine the
   		distance of the nearest object by bouncing soundwaves off of it. */
  digitalWrite(trigPin, LOW);
  digitalWrite(LEDPin, HIGH);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = pulseIn(echoPin, HIGH) / 58L;

  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print(distance);

  if (distance >= minimumRange && distance <= maximumRange)
  {
    sensorA.values[index++] = (byte) distance;

    digitalWrite(LEDPin, LOW);

    /* Send the distance to the computer using Serial protocol, and
     			turn LED OFF to indicate successful reading. */
    if (index == SAMPLES_MAX_READ)
    {
      if (pc_ready)
      {
        Serial.write((const byte *) &sensorA, sizeof(sample_info));
        
        pc_ready = 0;
      }
      index = 0;
    }
  }

  //Delay 10ms before next reading.
  //delay(10);
}

