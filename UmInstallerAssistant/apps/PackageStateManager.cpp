#include "PackageStateManager.h"
#include "configuration.h"
#include "SignalManager.h"
#include "ConnectManager.h"
#include "UMessageBox.h"

PackageStateMan::PackageStateMan()
{
    _map.clear();
    connect(CSignalManager::getInstance(), SIGNAL(completedDownloadOne(TKuInstallItem, bool, QString)),
        this, SLOT(completedDownloadOneSlot(TKuInstallItem, bool, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(completedPushfileOne(TKuInstallItem, bool, QString,QString)),
        this, SLOT(completedPushfileOneSlot(TKuInstallItem, bool, QString,QString)));
    connect(CSignalManager::getInstance(), SIGNAL(completedInstallOne(TKuInstallItem ,bool,QString,QString)), 
        this, SLOT(completedInstallOneSlot(TKuInstallItem, bool, QString, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(clearPackageSignal(QString, bool)),
        this, SLOT(disconnectMobileSlot(QString, bool)));
    connect(CSignalManager::getInstance(), SIGNAL(startInstallOneSoftSignal(TKuInstallItem)), 
        this, SLOT(startInstallOneSoftSlot(TKuInstallItem)));
    
}
PackageStateMan::~PackageStateMan()
{

}

bool PackageStateMan::startInstallTask(TPackageVector packageVt, TMobileVector mobileVt, QString packageKey, TaskType type)
{
    
    
    QSet<QString> set;
    QString unique;
    for (auto it = packageVt.begin(); it != packageVt.end();it++)
    {
        QString packageName = it->_packageName;
        unique = it->_packageUpdateUnique;
        set.insert(packageName);
        
    }

    for (auto it_m = mobileVt.begin(); it_m != mobileVt.end();it_m ++)
    {
        PackInfo info;
        info.num = packageVt.size();
        info.success = 0;
        info.packageKey = packageKey;
        info.type = type;
        info.packageUpdateUnique = unique;
        info.set = set;
        _map.insert((*it_m), info);
    }
    if (type != BATCH_TASK)
    {
        emit CSignalManager::getInstance()->startTaskCenterMovieSignal(true);
    }
    return true;
}

void PackageStateMan::completedInstallOneSlot(TKuInstallItem item, bool res, QString serial,QString errInfo)
{
    dealwithPackage(item, res, serial, INSTALL_TYPE, errInfo);
}

bool PackageStateMan::getProgressInfo(QString &serial, int &num, int &success, int &finished, TaskType type)
{
    bool res = false;
    QList <PackInfo> list = _map.values(serial);
    auto it = list.begin();
    for (; it != list.end();it++)
    {
        if (type == (*it).type)
        {
            num = it->num;
            success = it->success;
            finished = it->num - it->set.size();
            res = true;
        }
    }
    return res;
}

void PackageStateMan::completedDownloadOneSlot(TKuInstallItem item, bool res, QString serial)
{
    dealwithPackage(item, res, serial, DOWNLOAD_TYPE);
}

void PackageStateMan::completedPushfileOneSlot(TKuInstallItem item, bool res, QString serial, QString error_info)
{
    dealwithPackage(item, res, serial, PUSHFILE_TYPE,error_info);
}

void PackageStateMan::deleteMapItem(QString serial, QString package, TaskType type, bool res, Signal_Type single_type)
{
    if (type == BATCH_TASK || type == SINGLE_TASK||type == NECESSARY_TASK)
    {
        QMultiMap<QString, PackInfo>::iterator it = _map.find(serial);
        while (it != _map.end() && it.key() == serial) {
            if ((*it).type == type) //根据类型 判断找到 到底是一键装机还是批量装机
            {
                auto it_s = (*it).set.find(package);
                if (it_s != (*it).set.end())
                {
                    if (!res)
                    {
                        (*it).set.erase(it_s);
                    }

                    if (res && (single_type == INSTALL_TYPE))
                    {
                        (*it).set.erase(it_s);
                        if (!(*it).bStartFinish)
                        {
                            (*it).success++;
                        }
                    }

                }

                if ((*it).set.empty())
                {
                    bool bBatchTask = false;
                    if ((*it).type == BATCH_TASK) {
                        bBatchTask = true;
                    }

                    if (isFinishAllTask(serial, &(*it))) {
                        _map.erase(it);
                    }

                    if (bBatchTask && !isExistBatchTask()) {
                        emit CSignalManager::getInstance()->finishedAllBatchPackageInstallSignal();
                    }
                }
                break;
            }
            it++;
        }

    }
}

bool PackageStateMan::addNessaryTask(QString serial, PackInfo *info)
{
    bool bfinished = true;
    TPackageVector vt_Necessary;
    if (vt_Necessary.size() > 0)
    {
        auto it_n = vt_Necessary.begin();
        for (it_n; it_n != vt_Necessary.end(); it_n++)
        {
            (*it_n)._type = info->type;
            info->set.insert((*it_n)._packageName);
        }
        info->bStartFinish = true;
        TMobileVector vt_m;
        vt_m.push_back(serial);
        emit CSignalManager::getInstance()->startInstallSingal(vt_Necessary, vt_m);
        bfinished = false;
    }
    return bfinished;
}

bool PackageStateMan::isFinishAllTask(QString serial,PackInfo *info)
{
    bool bFinish = true;
   
    return bFinish;
}

void PackageStateMan::disconnectMobileSlot(QString serial, bool bOnekey)
{

}

void PackageStateMan::dealwithPackage(TKuInstallItem item, bool res, QString serial, Signal_Type ntype, QString error)
{
    SOFT_STATE s = convertState(ntype, res);
    if (item._type == SINGLE_TASK || item._type == QUICK_TASK || item._type == RETRY_TASK) {  //设置快捷安装 和 一键安装 状态
        if (ConnectManager::getInstance()->getOnekeySerial(false) == item._serial) {
            setSingleTaskState(item._packageName, s, item._type, error);
        }
        
    } else if(item._type == BATCH_TASK){
        setBatchTaskState(serial,item._packageName,s);
    } 
    if (item._type == SINGLE_TASK || item._type == BATCH_TASK || NECESSARY_TASK == item._type)
    {
        deleteMapItem(serial, item._packageName, item._type, res, ntype);
    }
}

SOFT_STATE PackageStateMan::convertState(Signal_Type type, bool res)
{
    SOFT_STATE s;
    if (type == DOWNLOAD_TYPE)
    {
        s = res ? DOWNLOADSUCCESS : DOWNLOADFAILED;
    }
    else if (type == INSTALL_TYPE)
    {
        s = res ? INSTALLSUCCESS : INSTALLFAILED;
    }
    else if (type == PUSHFILE_TYPE)
    {
        s = res ? DOWNLOADSUCCESS : INSTALLFAILED;
    }
    return s;
}

void PackageStateMan::startInstallOneSoftSlot(TKuInstallItem item)
{
    
    
}

void PackageStateMan::finishedInstallDesktop(QString serial,int num,int success,TaskType type)
{
   
}

bool PackageStateMan::isExistSingleTask()
{
    bool res = false;
    auto it = _map.begin();
    for (it; it != _map.end();it++)
    {
        if (it->type == SINGLE_TASK)
        {
            res = true;
            break;
        }
    }
    return res;
}

void PackageStateMan::setSingleTaskState(QString packageName, SOFT_STATE state, TaskType type,QString errinfo)
{
    emit CSignalManager::getInstance()->setItemStateSignal(packageName, state);

    if (state == INSTALLFAILED) {
        emit CSignalManager::getInstance()->setErrorInfoSignal(packageName, errinfo);
    }
    else if (state == DOWNLOADFAILED)
    {
        if (!Common::isNetConnected())
        {
            emit CSignalManager::getInstance()->setErrorInfoSignal(packageName, "网络异常,下载失败");
        }
        
    }
    SOFT_STATE qs = state;
    if (type == SINGLE_TASK)
    {
        //如果是一件装机的时候 安装失败了，将快捷安装中的状态设置为 安装 而不是重试状态
        if (state == DOWNLOADFAILED || state == INSTALLFAILED)
        {
            qs = RESTOREINSTALL;
        }
    }
    emit CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(packageName, qs);
    if (type == QUICK_TASK || type == RETRY_TASK)
    {
        if (state == INSTALLSUCCESS || state == INSTALLFAILED || state == DOWNLOADFAILED)
        {
            auto it = _set.find(packageName);
            if (it != _set.end())
                _set.erase(it);

            if (!isExistSingleTask())
            {
                if (_set.isEmpty())
                {
                    emit CSignalManager::getInstance()->startTaskCenterMovieSignal(false);
                }
            }
        }
        
    }
    
}

bool PackageStateMan::addSingleTask(QString packageName)
{
    if (packageName.isEmpty()) return false;
    _set.insert(packageName);
    return true;
}

void PackageStateMan::setBatchTaskState(QString serial,QString packageName, SOFT_STATE s)
{
    int num = 0;
    int success = 0;
    int finished = 0;
    if (s == INSTALLFAILED || s == INSTALLSUCCESS || s == DOWNLOADFAILED)
    {
        if (getProgressInfo(serial, num, success, finished,BATCH_TASK))
        {
            if (s != INSTALLSUCCESS)
                emit CSignalManager::getInstance()->batchInstallProgressSignal(serial, num, success, finished);
        }
    }
    if (serial == ConnectManager::getInstance()->getOnekeySerial())
    {
        //如果是批量装机的时候 安装失败了，将快捷安装中的状态设置为 安装 而不是重试状态
        SOFT_STATE qs = s;
        if (s == DOWNLOADFAILED || s == INSTALLFAILED)
        {
            qs = RESTOREINSTALL;
        }
        emit CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(packageName, qs);
    }
}

bool PackageStateMan::isExistBatchTask()
{
    bool res = false;
    auto it = _map.begin();
    for (it; it != _map.end(); it++)
    {
        if (it->type == BATCH_TASK)
        {
            res = true;
            break;
        }
    }
    return res;
}