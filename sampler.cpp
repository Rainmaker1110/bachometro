#include <string.h>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "sampler.h"

const int Sampler::SERIAL_TIMEOUT = 5000;

Sampler::Sampler()
{
	reading = false;

	memset(samples.values, 0, sizeof(int) * SAMPLES_MAX_READ);

	samplingThread = NULL;
}

string Sampler::getSerialPortName()
{
	return serialPortName;
}

void Sampler::setSerialPortName(string serialPortName)
{
	this->serialPortName = serialPortName;
}

string Sampler::getSamplingFileName()
{
	return samplingFileName;
}

void Sampler::setSamplingFileName(string samplingFileName)
{
	this->samplingFileName = samplingFileName;
}

void Sampler::read()
{
	QSerialPort serialPort(serialPortName.c_str());

	serialPort.open(QSerialPort::ReadWrite);
	serialPort.setBaudRate(QSerialPort::Baud9600);
	serialPort.setDataBits(QSerialPort::Data8);
	serialPort.setParity(QSerialPort::NoParity);
	serialPort.setStopBits(QSerialPort::OneStop);
	serialPort.setFlowControl(QSerialPort::NoFlowControl);

	if (!(serialPort.isOpen() && serialPort.isReadable()))
	{
		return;
	}

	while (serialPort.waitForReadyRead(5000) && reading)
	{
		serialPort.read((char *) &samples, sizeof(sample_info));
	}
}

void Sampler::start()
{
	if (samplingThread == NULL)
	{
		reading = true;

		samplingThread = new std::thread(&Sampler::read, this);
	}
}

void Sampler::stop()
{
	if (samplingThread != NULL)
	{
		reading = false;

		samplingThread->join();

		delete samplingThread;
		samplingThread = NULL;
	}
}
