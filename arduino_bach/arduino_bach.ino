/* --- INCLUDES --- */
#include <LiquidCrystal.h>
#include <Servo.h>
#include <TinyGPS.h>

/* --- DEFINES --- */

#define SENSORS 3

#define SAMPLES_SIZE  100

/* -- Ultrasonic sensor pins -- */
#define ECHO1   3
#define ECHO2   4
#define ECHO3   5
#define TRIGGER 6

/* -- Servo pins -- */
#define SERVO  2

/* -- LED -- */
#define LED 13

/* -- LCD pins -- */
#define RS  7
#define E   8
#define D4  9
#define D5  10
#define D6  11
#define D7  12
// R/W to ground

/* --- TYPEDEFS --- */
typedef struct SensorData
{
  char id;
  byte values[SAMPLES_SIZE];
} 
SensorData;

typedef struct Position
{
  long lng;
  long lat;
} 
Position;

/* --- CONSTANTS --- */
const char echoPins[3] = {
  ECHO1, ECHO2, ECHO3};

// Range of distance for ultrasonic sensor
const int minDistance = 1;
const int maxDistance = 200;

/* --- GLOBAL VARIABLES --- */
bool pcReady;

bool ascending;

char bufferCount[SENSORS];

int gpsData;

int angle;

long distance;

unsigned long fix_age;
unsigned long gpsSpeed;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

Servo servo;

TinyGPS gps;

SensorData sensor[SENSORS];

Position pos;

void setup()
{
  /* -- Serial initialization -- */
  Serial.begin(115200); // PC comunication
  Serial1.begin(38400); // GPS baud rate

  /* --- PIN MODES --- */

  pinMode(LED, OUTPUT);

  /* -- Ultrasonic sensor pins -- */
  pinMode(echoPins[0], INPUT);
  pinMode(echoPins[1], INPUT);
  pinMode(echoPins[2], INPUT);
  pinMode(TRIGGER, OUTPUT);

  // LCD Initialization
  lcd.begin(16, 2);

  // Attaching pin to servo
  servo.attach(SERVO);

  // Initialize bufferCount
  memset(bufferCount, 0, sizeof(char) * 3);

  // Sensors ID
  sensor[0].id = 'A';
  sensor[1].id = 'B';
  sensor[2].id = 'C';

  // Initialize variables;
  pcReady = false;

  ascending = true;

  pos.lat = 0;
  pos.lng = 0;

  angle = 0;

  servo.write(angle);
}

void loop()
{
  // Enables or disables communication with computer
  if (Serial.available())
  {
    pcReady = Serial.read();
  }

  /* --- GETTING GPS DATA --- */
  while (Serial1.available())
  {
    digitalWrite(LED, HIGH);

    //gpsData = Serial1.read();

    gps.encode(Serial1.read());
  }

  digitalWrite(LED, LOW);

  // Get encoded data
  gps.get_position(&(pos.lat), &(pos.lng), &fix_age);

  // Sending GPS data (2)
  Serial.write(2);
  Serial.write((const byte *) &pos, sizeof(Position));

  /* --- SERVO MOVEMENT --- */
  angle += ascending ? 1 : -1;

  // 1 degree steps
  servo.write(angle);

  // Change direction
  if (angle == 180)
  {
    ascending = false;
  }

  if (angle == 0)
  {
    ascending = true; 
  }

  int i = 0;

  /* --- ULTRASONIC SENSOR HANDLING --- */
  //for (i = 0; i < 3; i++)
  //{
  // 10us ultrasonic cycle
  digitalWrite(TRIGGER, LOW);

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW);

  // Calculate the distance (in cm) based on the speed of sound.
  distance = pulseIn(echoPins[i], HIGH) / 58L;

  // Verifying distance is in range
  if (distance >= minDistance && distance <= maxDistance)
  {
    sensor[i].values[bufferCount[i]++] = (byte) distance;

    // If buffer is full, it's send to computer
    if (bufferCount[i] == SAMPLES_SIZE)
    {
      if (pcReady)
      {
        // Sending sensor data (1)
        Serial.write(1);
        Serial.write((const byte *) (sensor + i), sizeof(SensorData));
      }

      bufferCount[i] = 0;
    }
  }

  //Delay 10ms before next reading.
  //delay(500);
  //}

  /* --- LCD TESTING --- */

  // GPS
  /*
  lcd.setCursor(0, 0);
   lcd.print("LAT: ");
   lcd.print(pos.lat);
   
   lcd.setCursor(0, 1);
   lcd.print("LNG: ");
   lcd.print(pos.lng);
   */

  // SERVO
  /*
  lcd.setCursor(0, 0);
   lcd.print("ANGLE:    ");
   lcd.setCursor(7, 0);
   lcd.print(angle);
   */

  // ULSTRASONIC SENSOR

  lcd.setCursor(0, 0);
  lcd.print(sensor[i].id);
  lcd.print(":    ");
  lcd.setCursor(5, 0);
  lcd.print(distance);
}

















