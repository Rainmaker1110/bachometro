/* --- INCLUDES --- */
#include <LiquidCrystal.h>
#include <Servo.h>
#include <TinyGPS.h>

/* --- DEFINES --- */

#define SENSORS 3

#define LED 13

#define SAMPLES_SIZE  100

/* -- Ultrasonic sensor pins -- */
#define TRIGGER 6
#define ECHO1   5
#define ECHO2   4
#define ECHO3   3

/* -- Servo pins -- */
#define SERVO  4

/* -- LCD pins -- */
#define RS  12
#define E   11
#define D4  10
#define D5  9
#define D6  8
#define D7  7
// R/W to ground

/* --- TYPEDEFS --- */
typedef struct SensorData
{
  char id;
  byte values[SAMPLES_SIZE];
} SensorData;

/* --- CONSTANTS --- */
const char echoPins[3] = {ECHO1, ECHO2, ECHO3};

// Range of distance for ultrasonic sensor
const int minDistance = 1;
const int maxDistance = 200;

/* --- GLOBAL VARIABLES --- */
bool pcReady;

char indexes[SENSORS];

int pos;

long distance;

long lat;
long lng;

unsigned long speed;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

Servo servo;

TinyGPS gps;

SensorData sensor[SENSORS];

void setup()
{
  /* -- Serial initialization -- */
  Serial.begin(115200); // PC comunication
  Serial1.begin(38400); // GPS baud rate

  /* --- PIN MODES --- */

  pinMode(LED, OUTPUT);

  /* -- Ultrasonic sensor pins -- */
  pinMode(TRIGGER, OUTPUT);
  pinMode(echoPins[0], INPUT);
  pinMode(echoPins[1], INPUT);
  pinMode(echoPins[2], INPUT);

  // LCD Initialization
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); 

  // Attaching pin to servo
  servo.attach(SERVO);

  // Initialize indexes
  memset(index, 0, sizeof(char) * 3);

  sensor[0].id = 'A';
  sensor[1].id = 'B';
  sensor[2].id = 'C';

  pc_ready = false;
}

void loop()
{
  // put your main code here, to run repeatedly:

}
