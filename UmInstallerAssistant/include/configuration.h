#include "qstring.h"
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define SOFTWARE_MAINVERSION                "1.0"
#define SOFTWARE_SUBVERSION                 "1"

#define SOFTWARE_VERSION                    SOFTWARE_MAINVERSION##"."##SOFTWARE_SUBVERSION

#define HTTP_KEY    "ithis is the error code"
#define POST_INSTALL_DATA_URL               "http://shouji.youmeng.com/api/zs/installLog"
#define POST_CONNECTED_DATA_URL             "http://shouji.youmeng.com/Api/Zs/toChainingReport"
#define GET_ANDROID_SYSTEM_NAME_URL         "http://shouji.youmeng.com/api/Zs/getAllSystem"
#define GET_SIMULATION_CONFIGURATION_URL    "http://shouji.youmeng.com/api/Zs/getImitateClickList"
#define GET_OURSELF_APK_LIST_URL            "http://shouji.youmeng.com/api/Zs/getRecommandSoftlist"
#define GET_MOBILE_NUM_URL                  "http://shouji.youmeng.com/api/zs/getUserMobile"
#define GET_SOFT_VERSION                    "http://shouji.youmeng.com/Api/Zs/getNewVersion"

#define USERID_FILE                         "uuid"
#define DEFAULT_UUID                        "100060"

#define DATABASE_NAME                       "local.db"
#define INI_CFG_NAME                        "cfg.ini"
#define INI_GLOBAL_CFG_NAME                 "global.ini"

#define REMOTE_FILE_LOCATION                "/data/local/tmp"

#define ANDROID_REMOTE_TCP_PORT             50036
#define LOCAL_TCP_PORT_BASE                 50036
#define MAX_ANDROID_DEVICES                 5

#define DEFAULT_PACKAGE_NUM                 7
#define QUICK_DATA_MANAGER_KEY              1000
typedef enum TaskType
{
    BATCH_TASK = 1,
    SINGLE_TASK = 2,
    QUICK_TASK = 3,
    RETRY_TASK = 4,
    NECESSARY_TASK = 5,
    ERROR_TASK = 6,
};

enum PackageFrom {
    SELF = 1, 
    OTHER, 
};

/* 安装软件的信息
*/
typedef struct TKuInstallItem {
	QString _name;
	QString _icon;
	QString _packageName;
	unsigned __int64 _fileSize;
	QString _version;
	QString _downloadUrl;
	QString _filemd5;
    QString _isCooperate;
    QString _source;
    QString _indentify;
    TKuInstallItem()
    {
        _fileSize = 0;
        _packageFrom = OTHER;
        _needDownloadProgress = true;
        _type = NECESSARY_TASK;
    }
    QString _savePath;
    QString _pushedPath;
    TaskType _type;
    PackageFrom _packageFrom;
    QString _serial;
    bool _needDownloadProgress;
    QString _packageUpdateUnique;
    QString _introduce;
    float _price;
}TKuInstallItem, *PTKuInstallItem;

typedef enum SOFT_STATE
{
    DOWNLOADING = 0,
    INSTALLING,
    DOWNLOADSUCCESS,
    WAIT_DOWNLOAD,
    INSTALLFAILED,
    DOWNLOADFAILED,
    INSTALLSUCCESS,
    RESTOREINSTALL,   //此状态用于快捷安装页面恢复安装的状态
    ERROR_STATE,
};

enum ListItemType {
    DEFAULT = 0,
    HIGH_DOWNLOAD,
    HIGH_CONVERSION
};

typedef struct CustomPackageInfo
{
    QString package_unique_key;
    QString package_json;
}CustomPackageInfo;

enum DeviceState {
    UNCONNECT = 0,
    CONNECTFAILED,
    CONNECTING,
    CONNECTED,
    CONNECTOFFLINE
};

typedef struct LinkInfo
{
    DeviceState nState;
    __int64 connectTime;
    QString serNum;
    QString model;
    void *pItem;
    LinkInfo()
    {
        nState = UNCONNECT;
    }

    bool operator == (LinkInfo& ele)
    {

        return (ele.serNum == serNum &&
            ele.nState == nState &&
            ele.pItem == pItem
            );
    }

    LinkInfo& operator=(LinkInfo ele)
    {
        serNum = ele.serNum;
        nState = ele.nState;
        pItem = ele.pItem;
        return *this;
    }
}LinkInfo;

typedef QVector<TKuInstallItem>  TPackageVector;
typedef QVector<QString>  TMobileVector;
typedef QMap<QString, QString> TQMap;

#endif