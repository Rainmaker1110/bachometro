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

void SensorDataManager::setSensorData(sensor_data& data)
{
	vector<double>& v = sensorsData[SENSOR_FIRST_ID - data.id];

	for (int i : data.samples)
	{
		v.push_back(i);
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

void SensorDataManager::savgol(vector<double>& data)
{
	double sum;

	vector<double> dataCopy(data);

	sum = 0.0;

	// 2/3
	// -3 12 17 12 - 3 35
	// -2 3 6 7 6 3 -2 12
	// -21 14 39 54 59 54 39 14 -21 231

	// 4/5
	// 5 -30 75 131 75 -30 5 231
	// 15 -55 30 135 179 135 30 -55 15 429

	for (unsigned int i = 0; i < dataCopy.size(); i++)
	{
		sum += (static_cast<int>(i) - 2 < 0) ? dataCopy[i] : -3.0 * dataCopy[i - 2];
		sum += (static_cast<int>(i) - 1 < 0) ? dataCopy[i] : 12 * dataCopy[i - 1];

		sum += 17.0 * dataCopy[i];

		sum += (i + 1 >= dataCopy.size()) ? dataCopy[i] : 12.0 * dataCopy[i + 1];
		sum += (i + 2 >= dataCopy.size()) ? dataCopy[i] : -3.0 * dataCopy[i + 2];

		data[i] -= sum / 35.0;

		sum = 0;
	}

	/*
	for (int i = 0; i < size; i++)
	{
		sum += i - 3 < 0 ? 0.0 : 5.0 * data[i - 3];
		sum += i - 2 < 0 ? 0.0 : -30.0 * data[i - 2];
		sum += i - 1 < 0 ? 0.0 : 75 * data[i - 1];

		sum += 131.0 * data[i];

		sum += i + 1 >= size ? 0.0 : 75.0 * data[i + 1];
		sum += i + 2 >= size ? 0.0 : -30.0 * data[i + 2];
		sum += i + 3 >= size ? 0.0 : 5.0 * data[i + 3];

		data[i] -= sum / 231.0;

		qDebug() << data[i];

		sum = 0;
	}
	*/
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

		file.write(reinterpret_cast<char*>(&size), sizeof(int));

		for (double& d : v)
		{
			file.write(reinterpret_cast<char*>(&d), sizeof(double));
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
