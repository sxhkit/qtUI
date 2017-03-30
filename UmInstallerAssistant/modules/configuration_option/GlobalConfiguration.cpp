#include "GlobalConfiguration.h"
#include <qstring.h>
#include <qiodevice.h>
#include "configuration.h"
#include <qdir.h>
#include <qdebug.h>
#include <qsqlquery.h>
#include <qregexp.h>
#include <qsqlerror.h>
#include <qstandardpaths.h>
#include <qsqldriver.h>
#include "Common.h"

DatabaseMan::DatabaseMan()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if ( !dir.exists(path) ) {
        dir.mkpath(path);
    }
    path.append("/");
    path.append(DATABASE_NAME);
    _databasePath = path;
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(path);
    if ( !_db.open()) {
        qDebug() << __FILE__ << ":" << __LINE__ << " " << _db.lastError().text();
        return;
    }   
    createSoftPackageTable();
    createCustomPackageTable();
    createRecommendTable();
    createUrlTable();
    createOurselvesApklistTable();
}

DatabaseMan::~DatabaseMan()
{
    _db.close();
}

   
bool DatabaseMan::createSoftPackageTable()
{
    //table name is local_soft_info
    QString sql = "create table if not exists local_soft_info(package_name TEXT, version TEXT, user_id TEXT, unique( package_name, user_id))";
    QSqlQuery query(_db); 
    return query.exec(sql);
}
    
bool DatabaseMan::createCustomPackageTable()
{
    return true;
}
    
bool DatabaseMan::createRecommendTable()
{
   
    return true;
}
    
bool DatabaseMan::createUrlTable()
{
    return true;
   
}

bool DatabaseMan::createOurselvesApklistTable()
{
    return true;
    
}

bool DatabaseMan::saveOurselvesApkList(QString info, QString uuid)
{
    
    return false;     
        
}

QString DatabaseMan::queryOurselvesApkList(QString uuid)
{
   
    QString apkList;
    
    return apkList;
}

bool DatabaseMan::queryUrl(QString requestUrl, QString& downloadUrl, QString& md5)
{
   
    return true;
}
    
bool DatabaseMan::saveUrl(QString requestUrl, QString realUrl, QString md5)
{
    
    return false;            
}

QString DatabaseMan::queryVersion(QString packageName, QString userid)
{
    
    QString version;
    
    return version;
}
    
bool DatabaseMan::saveVersion(QString packageName, QString version, QString userid)
{
    
    return true;
}

bool DatabaseMan::updateVersion(QString packageName, QString version, QString userid)
{
    QString sql;
    QTextStream(&sql) << "update local_soft_info set version = '" << version << "'where package_name = '" << packageName << "' and user_id = '" << userid << "'";
    _mutex.lock();
    QSqlQuery query(_db); 
    query.exec(sql);
    _mutex.unlock();
    return true;
}

QString DatabaseMan::queryCustomPackage(QString key, QString userid)
{
    _mutex.lock();
    QString sql;
    QTextStream(&sql) << "select packageinfo from custom_package where package_uniquekey = '" << key << "' and user_id = '" << userid << "'";
    QSqlQuery query(_db);
    query.exec(sql);
    QString packageinfo;
    while ( query.next() ) {
        packageinfo = query.value("packageinfo").toString();
    }
    _mutex.unlock();
    return packageinfo; 
}

QVector<CustomPackageInfo>DatabaseMan::queryCustomPackages(QString userid)
{
   
    QVector<CustomPackageInfo> vt;
    
    return vt;
}
    
bool DatabaseMan::saveCustomPackage(QString key, QString packageInfo, QString userid)
{
    
    return true;
}
    
bool DatabaseMan::updateCustomPackage(QString key, QString packageInfo, QString userid)
{
    
    return true;
}

bool DatabaseMan::deleteCustomPackage(QString key, QString userid)
{
   
    return true;
}

bool DatabaseMan::saveRecommendPackage(QString key, QString info, QString userid)
{
    
    return true;
}

QString DatabaseMan::queryRecommendPackage(QString key, QString userid)
{
    _mutex.lock();
    QString sql;
    QTextStream(&sql) << "select packageinfo from recommend_package where package_uniquekey = '" << key << "' and user_id = '" << userid << "'";
    QSqlQuery query(_db);
    query.exec(sql);
    QString packageinfo;
    while (query.next()) {
        packageinfo = query.value("packageinfo").toString();
    }
    _mutex.unlock();
    return packageinfo;
}

QString GlobalConfiguration::getUserId()
{
    
    return _userId;
}
    
quint16 GlobalConfiguration::applyForPort()
{
    _portMutex.lock();
    quint16 port = _portQueue.dequeue();  
    _portMutex.unlock();
    return port; 
}

quint16 GlobalConfiguration::logoutPort(quint16 port)
{
    _portMutex.lock();
    _portQueue.enqueue(port);
    _portMutex.unlock();
    return port;
}

GlobalIniCfg::GlobalIniCfg()
{
    QString path_app = getAppPath();
    QDir dir;
    QString cur;
    
    _qSetting = new QSettings(path_app, QSettings::IniFormat);
    _qSetting->setIniCodec(QTextCodec::codecForName("utf-8"));

    _globalSetting = new QSettings(cur, QSettings::IniFormat);
    _globalSetting->setIniCodec(QTextCodec::codecForName("utf-8"));

    _uuid = GlobalConfiguration::getInstance()->getUserId();
}

QString GlobalIniCfg::getAppPath()
{
    QString path;
   
    return path;
}

GlobalIniCfg::~GlobalIniCfg()
{
    delete _qSetting;
}

QString GlobalIniCfg::queryValue(QString key)
{
    QString val = "";
    if (_qSetting)
    {
        QString realkey = QString("/").append(_uuid).append("/").append(key);
        val = _qSetting->value(realkey).toString();
    }
    return val;
}
void GlobalIniCfg::setValue(QString key, QString val)
{
    if (_qSetting)
    {
        QString realkey = QString("/").append(_uuid).append("/").append(key);
        _qSetting->setValue(realkey, val);
    }
}

QString GlobalIniCfg::queryGlobalValue(QString key)
{
    QString val = "";
    if (_globalSetting)
    {
        QString realkey = QString("/global").append("/").append(key);
        val = _globalSetting->value(realkey).toString();
    }
    return val;
}

void GlobalIniCfg::setGlobalValue(QString key, QString val)
{
    if (_globalSetting)
    {
        QString realkey = QString("/global").append("/").append(key);
        _globalSetting->setValue(realkey, val);
    }
}

int GlobalIniCfg::getRecPackageNum()
{
    int res = 0;
    if (_globalSetting)
    {
        QString val = _globalSetting->value("/global/rec_package_list").toString();
        QStringList list = Common::qstr2QstrList(val, "&");
        res = list.size();
    }
    return res;
}