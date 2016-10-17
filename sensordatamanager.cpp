#include <fstream>

#include "sensordatamanager.h"

#include "sgsmooth.h"

using namespace std;

SensorDataManager::SensorDataManager(int sensorsNumber)
{
	filter = false;

	sensorsData.resize(sensorsNumber);

	for (vector<double>& v : sensorsData)
	{
		v.resize(SENSOR_TOTAL_SAMPLES);
	}
}

SensorDataManager::~SensorDataManager()
{

}

bool SensorDataManager::isFilter()
{
	return filter;
}

void SensorDataManager::setFilter(bool filter)
{
	this->filter = filter;
}

int SensorDataManager::getWindow()
{
	return window;
}

void SensorDataManager::setWindow(int window)
{
	this->window = window;
}

int SensorDataManager::getOrder()
{
	return order;
}

void SensorDataManager::setOrder(int order)
{
	this->order = order;
}

void SensorDataManager::setSensorData(char id, unsigned char * data)
{
	vector<double>& v = sensorsData[SENSOR_FIRST_ID - id];

	for (int i = 0; i < SENSOR_TOTAL_SAMPLES; i++)
	{
		v.push_back(data[i]);
	}

	if (filter)
	{
		calc_sgsmooth(v.size(), v.data(), window, order);
	}
}

vector<vector<double> >& SensorDataManager::getSensorsData()
{
	return sensorsData;
}

void SensorDataManager::writeToFile(string fileName)
{
	int size;

	ofstream file(fileName, ofstream::out | ofstream::binary);

	size = sensorsData.size();

	file.write(reinterpret_cast<char *>(&size), sizeof(int));

	for (vector<double>& v : sensorsData)
	{
		size = v.size();

		file.write(reinterpret_cast<char *>(&size), sizeof(int));

		for (double& d : v)
		{
			file.write(reinterpret_cast<char *>(&d), sizeof(double));
		}
	}

	file.close();
}

void SensorDataManager::readFromFile(string fileName)
{
	int size;

	double data;

	ifstream file(fileName, ifstream::in | ifstream::binary);

	file.read(reinterpret_cast<char *>(&size), sizeof(int));

	sensorsData.resize(size);

	for (unsigned int i = 0; i < sensorsData.size(); i++)
	{
		file.read(reinterpret_cast<char *>(&size), sizeof(int));

		sensorsData[i].resize(size);

		for (int j = 0; j < size; j++)
		{
			file.read(reinterpret_cast<char *>(&data), sizeof(double));

			sensorsData[i][j] = data;
		}
	}

	file.close();
}

void SensorDataManager::matlabExport(string fileName, int dataIndex)
{
	ofstream file(fileName);

	/*
	filename.replace(filename.size() - 4, 2, ".m");
	filename.resize(filename.size() - 2);
	*/

	file << "x = [";

	for (double& d : sensorsData[dataIndex])
	{
		file << to_string(d);

		file << " ";
	}

	file << "];\n";
	file << "[m, n] = size(x);\n";
	file << "if mod(n, 2) == 0\n";
	file << "m = n - 1;\n";
	file << "else;\n";
	file << "m = n;\n";
	file << "end\n";
	file << "f = sgolayfilt(x, 3, 5);\n";
	file << "x1 = x - f;\n";
	file << "subplot(3, 1, 1)\n";
	file << "plot(1:n, x)\n";
	file << "axis([0 n 0 120])\n";
	file << "grid\n";
	file << "subplot(3, 1, 2)\n";
	file << "plot(1:n, f)\n";
	file << "axis([0 n 0 120])\n";
	file << "grid\n";
	file << "subplot(3, 1, 3)\n";
	file << "plot(1:n, x1)\n";
	file << "axis([0 n 0 120])\n";
	file << "grid";

	file.flush();

	file.close();
}
