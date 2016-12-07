#ifndef SENSORDATAPROCESSOR_H
#define SENSORDATAPROCESSOR_H

#include <vector>

#include "arduinodata.h"

using namespace std;

class SensorDataProcessor
{
	private:
		bool detected;

		unsigned int frame;
		unsigned int order;

		unsigned int threshold;

		unsigned int average;
		unsigned int avgCount;

		vector<double> sensorsData;
		vector<double> filterData;

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

		void reset();
		void processData(unsigned short * data);
};

#endif // SENSORDATAPROCESSOR_H
