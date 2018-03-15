#ifndef ARDUINODATA_H
#define ARDUINODATA_H

static const char SENSOR_FIRST_ID = 'A';

static const int SENSOR_TOTAL_SAMPLES = 101;

typedef struct ArduinoData
{
		// Sensor ID
		char id;

		// Dummy Data
		char dummy;

		// Samples buffer
		unsigned short samples[SENSOR_TOTAL_SAMPLES];

		// GPS data
		int lng;
		int lat;
} ArduinoData;


#endif // ARDUINODATA_H
