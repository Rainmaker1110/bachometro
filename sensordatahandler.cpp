#include <cstring>

#include <iterator>

#include <fstream>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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
	serialPort = new QSerialPort(serialPortName.c_str());

	serialPort->open(QSerialPort::ReadWrite);
	serialPort->setBaudRate(QSerialPort::Baud115200);
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);

	if (!(serialPort.isOpen() && serialPort.isReadable()))
	{
		return;
	}
}

void SensorDataHandler::closeSerial()
{
	if (serialPort != NULL)
	{
		delete serialPort;
		serialPort = NULL;
	}
}

void SensorDataHandler::startReading(function<void(char id, vector<int>&)> copyData)
{
	if (samplingThread == NULL)
	{
		reading = true;

		samplingThread = new std::thread(&SensorDataHandler::read, this, copyData);
	}
}

void SensorDataHandler::stopReading()
{
	if (samplingThread != NULL)
	{
		reading = false;

		samplingThread->join();

		delete samplingThread;
		samplingThread = NULL;

		delete serialPort;
		serialPort = NULL;
	}
}

void SensorDataHandler::read(function<void(char id, vector<int>&)> copyData)
{
	while (serialPort->waitForReadyRead(SERIAL_TIMEOUT) && reading)
	{
		if (serialPort->bytesAvailable() >= SENSOR_TOTAL_SAMPLES + 1)
		{
			serialPort->read((char *) &data, sizeof(sensor_data));

			vector<int> buffer(begin(data.samples), end(data.samples));
			copyData(data.id, buffer);
		}
	}
}

template<typename ... T>
void SensorDataHandler::writeToFile(string fileName, T ... args)
{
	ofstream file(fileName, ofstream::out | ofstream::binary);

	writeToFile(file, args...);

	file.close();
}

template<typename ... T>
void SensorDataHandler::writeToFile(ofstream& file, vector<double>& v, T ... args)
{
	int vectorSize = v.size();

	file.write(static_cast<const char *>(&vectorSize), sizeof(int));
	file.write(static_cast<const char *>(v.data()), sizeof(double) * vectorSize);

	writeToFile(file, forward<T>(args)...);
}
