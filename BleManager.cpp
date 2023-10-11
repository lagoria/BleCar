#include "BleManager.h"


/* 对象初始化 */
BleManager::BleManager()
{
    localDevice = new QBluetoothLocalDevice();
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000); // 设置扫描超时时间（毫秒）

    QObject::connect(discoveryAgent, SIGNAL(finished()), this, SLOT(BluetoothScanFinished()));  //扫描完成之后会触发此信号

    // 发现设备时会触发deviceDiscovered信号，转到槽显示蓝牙设备
    QObject::connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(BluetoothDeviceScanList(QBluetoothDeviceInfo)));
}

BleManager::~BleManager()
{
    delete localDevice;
    delete discoveryAgent;
    if (controller != NULL) {
        delete controller;
    }
}


bool BleManager::BluetoothDeviceDetect()
{
    if( localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        return false;
    } else {
        return true;
    }
}

void BleManager::BluetoothTurnOn()
{
    if (BluetoothDeviceDetect() == false) {
        localDevice->powerOn(); // 打开本地的蓝牙设备
    }
}

void BleManager::BluetoothScan()
{
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod); // 开始扫描BLE蓝牙设备
}

void BleManager::BluetoothScanFinished()
{
    // 发射信号
    status = BLUETOOTH_SCAN_FINISH;
    emit BleStatusChanged(status);
}

void BleManager::BluetoothDeviceScanList(const QBluetoothDeviceInfo &info)
{
    // 送入信号处理
    emit BleDeviceScanInfoReport(info);
}


void BleManager::BluetoothConnect(QBluetoothAddress address)
{
    if (controller != nullptr) {
        // 断开所有连接
        controller->disconnect();

        // 释放对象
        delete controller;
        controller = nullptr; // 将指针设置为 nullptr，以避免悬空指针
    }
    controller = new QLowEnergyController(address, this);
    controller->setRemoteAddressType(QLowEnergyController::PublicAddress); // 设置地址类型

    // 连接到控制器的信号和槽，以处理连接状态
    QObject::connect(controller, SIGNAL(serviceDiscovered(QBluetoothUuid)), this, SLOT(GattServiceDiscover(QBluetoothUuid)));

    // 连接成功
    QObject::connect(controller, &QLowEnergyController::connected, this, [this]() {
        discoveryAgent->stop();  //停止搜索设备
        // 搜索服务
        controller->discoverServices();
        // 发射信号
        status = BLUETOOTH_CONNECT;
        emit BleStatusChanged(status);
    });
    // 断开连接
    QObject::connect(controller, &QLowEnergyController::disconnected, this, [this]() {
        // 发射信号
        status = BLUETOOTH_DISCONNECT;
        emit BleStatusChanged(status);
    });

    // 连接目标设备
    controller->connectToDevice();
}


// 服务发现
void BleManager::GattServiceDiscover(const QBluetoothUuid & serviceUuid)
{
    if (service != nullptr) {
        // 断开所有连接
        service->disconnect();

        // 释放对象
        delete service;
        service = nullptr; // 将指针设置为 nullptr，以避免悬空指针
    }
    if (!controller) return;
    // 找到目标服务
    if (serviceUuid != targetServiceUuid) return;

    service = controller->createServiceObject(serviceUuid);
    if (!service) {
        qDebug() << "Service not found.";
        return;
    }

    QObject::connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                     this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    QObject::connect(service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
                     this, SLOT(characteristicValueChanged(QLowEnergyCharacteristic,QByteArray)));

    // 扫描特征值
    if(service->state() == QLowEnergyService::DiscoveryRequired)
        service->discoverDetails();

    // 发射信号
    status = BLE_SERVICE_DISCOVER;
    emit BleStatusChanged(status);
}

void BleManager::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::ServiceDiscovered) {
        if (service) {
            // 获取该服务的所有特征
            const QList<QLowEnergyCharacteristic> characteristics = service->characteristics();

            // 遍历特征并查找目标特征的UUID
            for (const QLowEnergyCharacteristic &characteristic : characteristics) {
               const QBluetoothUuid characteristicUuid = characteristic.uuid();
               if (characteristicUuid == targetCharacterUuid) {
                   // 获取特征的客户端特征配置描述符（Client Characteristic Configuration Descriptor）
                   QLowEnergyDescriptor notificationDescriptor = characteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

                   if (notificationDescriptor.isValid()) {
                       // 准备启用通知的数据（通常是0x01 0x00表示启用通知）
                       QByteArray enableNotificationValue = QByteArray(2, 0x01);

                       // 写入启用通知的数据到描述符
                       service->writeDescriptor(notificationDescriptor, enableNotificationValue);
                   }

                   m_Characteristic = characteristic;

                   // 发射信号
                   status = BLE_CHARACTER_FIND;
                   emit BleStatusChanged(status);
               }
            }

        }
    }
}

// 特征值改变，通知接收数据
void BleManager::characteristicValueChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    (void ) characteristic;
    // 发射信号
    emit BleGattReceiveReport(value);
}

void BleManager::BleGattDataSend(QByteArray &value)
{
    if (status != BLE_CHARACTER_FIND) return;
    service->writeCharacteristic(m_Characteristic, value);
}





