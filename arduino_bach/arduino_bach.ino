/* --- INCLUDES --- */
#include <TinyGPS.h>

/* --- DEFINES --- */

#define SENSORS			3
#define SAMPLES_SIZE	101
#define IN_TIMEOUT  4000

/* -- Ultrasonic sensor pins -- */
// ECHOS
#define ECHO1	11
#define ECHO2	9
#define ECHO3	7
#define ECHO4	5
#define ECHO5	3

// TRIGGERS
#define TRIGGER1	12
#define TRIGGER2	10
#define TRIGGER3	8
#define TRIGGER4	6
#define TRIGGER5	4

/* -- LED -- */
#define LED		13

/* -- LCD pins -- */
#define RS	7
#define E	8
#define D4	9
#define D5	10
#define D6	11
#define D7	12
// R/W to ground

/* --- TYPEDEFS --- */
typedef struct ArduinoData
{
	// Sensor ID
	char id;

	// Dummy data
	char dummy;

	// Samples buffer
	int values[SAMPLES_SIZE];

	// GPS data
	long lng;
	long lat;
} ArduinoData;

/* --- CONSTANTS --- */
const char echoPins[] = {
	ECHO1, ECHO2, ECHO3, ECHO4, ECHO5
};

const char triggerPins[] = {
	TRIGGER1, TRIGGER2, TRIGGER3, TRIGGER4, TRIGGER5
};

// Range of distance for ultrasonic sensor
const int MIN_DISTANCE = 3;
const int MAX_DISTANCE = 30;

const byte SENSOR_DATA = 1;
const byte GPS_DATA = 2;

/* --- GLOBAL VARIABLES --- */
byte sensorIndex;

byte bufferCount[SENSORS];

unsigned char pcReady;

int gpsData;

long distance;
long utime;

unsigned long fix_age;
unsigned long gpsSpeed;

TinyGPS gps;

ArduinoData data[SENSORS];

void setup()
{
	/* -- Serial initialization -- */
	Serial.begin(115200); // PC comunication
	Serial1.begin(38400); // GPS baud rate

	/* --- PIN MODES --- */

	pinMode(LED, OUTPUT);

	/* -- Ultrasonic sensor pins -- */
	for (int i = 0; i < SENSORS; i++)
	{
		pinMode(echoPins[i], INPUT);
	}

	for (int i = 0; i < SENSORS; i++)
	{
		pinMode(triggerPins[i], OUTPUT);
	}

	// Initialize bufferCount
	memset(bufferCount, 0, sizeof(char) * 3);

	// Sensors ID
	data[0].id = 'A';
	data[1].id = 'B';
	data[2].id = 'C';

	// Initialize variables;
	pcReady = false;

	sensorIndex = 0;
}

void loop()
{
	/* --- GETTING GPS DATA --- */
	while (Serial1.available())
	{
		gps.encode(Serial1.read());
	}

	// Get encoded data
	gps.get_position(&(data[sensorIndex].lat), &(data[sensorIndex].lng), &fix_age);

	/* --- ULTRASONIC SENSOR HANDLING --- */
	digitalWrite(LED, LOW);
	
	// 10us ultrasonic cycle
	digitalWrite(triggerPins[sensorIndex], LOW);
	digitalWrite(triggerPins[sensorIndex], HIGH);
	delayMicroseconds(10);

	digitalWrite(triggerPins[sensorIndex], LOW);

	// Calculate the distance (in cm) based on the speed of sound.
	utime = pulseIn(echoPins[sensorIndex], HIGH, IN_TIMEOUT);
	distance = utime / 58L;

	// Verifying distance is in range
	if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE)
	{
		digitalWrite(LED, HIGH);
    //delayMicroseconds(900);
		delay(1);

		data[sensorIndex].values[bufferCount[sensorIndex]] = utime;
		
		bufferCount[sensorIndex]++;

		// If buffer is full, it's send to computer
		if (bufferCount[sensorIndex] == SAMPLES_SIZE)
		{
			if (pcReady == 'Y')
			{
				Serial.write((const byte *) &(data[sensorIndex]), sizeof(ArduinoData));
			}

			bufferCount[sensorIndex] = 0;
		}
	}

	sensorIndex++;

	if (sensorIndex == SENSORS)
	{
		sensorIndex = 0;
	}

	if (SENSORS == 1)
	{
		delay(8);
	}
}

void serialEvent()
{
	// Enables or disables serial transmission
	pcReady = Serial.read();
}
