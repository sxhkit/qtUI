#pragma once
#include "qmap.h"
#include "qvector.h"
#include <configuration.h>
#include "Common.h"
#include <qmutex.h>
class CDataManager
{
private:
    CDataManager();
public:
    static CDataManager *getInstance()
    {
        static CDataManager dataManager;
        return &dataManager;
    }
    ~CDataManager();

    void insert(__int64 key, QVector<TKuInstallItem> &vt);
    QVector<TKuInstallItem> getPackageVector(__int64 key, bool bSingle = true);
    float getPackageSize(__int64 key);
    int getPackItemNum(__int64 key);
    TKuInstallItem getItemInfoByPackageName(QString packageName);
    void insertQuickItem(TKuInstallItem item);
private:
    QMap<__int64, QVector<TKuInstallItem>> _mapPackageInfo;
    QBasicMutex _mute;
private:
    QString getHex(QString s, bool bSingle);

private:
    int _maxIndex;
    QMap<QString,QString> _nameMap;
public:
    void setMaxIndex(int num);
    int  getMaxIndex();
    QString getRealName(QString &packageName);
    void insertName(QString unique, QString name);
    QString queryName(QString unique);
    void eraseName(QString unique);
    static bool parserPackage(QString json, QString &package_name, QVector<TKuInstallItem> &vt);
};
