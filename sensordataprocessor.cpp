#include "sensordataprocessor.h"

#include "sgsmooth.h"

void SensorDataProcessor::savgol(vector<double>& data)
{
	double sum;

	vector<double> dataCopy(data);

	sum = 0.0;

	/*
	* 2/3
	* -3 12 17 12 -3 35
	* -2 3 6 7 6 3 -2 12
	* -21 14 39 54 59 54 39 14 -21 231

	* 4/5
	* 5 -30 75 131 75 -30 5 231
	* 15 -55 30 135 179 135 30 -55 15 429
	*/

	for (unsigned int i = 0; i < dataCopy.size(); i++)
	{
		sum += (static_cast<int>(i) - 2 < 0) ? dataCopy[i] : -3.0 * dataCopy[i - 2];
		sum += (static_cast<int>(i) - 1 < 0) ? dataCopy[i] : 12.0 * dataCopy[i - 1];

		sum += 17.0 * dataCopy[i];

		sum += (i + 1 >= dataCopy.size()) ? dataCopy[i] : 12.0 * dataCopy[i + 1];
		sum += (i + 2 >= dataCopy.size()) ? dataCopy[i] : -3.0 * dataCopy[i + 2];

		data[i] -= sum / 35.0;

		sum = 0;
	}

	/*for (int i = 0; i < size; i++)
	{
		sum += i - 3 < 0 ? 0.0 : 5.0 * data[i - 3];
		sum += i - 2 < 0 ? 0.0 : -30.0 * data[i - 2];
		sum += i - 1 < 0 ? 0.0 : 75.0 * data[i - 1];

		sum += 131.0 * data[i];

		sum += i + 1 >= size ? 0.0 : 75.0 * data[i + 1];
		sum += i + 2 >= size ? 0.0 : -30.0 * data[i + 2];
		sum += i + 3 >= size ? 0.0 : 5.0 * data[i + 3];

		data[i] -= sum / 231.0;

		qDebug() << data[i];

		sum = 0;
	}*/
}

SensorDataProcessor::SensorDataProcessor()
{
	reset();
}

SensorDataProcessor::~SensorDataProcessor()
{

}

bool SensorDataProcessor::isDetected()
{
	return detected;
}

void SensorDataProcessor::setDetected(bool detected)
{
	this->detected = detected;
}

unsigned int SensorDataProcessor::getFrame()
{
	return frame;
}

void SensorDataProcessor::setFrame(unsigned int window)
{
	this->frame = window;
}

unsigned int SensorDataProcessor::getOrder()
{
	return order;
}

void SensorDataProcessor::setOrder(unsigned int order)
{
	this->order = order;
}

unsigned int SensorDataProcessor::getThreshold()
{
	return threshold;
}

void SensorDataProcessor::setThreshold(unsigned int threshold)
{
	this->threshold = threshold;
}

void SensorDataProcessor::reset()
{
	detected = false;

	sensorsData.resize(SENSOR_TOTAL_SAMPLES, 0.0);
	filterData.resize(SENSOR_TOTAL_SAMPLES, 0.0);

	average = 0.0;
	avgCount = 0;
}

void SensorDataProcessor::processData(unsigned short * data)
{
	for (unsigned int i = 0; i < SENSOR_TOTAL_SAMPLES; i++)
	{
		sensorsData[i] = static_cast<double>(data[i]) / 58.27;
		filterData[i] = sensorsData[i];
	}

	calc_sgsmooth(filterData.size(), filterData.data(), frame, order);

	if (avgCount < 5)
	{
		for (unsigned int i = 0; i < SENSOR_TOTAL_SAMPLES; i++)
		{
			average += filterData[i];
		}

		avgCount++;

		return;
	}
	else if (avgCount == 5)
	{
		average /= 505.0;
		avgCount++;

		return;
	}

	for (unsigned int i = 0; i < SENSOR_TOTAL_SAMPLES; i++)
	{
		filterData[i] -= average;

		if (filterData[i] >= threshold)
		{
			detected = true;
		}
	}
}
