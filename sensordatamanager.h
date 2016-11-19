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

		unsigned int window;
		unsigned int order;

		vector<double> average;
		vector<int> avgCount;

		vector<vector<double> > sensorsData;

	public:
		SensorDataManager();
		SensorDataManager(unsigned int sensorsNum);
		~SensorDataManager();

		bool isFilter();
		void setFilter(bool filter);

		unsigned int getWindow();
		void setWindow(unsigned int window);

		unsigned int getOrder();
		void setOrder(unsigned int order);

		unsigned int getSensosrNum();
		void setSensorsNum(unsigned int sensorsNum);

		void setSensorData(char id, unsigned short * data);
		vector<double>& getSensorsData(unsigned int index);

		void writeToFile(string fileName);
		void readFromFile(string fileName);

		void matlabExport(string fileName, unsigned int dataIndex);
};

#endif // SENSORDATAMANAGER_H
