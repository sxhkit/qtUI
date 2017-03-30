#pragma once
#include "qobjectdefs.h"
#include "qwidget.h"
#include "configuration.h"
#include "Common.h"
#include "check_devices/CheckDevices.h"

typedef QVector<QWidget *> TListItemVector;

class CSignalManager:public QObject
{
    Q_OBJECT
    CSignalManager() {}
public:
    ~CSignalManager(){}

    static CSignalManager *getInstance()
    {
        static CSignalManager signalManager;
        return &signalManager;
    }
signals:
    void addBatchItemSingal(QWidget *pItem);
    void addOnekeyItemSingal(QWidget *pItem);
    void addQuickItemSingal(QWidget *pItem);
    void addQuickItemsSingal(TListItemVector vt);
    void createQuickItemSignal(TPackageVector vt);
    void createCustomItemSignal(TPackageVector vt);
    //删除套餐列表里面套餐的信号
    void deleteOnekeyItemSignal(QWidget *item);
    void deleteBatchItemSignal(QString unique);

    void setBatchItemUniqueSignal(QWidget *pWidget,QString unique);
    void setOnekeyItemUniqueSignal(QWidget *pWidget, QString unique);

    void setBatchPackageInfoByUniqueSignal(QString, QString);
    void setOnekeyPackageInfoByUniqueSignal(QString, QString);


    void showOneKeyQueryInfoSingal(QWidget *pItem,QString unique);
    void hideOneKeyQueryInfoSingal();

    // 一键装机 安装信号
    void InstallItemSignal(QString unique);

    //设置 套餐信息的 信号
    void setBatchPackageInfoSignal(QWidget *pWidget, QString packagename, float size, int num);
    void setOnekeyPackageInfoSignal(QWidget *pWidget, QString packagename, float size, int num);

    //批量装机 安装信号
    void startInstallSingal(TPackageVector vt,TMobileVector vtMpbile);

    //创建手机安装下载相关事例信号
    void mobileConnectedSignal(QString serial);
    void mobileDisconnectSignal(QString serial);

    //任务中心添加item
    void addTaskItemSignal(QString package,QString icon, QString name, float fsize);
    void addTaskItemFromQuickSignal(QString package, QString icon, QString name, float fsize);
    
    void setItemStateSignal(QString package,SOFT_STATE nstate);
    void setErrorInfoSignal(QString package,QString err);
    void setProgressValueSignal(QString package,int val);

    //添加套餐
    void addPackageSignal(TPackageVector vt, QString packagename, QString unique_key, ListItemType type);
    // 下在icon 信号
    void addDownloadIconSignal(QString url, QString unique);
    //某个下载项完成
    void completedDownloadOne(TKuInstallItem item, bool res, QString serial);
    //icon的下载项完成
    void completedDownloadIconSignal(QString savepath, QString identify);
    //往安卓手机推送文件完成
    void completedPushfileOne(TKuInstallItem item, bool res, QString serial, QString info);
    //完成某个文件的安装
    void completedInstallOne(TKuInstallItem item, bool res, QString serial, QString info);
    //对一个文件的下载进度的汇报
    void downloadProgress(QString identify, qint64 bytesReceived, qint64 bytesTotal);

    //批量装机 安装进度
    void batchInstallProgressSignal(QString serial, int success, int total,int finished);
    //开始下载的信号
    void startDownloadSignal(TKuInstallItem item);
    //编辑自定义套餐
    void modifyPackageSignal(QString unique);
    //快捷安装单款安装
    void quickSingleInstallSignal(TKuInstallItem item);
    //某个套餐安装完成后发送的信号，用于数据上报
    void completePackageInstallSignal(QString serial, QString packageKey,QString updateUniqueKey);
    void sendCompletionStatusToAndroidSignal(QString serial, int installed, int total);
    void sendInstalledStatusToAndroidSignal(QString serial, int installed, int total);
    
    //快捷安装设置状态信号
    void quickInstallChangeSoftStatusSignal(QString, SOFT_STATE);

    //清理任务中心item
    void clearListSignal();
    //清理套餐管理里面连接手机的套餐信息
    void clearPackageSignal(QString serial,bool bonekey);

    //一键装机套餐安装完成
    void finishOnekeyPackageAbnormalSignal();    //拔出异常情况
    void finishedOnekeyPackageInstallSignal(int num, int success);//正常安装完成情况
    //批量装机的安装完成
    void finishedBatchPackageInstallSignal(QString serial);
    void finishedAllBatchPackageInstallSignal();
    //开始安装某款软件 不同于开始安装startInstallSingal 安装套餐
    void startInstallOneSoftSignal(TKuInstallItem item);

    //一键装机 连接的手机发生变法时候要发送 获取新的手机安装APK的信号
    void queryApkListSignal(QString serial);

    // 无手机连接时候还原快捷安装页面 所有 软件状态为安装状态
    void restoreOriginalStateSignal();

    //tcp 连接返回的信号 传递安装的应用列表
    void AckApkListSignal(QString serial,QString data);

    // 用户电话获取
    void setUserInfoSignal(QString);

    void startTailWorkSignal(QString serial,TaskType type);
    void completeTailWorkSignal(QString serial);

    //设置一键装机中删除按钮不可用    
    void sendDisableDeletePackageSignal(QString unique,bool isEnable);
    //设置客户端版本信息
    void setVersionInfoSignal(QString ver, bool bMustUpdate);

    // 设置批量装机 按钮是否可用
    void setBatchInstallBtnEnableSignal(bool bEnable);
    //任务中心，任务统计
    void sendTaskCenterProgressSignal(int success, int num);

    // gif 停止或者开始
    void startTaskCenterMovieSignal(bool bStart);

    void setSingleConnectStateSignal(QString serial ,QString model, DeviceState state);

    //获取sd卡信息的信号
    void hasGotstorageInfoSignal(QString serial, TQMap info);
    void queryStorageInfoSignal(QString serial);

    void setMobileModelSignal(QString serial, QString model);

    void customNoNetSignal(bool bVisible);
    void quickNoNetSignal(bool bVisible);
    //修改查询页面的信号
    void changeItemSignal(QString unique);

    //刷新查询页面
    void refreshQueryInfoViewSignal(QString curUnique, QString changeUnique);
};
