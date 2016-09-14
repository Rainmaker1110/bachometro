#include <LiquidCrystal.h>

/* --- DEFINES -- */
#define trigPin 6 // Trigger Pin
#define echoPin1 5 // Echo1 Pin
#define echoPin2 4 // Echo2 Pin
#define echoPin3 3 // Echo3 Pin
#define LEDPin 13 // Onboard LED

#define SAMPLES_MAX_READ 100

/* --- TYPEDEFS -- */
typedef struct sample_info
{
  char sensor_id;
  byte values[SAMPLES_MAX_READ];
}
sample_info;

const char echoPins[3] = {5, 4, 3};

const int minimumRange = 1; // Minimum range needed
const int maximumRange = 200; // Maximum range needed

/* --- VARIABLES --- */
bool pc_ready;

char sensorId;

char i;

char index[3];

long distance;

sample_info sensor[3];

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

void setup() {
  Serial.begin (115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPins[0], INPUT);
  pinMode(echoPins[1], INPUT);
  pinMode(echoPins[2], INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)

  lcd.begin(16, 2);

  pc_ready = false;

  memset(index, 0, sizeof(char) * 3);
  memset(sensor, 0, sizeof(char) * 3);

  sensor[0].sensor_id = 'A';
  sensor[1].sensor_id = 'B';
  sensor[2].sensor_id = 'C';
}

void loop()
{
  if (Serial.available())
  {
    pc_ready = Serial.read();
  }

  for (i = 0; i < 3; i++)
  {
    /* The following trigPin/echoPin cycle is used to determine the
         distance of the nearest object by bouncing soundwaves off of it. */
    digitalWrite(trigPin, LOW);
    digitalWrite(LEDPin, HIGH);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    //Calculate the distance (in cm) based on the speed of sound.
    distance = pulseIn(echoPins[i], HIGH) / 58L;

    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print(distance);

    if (distance >= minimumRange && distance <= maximumRange)
    {
      sensor[i].values[index[i]++] = (byte) distance;

      digitalWrite(LEDPin, LOW);

      /* Send the distance to the computer using Serial protocol, and
            turn LED OFF to indicate successful reading. */
      if (index[i] == SAMPLES_MAX_READ)
      {
        if (pc_ready)
        {
          Serial.write((const byte *) (sensor + i), sizeof(sample_info));
        }

        index[i] = 0;
      }
    }

    //Delay 10ms before next reading.
    //delay(10);
  }
}

