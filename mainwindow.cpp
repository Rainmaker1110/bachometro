#include <string>

#include <thread>

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sgsmooth.h"

#define SAMPLES_MAX_READ 100

using namespace std;

typedef struct sample_info
{
		char sensor_id;
		unsigned char values[SAMPLES_MAX_READ];
} sample_info;

QVector<double> xData(100000);

QVector<double> yData1(0, 0);
QVector<double> yData2(0, 0);
QVector<double> yData3(0, 0);

QTimer * timer;

bool capture;
thread * capturer;

FILE * file;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Set thread and stop condition
	capture = false;
	capturer = NULL;

	// Fill x axis with increasing numbers (C++11)
	iota(xData.begin(), xData.end(), 0);

	// File path
	ui->leFile->setText(QDir::homePath() + "/datos01_01.dat");

	// Set values for Savitzky-Golay Filter
	ui->cmbxWindow->addItems({"1", "2", "3", "4", "5", "6", "7", "8", "9"});
	ui->cmbxGrade->addItems({"1", "2", "3", "4", "5", "6", "7", "8"});

	// List available serial ports
	for (QSerialPortInfo &serialPortInfo : QSerialPortInfo::availablePorts())
	{
		ui->cmbxSerialPorts->addItem(serialPortInfo.portName());
	}

	// Set graphs for QCustomPlot
	ui->customPlot->addGraph();
	ui->customPlot->addGraph();
	ui->customPlot->addGraph();

	ui->customPlot->graph(1)->setPen(QPen(Qt::red));
	ui->customPlot->graph(2)->setPen(QPen(Qt::green));

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("Time");
	ui->customPlot->yAxis->setLabel("Distance");

	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, 300);
	ui->customPlot->yAxis->setRange(0, 120);
	ui->customPlot->replot();

	// Set style
	ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));

	// Set user interaction
	ui->customPlot->setInteraction(QCP::iRangeDrag, true);
	ui->customPlot->setInteraction(QCP::iRangeZoom, true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_btnCapturar_clicked()
{
	if (capturer == NULL)
	{
		ui->customPlot->xAxis->setRange(0, 300);

		capture = true;
		capturer = new std::thread(&MainWindow::capture_data, this);

		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(plotGraph()));

		timer->setSingleShot(false);
		timer->start(50);

		ui->btnCapturar->setText("Detener");
	}
	else if (capturer != NULL)
	{
		capture = false;
		capturer->join();

		delete capturer;
		capturer = NULL;
		ui->lblCmNum->setText("0");

		ui->customPlot->graph(0)->setData(xData, yData1);
		ui->customPlot->graph(1)->setData(xData, yData2);
		ui->customPlot->graph(2)->setData(xData, yData3);

		ui->customPlot->replot();

		timer->stop();
		delete timer;

		//file = fopen(ui->leFile->text().toStdString().c_str(), "wb");

		//fwrite(xData, sizeof(sensor_data), dataIndex, file);

		//fclose(file);

		yData1.fill(0, 0);
		yData2.fill(0, 0);
		yData3.fill(0, 0);

		ui->btnCapturar->setText("Capturar");
	}
}

void MainWindow::capture_data()
{
	sample_info sensor;

	string serialPortName = ui->cmbxSerialPorts->currentText().toStdString();

	QSerialPortInfo info(serialPortName.c_str());

	qDebug() << "Name		 : " << info.portName();
	qDebug() << "Manufacturer: " << info.manufacturer(); //if showing manufacturer, means Qstring &name is good
	qDebug() << "Busy		 : " << info.isBusy() << endl;

	QSerialPort serial(serialPortName.c_str());

	serial.open(QSerialPort::ReadWrite);
	serial.setBaudRate(QSerialPort::Baud115200);
	serial.setDataBits(QSerialPort::Data8);
	serial.setParity(QSerialPort::NoParity);
	serial.setStopBits(QSerialPort::OneStop);
	serial.setFlowControl(QSerialPort::NoFlowControl);

	if (!(serial.isOpen() && serial.isReadable()))
	{
		qDebug() << "No hay puerto.";

		return;
	}

	qDebug() << "Is open : " << serial.isOpen() << endl;
	qDebug() << "Is readable : " << serial.isReadable() << endl;

	yData1.fill(0, 0);
	yData2.fill(0, 0);
	yData3.fill(0, 0);

	int data_read;

	serial.write("Y");
	while (serial.waitForReadyRead(20000) && capture)
	{
		if (serial.bytesAvailable() >= SAMPLES_MAX_READ)
		{
			data_read = serial.read((char *) &sensor, sizeof(sample_info));

			qDebug() << sensor.sensor_id << " - " << data_read << endl;

			/*for (char c : sensorA.values)
		{
			qDebug() << (int) c << endl;
		}*/
			for (char v : sensor.values)
			{
				if (sensor.sensor_id == 'A')
				{
					yData1.append((double) v);
				}
				else if (sensor.sensor_id == 'B')
				{

					yData2.append((double) v);
				}
				else
				{
					yData3.append((double) v);
				}
			}

			// sensor[dataIndex].value = data & 0xFF; // 0xFFFF
			// sensor[dataIndex].pothole = pothole;


			/*if (dataIndex < MAX_READ)
		{
			xData[dataIndex] = dataIndex;
			yData[dataIndex] = sensor[dataIndex].value;
		}
		else
		{
			xData.append(dataIndex);
			yData.append(sensor[dataIndex].value);
		}

		if (pothole)
		{
			xPothole.append(dataIndex);
			yPothole.append(sensor[dataIndex].value);
		}*/

			//qDebug() << xData[dataIndex] << " " << yData[dataIndex];

			//qDebug() << sensor[dataIndex].pothole << sensor[dataIndex].value << endl;

			//dataIndex++;

			//ui->lblCmNum->setText(QString::number(data));
			//data &= 255;
		}
	}

	serial.write("\0");

	//qDebug() << endl;
}

