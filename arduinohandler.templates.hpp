
#ifndef ARDUINOHANDLER_TEMPLATES_HPP
#define ARDUINOHANDLER_TEMPLATES_HPP

#include <QDebug>

#include "arduinohandler.h"

using namespace std;

template<typename T, typename S>
void ArduinoHandler::read(T& t, S& s)
{
	char type;

	int twoSize = sizeof(int) * 2;

	serialPort->write("Y");

	while (serialPort->waitForReadyRead(SERIAL_TIMEOUT) && reading)
	{

		serialPort->read(&type, sizeof(char));

		if (type == 1)
		{
			while (serialPort->bytesAvailable() <= SENSOR_TOTAL_SAMPLES)
			{
				serialPort->read(reinterpret_cast<char *>(&data), sizeof(arduino_data));

				t.setSensorData(data.id, data.samples);

				qDebug() << data.id << endl;
			}
		}
		else if (type == 2)
		{
			while (serialPort->bytesAvailable() <= twoSize)
			{
				serialPort->read(reinterpret_cast<char *>(&data), twoSize);

				s.setCoordinates(data.lng, data.lat);

				qDebug() << data.lng << " " << data.lat << endl;
			}
		}
	}

	serialPort->write("\0");
}

template<typename T, typename S>
void ArduinoHandler::startReading(T& t, S& s)
{
	if (serialPort == NULL)
	{
		throw "Serial port is not opened.";
	}

	if (samplingThread != NULL)
	{
		throw "Already reading from serial port.";
	}

	reading = true;

	memset(data.samples, 0, sizeof(char) * SENSOR_TOTAL_SAMPLES);

	samplingThread = new std::thread(&ArduinoHandler::read<T, S>, this, ref(t), ref(s));
}

#endif // ARDUINOHANDLER_TEMPLATES_HPP
