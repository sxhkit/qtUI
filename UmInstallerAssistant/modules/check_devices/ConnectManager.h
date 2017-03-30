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
    //连接信息的结构体
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
    // 修改连接状态的槽
    void mobileStatemChangeSlot(const QString& serial,
        DeviceState nstate, struct device_info info);
    void setMobileModelSlot(QString serial, QString model);
    
 private:
    //获取可用的串号的连接信息
    LinkInfo *getValidLinkInfo(const QString &serial);

    //判断是否已经存在该串号的连接
    bool isExistLink(const QString &serial);

    //获取一个新的连接节点， 如果超过可连接的数量， 那么返回为NULL
    LinkInfo *getNewLinkInfo();

    //更新连接信息 主要是状态信息
    LinkInfo* updateLinkInfo(const QString &serial,
        DeviceState nstate, const QString &model);

    //清除连接列表中的连接
    void earseLink(const QString &serial);

    //连接信号和槽
    void connectSignal2Slot();

 public:
    //判断是否存在连接成功的手机
    bool isExistConnectedMobile();
};
#endif  // UMINSTALLERASSISTANT_MODULES_CHECK_DEVICES_CONNECTMANAGER_H_
