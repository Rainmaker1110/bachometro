#include <cstring>

#include <QtSerialPort>

#include "sensordata.h"
#include "sensordatahandler.h"

using namespace std;

const int SensorDataHandler::SERIAL_TIMEOUT = 20000; // Milliseconds

SensorDataHandler::SensorDataHandler()
{
	reading = false;

	memset(data.samples, 0, sizeof(int) * SENSOR_TOTAL_SAMPLES);

	samplingThread = NULL;

	serialPort = NULL;
}

SensorDataHandler::SensorDataHandler(string serialPortName)
{
	this->serialPortName = serialPortName;

	SensorDataHandler();
}

SensorDataHandler::~SensorDataHandler()
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

string SensorDataHandler::getSerialPortName()
{
	return serialPortName;
}

void SensorDataHandler::setSerialPortName(string serialPortName)
{
	this->serialPortName = serialPortName;
}

void SensorDataHandler::openSerial()
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

void SensorDataHandler::closeSerial()
{
	if (serialPort == NULL)
	{
		throw "Serial port is not opened.";
	}

	delete serialPort;
	serialPort = NULL;
}

template<typename T>
void SensorDataHandler::startReading(const T& t, function<void(sensor_data&)> copyData)
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

	samplingThread = new std::thread(&SensorDataHandler::read, this, t, copyData);
}

void SensorDataHandler::stopReading()
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

template<typename T>
void SensorDataHandler::read(const T& t, function<void(sensor_data&)> copyData)
{
	serialPort->write("Y");

	while (serialPort->waitForReadyRead(SERIAL_TIMEOUT) && reading)
	{
		if (serialPort->bytesAvailable() >= SENSOR_TOTAL_SAMPLES + 1)
		{
			serialPort->read(reinterpret_cast<char *>(&data), sizeof(sensor_data));

			t.copyData(data);
		}
	}

	serialPort->write("\0");
}
