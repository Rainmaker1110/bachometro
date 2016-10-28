#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>

#include <QFuture>

#include <QMainWindow>

#include "arduinohandler.h"
#include "coordinatesregister.h"
#include "sensordatamanager.h"
#include "sensordataprocessor.h"

using namespace std;

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void on_btnCapturar_clicked();

		void on_btnFile_clicked();

		void on_btnPlot_clicked();

		void on_btnLngLat_clicked();

		void on_btnSave_clicked();

		void on_chbxFilter_clicked();

		void on_cmbxWindow_currentIndexChanged(const QString &arg1);

		void on_cmbxOrder_currentIndexChanged(const QString &arg1);

		void on_cmbxSensorsNum_currentIndexChanged(const QString &arg1);

		void on_btnHost_clicked();

		void plotGraphs();

	private:
		Ui::MainWindow *ui;

		static const int DEFAULT_SENSOR_NUMBER;
		static const int SERIAL_TIMEOUT;

		static const int COLORS[];

		static const string DEFAULT_HOST;

		bool reading;

		SensorData sensor;
		GPSData gps;

		CoordinatesRegister coordsReg;

		SensorDataManager dataManager;

		SensorDataProcessor dataProcessor;

		QVector<double> xData;

		QFuture<void> readThread;

		QSerialPort * serialPort;

		QTimer * plotTimer;

		void readData();

		void setGraphs(int sensorsNum);
};

#endif // MAINWINDOW_H
