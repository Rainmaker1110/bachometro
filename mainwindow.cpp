#include <thread>

#include <QTimer>

#include <QDebug>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

const int MAX_READ = 1024 * 1024;

QVector<double> xData(500, 0);
QVector<double> yData(500, 0);

QTimer * timer;

typedef struct sensor_data
{
	long long int timestamp;
	int value;
} sensor_data;

bool capture;
thread * capturer;

int index;
sensor_data sensor[MAX_READ];

FILE * file;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->lneArchivo->setText("C:\\datos01-01.dat");

	capture = false;
	capturer = NULL;

	index = 0;

	ui->customPlot->addGraph();

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("x");
	ui->customPlot->yAxis->setLabel("y");

	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, 400);
	ui->customPlot->yAxis->setRange(70, 140);
	ui->customPlot->replot();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_btnCapturar_clicked()
{
	if (capturer == NULL)
	{
		capture = true;
		capturer = new std::thread(&MainWindow::capture_data, this);

		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(plotGraph()));

		timer->setSingleShot(false);
		timer->start(50);
	}
}

void MainWindow::on_btnDetener_clicked()
{
	if (capturer != NULL)
	{
		capture = false;
		capturer->join();

		delete capturer;
		capturer = NULL;
		ui->lblCmNum->setText("0");

		ui->customPlot->graph(0)->setData(xData, yData);

		ui->customPlot->replot();

		timer->stop();
		delete timer;

		file = fopen(ui->lneArchivo->text().toStdString().c_str(), "wb");

		fwrite(sensor, sizeof(sensor_data), index, file);

		index = 0;

		fclose(file);

		xData.fill(0);
		yData.fill(0);
	}
}

void MainWindow::capture_data()
{
	unsigned int data;

	QSerialPortInfo info("COM6");
	qDebug() << "Name		 : " << info.portName();
	qDebug() << "Manufacturer: " << info.manufacturer(); //if showing manufacturer, means Qstring &name is good
	qDebug() << "Busy		 : " << info.isBusy() << endl;

	QSerialPort serial("COM6");
	serial.open(QSerialPort::ReadWrite);
	serial.setBaudRate(QSerialPort::Baud9600);
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

	data = 0;

	while (serial.waitForReadyRead(5000) && capture)
	{
		serial.read((char *) &data, sizeof(unsigned int));

		sensor[index].timestamp = QDateTime::currentMSecsSinceEpoch();
		sensor[index].value = data & 0xFF; // 0xFFFF

		xData[index] = index;
		yData[index] = sensor[index].value;

		qDebug() << xData[index] << " " << yData[index] << endl;

		qDebug() << sensor[index].timestamp << sensor[index].value << endl;

		index++;

		ui->lblCmNum->setText(QString::number(data));
		//data &= 255;
	}
}

void MainWindow::plotGraph()
{
	ui->customPlot->graph(0)->setData(xData, yData);
	ui->customPlot->replot();
}
