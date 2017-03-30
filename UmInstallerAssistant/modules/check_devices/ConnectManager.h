//  Copyright (C)  WSWY
//
//  File:       ConnectManager.h
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#ifndef UMINSTALLERASSISTANT_MODULES_CHECK_DEVICES_CONNECTMANAGER_H_
#define  UMINSTALLERASSISTANT_MODULES_CHECK_DEVICES_CONNECTMANAGER_H_
#include "Common.h"
#include <qwidget.h>
#include <qstring.h>
#include <configuration.h>
#include "CheckDevices.h"
class ConnectManager:public QObject {
    Q_OBJECT
    //������Ϣ�Ľṹ��
    QList<LinkInfo> _listConnect;
    ConnectManager();

 public:
    static ConnectManager*getInstance() {
        static ConnectManager connectManager;
        return &connectManager;
    }

    ~ConnectManager();

    void insert(QWidget *pItem);
    bool getMobiles(TMobileVector *vt);
    QString getOnekeySerial(bool isNeedConnectSuccess = true);

    LinkInfo *getLinkInfoBySerial(QString &serial);

 private slots:
    // �޸�����״̬�Ĳ�
    void mobileStatemChangeSlot(const QString& serial,
        DeviceState nstate, struct device_info info);
    void setMobileModelSlot(QString serial, QString model);
    
 private:
    //��ȡ���õĴ��ŵ�������Ϣ
    LinkInfo *getValidLinkInfo(const QString &serial);

    //�ж��Ƿ��Ѿ����ڸô��ŵ�����
    bool isExistLink(const QString &serial);

    //��ȡһ���µ����ӽڵ㣬 ������������ӵ������� ��ô����ΪNULL
    LinkInfo *getNewLinkInfo();

    //����������Ϣ ��Ҫ��״̬��Ϣ
    LinkInfo* updateLinkInfo(const QString &serial,
        DeviceState nstate, const QString &model);

    //��������б��е�����
    void earseLink(const QString &serial);

    //�����źźͲ�
    void connectSignal2Slot();

 public:
    //�ж��Ƿ�������ӳɹ����ֻ�
    bool isExistConnectedMobile();
};
#endif  // UMINSTALLERASSISTANT_MODULES_CHECK_DEVICES_CONNECTMANAGER_H_
