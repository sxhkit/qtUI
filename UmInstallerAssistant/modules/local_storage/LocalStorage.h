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

    /* ����װ���ϱ���Ϣ��api
    *  @data: [out]  ���洢�ڱ����ļ��е���Ϣ���ص��ñ�����
    */
    void loadUploadData(QVector<QByteArray>& data);
    /* �����������ݵ�api
    */
    void cleanData();
public slots:
    /* ����װ���ϱ���Ϣ��api��ͨ������������ϴ�ʧ�ܵ�ʱ����ô�api
    *  @data: [in] ��Ҫ�洢�����ص�����
    */
    void saveUploadData(const QByteArray& data);
    /* ����װ���ϱ���Ϣ��api�� ��һ�α��������¼, ���ǻὫ���ش洢�����ݸ��ǵ�
    *  @data: [in] ��Ҫ�洢�����ص�����
    */
    void saveUploadData(QVector<QByteArray>& data);

private:
    QString _dataPath;
    QMutex _mutex;
};

#endif