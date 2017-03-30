
#include "UmInstallerAssistant.h"
#include "qfile.h"
#include <windowsx.h>
#include "QtCore"
#include "QtGui"
#include "qtextcodec.h"
#include "Common.h"
#include "qprogressbar.h"
#include "TaskWndDialog.h"
#include "configuration.h"
#include "SignalManager.h"
#include "ConnectManager.h"
#include "DataManager.h"
#include "configuration_option\GlobalConfiguration.h"
#include "qdatetime.h"
#include "PackageStateManager.h"
#include "qmovie.h"
#include "qlocale.h"
QWidget *g_main_frame = NULL;
UmInstallerAssistant::UmInstallerAssistant(QApplication *a, QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint)
{
    g_main_frame = this;
    qRegisterMetaType<TMobileVector>("TMobileVector");
    qRegisterMetaType<TPackageVector>("TPackageVector");
    qRegisterMetaType<SOFT_STATE>("SOFT_STATE");
    qRegisterMetaType<ListItemType>("ListItemType");
    qRegisterMetaType<TQMap>("TQMap");
    qRegisterMetaType<TListItemVector>("TListItemVector");
//    _checkDevices = new CheckDevices;
    this->setWindowIcon(QIcon(":/image/window_icon"));
    initUIinfo(a);
}

