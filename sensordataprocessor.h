#ifndef SENSORDATAPROCESSOR_H
#define SENSORDATAPROCESSOR_H

#include <vector>

using namespace std;

class SensorDataProcessor
{
	public:
		SensorDataProcessor();
		~SensorDataProcessor();

		void savgol(vector<double>& data);
};

#endif // SENSORDATAPROCESSOR_H
