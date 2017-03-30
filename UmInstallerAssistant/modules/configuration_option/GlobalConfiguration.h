#ifndef GLOBAL_CONFIGURATION_H
#define GLOBAL_CONFIGURATION_H

#include <qobject.h>
#include <qfile.h>
#include <qdir.h>
#include <qmutex.h>
#include <qqueue.h>
#include <qsqldatabase.h>
#include "qvector.h"
#include "configuration.h"
#include "qsettings.h"

class DatabaseMan: public QObject{
    Q_OBJECT
public:
    static DatabaseMan* getInstance()
    {
        static DatabaseMan db;
        return &db;
    }
    ~DatabaseMan();
    QString queryVersion(QString packageName, QString userid);
    bool saveVersion(QString packageName, QString version, QString userid);
    bool updateVersion(QString packageName, QString version, QString userid);
    bool saveUrl(QString requestUrl, QString realUrl, QString md5);
    bool queryUrl(QString requestUrl, QString& downloadUrl, QString& md5);
    bool saveOurselvesApkList(QString info, QString uuid);
    QString queryOurselvesApkList(QString uuid);
    QString queryCustomPackage(QString key, QString userid);
    bool saveCustomPackage(QString key, QString packageInfo, QString userid);
    bool updateCustomPackage(QString key, QString packageInfo, QString userid);
    QVector<CustomPackageInfo> queryCustomPackages(QString userid);
    bool deleteCustomPackage(QString key, QString userid);
    bool saveRecommendPackage(QString key, QString info, QString userid);
    QString queryRecommendPackage(QString key, QString userid);
private:
    DatabaseMan();
    bool createSoftPackageTable();
    bool createCustomPackageTable();
    bool createRecommendTable();
    bool createUrlTable();
    bool createOurselvesApklistTable();
    QString _databasePath;
    QSqlDatabase _db;
    QMutex _mutex;
};

class GlobalConfiguration: public QObject{
    Q_OBJECT

public:
    static GlobalConfiguration* getInstance()
    {
        static GlobalConfiguration globalConfiguration;
        return &globalConfiguration;
    }
    QString getUserId();
    quint16 applyForPort();
    quint16 logoutPort(quint16 port);

private:
    GlobalConfiguration()
    {
        _userId = "";
        getUserId();
        for ( int i = 0; i < MAX_ANDROID_DEVICES; i++ )
        _portQueue.enqueue(LOCAL_TCP_PORT_BASE + i);
    }

    QString _userId;
    QQueue<quint16> _portQueue;
    QMutex _portMutex;
};

class GlobalIniCfg
{
    GlobalIniCfg();
public:
    static GlobalIniCfg *getInstance()
    {
        static GlobalIniCfg globalIniCfg;
        return &globalIniCfg;
    }
    ~GlobalIniCfg();

    QString queryValue(QString key);
    void setValue(QString key, QString val);
    QString queryGlobalValue(QString key);
    void setGlobalValue(QString key, QString val);

    int getRecPackageNum();
private:
    QString getAppPath();
private:
    QMutex _mutex;
    QSettings  *_qSetting;
    QSettings  *_globalSetting;
    QString _uuid;
};

#endif