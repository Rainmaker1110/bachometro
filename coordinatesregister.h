#ifndef COORDINATESREGISTER_H
#define COORDINATESREGISTER_H

#include <string>

using namespace std;

class CoordinatesRegister
{
	private:
		string host;

	public:
		CoordinatesRegister(string host);
		~CoordinatesRegister();

		string getHost();
		void setHost(string host);

		string sendCoordinates(double lng, double lat);
};

#endif // COORDINATESREGISTER_H
