#include "qfile.h"
#include "Common.h"
#include "UmInstallerAssistant.h"
#include "OnekeyPageView.h"
#include "QueryInfoView.h"
#include "SignalManager.h"
#include "OneKeyListWidget.h"
#include "DataManager.h"
#include "ConnectManager.h"
#include "PackageStateManager.h"
#include "configuration_option\GlobalConfiguration.h"
#include "qdesktopservices.h"

COnekeyPageView::COnekeyPageView(QWidget *parent)
    : QWidget(parent),
    _bShowDel(true),
    _pListCheckboxGroup(NULL),
    _pItemLayout(NULL),
    _checkbox(NULL),
    _pDeleteGroup(NULL),
    _pModifyGroup(NULL),
    _pInstallGroup(NULL)
{
    ui.setupUi(this);
    _customWnd = new CustomWnd(this);
    _customWnd->setModal(true);
    _pItemLayout = new QGridLayout;
    
    _pListCheckboxGroup = new QButtonGroup;
    _pListCheckboxGroup->setExclusive(false);

    _pDeleteGroup = new QButtonGroup;
    _pModifyGroup = new QButtonGroup;
    _pInstallGroup = new QButtonGroup;
    _pMaskCloseGroup = new QButtonGroup;
    _pDeleteGroup->setExclusive(false);
    _pModifyGroup->setExclusive(false);
    _pInstallGroup->setExclusive(false);
    _pMaskCloseGroup->setExclusive(false);

    _pOnekeyList = new COnekeyListWidget(QRect(13, 13, 777, 535), 3, this);
    _pOnekeyList->setHorizontalSpacing(14);
    _pOnekeyList->setVerticalSpacing(16);
    _pOnekeyList->setStyleSheet("background-color:#ffffff;border:0px;");

    QString styleSheet = Common::readStyleSheet(":/qss/onekeyPage");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    connectSignal2Slot();
}

