
#include "Common.h"
#include "QWebEngineView"
#include "QuickInstallView.h"
#include "qwebenginepage.h"
#include "SignalManager.h"
#include "ConnectManager.h"
#include "UMessageBox.h"
#include "configuration_option\GlobalConfiguration.h"
#include "PackageStateManager.h"
#include "DataManager.h"
#include "qvector.h"
CQuickInstallView::CQuickInstallView(CTaskWndDialog *pTaskWnd, QWidget *parent)
    : QWidget(parent), _pTaskCenterWnd(pTaskWnd), _pQuickListWidget(NULL), _installGroup(NULL), _installBtnGroup(NULL)
{
    ui.setupUi(this);

    QString stylesheet = Common::readStyleSheet(":/qss/quickPage.qss");
    if (!stylesheet.isEmpty()) {
        setStyleSheet(stylesheet);
    }

    ui.quickInstall_noNet->setVisible(false);
    _initThreadFlag = QUICK_REC | CUSTOM_REC;
    setGeometry(QRect(189, 94, 801, 560));
    _pQuickListWidget = new CQuickListWidget(QRect(0, 0, 801, 560), 4, this);
    _pQuickListWidget->setMargin(12);
    _pQuickListWidget->setVerticalSpacing(14);
    _pQuickListWidget->setHorizontalSpacing(18);
    _pQuickListWidget->setStyleSheet("background-color:#ffffff;border:0px;");
    
    _itemStyleSheet = Common::readStyleSheet(":/qss/quickListItem.qss");

    _installGroup = new QButtonGroup;
    _installBtnGroup = new QButtonGroup;
    _timer.setInterval(10000);
    connect(CSignalManager::getInstance(), SIGNAL(addQuickItemSingal(QWidget *)),
        _pQuickListWidget, SLOT(addItemSlot(QWidget *)));
    connect(CSignalManager::getInstance(), SIGNAL(createQuickItemSignal(TPackageVector)),
        this, SLOT(createQuickItemSlot(TPackageVector)));
    connect(CSignalManager::getInstance(), SIGNAL(completedDownloadIconSignal(QString, QString)),
       this, SLOT(setIconSlot(QString, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(quickInstallChangeSoftStatusSignal(QString, SOFT_STATE)),
        this, SLOT(changeSoftStatusSlot(QString, SOFT_STATE)));
    connect(CSignalManager::getInstance(), SIGNAL(AckApkListSignal(QString, QString)),
        this, SLOT(sendInstalledApkListSlot(QString, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(restoreOriginalStateSignal()),
        this, SLOT(restoreOriginalStateSlot()));
    connect(_installGroup, SIGNAL(buttonClicked(QAbstractButton *)),
        this, SLOT(installQuickSlot(QAbstractButton *)));
    connect(_installBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)),
        this, SLOT(installBtnQuickSlot(QAbstractButton *)));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timerOutSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(quickNoNetSignal(bool)), this, SLOT(NoNetSlot(bool)));
    startThread();
}

void CQuickInstallView::NoNetSlot(bool bVisible)
{
    ui.quickInstall_noNet->setVisible(bVisible);
    if (bVisible) {
        _pQuickListWidget->setVisible(false);
    }
    else {
        _pQuickListWidget->setVisible(true);
    }
}

void CQuickInstallView::timerOutSlot()
{
    if (Common::isNetConnected()) {
        _timer.stop();
        startThread();
    }
}

void CQuickInstallView::startThread()
{
    _pthread = new InitQuickInstallThread;
    connect(_pthread, SIGNAL(ThreadFinishSignal(bool,int)), this, SLOT(ThreadFinishSlot(bool,int)));
    _pthread->setFlag(_initThreadFlag);
    _pthread->start();
}

void CQuickInstallView::ThreadFinishSlot(bool bRestart, int flag)
{
    disconnect(_pthread, SIGNAL(ThreadFinishSignal(bool, int)), this, SLOT(ThreadFinishSlot(bool, int)));
    delete _pthread;
    _pthread = NULL;
    _initThreadFlag = flag;
    if (bRestart) {
        _timer.start();
    }
}

CQuickInstallView::~CQuickInstallView()
{
    delete _installGroup;
}
void CQuickInstallView::installBtnQuickSlot(QAbstractButton *item)
{
    QWidget *parent = (QWidget *)item->parent();
    installQuickSlot((QAbstractButton *)parent);
}

void CQuickInstallView::installQuickSlot(QAbstractButton *item)
{
    QPushButton *pLabel = item->findChild<QPushButton *>("quick_state");
    if (!pLabel) return;

    if (pLabel->text() == "重试" || pLabel->text() == "安装") {
        auto it = _mapInfo.find(item);
        if (it != _mapInfo.end()) {
            quickSingleInstallSlot(*it);
        }
    }
    
}

void CQuickInstallView::quickSingleInstallSlot(TKuInstallItem item)
{
    
}

void CQuickInstallView::sendInstalledApkListSlot(QString serial, QString data)
{
}


void CQuickInstallView::changeSoftStatusSlot(QString packageName, SOFT_STATE nState)
{
    auto it = _map.find(packageName);
    if (it!=_map.end())
    {
        CQuickListItem *parent = static_cast<CQuickListItem *>(it.value());
        if (nState == WAIT_DOWNLOAD) {
            QPushButton *state = parent->findChild<QPushButton *>("quick_state");
            if (state->text() != "重试" && state->text() != "安装") {
                return;
            }
        }
        emit parent->setStateSignal(nState);
    }
}

void CQuickInstallView::restoreOriginalStateSlot()
{
    auto it = _map.begin();
    for (it; it != _map.end();it++)
    {
         emit CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(it.key(), RESTOREINSTALL);
    }
}

void CQuickInstallView::createQuickItemSlot(TPackageVector vt)
{
    TKuInstallItem *item = NULL;
    TListItemVector vt_l;
    for (auto it = vt.begin(); it != vt.end();it++)
    {
        item = &(*it);
        CQuickListItem *pQwidget = new CQuickListItem(getListContainer());

        pQwidget->setStyleSheet(_itemStyleSheet);
        if (pQwidget)
        {
            _map.insert(item->_packageName, pQwidget);
            _mapInfo.insert(pQwidget, *item);
        }
        else
            return;
        _installGroup->addButton(pQwidget);
        QPushButton *btn = pQwidget->findChild<QPushButton *>("quick_state");
        if (btn) {
            _installBtnGroup->addButton(btn);
        }
        
        emit pQwidget->setItemInfoSignal(item->_name, item->_introduce, item->_isCooperate == "1" ? true : false);
        
        if (!Common::isIconFileExist(item->_icon))
        {
            QString unique = QString(item->_packageName).append("_quickInstall");
            emit CSignalManager::getInstance()->addDownloadIconSignal(item->_icon, unique);
        }
        else
        {
            emit pQwidget->setIconSignal(Common::getIconSavePath(item->_icon));
        }
        vt_l.push_back(pQwidget);
        
    }
    
   
    if (vt_l.size() > 0)
    {
        _pQuickListWidget->addItemsSlot(vt_l);
    }
    
}

void CQuickInstallView::setIconSlot(QString icon, QString unique)
{
    if (!unique.endsWith("_quickInstall"))
        return;
    QString soft_name = unique.mid(0, unique.length() - QString("_quickInstall").length());
    auto it = _map.find(soft_name);
    if (it != _map.end()) {
        emit static_cast<CQuickListItem *>(*it)->setIconSignal(Common::getIconSavePath(icon));
    }
}

void InitQuickInstallThread::run()
{
   
    int flag = 0;
    //自定义套餐初始化
    QVector<TKuInstallItem> vt;
    bool bOk = _flag & CUSTOM_REC ? true : false;
    if (bOk)
    {
        if (vt.size() > 0)
        {
            emit CSignalManager::getInstance()->customNoNetSignal(false);
            int index = 8;
            int size = vt.size();

            //通过循环
            while (size > 0)
            {
                if (size > 8)
                {
                    QVector<TKuInstallItem> mid = vt.mid(index, 8);
                    emit CSignalManager::getInstance()->createCustomItemSignal(mid);
                    index += 8;
                    size -= 8;
                }
                else {
                    QVector<TKuInstallItem> mid = vt.mid(index, size);
                    emit CSignalManager::getInstance()->createCustomItemSignal(mid);
                    break;
                }
                QThread::msleep(400);
            }
        } else {
            flag |= CUSTOM_REC;
            emit CSignalManager::getInstance()->customNoNetSignal(true);
        }
    }
    

    //快捷安装初始化
    vt.clear();
    bOk = _flag & QUICK_REC ? true : false;
    if (bOk)
    {
        if (vt.size() > 0)
        {
            emit CSignalManager::getInstance()->quickNoNetSignal(false);
            CDataManager::getInstance()->insert(QUICK_DATA_MANAGER_KEY, vt);
            int index = 8;
            int size = vt.size();

            //通过循环
            while (size > 0)
            {
                if (size > 8)
                {
                    QVector<TKuInstallItem> mid = vt.mid(index, 8);
                    emit CSignalManager::getInstance()->createQuickItemSignal(mid);
                    index += 8;
                    size -= 8;
                } else {
                    QVector<TKuInstallItem> mid = vt.mid(index, size);
                    emit CSignalManager::getInstance()->createQuickItemSignal(mid);
                    break;
                }
                QThread::msleep(400);
            }
        } else {
            flag |= QUICK_REC;
            emit CSignalManager::getInstance()->quickNoNetSignal(true);
        }
    }
    bool brestart = false;
    if (flag > 0) {
        brestart = true;
    }
    emit ThreadFinishSignal(brestart, flag);
}