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

//初始化每一个连接
void CMoblieItem::initConnectItem()
{
    QString childName;
    //进度条
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

    //手机名显示控件
    childName = QString("mobile_name");
    pLabel = pItem->findChild<QLabel *>(childName);
    if (!pLabel) return;
    pLabel->setText(QString("手机未连接"));

    //已连接，已安装，完成安装 显示控件

    childName = QString("install_state");
    pLabel = findChild<QLabel *>(childName);
    if (!pLabel) return;
    pLabel->setText(QString("请连接"));

    //安装统计显示控件
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
    //图标
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/connected");
    pLabel->setPixmap(pixmap);

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText(model);
    pLabel->setVisible(true);

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("已连接"));
}

void CMoblieItem::setUIUnconnectedInfo( const QString &model)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/unconnected");
    pLabel->setPixmap(pixmap);

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText("手机未连接");

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("请连接"));
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
        pLabel->setText("请重新插拔手机");
    } else {
        pLabel->setText("请打开USB调试权限");
    }

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("连接失败"));
}

void CMoblieItem::setUIConnnectingInfo(const QString &model)
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/connecting");
    pLabel->setPixmap(pixmap); 

    pLabel = findChild<QLabel *>("mobile_name");
    if (!pLabel) return;
    pLabel->setText("正在连接手机...");

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("请稍候"));
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

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("安装中"));

    pLabel = findChild<QLabel *>("installed_count");
    if (!pLabel) return;
    QString str = QString("%1/%2").arg(QString::number(success), QString::number(num));
    pLabel->setText(str);
    QProgressBar *progress = findChild<QProgressBar *>("progressBar");
    if (!progress) return;
    int val = finished * 100 / (num + 1);//+1 因为默认桌面 保证进度不满
    progress->setValue(val);
    this->update();
}

bool CMoblieItem::isInstallState()
{
    QLabel *pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return false;
    return  (pLabel->text() == "安装中" ? true : false);
}

void CMoblieItem::setInstallFinishInfo()
{
    QLabel *pLabel = findChild<QLabel *>("IconImage");
    QPixmap pixmap(":/image/installed");
    pLabel->setPixmap(pixmap);

    //已连接，已安装，完成安装 显示控件
    pLabel = findChild<QLabel *>("install_state");
    if (!pLabel) return;
    pLabel->setText(QString("已安装"));

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