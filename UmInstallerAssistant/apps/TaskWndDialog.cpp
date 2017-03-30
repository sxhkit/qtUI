#include "TaskWndDialog.h"
#include <windowsx.h>
#include "Common.h"
#include "QtCore"
#include "QtGui"
#include "UmInstallerAssistant.h"
#include "SignalManager.h"
#include "PackageStateManager.h"
#include "DataManager.h"
#include "ConnectManager.h"
#include <QScrollBar> 
CTaskWndDialog::CTaskWndDialog( QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint)
{
    qRegisterMetaType<SOFT_STATE>("SOFT_STATE");
    _retryButtonGroup = new QButtonGroup;
    ui.setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    QString styleSheet = Common::readStyleSheet(":/qss/taskwnd.qss");
    if (!styleSheet.isEmpty())
        setStyleSheet(styleSheet);
    QString styleSheet_s = Common::readStyleSheet(":/qss/vert_scroll.qss");
    ui.task_listwidget->verticalScrollBar()->setStyleSheet(styleSheet_s);
    _noItemPix.load(":/image/task_noitem");
    _hasItemPix.load(":/image/task_listwidget_bk");
    ui.task_no_listitem->setPixmap(_noItemPix);
    connectSignal2Slot();
}

CTaskWndDialog::~CTaskWndDialog()
{
}

void CTaskWndDialog::clearListSlot()
{
    ui.task_no_listitem->setPixmap(_noItemPix);
    emit CSignalManager::getInstance()->startTaskCenterMovieSignal(false);
    emit CSignalManager::getInstance()->sendTaskCenterProgressSignal(0, 0);
    _listItem.clear();
    ui.task_listwidget->clear();
}

void CTaskWndDialog::hideWndSlot()
{
    hide();
}

bool CTaskWndDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

bool CTaskWndDialog::winEvent(MSG *message, long *result)
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
            if (ui.centralWidget->childAt(xPos, yPos) == NULL)
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
        
    }
    return true;
}

void CTaskWndDialog::setItemStateSlot(QString packageName, SOFT_STATE nstate)
{
    TaskCenterItemInfo * pInfo = getTaskItemInfo(packageName);
    if (!pInfo || !pInfo->pItem) return ;
    CTaskListItem *pItem = (CTaskListItem *)ui.task_listwidget->itemWidget(pInfo->pItem);
    if (!pItem) return;
    
     setListItemState(packageName, nstate);
     if (nstate == DOWNLOADSUCCESS || nstate == INSTALLING || nstate == INSTALLSUCCESS || nstate == INSTALLFAILED)
     {
         setListItemProgress(packageName, 100);
     }
     if (nstate ==  WAIT_DOWNLOAD)
     {
         setListItemProgress(packageName, 0);
     }
     CTaskListItem *tmp = movePosition(packageName);
     if (tmp) pItem = tmp;
     //判断如果 需要同步到快捷安装的话，发送信号到快捷安装web
     emit pItem->setItemStateSignal(nstate);
     if (nstate ==  INSTALLSUCCESS)
     {
         //发送给mainframe 任务中心 任务进度
         int numsuccess = getInstalledSuccess();
         emit CSignalManager::getInstance()->sendTaskCenterProgressSignal(numsuccess, _listItem.size());
     }
}

int CTaskWndDialog::getInstalledSuccess()
{
    int num = 0;
    for (auto it = _listItem.begin(); it != _listItem.end();it++)
    {
        if ((*it).nstate == INSTALLSUCCESS)
        {
            num++;
        }
    }
    return num;
}

CTaskListItem *CTaskWndDialog::getItemWidget(QString packageName)
{
    TaskCenterItemInfo *p = getTaskItemInfo(packageName);
    if (!p || !p->pItem) return NULL;
    return (CTaskListItem *)ui.task_listwidget->itemWidget(p->pItem);
}

int CTaskWndDialog::getCorrectLineNum(QListWidgetItem* listItem)
{
    if (!listItem) return ERROR_STATE;
    int count = ui.task_listwidget->count();
    QListWidgetItem *tmp = NULL;
    SOFT_STATE state = getItemStateByListItem(listItem);
    int index = 0;
    for (int i = 0; i < count;i++)
    {
        tmp = ui.task_listwidget->item(i);
        if (tmp!= listItem)
        {
            SOFT_STATE s = getItemStateByListItem(tmp);
            if (s <= state)
            {
                index++;
            }else
                break;
        }
    }
    return index;
}

SOFT_STATE CTaskWndDialog::getItemStateByListItem(QListWidgetItem *pItem)
{
    SOFT_STATE  s = ERROR_STATE;
    auto it = _listItem.begin();
    for (; it != _listItem.end();it++)
    {
        if (it->pItem == pItem)
        {
            s = it->nstate;
            break;
        }
    }
    return s;
}

