#include <string>

#include <thread>

#include <QDebug>
#include <QFileDialog>

#include <QSerialPortInfo>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

// Expand if needed
const int MainWindow::colors[] = {Qt::blue,
								  Qt::red,
								  Qt::green,
								  Qt::cyan,
								  Qt::magenta,
								  Qt::yellow,
								  Qt::gray};



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
	ui->cmbxGrade->addItems({"1", "2", "3", "4", "5", "6", "7", "8"});

	// List available serial ports
	for (QSerialPortInfo& serialPortInfo : QSerialPortInfo::availablePorts())
	{
		ui->cmbxSerialPorts->addItem(serialPortInfo.portName());
	}

	// Set initial graphs
	setGraphs(3);

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
	/*
	if (capturer == NULL)
	{
		ui->customPlot->xData->setRange(0, 300);

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
	*/
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

void MainWindow::on_btnExport_clicked()
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
		ui->customPlot->graph(i)->setPen(QPen(colors[i]));

		// Set style
		ui->customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
	}
}

void MainWindow::plotGraph(vector<vector<double> >& vectors)
{
	for (unsigned int i = 0; i < vectors.size(); i++)
	{
		QVector<double> v;
		v.append(v);

		ui->customPlot->graph(i)->setData(xData, v);
	}

	ui->customPlot->replot();
}
