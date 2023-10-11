#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include <QListWidgetItem>

#include "BleManager.h"


#define TAG     "Info"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建蓝牙对象
    ble = new BleManager;

    //发现设备时会触发deviceDiscovered信号，转到槽显示蓝牙设备
    connect(ble, SIGNAL(BleDeviceScanInfoReport(QBluetoothDeviceInfo)), this, SLOT(addBlueToothDevicesToList(QBluetoothDeviceInfo)));

    connect(ble, SIGNAL(BleStatusChanged(BleManager::BluetoothStatus)), this, SLOT(BleManagerStatusChanged(BleManager::BluetoothStatus)));

    connect(ble, SIGNAL(BleGattReceiveReport(QByteArray)), this, SLOT(BleGattDataReceive(QByteArray)));

    if (ble->BluetoothDeviceDetect() == false) {
        LogInfoDisplay(TAG, "Bluetooth closed");
    } else {
        LogInfoDisplay(TAG, "Bluetooth opened");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::BleManagerStatusChanged(BleManager::BluetoothStatus status)
{
    switch (status) {
    case BleManager::BLUETOOTH_SCAN_FINISH:
        LogInfoDisplay(TAG, "Scan finished.");
        break;

    case BleManager::BLUETOOTH_CONNECT:
        LogInfoDisplay(TAG, "Bluetooth connected.");
        break;

    case BleManager::BLUETOOTH_DISCONNECT:
        LogInfoDisplay(TAG, "Bluetooth disconnected.");
        break;

    case BleManager::BLE_SERVICE_DISCOVER:
        LogInfoDisplay(TAG, "Gatt service discovering.");
        break;

    case BleManager::BLE_CHARACTER_FIND:
        LogInfoDisplay(TAG, "Gatt service characteristic finded.");
        break;

    default : break;
    }
}


// 在 listWidget_Ble 上显示查找到的蓝牙设备
void MainWindow::addBlueToothDevicesToList(const QBluetoothDeviceInfo &info)
{
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    QList<QListWidgetItem *> items = ui->listWidget_Ble->findItems(label, Qt::MatchExactly);

    if (items.empty())
    {
        QListWidgetItem *item = new QListWidgetItem(label);
        item->setForeground(QColor(Qt::black));
        ui->listWidget_Ble->addItem(item);
    }
}

void MainWindow::BleGattDataReceive(const QByteArray &value)
{
    QString buffer;
    if (value.size() > 64) {
        buffer = QString::fromUtf8(value.left(64));
    } else {
        buffer = QString::fromUtf8(value);
    }

    // 接收到数据
    LogInfoDisplay("Remote", buffer);
}

// OnButton 点击回调处理
void MainWindow::on_OnButton_clicked()
{
    if (ble->BluetoothDeviceDetect() == false) {
        ble->BluetoothTurnOn();
        LogInfoDisplay(TAG, "Bluetooth turn on.");
    } else {
        LogInfoDisplay(TAG, "Bluetooth already on.");
    }
}


// ScanButton 点击回调处理
void MainWindow::on_ScanButton_clicked()
{
    if (ble->BluetoothDeviceDetect() == false) {
        LogInfoDisplay(TAG, "Bluetooth not enabled");
    } else {
        ui->listWidget_Ble->clear();
        ble->BluetoothScan();
    }
}


// ConnectButton 点击回调处理
void MainWindow::on_ConnectButton_clicked()
{
    QListWidgetItem *item = ui->listWidget_Ble->currentItem();
    if (item == nullptr) return ;
    QString text = item->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;
    QBluetoothAddress address(text.left(index));
    QString name(text.mid(index + 1));
    LogInfoDisplay(TAG, "Connect to " + name);
    ble->BluetoothConnect(address);
}

void MainWindow::LogInfoDisplay(QString Tag, QString StrData)
{
    QString comStr;
    comStr.append(StrData);
    if(comStr.length() > 0 && Tag.length() > 0)
    {
        ui->LogtextBrowser->append("[" + Tag + "]: " + comStr);
    }
}

/* -----------------控制按钮----------------- */
// UpButton handle
void MainWindow::on_UpButton_pressed()
{
    // 发送 前进 指令
    command = (uint8_t ) (command | (1 << COMMAND_ADVANCE));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}

void MainWindow::on_UpButton_released()
{
    // 清除 前进 指令
    command = (uint8_t ) (command & ~(1 << COMMAND_ADVANCE));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}


// DownButton handle
void MainWindow::on_DownButton_pressed()
{
    // 发送 后退 指令
    command = (uint8_t ) (command | (1 << COMMAND_RETREAT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}

void MainWindow::on_DownButton_released()
{
    // 清除 后退 指令
    command = (uint8_t ) (command & ~(1 << COMMAND_RETREAT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}


// LeftButton handle
void MainWindow::on_LeftButton_pressed()
{
    // 发送 左转 指令
    command = (uint8_t ) (command | (1 << COMMAND_TURN_LEFT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}

void MainWindow::on_LeftButton_released()
{
    // 清除 左转 指令
    command = (uint8_t ) (command & ~(1 << COMMAND_TURN_LEFT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}


// RightButton handle
void MainWindow::on_RightButton_pressed()
{
    // 发送 右转 指令
    command = (uint8_t ) (command | (1 << COMMAND_TURN_RIGHT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}

void MainWindow::on_RightButton_released()
{
    // 清除 右转 指令
    command = (uint8_t ) (command & ~(1 << COMMAND_TURN_RIGHT));
    // 使用 QByteArray 的构造函数将 uint8_t 数据转化为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(&command), sizeof(command));
    ble->BleGattDataSend(byteArray);
}


// ModeButton handle
void MainWindow::on_ModeButton_pressed()
{
    // Reserved
}

void MainWindow::on_ModeButton_released()
{
    // Reserved
}

