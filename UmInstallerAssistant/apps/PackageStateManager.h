#pragma once
#include "Common.h"
#include "configuration.h"
#include "qobject.h"
#include "qmap.h"
#include "qstring.h"
#include "qset.h"
#include "qvector.h"
class PackageStateMan:public QObject
{
    Q_OBJECT
    PackageStateMan();
private:
    typedef struct PackInfo
    {
        int num;
        int success;
        bool bStartFinish;
        TaskType type;
        QString packageKey;
        QString packageUpdateUnique;
        QSet<QString> set;
        PackInfo()
        {
            num = 0;
            success = 0;
            bStartFinish = false;
        }
    };
    typedef enum Signal_Type
    {
        DOWNLOAD_TYPE = 0,
        PUSHFILE_TYPE,
        INSTALL_TYPE,
    };
public:
    static PackageStateMan * getInstance()
    {
        static PackageStateMan packageStateMan;
        return &packageStateMan;
    }
    ~PackageStateMan();
    bool startInstallTask(TPackageVector packageVt, TMobileVector mobileVt, QString packageKey,TaskType type = SINGLE_TASK);
    bool isExistSingleTask();
    bool isExistBatchTask();
    bool addSingleTask(QString packageName);
private slots:
    void completedInstallOneSlot(TKuInstallItem item, bool res, QString serial,QString error_info);
    void completedDownloadOneSlot(TKuInstallItem item, bool res, QString serial);
    void completedPushfileOneSlot(TKuInstallItem item, bool res, QString serial,QString error_info);
    void disconnectMobileSlot(QString serial,bool bOneKey);
    void startInstallOneSoftSlot(TKuInstallItem item);
    
private:
    QMultiMap<QString, PackInfo>_map;
    QSet<QString> _set;
private:
    void deleteMapItem(QString serial, QString package, TaskType type, bool res, Signal_Type single_type);
    bool getProgressInfo(QString &serial, int &num, int &success,int &finished,TaskType type);
    void dealwithPackage(TKuInstallItem item, bool res, QString serial,Signal_Type type, QString error = "");
    void finishedInstallDesktop(QString serial, int num, int success, TaskType type);
    SOFT_STATE convertState(Signal_Type type, bool res);

    void setSingleTaskState(QString packageName, SOFT_STATE state, TaskType type, QString errinfo = "");
    void setBatchTaskState(QString serial,QString packageName, SOFT_STATE s);
    bool isFinishAllTask(QString serial, PackInfo *info);
    bool addNessaryTask(QString serial, PackInfo *info);
};

