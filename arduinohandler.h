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
		void read(const T& t,
				  function<void(const T&,  char, unsigned char *)> copyData,
				  const S& s,
				  function<void(const S&, int, int)> copyCoordinates);

	public:
		ArduinoHandler();
		ArduinoHandler(string serialPortName);
		~ArduinoHandler();

		string getSerialPortName();
		void setSerialPortName(string serialPortName);

		void openSerial();
		void closeSerial();

		template<typename T, typename S>
		void startReading(const T& t,
						  function<void(const T&,  char, unsigned char *)> copyData,
						  const S& s,
						  function<void(const S&, int, int)> copyCoordinates);
		void stopReading();
};

#endif // ARDUINOHANDLER_H
