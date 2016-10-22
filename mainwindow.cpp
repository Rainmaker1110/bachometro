#include <string>

#include <thread>

#include <QDebug>
#include <QFileDialog>

#include <QSerialPortInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "arduinohandler.templates.hpp"

using namespace std;

// Expand if needed
const int MainWindow::COLORS[] = {Qt::blue,
								  Qt::red,
								  Qt::green,
								  Qt::cyan,
								  Qt::magenta,
								  Qt::yellow,
								  Qt::gray};

const int MainWindow::DEFAULT_SENSOR_NUMBER = 3;

const string MainWindow::DEFAULT_HOST = "http://rainmaker.host56.com/maps/add_pothole.php";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

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

	// Set initial graphs
	setGraphs(DEFAULT_SENSOR_NUMBER);
	dataManager.setSensorNumber(DEFAULT_SENSOR_NUMBER);

	coordsReg.setHost(DEFAULT_HOST);

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
	ui->customPlot->xAxis->setRange(0, 300);
	ui->customPlot->yAxis->setRange(0, 120);

	ui->customPlot->replot();

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
	if (!arduino.isReading())
	{
		ui->customPlot->xAxis->setRange(0, 300);

		try
		{
			arduino.startReading(dataManager, coordsReg);
		}
		catch (const char * exception)
		{
			qDebug() << exception << endl;

			QMessageBox::critical(
						this,
						"Error en puerto serial",
						exception);

			return;
		}

		plotTimer = new QTimer(this);
		connect(plotTimer, SIGNAL(timeout()), this, SLOT(plotGraphs()));

		plotTimer->setSingleShot(false);
		plotTimer->start(50);

		ui->btnCapturar->setText("Detener");
	}
	else
	{
		try
		{
			arduino.stopReading();
		}
		catch (const char * exception)
		{
			qDebug() << exception << endl;

			return;
		}

		plotGraphs(dataManager.getSensorsData());

		plotTimer->stop();
		delete plotTimer;

		ui->btnCapturar->setText("Capturar");
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

}

void MainWindow::on_btnLngLat_clicked()
{

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

void MainWindow::plotGraphs(vector<vector<double> >& vectors)
{
	for (unsigned int i = 0; i < vectors.size(); i++)
	{
		QVector<double> v;
		v.append(v);

		ui->customPlot->graph(i)->setData(xData, v);
	}

	ui->customPlot->replot();
}

void MainWindow::on_btnSave_clicked()
{
	dataManager.writeToFile(ui->leFile->text().toStdString());
}

void MainWindow::on_chbxFilter_clicked()
{
	if (ui->cmbxWindow->isEnabled())
	{
		ui->cmbxWindow->setEnabled(false);
		ui->cmbxOrder->setEditable(false);

		dataManager.setFilter(false);
	}
	else
	{
		ui->cmbxWindow->setEnabled(true);
		ui->cmbxOrder->setEditable(true);

		dataManager.setFilter(true);
	}
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

	dataManager.setSensorNumber(sensorsNum);
}

void MainWindow::on_btnHost_clicked()
{
	coordsReg.setHost(ui->leHost->text().toStdString());
}
