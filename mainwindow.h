#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>
#include <QTimer>

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

		void on_btnExport_clicked();

		void on_btnLngLat_clicked();

	private:
		Ui::MainWindow *ui;

		static int colors[];

		int sensorsNum;

		QVector<double> xData;

		QTimer * plotTimer;

		void setGraphs();
		void plotGraph();
};

#endif // MAINWINDOW_H
