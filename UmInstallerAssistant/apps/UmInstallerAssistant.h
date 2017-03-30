#pragma once
#include "Common.h"
#include <QtWidgets/QMainWindow>
#include "ui_Mainframe.h"
#include "qbytearray.h"
#include "qcoreapplication.h"
#include "BatchPageView.h"
#include "qbuttongroup.h"
#include "CheckDevices.h"
#include "ListWidget.h"
#include "OnekeyPageView.h"
#include "TaskWndDialog.h"
#include "QuickInstallView.h"
#include "configuration.h"
#include "UMessageBox.h"
#include "qtimer.h"
#include "qmovie.h"

class InitPackageThread :public QThread
{
    Q_OBJECT
public:
    InitPackageThread(QWidget *AddItem,QObject *parent = NULL);
    ~InitPackageThread();
    virtual void run();
signals:
    void initFinish();
private:
    QWidget *_addItem;
    void dealWithRecPackage();
};

struct MemInfo
{
    float fused;
    float ffree;
    float ftotal;
    MemInfo()
    {
        fused = 0.0;
        ffree = 0.0;
        ftotal = 0.0;
    }

    MemInfo& operator=(MemInfo ele)
    {
        fused = ele.fused;
        ffree = ele.ffree;
        ftotal = ele.ftotal;
        return *this;
    }

};
class UmInstallerAssistant : public QMainWindow
{
    Q_OBJECT
public:
    UmInstallerAssistant(QApplication *a,QWidget *parent = Q_NULLPTR);
    virtual ~UmInstallerAssistant();
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool winEvent(MSG *message, long *result);
    void initUIinfo(QApplication *a);
    void initPackageList();
    
private:
    float calcPackageSize(TPackageVector &vt);
    void connectSignal2Slot();
    void setWelcome(QTime time);
    float getSize(QString str);
    void setMemProgress(float used, float free, float total, bool isSd);
    void parseMemoryInfo(TQMap info);
    void clearMemoryInfo();
    
private:
    Ui::MainframeClass ui;
    CBatchPageView *_pBatchPageView;
    COnekeyPageView *_pOnekeyPageView;
    CQuickInstallView *_pQuickPageView;
    QButtonGroup * _tabButtonGroup;
    CTaskWndDialog *_taskCenterWnd;
    InitPackageThread *_pInitThread;
    COnekeyListAddItem *_pAddItem;
    CheckDevices *_checkDevices;
    QTimer _timer;
    QMovie _movie_c;
    QMovie _movie;
    QMap<int, MemInfo> _memMap;
private slots:
    void tabClickSlot(QAbstractButton *, bool);
    void openUrlLinkSlot();
    void openTaskWndSlot();
    void createPackageItemSlot(TPackageVector vt, QString packageName, QString unique_key, ListItemType type);
    void refreshSlot();
    void TimerTimeoutSlot();
    void setUserInfoSlot(QString);
    void sendTaskCenterProgressSlot(int success, int num);
    void startTaskCenterMovieSlot(bool bstart);
    void setSingleConnectStateSlot(QString serial ,QString model, DeviceState state);
    void setVersionInfoSlot(QString ver, bool bMustUpadate);
    void openUpdataLinkSlot();
    void setMemoryInfoSlot(QString serial, TQMap info);
    void currentIndexChangedSlot(QString);

};