void UmInstallerAssistant::initUIinfo(QApplication *a )
{
    ui.setupUi(this);
//    _checkDevices->startAdb();
    _pQuickPageView = NULL;
    _pBatchPageView = new CBatchPageView(ui.centralWidget);
    _pBatchPageView->move(QPoint(189, 94));
    _pBatchPageView->setVisible(false);
    _pOnekeyPageView = new COnekeyPageView(ui.centralWidget);
    _pOnekeyPageView->move(QPoint(189, 94));
    _pOnekeyPageView->setVisible(false);
    //y阴影窗口导致web页面内嵌存在问题
    //setAttribute(Qt::WA_TranslucentBackground);

    QString styleSheet = Common::readStyleSheet(":/qss/mainframe.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);

    _tabButtonGroup = new QButtonGroup;
    _tabButtonGroup->addButton(ui.tab_btn_1);
    _tabButtonGroup->addButton(ui.tab_btn_2);
    _tabButtonGroup->addButton(ui.tab_btn_3);
    _tabButtonGroup->setExclusive(true);
    ui.tab_btn_1->setChecked(true);

    _taskCenterWnd = new CTaskWndDialog(this);
    _taskCenterWnd->setModal(true);
    _pQuickPageView = new CQuickInstallView(_taskCenterWnd,ui.centralWidget);
    _pQuickPageView->setVisible(false);
    _pAddItem = new COnekeyListAddItem(_pOnekeyPageView->getListContainer());
    _pInitThread = new InitPackageThread(_pAddItem);

    _movie.setFileName(":/image/main_frame_install_gif");
    ui.main_frame_install_gif->setMovie(&_movie);
    _movie.start();   // 如果不调用start gif 不会显示出来
    _movie.stop();

    _movie_c.setFileName(":/image/main_frame_install_gif");
    ui.single_gif->setMovie(&_movie_c);
    _movie_c.start();

    QString text = QString("版本").append(SOFTWARE_VERSION).append("(");
    ui.mainframe_version->setText(text);
    ui.mainframe_version_btn->setVisible(false);

    PackageStateMan::getInstance();
    connectSignal2Slot();
    emit CSignalManager::getInstance()->setSingleConnectStateSignal("","", UNCONNECT);
    if (a)
        connect(ui.main_frame_close, SIGNAL(clicked()), this, SLOT(close()));
    
    initPackageList();
    _pOnekeyPageView->setVisible(true);
    
}

void UmInstallerAssistant::connectSignal2Slot()
{
    connect(CSignalManager::getInstance(), SIGNAL(setMobileModelSignal(QString,QString)),
        ConnectManager::getInstance(), SLOT(setMobileModelSlot(QString, QString)));
    connect(_tabButtonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this, SLOT(tabClickSlot(QAbstractButton *, bool)));
    connect(ui.main_frame_min, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui.mainframe_link, SIGNAL(clicked()), this, SLOT(openUrlLinkSlot()));
    connect(ui.mainframe_taskcenter_btn, SIGNAL(clicked()), SLOT(openTaskWndSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(addPackageSignal(TPackageVector, QString, QString, ListItemType)),
        this, SLOT(createPackageItemSlot(TPackageVector, QString, QString, ListItemType)));
    connect(_pInitThread, SIGNAL(initFinish()), this, SLOT(refreshSlot()));
    connect(_pAddItem, SIGNAL(clicked()), _pOnekeyPageView, SLOT(OnekeyAddPackageClickSlot()));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(TimerTimeoutSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(setUserInfoSignal(QString)), this, SLOT(setUserInfoSlot(QString)));
    connect(CSignalManager::getInstance(), SIGNAL(sendTaskCenterProgressSignal(int ,int)), this, SLOT(sendTaskCenterProgressSlot(int, int)));
    connect(CSignalManager::getInstance(), SIGNAL(startTaskCenterMovieSignal(bool)), this, SLOT(startTaskCenterMovieSlot(bool)));
    connect(CSignalManager::getInstance(), SIGNAL(setSingleConnectStateSignal(QString, QString, DeviceState)),
        this, SLOT(setSingleConnectStateSlot(QString,QString, DeviceState)));
    connect(CSignalManager::getInstance(), SIGNAL(setVersionInfoSignal(QString, bool)), this, SLOT(setVersionInfoSlot(QString, bool)));
    connect(ui.mainframe_version_btn, SIGNAL(clicked()), this, SLOT(openUpdataLinkSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(hasGotstorageInfoSignal(QString, TQMap)), this, SLOT(setMemoryInfoSlot(QString,TQMap)));
    connect(ui.mainframe_sd_combox, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChangedSlot(QString)));
}

void UmInstallerAssistant::setUserInfoSlot(QString num)
{
    if (GlobalConfiguration::getInstance()->getUserId() == DEFAULT_UUID)
    {
        UMessageBox::getInstance()->showNormal("用户信息丢失，请重新下载有盟装机助手", UM_OK);
    }
    if (num.isEmpty())
    {
        ui.main_frame_user->setText("用户信息错误");
        ui.main_frame_user->setStyleSheet("color:red");
        _timer.start();
        return;
    }

    QString text ="欢迎你,";
    text.append(num);
    ui.main_frame_user->setText(text);
    _timer.start();
}

void UmInstallerAssistant::TimerTimeoutSlot()
{
   _timer.stop();
   QTime t= QTime::currentTime();
   setWelcome(t);
}

void UmInstallerAssistant::setWelcome(QTime time)
{
    QTime n1(6, 0, 0);
    QTime n2(8, 0, 0);
    QTime n3(12, 0, 0);
    QTime n4(14, 0, 0);
    QTime n5(20, 0, 0);
    QString text;
    int esp = 0;
    if (time < n1 || time > n5)
    {
        text = "晚上好!";
        esp = time.msecsTo(n1);

    }else if (time < n2)
    {
        text = "早上好!";
        esp = time.msecsTo(n2);
    }else if (time < n3)
    {
        text = "上午好!";
        esp = time.msecsTo(n3);
    }
    else if (time < n4)
    {
        text = "中午好!";
        esp = time.msecsTo(n4);
    }
    else if (time < n5)
    {
        text = "下午好!";
        esp = time.msecsTo(n5);
    }

    _timer.start(esp);
    ui.main_frame_hello->setText(text);
}

void UmInstallerAssistant::refreshSlot()
{
    //初始化完成释放 线程
    QList <QAbstractButton *>list = _pBatchPageView->getRadionGroup()->buttons();
    if (list.size() > 0)
    {
        list.front()->setChecked(true);
    }
    _pInitThread->exit();
    delete _pInitThread;
    _pInitThread = NULL;
}

void UmInstallerAssistant::openTaskWndSlot()
{
    _taskCenterWnd->showNormal();
}

void UmInstallerAssistant::openUrlLinkSlot()
{
    QDesktopServices::openUrl(QUrl("http://shouji.youmeng.com/Help/theList/id/68.html"));
}

void UmInstallerAssistant::tabClickSlot(QAbstractButton *pRadio, bool bchecked)
{
    if (bchecked)
    {
        if (pRadio == ui.tab_btn_1)
        {
            _pBatchPageView->setVisible(false);
            _pOnekeyPageView->setVisible(true);
            _pQuickPageView->setVisible(false);
            ui.single_widget->setVisible(true);
            ui.mainframe_bottom->setVisible(true);
        }
        else if (pRadio == ui.tab_btn_2)
        {
            _pBatchPageView->setVisible(true);
            _pOnekeyPageView->setVisible(false);
            _pQuickPageView->setVisible(false);
            ui.single_widget->setVisible(false);
            ui.mainframe_bottom->setVisible(false);
        }
        else if (pRadio == ui.tab_btn_3)
        {
            _pBatchPageView->setVisible(false);
            _pOnekeyPageView->setVisible(false);
            _pQuickPageView->setVisible(true);
            ui.single_widget->setVisible(true);
            ui.mainframe_bottom->setVisible(true);

        }
    }
}

UmInstallerAssistant::~UmInstallerAssistant()
{
//    delete _checkDevices;
    delete _pBatchPageView;
    delete _pOnekeyPageView;
    delete _pQuickPageView;
    delete _tabButtonGroup;
    delete _taskCenterWnd;
    UMessageBox::getInstance()->setParent(NULL);
    UMessageBoxEx::getInstance()->setParent(NULL);
    delete UMessageBox::getInstance();
    delete UMessageBoxEx::getInstance();
}

bool UmInstallerAssistant::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message != WM_NCHITTEST)
    {
#if QT_VERSION < 0x050000  
        return QDialog::winEvent(message, result);
#else  
        return QMainWindow::nativeEvent("", message, result);
#endif  
    }
    return winEvent(msg, result);
}

