#include <cstring>

#include <QtSerialPort>

#include "arduinodata.h"
#include "arduinohandler.h"

using namespace std;

const int ArduinoHandler::SERIAL_TIMEOUT = 20000; // Milliseconds

ArduinoHandler::ArduinoHandler()
{
	reading = false;

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
		delete serialPort;
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

bool ArduinoHandler::isReading()
{
	return reading;
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

		serialPort = NULL;

		throw "Serial port is not open.";
	}

	if (!serialPort->isReadable())
	{
		delete serialPort;

		serialPort = NULL;

		throw "Serial port is not redeable.";
	}
}

void ArduinoHandler::closeSerial()
{
	if (serialPort == NULL)
	{
		throw "Serial port is not open.";
	}

	delete serialPort;
	serialPort = NULL;
}

void ArduinoHandler::stopReading()
{
	if (samplingThread == NULL || !reading)
	{
		throw "Serial port is not reading.";
	}

	reading = false;

	samplingThread->join();

	delete samplingThread;
	samplingThread = NULL;
}
