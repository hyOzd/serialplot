#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QString>
#include <QVector>
#include <QSerialPort>
#include <QSignalMapper>
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
    enum NumberFormat
    {
        NumberFormat_uint8,
        NumberFormat_uint16,
        NumberFormat_uint32,
        NumberFormat_int8,
        NumberFormat_int16,
        NumberFormat_int32
    };

    Ui::MainWindow *ui;
    QButtonGroup numberFormatButtons;
    QButtonGroup parityButtons;
    QButtonGroup dataBitsButtons;

    QSerialPort serialPort;

    unsigned int numOfSamples;
    QwtPlotCurve curve;
    QVector<double> dataArray;
    QVector<double> dataX;
    void addData(QVector<double> data);

    NumberFormat numberFormat;
    unsigned int sampleSize; // number of bytes in the selected number format
    double (MainWindow::*readSample)();

    // note that serialPort should already have enough bytes present
    template<typename T> double readSampleAs();

private slots:
    void loadPortList();
    void loadBaudRateList();
    void togglePort();
    void selectPort(QString portName);
    void onPortToggled(bool open);
    void selectBaudRate(QString baudRate);
    void selectParity(int parity); // parity must be one of QSerialPort::Parity
    void selectDataBits(int dataBits); // bits must be one of QSerialPort::DataBits

    void onDataReady();
    void onPortError(QSerialPort::SerialPortError error);

    void onNumOfSamplesChanged(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();

    void onNumberFormatButtonToggled(int numberFormatId, bool checked);
    void selectNumberFormat(NumberFormat numberFormatId);

    void clearPlot();

signals:
    void portToggled(bool open);
};

#endif // MAINWINDOW_H