bool UmInstallerAssistant::winEvent(MSG *message, long *result)
{
    switch (message->message)
    {
    case WM_NCHITTEST:
        //12 和21 为阴影部分去除
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x() - 0;
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y() - 0;
        if (yPos < 95 && (ui.centralWidget->childAt(xPos, yPos) == ui.maimframe_caption ||
            ui.centralWidget->childAt(xPos, yPos) == ui.label_caption_icon))
        {
            *result = HTCAPTION;
        }
        else{
            return false;
        }
    }
    return true;
}

void UmInstallerAssistant::initPackageList()
{
    _pInitThread->start();
}

void UmInstallerAssistant::createPackageItemSlot(TPackageVector vt, QString packageName, QString unique_key, ListItemType type)
{
    CDataManager::getInstance()->insertName(unique_key, packageName);
    QString name = packageName;
    QWidget  *pItem = NULL,*pOnekeyItem = NULL;
    QCheckBox *listCheckbox = NULL;

    pItem = new CBatchListItem(type,_pBatchPageView->getListContainer());
    pOnekeyItem = new COnekeyListItem(type,_pOnekeyPageView->getListContainer());

    CDataManager::getInstance()->insert(unique_key.toLongLong(), vt);
    float fsize = 0;
    fsize = calcPackageSize(vt);
    if (pItem)
    {
        emit CSignalManager::getInstance()->setBatchPackageInfoSignal(pItem, name, fsize, vt.size());
        emit CSignalManager::getInstance()->addBatchItemSingal(pItem);
        emit CSignalManager::getInstance()->setBatchItemUniqueSignal(pItem, unique_key);
        _pBatchPageView->getRadionGroup()->addButton((QAbstractButton*)pItem);
    }
      
    if (pOnekeyItem)
    {
        emit CSignalManager::getInstance()->addOnekeyItemSingal(pOnekeyItem);
        emit CSignalManager::getInstance()->setOnekeyPackageInfoSignal(pOnekeyItem, name, fsize, vt.size());
        emit CSignalManager::getInstance()->setOnekeyItemUniqueSignal(pOnekeyItem, unique_key);

        listCheckbox = pOnekeyItem->findChild<QCheckBox *>("onekey_listitem_query");
        if (!listCheckbox) return;
        _pOnekeyPageView->getButtonGroup(QUERY_CHECKBOX)->addButton(listCheckbox);
        QPushButton *pButton = pOnekeyItem->findChild<QPushButton *>("onekey_listitem_delete");
        if (!pButton) return;
        _pOnekeyPageView->getButtonGroup(DELETE_BUTTON)->addButton(pButton);
        pButton = pOnekeyItem->findChild<QPushButton *>("onekey_listitem_modify");
        if (!pButton) return;
        _pOnekeyPageView->getButtonGroup(MODIFY_BUTTON)->addButton(pButton);
        pButton = pOnekeyItem->findChild<QPushButton *>("onekey_listitem_install");
        if (!pButton) return;
        _pOnekeyPageView->getButtonGroup(INSTALL_BUTTON)->addButton(pButton);

        pButton = pOnekeyItem->findChild<QPushButton *>("onekey_listitem_mask_close");
        if (!pButton) return;
        _pOnekeyPageView->getButtonGroup(MASK_CLOSE_BUTTON)->addButton(pButton);
    }
}

