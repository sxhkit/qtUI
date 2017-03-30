#pragma once
#include "Common.h"
#include "ui_MoblieItem.h"
#include "qwidget.h"
#include "CheckDevices.h"

class CMoblieItem : public QWidget
{
    Q_OBJECT

public:
    virtual ~CMoblieItem();
    CMoblieItem(QWidget *parent = Q_NULLPTR);
    void setItemState(DeviceState nstate, const QString &model);
    bool isInstallState();
    void setInstallFinishInfo();
private:
    Ui::MoblieItemClass ui;
signals:
    void setItemInstallProgressSignal(int num, int success,int finished);
    void setItemMobileModelSignal(QString model);
private slots:
    void setItemInstallProgressSlot(int num, int success, int finished);
    void setItemMobileModelSlot(QString model);
private:
    void initConnectItem();
    void setUIConnectedInfo( const QString &model);
    void setUIUnconnectedInfo( const QString &model);
    void setUIConnnectFailedInfo( const QString &model,bool bOffline = false);
    void setUIConnnectingInfo( const QString &model);
};
