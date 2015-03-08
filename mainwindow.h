#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QSerialPort>
#include <qwt_plot_curve.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort serialPort;

    unsigned int numOfSamples;
    QwtPlotCurve curve;
    QVector<double> dataArray;
    QVector<double> dataX;
    void addData(double data);

private slots:
    void loadPortList();
    void loadBaudRateList();
    void togglePort();
    void selectPort(QString portName);
    void onPortToggled(bool open);
    void selectBaudRate(QString baudRate);

    void onDataReady();
    void onPortError(QSerialPort::SerialPortError error);

    void onNumOfSamplesChanged(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();

signals:
    void portToggled(bool open);
};

#endif // MAINWINDOW_H
