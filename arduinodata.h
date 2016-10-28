#ifndef ARDUINODATA_H
#define ARDUINODATA_H

static const char SENSOR_FIRST_ID = 'A';

static const int SENSOR_TOTAL_SAMPLES = 100;

typedef struct SensorData
{

		char id;
		unsigned char samples[SENSOR_TOTAL_SAMPLES];
} SensorData;

typedef struct GPSData
{
	int lng;
	int lat;
} GPSData;

#endif // ARDUINODATA_H
