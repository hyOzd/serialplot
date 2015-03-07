#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>

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

private slots:
    void loadPortList();
    void loadBaudRateList();
    void togglePort();
    void selectPort(QString portName);
    void onPortToggled(bool open);
    void selectBaudRate(QString baudRate);

    void onDataReady();

signals:
    void portToggled(bool open);
};

#endif // MAINWINDOW_H
