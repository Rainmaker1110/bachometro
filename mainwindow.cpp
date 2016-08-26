#include <string>

#include <thread>

#include <QTimer>

#include <QDebug>
#include <QDateTime>

#include <QFileDialog>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sgsmooth.h"

using namespace std;

const int MAX_READ = 1000;

bool pothole;

QVector<double> xData(MAX_READ, 0);
QVector<double> yData(MAX_READ, 0);

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
	ui->leFile->setText(QDir::homePath() + "/datos01_01.dat");

	capture = false;
	capturer = NULL;

	dataIndex = 0;

	ui->cmbxWindow->addItem("1");
	ui->cmbxWindow->addItem("2");
	ui->cmbxWindow->addItem("3");
	ui->cmbxWindow->addItem("4");
	ui->cmbxWindow->addItem("5");
	ui->cmbxWindow->addItem("6");
	ui->cmbxWindow->addItem("7");
	ui->cmbxWindow->addItem("8");
	ui->cmbxWindow->addItem("9");

	ui->cmbxGrade->addItem("1");
	ui->cmbxGrade->addItem("2");
	ui->cmbxGrade->addItem("3");
	ui->cmbxGrade->addItem("4");
	ui->cmbxGrade->addItem("5");
	ui->cmbxGrade->addItem("6");
	ui->cmbxGrade->addItem("7");
	ui->cmbxGrade->addItem("8");

	ui->customPlot->setInteraction(QCP::iRangeDrag, true);
	ui->customPlot->setInteraction(QCP::iRangeZoom, true);

	ui->customPlot->addGraph();
	ui->customPlot->addGraph();
	ui->customPlot->graph(1)->setPen(QPen(Qt::red));

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("Medicion");
	ui->customPlot->yAxis->setLabel("Valor");

	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, 400);
	ui->customPlot->yAxis->setRange(0, 120);
	ui->customPlot->replot();

	ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
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

	QSerialPort serial("/dev/ttyACM0");
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


		if (dataIndex < MAX_READ)
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
		}

		qDebug() << xData[dataIndex] << " " << yData[dataIndex];

		//qDebug() << sensor[dataIndex].pothole << sensor[dataIndex].value << endl;

		dataIndex++;

		ui->lblCmNum->setText(QString::number(data));
		//data &= 255;
	}

	qDebug() << endl;
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

	fileString = QFileDialog::getOpenFileName(
				this,
				"Selecciona un archivo...",
				QDir::homePath(),
				tr("Data (*.dat);;Todos los archivos (*.*)"));

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

void MainWindow::on_btnExport_clicked()
{
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
