#pragma once
#include "Common.h"
#include "qwidget.h"
#include "qwebengineview.h"
#include "qwebchannel.h"
#include "TaskWndDialog.h"
#include "QucikInstallListWidget.h"
#include "ui_QuickInstallPage.h"
enum ThreadType
{
    QUICK_REC = 1,
    CUSTOM_REC = 2,
};
class InitQuickInstallThread :public QThread
{
    Q_OBJECT
public:
    InitQuickInstallThread(){ _flag = 0; }
    virtual ~InitQuickInstallThread(){}
    virtual void run();
    void setFlag(int flag)
    {
        _flag = flag;
    }
signals:
    void ThreadFinishSignal(bool bRestart,int flag);
private:
    int _flag;
};

class CQuickInstallView : public QWidget
{
    Q_OBJECT

public:
    CQuickInstallView(CTaskWndDialog *taskWnd,QWidget *parent = Q_NULLPTR);
    virtual ~CQuickInstallView();
    QWidget *getListContainer(){
        return _pQuickListWidget->getContainer();
    }
private:
    CTaskWndDialog *_pTaskCenterWnd;
    CQuickListWidget *_pQuickListWidget;
    InitQuickInstallThread *_pthread;
    QMap<QString, QWidget*> _map;
    QMap<QWidget *, TKuInstallItem> _mapInfo;
    QButtonGroup *_installGroup;
    QButtonGroup *_installBtnGroup;
    QString _itemStyleSheet;
    QTimer _timer;
    int _initThreadFlag;
    Ui::QuickInstallPageUIClass ui;
public slots :
    void quickSingleInstallSlot(TKuInstallItem item);
    void sendInstalledApkListSlot(QString serial, QString);
    void changeSoftStatusSlot(QString packageName, SOFT_STATE nState);
    void restoreOriginalStateSlot();
    void createQuickItemSlot(TPackageVector vt);
    void setIconSlot(QString icon, QString unique);
    void installQuickSlot(QAbstractButton *item);
    void installBtnQuickSlot(QAbstractButton *item);
    void ThreadFinishSlot(bool bRestart, int flag);
    void timerOutSlot();
    void NoNetSlot(bool bVisible);
private:
    void changeSoftStatus(QString packageName,int nState);
    void startThread();
};
