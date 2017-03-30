#include "qfile.h"
#include "QtCore"
#include "QtGui"
#include "Common.h"
#include "qprogressbar.h"
#include "MoblieItem.h"
#include "UmInstallerAssistant.h"

CMoblieItem::CMoblieItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initConnectItem();
    QString styleSheet = Common::readStyleSheet(":/qss/moblieItem.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    connect(this, SIGNAL(setItemInstallProgressSignal(int ,int,int)), this, SLOT(setItemInstallProgressSlot(int, int,int)));
    connect(this, SIGNAL(setItemMobileModelSignal(QString)), this, SLOT(setItemMobileModelSlot(QString)));
}

CMoblieItem::~CMoblieItem()
{

}

//��ʼ��ÿһ������
void CMoblieItem::initConnectItem()
{
    QString childName;
    //������
    childName = QString("progressBar");
    QProgressBar *progress = findChild<QProgressBar *>(childName);
    if (!progress)
        return;
    progress->setValue(0);
    progress->setVisible(true);

    QWidget *pItem = progress;
    //icon
    childName = QString("IconImage");
    QLabel *pLabel = findChild<QLabel *>(childName);

    //�ֻ�����ʾ�ؼ�
    childName = QString("mobile_name");
    pLabel = pItem->findChild<QLabel *>(childName);
    if (!pLabel) return;
    pLabel->setText(QString("�ֻ�δ����"));

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�

    childName = QString("install_state");
    pLabel = findChild<QLabel *>(childName);
    if (!pLabel) return;
    pLabel->setText(QString("������"));

    //��װͳ����ʾ�ؼ�
    childName = QString("installed_count");
    pLabel = findChild<QLabel *>(childName);
    if (!pLabel) return;
    pLabel->setText(QString(""));
}

void CMoblieItem::setItemState( DeviceState nstate, const QString &model)
{
    switch (nstate)
    {
    case UNCONNECT:
        setUIUnconnectedInfo(model);
        break;
    case CONNECTFAILED:
        setUIConnnectFailedInfo(model);
        break;
    case CONNECTING:
        setUIConnnectingInfo(model);
        break;
    case CONNECTED:
        setUIConnectedInfo(model);
        break;
    case CONNECTOFFLINE:
        setUIConnnectFailedInfo(model,true);
    default:
        break;
    }
}

void CMoblieItem::setUIConnectedInfo( const QString &model)
{
    //ͼ��
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/connected");
    pLabel->setPixmap(pixmap);

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText(model);
    pLabel->setVisible(true);

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("������"));
}

void CMoblieItem::setUIUnconnectedInfo( const QString &model)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/unconnected");
    pLabel->setPixmap(pixmap);

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText("�ֻ�δ����");

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("������"));
    pLabel = findChild<QLabel *>("installed_count");
    if (!pLabel) return;
    pLabel->setText("");

    QProgressBar *progress = findChild<QProgressBar *>("progressBar");
    if (!progress) return;
    progress->setValue(0);
}

void CMoblieItem::setUIConnnectFailedInfo(const QString &model, bool bOffline)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/connectfailed");
    pLabel->setPixmap(pixmap);

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    if (bOffline) {
        pLabel->setText("�����²���ֻ�");
    } else {
        pLabel->setText("���USB����Ȩ��");
    }

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("����ʧ��"));
}

void CMoblieItem::setUIConnnectingInfo(const QString &model)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/connecting");
    pLabel->setPixmap(pixmap); 

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText("���������ֻ�...");

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("���Ժ�"));
    pLabel = findChild<QLabel *>("installed_count");
    if (!pLabel) return;
    pLabel->setText("");

    QProgressBar *progress = findChild<QProgressBar *>("progressBar");
    if (!progress) return;
    progress->setValue(0);
}

void CMoblieItem::setItemInstallProgressSlot(int num, int success,int finished)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/installing");
    pLabel->setPixmap(pixmap);

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("��װ��"));

    pLabel = findChild<QLabel *>("installed_count");
    if (!pLabel) return;
    QString str = QString("%1/%2").arg(QString::number(success), QString::number(num));
    pLabel->setText(str);
    QProgressBar *progress = findChild<QProgressBar *>("progressBar");
    if (!progress) return;
    int val = finished * 100 / (num + 1);//+1 ��ΪĬ������ ��֤���Ȳ���
    progress->setValue(val);
    this->update();
}

bool CMoblieItem::isInstallState()
{
    QLabel *pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return false;
    return  (pLabel->text() == "��װ��" ? true : false);
}

void CMoblieItem::setInstallFinishInfo()
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/installed");
    pLabel->setPixmap(pixmap);

    //�����ӣ��Ѱ�װ����ɰ�װ ��ʾ�ؼ�
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("�Ѱ�װ"));

    QProgressBar *progress = findChild<QProgressBar *>("progressBar");
    if (!progress) return;
    progress->setValue(100);
}

void CMoblieItem::setItemMobileModelSlot(QString model)
{
    QLabel *pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText(model);
}