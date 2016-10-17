#include <cstring>

#include <QtSerialPort>

#include "arduinodata.h"
#include "arduinohandler.h"

using namespace std;

const int ArduinoHandler::SERIAL_TIMEOUT = 20000; // Milliseconds

ArduinoHandler::ArduinoHandler()
{
	reading = false;

	memset(data.samples, 0, sizeof(int) * SENSOR_TOTAL_SAMPLES);

	samplingThread = NULL;

	serialPort = NULL;
}

ArduinoHandler::ArduinoHandler(string serialPortName)
{
	this->serialPortName = serialPortName;

	ArduinoHandler();
}

ArduinoHandler::~ArduinoHandler()
{
	if (samplingThread != NULL)
	{
		delete samplingThread;
	}

	if (serialPort != NULL)
	{
		delete samplingThread;
	}
}

string ArduinoHandler::getSerialPortName()
{
	return serialPortName;
}

void ArduinoHandler::setSerialPortName(string serialPortName)
{
	this->serialPortName = serialPortName;
}

void ArduinoHandler::openSerial()
{
	if (serialPort != NULL)
	{
		throw "Serial port already opened.";
	}

	serialPort = new QSerialPort(serialPortName.c_str());

	serialPort->open(QSerialPort::ReadWrite);
	serialPort->setBaudRate(QSerialPort::Baud115200);
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);

	if (!serialPort->isOpen())
	{
		delete serialPort;

		throw "Serial port is not open.";
	}

	if (!serialPort->isReadable())
	{
		delete serialPort;

		throw "Serial port is not redeable.";
	}
}

void ArduinoHandler::closeSerial()
{
	if (serialPort == NULL)
	{
		throw "Serial port is not opened.";
	}

	delete serialPort;
	serialPort = NULL;
}

template<typename T, typename S>
void ArduinoHandler::startReading(const T& t,
								  function<void(const T&,  char, unsigned char *)> copyData,
								  const S& s,
								  function<void(const S&, int, int)> copyCoordinates)
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

	samplingThread = new std::thread(&ArduinoHandler::read, this, t, copyData);
}

void ArduinoHandler::stopReading()
{
	if (samplingThread == NULL)
	{
		throw "Serial port is not reading.";
	}

	reading = false;

	samplingThread->join();

	delete samplingThread;
	samplingThread = NULL;

	delete serialPort;
	serialPort = NULL;
}

template<typename T, typename S>
void ArduinoHandler::read(const T& t,
						  function<void(const T&, char, unsigned char *)> copyData,
						  const S& s,
						  function<void(const S&, int, int)> copyCoordinates)
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

				copyData(t, data.id, data.samples);
			}
		}
		else if (type == 2)
		{
			while (serialPort->bytesAvailable() <= twoSize)
			{
				serialPort->read(reinterpret_cast<char *>(&data), twoSize);

				copyCoordinates(s, data.lng, data.lat);
			}
		}
	}

	serialPort->write("\0");
}