CTaskListItem* CTaskWndDialog::movePosition(QString &packagename)
{
    TaskCenterItemInfo *pListItem = getTaskItemInfo(packagename);
    if (!pListItem) return NULL;
    int count = ui.task_listwidget->count();
    QListWidgetItem* pItem = pListItem->pItem;
    if (!pItem) return NULL;
    int i = 0;
    for (; i < count;i++)
    {
        if (pItem == ui.task_listwidget->item(i))
        {
            break;
        }
    }
   
    int index = getCorrectLineNum(pItem);
    if (index == i) return NULL;
    QWidget *pDeleteItem = ui.task_listwidget->itemWidget(pItem);
    
    QPushButton *pRetryButton = pDeleteItem->findChild<QPushButton *>("TaskItem_TryBtn");
    // 移除BUTTON  里面删除掉的ITEM 中的retry
    if (pRetryButton) {
        _retryButtonGroup->removeButton(pRetryButton);
    }
    SAFEDELETE(pDeleteItem);
    ui.task_listwidget->removeItemWidget(pItem);
    pItem = ui.task_listwidget->takeItem(i);
    ui.task_listwidget->insertItem(index, pItem);
    CTaskListItem *item_n = new CTaskListItem;
    pRetryButton = item_n->findChild<QPushButton *>("TaskItem_TryBtn");
    // 添加 新建的 正确位置ITEM  里面的retry
    if (pRetryButton) {
        _retryButtonGroup->addButton(pRetryButton);
    }

    emit item_n->setItemRelevantInfoSignal(pListItem->name, pListItem->fsize);
    emit item_n->setProgressValueSignal(pListItem->progressValue);
    emit item_n->setItemStateSignal(pListItem->nstate);

    if (!Common::isIconFileExist(pListItem->icon))
    {
        QString unique = QString(pListItem->name).append("_taskWndDialog");
        emit CSignalManager::getInstance()->addDownloadIconSignal(pListItem->icon, unique);
    }
    else
    {
        emit item_n->setIconSignal(Common::getIconSavePath(pListItem->icon));
    }

    if (!pListItem->err.isEmpty())
        emit item_n->setErrorInfoSignal(pListItem->err);
    ui.task_listwidget->setItemWidget(pItem, item_n);
    return item_n;
}

void CTaskWndDialog::addListItem(QString &package, QString &icon, QString &name, float &fsize)
{
   
    
}

void CTaskWndDialog::addListItemSlot(QString package,QString icon, QString name, float fsize)
{
    ui.task_no_listitem->setPixmap(_hasItemPix);
    addListItem(package, icon, name, fsize);
}

void CTaskWndDialog::setListItemState(QString packagename,SOFT_STATE nstate)
{
    TaskCenterItemInfo *pItem = getTaskItemInfo(packagename);
    if (!pItem) return;
    pItem->nstate = nstate;
}

void CTaskWndDialog::connectSignal2Slot()
{
    connect(ui.task_close, SIGNAL(clicked()), this, SLOT(hideWndSlot()));
    connect(CSignalManager::getInstance(), SIGNAL(addTaskItemSignal(QString,QString, QString, float)),
        this, SLOT(addListItemSlot(QString,QString, QString, float)));
    connect(CSignalManager::getInstance(), SIGNAL(setItemStateSignal(QString, SOFT_STATE)),
        this, SLOT(setItemStateSlot(QString, SOFT_STATE)));
    connect(CSignalManager::getInstance(), SIGNAL(setErrorInfoSignal(QString,QString)),
        this, SLOT(setErrorInfoSlot(QString, QString)));
    connect(CSignalManager::getInstance(), SIGNAL(completedDownloadIconSignal(QString, QString)),
        this, SLOT(setIconSlot(QString, QString)));

    connect(CSignalManager::getInstance(), SIGNAL(downloadProgress(QString, qint64, qint64)),
        this, SLOT(setItemProgressSlot(QString, qint64, qint64)));
    connect(CSignalManager::getInstance(), SIGNAL(startDownloadSignal(TKuInstallItem)), this, SLOT(startDownloadSlot(TKuInstallItem)));
    connect(CSignalManager::getInstance(), SIGNAL(clearListSignal()), this, SLOT(clearListSlot()));
    connect(_retryButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(retryButtonClick(QAbstractButton *)));
}

void CTaskWndDialog::setProgressValueSlot(QString package, int val)
{
    CTaskListItem *pItem = getItemWidget(package);
    emit pItem->setProgressValueSignal(val);
    setListItemProgress(package,val);
}

void CTaskWndDialog::setErrorInfoSlot(QString package, QString err)
{
    CTaskListItem *pItem = getItemWidget(package);
    if (!pItem) return;
    emit pItem->setErrorInfoSignal(err);
    setListItemErrorCode(package, err);
}

