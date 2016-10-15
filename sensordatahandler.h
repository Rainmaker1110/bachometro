#ifndef SENSORDATAHANDLER_H
#define SENSORDATAHANDLER_H

#include <string>
#include <thread>

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

		template<typename T>
		void read(const T& t, function<void(sensor_data&)> copyData);

	public:
		SensorDataHandler();
		SensorDataHandler(string serialPortName);
		~SensorDataHandler();

		string getSerialPortName();
		void setSerialPortName(string serialPortName);

		void openSerial();
		void closeSerial();

		template<typename T>
		void startReading(const T& t, function<void(sensor_data&)> copyData);
		void stopReading();
};

#endif // SENSORDATAHANDLER_H
