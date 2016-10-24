#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QMainWindow>

#include <QVector>
#include <QTimer>

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

		void on_cmbxSerialPorts_currentIndexChanged(const QString &arg1);

		void plotGraphs();

	private:
		Ui::MainWindow *ui;

		static const int COLORS[];
		static const int DEFAULT_SENSOR_NUMBER;
		static const string DEFAULT_HOST;

		vector<vector<double> > * data;

		ArduinoHandler arduino;

		CoordinatesRegister coordsReg;

		SensorDataManager dataManager;

		SensorDataProcessor dataProcessor;

		QVector<double> xData;

		QTimer * plotTimer;

		QSerialPort serialPort;

		void setGraphs(int sensorsNum);
};

#endif // MAINWINDOW_H
