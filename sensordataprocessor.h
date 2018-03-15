#ifndef SENSORDATAPROCESSOR_H
#define SENSORDATAPROCESSOR_H

#include <vector>

#include "arduinodata.h"

using namespace std;

class SensorDataProcessor
{
	private:
		unsigned int window;
		unsigned int order;

		vector<double> sensorsData;
		vector<double> filterData;

	public:
		static void savgol(vector<double>& data);

		SensorDataProcessor();
		~SensorDataProcessor();

		unsigned int getWindow();
		void setWindow(unsigned int window);

		unsigned int getOrder();
		void setOrder(unsigned int order);

		void setSensorData(char id, unsigned short * data);
};

#endif // SENSORDATAPROCESSOR_H