void MainWindow::plotGraph()
{
	ui->customPlot->graph(0)->setData(xData, yData1);
	ui->customPlot->graph(1)->setData(xData, yData2);
	ui->customPlot->graph(2)->setData(xData, yData3);
	ui->customPlot->replot();
}

void MainWindow::on_btnFile_clicked()
{
	QString fileString;

	fileString = QFileDialog::getOpenFileName(
				this,
				"Selecciona un archivo...",
				QDir::homePath(),
				tr("Data (*.dat);;Todos los archivos (*.*)"));

	ui->leFile->setText(fileString);
}

void MainWindow::on_btnPlot_clicked()
{
	/*
	FILE * file;

	long long int fsize;
	int lectures;
	file = fopen(ui->leFile->text().toStdString().c_str(), "rb");

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	lectures = fsize / (long long int) sizeof(sensor_data);
	qDebug() << lectures << endl;
	data = (sensor_data *) malloc(sizeof(sensor_data) * lectures);

	fread(data, sizeof(sensor_data), lectures, file);
	fclose(file);

	QVector<double> x(lectures);
	QVector<double> y(lectures); // initialize with entries 0..100

	xPothole.fill(0, 0);
	yPothole.fill(0, 0);

	for (int i = 0; i < lectures; i++)
	{
		qDebug() << data[i].pothole << " - " << data[i].value;
		x[i] = i; // x goes from -1 to 1
		y[i] = data[i].value; // let's plot a quadratic function

		if (data[i].pothole)
		{
			xPothole.append(i);
			yPothole.append(data[i].value);
		}
	}

	qDebug() << endl;

	if (ui->chbxFilter->isChecked())
	{
		//savgol(y);

		double * ysmooth = new double[y.size()];

		for (int i = 0; i < y.size(); i++)
		{
			ysmooth[i] = y.at(i);
		}

		calc_sgsmooth(y.size(), ysmooth, ui->cmbxWindow->currentText().toInt(), ui->cmbxGrade->currentText().toInt());

		for (int i = 0; i < y.size(); i++)
		{
			y[i] -= ysmooth[i];
			qDebug() << y[i];
		}

		qDebug() << endl;

		delete ysmooth;
	}

	// create graph and assign data to it:
	ui->customPlot->graph(0)->setData(x, y);
	ui->customPlot->graph(1)->setData(xPothole, yPothole);
	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, lectures);
	ui->customPlot->yAxis->setRange(0, 120);
	ui->customPlot->replot();
*/
}

void MainWindow::on_btnExport_clicked()
{
	/*
	FILE * file;

	string filename;
	string matlab;

	sensor_data * data;

	long long int fsize;
	int lectures;

	filename = ui->leFile->text().toStdString();

	file = fopen(filename.c_str(), "rb");

	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	lectures = fsize / (long long int) sizeof(sensor_data);
	qDebug() << lectures << endl;
	data = (sensor_data *) malloc(sizeof(sensor_data) * lectures);

	fread(data, sizeof(sensor_data), lectures, file);
	fclose(file);

	filename.replace(filename.size() - 4, 2, ".m");
	filename.resize(filename.size() - 2);

	file = fopen(filename.c_str(), "w");

	matlab = "x = [";
	for (int i = 0; i < lectures; i++)
	{
		matlab += to_string(data[i].value);

		if (i < lectures - 1)
		{
			matlab += " ";
		}
	}
	matlab += "];\n";
	matlab += "[m, n] = size(x);\n";
	matlab += "if mod(n, 2) == 0\n";
	matlab += "m = n - 1;\n";
	matlab += "else;\n";
	matlab += "m = n;\n";
	matlab += "end\n";
	matlab += "f = sgolayfilt(x, 9, m);\n";
	matlab += "x1 = x - f;\n";
	matlab += "subplot(3, 1, 1)\n";
	matlab += "plot(1:n, x)\n";
	matlab += "axis([0 n 0 120])\n";
	matlab += "grid\n";
	matlab += "subplot(3, 1, 2)\n";
	matlab += "plot(1:n, f)\n";
	matlab += "axis([0 n 0 120])\n";
	matlab += "grid\n";
	matlab += "subplot(3, 1, 3)\n";
	matlab += "plot(1:n, x1)\n";
	matlab += "axis([0 n 0 120])\n";
	matlab += "grid";

	fprintf(file, "%s", matlab.c_str());

	fclose(file);

	QMessageBox::information(this,
							 "Exportacion finalizada",
							 "Datos exportados a MATLAB",
							 QMessageBox::NoButton);
							 */
}

