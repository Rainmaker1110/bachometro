#ifndef ARDUINODATA_H
#define ARDUINODATA_H

static const char SENSOR_FIRST_ID = 'A';

static const int SENSOR_TOTAL_SAMPLES = 100;

typedef union arduino_data
{
		struct
		{
				char id;
				unsigned char samples[SENSOR_TOTAL_SAMPLES];
		};

		struct
		{
				int lng;
				int lat;
		};
} arduino_data;

#endif // ARDUINODATA_H
