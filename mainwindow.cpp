#include <string>

#include <thread>

#include <QDebug>
#include <QFileDialog>

#include <QSerialPortInfo>

#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "arduinodata.h"

using namespace std;

const int MainWindow::DEFAULT_SENSOR_NUMBER = 3;
const int MainWindow::SERIAL_TIMEOUT = 2000; // Millis

// Expand if needed
const QColor MainWindow::COLORS[] = {Qt::blue,
									 Qt::red,
									 Qt::green,
									 Qt::cyan,
									 Qt::magenta,
									 Qt::yellow,
									 Qt::gray};

const string MainWindow::DEFAULT_HOST = "http://rainmaker.host56.com/maps/add_pothole.php";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	reading = false;

	serialPort = NULL;

	plotTimer = NULL;

	// Large vector for x-axis values
	xData.resize(100000);

	// Fill x axis with increasing numbers, C++11
	iota(xData.begin(), xData.end(), 0);

	// File path
	ui->leFile->setText(QDir::homePath() + "/datos01_01.dat");

	// Set values for Savitzky-Golay Filter
	ui->cmbxWindow->addItems({"1", "2", "3", "4", "5", "6", "7", "8", "9"});
	ui->cmbxOrder->addItems({"1", "2", "3", "4", "5", "6", "7", "8"});

	ui->cmbxSensorsNum->addItems({"1", "2", "3", "4", "5"});

	// List available serial ports
	for (QSerialPortInfo& serialPortInfo : QSerialPortInfo::availablePorts())
	{
		ui->cmbxSerialPorts->addItem(serialPortInfo.portName());
	}

	ui->cmbxWindow->setCurrentText(QString::number(5));
	ui->cmbxOrder->setCurrentText(QString::number(3));

	dataManager.setWindow(5);
	dataManager.setOrder(3);

	ui->cmbxSensorsNum->setCurrentText(QString::number(DEFAULT_SENSOR_NUMBER));
	ui->leHost->setText(QString::fromStdString(DEFAULT_HOST));

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("Time");
	ui->customPlot->yAxis->setLabel("Distance");

	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(0, 800);
	ui->customPlot->yAxis->setRange(0, 120);

	ui->customPlot->replot();

	// Set user interaction
	ui->customPlot->setInteraction(QCP::iRangeDrag, true);
	ui->customPlot->setInteraction(QCP::iRangeZoom, true);

	ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal);

	// Set initial graphs
	dataManager.setSensorsNum(DEFAULT_SENSOR_NUMBER);

	coordsReg.setHost(DEFAULT_HOST);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_btnCapturar_clicked()
{

	ui->customPlot->xAxis->setRange(0, 800);

	if (!reading)
	{
		qDebug () << "BTN " << thread()->currentThreadId();

		plotTimer = new QTimer();

		plotTimer->setSingleShot(false);
		connect(plotTimer, SIGNAL(timeout()), this, SLOT(plotGraphs()));

		reading = true;

		readThread = QtConcurrent::run(this, &MainWindow::readData);

		plotTimer->start(500);
	}
	else
	{
		plotTimer->stop();

		delete plotTimer;
		plotTimer = NULL;

		reading = false;

		readThread.waitForFinished();

		plotGraphs();
	}
}

void MainWindow::on_btnFile_clicked()
{
	QString fileString;

	fileString = QFileDialog::getOpenFileName(
				this,
				"Selecciona un archivo...",
				QDir::homePath(),
				tr("Data (*.dat);;Todos los archivos (*.*)"));

	if (fileString == NULL)
	{
		ui->leFile->setText(QDir::homePath() + "/datos01_01.dat");
	}
	else
	{
		ui->leFile->setText(fileString);
	}
}

void MainWindow::on_btnPlot_clicked()
{
	dataManager.readFromFile(ui->leFile->text().toStdString());
	setGraphs(dataManager.getSensosrNum());
	plotGraphs();
}

void MainWindow::on_btnLngLat_clicked()
{
	coordsReg.sendCoordinates();
}

void MainWindow::on_btnSave_clicked()
{
	dataManager.writeToFile(ui->leFile->text().toStdString());
}

void MainWindow::on_cmbxWindow_currentIndexChanged(const QString &arg1)
{
	dataManager.setWindow(arg1.toInt());
}

void MainWindow::on_cmbxOrder_currentIndexChanged(const QString &arg1)
{
	dataManager.setOrder(arg1.toInt());
}

void MainWindow::on_cmbxSensorsNum_currentIndexChanged(const QString &arg1)
{
	int sensorsNum;

	sensorsNum = arg1.toInt();

	setGraphs(sensorsNum);

	dataManager.setSensorsNum(sensorsNum);
}

