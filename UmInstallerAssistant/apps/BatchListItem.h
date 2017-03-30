//  Copyright (C)  WSWY
//
//  File:       BatchListItem.h
//
//  History:    2017-Jan-1-22   Xhsund  Created
//
//--------------------------------------------------------------------------
#ifndef UMINSTALLERASSISTANT_APPS_BATCHLISTITEM_H_
#define UMINSTALLERASSISTANT_APPS_BATCHLISTITEM_H_
#include "Common.h"
#include "ui_MoblieItem.h"
#include <qwidget.h>
#include "CheckDevices.h"
#include <qradiobutton.h>
#include "ui_BatchListItem.h"
#include "ui_OnekeyListItem.h"
#include "ui_QueryItem.h"
#include "ui_TaksListItem.h"
#include <configuration.h>
#include "ui_OnekeyAdd.h"
#include "ui_QuickInstallItem.h"
#include "ui_CustomItem.h"

enum RecommendSoft {
    Recommended = 0,
    UnRecommended,
};
class CBatchListItem : public QRadioButton {
 public:
    explicit CBatchListItem(ListItemType nType = DEFAULT,
        QWidget *parent = Q_NULLPTR);
    void setItemState(DeviceState nstate, const QString &model);
 private:
    Ui::BatchListItemUI ui;
    void initItemInfo(ListItemType nType);
};

class COnekeyListItem :public QPushButton {
 public:
     explicit COnekeyListItem(ListItemType nType = DEFAULT,
         QWidget *parent = Q_NULLPTR);
    ~COnekeyListItem();

 private:
    Ui::OnekeyListItemUI ui;
 private:
    void initItemInfo(ListItemType nType);
};

class COnekeyListAddItem :public QPushButton {
 public:
    explicit COnekeyListAddItem(QWidget *parent = Q_NULLPTR);
    ~COnekeyListAddItem();

 private:
    Ui::OnekeyListAddUI ui;
};

class CQueryItem :public QWidget {
 public:
    explicit CQueryItem(RecommendSoft nType = Recommended,
        QWidget *parent = Q_NULLPTR);
    ~CQueryItem();
    void setItemInfo(QString name, float fsize);
    void setIcon(QString icon);

 private:
    Ui::QueryItemClass ui;
};

class CTaskListItem :public QWidget{
    Q_OBJECT

 public:
    explicit CTaskListItem(QWidget *parent = Q_NULLPTR);
    ~CTaskListItem();
    void initItem();

 signals:
    void setProgressValueSignal(int val);
    void setItemRelevantInfoSignal(QString name, float fsize);
    void setItemStateSignal(SOFT_STATE nstate);
    void setErrorInfoSignal(QString err);
    void setIconSignal(QString icon);

 private slots:
    void setProgressValueSlot(int val);
    void setItemRelevantInfoSlot(QString name, float fsize);
    void setItemStateSlot(SOFT_STATE nstate);
    void setErrorInfoSlot(QString err);
    void setIconSlot(QString icon);

 private:
    Ui::TaskListItemClass ui;

 private:
    QString GetStateString(int nState);
    QString GetStateIcon(int state);
    void connectSignal2Slot();
};

class CQuickListItem :public QPushButton {
    Q_OBJECT
public:
    explicit CQuickListItem(QWidget *parent = Q_NULLPTR);
    virtual ~CQuickListItem();

 private:
     Ui::QuickInstallItemClass ui;
 private:
    void initItem();
    void connectSignal2Slot();

signals:
    void setIconSignal(QString icon);
    void setStateSignal(SOFT_STATE state);
    void setItemInfoSignal(QString name, QString details, bool brec);
 public slots:
    void setIconSlot(QString icon);
    void setStateSlot(SOFT_STATE state);
    void setItemInfoSlot(QString name, QString detail,bool brec);
};

class CustomWndItem:public QCheckBox
{
    Q_OBJECT
public:
    CustomWndItem(QWidget *parent = Q_NULLPTR);
    ~CustomWndItem();
private:
    Ui::CustomItemClass ui;
signals:
    void setIconSignal(QString icon);
    void setItemInfoSignal(QString name, bool brec);
 public slots:
    void setIconSlot(QString icon);
    void setItemInfoSlot(QString name, bool brec);
    void setCheckedSlot(bool bckecked);

};
#endif  // UMINSTALLERASSISTANT_APPS_BATCHLISTITEM_H_
