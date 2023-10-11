#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BleManager.h"
#include <QBitArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum commandBit {
        COMMAND_ADVANCE = 0,
        COMMAND_RETREAT,
        COMMAND_TURN_LEFT,
        COMMAND_TURN_RIGHT,
        COMMAND_RESEERVED = 7,
    };


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_OnButton_clicked();
    void on_ScanButton_clicked();
    void on_ConnectButton_clicked();

    void on_UpButton_pressed();
    void on_UpButton_released();

    void on_DownButton_pressed();
    void on_DownButton_released();

    void on_LeftButton_pressed();
    void on_LeftButton_released();

    void on_RightButton_pressed();
    void on_RightButton_released();

    void on_ModeButton_pressed();
    void on_ModeButton_released();

    void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
    void LogInfoDisplay(QString Tag, QString StrData);
    void BleManagerStatusChanged(BleManager::BluetoothStatus status);
    void BleGattDataReceive(const QByteArray &value);

private:
    Ui::MainWindow *ui;
    uint8_t command = 0;
    BleManager *ble;

};
#endif // MAINWINDOW_H
