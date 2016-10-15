#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <string>
#include <vector>

#include "sensordata.h"

using namespace std;

class SensorDataManager
{
	private:
		bool filter;

		int window;
		int order;

		vector<vector<double> > sensorsData;

	public:
		SensorDataManager(int sensorsNumber);
		~SensorDataManager();

		bool isFilter();
		void setFilter(bool filter);

		int getWindow();
		void setWindow(int window);

		int getOrder();
		void setOrder(int order);

		void setSensorData(sensor_data& data);

		vector<vector<double> >& getSensorsData();

		void savgol(vector<double>& data);

		void writeToFile(string fileName);
		void readFromFile(string fileName);

		void matlabExport(string fileName, int dataIndex);
};

#endif // SENSORDATAMANAGER_H
