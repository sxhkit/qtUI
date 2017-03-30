#pragma once
#include "Common.h"
#include "ui_TaskCenterWnd.h"
#include "qdialog.h"
#include "BatchListItem.h"
struct  TaskCenterItemInfo
{
    int progressValue;
    QString err;
    float fsize;
    SOFT_STATE nstate;
    QString name;
    QString icon;
    QString packageName;
    QListWidgetItem *pItem;
    TaskCenterItemInfo()
    {
        progressValue = 0;
        fsize = 0;
        nstate = WAIT_DOWNLOAD;
        pItem = NULL;
    }

    bool operator == (TaskCenterItemInfo& ele)
    {

        return (
            progressValue == ele.progressValue&&
            fsize == ele.fsize&&
            nstate == ele.nstate&&
            name == ele.name&&
            packageName == ele.packageName &&
            pItem == ele.pItem &&
            icon == ele.icon&&
            err == ele.err 
            );
    }

    TaskCenterItemInfo& operator=(TaskCenterItemInfo ele)
    {
        progressValue = ele.progressValue;
        fsize = ele.fsize;
        nstate = ele.nstate;
        name = ele.name;
        packageName = ele.packageName;
        pItem = ele.pItem;
        icon = ele.icon;
        err = ele.err;
        return *this;
    }

};

class CTaskWndDialog : public QDialog
{
    Q_OBJECT
public:
    CTaskWndDialog(QWidget *parent = Q_NULLPTR);
    virtual ~CTaskWndDialog();
    TaskCenterItemInfo *getTaskItemInfo(QString package);
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    bool winEvent(MSG *message, long *result);
private:
    Ui::TaskCenterWndClass ui;
private slots:
    void hideWndSlot();
    void setProgressValueSlot(QString package, int val);
    void setItemStateSlot(QString package, SOFT_STATE nstate);
    void addListItemSlot(QString package,QString icon, QString name, float fsize);
    void setErrorInfoSlot(QString package, QString err);
    void setIconSlot(QString icon, QString unique);
    void setItemProgressSlot(QString packagename, qint64 readbyte, qint64 totalbyte);
    void startDownloadSlot(TKuInstallItem item);
    void clearListSlot();
    void retryButtonClick(QAbstractButton *pRetry);
    
private:
    QList<TaskCenterItemInfo> _listItem;
    QButtonGroup* _retryButtonGroup;
    QPixmap _noItemPix;
    QPixmap _hasItemPix;
private:
    void addListItem(QString &package, QString &icon, QString &name, float &fsize);
    int getCorrectLineNum(QListWidgetItem *listItem);
    void connectSignal2Slot();
    CTaskListItem* movePosition(QString &packagename);
    int getInstalledSuccess();
    
private:
    void setListItemState(QString packagename, SOFT_STATE nstate);
    void setListItemErrorCode(QString packagename, QString err);
    void setListItemProgress(QString packagename, int value);
    //list 数据相关的操作
    
    CTaskListItem *getItemWidget(QString packageName);
    SOFT_STATE getItemStateByListItem(QListWidgetItem *pItem);
    TaskCenterItemInfo *getItemInfoByWidget(QWidget *pTaskListItem);
    
};
