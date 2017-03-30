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
    *  ���ݽ������� kill ������صĽ���
    */
    void killAdb(QString processName);
    /*
    *  kill �ϰ汾�� ���ֿͻ��˺�ADB����
    */
    void killOldVersionAsstProcesses();
public slots:
    //����adb����״̬�����ı���ź�
    void handleState(QProcess::ProcessState newState);
    void processData(QByteArray& datagram);
signals:
    void stateChanged(const QString& serial, DeviceState nstate, struct device_info info);

protected:
    void timerEvent(QTimerEvent* event);

private:
    /*
    *  ����adb���ص��豸�ַ������������豸�ţ��豸״̬����Ʒ�ͺţ��ֻ����Ƶ���Ϣ
    *  @str [in] ��adb device-l ����ص�����
    */
    void parserConnectInfo(QString& str);
    /*
    *  ���ݹ�ȥ�洢���豸��״̬�����ڵõ����豸��Ϣ�����豸״̬�Ƿ����˱仯�ļ�⣬�������֪ͨui�̸߳���״̬
    *  @deviceInfo [in] : �ջ�õ�һ���豸��Ϣ
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