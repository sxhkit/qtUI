#ifndef CHECK_DEVICES_H
#define CHECK_DEVICES_H

#include "qprocess.h"
#include "qdialog.h"
#include <QTime>
#include <qtimer.h>
#include <QTimerEvent>
#include "Ipc.h"
#include "configuration.h"

#define ADB_EXE    "UMadb.exe"

struct device_info{
    QString _state;
    QString _model;
    QString _product;
    QString _device;
};

class CheckDevices: public QObject{

    Q_OBJECT

public:
    /*
    CheckDevices* getInstance()
    {
        static CheckDevices checkDevice;
        return &checkDevice;
    }
    */
    CheckDevices();
    ~CheckDevices();
    void startAdb();
    void killAdb();
    /*
    *  根据进程名字 kill 所有相关的进程
    */
    void killAdb(QString processName);
    /*
    *  kill 老版本的 助手客户端和ADB进程
    */
    void killOldVersionAsstProcesses();
public slots:
    //处理adb进程状态发生改变的信号
    void handleState(QProcess::ProcessState newState);
    void processData(QByteArray& datagram);
signals:
    void stateChanged(const QString& serial, DeviceState nstate, struct device_info info);

protected:
    void timerEvent(QTimerEvent* event);

private:
    /*
    *  根据adb返回的设备字符串，解析出设备号，设备状态，产品型号，手机名称等信息
    *  @str [in] ：adb device-l 命令返回的内容
    */
    void parserConnectInfo(QString& str);
    /*
    *  根据过去存储的设备的状态和现在得到的设备信息进行设备状态是否发生了变化的检测，如果有则通知ui线程更新状态
    *  @deviceInfo [in] : 刚获得的一批设备信息
    */
    void CheckDevices::stateGo(QMap<QString, struct device_info>& deviceInfo);

    QProcess* _adb;
    UdpIpc _ipc;
    QByteArray _lastData;
    QMap<QString, QString> _deviceState; 
    QMap<QString, struct device_info> _deviceInfo;

    int _heartBeatTimerId;
    QTime _heartBeatTime;
};

#endif