void MainWindow::savgol(QVector<double>& data)
{
	int size;

	double sum;

	size = data.size();

	sum = 0;

	// 2/3
	// -3 12 17 12 - 3 35
	// -2 3 6 7 6 3 -2 12
	// -21 14 39 54 59 54 39 14 -21 231

	// 4/5
	// 5 -30 75 131 75 -30 5 231
	// 15 -55 30 135 179 135 30 -55 15 429

	for (int i = 0; i < size; i++)
	{
		sum += i - 2 < 0 ? 0.0 : -3.0 * data[i - 2];
		sum += i - 1 < 0 ? 0.0 : 12 * data[i - 1];

		sum += 17.0 * data[i];

		sum += i + 1 >= size ? 0.0 : 12.0 * data[i + 1];
		sum += i + 2 >= size ? 0.0 : -3.0 * data[i + 2];

		data[i] -= sum / 35.0;

		qDebug() << data[i];

		sum = 0;
	}
	/*
	for (int i = 0; i < size; i++)
	{
		sum += i - 3 < 0 ? 0.0 : 5.0 * data[i - 3];
		sum += i - 2 < 0 ? 0.0 : -30.0 * data[i - 2];
		sum += i - 1 < 0 ? 0.0 : 75 * data[i - 1];

		sum += 131.0 * data[i];

		sum += i + 1 >= size ? 0.0 : 75.0 * data[i + 1];
		sum += i + 2 >= size ? 0.0 : -30.0 * data[i + 2];
		sum += i + 3 >= size ? 0.0 : 5.0 * data[i + 3];

		data[i] -= sum / 231.0;

		qDebug() << data[i];

		sum = 0;
	}
*/
	qDebug() << endl;
}

void MainWindow::on_btnLngLat_clicked()
{
	int lng;
	int lat;

	lng = 0;
	lat = 0;

	string serialPortName = ui->cmbxSerialPorts->currentText().toStdString();

	QSerialPortInfo info(serialPortName.c_str());

	qDebug() << "Name		 : " << info.portName();
	qDebug() << "Manufacturer: " << info.manufacturer(); //if showing manufacturer, means Qstring &name is good
	qDebug() << "Busy		 : " << info.isBusy() << endl;

	QSerialPort serial(serialPortName.c_str());

	serial.open(QSerialPort::ReadWrite);
	serial.setBaudRate(QSerialPort::Baud115200);
	serial.setDataBits(QSerialPort::Data8);
	serial.setParity(QSerialPort::NoParity);
	serial.setStopBits(QSerialPort::OneStop);
	serial.setFlowControl(QSerialPort::NoFlowControl);

	if (!(serial.isOpen() && serial.isReadable()))
	{
		qDebug() << "No hay puerto.";

		return;
	}

	qDebug() << "Is open : " << serial.isOpen() << endl;
	qDebug() << "Is readable : " << serial.isReadable() << endl;

	serial.write("Y");

	while (serial.waitForReadyRead(2000))
	{
		if (serial.bytesAvailable() >= sizeof(int) * 2)
		{
			serial.read((char *) &lng, sizeof(int));
			serial.read((char *) &lat, sizeof(int));
		}
	}

	if (lat / 1000000 != 19)
	{
		return;
	}
	qDebug() << lng << " " << lat << endl;
	send_localization(lng, lat);
}

void MainWindow::send_localization(int lng, int lat)
{
	double dlng;
	double dlat;

	dlng = static_cast<double>(lng); // (double) lng;
	dlat = static_cast<double>(lat);

	dlng /= 1000000.0;
	dlat /= 1000000.0;


	QEventLoop eventLoop;
	QNetworkAccessManager mgr;

	QUrlQuery params;
	params.addQueryItem("lng", QString::number(dlng));
	params.addQueryItem("lat", QString::number(dlat));
	params.addQueryItem("new_pothole", "Submit");

	QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

	QNetworkRequest req(QUrl(QString("http://rainmaker.host56.com/maps/add_pothole.php")));

	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	mgr.post(req, params.query(QUrl::FullyEncoded).toUtf8());

	QNetworkReply *reply = mgr.get(req);
	eventLoop.exec(); // blocks stack until "finished()" has been called

	if (reply->error() == QNetworkReply::NoError) {
		//success
		qDebug() << "Success" << reply->readAll();
		delete reply;
	}
	else
	{
		//failure
		qDebug() << "Failure" << reply->errorString();
		delete reply;
	}
}
