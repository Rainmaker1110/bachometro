#ifndef ARDUINODATA_H
#define ARDUINODATA_H

static const char SENSOR_FIRST_ID = 'A';

static const int SENSOR_TOTAL_SAMPLES = 103;

typedef struct ArduinoData
{
		char id;
		unsigned char samples[SENSOR_TOTAL_SAMPLES];
		int lng;
		int lat;
} ArduinoData;


#endif // ARDUINODATA_H