void CTaskWndDialog::setListItemProgress(QString packagename, int value)
{
    TaskCenterItemInfo *pItem = getTaskItemInfo(packagename);
    if (!pItem) return;
    pItem->progressValue = value;
}

void CTaskWndDialog::setListItemErrorCode(QString packagename, QString err)
{
    TaskCenterItemInfo *pItem = getTaskItemInfo(packagename);
    if (!pItem) return;
    pItem->err = err;
}

TaskCenterItemInfo *CTaskWndDialog::getTaskItemInfo(QString package)
{
    TaskCenterItemInfo *p = NULL;
    auto it = _listItem.begin();
    for (it; it != _listItem.end();it++)
    {
        if (it->packageName == package)
        {
            p = &(*it);
            break;
        }
    }
    return  p;
}

void CTaskWndDialog::setIconSlot(QString icon, QString unique)
{
    if (!unique.endsWith("_taskWndDialog"))
        return;
    QString soft_name = unique.mid(0, unique.length() - QString("_taskWndDialog").length());
    auto it = _listItem.begin();
    for (it; it != _listItem.end(); it++)
    {
        if ((*it).name == soft_name)
        {
            CTaskListItem *pItem =getItemWidget((*it).packageName);
            emit pItem->setIconSignal(Common::getIconSavePath(icon));
            break;
        }
    }
}

void CTaskWndDialog::setItemProgressSlot(QString packagename, qint64 readbyte, qint64 totalbyte)
{
    if (totalbyte == 0)
        return;
    //此处无用操作很多 设置进度 不管批量装机 还是一键装机都会遍历任务中心的list 甚至 默认桌面等 下载任务都会遍历
    auto it = _listItem.begin();
    for (it; it != _listItem.end(); it++)
    {
        if ((*it).packageName == packagename)
        {
            CTaskListItem *pItem =(CTaskListItem*)ui.task_listwidget->itemWidget(it->pItem);
            int  val = readbyte * 100 / totalbyte;
            it->progressValue = val;
            emit pItem->setProgressValueSignal(val);
        }
    }
    
}

void CTaskWndDialog::startDownloadSlot(TKuInstallItem item)
{
    if (item._type == QUICK_TASK || item._type == SINGLE_TASK || item._type == RETRY_TASK)
    {
        auto it = _listItem.begin();
        for (it; it != _listItem.end(); it++)
        {
            if ((*it).packageName == item._packageName)
            {
                CTaskListItem *pItem = (CTaskListItem*)ui.task_listwidget->itemWidget(it->pItem);
                if (it->nstate == WAIT_DOWNLOAD)
                {
                    CSignalManager::getInstance()->setItemStateSignal(it->packageName, DOWNLOADING);
                    it->nstate = DOWNLOADING;
                    emit CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal((*it).packageName, DOWNLOADING);
                }
            }
        }
    }
}

TaskCenterItemInfo *CTaskWndDialog::getItemInfoByWidget(QWidget *pTaskListItem)
{
    TaskCenterItemInfo *res = NULL;
    auto it = _listItem.begin();
    for (it; it != _listItem.end();it++)
    {
        if ((*it).pItem)
        {
            QWidget *pWidget = ui.task_listwidget->itemWidget((*it).pItem);
            if (pWidget == pTaskListItem)
            {
                res = &(*it);
                break;
            }
        }
    }
    return res;
}

void CTaskWndDialog::retryButtonClick(QAbstractButton *pRetry)
{
    TaskCenterItemInfo *pItem = NULL;
    if (!pRetry->parent()) return;

    pItem = getItemInfoByWidget((QWidget *)pRetry->parent());
    if (!pItem) return;
    TKuInstallItem it = CDataManager::getInstance()->getItemInfoByPackageName(pItem->packageName);
    if (it._packageName.isEmpty()) return;
    it._type = RETRY_TASK;
    TPackageVector v;
    TMobileVector vm;
    v.push_back(it);
    vm.push_back(ConnectManager::getInstance()->getOnekeySerial());
    CSignalManager::getInstance()->startInstallSingal(v, vm);
    pRetry->setVisible(false);
    PackageStateMan::getInstance()->addSingleTask(it._packageName);
    emit CSignalManager::getInstance()->startTaskCenterMovieSignal(true);
    emit CSignalManager::getInstance()->setItemStateSignal(it._packageName, WAIT_DOWNLOAD);
    CTaskListItem*pListItem = (CTaskListItem *)ui.task_listwidget->itemWidget(pItem->pItem);
    emit CSignalManager::getInstance()->quickInstallChangeSoftStatusSignal(it._packageName, WAIT_DOWNLOAD);
    emit pListItem->setProgressValueSignal(0);
}
