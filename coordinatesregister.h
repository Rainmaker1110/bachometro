#ifndef COORDINATESREGISTER_H
#define COORDINATESREGISTER_H

#include <string>

using namespace std;

class CoordinatesRegister
{
	private:
		int lng;
		int lat;

		string host;

	public:
		CoordinatesRegister();
		CoordinatesRegister(string host);
		~CoordinatesRegister();

		int getLng();
		void setLng(int lng);

		int getLat();
		void setLat(int lat);

		string getHost();
		void setHost(string host);

		void setCoordinates(int lng, int lat);
		string sendCoordinates();
};

#endif // COORDINATESREGISTER_H
