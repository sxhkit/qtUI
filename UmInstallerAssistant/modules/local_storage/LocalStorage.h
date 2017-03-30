#ifndef LOCAL_STORAGE_H
#define LOCAL_STORAGE_H

#include <qobject.h>
#include <qmutex.h>
#include "Common.h"

class LocalStorage: public QObject {
    Q_OBJECT

public:
    LocalStorage();
    ~LocalStorage();

    /* 加载装机上报信息的api
    *  @data: [out]  将存储在本地文件中的信息加载到该变量中
    */
    void loadUploadData(QVector<QByteArray>& data);
    /* 清理所有数据的api
    */
    void cleanData();
public slots:
    /* 保存装机上报信息的api，通常情况下是在上传失败的时候调用此api
    *  @data: [in] 需要存储到本地的数据
    */
    void saveUploadData(const QByteArray& data);
    /* 保存装机上报信息的api， 可一次保存多条记录, 但是会将本地存储的内容覆盖掉
    *  @data: [in] 需要存储到本地的数据
    */
    void saveUploadData(QVector<QByteArray>& data);

private:
    QString _dataPath;
    QMutex _mutex;
};

#endif