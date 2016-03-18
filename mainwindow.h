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

	void on_btnDetener_clicked();

	void plotGraph();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
