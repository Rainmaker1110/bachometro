#include <QEventLoop>

#include <QUrl>
#include <QUrlQuery>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "coordinatesregister.h"

CoordinatesRegister::CoordinatesRegister(string host)
{
	this->host = host;
}

CoordinatesRegister::~CoordinatesRegister()
{

}

string CoordinatesRegister::getHost()
{
	return host;
}

void CoordinatesRegister::setHost(string host)
{
	this->host = host;
}

string CoordinatesRegister::sendCoordinates(double lng, double lat)
{
	string response;

	QEventLoop eventLoop;

	QNetworkAccessManager mgr;

	QUrlQuery params;

	params.addQueryItem("lng", QString::number(lng));
	params.addQueryItem("lat", QString::number(lat));
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