//连接信号和槽
void COnekeyPageView::connectSignal2Slot()
{
    connect(_pDeleteGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(deleteItemSlot(QAbstractButton *)));
    connect(_pModifyGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(ModifyItemSlot(QAbstractButton *)));
    connect(_pInstallGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(InstallItemSlot(QAbstractButton *)));
    connect(_pMaskCloseGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(maskCloseSlot(QAbstractButton *)));
    connect(_pListCheckboxGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this, SLOT(radioClick(QAbstractButton *, bool)));

    connect(CSignalManager::getInstance(), SIGNAL(showOneKeyQueryInfoSingal(QWidget *, QString)), _pOnekeyList, SLOT(showLineItemSlot(QWidget *, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(hideOneKeyQueryInfoSingal()), _pOnekeyList, SLOT(hideRowItemSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(addOnekeyItemSingal(QWidget *)), _pOnekeyList, SLOT(addItemSlot(QWidget *)));
    connect(CSignalManager::getInstance(), SIGNAL(deleteOnekeyItemSignal(QWidget *)), _pOnekeyList, SLOT(deleteItemSlot(QWidget *)));
    connect(CSignalManager::getInstance(), SIGNAL(modifyPackageSignal(QString)), this, SLOT(modifyPackageSlot(QString)));
    connect(CSignalManager::getInstance(), SIGNAL(finishOnekeyPackageAbnormalSignal()), this, SLOT(restoreInstallButtonSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(finishedOnekeyPackageInstallSignal(int, int)), this, SLOT(showMaskSlot(int, int)));
    connect(CSignalManager::getInstance(), SIGNAL(sendDisableDeletePackageSignal(QString, bool)), this, SLOT(setDeleteEnableSlot(QString, bool)));
    connect(CSignalManager::getInstance(), SIGNAL(refreshQueryInfoViewSignal(QString,QString)), this, SLOT(refreshQueryInfoViewSlot(QString,QString)));
}

void COnekeyPageView::setDeleteEnableSlot(QString unique, bool isEnabale)
{
    QList<QAbstractButton *> list = _pDeleteGroup->buttons();
    auto it = list.begin();
    for (it; it != list.end();it++)
    {
        QWidget *parent = (QWidget *)(*it)->parent();
        if (!parent) continue;
        QLabel *plabel = parent->findChild<QLabel *>("onekey_listitem_unique");
        if (!plabel) continue;
        QString key = plabel->text();
        if (unique == key)
        {
            (*it)->setEnabled(isEnabale);
            QPushButton *pModify = parent->findChild<QPushButton *>("onekey_listitem_modify");
            if (!pModify) continue;
            pModify->setEnabled(isEnabale);
            break;
        }
        
    }
}

void COnekeyPageView::showMaskSlot(int num, int success)
{
    QList<QAbstractButton *> list = _pInstallGroup->buttons();
    auto it = list.begin();
    for (it; it != list.end(); it++)
    {
        if (!(*it)->isEnabled())
        {
            (*it)->setEnabled(true);
            (*it)->setText("一键安装");

            QWidget *pItem = (QWidget *)(*it)->parent();
            if (!pItem) continue;;
            QLabel *mask = pItem->findChild<QLabel *>("onekey_listitem_mask");
            if (!mask) continue;
            QLabel *text = mask->findChild<QLabel *>("onekey_listitem_mask_text");
            if (!text) continue;
            QString str = QString("已成功安装%1/%2款软件").arg(success).arg(num);
            text->setText(str);
            mask->setVisible(true);

            QPushButton *pDelete = pItem->findChild<QPushButton *>("onekey_listitem_delete");
            if (!pDelete) continue;
            pDelete->setEnabled(true);
            QPushButton *pModify = pItem->findChild<QPushButton *>("onekey_listitem_modify");
            if (!pModify) continue;
            pModify->setEnabled(true);
            break;
        }
    }
}

void COnekeyPageView::maskCloseSlot(QAbstractButton *close)
{
    QLabel *pLabel = (QLabel*)close->parent();
    if (!pLabel) return;
    if (pLabel->isVisible())
    {
        pLabel->setVisible(false);
    }
}

void COnekeyPageView::restoreInstallButtonSlot()
{
   QList<QAbstractButton *> list = _pInstallGroup->buttons();
   auto it = list.begin();
   for (it; it != list.end();it++)
   {
       if (!(*it)->isEnabled())
       {
           (*it)->setEnabled(true);
           (*it)->setText("一键安装");
           QWidget *parent = (QWidget *)(*it)->parent();
           if (!parent) return;
           QPushButton *pDelete = parent->findChild<QPushButton *>("onekey_listitem_delete");
           if (!pDelete) return;
           pDelete->setEnabled(true); 
           QPushButton *pModify = parent->findChild<QPushButton *>("onekey_listitem_modify");
           if (!pModify) continue;
           pModify->setEnabled(true);
           break;
       }
   }
}

void COnekeyPageView::modifyPackageSlot(QString unique)
{
    _customWnd->showNormal(unique);
}

COnekeyPageView::~COnekeyPageView()
{
    delete _pItemLayout;
    delete _pDeleteGroup;
    delete _pModifyGroup;
    delete _pInstallGroup;
    delete _pOnekeyList;
}

QButtonGroup *COnekeyPageView::getButtonGroup(GroupType nType)
{
    QButtonGroup *r = NULL;
    switch (nType)
    {
    case DELETE_BUTTON:
        r = _pDeleteGroup;
        break;
    case MODIFY_BUTTON:
        r = _pModifyGroup;
        break;
    case INSTALL_BUTTON:
        r = _pInstallGroup;
        break;
    case QUERY_CHECKBOX:
        r = _pListCheckboxGroup;
        break;
    case MASK_CLOSE_BUTTON:
        r = _pMaskCloseGroup;
        break;
    default:
        break;
    }
    return r;
}

void COnekeyPageView::radioClick(QAbstractButton *pRadio, bool bChecked)
{
    CBatchListItem *pItem = (CBatchListItem *)pRadio->parent();
    if (bChecked)
    {
        QLabel *pLabel = pItem->findChild<QLabel *>("onekey_listitem_query_desc");
        if (!pLabel) return;
        pLabel->setText("收起");
        if (_checkbox == pRadio) return;
        QLabel *pLabel_u = pItem->findChild<QLabel *>("onekey_listitem_unique");
        if (!pLabel_u) return;
        QString unique = pLabel_u->text();

        _checkbox = (QCheckBox *)pRadio;
        MuteCheckBox(pRadio);
        
        int index = _pOnekeyList->getItemIndex(pItem);
        int cols = _pOnekeyList->getColCount();
        int row = index / cols + 1;
        pLabel = pItem->findChild<QLabel *>("onekey_listitem_unique");
        emit CSignalManager::getInstance()->showOneKeyQueryInfoSingal(pItem, unique);
    }
    else
    {
        QLabel *pLabel = pItem->findChild<QLabel *>("onekey_listitem_query_desc");
        if (!pLabel) return;
        pLabel->setText("展开查看");
        _pOnekeyList->ModifyItemStyleSheet(pItem, false);
        if (_checkbox == pRadio)
        {
            emit CSignalManager::getInstance()->hideOneKeyQueryInfoSingal();
            _checkbox = NULL;
        }
    }

}

void COnekeyPageView::MuteCheckBox(QAbstractButton *pRadio)
{
    QList<QAbstractButton *>list = _pListCheckboxGroup->buttons();
    auto it = list.begin();
    for (; it != list.end();it++)
    {
        if (*it != pRadio)
        {
           
            if ((*it)->isChecked())
            {
                (*it)->setChecked(false);
            }
        }
    }
}

void COnekeyPageView::deleteItemSlot(QAbstractButton *item)
{
    QWidget *parent = NULL;
    if (item)
    {
        parent =  (QWidget *)item->parent();

        QLabel *pLabel = parent->findChild<QLabel *>("onekey_listitem_name");
        if (!pLabel) return;
         QString packageName = pLabel->text();
        //QString val = GlobalIniCfg::getInstance()->queryValue("del_package_warn_wnd");
        //if (val.isEmpty() || val != "hide")
        if (_bShowDel)
        {
            QString tipText = QString("确定删除%0套餐吗？").arg(packageName);
            QEventLoop waitLoop;
            UMessageBox::getInstance()->showNormal(tipText, UM_OK | UM_CANCEL | UM_REMEMBER, &waitLoop);
            int ret = waitLoop.exec();
            if (ret == 0)
                return;
            if (ret == 2)
            {
                _bShowDel = false;
            }
        }

        if (parent)
        {
            QLabel *pLabel = parent->findChild<QLabel *>("onekey_listitem_unique");
            if (!pLabel) return;
            QString key = pLabel->text();
            QString nextUniuqe = _pOnekeyList->getNextItemUnique(parent);
            emit CSignalManager::getInstance()->deleteBatchItemSignal(pLabel->text());
            emit CSignalManager::getInstance()->deleteOnekeyItemSignal(parent);
            emit CSignalManager::getInstance()->refreshQueryInfoViewSignal(key, nextUniuqe);
            QString user_id = GlobalConfiguration::getInstance()->getUserId();
            DatabaseMan::getInstance()->deleteCustomPackage(key, user_id);
            CDataManager::getInstance()->eraseName(key);
            
        }
    }
}

void COnekeyPageView::ModifyItemSlot(QAbstractButton *item)
{
    QWidget *parent = NULL;
    if (item)
    {
        parent = (QWidget *)item->parent();
        if (parent)
        {
            QLabel *pLabel = parent->findChild<QLabel *>("onekey_listitem_unique");
            if (!pLabel) return;
            emit CSignalManager::getInstance()->modifyPackageSignal(pLabel->text());
        }
            
    }
}
void COnekeyPageView::InstallItemSlot(QAbstractButton *item)
{
    if (!Common::isNetConnected())
    {
        UMessageBox::getInstance()->showNormal("无网络连接,请联网重试！", UM_OK);
        return;
    }
    QWidget *parent = NULL;
    if (item)
    {
        parent = (QWidget *)item->parent();
        if (parent)
        {
            QLabel *pLabel = parent->findChild<QLabel *>("onekey_listitem_unique");
            QString unique = pLabel->text();
            __int64 unique_key = unique.toLongLong();
            TPackageVector vt = CDataManager::getInstance()->getPackageVector(unique_key);
   
            if (vt.size() == 0)
            {
                //获取套餐信息错误
                UMessageBox::getInstance()->showNormal("无网络连接,请联网重试！", UM_OK);
                return;
            }
            TMobileVector mobileVt;
            QString serial = ConnectManager::getInstance()->getOnekeySerial();
            if (serial.isEmpty())
            {
                UMessageBox::getInstance()->showNormal("手机未连接，请先连接手机！", UM_OK);
                return;
            }
            mobileVt.push_back(serial);
            QString key;
            if (unique == "1" || unique == "2")
            {
                key = "znzjtc";
            } else {
                key = "zdytc";
            }

            if (PackageStateMan::getInstance()->startInstallTask(vt, mobileVt, key))
            {
                for (auto it_i = vt.begin(); it_i != vt.end(); it_i++)
                {
                    float fsize = (float)it_i->_fileSize / (1024 * 1024);
                    CSignalManager::getInstance()->addTaskItemSignal(it_i->_packageName, it_i->_icon, it_i->_name, fsize);
                    CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(it_i->_packageName, WAIT_DOWNLOAD);
                }
                emit CSignalManager::getInstance()->startInstallSingal(vt, mobileVt);
                item->setDisabled(true);
                item->setText("安装中");

                QPushButton *pDelete = parent->findChild<QPushButton *>("onekey_listitem_delete");
                if (!pDelete) return;
                pDelete->setEnabled(false);

                QPushButton *pModify = parent->findChild<QPushButton *>("onekey_listitem_modify");
                if (!pModify) return;
                pModify->setEnabled(false);
            }
        } 
    }
}

void COnekeyPageView::OnekeyAddPackageClickSlot()
{
    QWidget * pSend = (QWidget *)QObject::sender();
    int i = _pOnekeyList->getItemIndex(pSend);
    QString numStr = GlobalIniCfg::getInstance()->queryGlobalValue("max_package_num");
    int num = 0;
    if (numStr.isEmpty())
    {
        num = DEFAULT_PACKAGE_NUM;
    }
    else
        num = numStr.toInt();
    if (i >= num + 2)
    {
        UMessageBox::getInstance()->showNormal("", UM_OK);
    }
    else{
        _customWnd->showNormal(QString(""));
    }

}

void COnekeyPageView::refreshQueryInfoViewSlot(QString curUnique,QString changeUnique)
{
    QWidget *item = _pOnekeyList->getItemByUnique(changeUnique);
    if (!item)  return;
    QCheckBox *pCheck = item->findChild<QCheckBox *>("onekey_listitem_query");
    if (curUnique == changeUnique) {
        _pOnekeyList->refreshQueryInfoView(changeUnique);
    } else {
        _checkbox = pCheck;
        pCheck->setChecked(true);
        _pOnekeyList->refreshQueryInfoView(changeUnique);
    }

}

