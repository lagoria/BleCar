#ifndef BleManager_H
#define BleManager_H


#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyService>
#include <QLowEnergyController>


class BleManager : public QObject
{
    Q_OBJECT

public:

    typedef enum {
        BLUETOOTH_EMPTY = 0,
        BLUETOOTH_SCAN_FINISH,
        BLUETOOTH_CONNECT,
        BLUETOOTH_DISCONNECT,
        BLE_SERVICE_DISCOVER,
        BLE_CHARACTER_FIND,
    } BluetoothStatus;

    BleManager();
    ~BleManager();

    bool BluetoothDeviceDetect();
    void BluetoothTurnOn();
    void BluetoothScan();
    void BluetoothConnect(QBluetoothAddress address);
    void BleGattDataSend(QByteArray &value);



signals:
    void BleStatusChanged(BleManager::BluetoothStatus status);
    void BleDeviceScanInfoReport(const QBluetoothDeviceInfo &info);
    void BleGattReceiveReport(const QByteArray &value);

private slots:
    void BluetoothScanFinished();
    void BluetoothDeviceScanList(const QBluetoothDeviceInfo &info);
    void GattServiceDiscover(const QBluetoothUuid & serviceUuid);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void characteristicValueChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);


private:
    BluetoothStatus status = BLUETOOTH_EMPTY;
    QBluetoothLocalDevice *localDevice = nullptr;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = nullptr;
    QLowEnergyController *controller = nullptr;
    QLowEnergyService *service = nullptr;
    QLowEnergyCharacteristic m_Characteristic;

    QBluetoothUuid targetServiceUuid = QBluetoothUuid(QString("8653000a-43e6-47b7-9cb0-5fc21d4ae340"));
    QBluetoothUuid targetCharacterUuid = QBluetoothUuid(QString("8653000b-43e6-47b7-9cb0-5fc21d4ae340"));
};

#endif // BleManager_H
