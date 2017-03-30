#include <windowsx.h>
#include "Common.h"
#include "QtCore"
#include "QtGui"
#include "CustomWnd.h"
#include "qwebengineview.h"
#include "qwebenginepage.h"
#include "configuration_option\GlobalConfiguration.h"
#include <QWebEngineSettings> 
#include "SignalManager.h"
#include "BatchListItem.h"
#include "DataManager.h"
#include "UMessageBox.h"
CustomWnd::CustomWnd(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint),
    _pCheckGroup(NULL)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    QString styleSheet = Common::readStyleSheet(":/qss/customWnd.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    ui.customwnd_noNet->setVisible(false);
    _pCheckGroup = new QButtonGroup;
    _pCheckGroup->setExclusive(false);
    _pListWidget = new CQuickListWidget(QRect(18, 49, 887, 453), 8, this);
    _pListWidget->setMargin(12);
    _pListWidget->setVerticalSpacing(15);
    _pListWidget->setHorizontalSpacing(13);
    _pListWidget->setStyleSheet("background-color:#ffffff;border:0px;");
    _itemStyleSheet = Common::readStyleSheet(":/qss/CustomItem.qss");
    ui.custom_edit->installEventFilter(this);
    connect(CSignalManager::getInstance(), SIGNAL(createCustomItemSignal(TPackageVector)),
        this, SLOT(createQuickItemSlot(TPackageVector)));
    connect(_pCheckGroup, SIGNAL(buttonToggled(QAbstractButton *, bool )),
        this, SLOT(checkSlot(QAbstractButton *, bool)));
    connect(ui.customwnd_close, SIGNAL(clicked()), this, SLOT(closeSlot()));
    connect(ui.customwnd_selecteall, SIGNAL(clicked(bool)), this, SLOT(selectAllCheckSlot(bool)));
    connect(ui.customwnd_selectepart, SIGNAL(clicked(bool)), this, SLOT(cooperateCheckSlot(bool)));
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
    connect(ui.custom_save_btn, SIGNAL(clicked()), this, SLOT(savePackageSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(customNoNetSignal(bool)), this, SLOT(NoNetSlot(bool)));
    connect(CSignalManager::getInstance(), SIGNAL(completedDownloadIconSignal(QString, QString)),
        this, SLOT(setIconSlot(QString, QString)));
    _timer.setInterval(30);
}
bool CustomWnd::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui.custom_edit) {
        if (event->type() == QEvent::FocusIn) {
            if (ui.custom_edit->text() == "请输入套餐名") {
                ui.custom_edit->setText("");
            }
            
        } else if (event->type() == QEvent::FocusOut){
            if (ui.custom_edit->text().isEmpty()) {
                ui.custom_edit->setText("请输入套餐名");
            }
        }
    }
    return __super::eventFilter(obj, event);
}

void CustomWnd::NoNetSlot(bool bVisible)
{
    ui.customwnd_noNet->setVisible(bVisible);
    if (bVisible) {
        _pListWidget->setVisible(false);
    } else {
        _pListWidget->setVisible(true);
    }
}

QString CustomWnd::generateJsonStr(QString name, QVector<TKuInstallItem>& vec)
{
    
    return "";
}

void CustomWnd::savePackageSlot()
{
    //缺少软件数量判断
    QString name = ui.custom_edit->text();
    if (name.isEmpty()) {
        name = "请输入套餐名";
    }
    QString realName = CDataManager::getInstance()->getRealName(name);
    QVector<TKuInstallItem> vec;
    QString json_str = generateJsonStr(realName, vec);
    if (vec.size() < 4) {
        //messagebox
        QEventLoop loop;
        UMessageBoxEx::getInstance()->showNormal("软件数量有点少哦，多选几个软件吧！","", &loop);
        if (loop.exec() == 0) {
            return;
        }
    }
    bool isHasCoorp = false;
    auto it = vec.begin();
    for (it; it != vec.end();it++)
    {
        if (it->_isCooperate == "1")
        {
            isHasCoorp = true;
            break;
        }
    }
    if (!isHasCoorp)
    {
        QEventLoop loop;
        UMessageBoxEx::getInstance()->showNormal("未选择计费软件,无可赚取金额哦！", "选择几个软件吧！", &loop);
        if (loop.exec() == 0) {
            return;
        }
    }


    closeSlot();
}

bool CustomWnd::isSelected(bool isCooperate)
{
    bool ret = true;
    auto it = _mapInfo.begin();
    for (it; it != _mapInfo.end();it++) {
        if (!isCooperate) {
            if (it->_isCooperate == "1" &&!it.key()->isChecked()) {
                ret = false;
                break;
            }
        } else {
            if (!it.key()->isChecked()) {
                ret = false;
                break;
            }
        }
    }
    return ret;
}

void CustomWnd::timeoutSlot()
{
    _timer.stop();
    hide();
}

void CustomWnd::closeSlot()
{
    selectItem(false, false);
    ui.customwnd_selecteall->setChecked(false);
    ui.customwnd_selectepart->setChecked(false);
    ui.custom_edit->setText("");
    _timer.start();
}

void CustomWnd::cooperateCheckSlot(bool bChecked)
{
    selectItem(bChecked, true);
    auto it = _mapInfo.begin();
    if (!bChecked)
    {
        if (isExistSoftByType(true)) {
            ui.customwnd_selecteall->setChecked(true);
        } else {
            ui.customwnd_selecteall->setChecked(false);
        }
    } else {
        if (isExistSoftByType(false)) {
            ui.customwnd_selecteall->setChecked(false);
        } else {
            ui.customwnd_selecteall->setChecked(true);
        }
    }
    
}

