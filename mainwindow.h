#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void capture_data();

private slots:
	void on_btnCapturar_clicked();

	void plotGraph();

	void on_btnFile_clicked();

	void on_btnPlot_clicked();

	void on_btnPothole_clicked();

	private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
