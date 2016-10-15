#ifndef SENSORDATA_H
#define SENSORDATA_H

static const char SENSOR_FIRST_ID = 'A';

static const int SENSOR_TOTAL_SAMPLES = 100;

typedef struct sensor_data
{
		char id;
		unsigned char samples[SENSOR_TOTAL_SAMPLES];
} sensor_data;

#endif // SENSORDATA_H
