
static const int MAX_READ = 100;

#ifndef SAMPLER_H
#define SAMPLER_H

#include <thread>
#include <string>

using namespace std;

static const int SAMPLES_MAX_READ = 100;

typedef struct sample_info
{
		char sensor_id;
		unsigned char values[SAMPLES_MAX_READ];
} sample_info;

class Sampler
{
	private:
		static const int SERIAL_TIMEOUT;

		bool reading;

		sample_info samples;

		string serialPortName;
		string samplingFileName;

		thread * samplingThread;

		void read();

		void writeFile();

	public:
		Sampler();

		string getSerialPortName();
		void setSerialPortName(string serialPortName);

		string getSamplingFileName();
		void setSamplingFileName(string samplingFileName);

		void start();
		void stop();
};

#endif // SAMPLER_H
