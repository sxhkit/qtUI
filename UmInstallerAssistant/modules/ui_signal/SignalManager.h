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
    //ɾ���ײ��б������ײ͵��ź�
    void deleteOnekeyItemSignal(QWidget *item);
    void deleteBatchItemSignal(QString unique);

    void setBatchItemUniqueSignal(QWidget *pWidget,QString unique);
    void setOnekeyItemUniqueSignal(QWidget *pWidget, QString unique);

    void setBatchPackageInfoByUniqueSignal(QString, QString);
    void setOnekeyPackageInfoByUniqueSignal(QString, QString);


    void showOneKeyQueryInfoSingal(QWidget *pItem,QString unique);
    void hideOneKeyQueryInfoSingal();

    // һ��װ�� ��װ�ź�
    void InstallItemSignal(QString unique);

    //���� �ײ���Ϣ�� �ź�
    void setBatchPackageInfoSignal(QWidget *pWidget, QString packagename, float size, int num);
    void setOnekeyPackageInfoSignal(QWidget *pWidget, QString packagename, float size, int num);

    //����װ�� ��װ�ź�
    void startInstallSingal(TPackageVector vt,TMobileVector vtMpbile);

    //�����ֻ���װ������������ź�
    void mobileConnectedSignal(QString serial);
    void mobileDisconnectSignal(QString serial);

    //�����������item
    void addTaskItemSignal(QString package,QString icon, QString name, float fsize);
    void addTaskItemFromQuickSignal(QString package, QString icon, QString name, float fsize);
    
    void setItemStateSignal(QString package,SOFT_STATE nstate);
    void setErrorInfoSignal(QString package,QString err);
    void setProgressValueSignal(QString package,int val);

    //����ײ�
    void addPackageSignal(TPackageVector vt, QString packagename, QString unique_key, ListItemType type);
    // ����icon �ź�
    void addDownloadIconSignal(QString url, QString unique);
    //ĳ�����������
    void completedDownloadOne(TKuInstallItem item, bool res, QString serial);
    //icon�����������
    void completedDownloadIconSignal(QString savepath, QString identify);
    //����׿�ֻ������ļ����
    void completedPushfileOne(TKuInstallItem item, bool res, QString serial, QString info);
    //���ĳ���ļ��İ�װ
    void completedInstallOne(TKuInstallItem item, bool res, QString serial, QString info);
    //��һ���ļ������ؽ��ȵĻ㱨
    void downloadProgress(QString identify, qint64 bytesReceived, qint64 bytesTotal);

    //����װ�� ��װ����
    void batchInstallProgressSignal(QString serial, int success, int total,int finished);
    //��ʼ���ص��ź�
    void startDownloadSignal(TKuInstallItem item);
    //�༭�Զ����ײ�
    void modifyPackageSignal(QString unique);
    //��ݰ�װ���װ
    void quickSingleInstallSignal(TKuInstallItem item);
    //ĳ���ײͰ�װ��ɺ��͵��źţ����������ϱ�
    void completePackageInstallSignal(QString serial, QString packageKey,QString updateUniqueKey);
    void sendCompletionStatusToAndroidSignal(QString serial, int installed, int total);
    void sendInstalledStatusToAndroidSignal(QString serial, int installed, int total);
    
    //��ݰ�װ����״̬�ź�
    void quickInstallChangeSoftStatusSignal(QString, SOFT_STATE);

    //������������item
    void clearListSignal();
    //�����ײ͹������������ֻ����ײ���Ϣ
    void clearPackageSignal(QString serial,bool bonekey);

    //һ��װ���ײͰ�װ���
    void finishOnekeyPackageAbnormalSignal();    //�γ��쳣���
    void finishedOnekeyPackageInstallSignal(int num, int success);//������װ������
    //����װ���İ�װ���
    void finishedBatchPackageInstallSignal(QString serial);
    void finishedAllBatchPackageInstallSignal();
    //��ʼ��װĳ����� ��ͬ�ڿ�ʼ��װstartInstallSingal ��װ�ײ�
    void startInstallOneSoftSignal(TKuInstallItem item);

    //һ��װ�� ���ӵ��ֻ������䷨ʱ��Ҫ���� ��ȡ�µ��ֻ���װAPK���ź�
    void queryApkListSignal(QString serial);

    // ���ֻ�����ʱ��ԭ��ݰ�װҳ�� ���� ���״̬Ϊ��װ״̬
    void restoreOriginalStateSignal();

    //tcp ���ӷ��ص��ź� ���ݰ�װ��Ӧ���б�
    void AckApkListSignal(QString serial,QString data);

    // �û��绰��ȡ
    void setUserInfoSignal(QString);

    void startTailWorkSignal(QString serial,TaskType type);
    void completeTailWorkSignal(QString serial);

    //����һ��װ����ɾ����ť������    
    void sendDisableDeletePackageSignal(QString unique,bool isEnable);
    //���ÿͻ��˰汾��Ϣ
    void setVersionInfoSignal(QString ver, bool bMustUpdate);

    // ��������װ�� ��ť�Ƿ����
    void setBatchInstallBtnEnableSignal(bool bEnable);
    //�������ģ�����ͳ��
    void sendTaskCenterProgressSignal(int success, int num);

    // gif ֹͣ���߿�ʼ
    void startTaskCenterMovieSignal(bool bStart);

    void setSingleConnectStateSignal(QString serial ,QString model, DeviceState state);

    //��ȡsd����Ϣ���ź�
    void hasGotstorageInfoSignal(QString serial, TQMap info);
    void queryStorageInfoSignal(QString serial);

    void setMobileModelSignal(QString serial, QString model);

    void customNoNetSignal(bool bVisible);
    void quickNoNetSignal(bool bVisible);
    //�޸Ĳ�ѯҳ����ź�
    void changeItemSignal(QString unique);

    //ˢ�²�ѯҳ��
    void refreshQueryInfoViewSignal(QString curUnique, QString changeUnique);
};
