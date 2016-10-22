#ifndef ARDUINOHANDLER_H
#define ARDUINOHANDLER_H

#include <string>
#include <thread>

#include <QSerialPort>

#include "arduinodata.h"

using namespace std;

class ArduinoHandler
{
	private:
		static const int SERIAL_TIMEOUT;

		bool reading;

		arduino_data data;

		string serialPortName;

		thread * samplingThread;

		QSerialPort * serialPort;

		template<typename T, typename S>
		void read(T& t, S& s);

	public:
		ArduinoHandler();
		ArduinoHandler(string serialPortName);
		~ArduinoHandler();

		string getSerialPortName();
		void setSerialPortName(string serialPortName);

		bool isReading();

		void openSerial();
		void closeSerial();

		template<typename T, typename S>
		void startReading(T& t, S& s);
		void stopReading();
};

#endif // ARDUINOHANDLER_H
