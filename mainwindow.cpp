#include <thread>

#include <QTimer>

#include <QDebug>
#include <QDateTime>

#include <QFileDialog>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

const int MAX_READ = 1024 * 1024;

bool pothole;

QVector<double> xData(500, 0);
QVector<double> yData(500, 0);

QVector<double> xPothole(0);
QVector<double> yPothole(0);

QTimer * timer;

typedef struct sensor_data
{
	int value;
	bool pothole;
} sensor_data;

bool capture;
thread * capturer;

int dataIndex;
sensor_data sensor[MAX_READ];

FILE * file;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->leFile->setText("C:\\datos01-01.dat");

	capture = false;
	capturer = NULL;

	dataIndex = 0;

	ui->customPlot->addGraph();
	ui->customPlot->addGraph();
	ui->customPlot->graph(1)->setPen(QPen(Qt::red));

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("Medicion");
	ui->customPlot->yAxis->setLabel("Valor");

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
		pothole = false;
		ui->customPlot->xAxis->setRange(0, 400);

		capture = true;
		capturer = new std::thread(&MainWindow::capture_data, this);

		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(plotGraph()));

		timer->setSingleShot(false);
		timer->start(50);

		ui->btnCapturar->setText("Detener");
		ui->btnPothole->setEnabled(true);
	}
	else if (capturer != NULL)
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

		file = fopen(ui->leFile->text().toStdString().c_str(), "wb");

		fwrite(sensor, sizeof(sensor_data), dataIndex, file);

		dataIndex = 0;

		fclose(file);

		xData.fill(0);
		yData.fill(0);

		ui->btnCapturar->setText("Capturar");
		ui->btnPothole->setEnabled(false);
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

	xPothole.fill(0, 0);
	yPothole.fill(0, 0);

	while (serial.waitForReadyRead(5000) && capture)
	{
		serial.read((char *) &data, sizeof(unsigned int));

		sensor[dataIndex].value = data & 0xFF; // 0xFFFF
		sensor[dataIndex].pothole = pothole;


		xData[dataIndex] = dataIndex;
		yData[dataIndex] = sensor[dataIndex].value;

		if (pothole)
		{
			xPothole.append(dataIndex);
			yPothole.append(sensor[dataIndex].value);
		}

		qDebug() << xData[dataIndex] << " " << yData[dataIndex] << endl;

		qDebug() << sensor[dataIndex].pothole << sensor[dataIndex].value << endl;

		dataIndex++;

		ui->lblCmNum->setText(QString::number(data));
		//data &= 255;
	}
}

void MainWindow::plotGraph()
{
	ui->customPlot->graph(0)->setData(xData, yData);
	ui->customPlot->graph(1)->setData(xPothole, yPothole);
	ui->customPlot->replot();
}

void MainWindow::on_btnFile_clicked()
{
	QString fileString;

	fileString = QFileDialog::getOpenFileName(this, "Selecciona un archivo...", QDir::rootPath(), tr("Data (*.dat)"));

	ui->leFile->setText(fileString);
}

void MainWindow::on_btnPlot_clicked()
{
	FILE * file;

	sensor_data * data;

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
		qDebug() << data[i].pothole << " - " << data[i].value << endl;
		x[i] = i; // x goes from -1 to 1
		y[i] = data[i].value; // let's plot a quadratic function

		if (data[i].pothole)
		{
			xPothole.append(i);
			yPothole.append(data[i].value);
		}
	}

	// create graph and assign data to it:
	ui->customPlot->graph(0)->setData(x, y);
	ui->customPlot->graph(1)->setData(xPothole, yPothole);
	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, lectures);
	ui->customPlot->yAxis->setRange(50, 150);
	ui->customPlot->replot();
}

void MainWindow::on_btnPothole_clicked()
{
	if (pothole)
	{
		ui->btnPothole->setText("Bache");
	}
	else
	{
		ui->btnPothole->setText("No bache");
	}

	pothole = !pothole;
}
