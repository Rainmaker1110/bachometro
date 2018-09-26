#ifndef SENSORDATAPROCESSOR_H
#define SENSORDATAPROCESSOR_H

#include <vector>

#include "arduinodata.h"
#include "coordinatesregister.h"

using namespace std;

class SensorDataProcessor
{
	private:
		bool detected;

		time_t rawtime;
		struct tm * timeinfo;
		char buffer[150];
		char filename[150];

		unsigned int frame;
		unsigned int order;

		unsigned int threshold;

		unsigned int average;
		unsigned int avgCount;

		vector<double> sensorsData;
		vector<double> filterData;

		CoordinatesRegister * coords;

	public:
		static void savgol(vector<double>& data);

		SensorDataProcessor();
		~SensorDataProcessor();

		bool isDetected();
		void setDetected(bool filter);

		unsigned int getFrame();
		void setFrame(unsigned int frame);

		unsigned int getOrder();
		void setOrder(unsigned int order);

		unsigned int getThreshold();
		void setThreshold(unsigned int threshold);

		void setCoordsReg(CoordinatesRegister * coords);

		void reset();
		void processData(unsigned short * data);
};

#endif // SENSORDATAPROCESSOR_H
