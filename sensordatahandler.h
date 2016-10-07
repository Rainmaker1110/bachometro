#ifndef SENSORDATAHANDLER_H
#define SENSORDATAHANDLER_H

#include <thread>
#include <string>

#include <QSerialPort>

#include "sensordata.h"

using namespace std;

class SensorDataHandler
{
	private:
		static const int SERIAL_TIMEOUT;

		bool reading;

		sensor_data data;

		string serialPortName;

		thread * samplingThread;

		QSerialPort * serialPort;

		void read(function<void(char id, vector<int>&)> copyData);

		template<typename ... T>
		static void writeToFile(ofstream& file, vector<double>& v, T ... args);

	public:
		SensorDataHandler();
		SensorDataHandler(string serialPortName);
		~SensorDataHandler();

		string getSerialPortName();
		void setSerialPortName(string serialPortName);

		void openSerial();

		void startReading(function<void(char id, vector<int>&)> copyData);
		void stopReading();

		void closeSerial();

		template<typename ... T>
		static void writeToFile(string fileName, T ... args);
};

#endif // SENSORDATAHANDLER_H