float UmInstallerAssistant::calcPackageSize(TPackageVector &vt)
{
    auto it = vt.begin();
    __int64 sum = 0;
    for (; it != vt.end(); it++)
    {
        sum += (*it)._fileSize; 
    }
    return (float)sum / (1024 * 1024);
}

void UmInstallerAssistant::sendTaskCenterProgressSlot(int success, int num)
{
    if (num > 0 )
    {
        QString text = QString("%1/%2").arg(success).arg(num);
        ui.mainframe_taskcenter_btn->setText(text);
    }
    else
    {
        ui.mainframe_taskcenter_btn->setText("任务中心");
    }
    
}

void UmInstallerAssistant::startTaskCenterMovieSlot(bool bstart)
{
    if (bstart) {
        _movie.start();
    } else {
        _movie.stop();
        _movie.jumpToFrame(0);
    }
}

void UmInstallerAssistant::setSingleConnectStateSlot(QString serial,QString model, DeviceState state)
{
    QPixmap pix;
    QString connectState;
    QString markedWords;
    if (state == UNCONNECT)
    {
        ui.single_gif->setVisible(false);
        pix.load(":/image/single_connect_failed");
        markedWords = "请用数据线连接手机";
        connectState = "未连接设备";
        ui.single_state->setStyleSheet("color:#000000");
        emit CSignalManager::getInstance()->restoreOriginalStateSignal();
        clearMemoryInfo();
    } 
    else if (state == CONNECTFAILED)
    {
        ui.single_gif->setVisible(false);
        pix.load(":/image/single_connect_failed");
        markedWords = "请打开调试权限";
        connectState = "连接失败";
        ui.single_state->setStyleSheet("color:#0979d5");
        emit CSignalManager::getInstance()->restoreOriginalStateSignal();
        clearMemoryInfo();
    }
    else if (state == CONNECTING)
    {
        ui.single_gif->setVisible(true);
        pix.load(":/image/single_connect_success");
        markedWords = "请稍候";
        connectState = "安卓设备连接中";
        ui.single_state->setStyleSheet("color:#0979d5");
        emit CSignalManager::getInstance()->restoreOriginalStateSignal();
        clearMemoryInfo();
    }
    else if (state == CONNECTED)
    {
        ui.single_gif->setVisible(false);
        pix.load(":/image/single_connect_success");
        markedWords = "立即安装软件吧";
        connectState = model + " 已连接";
        ui.single_state->setStyleSheet("color:#0979d5");
        
        //获取
        emit CSignalManager::getInstance()->queryApkListSignal(serial);
        emit CSignalManager::getInstance()->queryStorageInfoSignal(serial);
    }
    ui.single_tip->setText(markedWords);
    ui.single_state->setText(connectState);
    ui.single_icon->setPixmap(pix);
}