bool CustomWnd::isExistSoftByType(bool isCoorprate)
{
    bool ret = false;
    auto it = _mapInfo.begin();
    for (; it != _mapInfo.end();it++)
    {
        if (it->_isCooperate == "1" && isCoorprate)
        {
            ret = false;
            break;
        }
        if (it->_isCooperate != "1" && !isCoorprate)
        {
            ret = false;
            break;
        }
    }
    return ret;
}

void CustomWnd::selectAllCheckSlot(bool bChecked)
{
    selectItem(bChecked, false);
    if (bChecked) {
        ui.customwnd_selectepart->setChecked(true);
    } else {
        ui.customwnd_selectepart->setChecked(false);
    }
}

void CustomWnd::selectItem(bool bSelected, bool isCooperateCheck)
{
    auto it = _mapInfo.begin();
    for (it; it != _mapInfo.end();it++)
    {
        if (isCooperateCheck && it->_isCooperate != "1")
            continue;
        it.key()->setChecked(bSelected);    
    }
}

CustomWnd::~CustomWnd()
{
    delete _pListWidget;
}

void CustomWnd::showNormal(QString unique)
{
    if (!unique.isEmpty())
    {
        QVector<TKuInstallItem> vt = CDataManager::getInstance()->getPackageVector(unique.toLongLong());
        auto it = vt.begin();
        for (; it != vt.end(); it++)
        {
            auto it_m = _map.find(it->_packageName);
            if (it_m != _map.end())
            {
                (static_cast<CustomWndItem *>((*it_m)))->setChecked(true);
            }
        }
        ui.customwnd_selecteall->setChecked(isSelected(false));
        ui.customwnd_selectepart->setChecked(isSelected(true));
        QString name = CDataManager::getInstance()->queryName(unique);
        ui.custom_edit->setText(name);
    }else
        ui.custom_edit->setText("请输入套餐名");
    _unique_key = unique;
    // 为了初始化 保存按钮不可用
    checkSlot(NULL,false);
    __super::showNormal();
}

void CustomWnd::checkSlot(QAbstractButton *item, bool bchecked)
{
    float fsum = 0;
    int num = 0;
    int num_u = 0;
    unsigned __int64 fileSize = 0;
    auto it = _mapInfo.begin();
    for (it; it != _mapInfo.end();it++) {
        if (it.key()->isChecked()) {
            fsum +=it->_price;
            num++;
            if (it->_isCooperate == "1") {
                num_u++;
            }
            fileSize += it->_fileSize;
        }
    }
    ui.custom_label_2->setText(QString("%0款").arg(num));
    float f = (float)fileSize / (1024 * 1024);
    QString text = QString("软件,共").append(QString::number(f, 'f', 2)).append("MB,可赚钱软件");
    text.append(QString("%0").arg(num_u)).append("款,最多可赚");
    ui.custom_label_3->setText(text);
    ui.custom_label_4->setText(QString::number(fsum,'f',2).append("元"));
    if (num <= 0) {
        ui.custom_save_btn->setDisabled(true);
    } else {
        ui.custom_save_btn->setDisabled(false);

    }
}

bool CustomWnd::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
   
    Q_UNUSED(eventType);

    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message != WM_NCHITTEST)
    {
#if QT_VERSION < 0x050000  
        return QDialog::winEvent(message, result);
#else  
        return QDialog::nativeEvent("", message, result);
#endif  
    }
    return winEvent(msg, result);
}

bool CustomWnd::winEvent(MSG *message, long *result)
{
    switch (message->message)
    {
    case WM_NCHITTEST:
        //12 和21 为阴影部分去除
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x() - 12;
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y() - 9;
        if (yPos < 36 && yPos > 0)
        {
            QWidget *P = ui.centralWidget->childAt(xPos, yPos);
            if (ui.centralWidget->childAt(xPos, yPos) == NULL || ui.centralWidget->childAt(xPos, yPos) == ui.customwnd_title_1)
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
        
    }
    return true;
}

void CustomWnd::createQuickItemSlot(TPackageVector vt)
{
    TKuInstallItem *item = NULL;
    TListItemVector vt_l;
    for (auto it = vt.begin(); it != vt.end(); it++)
    {
        item = &(*it);
        CustomWndItem *pQwidget = new CustomWndItem(_pListWidget->getContainer());

        pQwidget->setStyleSheet(_itemStyleSheet);
        if (pQwidget)
        {
            _map.insert(item->_packageName, pQwidget);
            _mapInfo.insert(pQwidget, *item);
        }
        else
            return;
        _pCheckGroup->addButton(pQwidget);

        emit pQwidget->setItemInfoSignal(item->_name,  item->_isCooperate == "1" ? true : false);

        if (!Common::isIconFileExist(item->_icon))
        {
            QString unique = QString(item->_packageName).append("_customWnd");
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
        _pListWidget->addItemsSlot(vt_l);
    }
}

void CustomWnd::setIconSlot(QString icon, QString unique)
{
    if (!unique.endsWith("_customWnd"))
        return;
    QString soft_name = unique.mid(0, unique.length() - QString("_customWnd").length());
    auto it = _map.find(soft_name);
    if (it != _map.end()) {
        emit static_cast<CustomWndItem *>(*it)->setIconSignal(Common::getIconSavePath(icon));
    }
}