void MainWindow::on_btnHost_clicked()
{
	coordsReg.setHost(ui->leHost->text().toStdString());
}

void MainWindow::readData()
{
	int arduinoSize = sizeof(ArduinoData);

	dataManager.setSensorsNum(ui->cmbxSensorsNum->currentText().toInt());

	serialPort = new QSerialPort(ui->cmbxSerialPorts->currentText());

	serialPort->open(QSerialPort::ReadWrite);
	serialPort->setBaudRate(QSerialPort::Baud115200);
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);

	if (!serialPort->isOpen())
	{
		serialPort->close();

		delete serialPort;
		serialPort = NULL;

		reading = false;

		QMessageBox::critical(this, "Serial port error.", "Serial port is closed.");

		return;
	}

	if (!serialPort->isReadable())
	{
		serialPort->close();

		delete serialPort;
		serialPort = NULL;

		reading = false;

		QMessageBox::critical(this, "Serial port error.", "Serial port is not readable (busy).");

		return;
	}

	ui->btnCapturar->setText("Detener");


	qDebug () << "readData" << thread()->currentThreadId();

	serialPort->clear();
	serialPort->write("Y");
	serialPort->flush();
	qDebug() << "Y";

	while (reading && serialPort->waitForReadyRead(5000))
	{
		if (serialPort->bytesAvailable() >= arduinoSize)
		{
			qDebug() << "read(): " << serialPort->read(reinterpret_cast<char *>(&arduino), arduinoSize);
			qDebug() << arduino.id;

			try
			{
				dataManager.setSensorData(arduino.id, arduino.samples);
				coordsReg.setCoordinates(arduino.lng, arduino.lat);

				qDebug() << arduino.lng << " " << arduino.lat;
			}
			catch (const char * exception)
			{
				qDebug() << exception;
			}
		}
	}

	serialPort->write("N");
	serialPort->flush();
	serialPort->close();
	qDebug() << "N";

	delete serialPort;
	serialPort = NULL;

	reading = false;
	ui->btnCapturar->setText("Capturar");
}

void MainWindow::plotGraphs()
{
	for (unsigned int i = 0; i < dataManager.getSensosrNum(); i++)
	{
		QVector<double> v;

		for (double d : dataManager.getSensorsData(i))
		{
			v.append(d);
		}

		ui->customPlot->graph(i)->setData(xData, v);
	}

	ui->customPlot->replot();
}

void MainWindow::setGraphs(int sensorsNum)
{
	ui->customPlot->clearGraphs();

	// Set graphs for QCustomPlot
	for (int i = 0; i < sensorsNum; i++)
	{
		// Add graph
		ui->customPlot->addGraph();

		// Set color
		ui->customPlot->graph(i)->setPen(QPen(COLORS[i]));

		// Set style
		ui->customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	}
}

void MainWindow::on_btnRefreshPorts_clicked()
{
	ui->cmbxSerialPorts->clear();

	for (QSerialPortInfo& serialPortInfo : QSerialPortInfo::availablePorts())
	{
		ui->cmbxSerialPorts->addItem(serialPortInfo.portName());
	}
}

void MainWindow::on_chbxGraph_clicked(bool checked)
{
	if (checked)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this,
									  "Habilitar graficación",
									  "Esto desactivará la detección, ¿desea continuar?",
									  QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			ui->leFile->setEnabled(true);
			ui->btnFile->setEnabled(true);
			ui->btnSave->setEnabled(true);
			ui->btnPlot->setEnabled(true);
			ui->chbxFilter->setEnabled(true);
			ui->customPlot->setEnabled(true);
		}
		else
		{
			ui->chbxGraph->setChecked(false);
		}
	}
	else
	{
		ui->leFile->setEnabled(false);
		ui->btnFile->setEnabled(false);
		ui->btnSave->setEnabled(false);
		ui->btnPlot->setEnabled(false);
		ui->chbxFilter->setChecked(false);
		ui->chbxFilter->setEnabled(false);
		ui->customPlot->setEnabled(false);
	}
}

void MainWindow::on_chbxFilter_toggled(bool checked)
{
	if (!checked)
	{
		ui->cmbxWindow->setEnabled(false);
		ui->cmbxOrder->setEnabled(false);

		dataManager.setFilter(false);
	}
	else
	{
		ui->cmbxWindow->setEnabled(true);
		ui->cmbxOrder->setEnabled(true);

		dataManager.setFilter(true);
	}
}