void UmInstallerAssistant::setVersionInfoSlot(QString ver, bool bMustUpadate)
{
    if (ver == SOFTWARE_VERSION)
    {
        QString text = QString("版本").append(SOFTWARE_VERSION).append("(");
        ui.mainframe_version->setText(text);
        ui.mainframe_version_btn->setVisible(false);
    }
    else
    {
        ui.mainframe_version_btn->setVisible(true);
        QString text = QString("版本").append(SOFTWARE_VERSION).append("(");
        ui.mainframe_version->setText(text);
        ui.mainframe_version_btn->setVisible(false);
    }
    ui.horizontalLayoutWidget->update();
}

void UmInstallerAssistant::openUpdataLinkSlot()
{
    QDesktopServices::openUrl(QUrl("http://shouji.youmeng.com"));
}

void UmInstallerAssistant::setMemoryInfoSlot(QString serial, TQMap info)
{
    //判断是否是一键装机的连接，如果不是则退出
    if (serial != ConnectManager::getInstance()->getOnekeySerial(false)) return;

    // 下面解析 内存和SD卡相关信息
    parseMemoryInfo(info);

    auto r = _memMap.find(-1);
    if (r!=_memMap.end())
    {
        setMemProgress(r->fused, r->ffree, r->ftotal, false);
    }

    QStringList strList;
    auto it = _memMap.begin();
    for (it; it != _memMap.end();it++)
    {
        if (it.key() != -1)
        {
            strList << QString("SD卡%0").arg(it.key() + 1);
        }
    }

    ui.mainframe_sd_combox->clear();
    ui.mainframe_sd_combox->addItems(strList);
    ui.mainframe_sd_combox->setCurrentIndex(0);
}

void UmInstallerAssistant::parseMemoryInfo(TQMap info)
{
    _memMap.clear();
    MemInfo meminfo;
    float fused = 0.0, ffree = 0, total = 0;
    auto it = info.find("data_used");
    if (it != info.end())
        meminfo.fused = getSize(*it);
    it = info.find("data_free");
    if (it != info.end())
        meminfo.ffree = getSize(*it);
    it = info.find("data_total");
    if (it != info.end())
        meminfo.ftotal = getSize(*it);
    _memMap.insert(-1, meminfo);
    auto it_i = info.begin();
    int index = 0;
    for (it_i; it_i != info.end(); it_i++)
    {
        QString str = it_i.key();
        if (str.startsWith("sdcard", Qt::CaseInsensitive))
        {
            QStringList list = str.split("_");
            int cur = 0;
            QString sub = list.at(0).mid(6);
            if (sub.isEmpty())
                cur = 0;
            else
                cur = sub.toInt();
            MemInfo meminfo;
            auto it_m = _memMap.find(cur);
            if (it_m != _memMap.end())
                meminfo = *it_m;
            else
                _memMap.insert(cur, meminfo);


            if (list.size() == 2)
            {
                if (!list.at(1).isEmpty() && list.at(1).compare("used", Qt::CaseInsensitive) == 0)
                {
                    meminfo.fused = getSize(it_i.value());
                }
                if (!list.at(1).isEmpty() && list.at(1).compare("free", Qt::CaseInsensitive) == 0)
                {
                    meminfo.ffree = getSize(it_i.value());
                }
                if (!list.at(1).isEmpty() && list.at(1).compare("total", Qt::CaseInsensitive) == 0)
                {
                    meminfo.ftotal = getSize(it_i.value());
                }
                _memMap.insert(cur, meminfo);
            }
        }
    }
}

float UmInstallerAssistant::getSize(QString str)
{
    int i = 0;
    QString sub;
    for (int i = 0; i < str.length();i++)
    {
        QChar c = str.at(i);
        if (!(c== '.' || (c >= '0' && c <= '9')))
        {
            sub = str.mid(0, i);
            break;
        }
    }

    float r = 0;
    if (str.contains("G", Qt::CaseInsensitive))
    {
        r = sub.toFloat();
    }
    else if (str.contains("M", Qt::CaseInsensitive))
    {
        r = sub.toFloat() / 1024;
    }
    else
    {
        r = 0;
    }
    return r;
}

