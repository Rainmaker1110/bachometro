#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <string>
#include <vector>

#include "arduinodata.h"

using namespace std;

class SensorDataManager
{
	private:
		bool filter;
		bool detected;

		unsigned int frame;
		unsigned int order;

		unsigned int threshold;

		vector<double> average;
		vector<int> avgCount;

		vector<vector<double> > sensorsData;

	public:
		SensorDataManager();
		SensorDataManager(unsigned int sensorsNum);
		~SensorDataManager();

		bool isFilter();
		void setFilter(bool filter);

		bool isDetected();
		void setDetected(bool filter);

		unsigned int getFrame();
		void setFrame(unsigned int frame);

		unsigned int getOrder();
		void setOrder(unsigned int order);

		unsigned int getThreshold();
		void setThreshold(unsigned int threshold);

		unsigned int getSensosrNum();
		void setSensorsNum(unsigned int sensorsNum);

		void setSensorData(char id, unsigned short * data);
		vector<double>& getSensorsData(unsigned int index);

		void writeToFile(string fileName);
		void readFromFile(string fileName);

		void matlabExport(string fileName, unsigned int dataIndex);
};

#endif // SENSORDATAMANAGER_H
