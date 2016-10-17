#include <QEventLoop>

#include <QUrl>
#include <QUrlQuery>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "coordinatesregister.h"

CoordinatesRegister::CoordinatesRegister(string host)
{
	lng = 0;
	lat = 0;

	this->host = host;
}

CoordinatesRegister::~CoordinatesRegister()
{

}

int CoordinatesRegister::getLng()
{
	return lng;
}

void CoordinatesRegister::setLng(int lng)
{
	this->lng = lng;
}

int CoordinatesRegister::getLat()
{
	return lat;
}

void CoordinatesRegister::setLat(int lat)
{
	this->lat = lat;
}

string CoordinatesRegister::getHost()
{
	return host;
}

void CoordinatesRegister::setHost(string host)
{
	this->host = host;
}

void CoordinatesRegister::setCoordinates(int lng, int lat)
{
	this->lng = lng;
	this->lat = lat;
}

string CoordinatesRegister::sendCoordinates()
{
	double dlng;
	double dlat;

	dlng = static_cast<double>(lng) / 1000000.0;
	dlat = static_cast<double>(lat) / 1000000.0;

	string response;

	QEventLoop eventLoop;

	QNetworkAccessManager mgr;

	QUrlQuery params;

	params.addQueryItem("lng", QString::number(dlng));
	params.addQueryItem("lat", QString::number(dlat));
	params.addQueryItem("new_pothole", "Submit");

	QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

	QNetworkRequest req(QUrl(host.c_str()));

	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

	mgr.post(req, params.query(QUrl::FullyEncoded).toUtf8());

	QNetworkReply * reply = mgr.get(req);
	eventLoop.exec(); // blocks stack until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) // Success
	{
		response = "Success: ";
		response += reply->readAll().toStdString();
	}
	else // Failure
	{
		response = "Failure :";
		response += reply->errorString().toStdString();
	}

	delete reply;

	return response;
}