void UmInstallerAssistant::setMemProgress(float used, float free, float total, bool isSd)
{
    int val = 0;
    if (total == 0)
    {
        val = 0;
    }else
        val = used * 100 / total;
    QString s_t, s_f;
    s_f = QString("剩余:%1GB").arg(QString::number(free, 'f', 2));
    if (isSd)
    {
        s_t = QString(":%1GB").arg(QString::number(total, 'f', 2));
        ui.mainframe_sd_total->setText(s_t);
        ui.mainframe_sd_left->setText(s_f);
        ui.mainframe_bottom_sd_progress->setValue(val);
    }
    else
    {
        s_t = QString("内存:%1GB").arg(QString::number(total, 'f', 2));
   
        ui.mainframe_mobile_memory->setText(s_t);
        ui.mainframe_mobile_memory_left->setText(s_f);
        ui.mainframe_bottom_memory_progress->setValue(val);
    }
}

void UmInstallerAssistant::currentIndexChangedSlot(QString text)
{
    QString sub = text.mid(3);
    int index = sub.toInt();
    if (index > 0)
    {
        index = index - 1;
    }
    else
        return;
    auto r = _memMap.find(index);
    if (r != _memMap.end())
        setMemProgress(r->fused, r->ffree, r->ftotal, true);
}

void UmInstallerAssistant::clearMemoryInfo()
{
    setMemProgress(0, 0, 0, true);
    setMemProgress(0, 0, 0, false);
    ui.mainframe_sd_combox->clear();
}

InitPackageThread::InitPackageThread(QWidget *AddItem, QObject *parent):QThread(parent)
{
    _addItem = AddItem;
}

InitPackageThread::~InitPackageThread()
{

}

void InitPackageThread::dealWithRecPackage()
{
    QString val = GlobalIniCfg::getInstance()->queryGlobalValue("rec_package_list");
    QStringList list = Common::qstr2QstrList(val, "&");
    QVector<TKuInstallItem> vt;
    for (int i = 0; i < list.size(); i++)
    {
        QString user_id = GlobalConfiguration::getInstance()->getUserId();
        vt.clear();
        QString item = list.at(i);
        QStringList pair = Common::qstr2QstrList(item, "_");
        if (pair.size() == 3)
        {
            ListItemType type = DEFAULT;
            if (pair.at(2).toInt() == HIGH_CONVERSION)
            {
                type = HIGH_CONVERSION;
            }
            else if (pair.at(2).toInt() == HIGH_DOWNLOAD)
            {
                type = HIGH_DOWNLOAD;
            }
            emit CSignalManager::getInstance()->addPackageSignal(vt, pair.at(0), pair.at(1), type);
        }
    }
}

void InitPackageThread::run()
{
    QString version;
    if (!version.isEmpty())
    {
        emit CSignalManager::getInstance()->setVersionInfoSignal(version, false);
    }
    QString uuid = GlobalConfiguration::getInstance()->getUserId();
    QString photoNum;

    emit CSignalManager::getInstance()->setUserInfoSignal(photoNum);

    QVector<TKuInstallItem> vt;
    QWidget  *pItem = NULL;
    int maxCustomNum = GlobalIniCfg::getInstance()->queryGlobalValue("max_package_num").toInt();
    dealWithRecPackage();

    QString user_id = GlobalConfiguration::getInstance()->getUserId();
    QVector<CustomPackageInfo> vt_p;
    int i = 0;
    while (i < 10)
    {
        QString name = QString("%0").arg(i);
            if (name.contains("自定义套餐"))
            {
                int pos = name.indexOf("");
                int index = name.mid(QString("自定义套餐").length()).toInt();
                if (index > CDataManager::getInstance()->getMaxIndex())
                    CDataManager::getInstance()->setMaxIndex(index);
            }
            
            emit CSignalManager::getInstance()->addPackageSignal(vt, name, name, DEFAULT);
            i++;
    }
        

    //添加自定义套餐按钮
    emit CSignalManager::getInstance()->addOnekeyItemSingal(_addItem);
    emit initFinish();
}
