#include "DataManager.h"
#include "qmutex.h"
CDataManager::CDataManager()
{
}

CDataManager::~CDataManager()
{
}

void CDataManager::insert(__int64 key, QVector<TKuInstallItem> &vt)
{
    QMutexLocker l(&_mute);
    _mapPackageInfo.insert(key, vt);
}
QString CDataManager::getHex(QString s, bool bSingle)
{
    if (bSingle)
    {
        s = s.append("SINGLE_TASK");
    }
    else
    {
        s = s.append("BATCH_TASK");
    }
    QCryptographicHash md(QCryptographicHash::Md5);
    md.addData(s.toLocal8Bit());
    QByteArray res = md.result();
    return QString(res.toHex().data());
}

QVector<TKuInstallItem> CDataManager::getPackageVector(__int64 key,bool bSingle)
{
   QVector<TKuInstallItem> empty;
   auto it =  _mapPackageInfo.find(key);
   if (it != _mapPackageInfo.end())
   {
       
       auto it_l = it->begin();
       for (it_l; it_l != it->end();it_l++)
       {
           TKuInstallItem item = *it_l;
           item._type = bSingle ? SINGLE_TASK : BATCH_TASK;
           item._needDownloadProgress = bSingle ? true : false;
           item._packageUpdateUnique = getHex(QString::number(key), bSingle);
           empty.push_back(item);
       }
       return empty;
   }

   return empty;
}

float CDataManager::getPackageSize(__int64 key)
{
    QVector<TKuInstallItem> vt = getPackageVector(key);
    __int64 sum = 0;
    for (auto it = vt.begin(); it != vt.end();it++)
    {
        sum += (*it)._fileSize;
    }
    return (float)sum / (1024 * 1024);
}

int CDataManager::getPackItemNum(__int64 key)
{
    return getPackageVector(key).size();
}

TKuInstallItem CDataManager::getItemInfoByPackageName(QString packageName)
{
    TKuInstallItem item;
    auto it = _mapPackageInfo.begin();
    for (it; it != _mapPackageInfo.end();it++)
    {
        for (auto it_v = (*it).begin(); it_v != (*it).end(); it_v++)
        {
            if ((*it_v)._packageName == packageName)
            {
                item = *it_v;
                break;
            }
        }
    }
    return item;
}

void CDataManager::insertQuickItem(TKuInstallItem item)
{
    auto it = _mapPackageInfo.find(QUICK_DATA_MANAGER_KEY);
    if (it ==_mapPackageInfo.end())
    {
        QVector <TKuInstallItem> vt;
        vt.push_back(item);
        _mapPackageInfo.insert(QUICK_DATA_MANAGER_KEY, vt);
        return;
    }

    bool bNeedInsert = true;
    for (auto it_v = (*it).begin(); it_v != (*it).end(); it_v++)
    {
        if ((*it_v)._packageName == item._packageName)
        {
            bNeedInsert = false;
            break;
        }
    }
    if (bNeedInsert)
    {
        (*it).push_back(item);
    }
}

QString CDataManager::getRealName(QString &packageName)
{
    int suffix = 0;
    QString name = packageName;
    if (packageName == "请输入套餐名")
    {
        suffix = getMaxIndex();
        suffix++;
        setMaxIndex(suffix);
        name = QString("自定义套餐%0").arg(suffix);
    }
    return name;
}

void CDataManager::setMaxIndex(int num)
{
    if (num <= _maxIndex) return;
    _maxIndex = num;
}

int CDataManager::getMaxIndex()
{
    return _maxIndex;
}

bool CDataManager::parserPackage(QString json, QString &package_name, QVector<TKuInstallItem> &vt)
{
   
    return true;
}

void CDataManager::eraseName(QString unique)
{
    auto it = _nameMap.find(unique);
    if (it != _nameMap.end()) {
        _nameMap.erase(it);
    }
}

void CDataManager::insertName(QString unique, QString name)
{
    _nameMap.insert(unique, name);
}

QString CDataManager::queryName(QString unique)
{
    QString name;
    auto it = _nameMap.find(unique);
    if (it != _nameMap.end())
    {
        name = *it;
    }
    return name;
}